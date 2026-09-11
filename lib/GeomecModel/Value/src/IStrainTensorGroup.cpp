// IStrainTensorGroup.cpp: implementation of the IStrainTensorGroup class.
//
//////////////////////////////////////////////////////////////////////
#include "IStrainTensorGroup.h"
#include "BaseEntryTypes.h"
#include "NewWellPath.h"
#include "WellCasingModel.h"
#include "WellCasingRTCI.h"
#include "resourceIDS.h"
#include "resourceIDI.h"
#include "FemAppModel.h"
#include "Hexahedron.h"
#include "VolumetricStrainLoad.h"
#include "TextTagUtils.h"
#include "ResultInfo.h"
#include "ResultCache.h"
#include "MeshDataCacher.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IStrainTensorGroup::IStrainTensorGroup(unsigned int uResultName, TValueSetFunction function, CResultGroup& group)
: ITensorGroupTemplate<CStrainTensor>( uResultName, function, group ),
  m_pInVariant(0),
  m_pAxialStrainComposite(0)
{
}

IStrainTensorGroup::IStrainTensorGroup(const QString& sResultName, TValueSetFunction function, CResultGroup& group)
: ITensorGroupTemplate<CStrainTensor>( sResultName, function, group ),
  m_pInVariant(0),
  m_pAxialStrainComposite(0)
{
}

IStrainTensorGroup::~IStrainTensorGroup()
{

}

void IStrainTensorGroup::CreateComposite()
{
  ITensorGroupTemplate<CStrainTensor>::CreateComposite();

  CreatePrincipalDirections();

  delete m_pInVariant;
  m_pInVariant = new CInvariantComposite(IDS_RG_INVARIANTS, *this);

  delete m_pAxialStrainComposite;
  m_pAxialStrainComposite = new CWellPathAxialStrainComposite(IDS_RG_WPAXIALSTRAIN, *this);
}

bool IStrainTensorGroup::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& /*antype*/, int /*nRegister*/) const
{
  return !stage.Initial();
}

double IStrainTensorGroup::ConvertToField(const double& dValue) const
{
  return dValue;
}

QString IStrainTensorGroup::UnitName(CQuantity::UNIT /*unit*/) const
{
  return getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
}

//////////////////////////////////////////////////////////////////////
// Implementation if ITensorGroup::CInvariantComposite
//////////////////////////////////////////////////////////////////////
IStrainTensorGroup::CInvariantComposite::CInvariantComposite(const QString &name, IStrainTensorGroup &group)
:IResult(name, group)
{
  BuildComponents();
  Index(group.InvariantIndex());	// Index for loading and saving result refences
}

IStrainTensorGroup::CInvariantComposite::CInvariantComposite(unsigned int uName, IStrainTensorGroup &group)
:IResult(uName, group)
{
  BuildComponents();
  Index(group.InvariantIndex());	// Index for loading and saving result refences
}

void IStrainTensorGroup::CInvariantComposite::BuildComponent(const CDepletionStage &stage, const CAnalysisType& antype, int nRegister)
{
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(Parent());
  assert(pParent);
  if(pParent->OnBuildComponent(stage, antype, nRegister))
  {
    new CInvariantComponent(IDS_RC_INVVOLSTR, *this, INV_I1, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_INV2, *this, INV_I2, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_INV3, *this, INV_I3, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_VONMIS, *this, INV_VONMIS, stage, antype, nRegister);
  }
}

unsigned int IStrainTensorGroup::CInvariantComposite::IconId() const
{
  return IDI_RESULT_MAP;
}

bool IStrainTensorGroup::CInvariantComposite::OnDefined(const IResultComponent &component) const
{
  const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
  assert(pParent);
  return pParent->OnDefined(component);
}

bool IStrainTensorGroup::CInvariantComposite::CanMap(const COpenGLNode& node, int nRegister) const
{
  const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
  assert(pParent);
  return pParent->CanMap(node, nRegister);
}

geo::IParallelInitializationCallback *IStrainTensorGroup::CInvariantComposite::GetParallelInitializationCallback()
{
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
  return pGroup->GetParallelInitializationCallback();

  return 0;
}


//////////////////////////////////////////////////////////////////////
// Implementation if ITensorGroup::CInvariantComposite::CInvariantComponent
//////////////////////////////////////////////////////////////////////
IStrainTensorGroup::CInvariantComposite::CInvariantComponent::CInvariantComponent(unsigned int uInvariantName,		
                                          CInvariantComposite &parent,
                                          enum INVARIANT_TYPE type,	
                                          const CDepletionStage &stage,
                                          const CAnalysisType& antype,
                                          int nRegister)
: IResultComponent(uInvariantName, parent, stage, antype, nRegister, (int)type), m_type(type)
{

}

unsigned int IStrainTensorGroup::CInvariantComposite::CInvariantComponent::IconId() const
{
  return IDI_RESULT;
}

unsigned int IStrainTensorGroup::CInvariantComposite::CInvariantComponent::TypeId() const
{
  return 0;
}

QString IStrainTensorGroup::CInvariantComposite::CInvariantComponent::UnitName(const UNIT unit) const
{
  const IResult &result = (const IResult&)(Parent());
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(result.Parent());
  assert(pParent);
  return pParent->UnitName(unit);
}

geo::CValue IStrainTensorGroup::CInvariantComposite::CInvariantComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const IResult &result = (const IResult&)(Parent());
  const IStrainTensorGroup *pParent = dynamic_cast<const IStrainTensorGroup*>(result.Parent());
  assert(pParent);

  geo::CValue value = Value(pParent->TensorAtPoint(pt, *this, Stage(), AnalysisType(), false, cb), unit);

  if(pParent->Change())
  {
    CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    geo::CValue iniValue = Value(pParent->TensorAtPoint(pt, *this, dep_entry.MarkedAsInitialStage(), AnalysisType(), false, cb), unit);

    value = value.Valid() && iniValue.Valid() ? value - iniValue : geo::CValue();
  }

  return value;
}

void IStrainTensorGroup::CInvariantComposite::CInvariantComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  assert(elm.NrOfPoints());
  const IResult &result = (const IResult&)(Parent());
  const IStrainTensorGroup *pParent = dynamic_cast<const IStrainTensorGroup*>(result.Parent());
  assert(pParent);

  const IStrainTensorGroup::TTensorValueVec& vcTensor = pParent->TensorAtElement(elm, map_type, *this, Stage(), AnalysisType(), false, cb);
  for(int i = 0; i < values.size(); i++) {
    values[i] = Value(vcTensor[i], unit);
  }

  if(pParent->Change())
  {
    CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    const IStrainTensorGroup::TTensorValueVec& vcIniTensor = pParent->TensorAtElement(elm, map_type, *this, dep_entry.MarkedAsInitialStage(), AnalysisType(), false, cb);

    for(int i = 0; i < values.size(); ++i)
    {
      geo::CValue iniValue = Value(vcIniTensor[i], unit);
      values[i] = values[i].Valid() && iniValue.Valid() ? values[i] - iniValue : geo::CValue();
    }
  }
}

geo::CValue IStrainTensorGroup::CInvariantComposite::CInvariantComponent::Value(const CStrainTensor& tensor, CQuantity::UNIT /*unit*/) const
{
  if(tensor.Empty()) return geo::CValue();

  switch(m_type)
  {
  case INV_I1:
    return geo::CValue(tensor.VolumetricStrain());
  case INV_I2:
  {
    const CMechanicalTensor *pDevTens = tensor.DeviatoricTensor();
    // return deviatoric stress invariant
    geo::CValue vRet( pDevTens->InvarI2() );
    delete pDevTens;
    return vRet;
  }
  case INV_I3:
  {
    const CMechanicalTensor *pDevTens = tensor.DeviatoricTensor();
    // return deviatoric stress invariant
    geo::CValue vRet( pDevTens->InvarI3() );
    delete pDevTens;
    return vRet;
  }
  case INV_VONMIS:
    return tensor.InvarVonMis();
  default:
    assert(false);
  }

  return geo::CValue();
}

bool IStrainTensorGroup::CInvariantComposite::CInvariantComponent::NeedParallelInitializationCallback() const
{
  return true;
}

bool IStrainTensorGroup::CInvariantComposite::CInvariantComponent::PrepareMapping(const geo::IElementSet* /*pElementSet*/)
{
  bool retval = true;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRAIN_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 12))
  retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
  retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_STRAIN_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 12))
  retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
  retval = false;

  return retval;
}

void IStrainTensorGroup::CInvariantComposite::CInvariantComponent::FinishMapping()
{
}

QString IStrainTensorGroup::CInvariantComposite::ExportLabel(int nComponent) const
{
  // Get the type name of the current tensor
  const IStrainTensorGroup* pParent = dynamic_cast<const IStrainTensorGroup*>(Parent());
  assert(pParent);

  QString sRet;
  switch(nComponent)
  {
  case INV_I1:
      sRet = getStringTableEntry(IDS_ET_TENSOR_INV_VOLUME);
    break;
  case INV_I2:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_2);
    break;
  case INV_I3:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_3);
    break;
  case INV_VONMIS:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES);
    break;
  default:
    assert(false);
  }

  // Fetch names ...
  QString sPreName, sName, sPostName;
  if(pParent->PreExportNameId())
    sPreName = getStringTableEntry(pParent->PreExportNameId());
  sName = getStringTableEntry(pParent->ExportNameId());
  if(pParent->PostExportNameId())
    sPostName = getStringTableEntry(pParent->PostExportNameId());

  return sPreName + sName + sPostName + sRet;
}

bool IStrainTensorGroup::CInvariantComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  // Get the type name of the current tensor
  const IStrainTensorGroup* pParent = dynamic_cast<const IStrainTensorGroup*>(Parent());
  assert(pParent);

  // Fetch names ...
  std::string sPreName, sName, sPostName;
  if(pParent->PreExportNameId())
  sPreName = getStringTableEntry(pParent->PreExportNameId());
  sName = getStringTableEntry(pParent->ExportNameId());
  if(pParent->PostExportNameId())
  sPostName = getStringTableEntry(pParent->PostExportNameId());

  std::string sCombinedName= sPreName + sName + sPostName;

  return validateName(resultInfo, name, sCombinedName,
  StrainInvariantResultComponentList(), "_L|_M|_C|_N|_H", pParent);
}

///// IStrainTensorGroup::CWellPathAxialStrainComposite

IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComposite(unsigned int uName, CResultGroup& group)
: IResult(uName, group)
{
  m_pTensorGroup = static_cast<IStrainTensorGroup*>(&group);

  BuildComponents();

  // Set references for saving and loading ....
  Index(m_pTensorGroup->WellPathAxialStrainIndex());
}

IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComposite(const QString& sName, CResultGroup& group)
: IResult(sName, group)
{
  m_pTensorGroup = static_cast<IStrainTensorGroup*>(&group);

  BuildComponents();

  // Set references for saving and loading ....
  Index(m_pTensorGroup->WellPathAxialStrainIndex());
}

unsigned int IStrainTensorGroup::CWellPathAxialStrainComposite::IconId() const
{
  return IDI_RESULT;
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::OnDefined(const IResultComponent& component) const
{
  return m_pTensorGroup->OnDefined(component);
}

QString IStrainTensorGroup::CWellPathAxialStrainComposite::ExportLabel() const
{
  // Fetch names
  QString sPreName, sName, sPostName;
  if(m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
  if(m_pTensorGroup->PostExportNameId())
    sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  return sPreName + "WPAxial" + sName + sPostName; 
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  // Fetch names
  std::string sPreName, sName, sPostName;
  if(m_pTensorGroup->PreExportNameId())
  sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());

  if(m_pTensorGroup->PostExportNameId())
  sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  std::string sCombinedName= sPreName + "WPAxial" + sName + sPostName;

  return validateName(resultInfo, name, sCombinedName, "", "_L|_M|_C|_N|_H");
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::CanMap(const COpenGLNode& node, int /*nRegister*/) const
{
  return ((dynamic_cast<const CWellFormation *>(&node) || dynamic_cast<const CWellZoomInMesh *>(&node) || dynamic_cast<const CNewWellPath*>(&node) || (dynamic_cast<const CWellCasingModel*>(&Model()) != 0)) && dynamic_cast<const CWellCasingRTCI*>(&node) == 0);
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const
{
  return m_pTensorGroup->OnBuildComponent(stage, antype, nRegister);
}

void IStrainTensorGroup::CWellPathAxialStrainComposite::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if(OnBuildComponent(stage, antype, nRegister))
    new CWellPathAxialStrainComponent(IDS_RG_WPAXIALSTRAIN, *this, stage, antype, nRegister);
}

geo::IParallelInitializationCallback *IStrainTensorGroup::CWellPathAxialStrainComposite::GetParallelInitializationCallback()
{
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
  return pGroup->GetParallelInitializationCallback();

  return 0;
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *pValueComponent)
{
  return false;
  // TODO: fix the check below; it doesn't put the strains in the critical section of the result cache
  /*
  bool retval = true;
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (!pGroup || !pGroup->PrepareMapping(pElementSet, pValueComponent))
  retval = false;

  return retval;
  */
}

void IStrainTensorGroup::CWellPathAxialStrainComposite::FinishMapping()
{
}

///// IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent

IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::CWellPathAxialStrainComponent(unsigned int uName,
                                                                                                                CWellPathAxialStrainComposite &parent,
                                                                                                                const CDepletionStage &stage,
                                                                                                                const CAnalysisType& antype,
                                                                                                                int nRegister)
: IResultComponent(uName, parent, stage, antype, nRegister, 0)
{
}

unsigned int IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::IconId() const
{
  return IDI_RESULT;
}

unsigned int IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::TypeId() const
{
  return 0;
}

QString IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::UnitName(const UNIT /*unit*/) const
{
  return QString("-");
}

geo::CValue IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::ValuePoint(const geo::IPoint& pt,
  const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const // NOT CASCADING PARALLELINITIALIZATION
{
  const IStrainTensorGroup* pTensorGroup = dynamic_cast<const IStrainTensorGroup*>(Parent().parent());
  assert(pTensorGroup); 

  return pTensorGroup->WellPathAxialValuePoint(pt, *this);
}

void IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::MapValueElement(const geo::IElement& elm,
                                                                                                       IValueDomainScalar::TValueVec& values,
                                                                                                       TMapType map_type,
                                                                                                       UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const // NOT CASCADING PARALLELINITIALIZATION
{
  assert(elm.NrOfPoints() == values.size());
  const IStrainTensorGroup* pTensorGroup = dynamic_cast<const IStrainTensorGroup*>(Parent().parent());
  assert(pTensorGroup);
  values = pTensorGroup->WellPathAxialValueElement(elm, map_type, *this);
}

bool IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent::NeedParallelInitializationCallback() const
{
  return true;
}
