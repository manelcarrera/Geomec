
#include "MaterialResultTree.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "HorizonBase.h"
#include "ifaceelmmat.h"
#include "Material.h"
#include "ValueTypeFactory.h"
#include "IInterfaceElement.h"
#include "IDCElementProperty.h"
#include "FormationBase.h"
#include "WellCasingMesh.h"
#include "WellCasingModel.h"
#include "WellCasingSteel.h"
#include "TextTagUtils.h"
#include "ResultInfo.h"
#include "ValueMapper.h"
#include "ISurface.h"
#include "InterfaceBoundary.h"
#include "BoundaryInterfaceMaterial.h"

///// CMaterialResult::CMaterialResultComponent::CValueSet

CMaterialResult::CMaterialResultComponent::CValueSet::CValueSet(const CMaterialResultComponent& component)
: m_component(component),
  m_unit(IQuantityDouble::SI_UNIT)
{
}

const geo::IElementSet &CMaterialResult::CMaterialResultComponent::CValueSet::ElementSet() const
{
  const CModelBase& model = static_cast<const CModelBase&>(m_component.Model());
  return model.Mesh().Mesh();
}

void CMaterialResult::CMaterialResultComponent::CValueSet::ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback * /*cb*/) const
{
  const CModelBase& model = static_cast<const CModelBase&>(m_component.Model());
  const geo::IElement& elm = model.Mesh().Mesh().Element(iElementIndex);
  values = m_component.ElementValues(elm, m_unit);
}

bool CMaterialResult::CMaterialResultComponent::CValueSet::IsEmpty() const
{
  return false;
}

bool CMaterialResult::CMaterialResultComponent::CValueSet::IsValid(const geo::CValue& value) const
{
  return value.Valid();
}

void CMaterialResult::CMaterialResultComponent::CValueSet::Unit(IQuantityDouble::UNIT unit)
{
  m_unit = unit;
}


//// CMaterialResult::CMaterialResultComponent

CMaterialResult::CMaterialResultComponent::CMaterialResultComponent(const QString& strName, const CDepletionStage& stage, CMaterialResult& parent, const CAnalysisType& antype, int nRegister)
: IResultComponent(strName, parent, stage, antype, nRegister, 0),
  m_valueset(*this)
{
}

CMaterialResult::CMaterialResultComponent::~CMaterialResultComponent()
{
}

unsigned int CMaterialResult::CMaterialResultComponent::IconId() const
{
  return Parent().IconId();
}

unsigned int CMaterialResult::CMaterialResultComponent::TypeId() const
{
  return Parent().TypeId();
}

QString CMaterialResult::CMaterialResultComponent::UnitName(const UNIT unit) const
{
  return MaterialResult().UnitName(unit);
}

long CMaterialResult::CMaterialResultComponent::SavedItems() const
{
  return 0;
}

geo::CValue CMaterialResult::CMaterialResultComponent::SingleValueElement(const geo::IElement& elm, const UNIT unit) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CMeshBase& mesh = model.Mesh();
  const CHorizonBase* pHor = 0;

  if(dynamic_cast<const geo::IInterfaceElement*>(&elm))
  pHor = mesh.SlipHorizon(elm);
  if(pHor)
  {
  const dia::IMaterial& mat = pHor->InterfaceMaterial(elm, Stage());
  const CInterfaceElementMaterial* pIfaceMat = dynamic_cast<const CInterfaceElementMaterial*>(&mat);
  if(pIfaceMat)
  {
      if(pIfaceMat->ValidParameterValue(MaterialResult().TypeId()))
    return pIfaceMat->ParameterValue(MaterialResult().TypeId()) * MaterialResult().UnitFactor(unit);
  }
  }
  else
  {
  const CFFMaterial* material = mesh.getCFFMaterial(elm, Stage());

  if ((material != 0) && material->IsParameter(MaterialResult().TypeId()))
  {
      return material->ParameterValue(MaterialResult().TypeId()) *
    MaterialResult().UnitFactor(unit);
  }
  }

  return geo::CValue();
}

geo::CValue CMaterialResult::CMaterialResultComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback* /*cb*/) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CMeshBase& mesh = model.Mesh();
  const std::set<int>& stCandidates = mesh.Mesh().Candidates(pt);
  std::set<int>::const_iterator it;
  for(it = stCandidates.begin(); it != stCandidates.end(); ++it)
  {
  const geo::IElement& elm = mesh.Mesh().Element(*it);
  if(elm.Contains(pt, true))
  {
      geo::CValue val = SingleValueElement(elm, unit);
      if(val.Valid())
    return val;
  }
  }

  return geo::CValue();
}

IValueDomainScalar::TValueVec CMaterialResult::CMaterialResultComponent::ElementValues(const geo::IElement& elm, const UNIT unit) const
{
  geo::CValue val = SingleValueElement(elm, unit);
  IValueDomainScalar::TValueVec vcValues(elm.NrOfNodes(), val);
  return vcValues;
}

void CMaterialResult::CMaterialResultComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType /*map_type*/, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  m_valueset.Unit(unit);
  m_valueset.ElementValue(elm, values, cb);
}

QString CMaterialResult::CMaterialResultComponent::ExportLabel() const
{
  QString s;
  s = QString("%1_D%2").arg(MaterialResult().StoredExportLabel()).arg(
  Stage().Index());
  return s;
}

const CMaterialResult& CMaterialResult::CMaterialResultComponent::MaterialResult() const
{
  return static_cast<const CMaterialResult&>(Parent());
}

bool CMaterialResult::CMaterialResultComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  CMeshBase& mesh = model.Mesh();

  bool retval = true;

  CValueMapper *vm = CValueMapper::instance();

  if (!vm->PrepareMappingElementSet(pElementSet, &mesh.Mesh()))
  retval = false;

  if (!vm->PrepareMappingSurfaces(model, pElementSet, Stage()))
  retval = false;

  if (!vm->PrepareMappingMaterial(model, pElementSet, Stage()))
  retval = false;

  return retval;
}


///// CBoundaryMaterialResultComponent


CMaterialResult::CBoundaryMaterialResultComponent::CBoundaryMaterialResultComponent(const QString& strName, const CDepletionStage& stage, CMaterialResult& parent, const CAnalysisType& antype, int nRegister, bool showRad)
  : CMaterialResultComponent(strName, stage, parent, antype, nRegister)
  , m_showRad(showRad)
{
}

CMaterialResult::CBoundaryMaterialResultComponent::~CBoundaryMaterialResultComponent()
{
}

geo::CValue CMaterialResult::CBoundaryMaterialResultComponent::SingleValueElement(const geo::IElement& elm, const UNIT unit) const
{
  const geo::CInterfaceElement *ifElt = dynamic_cast<const geo::CInterfaceElement *>(&elm);

  if (ifElt && &ifElt->Front() == &ifElt->Back())
  {
  const IWellModel::CBoundary *pWellBoundary = dynamic_cast<const IWellModel::CBoundary *>(&static_cast<const CModelBase&>(Model()).Boundary());

  if (pWellBoundary)
  {
      const CBoundaryInterfaceMaterial& mat = pWellBoundary->InterfaceMaterial(*ifElt);
      return (m_showRad ? mat.Krad() : mat.Ktan()) / 1e6;
  }

  const CInterfaceBoundary* pBoundary = dynamic_cast<const CInterfaceBoundary*>(&static_cast<const CModelBase&>(Model()).Boundary());
  
  if (pBoundary)
  {
      const CBoundaryInterfaceMaterial& mat = pBoundary->InterfaceMaterial(*ifElt);
      return (m_showRad ? mat.Krad() : mat.Ktan()) / 1e6;
  }
  }
 
  return geo::CValue();
}


///// CMaterialResult

CMaterialResult::CMaterialResult(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group)
: IResult(uName, group),
  m_uiValueTypeID(uiValueTypeID),
  m_uiIconID(uiIconID)
{
  CPointSet ps(Model());
  const CValueTypeFactory *f = CValueTypeFactory::instance();
  const CValueType* pVT = f->BuildValueType(ps, uiValueTypeID, uName);
  assert(pVT);
  assert(dynamic_cast<const CValueComponent*>(&pVT->Component()));
  const CValueComponent& comp = static_cast<const CValueComponent&>(pVT->Component());

  m_strExportLabel = comp.ExportLabel();
  m_strUnitNameSI = comp.UnitName(IQuantityDouble::SI_UNIT);
  m_strUnitNameField = comp.UnitName(IQuantityDouble::FIELD_UNIT);
  m_dFieldFactor = comp.FieldFactor();

  BuildComponents();
}

CMaterialResult::~CMaterialResult()
{
}

bool CMaterialResult::Empty() const
{
  return false;
}

long CMaterialResult::SavedItems() const
{
  return 0;
}

unsigned int CMaterialResult::IconId() const
{
  return m_uiIconID;
}

unsigned int CMaterialResult::TypeId() const
{
  return m_uiValueTypeID;
}

bool CMaterialResult::Less(const CGraphNode &node) const
{
  const CMaterialResult* pResult = dynamic_cast<const CMaterialResult*>(&node);
  if(pResult)
  return TypeId() < pResult->TypeId();

  return IValueComposite::Less(node);
}

bool CMaterialResult::isMaterialResult() const
{
  return true;
}

bool CMaterialResult::OnDefined(const IResultComponent& /*component*/) const
{
  return true;
}

bool CMaterialResult::CanMap(const COpenGLNode& /*node*/, int /*nRegister*/) const
{
  return true;
}

QString CMaterialResult::ExportLabel(int /*nComponent*/) const
{
  assert(FALSE);
  return QString();
}

void CMaterialResult::OnNeighbourDeleted(const CGraphNode& node)
{
  TComponentMap::iterator it;
  for(it = m_mpComponents.begin(); it != m_mpComponents.end(); ++it)
  {
  if(it->second == &node)
  {
      m_mpComponents.erase(it);
      break;
  }
  }

  IResult::OnNeighbourDeleted(node);
}

const QString& CMaterialResult::StoredExportLabel() const
{
  return m_strExportLabel;
}

QString CMaterialResult::UnitName(const UNIT unit) const
{
  QString s;
  switch(unit)
  {
  case IQuantityDouble::SI_UNIT:
  s = m_strUnitNameSI;
  break;
  case IQuantityDouble::FIELD_UNIT:
  s = m_strUnitNameField;
  break;
  default:
  assert(FALSE);
  }

  return s;
}

double CMaterialResult::UnitFactor(const UNIT unit) const
{
  double factor = 1.0;
  if(unit == IQuantityDouble::FIELD_UNIT)
  factor = m_dFieldFactor;

  return factor;
}

void CMaterialResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  // we're only interested in depletion stages
  int idx = stage.Index();
  TComponentMap::iterator it = m_mpComponents.find(idx);
  if(it == m_mpComponents.end())
  m_mpComponents.insert(TComponentMap::value_type(idx, new CMaterialResultComponent(Name(), stage, *this, antype, nRegister)));
  else
  {
  RegisterMode(stage, antype, nRegister);
  }
}

IResultComponent* CMaterialResult::OnGetResultComponent(int nTimeStep, const CAnalysisType& /*antype*/, int /*nRegister*/, unsigned int /*nComponent*/) const
{
  TComponentMap::const_iterator it = m_mpComponents.find(nTimeStep);
  if(it != m_mpComponents.end())
  return it->second;

  return 0;
}

bool CMaterialResult::ValidName
( const std::string &/*name*/
, CResultInfo &/*resultInfo*/
) const
{
  assert(0); // not used
  return false;
}


///// CBoundaryMaterialResult

CBoundaryMaterialResult::CBoundaryMaterialResult(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group)
  : CMaterialResult(uName, uiValueTypeID, uiIconID, group)
{
}

void CBoundaryMaterialResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  // we're only interested in depletion stages
  int idx = stage.Index();
  TComponentMap::iterator it = m_mpComponents.find(idx);
  if (it == m_mpComponents.end())
  m_mpComponents.insert(TComponentMap::value_type(idx, new CBoundaryMaterialResultComponent(Name(), stage, *this, antype, nRegister, TypeId() == IDT_VALUETYPE_KRAD)));
  else
  {
  RegisterMode(stage, antype, nRegister);
  }
}


///// CMaterialResultTree

CMaterialResultTree::CMaterialResultTree(CModelBase& model)
: CResultGroup("Material Parameters", model)
{
  new CMaterialResult(IDS_VALUENAME_YOUNGS_MODULUS, IDT_VALUETYPE_YOUNGS_MODULUS, IDI_VALUETYPE_YOUNGS_MODULUS, *this);
  new CMaterialResult(IDS_VALUENAME_POISSONS_RATIO, IDT_VALUETYPE_POISSONS_RATIO, IDI_VALUETYPE_POISSONS_RATIO, *this);
  new CMaterialResult(IDS_VALUENAME_BULKSTIFFNESS, IDT_VALUETYPE_BULKSTIFFNESS, IDI_VALUETYPE_BULKSTIFFNESS, *this);
  new CMaterialResult(IDS_VALUENAME_SHEARMODULUS, IDT_VALUETYPE_SHEARMODULUS, IDI_VALUETYPE_SHEARMODULUS, *this);
  new CMaterialResult(IDS_VALUENAME_COHESION, IDT_VALUETYPE_COHESION, IDI_VALUETYPE_COHESION, *this);
  new CMaterialResult(IDS_VALUENAME_FRICTION_ANGLE, IDT_VALUETYPE_FRICTION_ANGLE, IDI_VALUETYPE_FRICTION_ANGLE, *this);
  new CMaterialResult(IDS_VALUENAME_INIPORO, IDT_VALUETYPE_POROSITY, IDI_VALUETYPE_INIPORO, *this);
  new CMaterialResult(IDS_VALUENAME_FLUID_BULK_MOD, IDT_VALUETYPE_FLUID_BULK_MOD, IDI_VALUETYPE_FLUID_BULK_MOD, *this);
  new CMaterialResult(IDS_VALUENAME_RHOB, IDT_VALUETYPE_RHOB, IDI_VALUETYPE_RHOB, *this);
  new CMaterialResult(IDS_VALUENAME_INITFRICTION, IDT_VALUETYPE_INITFRICTION, IDI_VALUETYPE_INITFRICTION, *this);
  new CMaterialResult(IDS_VALUENAME_HARDENING, IDT_VALUETYPE_HARDENING, IDI_VALUETYPE_HARDENING, *this);
  new CMaterialResult(IDS_VALUENAME_PRECONSOLIDATION, IDT_VALUETYPE_PRECONSOLIDATION, IDI_VALUETYPE_PRECONSOLIDATION, *this);
  new CMaterialResult(IDS_VALUENAME_CAPSHAPE, IDT_VALUETYPE_CAPSHAPE, IDI_VALUETYPE_CAPSHAPE, *this);
  new CMaterialResult(IDS_VALUENAME_DILATATION, IDT_VALUETYPE_DILATATION, IDI_VALUETYPE_DILATATION, *this);
  new CMaterialResult(IDS_VALUENAME_LATRATIO_MAX, IDT_VALUETYPE_LATRATIO_MAX, IDI_VALUETYPE_LATRATIO_MAX, *this);
  new CMaterialResult(IDS_VALUENAME_LATRATIO_MIN, IDT_VALUETYPE_LATRATIO_MIN, IDI_VALUETYPE_LATRATIO_MIN, *this);
  new CMaterialResult(IDS_VALUENAME_AZIMUTH, IDT_VALUETYPE_AZIMUTH, IDI_VALUETYPE_AZIMUTH, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_A1, IDT_VALUETYPE_CREEP_A1, IDI_VALUETYPE_CREEP_A1, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_A2, IDT_VALUETYPE_CREEP_A2, IDI_VALUETYPE_CREEP_A2, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_N1, IDT_VALUETYPE_CREEP_N1, IDI_VALUETYPE_CREEP_N1, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_N2, IDT_VALUETYPE_CREEP_N2, IDI_VALUETYPE_CREEP_N2, *this);
//  new CMaterialResult(IDS_VALUENAME_CAPSHAPE, IDT_VALUETYPE_CAPSHAPEPARAM, IDI_VALUETYPE_CAPSHAPE, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_COHESION1, IDT_VALUETYPE_HARD_COHESION1, IDI_VALUETYPE_HARD_COHESION1, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_COHESION2, IDT_VALUETYPE_HARD_COHESION2, IDI_VALUETYPE_HARD_COHESION2, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_COHESION3, IDT_VALUETYPE_HARD_COHESION3, IDI_VALUETYPE_HARD_COHESION3, *this);
//  new CMaterialResult(IDS_VALUENAME_EQUIV_PLAST_STRAIN1, IDT_VALUETYPE_EQUIV_PLAST_STRAIN1, IDI_VALUETYPE_EQUIV_PLAST_STRAIN1, *this);
//  new CMaterialResult(IDS_VALUENAME_EQUIV_PLAST_STRAIN2, IDT_VALUETYPE_EQUIV_PLAST_STRAIN2, IDI_VALUETYPE_EQUIV_PLAST_STRAIN2, *this);
//  new CMaterialResult(IDS_VALUENAME_EQUIV_PLAST_STRAIN3, IDT_VALUETYPE_EQUIV_PLAST_STRAIN3, IDI_VALUETYPE_EQUIV_PLAST_STRAIN3, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_FRICTION1, IDT_VALUETYPE_HARD_FRICTION1, IDI_VALUETYPE_HARD_FRICTION1, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_FRICTION2, IDT_VALUETYPE_HARD_FRICTION2, IDI_VALUETYPE_HARD_FRICTION2, *this);
  new CMaterialResult(IDS_VALUENAME_HARD_FRICTION3, IDT_VALUETYPE_HARD_FRICTION3, IDI_VALUETYPE_HARD_FRICTION3, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_QR1, IDT_VALUETYPE_CREEP_QR1, IDI_VALUETYPE_CREEP_QR1, *this);
  new CMaterialResult(IDS_VALUENAME_CREEP_QR2, IDT_VALUETYPE_CREEP_QR2, IDI_VALUETYPE_CREEP_QR2, *this);
//  new CMaterialResult(IDS_VALUENAME_CREEP_ALPHA, IDT_VALUETYPE_CREEP_ALPHA, IDI_VALUETYPE_CREEP_ALPHA, *this);
  new CMaterialResult(IDS_VALUENAME_TENSILE_STRETCH, IDT_VALUETYPE_TENSILE_STRETCH, IDI_VALUETYPE_TENSILE_STRETCH, *this);
  new CMaterialResult(IDS_VALUENAME_THERMAL_EXPANSION, IDT_VALUETYPE_THERMAL_EXPANSION, IDI_VALUETYPE_THERMAL_EXPANSION, *this);
  new CMaterialResult(IDS_VALUENAME_ELASTIC_HARDENING, IDT_VALUETYPE_ELASTIC_HARDENING, IDI_VALUETYPE_ELASTIC_HARDENING, *this);
  new CMaterialResult(IDS_VALUENAME_SEC_PRECON, IDT_VALUETYPE_SEC_PRECON, IDI_VALUETYPE_SEC_PRECON, *this);
  new CMaterialResult(IDS_VALUENAME_SEC_HARDENING, IDT_VALUETYPE_SEC_HARDENING, IDI_VALUETYPE_SEC_HARDENING, *this);
  new CMaterialResult(IDS_VALUENAME_ELASCOMPRES, IDT_VALUETYPE_ELASCOMPRES, IDI_VALUETYPE_ELASCOMPRES, *this);
  new CMaterialResult(IDS_VALUENAME_PLASCOMPRES, IDT_VALUETYPE_PLASCOMPRES, IDI_VALUETYPE_PLASCOMPRES, *this);
  new CMaterialResult(IDS_VALUENAME_YOUNGMODULUS_NORM, IDT_VALUETYPE_YOUNGMODULUS_NORM, IDI_VALUETYPE_YOUNGMODULUS_NORM, *this);
  new CMaterialResult(IDS_VALUENAME_YOUNGMODULUS_TRANS, IDT_VALUETYPE_YOUNGMODULUS_TRANS, IDI_VALUETYPE_YOUNGMODULUS_TRANS, *this);
  new CMaterialResult(IDS_VALUENAME_POISSONRATIO_NORM, IDT_VALUETYPE_POISSONRATIO_NORM, IDI_VALUETYPE_POISSONRATIO_NORM, *this);
  new CMaterialResult(IDS_VALUENAME_POISSONRATIO_TRANS, IDT_VALUETYPE_POISSONRATIO_TRANS, IDI_VALUETYPE_POISSONRATIO_TRANS, *this);
  new CMaterialResult(IDS_VALUENAME_LAYER_NORMAL_INCL, IDT_VALUETYPE_LAYER_NORMAL_INCL, IDI_VALUETYPE_LAYER_NORMAL_INCL, *this);
  new CMaterialResult(IDS_VALUENAME_LAYER_NORMAL_AZI, IDT_VALUETYPE_LAYER_NORMAL_AZI, IDI_VALUETYPE_LAYER_NORMAL_AZI, *this);
  new CMaterialResult(IDS_VALUENAME_FRACT_STIFF_NORMAL, IDT_VALUETYPE_FRACT_STIFF_NORMAL, IDI_VALUETYPE_FRACT_STIFF_NORMAL, *this);
  new CMaterialResult(IDS_VALUENAME_FRACT_STIFF_SHEAR, IDT_VALUETYPE_FRACT_STIFF_SHEAR, IDI_VALUETYPE_FRACT_STIFF_SHEAR, *this);
//  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS, IDT_VALUETYPE_HIGH_FRACT_DENS, IDI_VALUETYPE_HIGH_FRACT_DENS, *this);
//  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS_INCL, IDT_VALUETYPE_HIGH_FRACT_DENS_INCL, IDI_VALUETYPE_HIGH_FRACT_DENS_INCL, *this);
//  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS_AZI, IDT_VALUETYPE_HIGH_FRACT_DENS_AZI, IDI_VALUETYPE_HIGH_FRACT_DENS_AZI, *this);
//  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS, IDT_VALUETYPE_LOW_FRACT_DENS, IDI_VALUETYPE_LOW_FRACT_DENS, *this);
//  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS_INCL, IDT_VALUETYPE_LOW_FRACT_DENS_INCL, IDI_VALUETYPE_LOW_FRACT_DENS_INCL, *this);
//  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS_AZI, IDT_VALUETYPE_LOW_FRACT_DENS_AZI, IDI_VALUETYPE_LOW_FRACT_DENS_AZI, *this);
//  new CMaterialResult(IDS_VALUENAME_INTER_FRACT_DENS, IDT_VALUETYPE_INTER_FRACT_DENS, IDI_VALUETYPE_INT_FRACT_DENS, *this);
  new CMaterialResult(IDS_VALUENAME_THERM_LIN_EXP_NORM, IDT_VALUETYPE_THERM_LIN_EXP_NORM, IDI_VALUETYPE_THERM_LIN_EXP_NORM, *this);
  new CMaterialResult(IDS_VALUENAME_THERM_LIN_EXP_LAT, IDT_VALUETYPE_THERM_LIN_EXP_LAT, IDI_VALUETYPE_THERM_LIN_EXP_LAT, *this);
  new CMaterialResult(IDS_VALUENAME_THERM_CONDUCT, IDT_VALUETYPE_THERM_CONDUCT, IDI_VALUETYPE_THERM_CONDUCT, *this);
  new CMaterialResult(IDS_VALUENAME_THERM_CAPACI, IDT_VALUETYPE_THERM_CAPACI, IDI_VALUETYPE_THERM_CAPACI, *this);
  new CMaterialResult(IDS_VALUENAME_PERMEA, IDT_VALUETYPE_PERMEA, IDI_VALUETYPE_PERMEA, *this);
  new CMaterialResult(IDS_VALUENAME_YOUNG_DECOMP, IDT_VALUETYPE_YOUNG_DECOMP, IDI_VALUETYPE_YOUNG_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_POISSON_DECOMP, IDT_VALUETYPE_POISSON_DECOMP, IDI_VALUETYPE_POISSON_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_VISCOSITY, IDT_VALUETYPE_VISCOSITY, IDI_VALUETYPE_VISCOSITY, *this);
  new CMaterialResult(IDS_VALUENAME_FLUIDX, IDT_VALUETYPE_FLUIDX, IDI_VALUETYPE_CAPSHAPE, *this);
  new CMaterialResult(IDS_VALUENAME_FLUID_DENSITY, IDT_VALUETYPE_FLUID_DENSITY, IDI_VALUETYPE_FLUID_DENSITY, *this);
  new CMaterialResult(IDS_VALUENAME_REFERENCESTRESS, IDT_VALUETYPE_REFERENCESTRESS, IDI_VALUETYPE_REFERENCESTRESS, *this);
  new CMaterialResult(IDS_VALUENAME_APERTUREPARAM, IDT_VALUETYPE_APERTUREPARAM, IDI_VALUETYPE_APERTUREPARAM, *this);
  new CMaterialResult(IDS_VALUENAME_GRAINSTIFFNESS, IDT_VALUETYPE_GRAINSTIFFNESS, IDI_VALUETYPE_GRAINSTIFFNESS, *this);
  new CMaterialResult(IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDI_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, IDI_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDI_VALUETYPE_POISSONRATIO_NORM_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP, IDI_VALUETYPE_POISSONRATIO_TRANS_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS, *this);
  new CMaterialResult(IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, *this);
  new CMaterialResult(IDS_VALUENAME_VELOCITYP, IDT_VALUETYPE_VELOCITYP, IDI_VALUETYPE_VELOCITYP, *this);
  new CMaterialResult(IDS_VALUENAME_VELOCITYS, IDT_VALUETYPE_VELOCITYS, IDI_VALUETYPE_VELOCITYS, *this);
  new CMaterialResult(IDS_VALUENAME_THOMSEN_EPSILON, IDT_VALUETYPE_THOMSEN_EPSILON, IDI_VALUETYPE_THOMSEN_EPSILON, *this);
  new CMaterialResult(IDS_VALUENAME_THOMSEN_GAMMA, IDT_VALUETYPE_THOMSEN_GAMMA, IDI_VALUETYPE_THOMSEN_GAMMA, *this);
  new CMaterialResult(IDS_VALUENAME_THOMSEN_DELTA, IDT_VALUETYPE_THOMSEN_DELTA, IDI_VALUETYPE_THOMSEN_DELTA, *this);
  new CMaterialResult(IDS_VALUENAME_V0_HI, IDT_VALUETYPE_V0_HI, IDI_VALUETYPE_V0_HI, *this);
  new CMaterialResult(IDS_VALUENAME_V0_ME, IDT_VALUETYPE_V0_ME, IDI_VALUETYPE_V0_ME, *this);
  new CMaterialResult(IDS_VALUENAME_V0_LO, IDT_VALUETYPE_V0_LO, IDI_VALUETYPE_V0_LO, *this);
  new CMaterialResult(IDS_VALUENAME_REFERENCE_APERTURE, IDT_VALUETYPE_REFERENCE_APERTURE, IDI_VALUETYPE_REFERENCE_APERTURE, *this);

  CResultGroup* pFractGroup = new CResultGroup(IDS_VALUENAME_FRACT_TENSOR, *this);
  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS, IDT_VALUETYPE_HIGH_FRACT_DENS, IDI_VALUETYPE_HIGH_FRACT_DENS, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS_INCL, IDT_VALUETYPE_HIGH_FRACT_DENS_INCL, IDI_VALUETYPE_HIGH_FRACT_DENS_INCL, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_HIGH_FRACT_DENS_AZI, IDT_VALUETYPE_HIGH_FRACT_DENS_AZI, IDI_VALUETYPE_HIGH_FRACT_DENS_AZI, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS, IDT_VALUETYPE_LOW_FRACT_DENS, IDI_VALUETYPE_LOW_FRACT_DENS, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS_INCL, IDT_VALUETYPE_LOW_FRACT_DENS_INCL, IDI_VALUETYPE_LOW_FRACT_DENS_INCL, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_LOW_FRACT_DENS_AZI, IDT_VALUETYPE_LOW_FRACT_DENS_AZI, IDI_VALUETYPE_LOW_FRACT_DENS_AZI, *pFractGroup);
  new CMaterialResult(IDS_VALUENAME_INTER_FRACT_DENS, IDT_VALUETYPE_INTER_FRACT_DENS, IDI_VALUETYPE_INT_FRACT_DENS, *pFractGroup);

  new CMaterialResult(IDS_VALUENAME_DUMMY, IDT_VALUETYPE_DUMMY, IDI_VALUETYPE_DUMMY, *this);

  CResultGroup *pBoundaryGroup = new CResultGroup("Boundary K values", *this);
  new CBoundaryMaterialResult(IDS_VALUENAME_KRAD, IDT_VALUETYPE_KRAD, IDI_VALUETYPE_KRAD, *pBoundaryGroup);
  new CBoundaryMaterialResult(IDS_VALUENAME_KTAN, IDT_VALUETYPE_KTAN, IDI_VALUETYPE_KTAN, *pBoundaryGroup);
}

unsigned int CMaterialResultTree::IconId() const
{
  return IDI_ROCK_MATERIAL;
}
