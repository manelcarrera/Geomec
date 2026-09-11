
#include "WellCasingDianaRunner.h"

#include "DCBodyFaceLoad.h"
#include "DCInitialStressLoad.h"
#include "DCTemperatureLoad.h"
#include "Hexahedron.h"
#include "IParallelInitializationCallback.h"
#include "Pressure.h"
#include "ValueTensor.h"
#include "WellCasingCement.h"
#include "WellCasingDianaRunnerHelper.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"
#include "lbfl.h"

// minimum values for pressure and temperature loads
// loads don't get written if all (absolute) element values are below these values
#define MIN_PRESSU_LOAD_VAL 1e-8
#define MIN_TEMPER_LOAD_VAL 1e-8

CWellCasingDianaRunner::CWellCasingDianaRunner(CWellCasingModel &model, CDianaRunController &controller)
    : IWellModelDianaRunner(model, controller) {}

CWellCasingDianaRunner::~CWellCasingDianaRunner() {}

bool CWellCasingDianaRunner::FetchInitialPressures(const geo::IElement &element, const CDepletionStage &stage,
                                                   IValueDomainScalar::TValueVec &vcInitialPressures) {
  ElementPressures(element, stage, vcInitialPressures);

  return true;
}

void CWellCasingDianaRunner::ElementPressures(const geo::IElement &elm, const CDepletionStage &stage,
                                              IValueDomainScalar::TValueVec &vcValues) {
  if (CasingModel().Mesh().IsCementElement(elm))
    vcValues = CasingModel().CasingCement().Pressure(stage).Component().ScalarData().ValueElement(elm);
  else if (CasingModel().Mesh().IsCementInterfaceElement(elm)) {
    CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
    helper.InterfaceElementPressures(*this, elm, stage, vcValues);
  } else if (!CasingModel().Mesh().IsSteelElement(elm) && !CasingModel().Mesh().IsOuterInterfaceElement(elm)) {
    assert(!dynamic_cast<const geo::CInterfaceElement *>(&elm));
    FetchElementInitialPressures(stage, elm, vcValues);
  }
}

bool CWellCasingDianaRunner::FetchInitialTemperatures(const geo::IElement &element, const CDepletionStage &stage,
                                                      IValueDomainScalar::TValueVec &vcInitialTemperatures) {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);

  if (CasingModel().Mesh().IsCementElement(element))
    vcInitialTemperatures =
        CasingModel().CasingCement().EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
  else if (!pIface && !CasingModel().Mesh().IsSteelElement(element))
    FetchElementInitialTemperatures(stage, element, vcInitialTemperatures);

  return true;
}

bool CWellCasingDianaRunner::CreateInitialLoads(const geo::IElement &element, const CDepletionStage &stage,
                                                dia::CLoadCase &lcase,
                                                const IValueDomainScalar::TValueVec &vcInitialPressures,
                                                const IValueDomainScalar::TValueVec &vcInitialTemperatures) {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  if (!pIface && !CasingModel().Mesh().IsSteelElement(element)) {
    // write the initial pressures
    WriteElementInitialLoads(stage, element, lcase, vcInitialPressures, vcInitialTemperatures);

    // write the initial stresses
    int n;
    for (n = 0; n < element.NrOfNodes(); ++n) {
      if (m_mpInterpolatedStress.find(&element.Node(n)) == m_mpInterpolatedStress.end()) {
        std::vector<int> vcElementIndices = CasingModel().ParentModel().Mesh().Mesh().ElementsAt(element.Node(n));
        if (vcElementIndices.empty()) {
          // can't write for this element
          return true;
        }
      }
    }

    const IWellModel::CBoundary &boundary = static_cast<const IWellModel::CBoundary &>(Model().Boundary());
    const IWellModel::CBoundary::CPressureSupport &pressusup = boundary.PressureSupport();
    const CValueType *pDistri = pressusup.DistriValues(Model().InitialDepletionStage());

    std::vector<CTensor> vcStresses;

    if (pDistri) {
      assert(pDistri->ComponentSize() == 6); // stress tensor
      std::vector<geo::CValue> vals(6 * element.NrOfNodes());
      bool bAllValid = true;

      for (int iComp = 0; iComp < 6 && bAllValid; ++iComp) {
        IValueDomainScalar::TValueVec vcValues = pDistri->Component(iComp).ScalarData().ValueElement(element);
        for (size_t n2 = 0; n2 < element.NrOfNodes() && bAllValid; ++n2) {
          if (!vcValues[n2].Valid()) {
            bAllValid = false;
            break;
          }

          vals[6 * n2 + iComp] = -vcValues[n2].Value() * 1e6;
        }
      }

      if (bAllValid) {
        for (size_t n2 = 0; n2 < element.NrOfNodes(); ++n2)
          vcStresses.push_back(CStressTensor(vals[6 * n2 + 0].Value(), vals[6 * n2 + 1].Value(),
                                             vals[6 * n2 + 2].Value(), vals[6 * n2 + 3].Value(),
                                             vals[6 * n2 + 4].Value(), vals[6 * n2 + 5].Value()));
      }
    } else {
      for (n = 0; n < element.NrOfNodes(); ++n) {
        std::vector<double> v(6);

        TInterpolatedStressMap::iterator it = m_mpInterpolatedStress.find(&element.Node(n));
        if (it == m_mpInterpolatedStress.end()) {
          const CDepletionStage &parentStage = CasingModel().ParentEquivalentDepletionStage(stage);
          const CModelBase &parentModel = CasingModel().ParentModel();

          CStressTensorValueSet stress = parentModel.ResultRegister().EffectiveStress(
              parentStage, ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

          CStressTensor stresstensor =
              stress.ValuePoint(element.Node(n), geo::IParallelInitializationCallback::Sequential);

          v[0] = -stresstensor.XX() * 1e6;
          v[1] = -stresstensor.YY() * 1e6;
          v[2] = -stresstensor.ZZ() * 1e6;
          v[3] = stresstensor.XY() * 1e6;
          v[4] = stresstensor.YZ() * 1e6;
          v[5] = stresstensor.XZ() * 1e6;

          m_mpInterpolatedStress.insert(TInterpolatedStressMap::value_type(&element.Node(n), v));
        } else
          v = it->second;

        vcStresses.push_back(CStressTensor(v[0], v[1], v[2], v[3], v[4], v[5]));
      }
    }

    // create the initial stress load
    new dia::CInitialStressLoad(lcase, vcStresses, element);
  }

  if (CasingModel().Mesh().IsSteelElement(element)) {
    // apply internal pressures and temperature
    assert(dynamic_cast<const geo::CHexahedron *>(&element));
    const geo::CHexahedron &hexa = static_cast<const geo::CHexahedron &>(element);
    const geo::CBodyQuadrilateral &quad =
        static_cast<const geo::CBodyQuadrilateral &>(hexa.Face(CWellCasingMesh::HS_INNER));

    int nNod = quad.NrOfNodes();

    // pressure
    std::vector<double> vcPressureValues(nNod);

    IValueDomainScalar::TValueVec vcValues =
        CasingModel().CasingNode().InternalPressure(stage).Component().ScalarData().ValueElement(quad);
    bool bWriteThem = false;
    bool bWriteMulti = false;
    int i;
    for (i = 0; i < nNod; ++i) {
      vcPressureValues[i] = vcValues[i].Value() * 1e6; // should be valid
      if (fabs(vcPressureValues[i]) > MIN_PRESSU_LOAD_VAL)
        bWriteThem = true;
      if (i && fabs(vcPressureValues[i] - vcPressureValues[i - 1]) > MIN_PRESSU_LOAD_VAL)
        bWriteMulti = true;
    }

    if (bWriteThem) {
      if (bWriteMulti)
        new dia::CBodyFaceLoad(lcase, vcPressureValues, quad.Normal().Flipped().UnitVector(), *quad.Parent(),
                               quad.Index());
      else
        new dia::CBodyFaceLoad(lcase, vcPressureValues[0], quad.Normal().Flipped().UnitVector(), *quad.Parent(),
                               quad.Index());
    }
  }

  m_progress->Step(LOADS_PROGRESS_SCALE);

  return true;
}

bool CWellCasingDianaRunner::CreateStageLoads(const geo::IElement &element, const CDepletionStage &stage,
                                              dia::CLoadCase &lcase,
                                              const IValueDomainScalar::TValueVec &vcInitialPressures,
                                              const IValueDomainScalar::TValueVec &vcInitialTemperatures) {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);

  if (!pIface && !CasingModel().Mesh().IsSteelElement(element))
    WriteElementStageLoads(stage, element, lcase, vcInitialPressures, vcInitialTemperatures);

  if (CasingModel().Mesh().IsSteelElement(element)) {
    // apply internal pressures and temperature
    assert(dynamic_cast<const geo::CHexahedron *>(&element));
    const geo::CHexahedron &hexa = static_cast<const geo::CHexahedron &>(element);
    const geo::CBodyQuadrilateral &quad =
        static_cast<const geo::CBodyQuadrilateral &>(hexa.Face(CWellCasingMesh::HS_INNER));

    int nNod = quad.NrOfNodes();

    // pressure
    std::vector<double> vcPressureValues(nNod);

    IValueDomainScalar::TValueVec vcValues =
        CasingModel().CasingNode().InternalPressure(stage).Component().ScalarData().ValueElement(quad);
    IValueDomainScalar::TValueVec vcIniValues =
        CasingModel().CasingNode().InternalPressure(stage.InitialStage()).Component().ScalarData().ValueElement(quad);
    bool bWriteThem = false;
    bool bWriteMulti = false;
    int i;
    for (i = 0; i < nNod; ++i) {
      vcPressureValues[i] = (vcValues[i].Value() - vcIniValues[i].Value()) * 1e6; // should be valid
      if (fabs(vcPressureValues[i]) > MIN_PRESSU_LOAD_VAL)
        bWriteThem = true;
      if (i && fabs(vcPressureValues[i] - vcPressureValues[i - 1]) > MIN_PRESSU_LOAD_VAL)
        bWriteMulti = true;
    }

    if (bWriteThem) {
      if (bWriteMulti)
        new dia::CBodyFaceLoad(lcase, vcPressureValues, quad.Normal().Flipped().UnitVector(), *quad.Parent(),
                               quad.Index());
      else
        new dia::CBodyFaceLoad(lcase, vcPressureValues[0], quad.Normal().Flipped().UnitVector(), *quad.Parent(),
                               quad.Index());
    }
  }

  m_progress->Step(LOADS_PROGRESS_SCALE);

  return true;
}

bool CWellCasingDianaRunner::WriteBoundaryLoads(TStageMap &mpStages) {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  return helper.WriteBoundaryLoads(mpStages);
}

const dia::IMaterial *CWellCasingDianaRunner::InterfaceMaterial(const geo::IElement &element) const {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  return helper.InterfaceMaterial(element);
}

const dia::IMaterial &CWellCasingDianaRunner::Material(const geo::IElement &element) const {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  const dia::IMaterial *pMat = helper.Material(element);
  if (pMat)
    return *pMat;

  return IWellModelDianaRunner::Material(element);
}

bool CWellCasingDianaRunner::HasTemperatureValue(const geo::IElement &element) const {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  return (!pIface && !CasingModel().Mesh().IsSteelElement(element) && !CasingModel().Mesh().IsCementElement(element));
}

bool CWellCasingDianaRunner::HasConcentrationValue(const geo::IElement &element) const {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  return (!pIface && !CasingModel().Mesh().IsSteelElement(element) && !CasingModel().Mesh().IsCementElement(element));
}

const CWellCasingModel &CWellCasingDianaRunner::CasingModel() const {
  return static_cast<const CWellCasingModel &>(Model());
}

CWellCasingModel &CWellCasingDianaRunner::CasingModel() { return static_cast<CWellCasingModel &>(Model()); }

void CWellCasingDianaRunner::CreateStrainLoad(const geo::IElement &elm, const CDepletionStage &stage,
                                              dia::CLoadCase &lcase) {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&elm);
  if (!pIface && !CasingModel().Mesh().IsSteelElement(elm) && !CasingModel().Mesh().IsCementElement(elm))
    IWellModelDianaRunner::CreateStrainLoad(elm, stage, lcase);
}

int CWellCasingDianaRunner::LoadsProgressSize() const {
  // scale number of steps
  return LOADS_PROGRESS_SCALE * IWellModelDianaRunner::LoadsProgressSize();
}

void CWellCasingDianaRunner::FetchStagePressures(const geo::IElement &element, const CDepletionStage &stage,
                                                 IValueDomainScalar::TValueVec &vcPressures) {
  if (CasingModel().Mesh().IsCementElement(element))
    vcPressures = CasingModel().CasingCement().Pressure(stage).Component().ScalarData().ValueElement(element);
  else
    IWellModelDianaRunner::FetchStagePressures(element, stage, vcPressures);
}

void CWellCasingDianaRunner::FetchStageTemperatures(const geo::IElement &element, const CDepletionStage &stage,
                                                    IValueDomainScalar::TValueVec &vcTemperatures) {
  if (CasingModel().Mesh().IsCementElement(element))
    vcTemperatures =
        CasingModel().CasingCement().EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
  else
    IWellModelDianaRunner::FetchStageTemperatures(element, stage, vcTemperatures);
}

void CWellCasingDianaRunner::executeCommandInGeomec() const {
  if (CasingModel().LargeDeformations())
    PutCharItem("GEOTYP", "UPDATE");
}

CWellCasingDianaRunner::TSolver CWellCasingDianaRunner::Solver() const { return PARALLEL_DIRECT_SPARSE; }
