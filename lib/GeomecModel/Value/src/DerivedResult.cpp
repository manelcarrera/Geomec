// DerivedResult.cpp: implementation of the CDerivedResult class.
//
//////////////////////////////////////////////////////////////////////

#include "DerivedResult.h"
#include "ModelBase.h"
#include "RpnFormationOperand.h"
#include "FormationBase.h"
#include "BaseEntryTypes.h"
#include "FieldFactor.h"
#include "rpn_version.h"
#include "ResultTree.h"
#include "RpnMaterialParameterProxy.h"
#include "ValueTypeFactory.h"
#include "RpnValueSet.h"
#include "resourceIDI.h"
#include "FemAppEntryTypes.h"
#include "ISettings.h"
#include "ISettings.h"
#include "lbcx.h"
#include "DerivedResult.1.0.9.h"
#include "ProxyPersistant.h"
#include "rpnconstantoperand.h"
#include "MeasuredTopDisplacementsNode.h"
#include "ResultInfo.h"
#include "TextTagUtils.h"

#define RESULT_SECTION "UserResult"
#define RESULT_KEY	   "Result%1"

//////////////////////////////////////////////////////////////////////
// Implementation of CDerivedResult::CResultCoordinateProxy
//////////////////////////////////////////////////////////////////////
CDerivedResult::CResultCoordinateProxy::CResultCoordinateProxy(unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: rpn::CRpnStack::CCoordinateProxy(uName, stack, coor, sProxyId)
{
}

CDerivedResult::CResultCoordinateProxy::CResultCoordinateProxy(unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor)
: rpn::CRpnStack::CCoordinateProxy(uName, stack, coor)
{
}

CDerivedResult::CResultCoordinateProxy::CResultCoordinateProxy(const QString& sName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: rpn::CRpnStack::CCoordinateProxy(sName, stack, coor, sProxyId)
{
}

CDerivedResult::CResultCoordinateProxy::CResultCoordinateProxy(rpn::CRpnStack& stack, const QString& sProxyId)
: rpn::CRpnStack::CCoordinateProxy(stack, sProxyId)
{
}

rpn::CRpnOperand::IValueProxy* CDerivedResult::CResultCoordinateProxy::Clone(rpn::CRpnStack& NewStack) const
{
  return new CResultCoordinateProxy(TextTag(), NewStack, m_coor, ProxyId());
}

unsigned int CDerivedResult::CResultCoordinateProxy::IconId() const
{
  return IDI_VALUETYPE_COORDINATE;
}

int CDerivedResult::CResultCoordinateProxy::TypeId() const
{
  return COORDINATE_ID;
}

bool CDerivedResult::CResultCoordinateProxy::Recursive(TParentSet /*stParent*/) const
{
  return false;
}

CDerivedResult::TValue CDerivedResult::CResultCoordinateProxy::Value(const geo::IPoint &pt, UNIT unit) const
{
  if(unit == CQuantity::SI_UNIT)
  {
    switch(m_coor)
    {
    case CO_X:
      return TValue(pt.X());
    case CO_Y:
      return TValue(pt.Y());
    case CO_Z:
      return TValue(pt.Z());
    default:
      assert(false);
    }
  }

  switch(m_coor)
  {
  case CO_X:
    return TValue(pt.X() * FF_FACTOR_LENGTH);
  case CO_Y:
    return TValue(pt.Y() * FF_FACTOR_LENGTH);
  case CO_Z:
    return TValue(pt.Z() * FF_FACTOR_LENGTH);
  default:
    assert(false);
  }

  return TValue();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::CValueComponentObserver(IValueComponentBase& value_component, CValueComponentProxy& proxy)
: CGraphNode(""), m_proxy(&proxy), m_value_component(value_component)
{
  assert(!value_component.IsCopy());
  reParent(&value_component);
}

CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::~CValueComponentObserver()
{
  if ( m_proxy ) m_proxy->m_pObserver = 0;
}

const IValueComponentBase& CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::ValueComponent() const
{
  assert(!m_value_component.IsCopy());
  return m_value_component;
}

IValueComponentBase& CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::ValueComponent()
{
  assert(!m_value_component.IsCopy());
  return m_value_component;
}

void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::OnNeighbourDeleted(const CGraphNode& item)
{
  if(&item == &m_value_component && m_proxy)
  {
    // When the proxy is a result, we disconnect and try to reconnect later. Otherwise we destroy our selves
 		if(m_proxy->IsResult())
      delete this;
    else
      delete m_proxy;

    return ;
  }

  CGraphNode::OnNeighbourDeleted(item);
}

unsigned int CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::IconId() const
{
   return 0;
}

unsigned int CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::TypeId() const
{
  return 0;
}

rpn::CRpnObject* CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentObserver::GetExpandedRpnObject(rpn::CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  const CDerivedResultComponent* pDerResComp = dynamic_cast<const CDerivedResultComponent*>(&m_value_component);
  if(pDerResComp)
  return pDerResComp->RpnStack().Top().GetExpandedRpnObject(targetstack, lstMessages);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CDerivedResult::CDerivedResultComponent::CValueComponentProxy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentProxy(const QString& sName, IValueComponentBase& value_component, rpn::CRpnStack& NewStack, bool bFixedLinStage)
: IValueProxy(NewStack),
  m_sName(sName),
  m_nIndex(-1),
  m_nComponent(-1),
  m_nTimeStep(-1),
  m_antype(CAnalysisType::AT_NONLIN),
  m_bStoreOnFile(false),
  m_bFixedLinStage(bFixedLinStage)
{
  m_pObserver = new CValueComponentObserver(value_component, *this);
  m_pModel = dynamic_cast<CModelBase*>(&value_component.Model());
  assert(m_pModel);

  // When we have a result component we store the index, the component index, timestep and linearity,
  // because when the depletion stage changes the result components are decoupled.
  const IResultComponent* pResult = dynamic_cast<const IResultComponent*>(&ValueComponent());
  if(pResult)
  {
    m_nIndex = pResult->Parent().Index();
    assert(m_nIndex > 0);
    m_nTimeStep = pResult->Stage().Index();
    m_antype = pResult->AnalysisType();
    m_nComponent = pResult->ComponentIndex();
  }
}

CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentProxy(const CValueComponentProxy& rhs, rpn::CRpnStack& NewStack)
: IValueProxy(NewStack, rhs.ProxyId().toStdString().c_str()), 
  m_pObserver(0),
  m_sName(rhs.m_sName), 
  m_nIndex(rhs.m_nIndex),
  m_nComponent(rhs.m_nComponent),
  m_nTimeStep(rhs.m_nTimeStep),
  m_antype(rhs.m_antype),
  m_pModel(rhs.m_pModel),
  m_bStoreOnFile(rhs.m_bStoreOnFile),
  m_bFixedLinStage(rhs.m_bFixedLinStage)
{
  if(rhs.m_pObserver)
    m_pObserver = new CValueComponentObserver(rhs.m_pObserver->ValueComponent(), *this);
}

CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CValueComponentProxy(rpn::CRpnStack& NewStack, const QString& sProxyId)
: IValueProxy(NewStack, sProxyId.toStdString().c_str()), m_pObserver(0),
  m_nIndex(-1),
  m_nComponent(-1),
  m_nTimeStep(-1),
  m_antype(CAnalysisType::AT_NONLIN),
  m_pModel(0),
  m_bFixedLinStage(false)
{
}

CDerivedResult::CDerivedResultComponent::CValueComponentProxy::~CValueComponentProxy()
{
  if ( m_pObserver ) {
    m_pObserver->m_proxy = 0;
    delete m_pObserver;
  }
}

rpn::CRpnOperand::IValueProxy* CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Clone(rpn::CRpnStack& NewStack) const
{
  return new CValueComponentProxy(*this, NewStack);
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::HasValueComponent() const
{
  if(!m_pObserver)
    (const_cast<CValueComponentProxy*>(this))->Connect();

  return (m_pObserver != 0);
}

const IValueComponentBase& CDerivedResult::CDerivedResultComponent::CValueComponentProxy::ValueComponent() const
{
  if(!m_pObserver)
    (const_cast<CValueComponentProxy*>(this))->Connect();

  assert(m_pObserver);
  return m_pObserver->ValueComponent();
}

IValueComponentBase& CDerivedResult::CDerivedResultComponent::CValueComponentProxy::ValueComponent()
{
  if(!m_pObserver)
    Connect();

  assert(m_pObserver);
  return m_pObserver->ValueComponent();
}

void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version)
{
  assert(m_pObserver == 0);
  m_sName = ProxyId().toStdString().c_str();
  stream >> m_nIndex;
  stream >> m_nComponent;
  stream >> m_nTimeStep;

  int bLinear;
  stream >> bLinear;
  m_antype = (bLinear ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN);

  m_bFixedLinStage = true;
  if(CStreamVersion(3, 0, 96) < version)
  {
  int nFixedLinStage;
  stream >> nFixedLinStage;
  m_bFixedLinStage = (nFixedLinStage != 0);
  }
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Recursive(TParentSet stParent) const
{
  if(m_pObserver)
  {
    const CDerivedResultComponent *pComponent = dynamic_cast<const CDerivedResultComponent*>(&m_pObserver->ValueComponent());
    if(pComponent)
    {
      return pComponent->RpnStack().Recursive(stParent);
    }
  }
  return false;
}

void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::LoadStream(std::stringstream& stream, CStreamVersion& version)
{
  assert(m_pObserver == 0);

  // From version 0.0.0 (RpnVersion) the names stored not the same as the 
  // proxyid.
  if(CStreamVersion(0, 0, 0) < version)
    m_sName = LoadFromString(stream).toStdString().c_str();
  else
    m_sName = ProxyId();
  
  stream >> m_nIndex;
  stream >> m_nComponent;
  stream >> m_nTimeStep;

  if(version < CStreamVersion(1, 0, 6))
  {
  int bLinear;
    stream >> bLinear;
  m_antype = (bLinear ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN);
  }
  else
  {
  m_antype.LoadStream(stream, version);
  }

  m_bFixedLinStage = true;
  if(CStreamVersion(1, 0, 3) < version)
  {
  int nFixedLinStage;
  stream >> nFixedLinStage;
  m_bFixedLinStage = (nFixedLinStage != 0);
  }
}
      
void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::SaveStream(std::stringstream& stream)
{
  // The variables nTimeStep and antype are only used in case of results.
  int nIndex(-1), nComponent(-1), nTimeStep(-1);
  CAnalysisType antype(CAnalysisType::AT_NONLIN);
  
  // Calculate index and component
  if(!m_pObserver)
    Connect();

  if(m_pObserver)
  {
    nComponent = ValueComponent().ComponentIndex();
    nIndex = ValueComponent().Parent().Index();

    // If we have results we also save the depletion stage and bLinearity
    const IResultComponent* pResult = dynamic_cast<const IResultComponent*>(&ValueComponent());
    if(pResult)
    {
      nTimeStep = pResult->Stage().Index();
      antype = pResult->AnalysisType();
    }
  }
  else
  {
    nIndex = m_nIndex;
    nComponent = m_nComponent;
    nTimeStep = m_nTimeStep;
    antype = m_antype;
  }

  // Save to disc....
  SaveString(m_sName, stream);
  stream << nIndex << " ";
  stream << nComponent << " ";
  stream << nTimeStep << " ";
  antype.SaveStream(stream);
  stream << int(m_bFixedLinStage != false) << " ";
}
      
void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::AttachToModel(CFemAppModel& model)
{
  assert(m_pObserver == 0);
  m_pModel = dynamic_cast<CModelBase*>(&model);
  Connect();
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Defined() const
{
  return ValueComponent().Defined();
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::exists()
  const
{
  return (m_pObserver != 0);
}

IValueComposite* CDerivedResult::CDerivedResultComponent::CValueComponentProxy::FindResultByIndex(CResultGroup& rg) const
{
  assert(m_nIndex >= 0);

  IValueComposite* pComp = 0;

  int sz = rg.ChildSize();
  for(int i = 0; i < sz && !pComp; ++i)
  {
  CResultGroup* pGroup = dynamic_cast<CResultGroup*>(&rg.Child(i));
  if(pGroup)
      pComp = FindResultByIndex(*pGroup);
  else
  {
      assert(dynamic_cast<IResult*>(&rg.Child(i)));
      IResult& res = static_cast<IResult&>(rg.Child(i));
      if(res.Index() == m_nIndex)
    pComp = &res;
  }
  }

  return pComp;
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Connect()
{
  assert(m_pObserver == 0);
  assert(m_nComponent > -1);	// Assure valid

  // Look-up the value composite by index
  IValueComposite *pComposite = 0;

  // first try the results (of this model)
  if(m_nIndex >= 0)
  {
  CResultTree& rt = m_pModel->ResultTree();
  pComposite = FindResultByIndex(rt);
  }

  if(!pComposite)
  {
  // try generic composites (of root model)
    TValueCompositeEntry *pEntry = (TValueCompositeEntry*)(m_pModel->GraphEntry(MD_BASE_VALUE_COMPOSITE));
    assert(pEntry);
    TValueCompositeEntry::TNodeSet stNode = pEntry->EntryNodes();
    for(TValueCompositeEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      if(m_nIndex >= 0 && (*it)->Index() == m_nIndex)
      {
        pComposite = *it;
    break;
      }
      else if(m_nIndex < 0)
      {
    CDerivedResult* pRes = dynamic_cast<CDerivedResult*>(*it);
    if(pRes)
    {
          int i;
          for(i = 0; i < pRes->ComponentSize(); ++i)
          {
      QString sExportLabel = pRes->ExportLabel(i);
      if(sExportLabel == m_sName)
      {
              pComposite = pRes;
              if(pRes->Index() < 0)
        pRes->RegisterResult();
              assert(pRes->Index() >= 0);
              m_nIndex = pRes->Index();
              break;
      }
          }
    }
      }
    }
  }

  if(pComposite == 0)
    return false;

  // Find the component ...
  int nMode = 0;
  IValueComponentBase *pComponent = 0;
  IResult *pResult = dynamic_cast<IResult*>(pComposite);
  if(pResult)
  {
    // Get the depletion stage
    nMode = pResult->Mode(m_antype, m_nTimeStep); 
    if(nMode < 0)
  {
      if(IsResult() && m_bFixedLinStage)
        return false;

      // allow this, another mode will be selected
      nMode = 0;
  }
  }

  if(nMode >= pComposite->ModeSize() || m_nComponent >= pComposite->ComponentSize(nMode))
  return false;

  pComponent = &pComposite->Component(m_nComponent, nMode);
  assert(pComponent);	
  m_pObserver = new CValueComponentObserver(*pComponent, *this);
  return true;
}

QString CDerivedResult::CDerivedResultComponent::CValueComponentProxy::TextTag() const
{
   return m_sName;
}

unsigned int CDerivedResult::CDerivedResultComponent::CValueComponentProxy::IconId() const
{
  assert(m_pObserver || IsResult());
  // When a result is decoupled, we return the result icon id
  if(m_pObserver == 0)
    return IDI_RESULT;

  if(m_pObserver->ValueComponent().Parent().ComponentSize() > 1)
    return m_pObserver->ValueComponent().IconId();
  return m_pObserver->ValueComponent().Parent().IconId();
}

CDerivedResult::TValue CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Value(const geo::IPoint &pt, UNIT unit) const
{
  assert(m_pObserver || IsResult());

  // Try to connect
  if(m_pObserver == 0)
  {
    if(!(const_cast<CValueComponentProxy*>(this))->Connect())
      return TValue();
  }

  if(m_pObserver->ValueComponent().Defined())
    return m_pObserver->ValueComponent().ScalarData().ValuePoint(pt, unit);
  return TValue();
}

CDerivedResult::TValueVec CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Value(const geo::IElement &el, UNIT unit) const
{
  assert(m_pObserver || IsResult());
  // Try to connect
  if(m_pObserver == 0)
  {
    if(!(const_cast<CValueComponentProxy*>(this))->Connect())
      return TValueVec(el.NrOfPoints());
  }

  if(m_pObserver && m_pObserver->ValueComponent().Defined())
    return m_pObserver->ValueComponent().ScalarData().ValueElement(el, unit);
  return TValueVec(el.NrOfPoints());
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::CanDelete() const
{
  return true;
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::HasProperties() const
{
  if(m_pObserver == 0)
    (const_cast<CValueComponentProxy*>(this))->Connect();

  return m_pObserver != 0;
}

void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::StoreOnFile(bool bStoreOnFile)
{
  m_bStoreOnFile = bStoreOnFile;
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::IsResult() const
{
  return m_nTimeStep >= 0;
}

int CDerivedResult::CDerivedResultComponent::CValueComponentProxy::TypeId() const
{
  return VALUE_ID;
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::Properties(
  bool fixedLinStage, const IValueComponentBase* selection, const QString& name)
{
  assert(m_pObserver);
  assert(m_pModel);
  
  m_bFixedLinStage= fixedLinStage;

    assert(selection);
    delete m_pObserver;
    m_pObserver = new CValueComponentObserver(const_cast<IValueComponentBase&>(*selection), *this);
    m_pModel = dynamic_cast<CModelBase*>(&const_cast<IValueComponentBase&>(*selection).Model());
    assert(m_pModel);

    // When we have a result component we store the index, the component index, timestep and linearity,
    // because when the depletion stage changes the result components are decoupled.
    const IResultComponent* pResult = dynamic_cast<const IResultComponent*>(&ValueComponent());
    if(pResult)
    {
      m_nIndex = pResult->Parent().Index();
      assert(m_nIndex > 0);
      m_nTimeStep = pResult->Stage().Index();
      m_antype = pResult->AnalysisType();
      m_nComponent = pResult->ComponentIndex();
    }
    else
    {
      m_nIndex = -1;
      m_nTimeStep = -1;
      m_nComponent = -1;
    }

    m_sName = name;

    return true;
}

void CDerivedResult::CDerivedResultComponent::CValueComponentProxy::SetAnalysisTypeAndStage(const CAnalysisType& antype, int nStage)
{
  if(!m_bFixedLinStage && (m_antype != antype || m_nTimeStep != nStage))
  {
  m_antype = antype;
  m_nTimeStep = nStage;
  delete m_pObserver;
  m_pObserver = 0;
  }
}

bool CDerivedResult::CDerivedResultComponent::CValueComponentProxy::
  StoreOnFile() const
{
  return m_bStoreOnFile;
}

rpn::CRpnObject *CDerivedResult::CDerivedResultComponent::CValueComponentProxy::GetExpandedRpnObject(rpn::CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  if(!m_pObserver)
  {
  lstMessages.push_back(std::string("The operand named '") + m_sName.toStdString() + "' is referring to a non-existing result. The value 1 is inserted at this location");
  rpn::CRpnConstantOperand* pOp = new rpn::CRpnConstantOperand;
  pOp->One();
  return pOp;
  }

  const CDerivedResultComponent *p = dynamic_cast<const CDerivedResultComponent *>(&ValueComponent());
  if(!p)
  return 0;
  CDerivedResultComponent *pDerResComponent = const_cast<CDerivedResultComponent *>( p);

  // the stack of this proxy (local)
  rpn::CRpnStack *stackToExpand= const_cast<rpn::CRpnStack*>(&pDerResComponent->RpnStack());
  rpn::CRpnObject *object = stackToExpand->Top().GetExpandedRpnObject(targetstack, lstMessages);

  return object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CDerivedResult::CDerivedResultComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDerivedResult::CDerivedResultComponent::CDerivedResultComponent(CDerivedResult& parent,
                                                                 const CDepletionStage& stage,
                                                                 const CAnalysisType& antype,
                                                                 int nRegister)
: IResultComponent("", parent, stage, antype, nRegister, 0)
{
}

CDerivedResult::CDerivedResultComponent::~CDerivedResultComponent()
{
}

// waij TFS 74558
// Expand MIES = NOOT + 1; NOOT = AAP + 2; AAP = 3
// to
// MIES = (3 + 2) + 1
//
void CDerivedResult::CDerivedResultComponent::RecursiveExpand(std::list<std::string>& lstMessages)
{
  rpn::CRpnStack* pStack = Stack();
  assert(pStack);
  if(!pStack || pStack->StackSize() != 1)
  return;

  pStack->Expand(lstMessages);
}

void CDerivedResult::CDerivedResultComponent::MapValueElement // NOT CASCADING PARALLELINITIALIZATION
  (const geo::IElement& elm
  , TValueVec& values
  , IVectorResult::TMapType /*map_type*/
  , UNIT unit //= CQuantity::SI_UNIT
  , geo::IParallelInitializationCallback* /*cb*/
  ) const
{
  (const_cast<CDerivedResultComponent&>(*this)).MakeCurrent(false);
  values= RpnStack().Value( elm, unit);
}

void CDerivedResult::CDerivedResultComponent::SaveStream(std::stringstream& stream)
{
  // Save the coordinate proxies
  rpn::CProxyPersistant<CResultCoordinateProxy> psCoordinate;
  psCoordinate.SaveStream(*Stack(), stream);

  // Save the formation proxies
  rpn::CProxyPersistant<CRpnFormationProxy> psFormation;
  psFormation.SaveStream(*Stack(), stream);
  rpn::CProxyPersistant<CRpnFormationThicknessProxy> psFormationThickness;
  psFormationThickness.SaveStream(*Stack(), stream);

  // Save the valueset proxies
  rpn::CProxyPersistant<CRpnValueSet::CValueSetProxy> psValueSet;
  psValueSet.SaveStream(*Stack(), stream);
  // save pointset ids
  rpn::CProxyPersistant<CRpnValueSet::CValueSetProxy>::TProxyVec vcProxies = psValueSet.Proxies(*Stack());
  stream << int(vcProxies.size()) << " ";
  for(size_t i = 0; i < vcProxies.size(); ++i)
  stream << vcProxies[i]->ValueSet().PointSet().Index() << " ";

  // Save the value component proxies
  rpn::CProxyPersistant<CValueComponentProxy> psValueComp;
  psValueComp.SaveStream(*Stack(), stream);

  // Save the reservoir proxy
  rpn::CProxyPersistant<CRpnReservoirProxy> psReservoir;
  psReservoir.SaveStream(*Stack(), stream);

  // Load the material parameters, Young, Poisson, Density, Cohesion and Friction angle
  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TYoungsModulus> > psYoung;
  psYoung.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPoissonsRatio> > psPoisson;
  psPoisson.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TDensity> > psDensity;
  psDensity.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TCohesion> > psCohesion;
  psCohesion.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TFrictionAngle> > psFriction;
  psFriction.SaveStream(*Stack(), stream);

  // mantis 2517 wjrx
  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TBulkStiffness> > psBulkStiffness;
  psBulkStiffness.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TShearModulus> > psShearModulus;
  psShearModulus.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPorosity> > psPorosity;
  psPorosity.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TInitFriction> > psInitFriction;
  psInitFriction.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<THardening> > psHardening;
  psHardening.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TCapShape> > psCapShape;
  psCapShape.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TDilatation> > psDilatation;
  psDilatation.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPreConsolidation> > psPreConsolidation;
  psPreConsolidation.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TLatRatioMax> > psLatRatioMax;
  psLatRatioMax.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TLatRatioMin> > psLatRatioMin;
  psLatRatioMin.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TAzimuth> > psAzimuth;
  psAzimuth.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TTensileStretch> > psTensileStretch;
  psTensileStretch.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TThermalExpansion> > psThermalExpansion;
  psThermalExpansion.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TElasticHardening> > psElasticHardening;
  psElasticHardening.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TSecondaryPreconsolidation> > psSecondaryPreconsolidation;
  psSecondaryPreconsolidation.SaveStream(*Stack(), stream);

  rpn::CProxyPersistant<CRpnMaterialParameterProxy<TSecondaryHardening> > psSecondaryHardening;
  psSecondaryHardening.SaveStream(*Stack(), stream);

  saveStream109(*Stack(), stream);

  if (Parent().Model().parentModel() == 0 && static_cast<CDerivedResult&>(Parent()).StoreOnFile()) // only in top model, only on file
  {
  rpn::CProxyPersistant<CRpnTopDisplacementProxy <0, IDS_MEASURED_TOP_DISPL_N> > psTDN;
  psTDN.SaveStream(*Stack(), stream);
  rpn::CProxyPersistant<CRpnTopDisplacementProxy <1, IDS_MEASURED_TOP_DISPL_E> > psTDE;
  psTDE.SaveStream(*Stack(), stream);
  rpn::CProxyPersistant<CRpnTopDisplacementProxy <2, IDS_MEASURED_TOP_DISPL_D> > psTDD;
  psTDD.SaveStream(*Stack(), stream);
  }

  if((static_cast<const CDerivedResult&>(Parent())).StoreOnFile())
  Stack()->SaveStream(stream);
}

void CDerivedResult::CDerivedResultComponent::LoadStream(std::stringstream& stream, rpn::CRpnVersion& version)
{
  // Create a stack assuming that we don't having any ...
  if(Stack() == 0)
    CreateStack();

  if(rpn::CRpnVersion(0, 0, 0) < version)
  {
    // Load coordinate proxies
    rpn::CProxyPersistant<CResultCoordinateProxy> psCoordinate;
    psCoordinate.LoadStream(*Stack(), stream, version);
  }
  else
  {
    // In the old case we create coordinates with know proxy id
    new CResultCoordinateProxy(IDS_RC_NORTHING, *Stack(), CResultCoordinateProxy::CO_X, "Northing");
    new CResultCoordinateProxy(IDS_RC_EASTING, *Stack(), CResultCoordinateProxy::CO_Y, "Easting");
    new CResultCoordinateProxy(IDS_RC_DEPTH, *Stack(), CResultCoordinateProxy::CO_Z, "Depth");
  }

  if(rpn::CRpnVersion(0, 0, 0) < version)
  {
    // Load formation proxies
    rpn::CProxyPersistant<CRpnFormationProxy> psFormation;
    psFormation.LoadStream(*Stack(), stream, version);
  }

  if(rpn::CRpnVersion(1, 0, 6) < version)
  {
  rpn::CProxyPersistant<CRpnFormationThicknessProxy> psFormationThickness;
  psFormationThickness.LoadStream(*Stack(), stream, version);
  }

  if(rpn::CRpnVersion(1, 0, 7) < version)
  {
  // load valueset proxies
  rpn::CProxyPersistant<CRpnValueSet::CValueSetProxy> psValueSet;
  psValueSet.LoadStream(*Stack(), stream, version);
  // load pointset ids
  rpn::CProxyPersistant<CRpnValueSet::CValueSetProxy>::TProxyVec vcProxies = psValueSet.Proxies(*Stack());
  int nps;
  stream >> nps;
  assert(nps == vcProxies.size());
  int i;
  for(i = 0; i < nps; ++i)
  {
      int idx;
      stream >> idx;
      TPointSetEntry& ps_entry = (TPointSetEntry&)(*Model().GraphEntry(MD_BASE_POINTSET));
      IPointSet* ps = ps_entry.FindIndex(idx);
      assert(ps);
      vcProxies[i]->AttachPointSet(*ps);
  }
  }

  // Load the value proxies
  rpn::CProxyPersistant<CValueComponentProxy> psValueComponent;
  psValueComponent.LoadStream(*Stack(), stream, version);
  rpn::CProxyPersistant<CValueComponentProxy>::TProxyVec vcProxy = psValueComponent.Proxies(*Stack());
  const CDerivedResult& result = dynamic_cast<const CDerivedResult&>(Parent());
  for(size_t i = 0; i < vcProxy.size(); i++)
    vcProxy[i]->StoreOnFile(result.StoreOnFile());

  if(rpn::CRpnVersion(0, 0, 0) < version)
  {
    // Load formation proxies
    rpn::CProxyPersistant<CRpnReservoirProxy> psReservoir;
    psReservoir.LoadStream(*Stack(), stream, version);
  }
  else
  {
    // Last we insert IsReservoir
    CModelBase& model = dynamic_cast<CModelBase&>(Model());
    new CRpnReservoirProxy(model.Mesh(), *Stack(), "IsReservoir");
  }

  if(rpn::CRpnVersion(1, 0, 1) < version) {
    // Load the material parameters, Young, Poisson, Density, Cohesion and Friction angle
    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TYoungsModulus> > psYoung;
    psYoung.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPoissonsRatio> > psPoisson;
    psPoisson.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TDensity> > psDensity;
    psDensity.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TCohesion> > psCohesion;
    psCohesion.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TFrictionAngle> > psFriction;
    psFriction.LoadStream(*Stack(), stream, version);
  }

  // mantis 2517 wjrx
  if (version >= rpn::CRpnVersion(1, 0, 3) ) //Check against rpn version (rpn_version.h)
  {
    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TBulkStiffness> > psBulkStiffness;
    psBulkStiffness.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TShearModulus> > psShearModulus;
    psShearModulus.LoadStream(*Stack(), stream, version);
    
    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPorosity> > psPorosity;
    psPorosity.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TInitFriction> > psInitFriction;
    psInitFriction.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<THardening> > psHardening;
    psHardening.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TCapShape> > psCapShape;
    psCapShape.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TDilatation> > psDilatation;
    psDilatation.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TPreConsolidation> > psPreConsolidation;
    psPreConsolidation.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TLatRatioMax> > psLatRatioMax;
    psLatRatioMax.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TLatRatioMin> > psLatRatioMin;
    psLatRatioMin.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TAzimuth> > psAzimuth;
    psAzimuth.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TTensileStretch> > psTensileStretch;
    psTensileStretch.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TThermalExpansion> > psThermalExpansion;
    psThermalExpansion.LoadStream(*Stack(), stream, version);
  }

  if(version >= rpn::CRpnVersion(1, 0, 5))
  {
    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TElasticHardening> > psElasticHardening;
    psElasticHardening.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TSecondaryPreconsolidation> > psSecondaryPreconsolidation;
    psSecondaryPreconsolidation.LoadStream(*Stack(), stream, version);

    rpn::CProxyPersistant<CRpnMaterialParameterProxy<TSecondaryHardening> > psSecondaryHardening;
    psSecondaryHardening.LoadStream(*Stack(), stream, version);
  }

  // moved source code to a separate source file due to:
  //   fatal error C1128: number of sections exceeded object file format limit :
  //   compile with /bigobj

  loadStream109(*Stack(), stream, version);

  bool bIsTopModel  = Parent().Model().parentModel() == 0;
  bool bIsRegResult = !static_cast<CDerivedResult&>(Parent()).StoreOnFile();

  if(version >= rpn::CRpnVersion(1, 0, 10))
  {
  if (version < rpn::CRpnVersion(1, 0, 11) || (bIsTopModel && !bIsRegResult))
  {
      rpn::CProxyPersistant<CRpnTopDisplacementProxy <0, IDS_MEASURED_TOP_DISPL_N> > psTDN;
      psTDN.LoadStream(*Stack(), stream, version);
      rpn::CProxyPersistant<CRpnTopDisplacementProxy <1, IDS_MEASURED_TOP_DISPL_E> > psTDE;
      psTDE.LoadStream(*Stack(), stream, version);
      rpn::CProxyPersistant<CRpnTopDisplacementProxy <2, IDS_MEASURED_TOP_DISPL_D> > psTDD;
      psTDD.LoadStream(*Stack(), stream, version);

      if (!bIsTopModel || bIsRegResult) // 1.0.10 erroneously loads top displacements for zoom-in and/or registry results, so we remove them here
      {
    rpn::CProxyPersistant<CRpnTopDisplacementProxy <0, IDS_MEASURED_TOP_DISPL_N> >::TProxyVec proxiesTDN(psTDN.Proxies(*Stack()));
    for (size_t i = 0; i < proxiesTDN.size(); ++i)
    {
          Stack()->DetachValueProxy(*proxiesTDN[i]);
    }

    rpn::CProxyPersistant<CRpnTopDisplacementProxy <1, IDS_MEASURED_TOP_DISPL_E> >::TProxyVec proxiesTDE(psTDE.Proxies(*Stack()));
    for (size_t i = 0; i < proxiesTDE.size(); ++i)
    {
          Stack()->DetachValueProxy(*proxiesTDE[i]);
    }

    rpn::CProxyPersistant<CRpnTopDisplacementProxy <2, IDS_MEASURED_TOP_DISPL_D> >::TProxyVec proxiesTDD(psTDD.Proxies(*Stack()));
    for (size_t i = 0; i < proxiesTDD.size(); ++i)
    {
          Stack()->DetachValueProxy(*proxiesTDD[i]);
    }
      }
  }
  }

  if(version < rpn::CRpnVersion(1, 0, 4) || (static_cast<const CDerivedResult&>(Parent())).StoreOnFile())
  {
    // Load the formula ....
  Stack()->LoadStream(stream, version, *Stack());
  }

  Modified();
}

void CDerivedResult::CDerivedResultComponent::UpdateFormations()
{
  // As third we check the formations
  TFormationBaseEntry* pEntry = (TFormationBaseEntry*)Model().GraphEntry(MD_BASE_FORMATION);
  assert(pEntry);
  TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    // Do we have already this formation?
    bool bCreateFormationProxy = true;
  bool bCreateThicknessProxy = true;
    rpn::CRpnStack::TValueProxyVec vcProxy = RpnStack().ValueProxies();
    for(size_t i = 0; i < vcProxy.size(); i++)
    {
      CRpnFormationProxy *pProxy = dynamic_cast<CRpnFormationProxy*>(vcProxy[i]);
      if(pProxy && &pProxy->Formation() == *it)
        bCreateFormationProxy = false;

      CRpnFormationThicknessProxy* pThicknessProxy = dynamic_cast<CRpnFormationThicknessProxy*>(vcProxy[i]);
      if(pThicknessProxy && &pThicknessProxy->Formation() == *it)
    bCreateThicknessProxy = false;
    }

    if(bCreateFormationProxy)
      new CRpnFormationProxy(**it, *Stack());
  if(bCreateThicknessProxy)
      new CRpnFormationThicknessProxy(**it, *Stack());
  }
}

geo::CValue CDerivedResult::CDerivedResultComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback* /*cb*/) const // NOT CASCADING PARALLELINITIALIZATION
{
  (const_cast<CDerivedResultComponent*>(this))->MakeCurrent(false);
  return RpnStack().Value(pt, unit);
}

IValueDomainScalar::TValueVec CDerivedResult::CDerivedResultComponent::ValueElement(const geo::IElement& elm, const UNIT unit, geo::IParallelInitializationCallback* /*cb*/) const // NOT CASCADING PARALLELINITIALIZATION
{
  (const_cast<CDerivedResultComponent*>(this))->MakeCurrent(false);
  return RpnStack().Value(elm, unit);
}

unsigned int CDerivedResult::CDerivedResultComponent::IconId() const
{
  // Single components should not be visible in tree
  return 0;
}

unsigned int CDerivedResult::CDerivedResultComponent::TypeId() const
{
   return 0;
}

bool CDerivedResult::CDerivedResultComponent::Empty() const
{
  return Stack() == 0;
}

const QString& CDerivedResult::CDerivedResultComponent::Name() const
{
  return Parent().Name();
}

void CDerivedResult::CDerivedResultComponent::Name(const QString &strName)
{
  Parent().Name(strName);
}

long CDerivedResult::CDerivedResultComponent::SavedItems() const
{
  long lRet = IValueComponentBase::SavedItems();
  rpn::CRpnStack::TValueProxyVec vcProxy = RpnStack().ValueProxies();
  for(size_t i = 0; i < vcProxy.size(); i++)
  {
    if(dynamic_cast<CValueComponentProxy*>(vcProxy[i]))
      ++lRet;

    if(dynamic_cast<CRpnFormationProxy*>(vcProxy[i]))
      ++lRet;

  if(dynamic_cast<CRpnFormationThicknessProxy*>(vcProxy[i]))
      ++lRet;
  }
  return lRet;
}

QString CDerivedResult::CDerivedResultComponent::LoadFromString(std::stringstream& stream)
{
  // Read the value name
  std::string sName;

  // Search for the first "
  while(stream.get() != '\"');

  // Read string till next "
  char ch;
  do {
    ch = stream.get();
    if(ch != '\"')
      sName.append(1, ch);
  }while(ch != '\"');

  return sName.c_str();
}

void CDerivedResult::CDerivedResultComponent::SaveString(std::stringstream& stream, const QString& string)
{
  stream << '\"' << string.toStdString() << "\" ";
}

void CDerivedResult::CDerivedResultComponent::RpnVersionFromFileVersion(TSTREAM& /*stream*/, const CStreamVersion& version, std::vector<rpn::CRpnVersion>& vcRpnVersion) const
{
  vcRpnVersion.clear();

  // wedx 11012008
  // Unfortunately a mistake has been made in the past because of which we have to guess the RPN version information

  if(version == CStreamVersion(3, 0, 26))
  {
  // 1.0.0 or 1.0.1
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 0));
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 1));
  }
  else if(CStreamVersion(3, 0, 26) < version && version < CStreamVersion(3, 0, 76))
  {
  // 1.0.1
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 1));
  }
  else if(version == CStreamVersion(3, 0, 76))
  {
  // 1.0.1 or 1.0.2
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 1));
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 2));
  }
  else if(CStreamVersion(3, 0, 76) < version && version < CStreamVersion(3, 0, 90))
  {
  // 1.0.2
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 2));
  }
  else if(version == CStreamVersion(3, 0, 90))
  {
  // 1.0.2 or 1.0.3
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 2));
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 3));
  }
  else
  {
  // In principle we shouldn't get here, because RPN version information is properly stored in the stream file
  // since version 3.0.91.

  // 1.0.3
  vcRpnVersion.push_back(rpn::CRpnVersion(1, 0, 3));
  }
}

void CDerivedResult::CDerivedResultComponent::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // loading base
  IValueComponentBase::LoadStream(stream, version, progress);

  if(CStreamVersion(3, 0, 25) < version)
  {
  std::vector<rpn::CRpnVersion> vcRpnVersion;

  if(CStreamVersion(3, 0, 90) < version)
  {
    // Read RPN version info from the stream
    int nMajor, nMinor, nRevision;
    stream >> nMajor;
    stream >> nMinor;
    stream >> nRevision;
      vcRpnVersion.push_back(rpn::CRpnVersion(nMajor, nMinor, nRevision));
  }
  else
  {
      // figure it out...
      RpnVersionFromFileVersion(stream, version, vcRpnVersion);
  }

    // Stack is created in the load stream function
    QString sBuffer;
    stream >> sBuffer;
    std::stringstream string_stream(sBuffer.toStdString());

  if(version != CStreamVersion(3, 0, 97)) // erroneous format saved in the string stream, skip
  {

      char* pEnv = DiGetenv("GEOMEC_LOAD_DERIVED_RESULTS");
      if(pEnv && vcRpnVersion.size() == 2)
      {
    if(!strcmp(pEnv, "NO"))
    {
          progress.StatusMessage("Skipping derived results"); // don't load them
    }
    else if(!strcmp(pEnv, "ALTERNATIVE"))
    {
          progress.StatusMessage(QString("Loading derived results (forcing version (%1))").arg(vcRpnVersion[1].GetVersionString()));
          LoadStream(string_stream, vcRpnVersion[1]);
    }
    else
    {
          progress.StatusMessage(QString("Loading derived results (forcing version (%1))").arg(vcRpnVersion[0].GetVersionString()));
          LoadStream(string_stream, vcRpnVersion[0]);
    }
      }
      else
      {
    progress.StatusMessage("Loading derived results");
    LoadStream(string_stream, vcRpnVersion[0]);
      }

  }
  }
  else
  {
    // Create a stack assuming that we don't having any ...
    if(Stack() == 0)
      CreateStack();

    // In the old case we create coordinates with know proxy id
    new CResultCoordinateProxy(IDS_RC_NORTHING, *Stack(), CResultCoordinateProxy::CO_X, "Northing");
    new CResultCoordinateProxy(IDS_RC_EASTING, *Stack(), CResultCoordinateProxy::CO_Y, "Easting");
    new CResultCoordinateProxy(IDS_RC_DEPTH, *Stack(), CResultCoordinateProxy::CO_Z, "Depth");

    // Fetch the value proxies
    int nSize1;
    stream >> nSize1;
    for(int i = 0; i < nSize1; i++)
    {
      // Create a proxy and load him
      QString sProxyId;
      stream >> sProxyId;
      CValueComponentProxy *pProxy = new CValueComponentProxy(*Stack(), sProxyId.toStdString().c_str());
      pProxy->LoadStream(stream, version);
      pProxy->StoreOnFile(true);
      progress.Step();
    }

    // From 3.0.22 we have formation proxy
    if(CStreamVersion(3,0,21) < version)
    {
      int nSize2;
      stream >> nSize2;
      for(int i = 0; i < nSize2; i++)
      {
        // Create a proxy and load him
        QString sProxyId;
        stream >> sProxyId;
        CRpnFormationProxy *pProxy = new CRpnFormationProxy(*Stack(), sProxyId.toStdString().c_str());
        pProxy->LoadStream(stream, version);
        progress.Step();
      }
    }

    // Last we insert IsReservoir
    CModelBase& model = dynamic_cast<CModelBase&>(Model());
    new CRpnReservoirProxy(model.Mesh(), *Stack(), "IsReservoir");

    // Load the formula ....
    QString sFormula;
    stream >> sFormula;
    std::stringstream string_stream(sFormula.toStdString());
    Stack()->LoadStream(string_stream, version, *Stack());
  }
}

void CDerivedResult::CDerivedResultComponent::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save base ...
  IValueComponentBase::SaveStream(stream, progress);

  // save rpn version info
  int nMajor = RPN_VERSION_MAJOR;
  int nMinor = RPN_VERSION_MINOR;
  int nRevision = RPN_VERSION_REVISION;
  stream << nMajor;
  stream << nMinor;
  stream << nRevision;

  // Save the string
  std::stringstream string_stream;
  SaveStream(string_stream);
  stream << QString(string_stream.str().c_str());
}

QString CDerivedResult::CDerivedResultComponent::ExportLabel() const
{
  QString nm = Name().trimmed();

  bool bAppend = false;

  AppendMaterialParameterExportLabel(bAppend);
  AppendValueComponentExportLabel(bAppend);

  if(bAppend)
  {
  if(nm.indexOf(' ') > -1)
      nm = QString("[") + nm + "]";
  nm = QString("%1_D%2_%3").arg(nm).arg(Stage().Index()).arg(
      AnalysisType().ExportCharacter());
  }

  return nm;
}

void CDerivedResult::CDerivedResultComponent::AppendMaterialParameterExportLabel(bool &bAppend) const
{
  std::vector<IRpnMaterialParameterProxy*> vcProxies;
  CollectProxies<IRpnMaterialParameterProxy>(vcProxies);
  for(size_t i = 0; i < vcProxies.size(); ++i)
  {
  if(!vcProxies[i]->FixedStage())
  {
      bAppend = true;
      break;
  }
  }
}

void CDerivedResult::CDerivedResultComponent::AppendValueComponentExportLabel(bool &bAppend) const
{
  std::vector<CValueComponentProxy*> vcProxies;
  CollectProxies<CValueComponentProxy>(vcProxies);
  for(size_t i = 0; i < vcProxies.size(); ++i)
  {
  if(vcProxies[i]->IsResult() && !vcProxies[i]->FixedLinearityAndStage())
  {
      bAppend = true;
      break;
  }
  }
}

QString CDerivedResult::CDerivedResultComponent::UnitName(const UNIT /*unit*/) const
{
  return QString("User defined unit");
}

bool CDerivedResult::CDerivedResultComponent::Defined() const
{
  if(Stack() == 0)
  return false;


//  (const_cast<CDerivedResultComponent&>(*this)).MakeCurrent();

  return true;
/*
  return Stack()->Defined();
*/
}

IValueDomainScalar::TMinMax CDerivedResult::CDerivedResultComponent::MinMax(IProgressBase& /*progressBase*/, const UNIT /*unit*/ ) const
{
  return IValueDomainScalar::TMinMax();
}

const rpn::CRpnStack* CDerivedResult::CDerivedResultComponent::Stack() const
{
  const CDerivedResult& parent = static_cast<const CDerivedResult&>(Parent());
  return parent.RpnStack();
}

rpn::CRpnStack* CDerivedResult::CDerivedResultComponent::Stack()
{
  CDerivedResult& parent = static_cast<CDerivedResult&>(Parent());
  return parent.RpnStack();
}

bool CDerivedResult::CDerivedResultComponent::MakeCurrent(bool bSendModified)
{
  bool bModified = false;

  MakeCurrentValueComponentProxies(bModified);
  MakeCurrentMaterialParameterProxies(bModified);

  static bool bSendingModified = false;

  if(bModified && bSendModified && !bSendingModified)
  {
  bSendingModified = true;
  Modified();
  bSendingModified = false;
  }

  return true;
}

void CDerivedResult::CDerivedResultComponent::MakeCurrentMaterialParameterProxies(bool &bModified)
{
  std::vector<IRpnMaterialParameterProxy*> vcProxies;
  CollectProxies<IRpnMaterialParameterProxy>(vcProxies);

  for(size_t i = 0; i < vcProxies.size(); ++i)
  {
  IRpnMaterialParameterProxy* pVCProxy = vcProxies[i];

  // check if the proxy is modifiable
  if(!pVCProxy->FixedStage())
  {
      if(!pVCProxy->TimeStep() || pVCProxy->TimeStep()->Index() != Stage().Index())
      {
    pVCProxy->TimeStep(&Stage());
    bModified = true;
      }
  }
  }
}

void CDerivedResult::CDerivedResultComponent::MakeCurrentValueComponentProxies(bool &bModified)
{
  std::vector<CValueComponentProxy*> vcProxies;
  CollectProxies<CValueComponentProxy>(vcProxies);

  for(size_t i = 0; i < vcProxies.size(); ++i)
  {
  CValueComponentProxy* pVCProxy = vcProxies[i];

  // check if the proxy is modifiable
  if(pVCProxy->IsResult() && !pVCProxy->FixedLinearityAndStage())
  {
      if(pVCProxy->AnalysisType() != AnalysisType() || pVCProxy->Stage() != Stage().Index())
      {
    pVCProxy->SetAnalysisTypeAndStage(AnalysisType(), Stage().Index());
    bModified = true;
      }
  }

  if(pVCProxy->HasValueComponent())
  {
      CDerivedResultComponent* pDerivedResultComp = dynamic_cast<CDerivedResultComponent*>(&pVCProxy->ValueComponent());
      if(pDerivedResultComp)
    pDerivedResultComp->MakeCurrent(false);
  }
  }
}

void CDerivedResult::CDerivedResultComponent::CreateStack(const rpn::CRpnStack* pSource)
{
  CDerivedResult& parent = static_cast<CDerivedResult&>(Parent());
  parent.CreateStack(pSource);
}

void CDerivedResult::CDerivedResultComponent::RpnStack(const rpn::CRpnStack& stack)
{
  CreateStack(&stack);
  Modified();
}

const rpn::CRpnStack& CDerivedResult::CDerivedResultComponent::RpnStack() const
{
  assert(Stack());
  return *Stack();
}

void CDerivedResult::CDerivedResultComponent::AttachToModel(CModelBase& model)
{
  if(Stack())
  {
    // Walk over RPN stack and enable the shit
    Stack()->AttachToModel(model);
  }
}

//////////////////////////////////////////////////////////////////////
// Implementation of CDerivedResult
//////////////////////////////////////////////////////////////////////

CDerivedResult::CDerivedResult(CDerivedResultGroup& group)
: IResult("", group), m_pGroup(&group), m_pStack(0)
{
  BuildComponents();
  assert(Index() == -1);
}

CDerivedResult::~CDerivedResult()
{
  delete m_pStack;
}

QString CDerivedResult::ExportLabel(int nComponent) const
{
  return  Component(nComponent).Name();
}

IResultComponent* CDerivedResult::OnGetResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const
{
  IResultComponent* pComponent = IResult::OnGetResultComponent(nTimeStep, antype, nRegister, nComponent);

  if(pComponent)
  {
  CDerivedResultComponent* pDComp = dynamic_cast<CDerivedResultComponent*>(pComponent);
  if(pDComp)
  {
      assert(pDComp->AnalysisType() == antype);
      assert(pDComp->Stage().Index() == nTimeStep);
      pDComp->MakeCurrent();
  }
  }

  return pComponent;
}

unsigned int CDerivedResult::IconId() const
{
   return IDI_RESULT;
}

unsigned int CDerivedResult::TypeId() const
{
  return 0;
}

bool CDerivedResult::RegisterResult() 
{
  Index(m_pGroup->UniqueIndex());
  return Index() >= 0;
}

bool CDerivedResult::StoreOnFile() const
{
  return m_pGroup->StoreOnFile();
}

bool CDerivedResult::Empty() const
{
   return Component(0, 0).Empty();
}

long CDerivedResult::SavedItems() const
{
  long lRet = IValueComposite::SavedItems();
  lRet += Component(0, 0).SavedItems();
  return lRet;
}

void CDerivedResult::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  IValueComposite::LoadStream(stream, version, progress);
  Component(0, 0).LoadStream(stream, version, progress);
}

void CDerivedResult::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IValueComposite::SaveStream(stream, progress);
  Component(0, 0).SaveStream(stream, progress);
}

void CDerivedResult::LoadFromRegistry(const QString& sResultKey)
{
  // Set registry key

  // Load formula name from the register
  ISettings* registry = ISettings::instance();
  QString sBuffer = registry->getProfileString(RESULT_SECTION, sResultKey, "");
  assert(!sBuffer.isEmpty());
  std::stringstream stream(sBuffer.toStdString());
  LoadStream(stream);

  m_sResultKey = sResultKey;
}

void CDerivedResult::LoadStream(std::stringstream& stream)
{
  // Read version
  int nMajor, nMinor, nRevision;
  stream >> nMajor;
  stream >> nMinor;
  stream >> nRevision;
  rpn::CRpnVersion version(nMajor, nMinor, nRevision);

  // From version 1.0.1 the reference is saved in the string
  if(rpn::CRpnVersion(1, 0, 0) < version)
  {
    int nIndex;
    stream >> nIndex;
    Index(nIndex);
    assert(StoreOnFile() || (nIndex >= REGISTRY_RESULT_BASE));
    assert(!StoreOnFile() || (nIndex >= COMPOSITE_BASE));
  }

  // Read the result name
  std::string sName;

  // Search for the first "
  while(stream.get() != '\"');

  // Read string till next "
  char ch;
  do {
    ch = stream.get();
    if(ch != '\"')
      sName.append(1, ch);
  }while(ch != '\"');

  Name(sName.c_str());

  CreateStack();

  CDerivedResultComponent& component = dynamic_cast<CDerivedResultComponent&>(Component());
  component.LoadStream(stream, version);

  if(rpn::CRpnVersion(1, 0, 3) < version)
  {
  m_pStack->LoadStream(stream, version, *m_pStack);
  }
}

void CDerivedResult::SaveStream(std::stringstream& stream)
{
  // We first save the version number
  int nMajor = RPN_VERSION_MAJOR;
  int nMinor = RPN_VERSION_MINOR;
  int nRevision = RPN_VERSION_REVISION;
  stream << nMajor << " " << nMinor << " " << nRevision << " ";

  // Save index. 
  assert(Index() >= 0);
  stream << Index() << " ";

  // Then we save the result name
  std::string sName(Name().toStdString());
  stream << '\"' << sName << "\" ";

  // Lastly we save the component ...
  CDerivedResultComponent& component = dynamic_cast<CDerivedResultComponent&>(Component());
  component.SaveStream(stream);

  m_pStack->SaveStream(stream);
}

void CDerivedResult::SaveToRegistry()
{
  ISettings* registry = ISettings::instance();
  std::stringstream stream;
  SaveStream(stream);

  // Do we have an entry? If not generate one
  if(m_sResultKey.isEmpty())
  {
    int n = 0;
    QString sResultKey;
    do {
      sResultKey = QString(RESULT_KEY).arg(n);

      n++;

    }while(!registry->getProfileString(RESULT_SECTION, sResultKey, "").isEmpty());
    m_sResultKey = sResultKey;
  }

  // Save to register
  registry->writeProfileString(RESULT_SECTION, m_sResultKey, stream.str().c_str());
}

void CDerivedResult::AttachToModel(CModelBase& model)
{
  CDerivedResultComponent *pComponent = dynamic_cast<CDerivedResultComponent*>(&Component());
  assert(pComponent);
  pComponent->AttachToModel(model);
}

bool CDerivedResult::CanExport() const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
  assert(pModel);
  return pModel->ResultRegister().ResultsAvailable();
}

bool CDerivedResult::Destroy()
{
  if(!StoreOnFile())
  {
    // Store all derived result in registry except this one
    int i = 0;
  int n;
  for(n = 0; n < m_pGroup->ChildSize(); ++n)
    {
      assert(dynamic_cast<CDerivedResult*>(&m_pGroup->Child(n)));
      CDerivedResult *pResult = static_cast<CDerivedResult*>(&m_pGroup->Child(n));;
      if(pResult != this)
      {	
        pResult->m_sResultKey = QString(RESULT_KEY).arg(i);
        pResult->SaveToRegistry();
        i++;
      }
    }

    QString sKey;
    sKey = QString(RESULT_KEY).arg(i);
    ISettings* registry = ISettings::instance();
    registry->writeProfileString(RESULT_SECTION, sKey, "");
  }

  return IValueComposite::Destroy();
}

bool CDerivedResult::CanMap(const COpenGLNode& /*node*/, int /*nRegister*/) const
{
  return true; //TODO: Component().CanMap(node);
}

const rpn::CRpnStack* CDerivedResult::RpnStack() const
{
  return m_pStack;
}

rpn::CRpnStack* CDerivedResult::RpnStack()
{
  return m_pStack;
}

void CDerivedResult::CreateStack(const rpn::CRpnStack* pSource)
{
  delete m_pStack;

  if(pSource)
  m_pStack = new rpn::CRpnStack(*pSource);
  else
  m_pStack = new rpn::CRpnStack();
}

void CDerivedResult::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  new CDerivedResultComponent(*this, stage, antype, nRegister);
}

bool CDerivedResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label= ExportLabel(0).toStdString();

  label= StripQuotes(label);

  return validateDerivedName(resultInfo, name, label, "", "_L|_M|_C|_N|_H");
}

bool CDerivedResult::PrepareMapping(const geo::IElementSet * /* pElementSet */, const IValueComponentBase * /* pValueComponent */)
{
  return false;
}


/////

CDerivedResultGroup::CDerivedResultGroup(unsigned int uGroupName, CResultGroup& parent, bool bStoreOnFile)
: CResultGroup(uGroupName, parent),
  m_bStoreOnFile(bStoreOnFile)
{
}

void CDerivedResultGroup::LoadResultFromRegistry()
{
  ISettings* registry = ISettings::instance();
  assert(!m_bStoreOnFile);
  // Load results from registry
  QString sResult;
  int n = 0;
  do
  {

    QString sResultKey;
    sResultKey = QString(RESULT_KEY).arg(n);
    sResult = registry->getProfileString(RESULT_SECTION, sResultKey, "");
    n++;
    if(!sResult.isEmpty())
    {
      CDerivedResult *pResult = new CDerivedResult(*this);
      pResult->LoadFromRegistry(sResultKey);
    }

  } while(!sResult.isEmpty());

  // Enable all the results, because they can be nested
  AttachToModel();
}

bool CDerivedResultGroup::StoreOnFile() const
{
  return m_bStoreOnFile;
}

void CDerivedResultGroup::AttachToModel()
{
  // Enable all derived results
  int i;
  for(i = 0; i < ChildSize(); ++i)
  {
  assert(dynamic_cast<CDerivedResult*>(&Child(i)));
  CDerivedResult& child = static_cast<CDerivedResult&>(Child(i));
  child.AttachToModel((CModelBase&)Model());
  }
}

void CDerivedResultGroup::detach()
{
  while( ChildSize() )
  {
    assert(dynamic_cast<CDerivedResult*>(&Child(0)));
    CDerivedResult& child = static_cast<CDerivedResult&>(Child( 0 ) );
    //child.Destroy(); // This one removes values even from the register
    child.IValueComposite::Destroy(); // this does: 'delete this'; (does not unlink but even so is teh one to use)
  }
}


int CDerivedResultGroup::UniqueIndex() const
{
  int nBase = (m_bStoreOnFile ? DERIVED_RESULT_BASE  : REGISTRY_RESULT_BASE);
  int nEnd  = (m_bStoreOnFile ? COMPOSITE_BASE       : DERIVED_RESULT_BASE );

  for(int nUniqueNumber = nBase; nUniqueNumber < nEnd; nUniqueNumber++)
  {
    // Does the current number exist
    bool bIsUnique = true;
  int i;
  for(i = 0; i < ChildSize(); ++i)
    {
      assert(dynamic_cast<const CDerivedResult*>(&Child(i)));
      const CDerivedResult& child = static_cast<const CDerivedResult&>(Child(i));
      if(child.Index() == nUniqueNumber)
        bIsUnique = false;
    }

    if(bIsUnique)
      return nUniqueNumber;
  }
  return -1;
}

long CDerivedResultGroup::SavedItems() const
{
  long lRet = 0;

  int i;
  for(i = 0; i < ChildSize(); ++i)
  {
  assert(dynamic_cast<const CDerivedResult*>(&Child(i)));
  const CDerivedResult& result = static_cast<const CDerivedResult&>(Child(i));
  lRet += result.SavedItems();
  }

  return lRet;
}

void CDerivedResultGroup::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prog)
{
  int nSize;
  stream >> nSize;
  int i;
  for(i = 0; i < nSize; ++i)
  {
  CDerivedResult* pResult = new CDerivedResult(*this);
  pResult->LoadStream(stream, version, prog);

  if(version == CStreamVersion(3, 0, 97)) // erroneous, skip
      delete pResult;
  }

  AttachToModel();
}

void CDerivedResultGroup::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prog)
{
  stream << ChildSize();
  int i;
  for(i = 0; i < ChildSize(); ++i)
  {
  assert(dynamic_cast<CDerivedResult*>(&Child(i)));
  CDerivedResult& result = static_cast<CDerivedResult&>(Child(i));
  result.SaveStream(stream, prog);
  }
}
