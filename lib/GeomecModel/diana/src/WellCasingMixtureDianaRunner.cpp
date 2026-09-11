
#include "WellCasingMixtureDianaRunner.h"
#include "DianaRunController.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "WellCasingCement.h"
#include "WellCasingDianaRunnerHelper.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"

#include "DCLoadCase.h"
#include "DCPrescribedPressurePotential.h"
#include "DCTemperatureLoad.h"
#include "lbfl.h"

CWellCasingMixtureDianaRunner::CWellCasingMixtureDianaRunner(CWellCasingModel &model, CDianaRunController &controller)
    : CGeomecMixtureDianaRunner(model.Mesh(), model, controller) {}

const dia::IMaterial &CWellCasingMixtureDianaRunner::Material(const geo::IElement &element) const {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  const dia::IMaterial *pMat = helper.Material(element);
  if (pMat)
    return *pMat;

  return CGeomecMixtureDianaRunner::Material(element);
}

const dia::IMaterial *CWellCasingMixtureDianaRunner::InterfaceMaterial(const geo::IElement &element) const {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  const dia::IMaterial *pMat = helper.InterfaceMaterial(element);
  return pMat;
}

const CWellCasingModel &CWellCasingMixtureDianaRunner::CasingModel() const {
  return static_cast<const CWellCasingModel &>(Model());
}

CWellCasingModel &CWellCasingMixtureDianaRunner::CasingModel() { return static_cast<CWellCasingModel &>(Model()); }

bool CWellCasingMixtureDianaRunner::HasTemperatureValue(const geo::IElement &element) const {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  return (!pIface && !CasingModel().Mesh().IsSteelElement(element));
}

bool CWellCasingMixtureDianaRunner::HasConcentrationValue(const geo::IElement &element) const {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  if (pIface || CasingModel().Mesh().IsSteelElement(element))
    return false;

  const CModelBase &model = Model();
  bool bVolumetric = false;
  for (CDepletionStageEntry::const_iterator it = model.DepletionStageEntry().begin();
       it != model.DepletionStageEntry().end(); ++it) {
    if (!it->Initial() && CasingModel().CasingCement().Strain(*it).IsVolumetric()) {
      bVolumetric = true;
      break;
    }
  }

  return bVolumetric;
}

void CWellCasingMixtureDianaRunner::WriteModelSpecificLoads(const TStageMap &mpStages) {
  // apply the inner pressure to the inner nodes of the steel elements

  int nNodesPerSpoke = CasingModel().Mesh().NodesPerSpoke();
  int nGrids = CasingModel().Mesh().NumberOfGrids();
  int nSpokes = CasingModel().Mesh().TangentialElements();
  const geo::IMesh &mesh = CasingModel().Mesh().Mesh();

  // the current node index
  int nNode = 0;

  int n;
  for (n = 0; n < nGrids; ++n) {
    bool bCap = (n == 0 || n == nGrids - 1);

    // loop over the spokes
    int i;
    for (i = 0; i < nSpokes; ++i) {
      const geo::INode &node = mesh.Node(nNode);

      double dIniPressure = 0;
      CDepletionStageEntry::const_iterator it;
      for (it = Model().DepletionStageEntry().begin(); it != Model().DepletionStageEntry().end(); ++it) {
        const CDepletionStage &stage = *it;
        TStageMap::const_iterator its = mpStages.find(&stage);
        assert(its != mpStages.end());
        dia::CLoadCase &lcase = *its->second;
        if (lcase.Active()) {
          const CWellCasingInternalPressure &internalPressure = CasingModel().CasingNode().InternalPressure(stage);
          double dPressure = internalPressure.Component().ScalarData().ValuePoint(node).Value();
          if (stage.Initial()) {
            dIniPressure = dPressure;
            if (fabs(dPressure) > EPS)
              new dia::CPrescribedPressurePotential(node, lcase, dPressure * 1e6);
          } else {
            double dDeltaPressure = dPressure - dIniPressure;
            if (fabs(dDeltaPressure) > EPS)
              new dia::CPrescribedPressurePotential(node, lcase, dDeltaPressure * 1e6);
          }
        }
      }

      nNode += (bCap ? 2 * nNodesPerSpoke : nNodesPerSpoke);
    }
  }

  // write temperatures in cement
  for (int i = 0; i < CasingModel().CasingCement().ElementSetSize(); ++i) {
    for (int j = 0; j < CasingModel().CasingCement().ElementSet(i).ElementSet().ElementSize(); ++j) {
      const geo::IElement &elm = CasingModel().CasingCement().ElementSet(i).ElementSet().Element(j);
      CDepletionStageEntry::const_iterator it;
      IValueDomainScalar::TValueVec vcInitialTemperatures;
      for (it = Model().DepletionStageEntry().begin(); it != Model().DepletionStageEntry().end(); ++it) {
        const CDepletionStage &stage = *it;
        TStageMap::const_iterator its = mpStages.find(&stage);
        assert(its != mpStages.end());
        dia::CLoadCase &lcase = *its->second;
        if (lcase.Active()) {
          bool bWriteMultiple = false;
          bool bWriteThem = false;
          const IValueDomainScalar::TValueVec &vcTemperatures = ElementTemperatures(elm, stage);
          std::vector<double> vcTemper(vcTemperatures.size());
          if (stage.Initial()) {
            vcInitialTemperatures = vcTemperatures;
            for (size_t l = 0; l < vcTemperatures.size(); ++l) {
              vcTemper[l] = vcTemperatures[l].Value() + 273.15; // convert to Kelvin
              if (l && fabs(vcTemper[l] - vcTemper[l - 1]) > MIN_TEMPER_LOAD_VAL)
                bWriteMultiple = true;
            }

            if (bWriteMultiple)
              new dia::CTemperatureLoad(lcase, vcTemper, elm);
            else
              new dia::CTemperatureLoad(lcase, vcTemper[0], elm);
          } else {
            assert(!vcInitialTemperatures.size() ==
                   vcTemperatures.size()); // should have seen initial stage (iterator works in stage order)
            for (size_t l = 0; l < vcTemperatures.size(); ++l) {
              vcTemper[l] = vcTemperatures[l].Value() - vcInitialTemperatures[l].Value();
              if (fabs(vcTemper[l]) > MIN_TEMPER_LOAD_VAL)
                bWriteThem = true;
              if (l && fabs(vcTemper[l] - vcTemper[l - 1]) > MIN_TEMPER_LOAD_VAL)
                bWriteMultiple = true;
            }

            if (bWriteThem) {
              if (bWriteMultiple)
                new dia::CTemperatureLoad(lcase, vcTemper, elm);
              else
                new dia::CTemperatureLoad(lcase, vcTemper[0], elm);
            }
          }
        }
      }
    }
  }
}

void CWellCasingMixtureDianaRunner::CreateStrainLoad(const geo::IElement &elm, const CDepletionStage &stage,
                                                     dia::CLoadCase &lcase) {
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&elm);
  if (!pIface && !CasingModel().Mesh().IsSteelElement(elm) && !CasingModel().Mesh().IsCementElement(elm))
    CGeomecMixtureDianaRunner::CreateStrainLoad(elm, stage, lcase);
}

void CWellCasingMixtureDianaRunner::ElementPressures(const geo::IElement &elm, const CDepletionStage &stage,
                                                     IValueDomainScalar::TValueVec &vcValues) const {
  if (CasingModel().Mesh().IsCementElement(elm))
    vcValues = CasingModel().CasingCement().Pressure(stage).Component().ScalarData().ValueElement(elm);
  else if (CasingModel().Mesh().IsCementInterfaceElement(elm)) {
    CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
    helper.InterfaceElementPressures(const_cast<CWellCasingMixtureDianaRunner &>(*this), elm, stage, vcValues);
  } else if (!CasingModel().Mesh().IsSteelElement(elm))
    CGeomecMixtureDianaRunner::ElementPressures(elm, stage, vcValues);
  else
    vcValues.resize(elm.NrOfNodes(), geo::CValue());
}

IValueDomainScalar::TValueVec CWellCasingMixtureDianaRunner::ElementTemperatures(const geo::IElement &element,
                                                                                 const CDepletionStage &stage) const {
  if (CasingModel().Mesh().IsCementElement(element))
    return CasingModel().CasingCement().EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);

  return CGeomecMixtureDianaRunner::ElementTemperatures(element, stage);
}

IValueDomainScalar::TValueVec CWellCasingMixtureDianaRunner::ElementConcentrations(const geo::IElement &element,
                                                                                   const CDepletionStage &stage) const {
  if (CasingModel().Mesh().IsCementElement(element)) {
    if (CasingModel().CasingCement().Strain(stage).IsVolumetric()) {
      return CasingModel().CasingCement().Strain(stage).Component().ScalarData().ValueElement(element);
    } else {
      IValueDomainScalar::TValueVec vcValues(element.NrOfNodes(), 0);
      return vcValues;
    }
  }

  return CGeomecMixtureDianaRunner::ElementConcentrations(element, stage);
}

void CWellCasingMixtureDianaRunner::executeCommandInGeomec() const {
  if (CasingModel().LargeDeformations())
    PutCharItem("GEOTYP", "UPDATE");
}

bool CWellCasingMixtureDianaRunner::WriteBoundaryLoads(const TStageMap &mpStages) {
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  return helper.WriteBoundaryLoads(mpStages);
}

bool CWellCasingMixtureDianaRunner::ParentLinearResults() const {
  return CasingModel().ParentLinearResults(Controller().AnalysisType());
}
