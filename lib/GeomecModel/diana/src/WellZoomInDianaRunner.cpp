
#include "WellZoomInDianaRunner.h"

#include "WellZoomInModel.h"
#include "BoundaryInterfaceMaterial.h"
#include "ResultRegister.h"
#include "DCInitialStressLoad.h"
#include "DCPrescribedDisplacement.h"
#include "DCBodyFaceLoad.h"
#include "VectorValueSet.h"
#include "IParallelInitializationCallback.h"

CWellZoomInDianaRunner::CWellZoomInDianaRunner(CWellZoomInModel& model, CDianaRunController& controller)
: IWellModelDianaRunner(model, controller)
{
}

CWellZoomInDianaRunner::~CWellZoomInDianaRunner()
{
}

bool CWellZoomInDianaRunner::FetchInitialPressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialPressures)
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(!pIface)
  FetchElementInitialPressures(stage, element, vcInitialPressures);

  return true;
}

bool CWellZoomInDianaRunner::CreateInitialLoads(
  const geo::IElement &element, 
  const CDepletionStage &stage, 
  dia::CLoadCase &lcase, 
  const IValueDomainScalar::TValueVec &vcInitialPressures, 
  const IValueDomainScalar::TValueVec &vcInitialTemperatures)
{
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *> (&element);
  if(!pIface)
  {
  // write the initial pressures
  WriteElementInitialLoads(stage, element, lcase, vcInitialPressures, vcInitialTemperatures);

  CWellZoomInModel& model = ZoomInModel();

  // write the initial stresses
  geo::CPoint ptSample = element.MidPoint();
  assert(!ptSample.Empty());

  std::vector<int> vcElementIndices = model.ParentModel().Mesh().Mesh().ElementsAt(ptSample);

  // can only write when sample point is inside parent model's mesh
  if(!vcElementIndices.empty())
  {
      const CDepletionStage& parentStage = model.ParentEquivalentDepletionStage(stage);
      const CModelBase& parentModel = model.ParentModel();

      CStressTensorValueSet stress = parentModel.ResultRegister().EffectiveStress(parentStage,
    ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

      CStressTensor stresstensor = stress.ValuePoint(ptSample, geo::IParallelInitializationCallback::Sequential);

      std::vector<double> v(6);
      v[0] = -stresstensor.XX() * 1e6;
      v[1] = -stresstensor.YY() * 1e6;
      v[2] = -stresstensor.ZZ() * 1e6;
      v[3] = stresstensor.XY() * 1e6;
      v[4] = stresstensor.YZ() * 1e6;
      v[5] = stresstensor.XZ() * 1e6;

      // create the initial stress load
      new dia::CInitialStressLoad(lcase, CStressTensor(v[0], v[1], v[2], v[3], v[4], v[5]), element);
  }
  }

  m_progress->Step();

  return true;
}

bool CWellZoomInDianaRunner::CreateStageLoads(
  const geo::IElement &element, 
  const CDepletionStage &stage, 
  dia::CLoadCase &lcase, 
  const IValueDomainScalar::TValueVec &vcInitialPressures, 
  const IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(!pIface)
  WriteElementStageLoads(stage, element, lcase, vcInitialPressures, vcInitialTemperatures);

  m_progress->Step();

  return true;
}

bool CWellZoomInDianaRunner::WriteBoundaryLoads(TStageMap& mpStages)
{
  if(ZoomInModel().HasDistributedBoundaryLoads())
  return WriteBoundaryLoadsFromDistributions(mpStages);

  // no distributions, take boundary loads from parent model
  return WriteBoundaryLoadsFromParentModel(mpStages);
}

const dia::IMaterial* CWellZoomInDianaRunner::InterfaceMaterial(const geo::IElement &element) const
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(pIface)
  {
  assert(&pIface->Front() == &pIface->Back());
  const CWellZoomInModel::CBoundary& boundary = static_cast<const CWellZoomInModel::CBoundary&>(Model().Boundary());
  return &boundary.InterfaceMaterial(*pIface);
  }

  return 0;
}

const CWellZoomInModel& CWellZoomInDianaRunner::ZoomInModel() const
{
  return static_cast<const CWellZoomInModel&>(Model());
}

CWellZoomInModel& CWellZoomInDianaRunner::ZoomInModel()
{
  return static_cast<CWellZoomInModel&>(Model());
}

bool CWellZoomInDianaRunner::WriteBoundaryLoadsFromDistributions(TStageMap& mpStages)
{
  const CWellZoomInModel& model = ZoomInModel();
  const IWellModel::CBoundary& boundary = static_cast<const IWellModel::CBoundary&>(model.Boundary());
  const IWellModel::CBoundary::CDisplacementSupport& displasup = boundary.DisplacementSupport();
  const IWellModel::CBoundary::CPressureSupport& pressusup = boundary.PressureSupport();

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

      int nBoundaryNode = model.Mesh().MeshNodeToBoundaryNode(node.Index());
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

bool CWellZoomInDianaRunner::WriteBoundaryLoadsFromParentModel(TStageMap &mpStages)
{
  CWellZoomInModel& model = ZoomInModel();

  typedef std::set<std::pair<const CDepletionStage*, const geo::INode*> > TNodeSet;
  TNodeSet stProcessedNodes;

  const CWellZoomInModel::CBoundary& boundary = static_cast<const CWellZoomInModel::CBoundary&>(model.Boundary());
  int n = boundary.BoundaryElementSize();
  int i;
  for(i = 0; i < n; ++i)
  {
  const geo::CInterfaceElement& iface = boundary.BoundaryElement(i);

  // get the sample point for this interface element
  geo::CPoint ptSample1 = model.Mesh().SamplePosition(iface);
  assert(!ptSample1.Empty());

  std::vector<int> vcElementIndices = model.ParentModel().Mesh().Mesh().ElementsAt(ptSample1);

  // we can only write a boundary load when the sample point is inside an element in the parent model's mesh
  if(!vcElementIndices.empty())
  {
      // the bodyface to which the load is applied
      const geo::IFace& bodyface = model.Mesh().BodyFace(iface);

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
    ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);


      TStageMap::const_iterator it;
      for(it = mpStages.begin(); it != mpStages.end(); ++it)
      {
    const CDepletionStage& stage = *it->first;
    dia::CLoadCase& lcase = *it->second;

    const CDepletionStage& parentStage = model.ParentEquivalentDepletionStage(stage);

    CStressTensorValueSet stress = parentModel.ResultRegister().TotalStress(parentStage,
          ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);
    CStressTensor stresstensor = stress.ValuePoint(ptSample1, geo::IParallelInitializationCallback::Sequential);

    if (it != initialIterator) // in D1 and higher we need to write the difference with D0
    {
          stresstensor = stresstensor - initialStress.ValuePoint(ptSample1, geo::IParallelInitializationCallback::Sequential);
    }

    // apply the stress tensor values to the inner nodes of the interface element (the bodyface)
    geo::CVector vecPressure = -(stresstensor.Matrix() * bodyface.Normal().UnitVector());

    // write values in Pa
    vecPressure = vecPressure * 1e6;

    // create the load
    new dia::CBodyFaceLoad(lcase, vecPressure.Length(), vecPressure.UnitVector(), *bodyface.Parent(), bodyface.Index());

    // displacement
    CVectorValueSet displa = parentModel.ResultRegister().Displacement(parentStage,
          ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    // iterate over the bodyface's nodes
    int iNode;
    for(iNode = 0; iNode < bodyface.NrOfNodes(); ++iNode)
    {
          const geo::INode& node = bodyface.Node(iNode);

          // see if this node has been processed already
          bool bNotProcessed = stProcessedNodes.insert(std::make_pair(&stage, &node)).second;

          if(bNotProcessed)
          {
      // get the index of the boundary node (outside the interface)
      int nBoundaryNode = model.Mesh().MeshNodeToBoundaryNode(node.Index());

      // get the support
      assert(model.Mesh().Mesh().HasTranslationSupport(nBoundaryNode));
      const geo::CTranslationSupport& support = model.Mesh().Mesh().TranslationSupport(nBoundaryNode);

      // get the sample point
      geo::CPoint ptSample2 = model.Mesh().SamplePosition(node);

      // get the displacement result value for that point
      geo::CVector vecDispla;

      // the elements at the sample position
      std::vector<int> vcElementIndex = parentModel.Mesh().Mesh().ElementsAt(ptSample2);
      for(size_t i2 = 0; i2 < vcElementIndex.size(); ++i2)
      {
              const geo::IElement& elm = parentModel.Mesh().Mesh().Element(vcElementIndex[i2]);
              vecDispla = GetDisplacementVector(displa, elm, ptSample2);
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
      }
  }

  m_progress->Step();
  }

  return true;
}