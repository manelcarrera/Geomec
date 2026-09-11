
#include "WellCasingDianaRunnerHelper.h"

#include "WellCasingModel.h"
#include "WellCasingNode.h"
#include "WellCasingSteel.h"
#include "DianaRunController.h"
#include "WellCasingCement.h"
#include "BoundaryInterfaceMaterial.h"
#include "WellCasingCementInterface.h"
#include "WellCasingCementInitialMaterial.h"
#include "BodyGroup.h"
#include "VectorValueSet.h"
#include "DCPrescribedDisplacement.h"
#include "DCBodyFaceLoad.h"
#include "GlobalConstants.h"
#include "IParallelInitializationCallback.h"
#include "GeomecDianaRunner.h"

CWellCasingDianaRunnerHelper::CWellCasingDianaRunnerHelper(const IWellModel& model, const CDianaRunController& controller, bool bParentLinearResults)
: m_model(model),
  m_controller(controller),
  m_bParentLinearResults(bParentLinearResults),
  m_progress( ((CDianaRunController*)&controller)->runner()->progress() )
{
}

CWellCasingDianaRunnerHelper::~CWellCasingDianaRunnerHelper()
{
}

const dia::IMaterial* CWellCasingDianaRunnerHelper::Material(const geo::IElement& element) const
{
  if(CasingModel().Mesh().IsSteelElement(element))
  return &CasingModel().CasingNode().Steel().Material(Controller().StartStage()).Material(element);

  if(CasingModel().Mesh().IsCementElement(element))
  {
  const CFFMaterial& mat = CasingModel().CasingCement().Material(Controller().StartStage()).Material(element);
  return &CasingModel().CasingCement().GetCementMaterial(mat);
  }

  return 0;
}

const dia::IMaterial* CWellCasingDianaRunnerHelper::InterfaceMaterial(const geo::IElement& element) const
{
  const dia::IMaterial* pMat = 0;
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(pIface)
  {
  if(CasingModel().Mesh().IsOuterInterfaceElement(*pIface))
  {
      assert(&pIface->Front() == &pIface->Back());
      const CWellCasingModel::CBoundary& boundary = static_cast<const CWellCasingModel::CBoundary&>(CasingModel().Boundary());
      pMat = &boundary.InterfaceMaterial(*pIface);
  }
  else
  {
      assert(CasingModel().Mesh().IsCementInterfaceElement(*pIface));
      CFFMaterial::TValueMap mpValueInit;

      if(Controller().AnalysisType() == CAnalysisType::AT_HEAT)
      {
    // determine value for DFLUX
    // DFLUX = 1000 * CONDUC / L
    // CONDUC = conductivity of adjacent elements
    // L      = characteristic length of adjacent elements
    if(CasingModel().Mesh().SteelElements().ElementSize() > 0)
    {
          const geo::IBody& body = CasingModel().Mesh().SteelElements().Body(0);
          double dConduc = CasingModel().CasingNode().Steel().Material(Controller().StartStage()).Material(body).ParameterValue(IDT_VALUETYPE_THERM_CONDUCT);
          double dL = body.RepresentativeLength();
          if(fabs(dL) > 1.e-10)
          {
      double dDFlux = 1000. * dConduc * SECONDS_PER_MONTH / dL;
      mpValueInit.insert(CFFMaterial::TValueMap::value_type(IDT_VALUETYPE_DFLUX, dDFlux));
          }
    }
      }

      // apply full stick or full slip normal and shear stiffnesses
      switch(CasingModel().CasingNode().CementInterface().SlipType())
      {
      case CWellCasingCementInterface::SLIP:
    mpValueInit.insert(CFFMaterial::TValueMap::value_type(IDT_VALUETYPE_NORMALSTIFFNESS, 1e3));
    mpValueInit.insert(CFFMaterial::TValueMap::value_type(IDT_VALUETYPE_SHEARSTIFFNESS, 1e-3));
    break;
      case CWellCasingCementInterface::STICK:
    mpValueInit.insert(CFFMaterial::TValueMap::value_type(IDT_VALUETYPE_NORMALSTIFFNESS, 1e3));
    mpValueInit.insert(CFFMaterial::TValueMap::value_type(IDT_VALUETYPE_SHEARSTIFFNESS, 1e3));
    break;
      case CWellCasingCementInterface::USER:
    // leave it as it is...
    break;
      default:
    assert(false);
      }

      pMat = &CasingModel().CasingNode().CementInterface().Material(Controller().StartStage()).Material(*pIface, mpValueInit);
  }
  }

  return pMat;
}

bool CWellCasingDianaRunnerHelper::WriteBoundaryLoads(const TStageMap& mpStages)
{
  if(m_model.HasDistributedBoundaryLoads())
  return WriteBoundaryLoadsFromDistributions(mpStages);

  // no distributions, take boundary loads from parent model
  return WriteBoundaryLoadsFromParentModel(mpStages);
}

const CWellCasingModel& CWellCasingDianaRunnerHelper::CasingModel() const
{
  assert(dynamic_cast<const CWellCasingModel*>(&m_model));
  return static_cast<const CWellCasingModel&>(m_model);
}

const CDianaRunController& CWellCasingDianaRunnerHelper::Controller() const
{
  return m_controller;
}

bool CWellCasingDianaRunnerHelper::WriteBoundaryLoadsFromDistributions(const TStageMap& mpStages)
{
  const IWellModel::CBoundary& boundary = static_cast<const IWellModel::CBoundary&>(m_model.Boundary());
  const IWellModel::CBoundary::CDisplacementSupport& displasup = boundary.DisplacementSupport();
  const IWellModel::CBoundary::CPressureSupport& pressusup = boundary.PressureSupport();

  const CWellCasingModel& model = CasingModel();

  typedef std::set<std::pair<const CDepletionStage*, const geo::INode*> > TNodeSet;
  TNodeSet stProcessedNodes;

  int nBound = boundary.BoundaryElementSize();
  int i;
  for(i = 0; i < nBound; ++i)
  {
  const geo::CInterfaceElement& iface = boundary.BoundaryElement(i);
  const geo::IFace& bodyface = model.Mesh().BodyFace(iface);
  geo::CVector vecNormal = bodyface.Normal().UnitVector();

  TStageMap::const_iterator it;
  for(it = mpStages.begin(); it != mpStages.end(); ++it)
  {
      const CDepletionStage& stage = *it->first;
      dia::CLoadCase& lcase = *it->second;

      geo::CVector vecPressureDir;
      std::vector<double> vcPressureValues(bodyface.NrOfNodes(), 0);

      int n;
      for(n = 0; n < bodyface.NrOfNodes(); ++n)
      {
    const geo::INode& node = bodyface.Node(n);

    // pressure
    const CValueType* pVTP = pressusup.DistriValues(stage);
    if(pVTP)
    {
          assert(pVTP->ComponentSize() == 6);

          std::vector<geo::CValue> vals(pVTP->ComponentSize());
          unsigned int j;
          bool bAllValid = true;
          for(j = 0; j < pVTP->ComponentSize(); ++j)
          {
      vals[j] = pVTP->Component(j).ScalarData().ValuePoint(node);
      if(!vals[j].Valid())
      {
              bAllValid = false;
              break;
      }
          }

          if(bAllValid)
          {
      CTensor t(vals[0].Value(),
                      vals[1].Value(),
                      vals[2].Value(),
                      vals[3].Value(),
                      vals[4].Value(),
                      vals[5].Value());
      geo::CVector vecPressure(-(t.Matrix() * vecNormal * 1e6)); // rotate to face and convert to Pa
      vcPressureValues[n] = vecPressure.Length();
      if(vcPressureValues[n] > EPS && vecPressureDir.Empty())
              vecPressureDir = vecPressure.UnitVector();
          }
    }

    // displacement
    bool bNotProcessed = stProcessedNodes.insert(std::make_pair(&stage, &node)).second;

    if(bNotProcessed)
    {
          const CValueType* pVTD = displasup.DistriValues(stage);
          if(pVTD)
          {
      assert(pVTD->ComponentSize() == 3);

      int nBoundaryNode = node.Index() + 1; // always the next
      assert(node == model.Mesh().Mesh().Node(nBoundaryNode)); // colocated

      assert(model.Mesh().Mesh().HasTranslationSupport(nBoundaryNode));
      const geo::CTranslationSupport& support = model.Mesh().Mesh().TranslationSupport(nBoundaryNode);

      std::vector<geo::CValue> vals(3);
      unsigned int j;
      bool bAllValid = true;
      for(j = 0; j < pVTD->ComponentSize(); ++j)
      {
              vals[j] = pVTD->Component(j).ScalarData().ValuePoint(node);
              if(!vals[j].Valid())
              {
        bAllValid = false;
        break;
              }
      }

      if(bAllValid)
      {
              geo::CVector v(vals[0].Value(),
                             vals[1].Value(),
                             vals[2].Value());
              double dDisplaLength = v.Length();
              if(dDisplaLength > EPS)
        new dia::CPrescribedDisplacement(lcase, dDisplaLength, v.UnitVector(), support);
      }
          }
    }
      }

      if(!vecPressureDir.Empty())
    new dia::CBodyFaceLoad(lcase, vcPressureValues, vecPressureDir, *bodyface.Parent(), bodyface.Index());
  }
  }

  return true;
}

bool CWellCasingDianaRunnerHelper::WriteBoundaryLoadsFromParentModel(const TStageMap& mpStages)
{
  const CWellCasingModel& model = CasingModel();

  typedef std::set<std::pair<const CDepletionStage*, const geo::INode*> > TNodeSet;
  TNodeSet stProcessedNodes;

  const CWellCasingModel::CBoundary& boundary = static_cast<const CWellCasingModel::CBoundary&>(model.Boundary());
  int nBound = boundary.BoundaryElementSize();

  for(int i = 0; i < nBound; ++i)
  {
  const geo::CInterfaceElement& iface = boundary.BoundaryElement(i);
  assert(model.Mesh().IsOuterInterfaceElement(iface));

  const geo::IFace& bodyface = model.Mesh().BodyFace(iface);

  bool bNodeOutsideParentMesh = false;
  int n;
  for(n = 0; n < bodyface.NrOfNodes(); ++n)
  {
      std::vector<int> vcElementIndices = CasingModel().ParentModel().Mesh().Mesh().ElementsAt(bodyface.Node(n));
      if(vcElementIndices.empty())
      {
    // can't write for this element
    bNodeOutsideParentMesh = true;
    break;
      }
  }

  if(!bNodeOutsideParentMesh)
  {
      // create the boundary load for each loadcase


      // find initial stage

      TStageMap::const_iterator initialIterator = mpStages.end();

      for (TStageMap::const_iterator it = mpStages.begin(); it != mpStages.end(); ++it)
      {
    const CDepletionStage& stage = *it->first;

    if (stage.IsMarkedAsInitial())
    {
          assert(initialIterator == mpStages.end());
          initialIterator = it;
#ifndef _DEBUG
          break;
#endif
    }
      }

      assert(initialIterator != mpStages.end());

      const CDepletionStage& parentInitialStage = model.ParentEquivalentDepletionStage(*initialIterator->first);
      const CModelBase& parentModel = model.ParentModel();

      CStressTensorValueSet initialStress = parentModel.ResultRegister().TotalStress(parentInitialStage,
    m_bParentLinearResults ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);


      TStageMap::const_iterator it;
      for(it = mpStages.begin(); it != mpStages.end(); ++it)
      {
    const CDepletionStage& stage = *it->first;
    dia::CLoadCase& lcase = *it->second;

    const CDepletionStage& parentStage = model.ParentEquivalentDepletionStage(stage);

    // get results from parent mesh
    CStressTensorValueSet stress = parentModel.ResultRegister().TotalStress(parentStage,
          m_bParentLinearResults ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);
    CVectorValueSet displa = parentModel.ResultRegister().Displacement(parentStage,
          m_bParentLinearResults ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    geo::CVector vecPressureDirection; // direction of the bodyface load
    std::vector<double> vcPressureValues(bodyface.NrOfNodes()); // values of the bodyface load in each node

    for(n = 0; n < bodyface.NrOfNodes(); ++n)
    {
          const geo::INode& node = bodyface.Node(n);

          CStressTensor stresstensor = stress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);

          if (it != initialIterator) // in D1 and higher we need to write the difference with D0
          {
      stresstensor = stresstensor - initialStress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);
          }

          // apply the stress tensor values to the inner node of the interface element
          geo::CVector vecPressure = -(stresstensor.Matrix() * bodyface.Normal().UnitVector());

          // convert to Pa
          vecPressure = vecPressure * 1e6;

          vcPressureValues[n] = vecPressure.Length();
          if(vecPressureDirection.Empty())
      vecPressureDirection = vecPressure.UnitVector();

          // displacement
          bool bNotProcessed = stProcessedNodes.insert(std::make_pair(&stage, &node)).second;

          if(bNotProcessed)
          {
      int nBoundaryNode = node.Index() + 1; // always the next
      assert(node == model.Mesh().Mesh().Node(nBoundaryNode)); // should be the same location

      // get the support
      assert(model.Mesh().Mesh().HasTranslationSupport(nBoundaryNode));
      const geo::CTranslationSupport& support = model.Mesh().Mesh().TranslationSupport(nBoundaryNode);

      // get the displacement result value for the node
      geo::CVector vecDispla;

      std::vector<int> vcElementIndex = parentModel.Mesh().Mesh().ElementsAt(node);
      for(size_t i2 = 0; i2 < vcElementIndex.size(); ++i2)
      {
              const geo::IElement& elm = parentModel.Mesh().Mesh().Element(vcElementIndex[i2]);
              vecDispla = GetDisplacementVector(displa, elm, node);
              if(!vecDispla.Empty())
        break;
      }

      assert(!vecDispla.Empty());
      if(!vecDispla.Empty())
      {
              // create the load
              double dDisplaLength = vecDispla.Length();
              if(dDisplaLength > EPS)
        new dia::CPrescribedDisplacement(lcase, dDisplaLength, vecDispla.UnitVector(), support);
      }
          }

    }

    assert(!vecPressureDirection.Empty());
    new dia::CBodyFaceLoad(lcase, vcPressureValues, vecPressureDirection, *bodyface.Parent(), bodyface.Index());
      }
  }

  m_progress->Step(LOADS_PROGRESS_SCALE);
  }

  return true;
}

geo::CVector CWellCasingDianaRunnerHelper::GetDisplacementVector(const CVectorValueSet& displa, const geo::IElement& elm, const geo::IPoint& ptSample) const
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&elm);
  if(pIface)
  {
  if(pIface->FrontFace())
      return GetDisplacementVector(displa, *pIface->FrontFace(), ptSample);

  assert(pIface->BackFace());
  return GetDisplacementVector(displa, *pIface->BackFace(), ptSample);
  }

  CVectorValueSet::TValueVec values(elm.NrOfNodes());
  displa.ElementValues(values, elm.Index(), geo::IParallelInitializationCallback::Sequential);

  bool bValid = true;
  for(size_t j = 0; j < values.size(); j++)
  {
  if(!displa.IsValid(values[j]))
  {
      bValid = false;
      break;
  }
  }

  geo::CVector vecDispla;
  if(bValid)
  {
  geo::IElement::TDoubleVec vcShape = elm.ShapeFunction(elm.WorldToIso(ptSample));
  assert(vcShape.size() == values.size());

  vecDispla = values[0] * vcShape[0];
  for(size_t j = 1; j < values.size(); j++)
      vecDispla += values[j] * vcShape[j];
  }

  return vecDispla;
}
