// ITensorGroup.cpp: implementation of the ITensorGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "ModelBase.h"
#include "ITensorGroup.h"
#include "resourceIDS.h"
#include "resourceIDI.h"
#include "TextTagUtils.h"
#include "ResultInfo.h"
#include "ValueMapper.h"
#include "ResultCache.h"
#include "MeshDataCacher.h"

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

//////////////////////////////////////////////////////////////////////
// Implementation if ITensorGroup
//////////////////////////////////////////////////////////////////////

ITensorGroup::ITensorGroup(const QString& sName, CResultGroup& parent)
: CResultGroup(sName, parent), 
  m_pComponent(0),
  m_pCylindric(0)
{
	m_pPrincipal[CVectorComposite::PD_MAX] = 0;
	m_pPrincipal[CVectorComposite::PD_MED] = 0;
	m_pPrincipal[CVectorComposite::PD_MIN] = 0;
}

ITensorGroup::ITensorGroup(unsigned int uName, CResultGroup& parent)
: CResultGroup(uName, parent), 
  m_pComponent(0),
  m_pCylindric(0)
{
	m_pPrincipal[CVectorComposite::PD_MAX] = 0;
	m_pPrincipal[CVectorComposite::PD_MED] = 0;
	m_pPrincipal[CVectorComposite::PD_MIN] = 0;
}

ITensorGroup::~ITensorGroup()
{
}

bool ITensorGroup::OnBuildComponent(const CDepletionStage& /*stage*/, const CAnalysisType& /*antype*/, int /*nRegister*/) const
{
	return true;
}

void ITensorGroup::CreateComposite()
{
	delete m_pComponent;
//	delete m_pInVariant;
	
	m_pComponent = new CComponentComposite(IDS_RG_TENSOR_COMPONENTS, *this);

//	m_pInVariant = new CInvariantComposite(IDS_RG_INVARIANTS, *this);

  if (static_cast<CModelBase&>(Model()).IsCylindricalModel())
    CreateCylindricalComposite();
}

void ITensorGroup::CreatePrincipalDirections()
{
  CResultGroup *pPrincipalDirections = new CResultGroup(IDS_RG_PRINC_DIR, *this);

  delete m_pPrincipal[CVectorComposite::PD_MAX];
  delete m_pPrincipal[CVectorComposite::PD_MED];
  delete m_pPrincipal[CVectorComposite::PD_MIN];

  // Create principal directions
  m_pPrincipal[CVectorComposite::PD_MAX] = new CVectorComposite(IDS_RG_PRINC_MAX, *pPrincipalDirections, CVectorComposite::PD_MAX);
  m_pPrincipal[CVectorComposite::PD_MED] = new CVectorComposite(IDS_RG_PRINC_MED, *pPrincipalDirections, CVectorComposite::PD_MED);
  m_pPrincipal[CVectorComposite::PD_MIN] = new CVectorComposite(IDS_RG_PRINC_MIN, *pPrincipalDirections, CVectorComposite::PD_MIN);
}

void ITensorGroup::CreateCylindricalComposite()
{
  delete m_pCylindric;
  m_pCylindric = new CVectorComposite(IDS_RG_CYLINDRICAL, *this, CVectorComposite::CYLINDRICAL);
}

bool ITensorGroup::OnDefined(const IResultComponent& component) const
{
  return component.ResultRegister().ResultsAvailable(component.AnalysisType());
}

const ITensorGroup::CVectorComposite *ITensorGroup::MaxDirection() const
{ 
	return m_pPrincipal[CVectorComposite::PD_MAX];
}

const ITensorGroup::CVectorComposite *ITensorGroup::MedDirection() const
{ 
	return m_pPrincipal[CVectorComposite::PD_MED];
}

const ITensorGroup::CVectorComposite *ITensorGroup::MinDirection() const
{ 
	return m_pPrincipal[CVectorComposite::PD_MIN];
}

const ITensorGroup::CVectorComposite& ITensorGroup::Direction(CVectorComposite::PRINCIPAL_DIRECTION direction) const
{
  assert(m_pPrincipal[direction]);
  return *m_pPrincipal[direction];
}


////////////////////////////////////////////////////////////////////////////
// Implementation of ITensorGroup::CTensorData
////////////////////////////////////////////////////////////////////////////

ITensorGroup::CTensorData::CTensorData(const IResultComponent& parent)
  : m_Group(0), m_Parent(parent), m_Min(DBL_MAX), m_Max(-DBL_MAX)
{
  ITensorGroup *pTensorGroup = dynamic_cast<ITensorGroup *>(parent.parent()->parent());
  if (pTensorGroup)
  {
    m_Group = pTensorGroup;
  }
  else
  {
    const ITensorGroup::CVectorComposite *pResult = static_cast<const ITensorGroup::CVectorComposite *>(&parent.Parent());

  	m_Group = const_cast<ITensorGroup *>(static_cast<const ITensorGroup *>(pResult->Parent()->Parent()));
  }


  assert(&m_Parent);
  assert(m_Group);
}

IValueDataInterfaceTensor::TMinMax ITensorGroup::CTensorData::MinMax(IProgressBase &, const IValueDataInterface::UNIT) const
{
  return TMinMax(m_Min, m_Max);
}

IValueDataInterfaceTensor::TValue ITensorGroup::CTensorData::ValuePoint(const geo::IPoint &pt, const IValueDataInterface::UNIT /*unit*/, geo::IParallelInitializationCallback *cb) const
{
  return m_Group->TensorPoint(pt, m_Parent, cb);
}

IValueDataInterfaceTensor::TValue ITensorGroup::CTensorData::Average(IProgressBase&, const IValueDataInterface::UNIT) const
{
  return IValueDataInterfaceTensor::TValue();
}

void ScaleTensor(CTensor &v, IQuantityDouble::UNIT unit, const ITensorGroup * pTensorGroup)
{
	if (unit == IQuantityDouble::FIELD_UNIT)
	{
		v.XX(pTensorGroup->ConvertToField(v.XX()));
		v.YY(pTensorGroup->ConvertToField(v.YY()));
		v.ZZ(pTensorGroup->ConvertToField(v.ZZ()));
		v.XY(pTensorGroup->ConvertToField(v.XY()));
		v.YZ(pTensorGroup->ConvertToField(v.YZ()));
		v.XZ(pTensorGroup->ConvertToField(v.XZ()));
	}

    if (pTensorGroup->Type() != ITensorGroup::TT_STRESS)
	{
		v.XY(2.0 * v.XY());
		v.YZ(2.0 * v.YZ());
		v.XZ(2.0 * v.XZ());
	}
}

void ITensorGroup::CTensorData::ResetMinMax() const
{
  m_Min = DBL_MAX;
  m_Max = -DBL_MAX;
}

void ITensorGroup::CTensorData::SetMinMax(const IValueDataInterfaceTensor::TValue& t) const
{
  if (t.XX() < m_Min) m_Min = t.XX();
  if (t.YY() < m_Min) m_Min = t.YY();
  if (t.ZZ() < m_Min) m_Min = t.ZZ();
  if (t.XY() < m_Min) m_Min = t.XX();
  if (t.YZ() < m_Min) m_Min = t.YZ();
  if (t.XZ() < m_Min) m_Min = t.XZ();

  if (t.XX() > m_Max) m_Max = t.XX();
  if (t.YY() > m_Max) m_Max = t.YY();
  if (t.ZZ() > m_Max) m_Max = t.ZZ();
  if (t.XY() > m_Max) m_Max = t.XX();
  if (t.YZ() > m_Max) m_Max = t.YZ();
  if (t.XZ() > m_Max) m_Max = t.XZ();
}

IValueDataInterfaceTensor::TValueVec ITensorGroup::CTensorData::ValueElement(const geo::IElement &elt, const IValueDataInterface::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDataInterfaceTensor::TValueVec tensors;
  tensors.reserve(elt.NrOfNodes());

  for (size_t i = 0; i < elt.NrOfNodes(); ++i) // TODO: for now we do it like this, but ITensorGroup needs to return a TValueVec, with the right values
  {
    CTensor t = m_Group->TensorElement(elt, i, CValueType::MT_NONE, m_Parent, cb);
    ScaleTensor(t, unit, m_Group);
    SetMinMax(t);
    tensors.push_back(t);
  }

  return tensors;
}

//////////////////////////////////////////////////////////////////////
// Implementation of ITensorGroup::CComponentComposite
//////////////////////////////////////////////////////////////////////
ITensorGroup::CComponentComposite::CComponentComposite(unsigned int uName, ITensorGroup& group)
: IResult(uName, group)
{
	BuildComponents();
	Index(group.ComponentIndex());	// Set the component index for saving and loading references
}

ITensorGroup::CComponentComposite::CComponentComposite(const QString& sName, ITensorGroup& group)
: IResult(sName, group)
{
	BuildComponents();
	Index(group.ComponentIndex());	// Set the component index for saving and loading references
}

void ITensorGroup::CComponentComposite::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
	const ITensorGroup& parent = dynamic_cast<const ITensorGroup&>(*Parent());
	if(parent.OnBuildComponent(stage, antype, nRegister))
	{
		new CComponent(IDS_RC_NN, *this, TC_XX, stage, antype, nRegister);
		new CComponent(IDS_RC_EE, *this, TC_YY, stage, antype, nRegister);
		new CComponent(IDS_RC_DD, *this, TC_ZZ, stage, antype, nRegister);
		new CComponent(IDS_RC_EN, *this, TC_XY, stage, antype, nRegister);
		new CComponent(IDS_RC_ED, *this, TC_YZ, stage, antype, nRegister);
		new CComponent(IDS_RC_ND, *this, TC_ZX, stage, antype, nRegister);
    new CFullTensorComponent(*this, stage, antype, nRegister);
	}
}

unsigned int ITensorGroup::CComponentComposite::IconId() const
{
	return IDI_RESULT_MAP;
}

bool ITensorGroup::CComponentComposite::OnDefined(const IResultComponent& component) const
{
	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
	assert(pParent);
	return pParent->OnDefined(component);
}

bool ITensorGroup::CComponentComposite::CanMap(const COpenGLNode& node, int nRegister) const
{
	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
	assert(pParent);
	return pParent->CanMap(node, nRegister);
}

QString ITensorGroup::CComponentComposite::ExportLabel(int nComponent) const
{
	// Get the type name of the current tensor
	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
	assert(pParent);

	// Fetch names
	QString sPreName, sName, sPostName;
	if(pParent->PreExportNameId())
		sPreName = getStringTableEntry(pParent->PreExportNameId());
	sName = getStringTableEntry(pParent->ExportNameId());
	if(pParent->PostExportNameId())
		sPostName = getStringTableEntry(pParent->PostExportNameId());

	// Get name of current component
	unsigned int uStringId;
	switch(nComponent)
	{
	case TC_XX:
		uStringId = IDS_ET_TENSOR_XX;
		break;
	case TC_YY:
		uStringId = IDS_ET_TENSOR_YY;
		break;
	case TC_ZZ:
		uStringId = IDS_ET_TENSOR_ZZ;
		break;
	case TC_XY:
		uStringId = IDS_ET_TENSOR_XY;
		break;
	case TC_YZ:
		uStringId = IDS_ET_TENSOR_YZ;
		break;
	case TC_ZX:
		uStringId = IDS_ET_TENSOR_XZ;
		break;
	case TC_FT:
		uStringId = IDS_ET_TENSOR_FT;
		break;
	default:
		uStringId = IDS_ERROR;
		assert(false);	// Bogus
	}	

	// Load component name
	QString sComponent;
	sComponent = getStringTableEntry(uStringId);

	return sPreName + sName + sPostName + sComponent;
}

QString ITensorGroup::CComponentComposite::ExportLabel() const
{
	// Get the type name of the current tensor
	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
	assert(pParent);

	// Fetch names
	QString sPreName, sName, sPostName;
	if(pParent->PreExportNameId())
		sPreName = getStringTableEntry(pParent->PreExportNameId());
	sName = getStringTableEntry(pParent->ExportNameId());
	if(pParent->PostExportNameId())
		sPostName = getStringTableEntry(pParent->PostExportNameId());

  return sPreName + sName + sPostName;
}

bool ITensorGroup::CComponentComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  // Get the type name of the current tensor
  const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent());
  assert(pParent);

  // Fetch names
  std::string sPreName, sName, sPostName;

  if(pParent->PreExportNameId())
    sPreName = getStringTableEntry(pParent->PreExportNameId());

  sName = getStringTableEntry(pParent->ExportNameId());

  if(pParent->PostExportNameId())
    sPostName = getStringTableEntry(pParent->PostExportNameId());

  std::string sCombinedName= sPreName + sName + sPostName;

  return validateName(resultInfo, name, sCombinedName,
    TensorResultComponentList(), "_L|_M|_C|_N", pParent);
}

//////////////////////////////////////////////////////////////////////
// Implementation if ITensorGroup::CComponentComposite::CComponent
//////////////////////////////////////////////////////////////////////

// NOTE: trying to do it the easy way by component+1 to make room for full tensor;
// component is used here as two different types: size_t index and the semantic TENSOR_COMPONENT indicator

ITensorGroup::CComponentComposite::CComponent::CComponent(unsigned int uComponentName,
														  CComponentComposite& parent,
														  enum TENSOR_COMPONENT component,		
														  const CDepletionStage& stage,
														  const CAnalysisType& antype,
														  int nRegister)
: IResultComponent(uComponentName, parent, stage, antype, nRegister, (int)component), m_component(component)
{
}

unsigned int ITensorGroup::CComponentComposite::CComponent::IconId() const
{
	return IDI_RESULT;
}

unsigned int ITensorGroup::CComponentComposite::CComponent::TypeId() const
{
	return 0;
}

QString ITensorGroup::CComponentComposite::CComponent::UnitName(const UNIT unit) const
{
	const IResult &result = (const IResult&)(Parent());
	const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(result.Parent());
	assert(pParent);
	return pParent->UnitName(unit);
}

geo::CValue ITensorGroup::CComponentComposite::CComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const IResult &result = (const IResult&)(Parent());
	const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(result.Parent());
	assert(pParent);
	return ComponentAt(pParent->TensorPoint(pt, *this, cb), unit);
}

void ITensorGroup::CComponentComposite::CComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  assert(elm.NrOfPoints());
  const IResult &result = (const IResult&)(Parent());
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(result.Parent());
  assert(pParent);
  for(int i = 0; i < values.size(); i++) {
	values[i] = ComponentAt(pParent->TensorElement(elm, i, map_type, *this, cb), unit);
  }
}

geo::CValue ITensorGroup::CComponentComposite::CComponent::ComponentAt(const ITensor& tensor, UNIT unit) const
{
	// Is the tensor valid?
	if( tensor.Empty() ) return geo::CValue();

	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(parent()->parent());
	assert(pParent);


	if( unit == CQuantity::FIELD_UNIT ) 
		return geo::CValue( pParent->ConvertToField( ComponentAt( tensor, CQuantity::SI_UNIT ).Value() ));

	switch(m_component)
	{
	case TC_XX:
		return geo::CValue( tensor.XX() );
	case TC_YY:
		return geo::CValue( tensor.YY() );
	case TC_ZZ:
		return geo::CValue( tensor.ZZ() );
	case TC_XY:
		if(pParent->Type() == TT_STRESS )
			return geo::CValue( tensor.XY() );
		else
			return geo::CValue( tensor.XY() * 2.0 );
	case TC_YZ:
		if(pParent->Type() == TT_STRESS )
			return geo::CValue( tensor.YZ() );
		else
			return geo::CValue( tensor.YZ() * 2.0 );
	case TC_ZX:
		if(pParent->Type() == TT_STRESS )
			return geo::CValue( tensor.XZ() );
		else
			return geo::CValue( tensor.XZ() * 2.0 );
	default:
		assert(false);	// Bogus
	}	

	return geo::CValue();
}

ITensorGroup::CComponentComposite::TENSOR_COMPONENT ITensorGroup::CComponentComposite::CComponent::Component() const
{
    return m_component;
}

bool ITensorGroup::CComponentComposite::CComponent::NeedParallelInitializationCallback() const
{
  return true;
}

geo::IParallelInitializationCallback *ITensorGroup::CComponentComposite::CComponent::GetParallelInitializationCallback()
{
  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  assert(pTensorGroup);
  return pTensorGroup->GetParallelInitializationCallback();
}

bool ITensorGroup::CComponentComposite::CComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  bool retval = !static_cast<CModelBase&>(Model()).IsCylindricalModel();;

  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());

  if (!pTensorGroup->PrepareMapping(pElementSet, this))
    return false;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;


  return retval;
}

void ITensorGroup::CComponentComposite::CComponent::FinishMapping()
{
}


////////////////////////////////////////////////////////////////////////////
// Implementation of ITensorGroup::CComponentComposite::CFullTensorComponent
////////////////////////////////////////////////////////////////////////////

ITensorGroup::CComponentComposite::CFullTensorComponent::CFullTensorComponent(CComponentComposite& parent,
														  const CDepletionStage& stage,
														  const CAnalysisType& antype,
														  int nRegister)
: IResultComponent("Full Tensor", parent, stage, antype, nRegister, 6), m_Data(*this)
{
}

unsigned int ITensorGroup::CComponentComposite::CFullTensorComponent::IconId() const
{
	return IDI_RESULT;
}

unsigned int ITensorGroup::CComponentComposite::CFullTensorComponent::TypeId() const
{
	return 0;
}

QString ITensorGroup::CComponentComposite::CFullTensorComponent::UnitName(const UNIT unit) const
{
	const IResult &result = (const IResult&)(Parent());
	const ITensorGroup *pParent = dynamic_cast<const ITensorGroup*>(result.Parent());
	assert(pParent);
	return pParent->UnitName(unit);
}

geo::CValue ITensorGroup::CComponentComposite::CFullTensorComponent::ValuePoint(const geo::IPoint& /*pt*/, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  return geo::CValue();
}

void ITensorGroup::CComponentComposite::CFullTensorComponent::MapValueElement(const geo::IElement& /*elm*/, IValueDomainScalar::TValueVec& /*values*/, TMapType /*map_type*/, UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
}

IValueComponentBase::TValueComponentType ITensorGroup::CComponentComposite::CFullTensorComponent::Type() const
{
  return TENSOR;
}

const IValueDataInterfaceTensor* ITensorGroup::CComponentComposite::CFullTensorComponent::TensorData() const
{
  return &m_Data;
}

IValueDataInterfaceTensor* ITensorGroup::CComponentComposite::CFullTensorComponent::TensorData()
{
  return &m_Data;
}

void ITensorGroup::CComponentComposite::CFullTensorComponent::ResetMinMax() const
{
  m_Data.ResetMinMax();
}

bool ITensorGroup::CComponentComposite::CFullTensorComponent::NeedParallelInitializationCallback() const
{
  return true;
}

geo::IParallelInitializationCallback *ITensorGroup::CComponentComposite::CFullTensorComponent::GetParallelInitializationCallback()
{
  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  assert(pTensorGroup);
  return pTensorGroup->GetParallelInitializationCallback();
}

bool ITensorGroup::CComponentComposite::CFullTensorComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  bool retval = !static_cast<CModelBase&>(Model()).IsCylindricalModel();;

  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());

  if (!pTensorGroup->PrepareMapping(pElementSet, this))
    return false;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  return retval;
}

void ITensorGroup::CComponentComposite::CFullTensorComponent::FinishMapping()
{
}


//////////////////////////////////////////////////////////////////////
// Implementation if ITensorGroup::CVectorComposite
//////////////////////////////////////////////////////////////////////
double ITensorGroup::CVectorComposite::ConvertToField(const double& dValue) const
{
	return m_pTensorGroup->ConvertToField(dValue);
}

ITensorGroup::CVectorComposite::PRINCIPAL_DIRECTION ITensorGroup::CVectorComposite::Direction() const
{
	return m_direction;
}

bool ITensorGroup::CVectorComposite::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  // Get direction
  std::string sDirection;
  switch(m_direction)
  {
  case PD_MAX:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MAX);
    break;
  case PD_MED:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MED);
    break;
  case PD_MIN:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MIN);
    break;
  default:
    assert(false);
    break;
  };

  // Fetch names
  std::string sPreName, sName, sPostName;
  if(m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());

  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());

  if(m_pTensorGroup->PostExportNameId())
    sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  std::string sCombinedName=
    sPreName + std::string("P") + sName + sDirection + sPostName;

  return validateName(resultInfo, name, sCombinedName,
    VectorResultComponentList(), "_L|_M|_C|_N");
}


bool ITensorGroup::CVectorComposite::OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const
{
	return m_pTensorGroup->OnBuildComponent(stage, antype, nRegister);
}

ITensorGroup *findTensorGroup(CResultGroup& group)
{
  // find parent ancestor that is tensor group
  CResultGroup *findTensorGroup = &group;
  while (findTensorGroup && !dynamic_cast<const ITensorGroup*>(findTensorGroup))
    findTensorGroup = findTensorGroup->Parent();

  assert(findTensorGroup);

  return static_cast<ITensorGroup *>(findTensorGroup);
}

const ITensorGroup *ITensorGroup::CVectorComposite::TensorGroup()
{
  return m_pTensorGroup;
}

const ITensorGroup *ITensorGroup::CVectorComposite::TensorGroup() const
{
  return m_pTensorGroup;
}

ITensorGroup::CVectorComposite::CVectorComposite(unsigned int uName, CResultGroup& group, PRINCIPAL_DIRECTION direction)
: IVectorResult(uName, group), m_direction(direction) 
{
	m_pTensorGroup = findTensorGroup(group);

	BuildComponents();

  // Set references for saving and loading ....
  Index(m_pTensorGroup->VectorIndex(m_direction));
}

ITensorGroup::CVectorComposite::CVectorComposite(const QString& sName, CResultGroup& group, PRINCIPAL_DIRECTION direction)
: IVectorResult(sName, group), m_direction(direction) 
{
	m_pTensorGroup = findTensorGroup(group);

	BuildComponents();
	// Set references for saving and loading ....
	Index(m_pTensorGroup->VectorIndex(m_direction));
}

const geo::IVector& ITensorGroup::CVectorComposite::VectorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
	return m_pTensorGroup->VectorPoint(point, m_direction, component, cb).first;
}

const geo::IVector& ITensorGroup::CVectorComposite::VectorElement(const geo::IElement& element, 
															      int nNodeIndex, 
																  TMapType map_type,
                                  const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
	return m_pTensorGroup->VectorElement(element, m_direction, nNodeIndex, map_type, component, cb).first;
}


const ITensor & ITensorGroup::CVectorComposite::TensorElement(const geo::IElement& element,
	int nNodeIndex,
	TMapType map_type,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
	return m_pTensorGroup->TensorElement(element, nNodeIndex, map_type, component, cb);
}

void ITensorGroup::CVectorComposite::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
	if(OnBuildComponent(stage, antype, nRegister))
	{
    //new CFullVectorComponent(*this, stage, antype, nRegister);
    if (m_direction == CYLINDRICAL)
    {
      new CVectorComponent(IDS_RC_RADIAL, *this, stage, antype, nRegister, VC_X);
      new CVectorComponent(IDS_RC_TANGENTIAL, *this, stage, antype, nRegister, VC_Y);
      new CVectorComponent(IDS_RC_AXIAL, *this, stage, antype, nRegister, VC_Z);
    }
    else
    {
      new CVectorComponent(IDS_RC_NORTHING, *this, stage, antype, nRegister, VC_X);
      new CVectorComponent(IDS_RC_EASTING, *this, stage, antype, nRegister, VC_Y);
      new CVectorComponent(IDS_RC_DEPTH, *this, stage, antype, nRegister, VC_Z);
    }
		new CLengthComponent(*this, stage, antype, nRegister);
    new CFullVectorComponent(*this, stage, antype, nRegister);
	}
}


ITensorGroup::CVectorComposite::CVectorComponent::CVectorComponent(unsigned int uComponentName,				// Name from resource
                                                                   IVectorResult& parent,				// The vector composite parent
                                                                   const CDepletionStage& stage,		// Depletion stage of the component
					                    		                   const CAnalysisType& antype,						// Analysis type
							                                       int nRegister,
                                                     VECTOR_COMPONENT uComponent) 					// Register index
: IVectorResult::CVectorComponent(uComponentName, parent, uComponent, stage, antype, nRegister), m_TensorData(*this)
{
}

const IValueDataInterfaceTensor* ITensorGroup::CVectorComposite::CVectorComponent::TensorData() const
{
  return &m_TensorData;
}

IValueDataInterfaceTensor* ITensorGroup::CVectorComposite::CVectorComponent::TensorData()
{
  return &m_TensorData;
}

void ITensorGroup::CVectorComposite::CVectorComponent::ResetMinMax() const
{
  m_TensorData.ResetMinMax();
}

bool ITensorGroup::CVectorComposite::CVectorComponent::NeedParallelInitializationCallback() const
{
  return true;
}

geo::IParallelInitializationCallback *ITensorGroup::CVectorComposite::CVectorComponent::GetParallelInitializationCallback()
{
  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  if (!pTensorGroup)
    pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent()->parent());
  assert(pTensorGroup);
  return pTensorGroup->GetParallelInitializationCallback();
}

bool ITensorGroup::CVectorComposite::CVectorComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  bool retval = !static_cast<CModelBase&>(Model()).IsCylindricalModel();;

  ITensorGroup* pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent());
  if (!pTensorGroup)
    pTensorGroup = dynamic_cast<ITensorGroup*>(Parent().parent()->parent());
  assert(pTensorGroup);

  if (!pTensorGroup->PrepareMapping(pElementSet, this))
    return false;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;


  return retval;
}

void ITensorGroup::CVectorComposite::CVectorComponent::FinishMapping()
{
}


ITensorGroup::CVectorComposite::CLengthComponent::CLengthComponent(IVectorResult& parent,				// The vector composite parent
                                                                   const CDepletionStage& stage,		// Depletion stage of the component
					                    		                   const CAnalysisType& antype,						// Analysis type
							                                       int nRegister,
                                                     VECTOR_COMPONENT uComponent) 					// Register index
: CVectorComponent(IDS_RC_LENGTH, parent, stage, antype, nRegister, uComponent)
{
}

geo::CValue ITensorGroup::CVectorComposite::CLengthComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const CVectorComposite* pResult = dynamic_cast<const CVectorComposite*>(&Parent());
	assert(pResult);
  const ITensorGroup* pTensorGroup = pResult->TensorGroup();

	if(pTensorGroup->VectorPoint(pt, pResult->Direction(), *this, cb).first.Empty()) return geo::CValue();

	double dValue = pTensorGroup->VectorPoint(pt, pResult->Direction(), *this, cb).second;
	if(unit == CQuantity::FIELD_UNIT)
		dValue = pTensorGroup->ConvertToField(dValue);

	return geo::CValue(dValue);
}

void ITensorGroup::CVectorComposite::CLengthComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  assert(elm.NrOfPoints() == values.size());
  const CVectorComposite* pResult = dynamic_cast<const CVectorComposite*>(&Parent());
  assert(pResult);
  const ITensorGroup* pTensorGroup = pResult->TensorGroup();

  for(int i = 0; i < values.size(); i++) {
	  if(pTensorGroup->VectorElement(elm, pResult->Direction(), i, map_type, *this, cb).first.Empty()) {
		  values[i] = geo::CValue();
	  } else {
        double dValue = pTensorGroup->VectorElement(elm, pResult->Direction(), i, map_type, *this, cb).second;
		if(unit == CQuantity::FIELD_UNIT)
			dValue = pTensorGroup->ConvertToField(dValue);

		values[i] = geo::CValue(dValue);
	  }
  }
}
/*
geo::CVector ITensorGroup::CVectorComposite::VectorAtComponent(const geo::IElement& mesh_element, int nNodeIndex, const IResultComponent& component, UNIT unit) const
{
	const ITensorGroup* pParent = dynamic_cast<const ITensorGroup*>(Parent()->Parent());
	assert(pParent); 
	geo::CVector vector = VectorAtEx(mesh_element, nNodeIndex, component).first;
		if(unit == CQuantity::FIELD_UNIT)
			vector = vector * FF_FACTOR_STRESS;
	return vector;
}
*/


ITensorGroup::CVectorComposite::CFullVectorComponent::CFullVectorComponent(IVectorResult& parent,				// The vector composite parent
                                                                   const CDepletionStage& stage,		// Depletion stage of the component
					                    		                   const CAnalysisType& antype,						// Analysis type
							                                       int nRegister)
: IVectorResult::CFullVectorComponent(parent, stage, antype, nRegister), m_TensorData(*this)
{
}

const IValueDataInterfaceTensor* ITensorGroup::CVectorComposite::CFullVectorComponent::TensorData() const
{
  return &m_TensorData;
}

IValueDataInterfaceTensor* ITensorGroup::CVectorComposite::CFullVectorComponent::TensorData()
{
  return &m_TensorData;
}

void ITensorGroup::CVectorComposite::CFullVectorComponent::ResetMinMax() const
{
  m_TensorData.ResetMinMax();

  IVectorResult::CFullVectorComponent::ResetMinMax();
}

bool ITensorGroup::CVectorComposite::CFullVectorComponent::NeedParallelInitializationCallback() const
{
  return true;
}

geo::IParallelInitializationCallback *ITensorGroup::CVectorComposite::CFullVectorComponent::GetParallelInitializationCallback()
{
  const CVectorComposite* pResult = dynamic_cast<const CVectorComposite*>(&Parent());
  assert(pResult);
  const ITensorGroup* pTensorGroup = pResult->TensorGroup();
  assert(pTensorGroup);
  return const_cast<ITensorGroup *>(pTensorGroup)->GetParallelInitializationCallback();
}

bool ITensorGroup::CVectorComposite::CFullVectorComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  bool retval = !static_cast<CModelBase&>(Model()).IsCylindricalModel();

  const CVectorComposite* pResult = dynamic_cast<const CVectorComposite*>(&Parent());
  assert(pResult);
  const ITensorGroup* pTensorGroup = pResult->TensorGroup();
  assert(pTensorGroup);
  if (!const_cast<ITensorGroup *>(pTensorGroup)->PrepareMapping(pElementSet, this))
    return false;

  int c = ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);

  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 13))
    retval = false;

  return retval;
}

void ITensorGroup::CVectorComposite::CFullVectorComponent::FinishMapping()
{
}


class CVectorLess
{
public:
	bool operator()(const geo::CVector& v1, const geo::CVector& v2) const
	{
	   if(v1.Z() < v2.Z()) return true;
	   if(v1.Z() > v2.Z()) return false;
	   if(v1.Y() < v2.Y()) return true;
	   if(v1.Y() > v2.Y()) return false;
	   return v1.X() < v2.X();
	}
};

/*
static void switchVector(geo::CVector& v1, geo::CVector&v2)
{
	geo::CVector temp(v1);
	v1 = v2;
	v2 = temp;
}
*/

void ITensorGroup::PrincipalDirections(geo::CVector& vcmax,
									   double& max_length,
									   geo::CVector& vcmed,
									   double& med_length,
									   geo::CVector& vcmin,
									   double& min_length,
									   const ITensor& tensor) const
{
	if(tensor.Empty()) vcmax = vcmed = vcmin = geo::CVector();

	geo::CMatrix matrix1 = tensor.GetPrincipalDir();
	geo::CMatrix matrix2 = tensor.GetPrincipalStr();

	vcmax.Set(matrix1.Value(0, 2), matrix1.Value(1, 2), matrix1.Value(2, 2)); 
	vcmed.Set(matrix1.Value(0, 1), matrix1.Value(1, 1), matrix1.Value(2, 1)); 
	vcmin.Set(matrix1.Value(0, 0), matrix1.Value(1, 0), matrix1.Value(2, 0)); 

//	assert(fabs(vcmax.Length() - 1.0) < EPS);
//	assert(fabs(vcmed.Length() - 1.0) < EPS);
//	assert(fabs(vcmin.Length() - 1.0) < EPS);

	min_length = matrix2.Value(0, 0);
	med_length = matrix2.Value(1, 1);
	max_length = matrix2.Value(2, 2);
	
	if(fabs(min_length) < EPS && fabs(med_length) < EPS && fabs(max_length) < EPS) {
		vcmax = vcmin = vcmed = geo::CVector::NullVector;
		return;
	}

	typedef std::set<geo::CVector, CVectorLess> TVectorSet;
	if((fabs(max_length - med_length) < EPS) && !(fabs(min_length - med_length) < EPS))
	{ // Max and Med principal values are equal.
		TVectorSet stVector;
		stVector.insert(vcmed);
		stVector.insert(vcmax);
		TVectorSet::iterator it = stVector.begin();
		vcmed = *it;
		vcmax = *++it;
	}

	if((fabs(min_length - med_length) < EPS) && !(fabs(max_length - med_length) < EPS))
	{ // Min and Med principal values are equal.
		TVectorSet stVector;
		stVector.insert(vcmed);
		stVector.insert(vcmin);
		TVectorSet::iterator it = stVector.begin();
		vcmin = *it;
		vcmed = *++it;
	}

	if((fabs(min_length - med_length) < EPS) && (fabs(max_length - med_length) < EPS))
	{ // Min and Med principal values are equal.
		TVectorSet stVector;
		stVector.insert(vcmax);
		stVector.insert(vcmed);
		stVector.insert(vcmin);
		TVectorSet::iterator it = stVector.begin();
		vcmin = *it;
		vcmed = *++it;
		vcmax = *++it;
	}

	assert(fabs(vcmax.Length() - 1.0) < EPS);
	assert(fabs(vcmed.Length() - 1.0) < EPS);
	assert(fabs(vcmin.Length() - 1.0) < EPS);

	vcmax = vcmax * max_length;
	vcmed = vcmed * med_length;
	vcmin = vcmin * min_length;


}

void ITensorGroup::SetSignConvention(geo::IVector& vec, double dLength) const
{
  const double eps = GetTypicalEpsilon();

  if(fabs(dLength) > eps)
  {
    // get the current sign of the product of the vector components
    double d = 1.;
    if(fabs(vec.X()) > eps)
      d *= vec.X();
    if(fabs(vec.Y()) > eps)
      d *= vec.Y();
    if(fabs(vec.Z()) > eps)
      d *= vec.Z();

    // the product should have the same sign as the length
    if((d < 0 && dLength > 0) || (d > 0 && dLength < 0))
      vec.Flip();
  }
}

QString ITensorGroup::CVectorComposite::UnitName(UNIT unit) const
{
	return m_pTensorGroup->UnitName(unit);
}

unsigned int ITensorGroup::CVectorComposite::IconId() const
{
	return IDI_RESULT_MAP;
}

bool ITensorGroup::CVectorComposite::OnDefined(const IResultComponent& component) const
{
	return m_pTensorGroup->OnDefined(component);
}

bool ITensorGroup::CVectorComposite::CanMap(const COpenGLNode& node, int nRegister) const
{
	return m_pTensorGroup->CanMap(node, nRegister);
}

QString ITensorGroup::CVectorComposite::ExportLabel(int nComponent) const
{
	// Fetch names
	QString sPreName, sName, sPostName;
	if(m_pTensorGroup->PreExportNameId())
		sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
	sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
	if(m_pTensorGroup->PostExportNameId())
		sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

	// Get direction
	QString sDirection;
	switch(m_direction)
	{
	case PD_MAX:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MAX);
		break;
	case PD_MED:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MED);
		break;
	case PD_MIN:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MIN);
		break;
	default:
		assert(false);
		break;
	};

	return sPreName + "P" + sName + sDirection + sPostName + IVectorResult::ExportLabel(nComponent); 
}

QString ITensorGroup::CVectorComposite::ExportLabel() const
{
	// Fetch names
	QString sPreName, sName, sPostName;
	if(m_pTensorGroup->PreExportNameId())
		sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
	sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
	if(m_pTensorGroup->PostExportNameId())
		sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  return sPreName + "P" + sName + sPostName;
}

QString ITensorGroup::CVectorComposite::VectorExportLabel() const
{
	// Fetch names
	QString sPreName, sName, sPostName;
	if(m_pTensorGroup->PreExportNameId())
		sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
	sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
	if(m_pTensorGroup->PostExportNameId())
		sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

	// Get direction
	QString sDirection;
	switch(m_direction)
	{
	case PD_MAX:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MAX);
		break;
	case PD_MED:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MED);
		break;
	case PD_MIN:
		sDirection = getStringTableEntry(IDS_ET_TENSOR_MIN);
		break;
	default:
		assert(false);
		break;
	};

	return sPreName + "P" + sName + sDirection + sPostName; 
}
