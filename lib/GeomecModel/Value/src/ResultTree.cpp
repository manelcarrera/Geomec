/////////////////////////////////////////////////////////////////////
//
// ResultTree.cpp: implementation of the CResultTree class.
//
/////////////////////////////////////////////////////////////////////

#include "ResultTree.h"
#include "TextTagUtils.h"
#include "FieldFactor.h"
#include "FormationBase.h"
#include "CrossSection.h"
#include "NewWellPath.h"
#include "FormationPlane.h"
#include "StaborResult.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "MF.h"
#include "Temperature.h"
#include "DerivedResult.h"
#include "FaultPressure.h"
#include "WellCasingModel.h"
#include "WellCasingCementInterface.h"
#include "WellCasingSteel.h"
#include "WellCasingCement.h"
#include "WellCasingNode.h"
#include "WellCasingRTCI.h"
#include "TetraSubHorizon.h"
#include "HexaHorizon.h"
#include "VectorValueSet.h"
#include "FaultParametersNode.h"
#include "InterfaceMaterial.h"
#include "GeoSurface.h"
#include "MaterialFractureApertureBase.h"
#include "NonMeshedSurface.h"
#include "NonMeshedSurfaceParametersNode.h"
#include "NonMeshedSurfacePressure.h"
#include "ResultInfo.h"
#include "VolumetricStrainLoad.h"
#include "ResultCache.h"
#include "MeshDataCacher.h"
#include "IElementSet.h"
#include "ValueMapper.h"
#include "WellCasingModel.h"
#include "WellCasingMesh.h"

#include <map>

#include "StrainLoad.h"

#define EPS_PORE 1E-2

namespace {

static const geo::CVector vecEmpty;

}

// These two local functions are used in the can b
static bool CanMapOnVolumeElements(const COpenGLNode& node, int nRegister)
{
  if(dynamic_cast<const IFormationElementSet*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CFormationBase*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CFormationPlane*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CCrossSection::CCSFormation*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CNewWellPath*>(&node)) // wjrx mantis 3440
	return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const IPointSet*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CWellCasingCement*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CWellCasingSteel*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CWellCasingRTCI*>(&node))
    return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CTetraSubHorizon*>(&node))
      return nRegister == PRIMARY_RESULT;

  if(dynamic_cast<const CHexaHorizon*>(&node))
      return nRegister == PRIMARY_RESULT;

  return false;
}

static bool CanMapOnFaultElements(const COpenGLNode& node, int nRegister)
{
  const CHorizonBase* pHorizon = dynamic_cast<const CHorizonBase*>(&node);

  // don't map directly on the 2D faults, but on their interface nodes (see above)
  if(pHorizon)
    return (pHorizon->Slip() == true) && nRegister == PRIMARY_RESULT;

  return (dynamic_cast<const CWellCasingCementInterface*>(&node) != 0);
}

static bool CanMapOnBoundaryElements(const COpenGLNode& node, int nRegister)
{
  return dynamic_cast<const CBoundaryBase *>(&node) != 0;
}

static geo::CVector FaultElementNormal(const geo::IElement& elm)
{
  const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
  assert(pInterface);

  const geo::IFace* pFace = dynamic_cast<const geo::IFace*> (&pInterface->Front());
  if(pFace) return pFace->Normal();

  const geo::ILine* pLine = dynamic_cast<const geo::ILine*> (&pInterface->Front());
  assert(pLine);
  return pLine->Dir().CrossProduct(geo::CVector::Zaxis).UnitVector();
}


//////////////////////////////////////////////////////////////////////
// The implementation of CEffectiveStressResult
//////////////////////////////////////////////////////////////////////
CResultTree::CEffectiveStressResult::CEffectiveStressResult(unsigned int uName, CResultGroup& group, bool bChange)
: IStressTensorGroup(uName, &CResultRegister::EffectiveStress, group, bChange)
{
  CreateComposite();
}

CResultTree::CEffectiveStressResult::CEffectiveStressResult(const QString& sName, CResultGroup& group, bool bChange)
: IStressTensorGroup(sName, &CResultRegister::EffectiveStress, group, bChange)
{
  CreateComposite();
}

bool CResultTree::CEffectiveStressResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return antype == CAnalysisType::AT_LINEAR              ||
         antype == CAnalysisType::AT_MIXTURE             ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE);
}

double CResultTree::CEffectiveStressResult::Convert(const double& dValue, CQuantity::UNIT unit) const
{
  if(unit == CQuantity::FIELD_UNIT)
    return dValue * FF_FACTOR_STRESS;
  return dValue;
}

unsigned int CResultTree::CEffectiveStressResult::PreExportNameId() const
{
  if(Change())
    return IDS_ET_EFFECTIVE_CHANGE;
  return IDS_ET_EFFECTIVE;
}

unsigned int CResultTree::CEffectiveStressResult::ExportNameId() const
{
  return IDS_ET_STRESS;
}

unsigned int CResultTree::CEffectiveStressResult::TypeId() const
{
  return IDT_ET_STRESS;
}

QString CResultTree::CEffectiveStressResult::TypeName() const
{
  return getStringTableEntry(IDS_ET_STRESS);
}

unsigned int CResultTree::CEffectiveStressResult::PostExportNameId() const
{
  return 0;
}

unsigned int CResultTree::CEffectiveStressResult::ComponentIndex() const
{
  if(Change())
    return RC_EFF_STRESS_COMPONENT_CHANGE;
  return RC_EFF_STRESS_COMPONENT;
}

unsigned int CResultTree::CEffectiveStressResult::VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  if(Change())
  {
    switch(direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_EFF_STRESS_PRINC_MAX_CHANGE;
    case CVectorComposite::PD_MED:
      return RC_EFF_STRESS_PRINC_MED_CHANGE;
    case CVectorComposite::PD_MIN:
      return RC_EFF_STRESS_PRINC_MIN_CHANGE;
    };
  }
  else
  {
    switch(direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_EFF_STRESS_PRINC_MAX;
    case CVectorComposite::PD_MED:
      return RC_EFF_STRESS_PRINC_MED;
    case CVectorComposite::PD_MIN:
      return RC_EFF_STRESS_PRINC_MIN;
    };
  }
  assert(false);
  return 0;
}

unsigned int CResultTree::CEffectiveStressResult::InvariantIndex() const
{
  if(Change())
    return RC_EFF_STRESS_INVARIANT_CHANGE;
  return RC_EFF_STRESS_INVARIANT;
}

bool CResultTree::CEffectiveStressResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnVolumeElements(node, nRegister);
}

unsigned int CResultTree::CEffectiveStressResult::WellPathStressIndex() const
{
  if (Change())
  {
    return RC_EFF_STRESS_WPSTRESS_CHANGE;
  }

  return RC_EFF_STRESS_WPSTRESS;
}


///// CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent
CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::CSurfaceStressComponent(unsigned int uName,
                                                                                              CSurfaceStressComposite& parent,
                                                                                              TComponent comp,
                                                                                              const CDepletionStage& stage,
                                                                                              const CAnalysisType& antype,
                                                                                              int nRegister)
: IResultComponent(uName, parent, stage, antype, nRegister, int(comp)),
  m_comp(comp)
{
}

unsigned int CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::IconId() const
{
  return IDI_RESULT;
}

unsigned int CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::TypeId() const
{
  return 0;
}

QString CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::UnitName(const UNIT unit) const
{
  if(unit == CDoubleQuantity::FIELD_UNIT)
    return "psi";
  return "MPa";
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::CanComputeOnPoints() const
{
    return false;
}

geo::CValue CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::ValuePoint(const geo::IPoint& /*pt*/, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  assert(false);
  return geo::CValue();
}

void CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT /*unit*/, geo::IParallelInitializationCallback *cb) const
{
  assert(dynamic_cast<const geo::IFace*>(&elm));
  const geo::IFace& face = static_cast<const geo::IFace&>(elm);

  assert(dynamic_cast<const IStressTensorGroup*>(Parent().parent()));

  const IStressTensorGroup& tensor_group = static_cast<const IStressTensorGroup&>(*Parent().parent());

  geo::CVector vecNormal = face.Normal();

  switch(m_comp)
  {
  case SSTC_NORMAL:
    values = tensor_group.AxialValueElement(elm, map_type, *this, vecNormal, cb);
    break;
  case SSTC_SHEAR_PLANE:
    values = tensor_group.ShearValueInPlaneElement(elm, map_type, *this, vecNormal, cb);
    break;
  case SSTC_SHEAR_DIP_ANGLE:
    values = tensor_group.ShearDipAngleInPlaneElement(elm, map_type, *this, vecNormal, cb);
    break;
  default:
    assert(false);
  }
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::NeedParallelInitializationCallback() const
{
  return true;
}

geo::IParallelInitializationCallback *CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::GetParallelInitializationCallback()
{
  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  assert(pTensorGroup);
  return pTensorGroup->GetParallelInitializationCallback();
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::PrepareMapping(const geo::IElementSet* /*pElementSet*/)
{
  bool retval = true;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  return retval;
}

void CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComponent::FinishMapping()
{
  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  pTensorGroup->FinishMapping();
}



///// CResultTree::CTotalStressResult::CSurfaceStressComposite
CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComposite(unsigned int uName, CResultGroup& group, int storageNodeIndex)
: IResult(uName, group)
{
  m_pTensorGroup = static_cast<IStressTensorGroup*>(&group);
  BuildComponents();
  Index(storageNodeIndex);
}

CResultTree::CTotalStressResult::CSurfaceStressComposite::CSurfaceStressComposite(const QString& sName, CResultGroup& group, int storageNodeIndex)
: IResult(sName, group)
{
  m_pTensorGroup = static_cast<IStressTensorGroup*>(&group);
  BuildComponents();
  Index(storageNodeIndex);
}

unsigned int CResultTree::CTotalStressResult::CSurfaceStressComposite::IconId() const
{
  return IDI_RESULT_MAP;
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::OnDefined(const IResultComponent& component) const
{
  return m_pTensorGroup->OnDefined(component);
}

QString CResultTree::CTotalStressResult::CSurfaceStressComposite::ExportLabel(int nComponent) const
{
  QString sCompLabel;
  switch(nComponent)
  {
  case CSurfaceStressComponent::SSTC_NORMAL:
    sCompLabel = "Normal";
    break;
  case CSurfaceStressComponent::SSTC_SHEAR_PLANE:
    sCompLabel = "ShearPlane";
    break;
  case CSurfaceStressComponent::SSTC_SHEAR_DIP_ANGLE:
    sCompLabel = "ShearPlaneDipAngle";
    break;
  default:
    assert(false);
  }

	// Fetch names
	QString sPreName, sName, sPostName;
	if(m_pTensorGroup->PreExportNameId())
		sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
	sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
	if(m_pTensorGroup->PostExportNameId())
		sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

	return sPreName + "Surface" + sName + sCompLabel + sPostName; 
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::CanMap(const COpenGLNode& node, int /*nRegister*/) const
{
  return (dynamic_cast<const CSurfaceBase*>(&node) != 0 ||
          (dynamic_cast<const CHorizonBase*>(&node) != 0 && !static_cast<const CHorizonBase*>(&node)->Slip()));
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string sPreName, sName;
  if(m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());

  std::string sCombinedName= sPreName + "Surface"+ sName;

  return validateName(resultInfo, name, sCombinedName,
    SurfaceStressComponentList(), "_L|_M|_C|_N");
}

bool CResultTree::CTotalStressResult::CSurfaceStressComposite::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const
{
  return m_pTensorGroup->OnBuildComponent(stage, antype, nRegister);
}

void CResultTree::CTotalStressResult::CSurfaceStressComposite::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
  {
    new CSurfaceStressComponent(IDS_RC_NORMAL, *this, CSurfaceStressComponent::SSTC_NORMAL, stage, antype, nRegister);
    new CSurfaceStressComponent(IDS_RC_SHEAR_PLANE, *this, CSurfaceStressComponent::SSTC_SHEAR_PLANE, stage, antype, nRegister);
    new CSurfaceStressComponent(IDS_RC_SHEAR_DIP_ANGLE, *this, CSurfaceStressComponent::SSTC_SHEAR_DIP_ANGLE, stage, antype, nRegister);
  }
}


//////////////////////////////////////////////////////////////////////
// The implementation of CTotalStressResult
//////////////////////////////////////////////////////////////////////
CResultTree::CTotalStressResult::CTotalStressResult(unsigned int uName, CResultGroup& group, bool bChange)
: IStressTensorGroup(uName, &CResultRegister::TotalStress, group, bChange),
  m_pSurfaceStressComposite(0)
{
  CreateComposite();
}

CResultTree::CTotalStressResult::CTotalStressResult(const QString& sName, CResultGroup& group, bool bChange)
: IStressTensorGroup(sName, &CResultRegister::TotalStress, group, bChange),
  m_pSurfaceStressComposite(0)
{
  CreateComposite();
}

void CResultTree::CTotalStressResult::CreateComposite()
{
  IStressTensorGroup::CreateComposite();

  delete m_pSurfaceStressComposite;

  if (Change())
  {
    m_pSurfaceStressComposite = new CSurfaceStressComposite(IDS_RG_SURFACESTRESS, *this, RC_TOTAL_STRESS_SURFACESTRESS_CHANGE);
  }
  else
  {
    m_pSurfaceStressComposite = new CSurfaceStressComposite(IDS_RG_SURFACESTRESS, *this, RC_TOTAL_STRESS_SURFACESTRESS);
  }
}

bool CResultTree::CTotalStressResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return antype == CAnalysisType::AT_LINEAR  ||
         antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE);
}

double CResultTree::CTotalStressResult::Convert(const double& dValue, CQuantity::UNIT unit) const
{
  if(unit == CQuantity::FIELD_UNIT)
    return dValue * FF_FACTOR_STRESS;
  return dValue;
}

bool CResultTree::CTotalStressResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnVolumeElements(node, nRegister);
}

unsigned int CResultTree::CTotalStressResult::PreExportNameId() const
{
  if(Change())
    return IDS_ET_CHANGE;
  return 0;
}

unsigned int CResultTree::CTotalStressResult::TypeId() const
{
  return IDT_ET_STRESS;
}

unsigned int CResultTree::CTotalStressResult::ExportNameId() const
{
  return IDS_ET_STRESS;
}

QString CResultTree::CTotalStressResult::TypeName() const
{
  return getStringTableEntry(IDS_ET_STRESS);
}

unsigned int CResultTree::CTotalStressResult::PostExportNameId() const
{
  return 0;
}

unsigned int CResultTree::CTotalStressResult::ComponentIndex() const
{
  if(Change())
    return RC_TOTAL_STRESS_COMPONENT_CHANGE;
  return RC_TOTAL_STRESS_COMPONENT;
}

unsigned int CResultTree::CTotalStressResult::VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  if(Change())
  {
    switch(direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_TOTAL_STRESS_PRINC_MAX_CHANGE;
    case CVectorComposite::PD_MED:
      return RC_TOTAL_STRESS_PRINC_MED_CHANGE;
    case CVectorComposite::PD_MIN:
      return RC_TOTAL_STRESS_PRINC_MIN_CHANGE;
    };
  }
  else
  {
    switch(direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_TOTAL_STRESS_PRINC_MAX;
    case CVectorComposite::PD_MED:
      return RC_TOTAL_STRESS_PRINC_MED;
    case CVectorComposite::PD_MIN:
      return RC_TOTAL_STRESS_PRINC_MIN;
    };
  }
  assert(false);
  return 0;
}

unsigned int CResultTree::CTotalStressResult::InvariantIndex() const
{
  if(Change())
    return RC_TOTAL_STRESS_INVARIANT_CHANGE;
  return RC_TOTAL_STRESS_INVARIANT;
}

unsigned int CResultTree::CTotalStressResult::WellPathStressIndex() const
{
  if (Change())
  {
    return RC_TOTAL_STRESS_WPSTRESS_CHANGE;
  }

  return RC_TOTAL_STRESS_WPSTRESS;
}

//////////////////////////////////////////////////////////////////////
// The implementation of CParentStressInputResult
//////////////////////////////////////////////////////////////////////
CResultTree::CParentStressInputResult::CParentStressInputResult(unsigned int uName, CResultGroup& group, bool bChange)
  : CEffectiveStressResult(uName, group, bChange)
{
}

CResultTree::CParentStressInputResult::CParentStressInputResult(const QString& sName, CResultGroup& group, bool bChange)
  : CEffectiveStressResult(sName, group, bChange)
{
}

bool CResultTree::CParentStressInputResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return antype == CAnalysisType::AT_LINEAR ||
    antype == CAnalysisType::AT_MIXTURE ||
    antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
    (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE);
}

double CResultTree::CParentStressInputResult::Convert(const double& dValue, CQuantity::UNIT unit) const
{
  if (unit == CQuantity::FIELD_UNIT)
    return dValue * FF_FACTOR_STRESS;
  return dValue;
}

unsigned int CResultTree::CParentStressInputResult::PreExportNameId() const
{
  if (Change())
    return IDS_ET_PARENT_EFFECTIVE_CHANGE;
  return IDS_ET_PARENT_EFFECTIVE;
}

unsigned int CResultTree::CParentStressInputResult::ExportNameId() const
{
  return IDS_ET_PARENT_STRESS;
}

unsigned int CResultTree::CParentStressInputResult::TypeId() const
{
  return IDT_ET_STRESS;
}

QString CResultTree::CParentStressInputResult::TypeName() const
{
  return getStringTableEntry(IDS_ET_PARENT_STRESS);
}

unsigned int CResultTree::CParentStressInputResult::PostExportNameId() const
{
  return 0;
}

bool CResultTree::CParentStressInputResult::OnDefined(const IResultComponent& component) const
{
  assert(dynamic_cast<const IWellModel*>(&Model()));
  const IWellModel& model = static_cast<const IWellModel&>(Model());

  bool resultsAvailable = model.ParentLinearResults(component.AnalysisType()) ? 
    model.ParentModel().ResultRegister().Linear() : 
    model.ParentModel().ResultRegister().NonLinear();

  return model.Mesh().IsMesh() && resultsAvailable;
}

const ITensor& CResultTree::CParentStressInputResult::TensorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  const CDepletionStage& stage = component.Stage();
  bool bChange = false;
  const CAnalysisType& antype = component.AnalysisType();

  assert(!point.Empty());
  // Is the point in the cache?
  _ResultCache<CStressTensor>& cache = GetCache(component, cb);
  if (!cache.m_element && !cache.m_point.Empty() && point == cache.m_point && cache.m_stage == &stage && bChange == cache.m_bChange) {
    assert(cache.m_value.size() == 1);
    return cache.m_value[0];
  }

  cache.m_element = 0;
  cache.m_point = point;
  cache.m_bPrincipalValid = false;
  cache.m_stage = &stage;
  cache.m_bChange = bChange;
  if (cache.m_value.size() != 1) cache.m_value.resize(1);
  
  // cache.m_value[0] = (component.ResultRegister().*m_function)(stage, antype, bChange).ValuePoint(point, cb);

  return cache.m_value[0];
}

const ITensor& CResultTree::CParentStressInputResult::TensorElement(const geo::IElement& element, int nNodeIndex, TMapType map_type, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  // Is the element in the cache?
  const CDepletionStage& stage = component.Stage();
  bool bChange = false;
  const CAnalysisType& antype = component.AnalysisType();

  _ResultCache<CStressTensor>& cache = GetCache(component, cb);
  if ((cache.m_element == &element) && (map_type == cache.m_map_type) && cache.m_point.Empty() && cache.m_stage == &stage && bChange == cache.m_bChange) {
    assert(cache.m_value.size() == element.NrOfPoints());
    return cache.m_value[nNodeIndex];
  }

  cache.m_element = &element;
  cache.m_map_type = map_type;
  cache.m_point = geo::CPoint();
  cache.m_bPrincipalValid = false;
  cache.m_stage = &stage;
  cache.m_bChange = bChange;
  if (cache.m_value.size() != element.NrOfPoints()) cache.m_value.resize(element.NrOfPoints());
  TensorElementFromParentModel(cache, element, map_type, stage, antype);
  return cache.m_value[nNodeIndex];
}

void CResultTree::CParentStressInputResult::TensorElementFromParentModel(_ResultCache<CStressTensor>& cache, const geo::IElement& element, TMapType map_type, const CDepletionStage& stage, const CAnalysisType& antype) const
{
  const IWellModel *model = dynamic_cast<const IWellModel *>(&Model());
  if (!model)
    return;

  const geo::IInterfaceElement *ifelt = dynamic_cast<const geo::IInterfaceElement *>(&element);

  if (ifelt)
    return;

  bool isCasing = dynamic_cast<const CWellCasingModel *>(&Model());

  if (isCasing)
  {
    if (static_cast<const CWellCasingModel &>(Model()).Mesh().IsSteelElement(element))
      return;
  }

  const CDepletionStage& parentStage = model->ParentEquivalentDepletionStage(stage);

  const CDepletionStage *initialStage = &stage;

  while (!initialStage->IsMarkedAsInitial())
    initialStage = &initialStage->Previous();

  if (initialStage != &stage)
    return;

  const CDepletionStage& parentInitialStage = model->ParentEquivalentDepletionStage(*initialStage);

  const CModelBase& parentModel = model->ParentModel();

  CStressTensorValueSet initialStress = parentModel.ResultRegister().EffectiveStress(parentInitialStage, model->ParentLinearResults(antype) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

  if (isCasing)
  {
    for (int n = 0; n < element.NrOfNodes(); ++n)
    {
      const geo::INode& node = element.Node(n);

      CStressTensor stresstensor = initialStress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);

      cache.m_value[n] = stresstensor;
    }
  }
  else
  {
    geo::CPoint ptSample = element.MidPoint();

    CStressTensor stresstensor = initialStress.ValuePoint(ptSample, geo::IParallelInitializationCallback::Sequential);

    for (int n = 0; n < element.NrOfNodes(); ++n)
    {
      cache.m_value[n] = stresstensor;
    }
  }
}

#if 0
void CResultTree::CParentStressInputResult::TensorBoundaryElementFromParentModel(_ResultCache<CStressTensor>& cache, const geo::IElement& element, TMapType map_type, const CDepletionStage& stage, const CAnalysisType& antype) const
{
  const IWellModel *model = dynamic_cast<const IWellModel *>(&Model());
  if (!model)
    return;

  const geo::IInterfaceElement *ifelt = dynamic_cast<const geo::IInterfaceElement *>(&element);

  if (!ifelt)
    return;

  const CDepletionStage& parentStage = model->ParentEquivalentDepletionStage(stage);

  const CDepletionStage *initialStage = &stage;

  while (!initialStage->IsMarkedAsInitial())
    initialStage = &initialStage->Previous();

  const CDepletionStage& parentInitialStage = model->ParentEquivalentDepletionStage(*initialStage);

  const CModelBase& parentModel = model->ParentModel();

  CStressTensorValueSet initialStress = parentModel.ResultRegister().TotalStress(parentInitialStage, CAnalysisType::AT_NONLIN, false);

  CStressTensorValueSet stress = parentModel.ResultRegister().TotalStress(parentStage, CAnalysisType::AT_NONLIN, false);

  const geo::CBodyQuadrilateral *bodyface = dynamic_cast<const geo::CBodyQuadrilateral *>(&ifelt->Front());

  geo::CVector vecPressureDirection; // direction of the bodyface load
  std::vector<double> vcPressureValues(bodyface->NrOfNodes()); // values of the bodyface load in each node

  for (int n = 0; n < bodyface->NrOfNodes(); ++n)
  {
    const geo::INode& node = bodyface->Node(n);

    CStressTensor stresstensor = stress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);

    cache.m_value[n] = stresstensor;
  }

#if 0
  //if (&stage != initialStage) // in D1 and higher we need to write the difference with D0
  //{
  //  stresstensor = stresstensor - initialStress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);
  //}

  // apply the stress tensor values to the inner node of the interface element
  geo::CVector vecPressure = -(stresstensor.Matrix() * bodyface->Normal().UnitVector());

  // convert to Pa
  vecPressure = vecPressure * 1e6;

  vcPressureValues[n] = vecPressure.Length();
  if (vecPressureDirection.Empty())
    vecPressureDirection = vecPressure.UnitVector();
  for (int i = 0; i < element.NrOfNodes(); ++i)
    cache.m_value[i] = CTensor(.2, .2, .2, 0, 0, 0);
#endif
}
#endif

#if 0
unsigned int CResultTree::CBoundaryStressInputResult::ComponentIndex() const
{
  //if (Change())
  //  return RC_EFF_STRESS_COMPONENT_CHANGE;
  return RC_EFF_STRESS_COMPONENT;
}

unsigned int CResultTree::CBoundaryStressInputResult::VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  if (Change())
  {
    switch (direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_EFF_STRESS_PRINC_MAX_CHANGE;
    case CVectorComposite::PD_MED:
      return RC_EFF_STRESS_PRINC_MED_CHANGE;
    case CVectorComposite::PD_MIN:
      return RC_EFF_STRESS_PRINC_MIN_CHANGE;
    };
  }
  else
  {
    switch (direction)
    {
    case CVectorComposite::PD_MAX:
      return RC_EFF_STRESS_PRINC_MAX;
    case CVectorComposite::PD_MED:
      return RC_EFF_STRESS_PRINC_MED;
    case CVectorComposite::PD_MIN:
      return RC_EFF_STRESS_PRINC_MIN;
    };
  }
  assert(false);
  return 0;
}

unsigned int CResultTree::CBoundaryStressInputResult::InvariantIndex() const
{
  if (Change())
    return RC_EFF_STRESS_INVARIANT_CHANGE;
  return RC_EFF_STRESS_INVARIANT;
}
#endif

bool CResultTree::CParentStressInputResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnVolumeElements(node, nRegister);
}

bool CResultTree::CParentStressInputResult::PrepareMapping(const geo::IElementSet *, const IValueComponentBase *)
{
  return false;
}

#if 0
unsigned int CResultTree::CBoundaryStressInputResult::WellPathStressIndex() const
{
  if (Change())
  {
    return RC_EFF_STRESS_WPSTRESS_CHANGE;
  }

  return RC_EFF_STRESS_WPSTRESS;
}
#endif


//////////////////////////////////////////////////////////////////////
// The implementation of CTotalStrainResult
//////////////////////////////////////////////////////////////////////
CResultTree::CTotalStrainResult::CTotalStrainResult(unsigned int uName, CResultGroup& group)
: IStrainTensorGroup(uName, &CResultRegister::TotalStrain, group)
{
  CreateComposite();
}

CResultTree::CTotalStrainResult::CTotalStrainResult(const QString& sName, CResultGroup& group)
: IStrainTensorGroup(sName, &CResultRegister::TotalStrain, group)
{
  CreateComposite();
}

unsigned int CResultTree::CTotalStrainResult::PreExportNameId() const
{
  return 0;
}

unsigned int CResultTree::CTotalStrainResult::ExportNameId() const
{
  return IDS_ET_STRAIN;
}

unsigned int CResultTree::CTotalStrainResult::TypeId() const
{
  return IDT_ET_STRAIN;
}

QString CResultTree::CTotalStrainResult::TypeName() const
{
  return getStringTableEntry(IDS_ET_STRAIN);
}

unsigned int CResultTree::CTotalStrainResult::PostExportNameId() const
{
  return 0;
}

unsigned int CResultTree::CTotalStrainResult::ComponentIndex() const
{
  return RC_TOTAL_STRAIN_COMPONENT;
}

bool CResultTree::CTotalStrainResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
        (antype == CAnalysisType::AT_LINEAR  ||
         antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         antype == CAnalysisType::AT_HEAT    ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}


unsigned int CResultTree::CTotalStrainResult::VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  switch(direction)
  {
  case CVectorComposite::PD_MAX:
    return RC_TOTAL_STRAIN_PRINC_MAX;
  case CVectorComposite::PD_MED:
    return RC_TOTAL_STRAIN_PRINC_MED;
  case CVectorComposite::PD_MIN:
    return RC_TOTAL_STRAIN_PRINC_MIN;
  };

  assert(false);
  return 0;
}

bool CResultTree::CTotalStrainResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnVolumeElements(node, nRegister);
}

unsigned int CResultTree::CTotalStrainResult::InvariantIndex() const
{
  return RC_TOTAL_STRAIN_INVARIANT;
}

bool CResultTree::CTotalStrainResult::OnDefined(const IResultComponent& component) const
{
  if(component.AnalysisType() == CAnalysisType::AT_HEAT)
    return false;

  return IStrainTensorGroup::OnDefined(component);
}

//////////////////////////////////////////////////////////////////////
// The implementation of CPlasticStrainResult
//////////////////////////////////////////////////////////////////////
CResultTree::CPlasticStrainResult::CPlasticStrainResult(unsigned int uName, CResultGroup& group)
: IStrainTensorGroup(uName, &CResultRegister::PlasticStrain, group)
{
  CreateComposite();
}

CResultTree::CPlasticStrainResult::CPlasticStrainResult(const QString& sName, CResultGroup& group)
: IStrainTensorGroup(sName, &CResultRegister::PlasticStrain, group)
{
  CreateComposite();
}

bool CResultTree::CPlasticStrainResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage())                         &&
         (antype == CAnalysisType::AT_NONLIN || antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
         stage.OutputType() != CDepletionStage::NONE;
}

unsigned int CResultTree::CPlasticStrainResult::PreExportNameId() const
{
  return 0;
}

unsigned int CResultTree::CPlasticStrainResult::TypeId() const
{
  return IDT_ET_STRAIN;
}

QString CResultTree::CPlasticStrainResult::TypeName() const
{
  return getStringTableEntry(IDS_ET_STRAIN);
}

unsigned int CResultTree::CPlasticStrainResult::ExportNameId() const
{
  return IDS_ET_STRAIN;
}

unsigned int CResultTree::CPlasticStrainResult::PostExportNameId() const
{
  return IDS_ET_PLASTIC;
}

unsigned int CResultTree::CPlasticStrainResult::ComponentIndex() const
{
  return RC_PLASTIC_STRAIN_COMPONENT;
}

unsigned int CResultTree::CPlasticStrainResult::VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  switch(direction)
  {
  case CVectorComposite::PD_MAX:
    return RC_PLASTIC_STRAIN_PRINC_MAX;
  case CVectorComposite::PD_MED:
    return RC_PLASTIC_STRAIN_PRINC_MED;
  case CVectorComposite::PD_MIN:
    return RC_PLASTIC_STRAIN_PRINC_MIN;
  };

  assert(false);
  return 0;
}

unsigned int CResultTree::CPlasticStrainResult::InvariantIndex() const
{
  return RC_PLASTIC_STRAIN_INVARIANT;
}

bool CResultTree::CPlasticStrainResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnVolumeElements(node, nRegister);
}

bool CResultTree::CPlasticStrainResult::PrepareMapping(const geo::IElementSet *, const IValueComponentBase *pValueComponent)
{
  bool retval = true;

  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &static_cast<CModelBase&>(Model()).ResultRegister();

  int c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_PSTRAIN_XX);
  int c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_PSTRAIN_XX);

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 6))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 6))
    retval = false;

  c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);
  c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_POREPRES);

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 1))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 1))
    retval = false;

  return retval;
}



//////////////////////////////////////////////////////////////////////
// The implementation of CDisplacementResult
//////////////////////////////////////////////////////////////////////

CResultTree::CDisplacementResult::CDisplacementResult(unsigned int uName, CResultGroup& group)
: CVectorResult(uName, &CResultRegister::Displacement, group, false)
{
  BuildComponents();
  Index(RC_DISPLACEMENT);
;
}

CResultTree::CDisplacementResult::CDisplacementResult(const QString& sName, CResultGroup& group)
: CVectorResult(sName, &CResultRegister::Displacement, group, false)
{
  BuildComponents();
  Index(RC_DISPLACEMENT);
}

bool CResultTree::CDisplacementResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= VectorExportLabel().toStdString();

  return validateName(resultInfo, name, label, VectorResultComponentList(),
    "_L|_M|_C|_N");
}

bool CResultTree::CDisplacementResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
        (antype == CAnalysisType::AT_LINEAR  ||
         antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

double CResultTree::CDisplacementResult::ConvertToField(const double& dValue) const
{
  return dValue * FF_FACTOR_LENGTH;
}

QString CResultTree::CDisplacementResult::ExportLabel(int nComponent) const
{
  return VectorExportLabel() + IVectorResult::ExportLabel(nComponent);
}

QString CResultTree::CDisplacementResult::VectorExportLabel() const
{
  return getStringTableEntry(IDS_ET_DISPLACEMENT);
}

bool CResultTree::CDisplacementResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  const CHorizonBase* pHor = dynamic_cast<const CHorizonBase*>(&node);
  if(pHor)
    return !pHor->Slip() && nRegister == PRIMARY_RESULT;

  return CanMapOnVolumeElements(node, nRegister);
}

QString CResultTree::CDisplacementResult::UnitName(UNIT unit) const
{
  QString sUnit;
  if(unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_LENGTH);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
  return sUnit;
}

bool CResultTree::CDisplacementResult::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase *pValueComponent)
{
  bool retval = true;

  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &static_cast<CModelBase&>(Model()).ResultRegister();

  int c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_DISPLA_X);
  int c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_DISPLA_X);

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 3))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 3))
    retval = false;

  return retval;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CFaultScalarResult
//////////////////////////////////////////////////////////////////////

CResultTree::CFaultScalarResult::CFaultScalarResultComponent::CFaultScalarResultComponent(CFaultScalarResult& parent,    // The tensor composite composite parent
                                              const CDepletionStage& stage,  // Depletion stage of the component
                                              const CAnalysisType& antype,
                                              int nRegister)
: IScalarResultComponent("", parent, stage, antype, nRegister, 0)
{
}

bool CResultTree::CFaultScalarResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= ExportLabel(0).toStdString();

  return validateName(resultInfo, name, label, TensorResultComponentList(),
    "_L|_M|_C|_N");
}

bool CResultTree::CFaultScalarResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  if((ResultType() == DILATATION || ResultType() == PLASTIC_ENERGY) && !stage.isAfter(stage.Entry().MarkedAsInitialStage()))
    return false;

  if(ResultType() == PLASTIC_ENERGY)
    return (antype == CAnalysisType::AT_MIXTURE ||
            antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
            (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));

  return (antype == CAnalysisType::AT_LINEAR  ||
          antype == CAnalysisType::AT_MIXTURE ||
          antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
          (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

unsigned int CResultTree::CFaultScalarResult::CFaultScalarResultComponent::IconId() const
{
  return 0;
}

unsigned int CResultTree::CFaultScalarResult::CFaultScalarResultComponent::TypeId() const
{
  return 0;
}

QString CResultTree::CFaultScalarResult::CFaultScalarResultComponent::UnitName(const UNIT unit) const
{
  const CFaultScalarResult *pResult = dynamic_cast<const CFaultScalarResult*>(&Parent());
  assert(pResult);

  QString sUnit;

  switch(pResult->ResultType())
  {
  case DILATATION:
  case BOUNDARY_DISPLACEMENT_INPUT:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_LENGTH);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
    break;
  case SHEAR_CAPACITY:
    sUnit = getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
    break;
  case PORE_PRESSURE:
  case PORE_PRESSURE_CHANGE:
  case PORE_PRESSURE_INPUT:
  case PORE_PRESSURE_CHANGE_INPUT:
  case BOUNDARY_PRESSURE_INPUT:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
    break;
  case PLASTIC_ENERGY:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_FAULT_PLASTIC_ENERGY);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_FAULT_PLASTIC_ENERGY);
    break;
  default:
    assert(false);
    break;
  };

  return sUnit;
}

void CResultTree::CFaultScalarResult::CFaultScalarResultComponent::DilatationValues(const CDepletionStage& stage, IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, UNIT unit) const
{
  CVectorValueSet::TValueVec vcValue(values.size());
  ResultRegister().FaultDisplacement(stage, AnalysisType(), false).ElementValues(vcValue, mesh_element.Index(), 0);

  geo::CVector normal = FaultElementNormal(mesh_element);
  assert(fabs(normal.Length() - 1) < EPS);

  for(size_t i = 0; i < values.size(); i++) {
    if( !vcValue[i].Empty() )
    {
      double v = -vcValue[i].DotProduct(normal);
      if(unit == CQuantity::FIELD_UNIT)
        v *= FF_FACTOR_LENGTH;
      values[i] = v;
    }
    else
    {
      values[i] = geo::CValue();
    }
  }
}

void CResultTree::CFaultScalarResult::CFaultScalarResultComponent::PlasticEnergy(const CDepletionStage& stage, IValueDomainScalar::TValueVec& values, const geo::IElement& element, UNIT unit) const
{
  /*
  WI 81410:
    For Faults: Tau*d_sh   (MJ/m^2)
    Tau is the resolved shear and d_sh is the plastic displacement in m

    The total energy for a stage is the total energy of the previous stage + the energy of the current stage, which is
 
    Tau*Δd_sh

  */

  IValueDomainScalar::TValueVec previous_values;

  if(!stage.Initial() && (&stage.Previous() != &stage.InitialStage()))
  {
    PlasticEnergy(stage.Previous(), previous_values, element, unit);
  }
  else
  {
    previous_values.resize(element.NrOfPoints(), 0);
  }

  IValueDomainScalar::TValueVec vcShear;
  IValueDomainScalar::TValueVec vcPlasticDispl;
  IValueDomainScalar::TValueVec vcPlasticDisplPrev;

  const CModelBase& model = (const CModelBase&)Model();
  const IResultComponent* pComponent;
  const IResult* pComposite = &model.ResultTree().FaultShearStress();

  pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), IVectorResult::VC_LENGTH);
  if(pComponent)
    vcShear = pComponent->ValueElement(element, unit);

  pComposite = &model.ResultTree().FaultPlasticSlip();

  pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), IVectorResult::VC_LENGTH);
  if(pComponent)
    vcPlasticDispl = pComponent->ValueElement(element, unit);

  if(!stage.Initial() && (&stage.Previous() != &stage.InitialStage()))
  {
    pComponent = pComposite->ResultComponent(stage.Previous(), AnalysisType(), RegisterIndex(), IVectorResult::VC_LENGTH);
    if(pComponent)
      vcPlasticDisplPrev = pComponent->ValueElement(element, unit);
  }
  else
  {
    vcPlasticDisplPrev.resize(element.NrOfPoints(), 0);
  }

  if(vcShear.size()            == element.NrOfPoints() &&
     vcPlasticDispl.size()     == element.NrOfPoints() &&
     vcPlasticDisplPrev.size() == element.NrOfPoints())
  {
    values.resize(element.NrOfPoints());
    for(int i = 0; i < element.NrOfPoints(); ++i)
    {
      if(previous_values[i].Valid() &&
         vcShear[i].Valid()         &&
         vcPlasticDispl[i].Valid()  &&
         vcPlasticDisplPrev[i].Valid())
      {
        double Tau = vcShear[i].Value();
        double d_sh = vcPlasticDispl[i].Value() - vcPlasticDisplPrev[i].Value();
        values[i] = Tau * d_sh + previous_values[i].Value();
      }
    }
  }
}

namespace {
geo::CVector GetDisplacementVectorCasing(const CVectorValueSet& displa, const geo::IElement& elm, const geo::IPoint& ptSample)
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&elm);
  if (pIface)
  {
    if (pIface->FrontFace())
      return GetDisplacementVectorCasing(displa, *pIface->FrontFace(), ptSample);

    assert(pIface->BackFace());
    return GetDisplacementVectorCasing(displa, *pIface->BackFace(), ptSample);
  }

  CVectorValueSet::TValueVec values(elm.NrOfNodes());
  displa.ElementValues(values, elm.Index(), geo::IParallelInitializationCallback::Sequential);

  bool bValid = true;
  for (size_t j = 0; j < values.size(); j++)
  {
    if (!displa.IsValid(values[j]))
    {
      bValid = false;
      break;
    }
  }

  geo::CVector vecDispla;
  if (bValid)
  {
    geo::IElement::TDoubleVec vcShape = elm.ShapeFunction(elm.WorldToIso(ptSample));
    assert(vcShape.size() == values.size());

    vecDispla = values[0] * vcShape[0];
    for (size_t j = 1; j < values.size(); j++)
      vecDispla += values[j] * vcShape[j];
  }

  return vecDispla;
}

geo::CVector GetDisplacementVectorWellZoomIn(const CVectorValueSet& displa, const geo::IElement& elm, const geo::IPoint& ptSample)
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&elm);
  if (pIface)
  {
    if (pIface->FrontFace())
      return GetDisplacementVectorWellZoomIn(displa, *pIface->FrontFace(), ptSample);

    assert(pIface->BackFace());
    return GetDisplacementVectorWellZoomIn(displa, *pIface->BackFace(), ptSample);
  }

  CVectorValueSet::TValueVec values(elm.NrOfNodes());
  displa.ElementValues(values, elm.Index(), geo::IParallelInitializationCallback::Sequential);

  bool bValid = true;
  for (size_t j = 0; j < values.size(); j++)
  {
    if (!displa.IsValid(values[j]))
    {
      bValid = false;
      break;
    }
  }

  geo::CVector vecDispla;
  if (bValid)
  {
    geo::IElement::TDoubleVec vcShape = elm.ShapeFunction(elm.WorldToIso(ptSample));
    assert(vcShape.size() == values.size());

    vecDispla = values[0] * vcShape[0];
    for (size_t j = 1; j < values.size(); j++)
      vecDispla += values[j] * vcShape[j];
  }

  return vecDispla;
}

}


void CResultTree::CFaultScalarResult::CFaultScalarResultComponent::ElementValues(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, UNIT unit, geo::IParallelInitializationCallback* /*cb*/) const
{
  const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&mesh_element);
  if(!pInterface)
    return;

  assert(values.size() == mesh_element.NrOfPoints());

  const CFaultScalarResult *pResult = dynamic_cast<const CFaultScalarResult*>(&Parent());
  assert(pResult);

  switch(pResult->ResultType())
  {
  case DILATATION:
  {
/*
    CVectorValueSet::TValueVec vcValue(values.size());
    ResultRegister().FaultDisplacement(Stage(), Linear(), false).ElementValues(vcValue, mesh_element.Index());

    geo::CVector normal = FaultElementNormal(mesh_element);
    assert(fabs(normal.Length() - 1) < EPS);

    for(size_t i = 0; i < values.size(); i++) {
      if( !vcValue[i].Empty() ) 
        values[i] = vcValue[i].DotProduct(normal);
      else
        values[i] = geo::CValue();
    }
*/
    DilatationValues(Stage(), values, mesh_element, unit);

    // subtract values from (marked) initial stage
    const CDepletionStage& iniStage = Stage().Entry().MarkedAsInitialStage();
    IValueDomainScalar::TValueVec iniValues(values.size());
    DilatationValues(iniStage, iniValues, mesh_element, unit);
    assert(values.size() == iniValues.size());

    for(size_t i = 0; i < values.size(); ++i)
    {
      if(values[i].Valid() && iniValues[i].Valid())
        values[i] = values[i].Value() - iniValues[i].Value();
      else
        values[i] = geo::CValue(); // invalidate
    }
    break;
  }
  case SHEAR_CAPACITY:
  {
    const CModelBase &model = (const CModelBase&)(Model());
    // Retrieve eff stress and normal stress
    CVectorValueSet::TValueVec eff_stress(mesh_element.NrOfPoints());
// wjrx mantis 2571 ResultRegister().FaultEffectiveStress(Stage(), Linear(), false).ElementValues(eff_stress, mesh_element.Index());
    ResultRegister().FaultShearStress(Stage(), AnalysisType(), false).ElementValues(eff_stress, mesh_element.Index(), 0); // wjrx mantis 2571

    IValueDomainScalar::TValueVec eff_normal_stress(mesh_element.NrOfPoints());
    const IScalarResultComponent& normal_component =
      dynamic_cast<const IScalarResultComponent&>(
        *model.ResultTree().FaultEffectiveNormalStress().ResultComponent(Stage(), AnalysisType(), RegisterIndex()));
    normal_component.ElementValues(eff_normal_stress, mesh_element, CQuantity::SI_UNIT, 0); // added UNIT wjrx mantis 2571

    double friction = ResultRegister().MaterialParameterValue(mesh_element.Index(), IDT_VALUETYPE_FRICTION_ANGLE, Stage());
    double cohesion = ResultRegister().MaterialParameterValue(mesh_element.Index(), IDT_VALUETYPE_COHESION, Stage());
    
    for(size_t i = 0; i < values.size(); i++) {
      if(!eff_stress[i].Empty() && eff_normal_stress[i].Valid()) 
        values[i] = MF::FaultShearCapacity(eff_stress[i], eff_normal_stress[i].Value(), friction, cohesion, MF::DEG);
      else
      values[i] = geo::CValue();
    }
    break;
  }
  case PORE_PRESSURE:
  case PORE_PRESSURE_CHANGE:
  {
    assert(ResultRegister().ResultsAvailable(AnalysisType()));
    ResultRegister().FaultPorePressure(Stage(), AnalysisType(), pResult->ResultType() == PORE_PRESSURE_CHANGE).ElementValues(values, mesh_element.Index(), 0);
    break;
  }
  case PORE_PRESSURE_INPUT:
  case PORE_PRESSURE_CHANGE_INPUT:
  {
    // get it from input data
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    const CHorizonBase* pFault = model.Mesh().SlipHorizon(mesh_element);
    if(pFault)
    {
      values = pFault->Pressure(Stage()).Component().ScalarData().ValueElement(mesh_element);
      if(pResult->ResultType() == PORE_PRESSURE_CHANGE_INPUT)
      {
        IValueDomainScalar::TValueVec vcIniValues = pFault->Pressure(Stage().InitialStage()).Component().ScalarData().ValueElement(mesh_element);
        for(size_t i = 0; i < values.size(); ++i)
        {
          if(vcIniValues[i].Valid() && values[i].Valid())
            values[i].Value(values[i].Value() - vcIniValues[i].Value());
          else
            values[i] = geo::CValue();
        }
      }

      if(unit != CQuantity::SI_UNIT)
      {
        for(size_t i = 0; i < values.size(); i++)
          if(values[i].Valid()) values[i] *= FF_FACTOR_STRESS;
      }
    }
    break;
  }
  case BOUNDARY_PRESSURE_INPUT:
  {
    const IWellModel *model = dynamic_cast<const IWellModel *>(&Model());
    if (!model)
      return;

    const CDepletionStage& stage = Stage();

    const CDepletionStage& parentStage = model->ParentEquivalentDepletionStage(stage);

    //const CDepletionStage *initialStage = &stage;

    //while (!initialStage->IsMarkedAsInitial())
      //initialStage = &initialStage->Previous();

    //const CDepletionStage& parentInitialStage = model->ParentEquivalentDepletionStage(*initialStage);

    const CModelBase& parentModel = model->ParentModel();

    //CStressTensorValueSet initialStress = parentModel.ResultRegister().TotalStress(parentInitialStage, model->ParentLinearResults() ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    CStressTensorValueSet stress = parentModel.ResultRegister().TotalStress(parentStage, model->ParentLinearResults(AnalysisType()) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    const geo::IFace *bodyface = dynamic_cast<const geo::IFace *>(&pInterface->Front());

    geo::CVector vecPressureDirection; // direction of the bodyface load
    std::vector<double> vcPressureValues(bodyface->NrOfNodes()); // values of the bodyface load in each node

    for (int n = 0; n < bodyface->NrOfNodes(); ++n)
    {
      const geo::INode& node = bodyface->Node(n);

      CStressTensor stresstensor = stress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);

      geo::CVector vecPressure = -(stresstensor.Matrix() * bodyface->Normal().UnitVector());

      values[n] = vecPressure.Length();
    }
    break;
  }
  case BOUNDARY_DISPLACEMENT_INPUT:
  {
    const IWellModel *model = dynamic_cast<const IWellModel *>(&Model());
    if (!model)
      return;

    const CWellZoomInModel *wellModel = dynamic_cast<const CWellZoomInModel *>(&Model());

    const CDepletionStage& stage = Stage();

    const CModelBase& parentModel = model->ParentModel();
    const geo::IFace *bodyface = dynamic_cast<const geo::IFace *>(&pInterface->Front());
    //const CDepletionStage *initialStage = &stage;

    //while (!initialStage->IsMarkedAsInitial())
      //initialStage = &initialStage->Previous();

    const CDepletionStage& parentStage = model->ParentEquivalentDepletionStage(stage);

    CVectorValueSet displa = parentModel.ResultRegister().Displacement(parentStage,
      model->ParentLinearResults(AnalysisType()) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    if (wellModel)
    {
      for (int n = 0; n < bodyface->NrOfNodes(); ++n)
      {
        const geo::INode& node = bodyface->Node(n);

        int nBoundaryNode = wellModel->Mesh().MeshNodeToBoundaryNode(node.Index());

        // get the sample point
        geo::CPoint ptSample2 = wellModel->Mesh().SamplePosition(node);

        // get the displacement result value for that point
        geo::CVector vecDispla;

        // the elements at the sample position
        std::vector<int> vcElementIndex = parentModel.Mesh().Mesh().ElementsAt(ptSample2);
        for (size_t i2 = 0; i2 < vcElementIndex.size(); ++i2)
        {
          const geo::IElement& elm = parentModel.Mesh().Mesh().Element(vcElementIndex[i2]);
          vecDispla = GetDisplacementVectorWellZoomIn(displa, elm, ptSample2);
          if (!vecDispla.Empty())
            break;
        }

        assert(!vecDispla.Empty());
        if (!vecDispla.Empty())
        {
          values[n] = vecDispla.Length();
        }
      }
    }
    else
    {
      for (int n = 0; n < bodyface->NrOfNodes(); ++n)
      {
        const geo::INode& node = bodyface->Node(n);

        int nBoundaryNode = node.Index() + 1; // always the next

        const geo::CTranslationSupport& support = model->Mesh().Mesh().TranslationSupport(nBoundaryNode);

        // get the displacement result value for the node
        geo::CVector vecDispla;

        std::vector<int> vcElementIndex = parentModel.Mesh().Mesh().ElementsAt(node);
        for (size_t i2 = 0; i2 < vcElementIndex.size(); ++i2)
        {
          const geo::IElement& elm = parentModel.Mesh().Mesh().Element(vcElementIndex[i2]);
          vecDispla = GetDisplacementVectorCasing(displa, elm, node);
          if (!vecDispla.Empty())
            break;
        }

        assert(!vecDispla.Empty());
        if (!vecDispla.Empty())
        {
          values[n] = vecDispla.Length();
        }
      }
    }
    break;
  }
  case PLASTIC_ENERGY:
    PlasticEnergy(Stage(), values, mesh_element, unit);
    break;
  default:
    assert(false);
    break;
  };
}

IValueDomainScalar::TValueVec CResultTree::CFaultScalarResult::CFaultScalarResultComponent::ValueElement(const geo::IElement& elm, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const CNonMeshedSurface::CGeoSurface* pGeoSurface = dynamic_cast<const CNonMeshedSurface::CGeoSurface*>(elm.IndexingElementSet());
  if(pGeoSurface)
  {
    const CFaultScalarResult *pResult = dynamic_cast<const CFaultScalarResult*>(&Parent());
    assert(pResult);
    switch(pResult->ResultType())
    {
    case SHEAR_CAPACITY:
      {
        IValueDomainScalar::TValueVec vcValues(elm.NrOfPoints());

        CStressTensorValueSet::TValueVec vcElementValues(elm.NrOfPoints());
        ResultRegister().EffectiveStress(Stage(), AnalysisType(), false).ElementValue(elm, vcElementValues, cb);
        assert(dynamic_cast<const geo::IFace*>(&elm));
        const geo::IFace& face = static_cast<const geo::IFace&>(elm);

	      geo::CVector norm = face.Normal();
	      geo::CVector tang1 = geo::CVector(face.Point(0), face.Point(1)).UnitVector();
	      geo::CVector tang2 = norm.CrossProduct(tang1).UnitVector();

	      geo::CMatrix rotation(3, 3);
	      rotation.Value(0, 0, norm.X()); rotation.Value(0, 1, tang1.X()); rotation.Value(0, 2, tang2.X());
	      rotation.Value(1, 0, norm.Y()); rotation.Value(1, 1, tang1.Y()); rotation.Value(1, 2, tang2.Y());
	      rotation.Value(2, 0, norm.Z()); rotation.Value(2, 1, tang1.Z()); rotation.Value(2, 2, tang2.Z());

        std::vector<double> vcCohesion = pGeoSurface->Parent().ParametersNode().Cohesion(face);
        std::vector<double> vcFriction = pGeoSurface->Parent().ParametersNode().FrictionAngle(face);

        for(int i = 0; i < face.NrOfPoints(); ++i)
        {
          CTensor tensor = vcElementValues[i].RotateTensor(rotation);
          double dNormal = tensor.XX();
          geo::CVector vecShear(tensor.XY(), tensor.XZ(), 0);

          if(!vecShear.Empty())
            vcValues[i] = MF::FaultShearCapacity(vecShear, dNormal, vcFriction[i], vcCohesion[i], MF::DEG);
        }

        return vcValues;
      }
      break;
    case PORE_PRESSURE:
    case PORE_PRESSURE_INPUT:
      {
        IValueDomainScalar::TValueVec vcValues;
        const CModelBase& model = static_cast<const CModelBase&>(Model());

        if(pGeoSurface->Parent().Pressure().ManualValues())
        {
          vcValues = pGeoSurface->Parent().Pressure().Component().ScalarData().ValueElement(elm, unit, cb);
        }
        else
        {
          vcValues = model.ResultTree().PorePressure().ResultComponent(Stage(), AnalysisType())->ScalarData().ValueElement(elm, unit, cb);
        }

        return vcValues;
      }
      break;
    case PORE_PRESSURE_CHANGE:
    case PORE_PRESSURE_CHANGE_INPUT:
      {
        assert(!Stage().Initial());
        if(pGeoSurface->Parent().Pressure().ManualValues())
        {
          // zero change...
          IValueDomainScalar::TValueVec vcValues(elm.NrOfPoints(), 0);
          return vcValues;
        }
        else
        {
          const CModelBase& model = static_cast<const CModelBase&>(Model());
          IValueDomainScalar::TValueVec vcValues;
          IValueDomainScalar::TValueVec vcValuesIni;
          vcValues = model.ResultTree().PorePressure().ResultComponent(Stage(), AnalysisType())->ScalarData().ValueElement(elm, unit, cb);
          vcValuesIni = model.ResultTree().PorePressure().ResultComponent(model.DepletionStageEntry().MarkedAsInitialStage(), AnalysisType())->ScalarData().ValueElement(elm, unit, cb);
          assert(vcValues.size() == vcValuesIni.size());
          for(size_t i = 0; i < vcValues.size(); ++i)
          {
            if(vcValues[i].Valid() && vcValuesIni[i].Valid())
              vcValues[i].Value(vcValues[i].Value() - vcValuesIni[i].Value());
            else
              vcValues[i] = geo::CValue();
          }

          return vcValues;
        }
      }
      break;
    case BOUNDARY_PRESSURE_INPUT:
      break;
    case BOUNDARY_DISPLACEMENT_INPUT:
      break;
    case DILATATION:
    case PLASTIC_ENERGY:
      break;
    }
  }

  return IScalarResultComponent::ValueElement(elm, unit, cb);
}


//////////////////////////////////////////////////////////////////////
// Implementation of CFaultScalarResult::CFaultScalarResultComponent
//////////////////////////////////////////////////////////////////////
void CResultTree::CFaultScalarResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
  {
    switch(ResultType())
    {
    case DILATATION:
    case SHEAR_CAPACITY:
    case PORE_PRESSURE:
    case PORE_PRESSURE_INPUT:
    case BOUNDARY_PRESSURE_INPUT:
    case BOUNDARY_DISPLACEMENT_INPUT:
      new CFaultScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case PORE_PRESSURE_CHANGE:
    case PORE_PRESSURE_CHANGE_INPUT:
    case PLASTIC_ENERGY:
      if(!stage.Initial())
        new CFaultScalarResultComponent(*this, stage, antype, nRegister);
      break;
    default:
      assert(false);
    }
  }
}

CResultTree::CFaultScalarResult::CFaultScalarResult(unsigned int uName, unsigned int uLabelId, CResultGroup& group, TResultType result_type)
: IResult(uName, group), m_result_type(result_type), m_uLabel(uLabelId)
{
  BuildComponents();
  SetIndex();
}

CResultTree::CFaultScalarResult::CFaultScalarResult(const QString& sName, unsigned int uLabelId, CResultGroup& group, TResultType result_type)
: IResult(sName, group), m_result_type(result_type), m_uLabel(uLabelId)
{
  BuildComponents();
  SetIndex();
}

void CResultTree::CFaultScalarResult::SetIndex()
{
  switch(m_result_type)
  {
  case DILATATION:
    Index(RC_FAULT_DILATATION);
    break;
//  case TOTAL_NORMAL_STRESS:
//    Index(RC_FAULT_TOTAL_NORMAL_STRESS);
//    break;
//  case EFFECTIVE_NORMAL_STRESS:
//    Index(RC_FAULT_EFF_NORMAL_STRESS);
//    break;
  case SHEAR_CAPACITY:
    Index(RC_FAULT_SHEAR_CAPACITY);
    break;
  case PORE_PRESSURE:
    Index(RC_FAULT_PRESSURE);
    break;
  case PORE_PRESSURE_CHANGE:
    Index(RC_FAULT_PRESSURE_CHANGE);
    break;
  case PLASTIC_ENERGY:
    Index(RC_FAULT_PLASTIC_ENERGY);
    break;
  case PORE_PRESSURE_INPUT:
    Index(RC_FAULT_PRESSURE_INPUT);
    break;
  case PORE_PRESSURE_CHANGE_INPUT:
    Index(RC_FAULT_PRESSURE_CHANGE_INPUT);
    break;
  case BOUNDARY_PRESSURE_INPUT:
    Index(RC_BOUNDARY_PRESSURE);
    break;
  case BOUNDARY_DISPLACEMENT_INPUT:
    Index(RC_BOUNDARY_DISPLACEMENT);
    break;
  default:
    assert(false);
    break;
  };
}

CResultTree::CFaultScalarResult::TResultType CResultTree::CFaultScalarResult::ResultType() const
{
  return m_result_type;
}

QString CResultTree::CFaultScalarResult::ExportLabel(int /*nComponent*/) const
{
  return getStringTableEntry(m_uLabel);
}

bool CResultTree::CFaultScalarResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  switch(m_result_type)
  {
  case SHEAR_CAPACITY: // when a fault is selected that is set to perfect slip, the shear capacity
    {         // is not allowed to be selected
      const CHorizonBase *pHorizon = dynamic_cast<const CHorizonBase*>(&node);
      if(pHorizon)
      {
        if(pHorizon->SlipType() == CHorizonBase::SLIP || pHorizon->SlipType() == CHorizonBase::FRACTURE)
          return false;
      }
      else
      {
        const CNonMeshedSurface* pNMSurface = dynamic_cast<const CNonMeshedSurface*>(&node);
        if(pNMSurface)
          return true;
      }

      return CanMapOnFaultElements(node, nRegister);
    }
    break;
  case PORE_PRESSURE:
  case PORE_PRESSURE_CHANGE:
  case PORE_PRESSURE_INPUT:
  case PORE_PRESSURE_CHANGE_INPUT:
    {
      const CNonMeshedSurface* pNMSurface = dynamic_cast<const CNonMeshedSurface*>(&node);
      if(pNMSurface)
        return true;
    }
    // fall through...
  default:
    return CanMapOnFaultElements(node, nRegister);
  case BOUNDARY_PRESSURE_INPUT:
  case BOUNDARY_DISPLACEMENT_INPUT:
    return CanMapOnBoundaryElements(node, nRegister);
  }
}

bool CResultTree::CFaultScalarResult::isFaultResult() const
{
  return true;
}

bool CResultTree::CFaultScalarResult::OnDefined(const IResultComponent& component) const
{
  if((m_result_type == PORE_PRESSURE_INPUT) || (m_result_type == PORE_PRESSURE_CHANGE_INPUT))
  {
    return true;
  }

  if (m_result_type == BOUNDARY_PRESSURE_INPUT || m_result_type == BOUNDARY_DISPLACEMENT_INPUT)
  {
    assert(dynamic_cast<const IWellModel*>(&Model()));
    const IWellModel& model = static_cast<const IWellModel&>(Model());

    bool resultsAvailable = model.ParentLinearResults(component.AnalysisType()) ?
      model.ParentModel().ResultRegister().Linear() :
      model.ParentModel().ResultRegister().NonLinear();

    if (!(model.Mesh().IsMesh() && resultsAvailable))
      return false;

      return m_result_type != BOUNDARY_DISPLACEMENT_INPUT || !component.Stage().IsMarkedAsInitial();
  }

  return IResult::OnDefined(component);
}

bool CResultTree::CFaultScalarResult::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase* /*pValueComponent*/)
{
  return false;
}


//////////////////////////////////////////////////////////////////////
// The implementation of CFaultDisplacementResult
//////////////////////////////////////////////////////////////////////
CResultTree::CFaultDisplacementResult::CFaultDisplacementResult(unsigned int uName, CResultGroup& group)
: CVectorResult(uName, &CResultRegister::FaultDisplacement, group, false)
{
  BuildComponents();
  Index(RC_FAULT_DISPLACEMENT);
}

CResultTree::CFaultDisplacementResult::CFaultDisplacementResult(const QString& sName, CResultGroup& group)
: CVectorResult(sName, &CResultRegister::FaultDisplacement, group, false)
{
  BuildComponents();
  Index(RC_FAULT_DISPLACEMENT);
}

bool CResultTree::CFaultDisplacementResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= VectorExportLabel().toStdString();

  return validateName(resultInfo, name, label, VectorResultComponentList(),
    "_L|_M|_C|_N");
}

bool CResultTree::CFaultDisplacementResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
        (antype == CAnalysisType::AT_LINEAR  ||
         antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

double CResultTree::CFaultDisplacementResult::ConvertToField(const double& dValue) const
{
  return dValue * FF_FACTOR_LENGTH;
}

QString CResultTree::CFaultDisplacementResult::ExportLabel(int nComponent) const
{
  return VectorExportLabel() + IVectorResult::ExportLabel(nComponent);
}

QString CResultTree::CFaultDisplacementResult::VectorExportLabel() const
{
  return getStringTableEntry(IDS_ET_FAULT_DISPLACEMENT);
}

bool CResultTree::CFaultDisplacementResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnFaultElements(node, nRegister);
}

bool CResultTree::CFaultDisplacementResult::isFaultResult() const
{
  return true;
}

QString CResultTree::CFaultDisplacementResult::UnitName(UNIT unit) const
{
  QString sUnit;
  if(unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_LENGTH);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
  return sUnit;
}

bool CResultTree::CFaultDisplacementResult::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase *pValueComponent)
{
  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &static_cast<CModelBase&>(Model()).ResultRegister();

  int c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRAIN_XX);
  int c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_STRAIN_XX);

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 3))
    return false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 3))
    return false;

  return true;
}


//////////////////////////////////////////////////////////////////////
// Implementation of CFaultPlasticSlipResult
//////////////////////////////////////////////////////////////////////
CResultTree::CFaultPlasticSlipResult::CFaultPlasticSlipResult(unsigned int uName, CResultGroup& group)
: IVectorResult(uName, group)
{
  BuildComponents();
  Index(RC_FAULT_PLASTIC_SLIP);
}

CResultTree::CFaultPlasticSlipResult::CFaultPlasticSlipResult(const QString& sName, CResultGroup& group)
: IVectorResult(sName, group)
{
  BuildComponents();
  Index(RC_FAULT_PLASTIC_SLIP);
}

const geo::IVector& CResultTree::CFaultPlasticSlipResult::VectorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback* /*cb*/) const
{
  if(m_cache.pPoint         == &point                                  &&
     m_cache.pStage         == &component.Stage()                      &&
     m_cache.antype         == component.AnalysisType().AnalysisType() &&
     m_cache.nRegisterIndex == component.RegisterIndex())
  {
    assert(m_cache.m_vcValues.size() >= 1);
    return m_cache.m_vcValues[0];
  }

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  std::set<int> stElements = model.Mesh().Mesh().Candidates(point);
  std::set<int>::iterator it;
  const geo::IInterfaceElement* pInterface = 0;
  for(it = stElements.begin(); it != stElements.end(); ++it)
  {
    const geo::IElement& elm = model.Mesh().Mesh().Element(*it);
    pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
    if(pInterface)
      break;
  }

  if(!pInterface)
    return vecEmpty;

  const CHorizonBase* pHor = model.Mesh().SlipHorizon(*pInterface);
  if(!pHor)
    return vecEmpty;

  double dDStiff = pHor->FaultParameters(component.Stage()).DStiffShear();

  const IResultComponent* pComp;

  pComp = model.ResultTree().FaultDisplacement().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultDisplacement = model.ResultTree().FaultDisplacement().VectorPoint(point, *pComp);
  if(vecFaultDisplacement.Empty())
    return vecEmpty;

  geo::CValue valFaultDilatation = model.ResultTree().FaultDilatation().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex())->ValuePoint(point);
  if(!valFaultDilatation.Valid())
    return vecEmpty;

  pComp = model.ResultTree().FaultShearStress().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultShearStress = model.ResultTree().FaultShearStress().VectorPoint(point, *pComp);
  if(vecFaultShearStress.Empty())
    return vecEmpty;

  pComp = model.ResultTree().FaultShearStress().ResultComponent(model.DepletionStageEntry().MarkedAsInitialStage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultShearStressIni = model.ResultTree().FaultShearStress().VectorPoint(point, *pComp);
  if(vecFaultShearStressIni.Empty())
    return vecEmpty;

  geo::CVector vecPlasticSlip = Calculate(
    *pInterface,
    point,
    vecFaultDisplacement,
    valFaultDilatation.Value(),
    vecFaultShearStress - vecFaultShearStressIni,
    dDStiff);

  m_cache.pElement       = 0;
  m_cache.pPoint         = &point;
  m_cache.pStage         = &component.Stage();
  m_cache.antype         = component.AnalysisType().AnalysisType();
  m_cache.nRegisterIndex = component.RegisterIndex();

  if(m_cache.m_vcValues.size() < 1)
    m_cache.m_vcValues.resize(1);

  m_cache.m_vcValues[0] = vecPlasticSlip;
  return m_cache.m_vcValues[0];
}

const geo::IVector& CResultTree::CFaultPlasticSlipResult::VectorElement(const geo::IElement& element, 
                                                                        int nNodeIndex, 
                                                                        TMapType map_type,
                                                                        const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  if(m_cache.pElement       == &element                                &&
     m_cache.pStage         == &component.Stage()                      &&
     m_cache.antype         == component.AnalysisType().AnalysisType() &&
     m_cache.nRegisterIndex == component.RegisterIndex()               &&
     m_cache.stNodeIndex.find(nNodeIndex) != m_cache.stNodeIndex.end())
  {
    assert(m_cache.m_vcValues.size() >= element.NrOfNodes());
    assert(nNodeIndex < element.NrOfNodes());
    return m_cache.m_vcValues[nNodeIndex];
  }

  const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&element);
  if(!pInterface) // should be an interface element
    return vecEmpty;

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  double dDStiff;
  const CHorizonBase* pHor = model.Mesh().SlipHorizon(*pInterface);
  if(pHor)
    dDStiff = pHor->FaultParameters(component.Stage()).DStiffShear();
  else
  {
    const CWellCasingModel* pWellCasingModel = dynamic_cast<const CWellCasingModel*>(&Model());
    if(pWellCasingModel && pWellCasingModel->Mesh().IsCementInterfaceElement(*pInterface))
    {
      const CInterfaceMaterial* pIfaceMat = pWellCasingModel->CasingNode().CementInterface().Material(component.Stage()).LibraryMaterial();
      if(pIfaceMat)
        dDStiff = pIfaceMat->ShearStiffness();
      else
        return vecEmpty;
    }
    else
    {
      return vecEmpty;
    }
  }

  const IResultComponent* pComp;

  pComp = model.ResultTree().FaultDisplacement().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultDisplacement = model.ResultTree().FaultDisplacement().VectorElement(element, nNodeIndex, map_type, *pComp, cb);
  if(vecFaultDisplacement.Empty())
    return vecEmpty;

  geo::CValue valFaultDilatation = model.ResultTree().FaultDilatation().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex())->ValueElement(element, CDoubleQuantity::SI_UNIT, cb)[nNodeIndex];
  if(!valFaultDilatation.Valid())
    return vecEmpty;

  pComp = model.ResultTree().FaultShearStress().ResultComponent(component.Stage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultShearStress = model.ResultTree().FaultShearStress().VectorElement(element, nNodeIndex, map_type, *pComp, cb);
  if(vecFaultShearStress.Empty())
    return vecEmpty;

  pComp = model.ResultTree().FaultShearStress().ResultComponent(model.DepletionStageEntry().MarkedAsInitialStage(), component.AnalysisType(), component.RegisterIndex(), component.ComponentIndex());
  if(!pComp)
    return vecEmpty;
  geo::CVector vecFaultShearStressIni = model.ResultTree().FaultShearStress().VectorElement(element, nNodeIndex, map_type, *pComp, cb);
  if(vecFaultShearStressIni.Empty())
    return vecEmpty;

  geo::CVector vecPlasticSlip = Calculate(
    *pInterface,
    element.Node(nNodeIndex),
    vecFaultDisplacement,
    valFaultDilatation.Value(),
    vecFaultShearStress - vecFaultShearStressIni,
    dDStiff);

  if(m_cache.pElement != &element)
    m_cache.stNodeIndex.clear();

  m_cache.pPoint         = 0;
  m_cache.pElement       = &element;
  m_cache.pStage         = &component.Stage();
  m_cache.antype         = component.AnalysisType().AnalysisType();
  m_cache.nRegisterIndex = component.RegisterIndex();
  m_cache.stNodeIndex.insert(nNodeIndex);

  if(m_cache.m_vcValues.size() < element.NrOfNodes())
    m_cache.m_vcValues.resize(element.NrOfNodes());

  m_cache.m_vcValues[nNodeIndex] = vecPlasticSlip;
  return m_cache.m_vcValues[nNodeIndex];
}

bool CResultTree::CFaultPlasticSlipResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  // plastic slip result is not available for fractures
  const CHorizonBase *pHorizon = dynamic_cast<const CHorizonBase*>(&node);
  if(pHorizon && pHorizon->SlipType() == CHorizonBase::FRACTURE)
    return false;

  return CanMapOnFaultElements(node, nRegister);
}

bool CResultTree::CFaultPlasticSlipResult::isFaultResult() const
{
  return true;
}

bool CResultTree::CFaultPlasticSlipResult::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase* /*pValueComponent*/)
{
  return false;
}

QString CResultTree::CFaultPlasticSlipResult::ExportLabel(int nComponent) const
{
  return VectorExportLabel() + IVectorResult::ExportLabel(nComponent);
}

QString CResultTree::CFaultPlasticSlipResult::VectorExportLabel() const
{
  return getStringTableEntry(IDS_ET_FAULT_PLASTIC_SLIP);
}

QString CResultTree::CFaultPlasticSlipResult::UnitName(UNIT unit) const
{
  QString sUnit;
  if(unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_LENGTH);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
  return sUnit;
}

double CResultTree::CFaultPlasticSlipResult::ConvertToField(const double& dValue) const
{
  return dValue * FF_FACTOR_LENGTH;
}

bool CResultTree::CFaultPlasticSlipResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= VectorExportLabel().toStdString();

  return validateName(resultInfo, name, label, VectorResultComponentList(),
    "_L|_M|_C|_N");
}


bool CResultTree::CFaultPlasticSlipResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
        (antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

geo::CVector CResultTree::CFaultPlasticSlipResult::Calculate(const geo::IInterfaceElement& iface,
                                                             const geo::IPoint& point,
                                                             const geo::IVector& vecDeformation,
                                                             double dDilatation,
                                                             const geo::IVector& vecShearStress,
                                                             double dElasticShearStiffness) const
{
  geo::CVector vecNormal; // the normal of the interface element
  const geo::CInterfaceElement* p3DInterface = dynamic_cast<const geo::CInterfaceElement*>(&iface);
  assert(p3DInterface);
  const geo::IFace& f = *p3DInterface->FrontFace();
  vecNormal = f.NormalInPoint(point);

  geo::CVector vecShear(vecShearStress.InPlaneComponent(vecNormal));

  // make sure elastic values are subtracted
  if(vecNormal.DotProduct(vecDeformation) > 0)
    vecNormal.Flip();

  if(vecShear.DotProduct(vecDeformation) > 0)
    vecShear.Flip();

  return vecDeformation + vecNormal * fabs(dDilatation) + vecShear / dElasticShearStiffness;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CFaultStressResult::CNormalStress
//////////////////////////////////////////////////////////////////////

CResultTree::CFaultNormalStress::CNormalStressComponent::CNormalStressComponent(CFaultNormalStress& parent,    // The tensor composite composite parent
                                        const CDepletionStage& stage,  // Depletion stage of the component
                                        const CAnalysisType& antype,
                                        int nRegister)
: IScalarResultComponent("", parent, stage, antype, nRegister, 0)
{
}

unsigned int CResultTree::CFaultNormalStress::CNormalStressComponent::IconId() const
{
  return 0;
}

unsigned int CResultTree::CFaultNormalStress::CNormalStressComponent::TypeId() const
{
  return 0;
}

QString CResultTree::CFaultNormalStress::CNormalStressComponent::UnitName(const UNIT unit) const
{
  if(unit == CQuantity::SI_UNIT)
    return QString("MPa");
  return QString("psi");
}

void CResultTree::CFaultNormalStress::CNormalStressComponent::ElementValues(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, UNIT unit, geo::IParallelInitializationCallback* /*cb*/) const
{
  const geo::IInterfaceElement* p = dynamic_cast<const geo::IInterfaceElement *>(&mesh_element);
  if ( p == NULL ) return;// wjrx mantis 2570

  const geo::IInterfaceElement& inf_elm = dynamic_cast<const geo::IInterfaceElement&>(mesh_element);
  const CFaultNormalStress& parent = (const CFaultNormalStress&)Parent();

  CVectorValueSet::TValueVec vcValue(values.size());
  ResultRegister().FaultEffectiveStress(Stage(), AnalysisType(), parent.m_bChange).ElementValues(vcValue, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);
  std::vector<geo::CValue> vcPressure(values.size(), geo::CValue(0));
  if(parent.m_bTotal) {
    ResultRegister().FaultPorePressure(Stage(), AnalysisType(), parent.m_bChange).ElementValues(vcPressure, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);
  }

  for(size_t i = 0; i < vcValue.size(); i++) {
    if(!vcValue[i].Empty() && vcPressure[i].Valid()) {
      values[i] = MF::FaultNormalStress(inf_elm, vcValue[i]) + vcPressure[i];//face.Normal().DotProduct(vCurrent);  
      if(unit != CQuantity::SI_UNIT) values[i] *= FF_FACTOR_STRESS;
    }
  }
}

//////////////////////////////////////////////////////////////////////
// Implementation of CFaultStressResult
//////////////////////////////////////////////////////////////////////
/*
CResultTree::CFaultStressResult::CFaultStressResult(unsigned int uName,
                          unsigned int uShearFileIndex,
                            unsigned int uShearExportName,
                          unsigned int uNormalFileIndex,
                          unsigned int uNormalExportName,
                          TValueSetFunction function,
                          CResultGroup& group, 
                          bool bChange)
: CResultGroup(uName, group)
{
  m_pNormalStress = new CNormalStress(uNormalFileIndex, uShearExportName, *this, bChange);
  m_pShearStress = new CShearStress(uShearFileIndex, uShearExportName, function, *this, bChange);
}

const CResultTree::CFaultStressResult::CNormalStress& CResultTree::CFaultStressResult::NormalStress() const
{
  return *m_pNormalStress;
}

const CResultTree::CFaultStressResult::CShearStress& CResultTree::CFaultStressResult::ShearStress() const
{
  return *m_pShearStress;
}
*/
//////////////////////////////////////////////////////////////////////
// Implementation of CFaultStressResult::CNormalStress::CNormalStressComponent
//////////////////////////////////////////////////////////////////////
bool CResultTree::CFaultNormalStress::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= ExportLabel(0).toStdString();

  return validateName(resultInfo, name, label, "", "_L|_M|_C|_N");
}


bool CResultTree::CFaultNormalStress::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return (!stage.Initial() || !m_bChange) &&
          (antype == CAnalysisType::AT_LINEAR  ||
           antype == CAnalysisType::AT_MIXTURE ||
           antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
           (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

void CResultTree::CFaultNormalStress::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
    new CNormalStressComponent(*this, stage, antype, nRegister);
}

CResultTree::CFaultNormalStress::CFaultNormalStress(unsigned int uFileIndex, unsigned int uName, unsigned int uLabelId, CResultGroup& group, bool bTotal, bool bChange)
: IResult(uName, group), m_bChange(bChange), m_uLabel(uLabelId), m_bTotal(bTotal)
{
  BuildComponents();
  Index(uFileIndex);
}

QString CResultTree::CFaultNormalStress::ExportLabel(int nComponent) const
{
  assert(nComponent == 0);
  QString sRet;
  sRet = getStringTableEntry(m_uLabel);
  if(m_bChange)
    sRet = "Dlt_" + sRet;
  return sRet;
}

bool CResultTree::CFaultNormalStress::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnFaultElements(node, nRegister);
}

bool CResultTree::CFaultNormalStress::isFaultResult() const
{
  return true;
}

bool CResultTree::CFaultNormalStress::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase* /*pValueComponent*/)
{
  return false;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CScalarResult
//////////////////////////////////////////////////////////////////////

CResultTree::CScalarResult::CScalarResultComponent::CScalarResultComponent(CScalarResult& parent,      // The tensor composite composite parent
                                       const CDepletionStage& stage,  // Depletion stage of the component
                                       const CAnalysisType& antype,
                                       int nRegister)
: IScalarResultComponent("", parent, stage, antype, nRegister, 0)
{
  m_mpValues.clear();
}

unsigned int CResultTree::CScalarResult::CScalarResultComponent::IconId() const
{
  return 0;
}

unsigned int CResultTree::CScalarResult::CScalarResultComponent::TypeId() const
{
  return 0;
}

bool CResultTree::CScalarResult::CScalarResultComponent::Defined() const
{
  const CScalarResult *pResult = dynamic_cast<const CScalarResult*>(&Parent());
  assert(pResult);
  if((pResult->ResultType() == SHEAR_CAPACITY) &&
     (&ResultRegister().DepletionStageEntry() != Model().GraphEntry(MD_BASE_DEPLETION_STAGE)))
    return false;

  return IResultComponent::Defined();
}

QString CResultTree::CScalarResult::CScalarResultComponent::UnitName(const UNIT unit) const
{
  QString sUnit;
  const CScalarResult *pResult = dynamic_cast<const CScalarResult*>(&Parent());
  assert(pResult);
  switch(pResult->ResultType())
  {
  case PORE_PRESSURE:
  case PORE_PRESSURE_CHANGE:
  case PORE_PRESSURE_INPUT:
  case PORE_PRESSURE_CHANGE_INPUT:
  case FRACTURE_MATRIX_PRESSURE:
  case FRACTURE_MATRIX_PRESSURE_CHANGE:
  case CONSOL:
  case CONSOL_CHANGE:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
    break;
  case TEMPERATURE:
  case TEMPERATURE_CHANGE:
  case TEMPERATURE_INPUT:
  case TEMPERATURE_CHANGE_INPUT:
    sUnit = getStringTableEntry(IDS_UNIT_TEMPERATURE);
    break;
  case SHEAR_CAPACITY:
  case COMPACTION_CAPACITY:
  case GAMMA_VERTICAL:
  case GAMMA_MAX:
  case GAMMA_MED:
  case GAMMA_MIN:
  case GAMMA_VOLUME:
  case VOLUME_AVERAGE:
  case POROSITY:
  case POROSITY_CHANGE:
  case SHTOTAZIMUTH:// wjrx mantis 2870
  case SHTOT_SvTOT: // wjrx mantis 2870
  case ShTOT_SvTOT: // wjrx mantis 2870
  case PERMEABILITY_MULTIPLIER_HI:
  case PERMEABILITY_MULTIPLIER_ME:
  case PERMEABILITY_MULTIPLIER_LO:
  case VOLUMETRIC_STRAIN_INPUT:
  case VOLUMETRIC_STRAIN_CHANGE_INPUT:
  case NORMAL_STRAIN_INPUT:
  case LATERAL_STRAIN_INPUT:
    sUnit = getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
    break;
  case THICKNESS:
  case THICKNESS_CHANGE:
  case APERTURE_HI:
  case APERTURE_ME:
  case APERTURE_LO:
  case APERTURE_HI_CHANGE:
  case APERTURE_ME_CHANGE:
  case APERTURE_LO_CHANGE:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_LENGTH);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
    break;
  case PLASTIC_ENERGY:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_ENERGY);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_ENERGY);
    break;
  case PORE_PRESSURE_GRADIENT:
  case PORE_PRESSURE_GRADIENT_INPUT:
    if(unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_STRESSGRADIENT);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESSGRADIENT);
    break;
  case TEMPERATURE_GRADIENT:
  case TEMPERATURE_GRADIENT_INPUT:
    if (unit == CQuantity::SI_UNIT)
      sUnit = getStringTableEntry(IDS_UNIT_SI_TEMPERATUREGRADIENT);
    else
      sUnit = getStringTableEntry(IDS_UNIT_FIELD_TEMPERATUREGRADIENT);
    break;
  default:
  assert(false);
  break;
  };

        return sUnit;
}

void CResultTree::CScalarResult::CScalarResultComponent::PorePressureUndrained(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage, const CFormationBase& formation) const
{
  assert(formation.Material(stage).Valid());
  assert(formation.Material(stage).MaterialModel() == MM_UNDRAINED);
  assert(!stage.Initial());

  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());

  const CDepletionStage& inistage = stage.InitialStage();

  // get initial pressures
  IValueDomainScalar::TValueVec vcIniPressures(mesh_element.NrOfNodes());
  PorePressure(vcIniPressures, mesh_element, inistage);

  // get initial average stresses
  IValueDomainScalar::TValueVec vcIniStresses = model.ResultTree().TotalStress().InvariantComposite()->ResultComponent(inistage, AnalysisType(), RegisterIndex(), CResultTree::CTotalStressResult::CInvariantComposite::INV_I1)->ValueElement(mesh_element);

  // get stage average stresses
  IValueDomainScalar::TValueVec vcStresses = model.ResultTree().TotalStress().InvariantComposite()->ResultComponent(stage, AnalysisType(), RegisterIndex(), CResultTree::CTotalStressResult::CInvariantComposite::INV_I1)->ValueElement(mesh_element);

  // get initial temperatures
  IValueDomainScalar::TValueVec vcIniTemper(mesh_element.NrOfNodes());
  Temperature(vcIniTemper, mesh_element, inistage);

  // get stage temperatures
  IValueDomainScalar::TValueVec vcTemper(mesh_element.NrOfNodes());
  Temperature(vcTemper, mesh_element, stage);

  // material for this element
  const CFFMaterial& mat = formation.Material(stage).Material(mesh_element);

  // bulk modulus
  assert(mat.IsParameter(IDT_VALUETYPE_BULKSTIFFNESS));
  double Kb = mat.ParameterValue(IDT_VALUETYPE_BULKSTIFFNESS);

  // porosity
  assert(mat.IsParameter(IDT_VALUETYPE_POROSITY));
  double phi = mat.ParameterValue(IDT_VALUETYPE_POROSITY);

  // fluid thermal expansion coefficient
  assert(mat.IsParameter(IDT_VALUETYPE_FLUIDX));
  double beta_f = mat.ParameterValue(IDT_VALUETYPE_FLUIDX);

  // volumetric thermal expansion coefficient
  assert(mat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION));
  double beta_b = mat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION);

  // fluid bulk modulus
  assert(mat.IsParameter(IDT_VALUETYPE_FLUID_BULK_MOD));
  double Kf = mat.ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD);

  // grain stiffness
  assert(mat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS));
  double Kg = mat.ParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS);

  int i;
  for(i = 0; i < mesh_element.NrOfNodes(); ++i)
  {
    if(vcIniPressures[i].Valid() && vcIniStresses[i].Valid() && vcStresses[i].Valid() && vcIniTemper[i].Valid() && vcTemper[i].Valid())
    {
      double P0 = vcIniPressures[i].Value();
      double sigma = vcStresses[i].Value();
      double sigma0 = vcIniStresses[i].Value();
      double T = vcTemper[i].Value();
      double T0 = vcIniTemper[i].Value();

      values[i] = P0 + ((1 / Kb - 1 / Kg) * (sigma - sigma0) + phi * (beta_f - beta_b) * (T - T0)) / (1 / Kb - (phi + 1) / Kg + phi / Kf);
    }
    else
    {
      values[i] = geo::CValue();
    }
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::PorePressure(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  assert(ResultRegister().ResultsAvailable(AnalysisType()) && AnalysisType() != CAnalysisType::AT_HEAT);

  if(!stage.Initial())
  {
    const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
    if(pFormation)
    {
      const CMaterialServer& mat_server = pFormation->Material(stage);
      if(mat_server.Valid() && mat_server.MaterialModel() == MM_UNDRAINED)
      {
        PorePressureUndrained(values, mesh_element, stage, *pFormation);
        return;
      }
    }
  }

  ResultRegister().PorePressure(stage, AnalysisType(), false).ElementValues(values, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);
}

void CResultTree::CScalarResult::CScalarResultComponent::PorePressureInput(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());

  // Get it from the input data
  if(!dynamic_cast<const geo::IInterfaceElement*>(&mesh_element))
  {
    // formation pressure
    const CFormationBase *pFormation = model.Mesh().Formation(mesh_element);
    if(pFormation)
      values = pFormation->Pressure(stage).Component().ScalarData().ValueElement(mesh_element);
  }
  assert(values.size() == mesh_element.NrOfNodes());
}

void CResultTree::CScalarResult::CScalarResultComponent::FractureMatrixPressure(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  const CScalarResult *pResult = dynamic_cast<const CScalarResult*>(&Parent());

  assert(pResult);

  if(!dynamic_cast<const geo::IInterfaceElement*>(&mesh_element))
  {
    const CFormationBase *pFormation = model.Mesh().Formation(mesh_element);

    if(pFormation)
      values = pFormation->FractureMatrixPressure(stage).Component().ScalarData().ValueElement(mesh_element);
  }

  assert(values.size() == mesh_element.NrOfNodes());
}

void CResultTree::CScalarResult::CScalarResultComponent::Temperature(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  assert(ResultRegister().ResultsAvailable(AnalysisType()));
  ResultRegister().Temperature(stage, AnalysisType(), false).ElementValues(values, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);
}

void CResultTree::CScalarResult::CScalarResultComponent::TemperatureInput(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  // get from input data
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(!dynamic_cast<const geo::IInterfaceElement*>(&mesh_element)) // not for faults
  {
    const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
    if(pFormation)
    {
      if(AnalysisType() == CAnalysisType::AT_HEAT)
        values = pFormation->UserTemperature(stage).Component().ScalarData().ValueElement(mesh_element);
      else
        values = pFormation->EffectiveTemperature(stage).Component().ScalarData().ValueElement(mesh_element);
    }
  }
  assert(values.size() == mesh_element.NrOfNodes());
}

void CResultTree::CScalarResult::CScalarResultComponent::Consolidation(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
  if(pFormation && pFormation->Material(stage).LibraryMaterial()->IsParameter(IDT_VALUETYPE_PRECONSOLIDATION))
    ResultRegister().Consolidation(stage, AnalysisType(), false).ElementValues(values, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);
}

void CResultTree::CScalarResult::CScalarResultComponent::Porosity(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, const CDepletionStage& stage, geo::IParallelInitializationCallback* cb) const
{
  // get the initial porosity from the material
  const CModelBase& model = static_cast<const CModelBase&>(Model());

  int iValueTypeID = 0;
  const CFFMaterial* pMat = 0;

  const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
  if(pFormation)
  {
    pMat = &pFormation->Material(stage).Material(mesh_element);
  }
  else
  {
    const CWellCasingModel* pWellCasingModel = dynamic_cast<const CWellCasingModel*>(&model);
    if(pWellCasingModel && pWellCasingModel->Mesh().IsCementElement(mesh_element))
    {
      pMat = &pWellCasingModel->CasingCement().Material(stage).Material(mesh_element);
    }
  }

  if(pMat)
  {
    if(pMat->IsParameter(IDT_VALUETYPE_POROSITY))
      iValueTypeID = IDT_VALUETYPE_POROSITY;
    else if(pMat->IsParameter(IDT_VALUETYPE_INIPORO))
      iValueTypeID = IDT_VALUETYPE_INIPORO;
  }

  if(iValueTypeID == 0)
  {
    // no initial porosity, result is invalid
    return;
  }

  double dIniPoro = pMat->ParameterValue(iValueTypeID);

  if(&stage == &stage.Entry().InitialStage())
  {
    // return input values
    for(size_t i = 0; i < values.size(); ++i)
      values[i] = dIniPoro;
  }

  // get the total volume strain result values
  const IResultComponent* pCompInvVolStrain =
    model.ResultTree().TotalStrain().Invariants().ResultComponent(stage, AnalysisType(), RegisterIndex(), IStrainTensorGroup::CInvariantComposite::INV_I1);

  if(!pCompInvVolStrain)
  {
    // no volumetric strain, result is invalid
    return;
  }

  values = pCompInvVolStrain->ValueElement(mesh_element, CDoubleQuantity::SI_UNIT, cb);

  for(size_t i = 0; i < values.size(); ++i)
  {
    if(values[i].Valid() && fabs(values[i].Value() - 1) > 1e-10)
      values[i] = (dIniPoro - values[i].Value()) / (1 - values[i].Value());
    else
      values[i] = geo::CValue(); // invalidate (division by zero)
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::GammaValues(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, TResultType type, UNIT /*unit*/, geo::IParallelInitializationCallback *cb) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());

  CTensorParallelInitializationCallback *pTempPool = dynamic_cast<CTensorParallelInitializationCallback *>(cb);

  if (pTempPool)
  {
    CStressTensorValueSet::TValueVec *pVcInitStress = pTempPool->getStressTensorVector(0, mesh_element.NrOfPoints());
    ResultRegister().EffectiveStress(model.DepletionStageEntry().MarkedAsInitialStage(), AnalysisType(), false).ElementValues(*pVcInitStress, mesh_element.Index(), cb);

    CStressTensorValueSet::TValueVec *pVcStress = pTempPool->getStressTensorVector(1, mesh_element.NrOfPoints());
    ResultRegister().EffectiveStress(Stage(), AnalysisType(), false).ElementValues(*pVcStress, mesh_element.Index(), cb);

    IValueDomainScalar::TValueVec *pVcInitPressure = pTempPool->getScalarVector(0, mesh_element.NrOfPoints());
    PorePressure(*pVcInitPressure, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage());

    IValueDomainScalar::TValueVec *pVcPressure = pTempPool->getScalarVector(1, mesh_element.NrOfPoints());
    PorePressure(*pVcPressure, mesh_element, Stage());

    for (size_t i = 0; i < values.size(); i++) {
      switch (type) {
      case GAMMA_VERTICAL:
        values[i] = MF::GammaVertical((*pVcInitStress)[i], (*pVcStress)[i], (*pVcInitPressure)[i].Value(), (*pVcPressure)[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MAX:
        values[i] = MF::GammaMax((*pVcInitStress)[i], (*pVcStress)[i], (*pVcInitPressure)[i].Value(), (*pVcPressure)[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MED:
        values[i] = MF::GammaMed((*pVcInitStress)[i], (*pVcStress)[i], (*pVcInitPressure)[i].Value(), (*pVcPressure)[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MIN:
        values[i] = MF::GammaMin((*pVcInitStress)[i], (*pVcStress)[i], (*pVcInitPressure)[i].Value(), (*pVcPressure)[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_VOLUME:
        values[i] = MF::GammaVolume((*pVcInitStress)[i], (*pVcStress)[i], (*pVcInitPressure)[i].Value(), (*pVcPressure)[i].Value(), MF::EFFECTIVE);
        break;
      default:
        assert(false);
      }
    }

  }
  else // legacy
  {
    CStressTensorValueSet::TValueVec vcInitStress(mesh_element.NrOfPoints());
    ResultRegister().EffectiveStress(model.DepletionStageEntry().MarkedAsInitialStage(), AnalysisType(), false).ElementValues(vcInitStress, mesh_element.Index(), cb);

    CStressTensorValueSet::TValueVec vcStress(mesh_element.NrOfPoints());
    ResultRegister().EffectiveStress(Stage(), AnalysisType(), false).ElementValues(vcStress, mesh_element.Index(), cb);

    IValueDomainScalar::TValueVec vcInitPressure(mesh_element.NrOfPoints());
    PorePressure(vcInitPressure, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage());

    IValueDomainScalar::TValueVec vcPressure(mesh_element.NrOfPoints());
    PorePressure(vcPressure, mesh_element, Stage());

    for (size_t i = 0; i < values.size(); i++) {
      switch (type) {
      case GAMMA_VERTICAL:
        values[i] = MF::GammaVertical(vcInitStress[i], vcStress[i], vcInitPressure[i].Value(), vcPressure[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MAX:
        values[i] = MF::GammaMax(vcInitStress[i], vcStress[i], vcInitPressure[i].Value(), vcPressure[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MED:
        values[i] = MF::GammaMed(vcInitStress[i], vcStress[i], vcInitPressure[i].Value(), vcPressure[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_MIN:
        values[i] = MF::GammaMin(vcInitStress[i], vcStress[i], vcInitPressure[i].Value(), vcPressure[i].Value(), MF::EFFECTIVE);
        break;
      case GAMMA_VOLUME:
        values[i] = MF::GammaVolume(vcInitStress[i], vcStress[i], vcInitPressure[i].Value(), vcPressure[i].Value(), MF::EFFECTIVE);
        break;
      default:
        assert(false);
      }
    }
  }
}

geo::CValue CResultTree::CScalarResult::CScalarResultComponent::FetchDisplacementValueFor(const geo::IPoint& pt, const CDepletionStage& stage, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue valRet;

  TDisplacementMap *cache = &m_mpDisplacementMap;

  TDisplacementMap::iterator it = cache->find(pt); // first try to find it in our own cache
  if (it != cache->end())
  {
    TDisplacementValueMap::iterator itV = it->second.find(std::make_pair(AnalysisType().AnalysisType(), &stage));
    if (itV != it->second.end())
    {
      valRet = itV->second;
      return valRet;
    }
  }

  if (dynamic_cast<CThicknessParallelInitializationCallback *>(cb)) // else try thread-local cache
  {
    static_cast<CThicknessParallelInitializationCallback *>(cb)->GetCache(&cache);

    it = cache->find(pt);
    if (it != cache->end())
    {
      TDisplacementValueMap::iterator itV = it->second.find(std::make_pair(AnalysisType().AnalysisType(), &stage));
      if (itV != it->second.end())
      {
        valRet = itV->second;
        return valRet;
      }
    }
  }

  geo::CVector vec = ResultRegister().Displacement(stage, AnalysisType(), false).ValuePoint(pt, cb);
  if(!vec.Empty())
    valRet = vec.Z();
  TDisplacementValueMap mpDVM;
  mpDVM.insert(TDisplacementValueMap::value_type(std::make_pair(AnalysisType().AnalysisType(), &stage), valRet));
  cache->insert(TDisplacementMap::value_type(pt, mpDVM));

  return valRet;
}

void CResultTree::CScalarResult::CScalarResultComponent::Thickness(IValueDomainScalar::TValueVec& values, const geo::IElement& element, const CDepletionStage& stage, geo::IParallelInitializationCallback *cb) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  values.resize(element.NrOfNodes());
  const CFormationBase* pFormation = model.Mesh().Formation(element);
  if(pFormation)
  {
    int i;
    for(i = 0; i < element.NrOfNodes(); ++i)
    {
      geo::CPoint ptTop;
      geo::CPoint ptBottom;
      values[i] = pFormation->ThicknessAt(element.Node(i), ptTop, ptBottom, cb);
      if(values[i].Valid() && !ptTop.Empty() && !ptBottom.Empty())
      {
        geo::CValue valTop = FetchDisplacementValueFor(ptTop, stage, cb);
        geo::CValue valBottom = FetchDisplacementValueFor(ptBottom, stage, cb);

        if(!valTop.Valid() || !valBottom.Valid())
        {
          values[i] = geo::CValue();
        }
        else
        {
          values[i] -= valTop.Value();
          values[i] += valBottom.Value();
        }
      }
    }
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::ShearCapacityMC(IValueDomainScalar::TValueVec& values, const geo::IElement& element, const CDepletionStage& /*stage*/) const
{
  // get the effective stresses
  CStressTensorValueSet::TValueVec vcStress(element.NrOfPoints());
  ResultRegister().EffectiveStress(Stage(), AnalysisType(), false).ElementValues(vcStress, element.Index(), geo::IParallelInitializationCallback::Sequential);

  double fric;
  double cohesion;

  // get the material model
  int iMatModel = ResultRegister().MaterialModel(element.Index());
  if(iMatModel == -1)
    return; // result not defined

  // get the friction angle
  if(iMatModel == MM_CAMCLAY)
    fric = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_INITFRICTION, Stage()) * PI / 180;
  else
    fric = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_FRICTION_ANGLE, Stage()) * PI / 180;

  // get the cohesion
  cohesion = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_COHESION, Stage());

  for(size_t i = 0; i < values.size(); i++)
  {
    if(!vcStress[i].Empty())
    {
      double sig1 = vcStress[i].PrincMaxLength();
      double sig3 = vcStress[i].PrincMinLength();

      values[i] = (0.5 * (sig1 - sig3)) / (0.5 * (sig1 + sig3) * sin(fric) + cohesion * cos(fric));
    }
    else
    {
      values[i] = geo::CValue();
    }
  }  
}

void CResultTree::CScalarResult::CScalarResultComponent::CompactionCapacity(IValueDomainScalar::TValueVec& values, const geo::IElement& element, const CDepletionStage& stage) const
{
  // get the material model
  int iMatModel = ResultRegister().MaterialModel(element.Index());
  if(iMatModel != MM_CAMCLAY && iMatModel != MM_MODIFIEDMOHRCOULOMB)
    return; // result not defined

  // get the effective stresses, preconsolidation, friction angle, cap shape, tensile stretch, and cohesion
  CStressTensorValueSet::TValueVec vcStress(element.NrOfPoints());
  ResultRegister().EffectiveStress(Stage(), AnalysisType(), false).ElementValues(vcStress, element.Index(), geo::IParallelInitializationCallback::Sequential);

  IValueDomainScalar::TValueVec vcPc(element.NrOfPoints());
  Consolidation(vcPc, element, stage);

  double fric, capShape, cohesion, tensileStretch, M;

  if(iMatModel == MM_CAMCLAY)
    fric = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_INITFRICTION, Stage()) * PI / 180;
  else
    fric = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_FRICTION_ANGLE, Stage()) * PI / 180;

  cohesion = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_COHESION, Stage());
  capShape = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_CAPSHAPE, Stage());

  if (capShape == 1)
    capShape += 0.001;

  tensileStretch = ResultRegister().MaterialParameterValue(element.Index(), IDT_VALUETYPE_TENSILE_STRETCH, Stage());
  if (tensileStretch == 1)
    tensileStretch += 0.001;


  M = 6.0 * sin(fric) / (3.0 - sin(fric));
  assert(M != 0);


  for(size_t i = 0; i < values.size(); ++i)
  {
    if(!vcStress[i].Empty() && vcPc[i].Valid())
    {
      geo::CMatrix S = vcStress[i].GetPrincipalStr();

      double S_1 = S.Value(0, 0);
      double S_2 = S.Value(1, 1);
      double S_3 = S.Value(2, 2);

      double p = (S_1 + S_2 + S_3) / 3.0;
      double q = sqrt(((S_1 - S_2)*(S_1 - S_2) + (S_2 - S_3)*(S_2 - S_3) + (S_1 - S_3)*(S_1 - S_3)) / 2.0);

      double px = p + cohesion / tan(fric);

      double X = (q > M * px || px < 0) ? tensileStretch : 1.0 / capShape;
      double Y = q / (M * (1 / X));
      double Z = (X * X - 1);

      double b  = 2 * px / Z;
      double d  = (Y * Y + px * px) / Z;

      double sign = b < 0 ? -1 : 1;

      double pa = -0.5 * b + sign * 0.5 * sqrt(b * b + 4 * d);

      double Pc_eq = (pa + cohesion / tan(fric)) * (1 + X);

      double CCU = Pc_eq / vcPc[i].Value();
      //assert(CCU > 0);

      values[i] = CCU;
    }
    else
    {
      values[i] = geo::CValue();
    }
  }  
}

void CResultTree::CScalarResult::CScalarResultComponent::ApertureValues(CScalarResult::TResultType result_type, IValueDomainScalar::TValueVec& values, const CDepletionStage& stage, const geo::IElement& mesh_element, UNIT unit) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
  if(pFormation)
  {
    const CMaterialServer& mat_server = pFormation->Material(stage);
    if(mat_server.Valid() && (mat_server.MaterialModel() == MM_FRACTURE_APERTURE || mat_server.MaterialModel() == MM_FRACTURE_APERTURE2))
    {
      const CFFMaterial& ffmat = mat_server.Material(mesh_element);
      assert(dynamic_cast<const CMaterialFractureApertureBase*>(&ffmat.Material()));
      const CMaterialFractureApertureBase& mat = static_cast<const CMaterialFractureApertureBase&>(ffmat.Material());

      geo::CVector vecHigh  = mat.HighDensityDirection(ffmat);
      geo::CVector vecLow   = mat.LowDensityDirection(ffmat);
      geo::CVector vecInter = mat.IntermediateDensityDirection(ffmat);

      geo::CVector* pVecNormal;
      geo::CVector* pVecTangent;

      bool bDelta = false;
      unsigned int uiValueTypeV0;

      switch(result_type)
      {
      case APERTURE_HI_CHANGE:
        bDelta = true;
      case APERTURE_HI:
        pVecNormal = &vecHigh;
        pVecTangent = &vecInter;
        uiValueTypeV0 = IDT_VALUETYPE_V0_HI;
        break;
      case APERTURE_ME_CHANGE:
        bDelta = true;
      case APERTURE_ME:
        pVecNormal = &vecInter;
        pVecTangent = &vecLow;
        uiValueTypeV0 = IDT_VALUETYPE_V0_ME;
        break;
      case APERTURE_LO_CHANGE:
        bDelta = true;
      case APERTURE_LO:
        pVecNormal = &vecLow;
        pVecTangent = &vecHigh;
        uiValueTypeV0 = IDT_VALUETYPE_V0_LO;
        break;
      default:
        assert(false);
        return;
      }

      // get effective stress
      CStressTensorValueSet eff_stress = ResultRegister().EffectiveStress(stage, AnalysisType(), false);
      std::vector<CStressTensor> stress_values(mesh_element.NrOfNodes());
      eff_stress.ElementValue(mesh_element, stress_values, geo::IParallelInitializationCallback::Sequential);
      assert(stress_values.size() == mesh_element.NrOfNodes());

      // initial effective stress
      const CModelBase& modelBase = static_cast<const CModelBase&>(Model());
      const CDepletionStage& inistage = modelBase.DepletionStageEntry().MarkedAsInitialStage();
      CStressTensorValueSet eff_stress_ini = ResultRegister().EffectiveStress(inistage, AnalysisType(), false);
      std::vector<CStressTensor> stress_values_ini(mesh_element.NrOfNodes());
      eff_stress_ini.ElementValue(mesh_element, stress_values_ini, geo::IParallelInitializationCallback::Sequential);
      assert(stress_values_ini.size() == mesh_element.NrOfNodes());

      IValueDomainScalar::TValueVec vcInitValues(mesh_element.NrOfNodes()); // initial values when 'marked as initial stage' != 'initial stage'
      if(&stage != &inistage && &inistage != &inistage.InitialStage())
        ApertureValues(result_type, vcInitValues, inistage, mesh_element, CQuantity::SI_UNIT);

      for(int i = 0; i < mesh_element.NrOfNodes(); ++i)
      {
        const CStressTensor& stress = stress_values[i];
        geo::CVector vecStress = stress.RotateToFace(*pVecNormal, *pVecTangent);
        double s1 = vecStress.X(); // the effective normal stress

        const CStressTensor& stress_ini = stress_values_ini[i];
        vecStress = stress_ini.RotateToFace(*pVecNormal, *pVecTangent);
        double s0 = vecStress.X(); // the initial effective normal stress

        double sRef = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCESTRESS); // reference stress
        double a = ffmat.ParameterValue(IDT_VALUETYPE_APERTUREPARAM); // aperture parameter

        double DRef;
        if(ffmat.IsParameter(IDT_VALUETYPE_FRACT_STIFF_NORMAL))
        {
          DRef = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_NORMAL); // fracture stiffness
        }
        else
        {
          assert(ffmat.IsParameter(IDT_VALUETYPE_REFERENCE_APERTURE));
          double wref = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCE_APERTURE);

          if (a == 0.0)
          {
            a = 1e-7;
          }

          if (wref < 1e-6)
          {
            wref = 1e-6;
          }

          DRef = 1 / (a * wref);
        }

        /*
        With reference fracture stiffness D_ref and reference stress s_ref the fracture opening can be computed
        from the effective normal stress at a stage (say s1 at D1) and initial stress (s0 at D0)
         
        The aperture increase (opening) is:
          Delta_v=  -1/( a * D_ref) * [  exp (  -a {s1  -  s_ref})  - exp (  -a {s0  -  s_ref}) ]

        The total aperture is hence  v = v0 + Delta_v
         
        If a=0, the equation is not valid.  Use  Delta_v=  -1/( D_ref) * [  s1  -  s0  ]
        */

        double dV;
        if(fabs(a) > 1e-8)
          dV = 1 / (a * DRef) * (exp(-a * (s1 - sRef)) - exp(-a * (s0 - sRef)));
        else
          dV = -1 / (DRef) * (s1 - s0);

        if(!bDelta)
        {
          if(&stage != &inistage && &inistage != &inistage.InitialStage())
          {
            dV += vcInitValues[i].Value();
          }
          else
          {
            double V0 = 0;
            if(ffmat.IsParameter(uiValueTypeV0))
            {
              V0 = ffmat.ParameterValue(uiValueTypeV0);
            }
            else
            {
              // Material model fracture aperture 2 computes initial aperture from initial stress:
              // Compute initial fracture aperture (per direction) from the initial (effective) normal stress s'n (hence after D0)
              // wi = wref * exp ( -a (s'n - s'ref))

              double wref = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCE_APERTURE);

              if (wref < 1e-6)
              {
                wref = 1e-6;
              }

              V0 = wref * exp(-a * (s0 - sRef));
            }

            dV += V0;
          }
        }

        if(unit == CQuantity::FIELD_UNIT)
          dV *= FF_FACTOR_LENGTH;
        values[i] = dV;
      }
    }
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::
  apertureValues(IValueDomainScalar::TValueVec& fractureAperture, const geo::IElement& mesh_element,
    const CDepletionStage& depletionStage, UNIT unit,
    TResultType resultType) const
{
  const CModelBase& model = (const CModelBase&) (Model());
  const IScalarResultComponent* scalarResultComponent =
    dynamic_cast <const IScalarResultComponent*> (
      model.ResultTree().Aperture(resultType).
        ResultComponent(depletionStage, AnalysisType(), 0));

  assert(scalarResultComponent != 0);

  scalarResultComponent->ElementValues(fractureAperture, mesh_element, unit, geo::IParallelInitializationCallback::Sequential);
}

void CResultTree::CScalarResult::CScalarResultComponent::PlasticEnergy(const CDepletionStage& stage, IValueDomainScalar::TValueVec& values, const geo::IElement& element, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	/*
	WI 81410:
	Total plastic energy per unit volume is (p'*Eps_vp+q*Eps_qp)   in MJ/m^3
		p' = isotropic effectieve stress
		q  =  Von Mises stress
		Eps_vp  =  plastic volumetric strain
		Eps_qp  = plastic Von Mises strain

    The total energy for a stage is the total energy of the previous stage + the energy of the current stage, which is
 
    (p'*ΔEps_vp + q*ΔEps_qp)
	*/
  IValueDomainScalar::TValueVec previous_values;

  if(!stage.Initial() && (&stage.Previous() != &stage.InitialStage()))
  {
    PlasticEnergy(stage.Previous(), previous_values, element, unit, cb);
  }
  else
  {
    previous_values.resize(element.NrOfPoints(), 0);
  }

	IValueDomainScalar::TValueVec vcIsoEffStress;
	IValueDomainScalar::TValueVec vcVonMisesStress;
	IValueDomainScalar::TValueVec vcPlastVolStrain;
  IValueDomainScalar::TValueVec vcPlastVolStrainPrev;
	IValueDomainScalar::TValueVec vcPlastVonMisesStrain;
  IValueDomainScalar::TValueVec vcPlastVonMisesStrainPrev;

	const CModelBase& model = (const CModelBase&)Model();
  const IResultComponent* pComponent;
	const IResult* pComposite = model.ResultTree().EffectiveStress().InvariantComposite();
	if(pComposite)
	{
    // isotropic effective stress
		pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), CResultTree::CEffectiveStressResult::CInvariantComposite::INV_I1);
		if(pComponent)
			vcIsoEffStress = pComponent->ValueElement(element, unit, cb);

    // von mises stress
		pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), CResultTree::CEffectiveStressResult::CInvariantComposite::INV_VONMIS);
		if(pComponent)
			vcVonMisesStress = pComponent->ValueElement(element, unit, cb);
	}

	pComposite = &model.ResultTree().PlasticStrain().Invariants();
	if(pComposite)
	{
    // plastic volumetric strain
		pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), CResultTree::CPlasticStrainResult::CInvariantComposite::INV_I1);
    if(pComponent)
      vcPlastVolStrain = pComponent->ValueElement(element, unit, cb);

    if(!stage.Initial() && (&stage.Previous() != &stage.InitialStage()))
    {
      pComponent = pComposite->ResultComponent(stage.Previous(), AnalysisType(), RegisterIndex(), CResultTree::CPlasticStrainResult::CInvariantComposite::INV_I1);
      if(pComponent)
        vcPlastVolStrainPrev = pComponent->ValueElement(element, unit, cb);
    }
    else
    {
      vcPlastVolStrainPrev.resize(element.NrOfPoints(), 0);
    }

    // plastic von mises strain
    pComponent = pComposite->ResultComponent(stage, AnalysisType(), RegisterIndex(), CResultTree::CPlasticStrainResult::CInvariantComposite::INV_VONMIS);
    if(pComponent)
      vcPlastVonMisesStrain = pComponent->ValueElement(element, unit, cb);

    if(!stage.Initial() && (&stage.Previous() != &stage.InitialStage()))
    {
      pComponent = pComposite->ResultComponent(stage.Previous(), AnalysisType(), RegisterIndex(), CResultTree::CPlasticStrainResult::CInvariantComposite::INV_VONMIS);
      if(pComponent)
        vcPlastVonMisesStrainPrev = pComponent->ValueElement(element, unit, cb);
    }
    else
    {
      vcPlastVonMisesStrainPrev.resize(element.NrOfPoints(), 0);
    }
	}

  if(vcIsoEffStress.size()            == element.NrOfPoints() &&
     vcVonMisesStress.size()          == element.NrOfPoints() &&
     vcPlastVolStrain.size()          == element.NrOfPoints() &&
     vcPlastVolStrainPrev.size()      == element.NrOfPoints() &&
     vcPlastVonMisesStrain.size()     == element.NrOfPoints() &&
     vcPlastVonMisesStrainPrev.size() == element.NrOfPoints())
  {
    values.resize(element.NrOfPoints());
    for(int i = 0; i < element.NrOfPoints(); ++i)
    {
      if(previous_values[i].Valid()       &&
         vcIsoEffStress[i].Valid()        &&
         vcVonMisesStress[i].Valid()      &&
         vcPlastVolStrain[i].Valid()      &&
         vcPlastVolStrainPrev[i].Valid()  &&
         vcPlastVonMisesStrain[i].Valid() &&
         vcPlastVonMisesStrainPrev[i].Valid())
      {
        double p = vcIsoEffStress[i].Value();
        double q = vcVonMisesStress[i].Value();
        double Eps_vp = vcPlastVolStrain[i].Value() - vcPlastVolStrainPrev[i].Value();
        double Eps_qp = vcPlastVonMisesStrain[i].Value() - vcPlastVonMisesStrainPrev[i].Value();
        values[i] = p * Eps_vp + q * Eps_qp + previous_values[i].Value();
      }
    }
  }
}

namespace
{

const double MICRO_METRE = 1e-6;

} // anonymous namespace

void CResultTree::CScalarResult::CScalarResultComponent::
  permeabilityMultiplierValues(TResultType result_type, IValueDomainScalar::TValueVec& values,
    const geo::IElement& mesh_element, UNIT unit) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  const CFormationBase* pFormation = model.Mesh().Formation(mesh_element);
  if(!pFormation)
    return;

  const CFFMaterial& mat = pFormation->Material(model.DepletionStageEntry().MarkedAsInitialStage()).Material(mesh_element);

  if(!mat.IsParameter(IDT_VALUETYPE_HIGH_FRACT_DENS) ||
     !mat.IsParameter(IDT_VALUETYPE_INTER_FRACT_DENS) ||
     !mat.IsParameter(IDT_VALUETYPE_LOW_FRACT_DENS))
    return;

  double rho1 = 0;
  double rho2 = 0;

  IValueDomainScalar::TValueVec fractureAperture1(mesh_element.NrOfPoints());
  IValueDomainScalar::TValueVec fractureApertureChange1(mesh_element.NrOfPoints());
  IValueDomainScalar::TValueVec fractureAperture2(mesh_element.NrOfPoints());
  IValueDomainScalar::TValueVec fractureApertureChange2(mesh_element.NrOfPoints());

  switch (result_type)
  {
    case PERMEABILITY_MULTIPLIER_HI:
      apertureValues(fractureAperture1, mesh_element, Stage(), unit,
        APERTURE_ME);
      apertureValues(fractureApertureChange1, mesh_element, Stage(), unit,
        APERTURE_ME_CHANGE);
      apertureValues(fractureAperture2, mesh_element, Stage(), unit,
        APERTURE_LO);
      apertureValues(fractureApertureChange2, mesh_element, Stage(), unit,
        APERTURE_LO_CHANGE);
      rho1 = mat.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS);
      rho2 = mat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS);
      break;
    case PERMEABILITY_MULTIPLIER_ME:
      apertureValues(fractureAperture1, mesh_element, Stage(), unit,
        APERTURE_LO);
      apertureValues(fractureApertureChange1, mesh_element, Stage(), unit,
        APERTURE_LO_CHANGE);
      apertureValues(fractureAperture2, mesh_element, Stage(), unit,
        APERTURE_HI);
      apertureValues(fractureApertureChange2, mesh_element, Stage(), unit,
        APERTURE_HI_CHANGE);
      rho1 = mat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS);
      rho2 = mat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS);
      break;
    case PERMEABILITY_MULTIPLIER_LO:
      apertureValues(fractureAperture1, mesh_element, Stage(), unit,
        APERTURE_HI);
      apertureValues(fractureApertureChange1, mesh_element, Stage(), unit,
        APERTURE_HI_CHANGE);
      apertureValues(fractureAperture2, mesh_element, Stage(), unit,
        APERTURE_ME);
      apertureValues(fractureApertureChange2, mesh_element, Stage(), unit,
        APERTURE_ME_CHANGE);
      rho1 = mat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS);
      rho2 = mat.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS);
      break;
    default:
      assert(false);
  }

  for (int i = 0; i < mesh_element.NrOfNodes(); ++i)
  {
    if (fractureApertureChange1[i].Valid() && fractureAperture1[i].Valid() &&
        fractureApertureChange2[i].Valid() && fractureAperture2[i].Valid())
    {
      double w1 = fractureAperture1[i].Value();
      double w2 = fractureAperture2[i].Value();
      double w0_1 = fractureAperture1[i].Value() - fractureApertureChange1[i].Value();
      double w0_2 = fractureAperture2[i].Value() - fractureApertureChange2[i].Value();

      if(w0_1 < 0)
        w0_1 = 0;
      if(w0_2 < 0)
        w0_2 = 0;

      double nom = rho1 * pow(w1, 3)   + rho2 * pow(w2, 3);
      double den = rho1 * pow(w0_1, 3) + rho2 * pow(w0_2, 3);

      if(den < 1e-20)
        values[i] = 1;
      else
        values[i] = nom / den;
    }
    else
    {
      values[i] = 1;
    }
  }
}

// wjrx mantis 2870
// B1/B4 and D1/D10 variables refer to mantis 2870 attached excelsheet fields
//
void CResultTree::CScalarResult::CScalarResultComponent::calcValues
  ( const geo::CValue &sEE
  , const geo::CValue &sNN
  , const geo::CValue &sVV
  , const geo::CValue &sEN
  , geo::CValue &SHTotazimuth
  , geo::CValue &SHTot_SvTot
  , geo::CValue &ShTot_SvTot
  ) const
{
  if ( sEE.Valid() && sNN.Valid() && sVV.Valid() && sEN.Valid() )
  {
    double B1= sEE.Value();
    double B2= sNN.Value();
    double B3= sVV.Value();
    double B4= sEN.Value();

    double D1= sqrt(B1*B1+4*B4*B4-2*B1*B2+B2*B2);
    double D2= 0.5*(B1+B2+D1);
    double D3= 0.5*(B1+B2-D1);
    double D4= B3;
    double D5= 0.5*(-B1+B2+D1);
    double D6= B4;

    if ( fabs(D5) > EPS )
    {
      double D7= 180/PI*atan(D6/D5+0.000001);
      double D8= D7<0 ? D7+360 : D7;
      if ( D8 >= 180 ) D8 -= 180; //wjrx mantis 2977
      SHTotazimuth.Value( D8);
    }
    else SHTotazimuth= geo::CValue();

    if ( fabs(D4) > EPS )
    {
      SHTot_SvTot.Value( D2/D4 ); //D9
      ShTot_SvTot.Value( D3/D4 ); //D10
    }
    else
    {
      SHTot_SvTot= geo::CValue();
      ShTot_SvTot= geo::CValue();
    }
  }
  else
  {
    SHTotazimuth= geo::CValue();
    SHTot_SvTot= geo::CValue();
    ShTot_SvTot= geo::CValue();
  }
}


void CResultTree::CScalarResult::CScalarResultComponent::ElementValues(IValueDomainScalar::TValueVec& values, const geo::IElement& mesh_element, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  const CScalarResult *pResult = dynamic_cast<const CScalarResult*>(&Parent());
  assert(pResult);

    switch(pResult->ResultType())
  {
    case SHTOTAZIMUTH: 
        {
    CStressTensorValueSet::TValueVec vcValues( mesh_element.NrOfPoints() );
    ResultRegister().TotalStress(Stage(), AnalysisType(), false).ElementValues(vcValues, mesh_element.Index(), 0);

    for(size_t i = 0; i < values.size(); i++)
    {
      geo::CValue dummy;
      calcValues( vcValues[i].YY(), vcValues[i].XX(), vcValues[i].ZZ(), vcValues[i].XY(), values[i], dummy, dummy);
    }
  }
        break;
    case SHTOT_SvTOT:
  {
    CStressTensorValueSet::TValueVec vcValues( mesh_element.NrOfPoints() );
    ResultRegister().TotalStress(Stage(), AnalysisType(), false).ElementValues(vcValues, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);

    for(size_t i = 0; i < values.size(); i++)
    {
      geo::CValue dummy;
      calcValues( vcValues[i].YY(), vcValues[i].XX(), vcValues[i].ZZ(), vcValues[i].XY(), dummy, values[i], dummy);
    }
  }
        break;
    case ShTOT_SvTOT:
  {
    CStressTensorValueSet::TValueVec vcValues( mesh_element.NrOfPoints() );
    ResultRegister().TotalStress(Stage(), AnalysisType(), false).ElementValues(vcValues, mesh_element.Index(), geo::IParallelInitializationCallback::Sequential);

    for(size_t i = 0; i < values.size(); i++)
    {
      geo::CValue dummy;
      calcValues( vcValues[i].YY(), vcValues[i].XX(), vcValues[i].ZZ(), vcValues[i].XY(), dummy, dummy, values[i]);
    }
  }
        break;
    case SHEAR_CAPACITY:
    ShearCapacityMC(values, mesh_element, Stage());
        break;
    case  COMPACTION_CAPACITY:
    CompactionCapacity(values, mesh_element, Stage());
        break;
    case PORE_PRESSURE:
    case PORE_PRESSURE_GRADIENT:
      {
        PorePressure( values, mesh_element, Stage() );
        for(size_t i = 0; i < values.size(); i++)
        {
           if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
             values[i] *= FF_FACTOR_PRESSURE;
        }
      }
      break;
    case PORE_PRESSURE_CHANGE:
      {
      	IValueDomainScalar::TValueVec vcInitial(values.size());
        PorePressure( values, mesh_element, Stage() );
        PorePressure( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
        for(size_t i = 0; i < values.size(); i++)
        {
          if(values[i].Valid())
          {
            values[i] -= vcInitial[i];
            if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
              values[i] *= FF_FACTOR_PRESSURE;
          }
        }
      }
      break;
    case PORE_PRESSURE_INPUT:
    case PORE_PRESSURE_GRADIENT_INPUT:
      {
        PorePressureInput( values, mesh_element, Stage() );
        for(size_t i = 0; i < values.size(); i++)
        {
           if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
             values[i] *= FF_FACTOR_PRESSURE;
        }
      }
      break;
    case PORE_PRESSURE_CHANGE_INPUT:
      {
      	IValueDomainScalar::TValueVec vcInitial(values.size());
        PorePressureInput( values, mesh_element, Stage() );
        PorePressureInput( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
        for(size_t i = 0; i < values.size(); i++)
        {
          if(values[i].Valid())
          {
            values[i] -= vcInitial[i];
            if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
              values[i] *= FF_FACTOR_PRESSURE;
          }
        }
      }
      break;
    case FRACTURE_MATRIX_PRESSURE:
        {
    FractureMatrixPressure( values, mesh_element, Stage() );
    for(size_t i = 0; i < values.size(); i++) {
                if(unit == CQuantity::FIELD_UNIT && values[i].Valid()) values[i] *= FF_FACTOR_PRESSURE;
    }
        }
        break;
    case FRACTURE_MATRIX_PRESSURE_CHANGE:
        {
	IValueDomainScalar::TValueVec vcInitial(values.size());
    FractureMatrixPressure( values, mesh_element, Stage() );
    FractureMatrixPressure( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
    for(size_t i = 0; i < values.size(); i++) {
      if(values[i].Valid())
      {
        values[i] -= vcInitial[i];
                    if(unit == CQuantity::FIELD_UNIT && values[i].Valid()) values[i] *= FF_FACTOR_PRESSURE;
      }
    }
        }
        break;
    case TEMPERATURE:
    case TEMPERATURE_GRADIENT:
      {
        Temperature( values, mesh_element, Stage() );
        for(size_t i = 0; i < values.size(); ++i)
        {
          if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
            values[i] *= FF_FACTOR_TEMPERATURE;
        }
      }
      break;
    case TEMPERATURE_CHANGE:
      {
	      IValueDomainScalar::TValueVec vcInitial(values.size());
        Temperature( values, mesh_element, Stage() );
        Temperature( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
        for(size_t i = 0; i < values.size(); ++i)
        {
          if(values[i].Valid())
          {
            values[i] -= vcInitial[i];
            if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
              values[i] *= FF_FACTOR_TEMPERATURE;
          }
        }
      }
      break;
    case TEMPERATURE_INPUT:
    case TEMPERATURE_GRADIENT_INPUT:
      {
        TemperatureInput( values, mesh_element, Stage() );
        for(size_t i = 0; i < values.size(); ++i)
        {
          if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
            values[i] *= FF_FACTOR_TEMPERATURE;
        }
      }
      break;
    case TEMPERATURE_CHANGE_INPUT:
      {
	      IValueDomainScalar::TValueVec vcInitial(values.size());
        TemperatureInput( values, mesh_element, Stage() );
        TemperatureInput( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
        for(size_t i = 0; i < values.size(); ++i)
        {
          if(values[i].Valid())
          {
            values[i] -= vcInitial[i];
            if(unit == CQuantity::FIELD_UNIT && values[i].Valid())
              values[i] *= FF_FACTOR_TEMPERATURE;
          }
        }
      }
      break;
    case CONSOL:
        {
    Consolidation( values, mesh_element, Stage() );
    for(size_t i = 0; i < values.size(); ++i) {
                if(unit == CQuantity::FIELD_UNIT && values[i].Valid()) values[i] *= FF_FACTOR_STRESS;
            }
    }
        break;
    case CONSOL_CHANGE:
        {
	IValueDomainScalar::TValueVec vcInitial(values.size());
    Consolidation( values, mesh_element, Stage() );
    Consolidation( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage() );
    for(size_t i = 0; i < values.size(); ++i) {
      if(values[i].Valid())
      {
        values[i] -= vcInitial[i];
                    if(unit == CQuantity::FIELD_UNIT && values[i].Valid()) values[i] *= FF_FACTOR_STRESS;
                }
      }
    }
        break;
    case POROSITY:
    Porosity( values, mesh_element, Stage(), cb );
        break;
    case POROSITY_CHANGE:
        {
	IValueDomainScalar::TValueVec vcInitial(values.size());
    Porosity( values, mesh_element, Stage(), cb );
    Porosity( vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage(), cb );
    for(size_t i = 0; i < values.size(); ++i)
    {
      if(values[i].Valid() && vcInitial[i].Valid())
        values[i] -= vcInitial[i];
    }
        } 
        break;
    case THICKNESS:
        {
    Thickness(values, mesh_element, Stage(), cb);
    if(unit == CQuantity::FIELD_UNIT)
    {
      for(size_t i = 0; i < values.size(); ++i)
                    if(values[i].Valid()) values[i] *= FF_FACTOR_LENGTH;
            }
    }
        break;
    case THICKNESS_CHANGE:
        {
    IValueDomainScalar::TValueVec vcInitial(values.size());
    Thickness(values, mesh_element, Stage(), cb);
    Thickness(vcInitial, mesh_element, model.DepletionStageEntry().MarkedAsInitialStage(), cb);
    for(size_t i = 0; i < values.size(); ++i)
    {
      if(values[i].Valid() && vcInitial[i].Valid())
      {
        values[i] -= vcInitial[i];
        if(unit == CQuantity::FIELD_UNIT)
          values[i] *= FF_FACTOR_LENGTH;
      }
    }
        }
        break;
    case VOLUME_AVERAGE:
      {
        if (mesh_element.IsInterfaceElement())
          break;

        const CFormationBase &formation = *model.Mesh().Formation(mesh_element);
      
        if (!formation.Depleting())
        {
          for(size_t i = 0; i < values.size(); ++i)
            values[i] = geo::CValue();
        }

        bool bCached = false;
        CTensorParallelInitializationCallback *pTempPool = dynamic_cast<CTensorParallelInitializationCallback *>(cb);

        TFormAvgVolMap *cache = &m_mpValues;
      
        // Value in cache?
        TFormAvgVolMap::const_iterator it = cache->find(&formation);
        if (it != cache->end())
        {
          for (size_t i = 0; i < values.size(); ++i)
          {
            values[i] = it->second;
          }
          bCached = true;
        }

        if (dynamic_cast<CAvgVolumeParallelInitializationCallback *>(cb))
        {
          static_cast<CAvgVolumeParallelInitializationCallback *>(cb)->GetCache(&cache);

          if (!bCached)
          {
            it = cache->find(&formation);
            if (it != cache->end())
            {
              for (size_t i = 0; i < values.size(); ++i)
              {
                values[i] = it->second;
              }
              bCached = true;
            }
          }
        }
    
        if (!bCached)
        {
          // Not in cache calculate for the formation
          double total_volume = 0.0;
          double total_weighted_local = 0.0;

          if (pTempPool)
          {
            for (int i = 0; i < formation.ElementSetSize(); ++i)
            {
              const geo::IElementSet *element_set = &formation.ElementSet(i).ElementSet();
              for (int j = 0; j < element_set->ElementSize(); ++j)
              {
                const geo::IElement *element = &element_set->Element(j);

                // 0 and 1 are used in GammaValues
                IValueDomainScalar::TValueVec *gamma_min = pTempPool->getScalarVector(2, element->NrOfPoints());
                IValueDomainScalar::TValueVec *gamma_med = pTempPool->getScalarVector(3, element->NrOfPoints());
                IValueDomainScalar::TValueVec *gamma_max = pTempPool->getScalarVector(4, element->NrOfPoints());
                GammaValues(*gamma_min, *element, GAMMA_MIN, unit, cb);
                GammaValues(*gamma_med, *element, GAMMA_MED, unit, cb);
                GammaValues(*gamma_max, *element, GAMMA_MAX, unit, cb);

                for (int k = 0; k < element->NrOfPoints(); ++k)
                {
                  if ((*gamma_min)[k].Valid() && (*gamma_med)[k].Valid() && (*gamma_max)[k].Valid())
                  {
                    double influence_volume = element->InfluenceSize(k);
                    total_weighted_local += influence_volume * (1.0 / 3.0) * ((*gamma_min)[k].Value() + (*gamma_med)[k].Value() + (*gamma_max)[k].Value());
                    total_volume += influence_volume;
                  }
                }
              }
            }
          }
          else // legacy code
          {
            for (int i = 0; i < formation.ElementSetSize(); ++i)
            {
              const geo::IElementSet *element_set = &formation.ElementSet(i).ElementSet();
              for (int j = 0; j < element_set->ElementSize(); ++j)
              {
                const geo::IElement *element = &element_set->Element(j);

                IValueDomainScalar::TValueVec gamma_min(element->NrOfPoints());
                IValueDomainScalar::TValueVec gamma_med(element->NrOfPoints());
                IValueDomainScalar::TValueVec gamma_max(element->NrOfPoints());
                GammaValues(gamma_min, *element, GAMMA_MIN, unit, cb);
                GammaValues(gamma_med, *element, GAMMA_MED, unit, cb);
                GammaValues(gamma_max, *element, GAMMA_MAX, unit, cb);

                for (int k = 0; k < element->NrOfPoints(); ++k)
                {
                  if (gamma_min[k].Valid() && gamma_med[k].Valid() && gamma_max[k].Valid())
                  {
                    double influence_volume = element->InfluenceSize(k);
                    total_weighted_local += influence_volume * (1.0 / 3.0) * (gamma_min[k].Value() + gamma_med[k].Value() + gamma_max[k].Value());
                    total_volume += influence_volume;
                  }
                }
              }
            }
          }

          geo::CValue val;
          if(fabs(total_volume) > EPS)
            val = total_weighted_local / total_volume;

          cache->insert(TFormAvgVolMap::value_type(&formation, val));

          ElementValues(values, mesh_element, unit, cb); // Get it now... because the value is in the map
        }
      }
      break;
    case APERTURE_HI:
    case APERTURE_ME:
    case APERTURE_LO:
    case APERTURE_HI_CHANGE:
    case APERTURE_ME_CHANGE:
    case APERTURE_LO_CHANGE:
    ApertureValues(pResult->ResultType(), values, Stage(), mesh_element, unit);
        break;
    case PERMEABILITY_MULTIPLIER_HI:
    case PERMEABILITY_MULTIPLIER_ME:
    case PERMEABILITY_MULTIPLIER_LO:
    permeabilityMultiplierValues(pResult->ResultType(), values, mesh_element, unit);
    break;
    case PLASTIC_ENERGY:
	  PlasticEnergy(Stage(), values, mesh_element, unit, cb);
    break;
    case VOLUMETRIC_STRAIN_INPUT:
    case VOLUMETRIC_STRAIN_CHANGE_INPUT:
      {
        const CModelBase& modelBase = dynamic_cast <const CModelBase&> (Model());

        if (!mesh_element.IsInterfaceElement())
        {
          const CFormationBase* formationBase = modelBase.Mesh().Formation(mesh_element);

          if (formationBase)
          {
            if(formationBase->Strain(Stage()).IsVolumetric())
            {
              values = formationBase->Strain(Stage()).Component().
                ScalarData().ValueElement(mesh_element);

              if(pResult->ResultType() == VOLUMETRIC_STRAIN_CHANGE_INPUT)
              {
                if(!modelBase.DepletionStageEntry().MarkedAsInitialStage().Initial())
                {
                  const IValueDomainScalar::TValueVec& inivalues = formationBase->Strain(modelBase.DepletionStageEntry().MarkedAsInitialStage()).Component().ScalarData().ValueElement(mesh_element);
                  for(size_t i = 0; i < values.size(); ++i)
                  {
                    if(values[i].Valid() && inivalues[i].Valid())
                      values[i] -= inivalues[i];
                    else
                      values[i] = geo::CValue();
                  }
                }
                else
                {
                  for(size_t i = 0; i < values.size(); ++i)
                    values[i] = geo::CValue();
                }
              }
            }
            else
            {
              for(size_t i = 0; i < values.size(); ++i)
                values[i] = geo::CValue();
            }
          }
        }
      }
      break;
    case NORMAL_STRAIN_INPUT:
      {
        const CModelBase& modelBase =
          dynamic_cast <const CModelBase&> (Model());

        if (!mesh_element.IsInterfaceElement())
        {
          const CFormationBase* formationBase =
            modelBase.Mesh().Formation(mesh_element);

          if (formationBase)
          {
            bool bIsTensor;
            IValueDomainTensor::TValueVec tensors = formationBase->Strain(Stage()).ValueTensor(mesh_element, bIsTensor);
            assert(values.size() == mesh_element.NrOfPoints());
            assert(tensors.size() == mesh_element.NrOfPoints());

            if (!bIsTensor)
            {
              for (size_t i = 0; i < tensors.size(); ++i)
                if (!tensors[i].Empty())
                  values[i] = tensors[i].ZZ();
            }
          }
        }
      }
      break;
    case LATERAL_STRAIN_INPUT:
      {
        const CModelBase& modelBase =
          dynamic_cast <const CModelBase&> (Model());

        if (!mesh_element.IsInterfaceElement())
        {
          const CFormationBase* formationBase =
            modelBase.Mesh().Formation(mesh_element);

          if (formationBase)
          {
            bool bIsTensor;
            IValueDomainTensor::TValueVec tensors = formationBase->Strain(Stage()).ValueTensor(mesh_element, bIsTensor);
            assert(values.size() == mesh_element.NrOfPoints());
            assert(tensors.size() == mesh_element.NrOfPoints());

            if (!bIsTensor)
            {
              for (size_t i = 0; i < tensors.size(); ++i)
              {
                if (!tensors[i].Empty())
                {
                  assert(tensors[i].XX() == tensors[i].YY());
                  values[i] = tensors[i].XX();
                }
              }
            }
          }
        }
      }
      break;
    default:
    // Should be the gamma's
      GammaValues(values, mesh_element, pResult->ResultType(), unit, cb);
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::ClearMap()
{
//  if(m_mpValues.size() > 0)
//    m_mpValues.clear();
}

void CResultTree::CScalarResult::CScalarResultComponent::AddToCache(TDisplacementMap *cache)
{
  for (TDisplacementMap::iterator it = cache->begin(); it != cache->end(); ++it)
  {
    m_mpDisplacementMap.insert(*it);
  }
}

void CResultTree::CScalarResult::CScalarResultComponent::AddToCache(TFormAvgVolMap *cache)
{
  for (TFormAvgVolMap::iterator it = cache->begin(); it != cache->end(); ++it)
  {
    m_mpValues.insert(*it);
  }
}


geo::IParallelInitializationCallback *CResultTree::CScalarResult::CScalarResultComponent::GetParallelInitializationCallback()
{
  const CScalarResult *pResult = dynamic_cast<const CScalarResult*>(&Parent());
  assert(pResult);

  switch (pResult->ResultType())
  {
  case THICKNESS_CHANGE:
  case THICKNESS:
    return new CThicknessParallelInitializationCallback(*this);
  case VOLUME_AVERAGE:
    return new CAvgVolumeParallelInitializationCallback(static_cast<CModelBase&>(Model()), *this);
  default:
    return new CTensorParallelInitializationCallback(static_cast<CModelBase&>(Model()));
  }
}

bool CResultTree::CScalarResult::CScalarResultComponent::Gradient() const
{
  const CScalarResult *pResult = static_cast<const CScalarResult*>(&Parent());

  return pResult->ResultType() == PORE_PRESSURE_GRADIENT ||
         pResult->ResultType() == PORE_PRESSURE_GRADIENT_INPUT ||
         pResult->ResultType() == TEMPERATURE_GRADIENT ||
         pResult->ResultType() == TEMPERATURE_GRADIENT_INPUT;
}


//////////////////////////////////////////////////////////////////////
// Implementation of CScalarResult::CScalarResultComponent
//////////////////////////////////////////////////////////////////////

bool CResultTree::CScalarResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= ExportLabel(0).toStdString();

  return validateName(resultInfo, name, label, "", "_L|_M|_C|_N|_H");
}


bool CResultTree::CScalarResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  if(antype == CAnalysisType::AT_HEAT && (ResultType() == TEMPERATURE_CHANGE_INPUT   ||
                                          ResultType() == TEMPERATURE_INPUT          ||
                                          ResultType() == TEMPERATURE_GRADIENT_INPUT ||
                                          ResultType() == TEMPERATURE                ||
                                          ResultType() == TEMPERATURE_CHANGE         ||
                                          ResultType() == TEMPERATURE_GRADIENT       ||
                                          ResultType() == PORE_PRESSURE_CHANGE_INPUT ||
                                          ResultType() == PORE_PRESSURE_INPUT        ||
                                          ResultType() == PORE_PRESSURE_GRADIENT_INPUT ||
                                          ResultType() == NORMAL_STRAIN_INPUT ||
                                          ResultType() == LATERAL_STRAIN_INPUT ||
                                          ResultType() == VOLUMETRIC_STRAIN_INPUT    ||
                                          ResultType() == VOLUMETRIC_STRAIN_CHANGE_INPUT))
    return true;

  return (antype == CAnalysisType::AT_LINEAR  ||
          antype == CAnalysisType::AT_MIXTURE ||
          antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
          (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

void CResultTree::CScalarResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
  {
    switch(ResultType())
    {
    case PORE_PRESSURE_CHANGE:
    case PORE_PRESSURE_CHANGE_INPUT:
    case FRACTURE_MATRIX_PRESSURE_CHANGE:
    case TEMPERATURE_CHANGE:
    case TEMPERATURE_CHANGE_INPUT:
    case GAMMA_VERTICAL:
    case GAMMA_MAX:
    case GAMMA_MED:
    case GAMMA_MIN:
    case GAMMA_VOLUME:
    case POROSITY_CHANGE:
    case THICKNESS_CHANGE:
    case APERTURE_HI_CHANGE:
    case APERTURE_ME_CHANGE:
    case APERTURE_LO_CHANGE:
    case PERMEABILITY_MULTIPLIER_HI:
    case PERMEABILITY_MULTIPLIER_ME:
    case PERMEABILITY_MULTIPLIER_LO:
      if(stage.isAfter(stage.Entry().MarkedAsInitialStage()))
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case VOLUME_AVERAGE:
      if(!stage.Initial())
        m_pVolumeAverage = new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case CONSOL:
      if(antype == CAnalysisType::AT_NONLIN || antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT)
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case CONSOL_CHANGE:
      if((antype == CAnalysisType::AT_NONLIN || antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
         !stage.Initial())
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case PLASTIC_ENERGY:
      if(stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
         (antype == CAnalysisType::AT_NONLIN || antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
         stage.OutputType() != CDepletionStage::NONE)
        new CScalarResultComponent(*this, stage, antype, nRegister);
    case POROSITY:
      if(stage.isAfter(stage.Entry().MarkedAsInitialStage()))
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case VOLUMETRIC_STRAIN_INPUT:
      if(!stage.Initial())
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case VOLUMETRIC_STRAIN_CHANGE_INPUT:
      if(stage.isAfter(stage.Entry().MarkedAsInitialStage()))
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case NORMAL_STRAIN_INPUT:
    case LATERAL_STRAIN_INPUT:
      if (!stage.Initial())
        new CScalarResultComponent(*this, stage, antype, nRegister);
      break;
    case APERTURE_HI:
    case APERTURE_ME:
    case APERTURE_LO:
    case SHTOTAZIMUTH: // wjrx mantis 2870
    case SHTOT_SvTOT: //wjrx mantis 2870
    case ShTOT_SvTOT: //wjrx mantis 2870
    default:
        new CScalarResultComponent(*this, stage, antype, nRegister);
        break;
    }
  }
}

CResultTree::CScalarResult::CScalarResult(unsigned int uName, unsigned int uLabelId, CResultGroup& group, TResultType result_type)
  : IResult(uName, group), m_result_type(result_type), m_uLabel(uLabelId), m_pVolumeAverage(0), m_bNeedTensorParallelInitializationCallback(false)
{
  BuildComponents();
  SetIndex();
}

CResultTree::CScalarResult::CScalarResult(const QString& sName, unsigned int uLabelId, CResultGroup& group, TResultType result_type)
  : IResult(sName, group), m_result_type(result_type), m_uLabel(uLabelId), m_pVolumeAverage(0), m_bNeedTensorParallelInitializationCallback(false)
{
  BuildComponents();
  SetIndex();
}

void CResultTree::CScalarResult::SetIndex()
{
  switch(m_result_type)
  {
  case PORE_PRESSURE:
    Index( RC_PORE_PRESSURE );
    break;
  case PORE_PRESSURE_CHANGE:
    Index( RC_PORE_PRESSURE_CHANGE );
    break;
  case FRACTURE_MATRIX_PRESSURE:
    Index( RC_FRACTURE_MATRIX_PRESSURE );
    break;
  case FRACTURE_MATRIX_PRESSURE_CHANGE:
    Index( RC_FRACTURE_MATRIX_PRESSURE_CHANGE );
    break;
  case SHTOTAZIMUTH: // wjrx mantis 2870
    Index(RC_SHTOTAZIMUTH);
    break;
  case SHTOT_SvTOT: // wjrx mantis 2870
    Index(RC_SHTOT_SvTOT);
    break;
  case ShTOT_SvTOT: // wjrx mantis 2870
    Index(RC_ShTOT_SvTOT);
    break;
  case TEMPERATURE:
    Index( RC_TEMPERATURE );
    break;
  case TEMPERATURE_CHANGE:
    Index( RC_TEMPERATURE_CHANGE );
    break;
  case CONSOL:
    Index( RC_CONSOL );
    break;
  case CONSOL_CHANGE:
    Index( RC_CONSOL_CHANGE );
    break;
  case POROSITY:
    Index( RC_POROSITY );
    break;
  case POROSITY_CHANGE:
    Index( RC_POROSITY_CHANGE );
    break;
  case SHEAR_CAPACITY:
    Index(  RC_SHEAR_CAPACITY );
    break;
  case COMPACTION_CAPACITY:
    Index(  RC_COMPACTION_CAPACITY );
  case GAMMA_VERTICAL:
    Index(  RC_GAMMA_VERTICAL );
    break;
  case GAMMA_MAX:
    Index( RC_GAMMA_MAX );
    break;
  case GAMMA_MED:
    Index( RC_GAMMA_MED );
    break;
  case GAMMA_MIN:
    Index( RC_GAMMA_MIN );
    break;
  case GAMMA_VOLUME:
    Index( RC_GAMMA_VOLUME );
    break;
  case VOLUME_AVERAGE:
    Index( RC_VOLUME_AVERAGE );
    break;
  case THICKNESS:
    Index(RC_THICKNESS);
    break;
  case THICKNESS_CHANGE:
    Index(RC_THICKNESS_CHANGE);
    break;
  case APERTURE_HI:
    Index(RC_APERTURE_HI);
    break;
  case APERTURE_ME:
    Index(RC_APERTURE_ME);
    break;
  case APERTURE_LO:
    Index(RC_APERTURE_LO);
    break;
  case APERTURE_HI_CHANGE:
    Index(RC_APERTURE_HI_CHANGE);
    break;
  case APERTURE_ME_CHANGE:
    Index(RC_APERTURE_ME_CHANGE);
    break;
  case APERTURE_LO_CHANGE:
    Index(RC_APERTURE_LO_CHANGE);
    break;
  case PERMEABILITY_MULTIPLIER_HI:
    Index(RC_PERMEABILITY_MULTIPLIER_HI);
    break;
  case PERMEABILITY_MULTIPLIER_ME:
    Index(RC_PERMEABILITY_MULTIPLIER_ME);
    break;
  case PERMEABILITY_MULTIPLIER_LO:
    Index(RC_PERMEABILITY_MULTIPLIER_LO);
    break;
  case PLASTIC_ENERGY:
    Index(RC_PLASTIC_ENERGY);
    break;
  case PORE_PRESSURE_GRADIENT:
    Index(RC_PORE_PRESSURE_GRADIENT);
    break;
  case TEMPERATURE_GRADIENT:
    Index(RC_TEMPERATURE_GRADIENT);
    break;
  case PORE_PRESSURE_INPUT:
    Index(RC_PORE_PRESSURE_INPUT);
    break;
  case PORE_PRESSURE_CHANGE_INPUT:
    Index(RC_PORE_PRESSURE_CHANGE_INPUT);
    break;
  case PORE_PRESSURE_GRADIENT_INPUT:
    Index(RC_PORE_PRESSURE_GRADIENT_INPUT);
    break;
  case TEMPERATURE_INPUT:
    Index(RC_TEMPERATURE_INPUT);
    break;
  case TEMPERATURE_CHANGE_INPUT:
    Index(RC_TEMPERATURE_CHANGE_INPUT);
    break;
  case TEMPERATURE_GRADIENT_INPUT:
    Index(RC_TEMPERATURE_GRADIENT_INPUT);
    break;
  case VOLUMETRIC_STRAIN_INPUT:
    Index(RC_VOLUMETRIC_STRAIN_INPUT);
    break;
  case VOLUMETRIC_STRAIN_CHANGE_INPUT:
    Index(RC_VOLUMETRIC_STRAIN_CHANGE_INPUT);
    break;
  case NORMAL_STRAIN_INPUT:
    Index(RC_NORMAL_STRAIN_INPUT);
    break;
  case LATERAL_STRAIN_INPUT:
    Index(RC_LATERAL_STRAIN_INPUT);
    break;
  default:
    assert(false);
    break;
  };
}

CResultTree::CScalarResult::TResultType CResultTree::CScalarResult::ResultType() const
{
  return m_result_type;
}

QString CResultTree::CScalarResult::ExportLabel(int /*nComponent*/) const
{
  return getStringTableEntry(m_uLabel);
}

bool CResultTree::CScalarResult::CanMap(const COpenGLNode& node, int nRegister) const
{
  if((m_result_type == PORE_PRESSURE ||
      m_result_type == PORE_PRESSURE_CHANGE ||
      m_result_type == PORE_PRESSURE_INPUT ||
      m_result_type == PORE_PRESSURE_CHANGE_INPUT))
  {
    if(dynamic_cast<const CNonMeshedSurface*>(&node))
      return true;

    const CHorizonBase* pFault = dynamic_cast<const CHorizonBase*>(&node);
    if(pFault && pFault->Slip())
      return false;
  }

  return CanMapOnVolumeElements(node, nRegister);
}

bool CResultTree::CScalarResult::OnDefined(const IResultComponent& component) const
{
  if((m_result_type == PORE_PRESSURE_INPUT            ) ||
     (m_result_type == PORE_PRESSURE_CHANGE_INPUT     ) ||
     (m_result_type == PORE_PRESSURE_GRADIENT_INPUT   ) ||
     (m_result_type == TEMPERATURE_INPUT              ) ||
     (m_result_type == TEMPERATURE_CHANGE_INPUT       ) ||
     (m_result_type == TEMPERATURE_GRADIENT_INPUT     ) ||
     (m_result_type == NORMAL_STRAIN_INPUT            ) ||
     (m_result_type == LATERAL_STRAIN_INPUT           ) ||
     (m_result_type == VOLUMETRIC_STRAIN_INPUT        ) ||
     (m_result_type == VOLUMETRIC_STRAIN_CHANGE_INPUT ))
  {
    return true;
  }

  return IResult::OnDefined(component);
}

void CResultTree::CScalarResult::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(m_pVolumeAverage)
    m_pVolumeAverage->ClearMap();
  IResult::OnNeighbourModified(node, uHint);
}


bool CResultTree::CScalarResult::NeedParallelInitializationCallback() const
{
  switch (ResultType())
  {
  case THICKNESS_CHANGE:
  case THICKNESS:
  case VOLUME_AVERAGE:
    return true;
  default:
    return m_bNeedTensorParallelInitializationCallback;
  }
}




bool CResultTree::CScalarResult::PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *pValueComponent)
{
  CModelBase *pModel = &static_cast<CModelBase&>(Model());
  assert(dynamic_cast<const IResultComponent *>(pValueComponent));
  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &pModel->ResultRegister();

  CValueMapper *vm = CValueMapper::instance();

  m_bNeedTensorParallelInitializationCallback = false;

  bool retval = true;
  bool change = false;

  switch (ResultType())
  {
  case SHTOTAZIMUTH:
  case SHTOT_SvTOT:
  case ShTOT_SvTOT:
    // TotalStress
    // calcValues
    if (!vm->PrepareMappingStress(*pModel, *pComponent, *pRegister, true))
      retval = false;
    break;
  case SHEAR_CAPACITY:
    // ShearCapacityMC
    if (!vm->PrepareMappingStress(*pModel, *pComponent, *pRegister, false))
      retval = false;

    if (!vm->PrepareMappingMaterial(*pModel, pElementSet, pComponent->Stage(), CValueMapper::CHECK_CEMENT))
      retval = false;
    break;
  case COMPACTION_CAPACITY:
    // CompactionCapacity
    if (!vm->PrepareMappingStress(*pModel, *pComponent, *pRegister, false))
      retval = false;

    if (!vm->PrepareMappingIndex(CI_PRECON, *pModel, *pComponent, *pRegister, false))
      retval = false;

    if (!vm->PrepareMappingMaterial(*pModel, pElementSet, pComponent->Stage(), CValueMapper::CHECK_CEMENT))
      retval = false;
    break;
  case PORE_PRESSURE_CHANGE:
    // PorePressure Di
    change = true;
    /* Fall-through */
  case PORE_PRESSURE:
  case PORE_PRESSURE_GRADIENT:
    // PorePressure
    if (!vm->PrepareMappingPorePressure(*pModel, *pComponent, *pRegister, pElementSet, change))
      retval = false;
    break;
  case PORE_PRESSURE_CHANGE_INPUT:
    // PorePressureInput Di
    if (!vm->PrepareMappingComponent(*pModel, pElementSet, pRegister->DepletionStageEntry().MarkedAsInitialStage(), CValueMapper::MAP_PRESSURE))
      retval = false;
    /* Fall-through */
  case PORE_PRESSURE_INPUT:
  case PORE_PRESSURE_GRADIENT_INPUT:
    // PorePressureInput
    if (!vm->PrepareMappingComponent(*pModel, pElementSet, pComponent->Stage(), CValueMapper::MAP_PRESSURE))
      retval = false;
    break;
  case FRACTURE_MATRIX_PRESSURE_CHANGE:
    // FractureMatrixPressure Di
    /* Fall-through */
  case FRACTURE_MATRIX_PRESSURE:
    // FractureMatrixPressure
    // nothing to do here
    break;
  case TEMPERATURE_CHANGE:
    // Temperature Di
    change = true;
    /* Fall-through */
  case TEMPERATURE:
  case TEMPERATURE_GRADIENT:
    // Temperature
    if (!vm->PrepareMappingIndex(CI_TEMPERATURE, *pModel, *pComponent, *pRegister, change))
      retval = false;
    break;
  case TEMPERATURE_CHANGE_INPUT:
    // TemperatureInput Di
    if (!vm->PrepareMappingComponent(*pModel, pElementSet, pRegister->DepletionStageEntry().MarkedAsInitialStage(), pComponent->AnalysisType().AnalysisType() != CAnalysisType::AT_HEAT ? CValueMapper::MAP_TEMPERATURE : CValueMapper::MAP_TEMPERATURE_HEAT))
      retval = false;
    /* Fall-through */
  case TEMPERATURE_INPUT:
  case TEMPERATURE_GRADIENT_INPUT:
    // TemperatureInput
    if (!vm->PrepareMappingComponent(*pModel, pElementSet, pComponent->Stage(), pComponent->AnalysisType().AnalysisType() != CAnalysisType::AT_HEAT ? CValueMapper::MAP_TEMPERATURE : CValueMapper::MAP_TEMPERATURE_HEAT))
      retval = false;
    break;
  case CONSOL_CHANGE:
    // Consolidation Di
    change = true;
    /* Fall-through */
  case CONSOL:
    // Consolidation
    if (!vm->PrepareMappingIndex(CI_PRECON, *pModel, *pComponent, *pRegister, change))
      retval = false;
    break;
  case POROSITY_CHANGE:
    // Porosity Di
    change = true;
    /* Fall-through */
  case POROSITY:
    // Porosity
    if (!vm->PrepareMappingStress(*pModel, *pComponent, *pRegister, change))
      retval = false;

    if (!vm->PrepareMappingStrain(*pModel, *pComponent, *pRegister))
      retval = false;

    if (!vm->PrepareMappingMaterial(*pModel, pElementSet, pComponent->Stage(), CValueMapper::CHECK_CEMENT))
      retval = false;
    break;
  case THICKNESS_CHANGE:
    // Thickness
    /* Fall-through */
  case THICKNESS:
    // Thickness
    {
      // Thickness uses the Displacement vectorvalueset, and along the line the existence of results is checked for D0, hence we need it
      int c = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_DISPLA_X);

      if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c, 3))
        retval = false;
 
      c = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_DISPLA_X);

      if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c, 3))
        retval = false;

      if (!vm->PrepareMappingSurfaces(*pModel, pElementSet, pComponent->Stage()))
        retval = false;
    }
    break;
  case VOLUME_AVERAGE:
    // GammaValues
    if (!vm->PrepareMappingGammaValues(*pModel, *pComponent, *pRegister, pElementSet))
      retval = false;
    break;
  case PERMEABILITY_MULTIPLIER_HI:
  case PERMEABILITY_MULTIPLIER_ME:
  case PERMEABILITY_MULTIPLIER_LO:
    // permeabilityMultiplierValues
    //if (!vm->PrepareMappingMaterial(*pModel, pElementSet, pRegister->DepletionStageEntry().MarkedAsInitialStage()))
      //retval = false;
    /* Fall-through */
  case APERTURE_HI:
  case APERTURE_ME:
  case APERTURE_LO:
  case APERTURE_HI_CHANGE:
  case APERTURE_ME_CHANGE:
  case APERTURE_LO_CHANGE:
    // ApertureValues
    if (!vm->PrepareMappingStress(*pModel, *pComponent, *pRegister, false))
      retval = false;

    if (!vm->PrepareMappingStrain(*pModel, *pComponent, *pRegister))
      retval = false;

    if (!vm->PrepareMappingMaterial(*pModel, pElementSet, pComponent->Stage(), CValueMapper::MARKED_AS_INITIAL | CValueMapper::CHECK_CEMENT | CValueMapper::CHECK_MARKED_VS_INITIAL))
      retval = false;
    break;
  case PLASTIC_ENERGY:
    // PlasticEnergy
    if(!vm->PrepareMappingPlasticEnergy(*pModel, *pComponent, *pRegister))
      retval = false;
    break;
  case VOLUMETRIC_STRAIN_CHANGE_INPUT:
    // VolumetricStrain Di
    if (!pModel->DepletionStageEntry().MarkedAsInitialStage().Initial())
    {
      if (!vm->PrepareMappingComponent(*pModel, pElementSet, pRegister->DepletionStageEntry().MarkedAsInitialStage(), CValueMapper::MAP_VOLUMETRIC_STRAIN))
        retval = false;
    }
    /* Fall-through */
  case VOLUMETRIC_STRAIN_INPUT:
    // VolumetricStrain
    if (!vm->PrepareMappingComponent(*pModel, pElementSet, pComponent->Stage(), CValueMapper::MAP_VOLUMETRIC_STRAIN))
      retval = false;
    break;
  case NORMAL_STRAIN_INPUT:
  case LATERAL_STRAIN_INPUT:
    break;
  default:
    // GammaValues
    if (!vm->PrepareMappingGammaValues(*pModel, *pComponent, *pRegister, pElementSet))
      retval = false;
    break;
  }

  m_bNeedTensorParallelInitializationCallback = vm->EncounteredStrainOrStress();

  return retval;
}



CResultTree::CScalarResult::CTensorParallelInitializationCallback::CTensorParallelInitializationCallback(CModelBase &model)
  : ITensorGroupTemplate<CStrainTensor>::CParallelInitializationCallback(model)
  , m_cb(model)
  , m_ScalarVectors(8)
  , m_StressTensorVectors(2)
{
  for (size_t i = 0; i < m_ScalarVectors.size(); ++i)
    m_ScalarVectors[i].reserve(16);
  for (size_t i = 0; i < m_StressTensorVectors.size(); ++i)
    m_StressTensorVectors[i].reserve(16);
}
 
CResultTree::CScalarResult::CTensorParallelInitializationCallback::~CTensorParallelInitializationCallback()
{
}

_ITensorResultCache& CResultTree::CScalarResult::CTensorParallelInitializationCallback::GetCache(const ITensorGroup *caller, const IResultComponent& component) const
{
  if (dynamic_cast<const IStrainTensorGroup *>(caller))
    return ITensorGroupTemplate<CStrainTensor>::CParallelInitializationCallback::GetCache(caller, component);
  return m_cb.GetCache(caller, component);
}

template<class Value>
void CResultTree::CScalarResult::CTensorParallelInitializationCallback::initVector(std::vector<std::vector<Value> >& vectors, size_t index, size_t size)
{
  if (size != vectors[index].size())
    vectors[index].resize(size);
  for (size_t i = 0; i < size; ++i)
    vectors[index][i].Invalidate();
}

IValueDomainScalar::TValueVec *CResultTree::CScalarResult::CTensorParallelInitializationCallback::getScalarVector(size_t index, size_t size)
{
   initVector<geo::CValue>(m_ScalarVectors, index, size);
   return &m_ScalarVectors[index];
}

CStressTensorValueSet::TValueVec *CResultTree::CScalarResult::CTensorParallelInitializationCallback::getStressTensorVector(size_t index, size_t size)
{
  initVector<CStressTensor>(m_StressTensorVectors, index, size);
  return &m_StressTensorVectors[index];
}


//////////////////////////////////////////////////////////////////////
// The implementation of CFaultStressResult::CShearStress
//////////////////////////////////////////////////////////////////////
CResultTree::CFaultShearStress::CFaultShearStress(unsigned int uFileIndex, 
                          unsigned int uLabelId, 
                          CResultGroup& group)
: CVectorResult(IDS_RG_SHEAR_STRESS, &CResultRegister::FaultShearStress, group, false), m_uLabelId(uLabelId)
{
  BuildComponents();
  Index(uFileIndex);
}

bool CResultTree::CFaultShearStress::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= VectorExportLabel().toStdString();

  return validateName(resultInfo, name, label, VectorResultComponentList(),
    "_L|_M|_C|_N");
}


bool CResultTree::CFaultShearStress::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return (antype == CAnalysisType::AT_LINEAR  ||
          antype == CAnalysisType::AT_MIXTURE ||
          antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
          (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

double CResultTree::CFaultShearStress::ConvertToField(const double &dValue) const
{
  return dValue * FF_FACTOR_STRESS;
}

QString CResultTree::CFaultShearStress::ExportLabel(int nComponent) const
{
  return VectorExportLabel() + IVectorResult::ExportLabel(nComponent);
}

QString CResultTree::CFaultShearStress::VectorExportLabel() const
{
  return getStringTableEntry(m_uLabelId);
}

bool CResultTree::CFaultShearStress::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnFaultElements(node, nRegister);
}

bool CResultTree::CFaultShearStress::isFaultResult() const
{
  return true;
}

QString CResultTree::CFaultShearStress::UnitName(UNIT unit) const
{
  QString sUnit;
  if(unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
  return sUnit;
}

bool CResultTree::CFaultShearStress::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase *pValueComponent)
{
  bool retval = true;

  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &static_cast<CModelBase&>(Model()).ResultRegister();

  int c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);
  int c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_STRESS_XX);

  if (Change() && !pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 3))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 3))
    retval = false;

  return retval;
}


///// CFaultShearStressLengthChangeComposite

CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComposite(unsigned int /*uName*/, unsigned int uLabelId, CResultGroup& group)
: CVectorResult(IDS_RG_SHEAR_STRESS, &CResultRegister::FaultShearStress, group, false),
  m_uLabelId(uLabelId)
{
  BuildComponents();
}

CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComposite(const QString& /*sName*/, unsigned int uLabelId, CResultGroup& group)
: CVectorResult(IDS_RG_SHEAR_STRESS, &CResultRegister::FaultDisplacement, group, false),
  m_uLabelId(uLabelId)
{
  BuildComponents();
}

CResultTree::CFaultShearStressLengthChangeComposite::~CFaultShearStressLengthChangeComposite()
{
}

bool CResultTree::CFaultShearStressLengthChangeComposite::OnDefined(const IResultComponent& component) const
{
  if(!CVectorResult::OnDefined(component))
    return false;

  if(dynamic_cast<const CFaultShearStressLengthChangeComponent*>(&component))
  {
    CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    return (component.Stage().Index() > dep_entry.MarkedAsInitialStage().Index());
  }

  return true;
}

unsigned int CResultTree::CFaultShearStressLengthChangeComposite::IconId() const
{
  return IDI_RESULT;
}

bool CResultTree::CFaultShearStressLengthChangeComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= VectorExportLabel().toStdString();

  return validateName(resultInfo, name, label, "", "_L|_M|_C|_N");
}


bool CResultTree::CFaultShearStressLengthChangeComposite::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) && antype != CAnalysisType::AT_HEAT;
}

void CResultTree::CFaultShearStressLengthChangeComposite::BuildComponent(const CDepletionStage &stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
    new CFaultShearStressLengthChangeComponent(IDS_RC_LENGTH, *this, stage, antype, nRegister);
}

bool CResultTree::CFaultShearStressLengthChangeComposite::CanMap(const COpenGLNode& node, int nRegister) const
{
  return CanMapOnFaultElements(node, nRegister);
}

bool CResultTree::CFaultShearStressLengthChangeComposite::isFaultResult() const
{
  return true;
}

double CResultTree::CFaultShearStressLengthChangeComposite::ConvertToField(const double& dValue) const
{
  return dValue * FF_FACTOR_STRESS;
}

QString CResultTree::CFaultShearStressLengthChangeComposite::ExportLabel(int /*nComponent*/) const
{
  return getStringTableEntry(m_uLabelId);
}

QString CResultTree::CFaultShearStressLengthChangeComposite::VectorExportLabel() const
{
  return getStringTableEntry(m_uLabelId);
}

QString CResultTree::CFaultShearStressLengthChangeComposite::UnitName(UNIT unit) const
{
  QString sUnit;
  if(unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
  return sUnit;
}

bool CResultTree::CFaultShearStressLengthChangeComposite::PrepareMapping(const geo::IElementSet* /*pElementSet*/, const IValueComponentBase *pValueComponent)
{
  bool retval = true;

  IResultComponent *pComponent = const_cast<IResultComponent *>(static_cast<const IResultComponent *>(pValueComponent));
  CResultRegister *pRegister = &static_cast<CModelBase&>(Model()).ResultRegister();

  int c0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);
  int d0 = pRegister->ColumnNumber(pComponent->AnalysisType(), pRegister->DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRAIN_XX);
  int c1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_STRESS_XX);
  int d1 = pRegister->ColumnNumber(pComponent->AnalysisType(), pComponent->Stage().Index(), CI_STRAIN_XX);

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c0, 3))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(d0, 3))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(c1, 3))
    retval = false;

  if (!pRegister->Cache().ActiveCacher().StartCriticalSection(d1, 3))
    retval = false;

  return retval;
}

///// CFaultShearStressLengthChangeComponent

CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::CFaultShearStressLengthChangeComponent(unsigned int uName,
                                                                                                               CFaultShearStressLengthChangeComposite& parent,
                                                                                                               const CDepletionStage& stage,
                                                                                                               const CAnalysisType& antype,
                                                                                                               int nRegister)
: IVectorResult::CVectorComponent(uName, parent, (VECTOR_COMPONENT)0, stage, antype, nRegister),
  m_parent(parent)
{
}

geo::CValue CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::ValuePoint(const geo::IPoint& pt, const IValueComponentBase::UNIT unit) const
{
  TSourceComponentPair prSources = SourceComponents();
  if(prSources.first == 0)
    return geo::CValue(); // invalid

  const IResultComponent& source_comp = *prSources.first;
  const IResultComponent& ini_source_comp = *prSources.second;

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFaultShearStress& group = model.ResultTree().FaultShearStress();

  const geo::IVector& vecStage = group.VectorPoint(pt, source_comp);
  const geo::IVector& vecIni   = group.VectorPoint(pt, ini_source_comp);

  double dValue = vecStage.Length() - vecIni.Length();
  if(unit == IQuantityDouble::FIELD_UNIT)
    dValue *= FF_FACTOR_STRESS;

  return geo::CValue(dValue);
}

void CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, IResultComponent::TMapType map_type, IValueComponentBase::UNIT unit) const
{
  TSourceComponentPair prSources = SourceComponents();
  if(prSources.first == 0)
    return; // invalid

  const IResultComponent& source_comp = *prSources.first;
  const IResultComponent& ini_source_comp = *prSources.second;

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFaultShearStress& group = model.ResultTree().FaultShearStress();

  int i;
  for(i = 0; i < values.size(); ++i)
  {
    const geo::IVector& vecStage = group.VectorElement(elm, i, map_type, source_comp);
    const geo::IVector& vecIni   = group.VectorElement(elm, i, map_type, ini_source_comp);

    double dValue = vecStage.Length() - vecIni.Length();
    if(unit == IQuantityDouble::FIELD_UNIT)
      dValue *= FF_FACTOR_STRESS;

    values[i] = geo::CValue(dValue);
  }
}

const CResultTree::CFaultShearStressLengthChangeComposite& CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::Composite() const
{
  return m_parent;
}

CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::TSourceComponentPair
CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::SourceComponents() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFaultShearStress& source_composite = model.ResultTree().FaultShearStress();

  CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
  const CDepletionStage& ini_stage = dep_entry.MarkedAsInitialStage();
  const CDepletionStage& stage = Stage();

  if(ini_stage.Index() >= stage.Index())
  {
    const IResultComponent* lhs = 0;
    const IResultComponent* rhs = 0;

    return TSourceComponentPair(lhs, rhs); // result invalid
  }

  const IResultComponent& source_comp = *source_composite.ResultComponent(stage, AnalysisType(), RegisterIndex(), VC_LENGTH);
  const IResultComponent& ini_source_comp = *source_composite.ResultComponent(ini_stage, AnalysisType(), RegisterIndex(), VC_LENGTH);

  return TSourceComponentPair(&source_comp, &ini_source_comp);
}

bool CResultTree::CFaultShearStressLengthChangeComposite::CFaultShearStressLengthChangeComponent::isFaultResult() const
{
  return true;
}


///// CRTCIStrainResult

CResultTree::CRTCIStrainResult::CRTCIStrainResult(CResultGroup& group)
: IResult("RTCI Strain", group)
{
  BuildComponents();
}

QString CResultTree::CRTCIStrainResult::ExportLabel(int /*nComponent*/) const
{
  return "RTCIStrain";
}

bool CResultTree::CRTCIStrainResult::CanMap(const COpenGLNode& node, int /*nRegister*/) const
{
  return dynamic_cast<const CWellCasingRTCI*>(&node) != 0;
}

bool CResultTree::CRTCIStrainResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= ExportLabel(0).toStdString();

  return validateName(resultInfo, name, label, "", "_L|_M|_C|_N");
}


bool CResultTree::CRTCIStrainResult::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int /*nRegister*/) const
{
  return stage.isAfter(stage.Entry().MarkedAsInitialStage()) &&
        (antype == CAnalysisType::AT_LINEAR  ||
         antype == CAnalysisType::AT_MIXTURE ||
         antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ||
         (antype == CAnalysisType::AT_NONLIN && stage.OutputType() != CDepletionStage::NONE));
}

void CResultTree::CRTCIStrainResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
    new CRTCIStrainResultComponent(*this, stage, antype, nRegister);
}


///// CRTCIStrainResultComponent

CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::CRTCIStrainResultComponent(CRTCIStrainResult& result, const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
: IResultComponent("", result, stage, antype, nRegister, 0)
{
}

unsigned int CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::IconId() const
{
  return IDI_RESULT;
}

unsigned int CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::TypeId() const
{
  return 0;
}

QString CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::UnitName(const UNIT /*unit*/) const
{
  return getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
}

geo::CValue CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::ValuePoint(const geo::IPoint& /*point*/, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  return geo::CValue();
}

void CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType /*map_type*/, UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CStrainTensorValueSet& strain = model.ResultRegister(RegisterIndex()).TotalStrain(Stage(), AnalysisType(), false);
  assert(values.size() == elm.NrOfPoints());
//  values.resize(elm.NrOfPoints());
  const CWellCasingRTCI::CSegmentInfo* pRtciSegment = dynamic_cast<const CWellCasingRTCI::CSegmentInfo*>(&elm);
  if(pRtciSegment)
  {
    values[0] = pRtciSegment->AxialStrain(pRtciSegment->Point(0), strain);
    values[1] = pRtciSegment->AxialStrain(pRtciSegment->Point(1), strain);
  }
}

bool CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::PrepareMapping(const geo::IElementSet* /*pElementSet*/)
{
  bool retval = true;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  return retval;
}

void CResultTree::CRTCIStrainResult::CRTCIStrainResultComponent::FinishMapping()
{
}


//////////////////////////////////////////////////////////////////////
// Implementation of result tree
//////////////////////////////////////////////////////////////////////

CResultTree::CResultTree(CModelBase& model)
: CResultGroup(IDS_RG_ROOT, model)
, m_pRTCIStrain(0)
, m_pParentStressInput(0)
, m_pBoundaryDisplacementInput(0)
, m_pBoundaryPressureInput(0)
{
  bool isWellModel = dynamic_cast<IWellModel *>(&Model()) != 0;

  // Input
  CResultGroup* pInput = new CResultGroup(IDS_RG_INPUT, *this);
  CResultGroup* pPressureInput = new CResultGroup(IDS_RG_PORE_PRESSURE, *pInput);
  m_pPorePressureInput = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_PORE_PRESSURE, *pPressureInput, CScalarResult::PORE_PRESSURE_INPUT);
  m_pPorePressureChangeInput = new CScalarResult(IDS_RG_CHANGE, IDS_ET_PORE_PRESSURE_CHANGE, *pPressureInput, CScalarResult::PORE_PRESSURE_CHANGE_INPUT);
  m_pPorePressureGradientInput = new CScalarResult(IDS_RG_GRADIENT, IDS_ET_PORE_PRESSURE_GRADIENT, *pPressureInput, CScalarResult::PORE_PRESSURE_GRADIENT_INPUT);

  CResultGroup* pTemperatureInput = new CResultGroup(IDS_RG_TEMPERATURE, *pInput);
  m_pTemperatureInput = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_TEMPERATURE, *pTemperatureInput, CScalarResult::TEMPERATURE_INPUT);
  m_pTemperatureChangeInput = new CScalarResult(IDS_RG_CHANGE, IDS_ET_TEMPERATURE_CHANGE, *pTemperatureInput, CScalarResult::TEMPERATURE_CHANGE_INPUT);
  m_pTemperatureGradientInput = new CScalarResult(IDS_RG_GRADIENT, IDS_ET_TEMPERATURE_GRADIENT, *pTemperatureInput, CScalarResult::TEMPERATURE_GRADIENT_INPUT);

  CResultGroup* pStrainInput = new CResultGroup(IDS_RG_STRAIN, *pInput);

  CResultGroup* pVolumetricStrainInput = new CResultGroup(IDS_VALUENAME_VOLUMETRICSTRAIN, *pStrainInput);
  m_pVolumetricStrainInput = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_VOLUMETRICSTRAIN, *pVolumetricStrainInput, CScalarResult::VOLUMETRIC_STRAIN_INPUT);
  m_pVolumetricStrainChangeInput = new CScalarResult(IDS_RG_CHANGE, IDS_ET_VOLUMETRICSTRAIN_CHANGE, *pVolumetricStrainInput, CScalarResult::VOLUMETRIC_STRAIN_CHANGE_INPUT);

  CResultGroup* pNormalStrainInput = new CResultGroup(IDS_VALUENAME_NRMSTRAIN,
    *pStrainInput);
  CResultGroup* pLateralStrainInput = new CResultGroup(IDS_VALUENAME_LATSTRAIN,
    *pStrainInput);

  new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_NRMSTRAIN, *pNormalStrainInput,
    CScalarResult::NORMAL_STRAIN_INPUT);
  new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_LATSTRAIN, *pLateralStrainInput,
    CScalarResult::LATERAL_STRAIN_INPUT);

  CResultGroup* pFaultsInput = new CResultGroup(IDS_RG_FAULT, *pInput);
  CResultGroup *pFaultPressureInput = new CResultGroup(IDS_RG_PORE_PRESSURE, *pFaultsInput);
  m_pFaultPressureInput = new CFaultScalarResult(IDS_RG_ABSOLUTE, IDS_ET_FAULT_PRESSURE, *pFaultPressureInput, CFaultScalarResult::PORE_PRESSURE_INPUT);
  m_pFaultPressureChangeInput = new CFaultScalarResult(IDS_RG_CHANGE, IDS_ET_FAULT_PRESSURE_CHANGE, *pFaultPressureInput, CFaultScalarResult::PORE_PRESSURE_CHANGE_INPUT);

  if (isWellModel)
  {
    CResultGroup *pBoundaryInput = new CResultGroup(IDS_RG_BOUNDARY_INPUT, *pInput);
    m_pBoundaryPressureInput = new CFaultScalarResult(IDS_RG_BOUNDARY_PRESSURE, IDS_ET_BOUNDARY_PRESSURE, *pBoundaryInput, CFaultScalarResult::BOUNDARY_PRESSURE_INPUT);
    m_pBoundaryDisplacementInput = new CFaultScalarResult(IDS_RG_BOUNDARY_DISPLACEMENT, IDS_ET_BOUNDARY_DISPLACEMENT, *pBoundaryInput, CFaultScalarResult::BOUNDARY_DISPLACEMENT_INPUT);

    m_pParentStressInput = new CParentStressInputResult(IDS_RG_PARENT_STRESS, *pInput);
    //CResultGroup *pBoundaryInput = new CResultGroup(IDS_RG_BOUNDARY_INPUT, *pInput);

  }

  // Output
  CResultGroup* pOutput = new CResultGroup(IDS_RG_OUTPUT, *this);

  // Create displacement group
  m_pDisplacement = new CDisplacementResult(IDS_RG_DISPLACEMENT, *pOutput);

  // Fill the StressGroup
  CResultGroup *pStressGroup = new CResultGroup(IDS_RG_STRESSES, *pOutput);

  // First we fill the absolute stress group with shit
  CResultGroup *pAbsoluteStressGroup = new CResultGroup(IDS_RG_ABSOLUTE, *pStressGroup);
  m_pTotalStress = new CTotalStressResult(IDS_RG_TOTAL, *pAbsoluteStressGroup);
  m_pEffectiveStress = new CEffectiveStressResult(IDS_RG_EFFECTIVE, *pAbsoluteStressGroup);
  // Now the gamma's
  CResultGroup *pGammaGroup = new CResultGroup(IDS_RG_GAMMA, *pStressGroup);
  m_pGammaVertical= new CScalarResult(IDS_RC_GAMMA_VERTICAL, IDS_ET_GAMMA_VERTICAL, *pGammaGroup, CScalarResult::GAMMA_VERTICAL);
  m_pGammaMax= new CScalarResult(IDS_RC_GAMMA_MAX, IDS_ET_GAMMA_MAX, *pGammaGroup, CScalarResult::GAMMA_MAX);
  m_pGammaMed= new CScalarResult(IDS_RC_GAMMA_MED, IDS_ET_GAMMA_MED, *pGammaGroup, CScalarResult::GAMMA_MED);
  m_pGammaMin= new CScalarResult(IDS_RC_GAMMA_MIN, IDS_ET_GAMMA_MIN, *pGammaGroup, CScalarResult::GAMMA_MIN);
  m_pGammaVolume= new CScalarResult(IDS_RC_GAMMA_VOLUME, IDS_ET_GAMMA_VOLUME, *pGammaGroup, CScalarResult::GAMMA_VOLUME);
  m_pVolumeAverage = new CScalarResult(IDS_RC_VOLUME_AVERAGE, IDS_RG_GAMMA, *pGammaGroup, CScalarResult::VOLUME_AVERAGE);
  // Shear capacity
  m_pShearCapacity = new CScalarResult(IDS_RC_SHEAR_CAPACITY, IDS_ET_SHEAR_CAPACITY, *pStressGroup, CScalarResult::SHEAR_CAPACITY);
  m_pCompactionCapacity = new CScalarResult(IDS_RC_COMPACTION_CAPACITY, IDS_ET_COMPACTION_CAPACITY, *pStressGroup, CScalarResult::COMPACTION_CAPACITY);

  // Second we make something like a change of stress group
  CResultGroup *pChangeStressGroup = new CResultGroup(IDS_RG_CHANGE, *pStressGroup);
  m_pTotalStressChange = new CTotalStressResult(IDS_RG_TOTAL, *pChangeStressGroup, true);
  m_pEffectiveStressChange = new CEffectiveStressResult(IDS_RG_EFFECTIVE, *pChangeStressGroup, true);

  // Preconsolidation
  CResultGroup* pConsolidation = new CResultGroup(IDS_RG_CONSOL, *pStressGroup);
  m_pConsolidation = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_CONSOL, *pConsolidation, CScalarResult::CONSOL);
  m_pConsolidationChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_CONSOL_CHANGE, *pConsolidation, CScalarResult::CONSOL_CHANGE);

  // Stress Ratios    wjrx mantis 2870
  CResultGroup* pStressRatios = new CResultGroup(IDS_RG_STRESSRATIOS, *pStressGroup);
  m_pSHtotAzimuth = new CScalarResult(IDS_RC_SHTOTAZIMUTH, IDS_ET_SHTOTAZIMUTH, *pStressRatios, CScalarResult::SHTOTAZIMUTH);
  m_pSHtotSvtot =   new CScalarResult(IDS_RC_SHTOT_SVTOT, IDS_ET_SHTOT_SVTOT, *pStressRatios, CScalarResult::SHTOT_SvTOT);
  m_pShtotSvtot =   new CScalarResult(IDS_RC_S_HTOT_SVTOT, IDS_ET_S_HTOT_SVTOT, *pStressRatios, CScalarResult::ShTOT_SvTOT);

  // Pressure
  CResultGroup *pPressure = new CResultGroup(IDS_RG_PRESSURE, *pOutput);

  // Pore pressure
  CResultGroup *pPorePressure = new CResultGroup(IDS_RG_PORE_PRESSURE, *pPressure);
  m_pPorePressure = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_PORE_PRESSURE, *pPorePressure, CScalarResult::PORE_PRESSURE);
  m_pPorePressureChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_PORE_PRESSURE_CHANGE, *pPorePressure, CScalarResult::PORE_PRESSURE_CHANGE);
  m_pPorePressureGradient= new CScalarResult(IDS_RG_GRADIENT, IDS_ET_PORE_PRESSURE_GRADIENT, *pPorePressure, CScalarResult::PORE_PRESSURE_GRADIENT);

  // Fracture Matrix Pressure
  CResultGroup *pFractureMatrixPressure = new CResultGroup(IDS_RG_FRACTURE_MATRIX_PRESSURE, *pPressure);
  m_pFractureMatrixPressure= new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_FRACTURE_MATRIX_PRESSURE, *pFractureMatrixPressure, CScalarResult::FRACTURE_MATRIX_PRESSURE);
  m_pFractureMatrixPressureChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_FRACTURE_MATRIX_PRESSURE_CHANGE, *pFractureMatrixPressure, CScalarResult::FRACTURE_MATRIX_PRESSURE_CHANGE);

  // Temperature
  CResultGroup* pTemperature = new CResultGroup(IDS_RG_TEMPERATURE, *pOutput);
  m_pTemperature = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_TEMPERATURE, *pTemperature, CScalarResult::TEMPERATURE);
  m_pTemperatureChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_TEMPERATURE_CHANGE, *pTemperature, CScalarResult::TEMPERATURE_CHANGE);
  m_pTemperatureGradient= new CScalarResult(IDS_RG_GRADIENT, IDS_ET_TEMPERATURE_GRADIENT, *pTemperature, CScalarResult::TEMPERATURE_GRADIENT);

  // Porosity
  CResultGroup* pPorosity = new CResultGroup(IDS_RG_POROSITY, *pOutput);
  m_pPorosity = new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_POROSITY_RES, *pPorosity, CScalarResult::POROSITY);
  m_pPorosityChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_POROSITY_RES_CHANGE, *pPorosity, CScalarResult::POROSITY_CHANGE);

  // Strain 
  CResultGroup *pStrainGroup = new CResultGroup(IDS_RG_STRAIN, *pOutput);
  m_pTotalStrain = new CTotalStrainResult(IDS_RG_TOTAL, *pStrainGroup);
  m_pPlasticStrain = new CPlasticStrainResult(IDS_RG_PLASTIC, *pStrainGroup);
//  m_pCreepStrain = new CCreepStrainResult(IDS_RG_CREEP, *pStrainGroup);

  CResultGroup* pPlasticEnergyGroup = new CResultGroup(IDS_RG_PLASTIC_ENERGY, *pOutput);
  m_pPlasticEnergy= new CScalarResult(IDS_RG_PLASTIC_ENERGY, IDS_ET_PLASTIC_ENERGY, *pPlasticEnergyGroup, CScalarResult::PLASTIC_ENERGY);

  // Thickness
  CResultGroup* pThicknessGroup = new CResultGroup(IDS_RG_THICKNESS, *pOutput);
  m_pThickness= new CScalarResult(IDS_RG_ABSOLUTE, IDS_ET_THICKNESS, *pThicknessGroup, CScalarResult::THICKNESS);
  m_pThicknessChange= new CScalarResult(IDS_RG_CHANGE, IDS_ET_THICKNESS_CHANGE, *pThicknessGroup, CScalarResult::THICKNESS_CHANGE);
  
  // Fractures result
  CResultGroup* pFractureAperture =
    new CResultGroup(IDS_RG_FRACTURE_APERTURE, *pOutput);
  CResultGroup* pFractureApertureAbs =
    new CResultGroup(IDS_RG_ABSOLUTE, *pFractureAperture);
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_HI,
    m_pApertureHi= new CScalarResult(IDS_APERTURE_HI, IDS_ET_APERTURE_HI,
      *pFractureApertureAbs, CScalarResult::APERTURE_HI)));
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_ME,
    m_pApertureMe= new CScalarResult(IDS_APERTURE_ME, IDS_ET_APERTURE_ME,
      *pFractureApertureAbs, CScalarResult::APERTURE_ME)));
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_LO,
    m_pApertureLo= new CScalarResult(IDS_APERTURE_LO, IDS_ET_APERTURE_LO,
      *pFractureApertureAbs, CScalarResult::APERTURE_LO)));
  CResultGroup* pFractureApertureChange =
    new CResultGroup(IDS_RG_CHANGE, *pFractureAperture);
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_HI_CHANGE,
    m_pApertureHiChange= new CScalarResult(IDS_APERTURE_HI, IDS_ET_APERTURE_HI_CHANGE,
      *pFractureApertureChange, CScalarResult::APERTURE_HI_CHANGE)));
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_ME_CHANGE,
    m_pApertureMeChange= new CScalarResult(IDS_APERTURE_ME, IDS_ET_APERTURE_ME_CHANGE,
      *pFractureApertureChange, CScalarResult::APERTURE_ME_CHANGE)));
  m_pAperture.insert(std::make_pair(CScalarResult::APERTURE_LO_CHANGE,
    m_pApertureLoChange= new CScalarResult(IDS_APERTURE_LO, IDS_ET_APERTURE_LO_CHANGE,
      *pFractureApertureChange, CScalarResult::APERTURE_LO_CHANGE)));
  CResultGroup* pPermeabilityMultiplier =
    new CResultGroup(IDS_RG_PERMEABILITY_MULTIPLIER, *pFractureAperture);
  m_pAperture.insert(std::make_pair(CScalarResult::PERMEABILITY_MULTIPLIER_HI,
    m_pPermeabilityMultiplierHi= new CScalarResult(IDS_PERMEABILITY_MULTIPLIER_HI,
      IDS_ET_PERMEABILITY_MULTIPLIER_HI, *pPermeabilityMultiplier,
      CScalarResult::PERMEABILITY_MULTIPLIER_HI)));
  m_pAperture.insert(std::make_pair(CScalarResult::PERMEABILITY_MULTIPLIER_ME,
    m_pPermeabilityMultiplierMe= new CScalarResult(IDS_PERMEABILITY_MULTIPLIER_ME,
      IDS_ET_PERMEABILITY_MULTIPLIER_ME, *pPermeabilityMultiplier,
      CScalarResult::PERMEABILITY_MULTIPLIER_ME)));
  m_pAperture.insert(std::make_pair(CScalarResult::PERMEABILITY_MULTIPLIER_LO,
    m_pPermeabilityMultiplierLo= new CScalarResult(IDS_PERMEABILITY_MULTIPLIER_LO,
      IDS_ET_PERMEABILITY_MULTIPLIER_LO, *pPermeabilityMultiplier,
      CScalarResult::PERMEABILITY_MULTIPLIER_LO)));

  // Fault result
  CResultGroup *pFaultGroup = new CResultGroup(IDS_RG_FAULT, *pOutput);
  // Create the displacement group
  CResultGroup *pFaultDisplacement = new CResultGroup(IDS_RG_FAULT_DISPLACEMENT, *pFaultGroup);
  m_pFaultDilatation = new CFaultScalarResult(IDS_RC_FAULT_DILATATION, IDS_ET_FAULT_DILATATION, *pFaultDisplacement, CFaultScalarResult::DILATATION);
  m_pFaultDisplacement = new CFaultDisplacementResult(IDS_RG_FAULT_DISPLACEMENT_VECTOR, *pFaultDisplacement);
  // Create the plastic slip group
  CResultGroup* pFaultPlasticSlip = new CResultGroup(IDS_RG_FAULT_PLASTIC_SLIP, *pFaultGroup);
  m_pFaultPlasticSlip = new CFaultPlasticSlipResult(IDS_RG_FAULT_PLASTIC_SLIP_VECTOR, *pFaultPlasticSlip);
  CResultGroup* pFaultPlasticEnergy = new CResultGroup(IDS_RG_FAULT_PLASTIC_ENERGY, *pFaultGroup);
  m_pFaultPlasticEnergy= new CFaultScalarResult(IDS_RG_FAULT_PLASTIC_ENERGY, IDS_ET_FAULT_PLASTIC_ENERGY, *pFaultPlasticEnergy, CFaultScalarResult::PLASTIC_ENERGY);
  // Create the stress group
  CResultGroup *pFaultStress = new CResultGroup(IDS_RG_FAULT_STRESS, *pFaultGroup);
  m_pFaultShearCapacity= new CFaultScalarResult(IDS_RC_SHEAR_CAPACITY, IDS_ET_FAULT_SHEARCAPACITY, *pFaultStress, CFaultScalarResult::SHEAR_CAPACITY);
  CResultGroup *pFaultAbsolute = new CResultGroup(IDS_RG_ABSOLUTE, *pFaultStress);
  m_pFaultTotalNormalStress = new CFaultNormalStress(RC_FAULT_TOTAL_NORMAL_STRESS,
                             IDS_FAULT_TOTAL_NORMAL_STRESS,
                             IDS_ET_FAULT_TOTAL_NORMAL_STRESS,
                             *pFaultAbsolute,
                             true, false );
  m_pFaultEffectiveNormalStress = new CFaultNormalStress(RC_FAULT_EFF_NORMAL_STRESS,
                               IDS_FAULT_EFF_NORMAL_STRESS,
                                 IDS_ET_FAULT_EFF_NORMAL_STRESS,
                                 *pFaultAbsolute,
                                 false, false );
  m_pFaultShearStress = new CFaultShearStress(RC_FAULT_SHEAR_STRESS,
                        IDS_ET_FAULT_SHEAR_STRESS,
                        *pFaultAbsolute);

  CResultGroup *pFaultChange = new CResultGroup(IDS_RG_CHANGE, *pFaultStress);
  m_pFaultTotalNormalStressChange=
    new CFaultNormalStress(RC_FAULT_TOTAL_NORMAL_STRESS_CHANGE, 
               IDS_FAULT_TOTAL_NORMAL_STRESS,
               IDS_ET_FAULT_TOTAL_NORMAL_STRESS,
               *pFaultChange,
               true, true );
  m_pFaultEffectiveNormalStressChange= 
  new CFaultNormalStress(RC_FAULT_EFF_NORMAL_STRESS_CHANGE,
               IDS_FAULT_EFF_NORMAL_STRESS,
                 IDS_ET_FAULT_EFF_NORMAL_STRESS,
                 *pFaultChange,
               false, true );
  m_pFaultShearStresschange= new CFaultShearStressLengthChangeComposite(RC_FAULT_SHEAR_STRESS_CHANGE, IDS_ET_FAULT_SHEAR_STRESS_CHANGE, *pFaultChange);
//  new CFaultShearStress(RC_FAULT_SHEAR_STRESS_CHANGE,
//              IDS_ET_FAULT_SHEAR_STRESS,
//              *pFaultChange, true);


  CResultGroup *pFaultPressure = new CResultGroup(IDS_RG_PORE_PRESSURE, *pFaultGroup);
  m_pFaultPressure= new CFaultScalarResult(IDS_RG_ABSOLUTE, IDS_ET_FAULT_PRESSURE, *pFaultPressure, CFaultScalarResult::PORE_PRESSURE);
  m_pFaultPressureChange= new CFaultScalarResult(IDS_RG_CHANGE, IDS_ET_FAULT_PRESSURE_CHANGE, *pFaultPressure, CFaultScalarResult::PORE_PRESSURE_CHANGE);

  if (Model().modelType() == CFemAppModel::eWellCasingModel)
  {
    CResultGroup* pRTCIGroup = new CResultGroup(IDS_RG_RTCI, *pOutput);
    m_pRTCIStrain = new CRTCIStrainResult(*pRTCIGroup);
  }

  m_pDerivedResults = new CDerivedResultGroup(IDS_RG_FILE_RESULT, *pOutput, true);
  m_pRegistryResults = new CDerivedResultGroup(IDS_RG_REGISTRY_RESULT, *pOutput, false);

  for(int nRegister = 0; nRegister < model.ResultRegisterSize(); nRegister++)
    LinkTo(model.ResultRegister(nRegister));
}

CResultTree::~CResultTree()
{
}

void CResultTree::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  for(int nRegister = 0; nRegister < model.ResultRegisterSize(); nRegister++)
  {
    if(&model.ResultRegister(nRegister) == &node)
      Modified();
  }

  CResultGroup::OnNeighbourModified(node, uHint);
}

const CDerivedResultGroup& CResultTree::DerivedResults() const
{
  assert(m_pDerivedResults);
  return *m_pDerivedResults;
}

CDerivedResultGroup& CResultTree::DerivedResults()
{
  assert(m_pDerivedResults);
  return *m_pDerivedResults;
}

const CDerivedResultGroup& CResultTree::RegistryResults() const
{
  assert(m_pRegistryResults);
  return *m_pRegistryResults;
}

CDerivedResultGroup& CResultTree::RegistryResults()
{
  assert(m_pRegistryResults);
  return *m_pRegistryResults;
}

/*!
  Returns basic result for geomec basic result export
*/
/*
CResultTree::TDataVec CResultTree::BasicResults() const
{
  TDataVec ret;
  // Do we have faults?
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
  assert(pModel);
  bool bHasFault = false;
  for(int i = 0; i < pModel->Mesh().Mesh().ElementSize(); i++) {
    const geo::CInterfaceElement* pInterface =
      dynamic_cast<const geo::CInterfaceElement*>( &pModel->Mesh().Mesh().Element(i) );
    if( pInterface ) {
      if( pInterface->BackFace() ) {
        // Fault element found 
        bHasFault = true;
        break;
      }
    }
  }
     
  if(pModel->ResultRegister().Linear()) 
    AddBasicResult(ret, true, bHasFault);
  if(pModel->ResultRegister().NonLinear())
    AddBasicResult(ret, false, bHasFault);

  return ret;
}

void CResultTree::AddBasicResult(TDataVec& result, const CAnalysisType& antype, bool bHasFault) const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
  assert(pModel);
  const CDepletionStage* pStage = &pModel->InitialDepletionStage();
  while( pStage ) {
    // Displacements
    int i;
    if( pStage != &pModel->InitialDepletionStage() ) {
      result.push_back(m_pDisplacement->ResultComponent(*pStage, bLinear, 0, 0));
      result.push_back(m_pDisplacement->ResultComponent(*pStage, bLinear, 0, 1));
      result.push_back(m_pDisplacement->ResultComponent(*pStage, bLinear, 0, 2));
    }
    // Effective stresses
    for(i = 0; i < 6; i++) {
      result.push_back(m_pEffectiveStress->Components().ResultComponent(*pStage, bLinear, 0, i));
    }
    // Strain
    if( pStage != &pModel->InitialDepletionStage() ) {
      for(i = 0; i < 6; i++) {
        result.push_back(m_pTotalStrain->Components().ResultComponent(*pStage, bLinear, 0, i));
      }
    }
    if(!bLinear && pStage != &pModel->InitialDepletionStage()) {
      // Plastic strain
      for(i = 0; i < 6; i++) {
        result.push_back(m_pPlasticStrain->Components().ResultComponent(*pStage, bLinear, 0, i));
      }
      // Creep strain
      for(i = 0; i < 6; i++) {
        result.push_back(m_pCreepStrain->Components().ResultComponent(*pStage, bLinear, 0, i));
      }
    }
    // Pore pressure
    result.push_back(m_pPorePressure->ResultComponent(*pStage, bLinear));
    if( bHasFault ) {
      if( pStage != &pModel->InitialDepletionStage() ) {
        result.push_back(m_pFaultDisplacement->ResultComponent(*pStage, bLinear, 0, 0));
        result.push_back(m_pFaultDisplacement->ResultComponent(*pStage, bLinear, 0, 1));
        result.push_back(m_pFaultDisplacement->ResultComponent(*pStage, bLinear, 0, 2));
      }
      result.push_back(m_pFaultEffectiveStress->ShearStress().ResultComponent(*pStage, bLinear, 0, 0));
      result.push_back(m_pFaultEffectiveStress->ShearStress().ResultComponent(*pStage, bLinear, 0, 1));
      result.push_back(m_pFaultEffectiveStress->ShearStress().ResultComponent(*pStage, bLinear, 0, 2));
    }
    if( pStage->Last() ) 
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

}
*/


CThicknessParallelInitializationCallback::CThicknessParallelInitializationCallback(CResultTree::CScalarResult::CScalarResultComponent& parent)
  : m_parent(parent), m_pmpDisplacementCache(new CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap())
{
}

CThicknessParallelInitializationCallback::~CThicknessParallelInitializationCallback()
{
  CValueMapper *vm = CValueMapper::instance();

  // we transfer ownership of the pointers to CValueMapper
  for (TFormationVolumeThicknessCacheMap::iterator it = m_mpFormationVolumeThicknessCache.begin(); it != m_mpFormationVolumeThicknessCache.end(); ++it)
  {
    vm->Add(it->first, it->second);
  }
  vm->Add(&m_parent, m_pmpDisplacementCache);
}

void CThicknessParallelInitializationCallback::GetCache(const CFormationVolume *pVolume, CFormationVolume::TThicknessCache **pCache)
{
  *pCache = m_mpFormationVolumeThicknessCache.insert(TFormationVolumeThicknessCacheMap::value_type(pVolume, new CFormationVolume::TThicknessCache())).first->second;
}

void CThicknessParallelInitializationCallback::GetCache(CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap **pCache)
{
  *pCache = m_pmpDisplacementCache;
}



CAvgVolumeParallelInitializationCallback::CAvgVolumeParallelInitializationCallback(CModelBase& model, CResultTree::CScalarResult::CScalarResultComponent& parent)
  : CResultTree::CScalarResult::CTensorParallelInitializationCallback(model)
  , m_parent(parent)
  , m_pmpValues(new CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap())
{
}

CAvgVolumeParallelInitializationCallback::~CAvgVolumeParallelInitializationCallback()
{
  CValueMapper *vm = CValueMapper::instance();

  vm->Add(&m_parent, m_pmpValues);
}

void CAvgVolumeParallelInitializationCallback::GetCache(CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap **cache)
{
  *cache = m_pmpValues;
}

