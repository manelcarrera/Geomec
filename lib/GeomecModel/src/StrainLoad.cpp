
#include "StrainLoad.h"
#include "FormationBase.h"
#include "FormationLoad.h"
#include "ValueTypes.h"
#include "DepletionStage.h"
#include "ConvexHullCheck.h"
#include "WellFormation.h"
#include "IWellMesh.h"
#include "IParallelInitializationCallback.h"


///// CStrainLoadGradientComponent

CStrainLoadGradientComponent::CStrainLoadGradientComponent(CStrainType &type, CFormationLoad<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>& parent, bool bEmpty)
  : CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>(parent, bEmpty), m_Type(&type)
  , m_UseVolumetric(false), m_UseInPlane(false), m_ReferenceValueNormal(0), m_ReferenceValueLateral(0) {}

CStrainLoadGradientComponent::CStrainLoadGradientComponent(const CStrainLoadGradientComponent& rhs)
  : CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>(rhs), m_Type(rhs.m_Type)
  , m_UseVolumetric(rhs.m_UseVolumetric), m_UseInPlane(rhs.m_UseInPlane), m_ReferenceValueNormal(rhs.m_ReferenceValueNormal), m_ReferenceValueLateral(rhs.m_ReferenceValueLateral) {}

unsigned int CStrainLoadGradientComponent::TypeId() const { return m_Type->GradientTypeId(); }
unsigned int CStrainLoadGradientComponent::IconId() const { return m_Type->GradientIconId(); }
QString CStrainLoadGradientComponent::TypeName() const { return m_Type->GradientTypeName(); }

QString CStrainLoadGradientComponent::UnitName(const CQuantity::UNIT /*unit*/) const { return QString("-"); }
QString CStrainLoadGradientComponent::ExportLabel() const { return m_Type->ExportLabel(); }

bool CStrainLoadGradientComponent::UseVolumetric() const { return m_UseVolumetric; }
void CStrainLoadGradientComponent::UseVolumetric(bool useVolumetric) { m_UseVolumetric = useVolumetric; }
bool CStrainLoadGradientComponent::UseInPlane() const { return m_UseInPlane; }
void CStrainLoadGradientComponent::UseInPlane(bool useInPlane) { m_UseInPlane = useInPlane; }

const CStrainQuantity& CStrainLoadGradientComponent::ReferenceValueNormal() const { return m_ReferenceValueNormal; }
const CStrainQuantity& CStrainLoadGradientComponent::ReferenceValueLateral() const { return m_ReferenceValueLateral; }

void CStrainLoadGradientComponent::ReferenceValueNormal(double value, const CQuantity::UNIT unit) { m_ReferenceValueNormal.Value(value, unit); }
void CStrainLoadGradientComponent::ReferenceValueLateral(double value, const CQuantity::UNIT unit) { m_ReferenceValueLateral.Value(value, unit); }

void CStrainLoadGradientComponent::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>::LoadStream(stream, version, progress);

  int iVal;
  stream >> iVal;
  m_UseVolumetric = iVal;
  stream >> iVal;
  m_UseInPlane = iVal;

  double dVal;
  stream >> dVal;
  m_ReferenceValueNormal.Value(dVal,CQuantity::SI_UNIT);
  stream >> dVal;
  m_ReferenceValueLateral.Value(dVal,CQuantity::SI_UNIT);
}

void CStrainLoadGradientComponent::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>::SaveStream(stream, progress);
  stream << (int)m_UseVolumetric;
  stream << (int)m_UseInPlane;
  stream << m_ReferenceValueNormal.Value();
  stream << m_ReferenceValueLateral.Value();
}

bool CStrainLoadGradientComponent::operator==(const CStrainLoadGradientComponent &rhs) const
{
  if (!CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>::operator==(rhs))
  return false;

  return m_UseVolumetric == rhs.m_UseVolumetric
  && m_UseInPlane == rhs.m_UseInPlane
  && m_ReferenceValueNormal == rhs.m_ReferenceValueNormal
  && m_ReferenceValueLateral == rhs.m_ReferenceValueLateral;
}



///// CStrainLoadGWCDummyComponent

CStrainLoadGWCDummyComponent::CStrainLoadGWCDummyComponent()
: IValueComponentBase(*this)
{
  assert(false);
}

CStrainLoadGWCDummyComponent::CStrainLoadGWCDummyComponent(CStrainLoadGWCDummyComponent& rhs)
: IValueComponentBase(rhs)
{
  assert(false);
}

const CStrainQuantity &CStrainLoadGWCDummyComponent::ContactPressure() const
{
  assert(false);
  CStrainQuantity* p = 0;
  return *p;
}

const CStrainQuantity &CStrainLoadGWCDummyComponent::TransitionPressure() const
{
  assert(false);
  CStrainQuantity* p = 0;
  return *p;
}

const CLengthQuantity &CStrainLoadGWCDummyComponent::ContactDepth() const
{
  assert(false);
  CLengthQuantity* p = 0;
  return *p;
}

const CLengthQuantity &CStrainLoadGWCDummyComponent::TransitionDepth() const
{
  assert(false);
  CLengthQuantity* p = 0;
  return *p;
}

const CStrainQuantity &CStrainLoadGWCDummyComponent::UpperGradient() const
{
  assert(false);
  CStrainQuantity* p = 0;
  return *p;
}

const CStrainQuantity &CStrainLoadGWCDummyComponent::LowerGradient() const
{
  assert(false);
  CStrainQuantity* p = 0;
  return *p;
}

bool CStrainLoadGWCDummyComponent::TransitionFromPrevious() const
{
  assert(false);
  return false;
}

void CStrainLoadGWCDummyComponent::Set(const double &/*dContactPressure*/,
                    const double &/*dTransitionPressure*/,
                    const double &/*dContactDepth*/,
     								const double &/*dTransitionDepth*/,
     								const double &/*dUpperGradient*/,
     								const double &/*dLowerGradient*/,
     								bool /*bTransitionFromPrevious*/,
     								const CQuantity::UNIT /*unit*/)
{
  assert(false);
}



CStrainLoadRepeaterComponent::CTensorData::CTensorData(const CStrainLoad& parent, const CStrainLoadRepeaterComponent& component)
: m_Parent(parent)
, m_Component(component)
, m_Min(DBL_MAX)
, m_Max(-DBL_MAX)
{
}

void CStrainLoadRepeaterComponent::CTensorData::ResetMinMax() const
{
  m_Min = DBL_MAX;
  m_Max = -DBL_MAX;
}

void CStrainLoadRepeaterComponent::CTensorData::SetMinMax(const IValueDataInterfaceTensor::TValue& t) const
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

IValueDomainTensor::TMinMax CStrainLoadRepeaterComponent::CTensorData::MinMax(IProgressBase &, const IValueDataInterface::UNIT /*unit*/) const
{
  return TMinMax(m_Min, m_Max);
}

IValueDomainTensor::TValue CStrainLoadRepeaterComponent::CTensorData::Average(IProgressBase&, const IValueDataInterface::UNIT) const
{
  return IValueDomainTensor::TValue();
}

IValueDomainTensor::TValue CStrainLoadRepeaterComponent::CTensorData::ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT, geo::IParallelInitializationCallback* /*cb*/) const
{
  return IValueDomainTensor::TValue();
}

IValueDomainTensor::TValueVec CStrainLoadRepeaterComponent::CTensorData::ValueElement(const geo::IElement &elm, const IValueDataInterface::UNIT unit, geo::IParallelInitializationCallback *cb) const  // NOT CASCADING PARALLELINITIALIZATION
{
  bool bIsStrainTensor;
  switch(m_Parent.Type())
  {
  case CStrainLoad::CONSTANT_MODE:
      return m_Parent.ValueTensor(elm, bIsStrainTensor);
  case CStrainLoad::REPEAT_MODE:
      {
        IValueDomainTensor::TValueVec vcRet = m_Parent.ValueTensor(elm, bIsStrainTensor);
        IValueDomainTensor::TValueVec vcPrevious(0);
        for(size_t i = 0; i < vcRet.size(); i++)
        {
  				if(vcRet[i].Empty())
    			{
      			if(vcPrevious.size() == 0)
        		{
          		if(m_Parent.IsFirstValidDepletionStage())
        vcPrevious.resize(elm.NrOfPoints()); // we shouldn't get here
              else
    						vcPrevious = m_Component.PreviousFormationLoad().Component().TensorData()->ValueElement(elm, unit, cb);
      			}
        		vcRet[i] = vcPrevious[i];
          }
        }
        return vcRet;
      }
  case CStrainLoad::GWC_MODE:
  default:
      assert(false);
  }

  return IValueDomainTensor::TValueVec(elm.NrOfPoints());
}


CStrainLoadRepeaterComponent::CScalarData::CScalarData(const CStrainLoad& parent, const CStrainLoadRepeaterComponent& component)
: m_Parent(parent)
, m_Component(component)
{
}

IValueDomainScalar::TMinMax CStrainLoadRepeaterComponent::CScalarData::MinMax(IProgressBase &, const IValueDataInterface::UNIT /*unit*/) const
{
  return TMinMax(0, 0);
}

IValueDomainScalar::TValue CStrainLoadRepeaterComponent::CScalarData::ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT, geo::IParallelInitializationCallback* /*cb*/) const
{
  return IValueDomainScalar::TValue();
}

IValueDomainScalar::TValue CStrainLoadRepeaterComponent::CScalarData::Average(IProgressBase&, const IValueDataInterface::UNIT) const
{
  return IValueDomainScalar::TValue();
}

IValueDomainScalar::TValueVec CStrainLoadRepeaterComponent::CScalarData::ValueElement(const geo::IElement &elm, const IValueDataInterface::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  switch(m_Parent.Type())
  {
  case CStrainLoad::CONSTANT_MODE:
      return m_Parent.ValueElement(elm, cb);
  case CStrainLoad::REPEAT_MODE:
      {
        IValueDomainScalar::TValueVec vcRet = m_Parent.ValueElement(elm, cb);
        IValueDomainScalar::TValueVec vcPrevious(0);
        for(size_t i = 0; i < vcRet.size(); i++)
        {
  				if(!vcRet[i].Valid())
    			{
      			if(vcPrevious.size() == 0)
        		{
          		if(m_Parent.IsFirstValidDepletionStage())
        vcPrevious.resize(elm.NrOfNodes()); // we shouldn't get here
              else
    						vcPrevious = m_Component.PreviousFormationLoad().Component().ScalarData().ValueElement(elm, unit, cb);
      			}
        		vcRet[i] = vcPrevious[i];
          }
        }
        return vcRet;
      }
  case CStrainLoad::GWC_MODE:
  default:
      assert(false);
  }

  return IValueDomainScalar::TValueVec(elm.NrOfPoints());
}


///// CStrainLoadRepeaterComponent

CStrainLoadRepeaterComponent::CStrainLoadRepeaterComponent(CStrainType &type, CStrainLoad& strain)
  : CFormationLoadRepeaterComponent<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>(strain),
      m_TensorData(strain, *this), m_ScalarData(strain, *this), m_Type(&type), m_GlobalValuesComponent(strain) {}

CStrainLoadRepeaterComponent::CStrainLoadRepeaterComponent(const CStrainLoadRepeaterComponent& rhs)
  : CFormationLoadRepeaterComponent<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>(rhs),
      m_TensorData(static_cast<const CStrainLoad&>(Parent()), *this), m_ScalarData(static_cast<const CStrainLoad&>(Parent()), *this), m_Type(rhs.m_Type), m_GlobalValuesComponent(rhs.m_GlobalValuesComponent) {}

IValueComponentBase::TValueComponentType CStrainLoadRepeaterComponent::Type() const
{ 
  const CStrainLoad& strainLoad = static_cast<const CStrainLoad&>(Parent());
  return strainLoad.IsPlanar() || strainLoad.IsTensor() ? IValueComponentBase::TENSOR : IValueComponentBase::SCALAR;
}
const IValueDataInterfaceTensor* CStrainLoadRepeaterComponent::TensorData() const
{
  return &m_TensorData;
}
IValueDataInterfaceTensor* CStrainLoadRepeaterComponent::TensorData()
{ 
  return &m_TensorData;
}
const IValueDataInterfaceScalar& CStrainLoadRepeaterComponent::ScalarData() const
{
  return m_ScalarData;
}
IValueDataInterfaceScalar& CStrainLoadRepeaterComponent::ScalarData()
{ 
  return m_ScalarData;
}

// IconId and TypeName
unsigned int CStrainLoadRepeaterComponent::TypeId() const
{
  const CStrainLoad *pVolStrain = dynamic_cast<const CStrainLoad*> (&FormationLoadBase());
  assert(pVolStrain);
  switch(pVolStrain->Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  return pVolStrain->Constant().TypeId();
  case CStrainLoad::REPEAT_MODE:
    return m_Type->RepeaterTypeId();
  default:
  assert(false); // Bogus
  };

  return 0;
}

unsigned int CStrainLoadRepeaterComponent::IconId() const
{
  const CStrainLoad *pStrain = dynamic_cast<const CStrainLoad*> (&FormationLoadBase());
  assert(pStrain);

  const CStrainLoad *pParentStrain = pStrain->ParentStrainLoad();

  bool bSimilarToParent = pParentStrain && pStrain->IsSimilar(*pParentStrain, false);

  switch(pStrain->Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  if (bSimilarToParent && pStrain->Constant() == pParentStrain->Constant())
      return IDI_COMPONENT_VOLSTRAIN_PARENT;
  else
      return pStrain->Constant().IconId();
  case CStrainLoad::REPEAT_MODE:
  if (pStrain->IsFirstValidDepletionStage())
  {
      if (bSimilarToParent && pStrain->Constant() == pParentStrain->Constant())
    return IDI_COMPONENT_VOLSTRAIN_PARENT;
      else
    return m_Type->InitialIconId();
  }
    else
  {
      const CStrainLoad *pPrevStrain = &pStrain->Formation()->Strain(pStrain->DepletionStage().Previous());
      assert(pPrevStrain);

      if (pStrain->IsSimilar(*pPrevStrain))
    return m_Type->RepeaterIconId();;

      if (bSimilarToParent)
    return IDI_COMPONENT_VOLSTRAIN_PARENT;

      return m_Type->ChangeIconId();;
    }
  default:
  assert(false);
  };
  
  return 0;
}


QString CStrainLoadRepeaterComponent::TypeName() const
{
  const CStrainLoad *pVolStrain = dynamic_cast<const CStrainLoad*> (&FormationLoadBase());
  assert(pVolStrain);
  switch(pVolStrain->Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  return pVolStrain->Constant().TypeName();
  case CStrainLoad::REPEAT_MODE:
  {
    return m_Type->RepeaterTypeName();
  }
  default:
  assert(false); // Bogus
  };

  return QString();
}


QString CStrainLoadRepeaterComponent::UnitName(const CQuantity::UNIT /*unit*/) const { return QString("-"); }
QString CStrainLoadRepeaterComponent::ExportLabel() const { return m_Type->ExportLabel(); }

const CStrainLoadRepeaterComponent::TFormationLoad& CStrainLoadRepeaterComponent::PreviousFormationLoad() const
{
 	const CStrainLoad *pVolStrain = dynamic_cast<const CStrainLoad*> (&FormationLoadBase());
  assert(pVolStrain);
  assert(!pVolStrain->DepletionStage().Initial());
  assert(!pVolStrain->DepletionStage().Previous().Initial());
  return pVolStrain->Formation()->Strain(pVolStrain->DepletionStage().Previous());
}

const IValueComponentBase& CStrainLoadRepeaterComponent::GlobalValues() const
{
  return m_GlobalValuesComponent;
}



CStrainLoadRepeaterComponent::CGlobalValuesComponent::CGlobalValuesComponent(IValueComposite& parent)
: IValueComponentBase(parent), m_Data(*this)
{
}

IValueComponentBase::TValueComponentType CStrainLoadRepeaterComponent::CGlobalValuesComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CStrainLoadRepeaterComponent::CGlobalValuesComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CStrainLoadRepeaterComponent::CGlobalValuesComponent::ScalarData()
{
  return m_Data;
}

QString CStrainLoadRepeaterComponent::CGlobalValuesComponent::ExportLabel() const
{
  assert(false);
  return QString("");
}

QString CStrainLoadRepeaterComponent::CGlobalValuesComponent::UnitName(const UNIT /*unit*/) const
{
  return QString("-");
}

bool CStrainLoadRepeaterComponent::CGlobalValuesComponent::Defined() const
{
  return true;
}

IValueDomainScalar::TMinMax CStrainLoadRepeaterComponent::CGlobalValuesComponent::MinMax(IProgressBase& /*progressBase*/, const UNIT /*unit*/) const
{
  return IValueDomainScalar::TMinMax(0, 0);
}

geo::CValue CStrainLoadRepeaterComponent::CGlobalValuesComponent::ValuePoint(const geo::IPoint& /*pt*/, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  return geo::CValue(0);
}

IValueDomainScalar::TValueVec CStrainLoadRepeaterComponent::CGlobalValuesComponent::ValueElement(const geo::IElement& elm, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  IValueDomainScalar::TValueVec vcValues(elm.NrOfNodes(), 0);
  return vcValues;
}

IValueDomainScalar::TValue CStrainLoadRepeaterComponent::CGlobalValuesComponent::Average(IProgressBase&, const UNIT) const
{
  return IValueDomainScalar::TValue();
}

unsigned int CStrainLoadRepeaterComponent::CGlobalValuesComponent::IconId() const
{
  return 0;
}

unsigned int CStrainLoadRepeaterComponent::CGlobalValuesComponent::TypeId() const
{
  return 0;
}

bool CStrainLoadRepeaterComponent::CGlobalValuesComponent::Empty() const
{
  return true;
}

long CStrainLoadRepeaterComponent::CGlobalValuesComponent::SavedItems() const
{
  return 0;
}



///// CStrainLoad

CStrainLoad::CStrainLoad(CStrainType &t, CFormationBase& f, CDepletionStage& d, bool bEmpty)
: TBase(f, d), m_Type(&t)
{
  LinkTo(d);
  reParent(&f);

  if(!bEmpty)
  {
  new CStrainLoadRepeaterComponent(*m_Type, *this);
  new CStrainLoadGradientComponent(*m_Type, *this);
  }
}

CStrainLoad::CStrainLoad(CStrainType &t, CFormationBase &f, CDepletionStage &d, const double& dVolStrain, const CQuantity::UNIT unit)
: TBase(f, d), m_Type(&t)
{
  LinkTo(d);
  reParent(&f);

  new CStrainLoadRepeaterComponent(*m_Type, *this);
  new CStrainLoadGradientComponent(*m_Type, *this);

  Constant().Set(dVolStrain,
           0,
           0,
           unit);

  assert(Type() == CONSTANT_MODE);
}

CStrainLoad::CStrainLoad(const CStrainLoad& rhs)
  : TBase(rhs), m_Type(rhs.m_Type), m_vcDistributedNodes(rhs.m_vcDistributedNodes)
{
}

CStrainLoad& CStrainLoad::operator=(const CStrainLoad& rhs)
{
  TBase::operator=(rhs);

  CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearAll();

  m_Type = rhs.m_Type;
  m_vcDistributedNodes = rhs.m_vcDistributedNodes;

  return *this;
}

bool CStrainLoad::IsSimilar(const CStrainLoad &rhs, bool bCheckDistribution) const
{
  if (Type() != rhs.Type())
  return false;

  if (bCheckDistribution)
  {
  if (DistributedSize() != 0)
  {
      if (DistributedSize() != rhs.DistributedSize())
    return false;
      if (DistributedOnly() != rhs.DistributedOnly())
    return false;
  }
  }
  else if (DistributedSize() != rhs.DistributedSize())
  return false;

  for (size_t i = 0; i < DistributedSize(); ++i)
  {
  size_t j;

  for (j = 0; j < DistributedSize(); ++j)
      if (&DistributedValue(i) == &rhs.DistributedValue(j))
    break;

  if (j == DistributedSize())
      return false;
  }

  return true;
}

void CStrainLoad::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if (version > CStreamVersion(4, 1, 19))
  {
  TBase::LoadStream(stream, version, progress);

  	TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*(static_cast<CModelBase&>(Model())).RootModel().GraphEntry(MD_BASE_VALUE_COMPOSITE);

  // Load the size of the disbuted values
    int distriSize;
    stream >> distriSize;

    // Load and link indices
    for (int i = 0; i < distriSize; i++)
    {

  		int nIndex;
    	stream >> nIndex;
      composite_entry.LinkNodeToIndex(*this, nIndex);
  }

  }
  else
  {
  new CStrainLoadRepeaterComponent(*m_Type, *this);
  new CStrainLoadGradientComponent(*m_Type, *this);
  }
}

void CStrainLoad::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  TBase::SaveStream(stream, progress);

  int distributedSize = DistributedSize();
  stream << distributedSize;
  for(size_t i = 0; i < DistributedSize(); i++)
  {
  const CGraphNode &node = DistributedValue(i);

  const TStrainTensor *pTensor = dynamic_cast<const TStrainTensor *>(&node);
  const TNormalStrain *pNormal = dynamic_cast<const TNormalStrain *>(&node);
  const TLateralStrain *pLateral = dynamic_cast<const TLateralStrain *>(&node);
  const TVolumetricStrain *pVolume = dynamic_cast<const TVolumetricStrain *>(&node);

  int index = -1;

  if (pTensor)
      index = pTensor->Index();
  if (pNormal)
      index = pNormal->Index();
  if (pLateral)
      index = pLateral->Index();
  if (pVolume)
      index = pVolume->Index();

  if (index > -1)
      stream << index;
    progress.Step();
  }

}



bool CStrainLoad::IsFirstValidDepletionStage() const
{
  assert(!DepletionStage().Initial());
  return (DepletionStage().Previous().Initial());
}

unsigned int CStrainLoad::TypeId() const
{
  return m_Type->TypeId();
}

QString CStrainLoad::TypeName() const
{
  bool isFullTensor;
  if (IsPlanar(isFullTensor))
  {
  if (isFullTensor)
      return "Full Strain";
  else
      return "Normal/Lateral Strain";
  }
  else if (IsTensor())
  {
  return "Normal/Lateral Strain";
  }
  else if (IsVolumetric())
  {
  return "Volumetric Strain";
  }

  return "Strain";
}

CStrainLoad& CStrainLoad::FormationStageLoad(CFormationBase& formation, CDepletionStage& stage)
{
  return formation.Strain(stage);
}

void CStrainLoad::OnNewNeighbour(const CGraphNode& node)
{
  const TStrain* pStrain = dynamic_cast<const TStrain*>(&node);
  const TStrainTensor *pTensor = dynamic_cast<const TStrainTensor *>(&node);
  const TNormalStrain *pNormal = dynamic_cast<const TNormalStrain *>(&node);
  const TLateralStrain *pLateral = dynamic_cast<const TLateralStrain *>(&node);
  const TVolumetricStrain *pVolume = dynamic_cast<const TVolumetricStrain *>(&node);

  if(pStrain || pTensor || pNormal || pLateral || pVolume)
  {
  CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();

 		m_vcDistributedNodes.push_back(&node);
    Modified();

    if(IValueComposite::ModeSize() > 0)
      Component().Modified();
  }

  TBase::OnNewNeighbour(node);
}

void CStrainLoad::OnNeighbourDeleted(const CGraphNode& node)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  if(!model.getDeleting())
  {
  bool haveDeleted = false;

  	for (std::vector<const CGraphNode*>::iterator it = m_vcDistributedNodes.begin(); it != m_vcDistributedNodes.end(); ++it)
    {
      assert(!(*it)->IsCopy());
      if(*it == &node)
      {
        m_vcDistributedNodes.erase(it);
    haveDeleted = true;
        Modified();
        Component().Modified();
        break;
      }
    }

  if (haveDeleted)
  {
      model.ResultRegister().ClearLinear(false);
      model.ResultRegister().ClearNonLinear(false);
      model.ResultRegister().ClearMixture();
  }
  }

  TBase::OnNeighbourDeleted(node);
}

void CStrainLoad::LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  new CStrainLoadRepeaterComponent(*m_Type, *this);

  switch(Type())
  {
  case REPEAT_MODE:
    new CStrainLoadGradientComponent(*m_Type, *this);
    break;
  case CONSTANT_MODE:
    {
      CStrainLoadGradientComponent* pConstant = new CStrainLoadGradientComponent(*m_Type, *this, true);
      pConstant->LoadStream(stream, version, progress);
    }
    break;
  default:
    assert(false);
    break;
  };
}



void CStrainLoad::SaveComponents(TSTREAM& stream, TPROGRESS& progress)
{
  if(Type() == CONSTANT_MODE)
    Constant().SaveStream(stream, progress);
}



/////

CStrainLoad::CStrainLoadComponent::CStrainLoadComponent(CStrainLoad& parent)
: TBase(parent, 0)
{
}

CStrainLoad::CStrainLoadComponent::CStrainLoadComponent(const CStrainLoadComponent& rhs)
: TBase(rhs)
{
}

unsigned int CStrainLoad::CStrainLoadComponent::TypeId() const
{
  return Parent().TypeId();
}

unsigned int CStrainLoad::CStrainLoadComponent::IconId() const
{
  return Parent().IconId();
}

QString CStrainLoad::CStrainLoadComponent::TypeName() const
{
  return Parent().TypeName();
}

QString CStrainLoad::CStrainLoadComponent::UnitName(const CQuantity::UNIT /*unit*/) const
{
  return getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
}

QString CStrainLoad::CStrainLoadComponent::ExportLabel() const
{
  return Parent().ExportLabel();
}

bool CStrainLoad::CStrainLoadComponent::Defined() const
{
  const CStrainLoad& parent = static_cast<const CStrainLoad&>(Parent());
  return parent.DistributedSize() > 0;
}

long CStrainLoad::CStrainLoadComponent::SavedItems() const
{
  return TBase::SavedItems();
}


bool CStrainLoad::CanConnectItem(const CGraphNode& item) const
{
  const TStrainTensor *pTensor = dynamic_cast<const TStrainTensor *>(&item);
  if (pTensor)
  {
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TStrainTensor *>(&referenceAt(i)))
    return false;
  }
  return true;
  }

  const TNormalStrain *pNormal = dynamic_cast<const TNormalStrain *>(&item);
  if (pNormal)
  {
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TNormalStrain *>(&referenceAt(i)))
    return false;
  }
  return true;
  }

  const TLateralStrain *pLateral = dynamic_cast<const TLateralStrain *>(&item);
  if (pLateral)
  {
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TLateralStrain *>(&referenceAt(i)))
    return false;
  }
  return true;
  }

  const TVolumetricStrain *pVolume = dynamic_cast<const TVolumetricStrain *>(&item);
  if (pVolume)
  {
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TVolumetricStrain *>(&referenceAt(i)))
    return false;
  }
  return true;
  }

  return TBase::CanConnectItem(item);
}

bool CStrainLoad::ConnectItem(const CGraphNode& item)
{
  const TStrainTensor *pTensor = dynamic_cast<const TStrainTensor *>(&item);
  if (pTensor)
  {
  if (!IsLinkedTo(*pTensor))
  {
      LinkTo((TStrainTensor&)*pTensor);
      Modified();
      return true;
  }
  }

  const TNormalStrain *pNormal = dynamic_cast<const TNormalStrain *>(&item);
  if (pNormal)
  {
  if (!IsLinkedTo(*pNormal))
  {
      LinkTo((TNormalStrain&)*pNormal);
      Modified();
      return true;
  }
  }

  const TLateralStrain *pLateral = dynamic_cast<const TLateralStrain *>(&item);
  if (pLateral)
  {
  if (!IsLinkedTo(*pLateral))
  {
      LinkTo((TLateralStrain&)*pLateral);
      Modified();
      return true;
  }
  }

  const TVolumetricStrain *pVolume = dynamic_cast<const TVolumetricStrain *>(&item);
  if (pVolume)
  {
  if (!IsLinkedTo(*pVolume))
  {
      LinkTo((TVolumetricStrain&)*pVolume);
      Modified();
      return true;
  }
  }

  return TBase::ConnectItem(item);
}


template<typename TYPE>
const TYPE* CStrainLoad::StrainComponent() const
{
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TYPE *>(&referenceAt(i)))
    return static_cast<const TYPE *>(&referenceAt(i));
  }
  return 0;
}


class CStrainDistributedSetWrapper : public IDistributedSetWrapper
{
  const  CValueType& parent;
  bool   m_distributedOnly;
  size_t m_Index;

public:

  CStrainDistributedSetWrapper(const CValueType& base, bool distributedOnly, size_t index = 0) : parent(base), m_distributedOnly(distributedOnly), m_Index(index) {}

  virtual size_t DistributedSize() { return 1; }

  virtual const IPointSet& DistributedPointset(size_t /*index*/) { return parent.PointSet(); }

  virtual void DistributedValue(size_t /*index*/, const geo::IElement& elm, const CDoubleQuantity::UNIT unit, IValueDomainScalar::TValueVec& vcValues)
  {
  vcValues = parent.Component(m_Index, CStrainLoad::CONSTANT_MODE).ScalarData().ValueElement(elm, unit);
  }

  virtual void DistributedValue(size_t /*index*/, const geo::IElement& /*elm*/, const CDoubleQuantity::UNIT /*unit*/, IValueDomainVector::TValueVec& /*vcValues*/) {}
  virtual void DistributedValue(size_t /*index*/, const geo::IElement& /*elm*/, const CDoubleQuantity::UNIT /*unit*/, IValueDomainTensor::TValueVec& /*vcValues*/) {}

  virtual bool DistributedOnly() { return m_distributedOnly; }
  virtual bool DoAveraging() { return false; }

};


IValueDomainScalar::TValueVec CStrainLoad::ValueElement(const geo::IElement &elm, geo::IParallelInitializationCallback* /*cb*/) const
{
  IValueDomainScalar::TValueVec vcRet;

  TVolumetricStrain *pVolume = 0;
  for (size_t i = 0; i < referenceSize(); ++i)
  {
  if (dynamic_cast<const TVolumetricStrain *>(&referenceAt(i)))
  {
      pVolume = const_cast<TVolumetricStrain *>(static_cast<const TVolumetricStrain *>(&referenceAt(i)));
      break;
  }
  }

  if (pVolume && pVolume->Component().Defined())
  {
  CStrainDistributedSetWrapper wrapper(*pVolume, DistributedOnly());
  CConvexHullCheck<IValueDomainScalar::TValueVec>::ValueElement(wrapper, elm, CQuantity::SI_UNIT, vcRet);
  }
  else if (Type() == REPEAT_MODE || Constant().UseVolumetric())
  {
  vcRet.resize(elm.NrOfNodes());
  }

  if (vcRet.size() > 0 && Type() == CONSTANT_MODE && Constant().UseVolumetric())
  {
   	const CModelBase& model = static_cast<const CModelBase&>(Model());
  CQuantity::UNIT unit = CQuantity::SI_UNIT;

  for (size_t i = 0; i < vcRet.size(); ++i)
  {
      if (!vcRet[i].Valid())
    vcRet[i].Value((Constant().ReferenceDepth().Convert(model.Depth(elm.Point(i)), unit, CQuantity::SI_UNIT)  - Constant().ReferenceDepth().Value(unit)) * Constant().Gradient().Value(unit) + Constant().ReferenceValue().Value(unit));
  }
  }

  return vcRet;
}


std::vector<CTensor> CStrainLoad::ValueTensor(const geo::IElement &elm, bool &bIsStrainTensor) const
{
  std::vector<CTensor> retVal(elm.NrOfNodes());

  std::vector<IValueDomainScalar::TValueVec> tmpVal(6);

  std::vector<TmpTensor> tmpTensor(elm.NrOfNodes());

  TStrainTensor  *pTensor  = 0;
  TNormalStrain  *pNormal  = 0;
  TLateralStrain *pLateral = 0;

  

  // set XY, YZ, XZ to valid (they have value 0 already) -- we do it here, so that later the normal/lateral strain constants are accepted
  for (size_t i = 3; i < 6; ++i)
  {
  for (size_t j = 0; j < elm.NrOfNodes(); ++j)
      tmpTensor[j].values[i].empty = false;
  }

  for (size_t i = 0; i < referenceSize(); ++i)
  {
  if (dynamic_cast<const TStrainTensor *>(&referenceAt(i)))
      pTensor = const_cast<TStrainTensor *>(static_cast<const TStrainTensor *>(&referenceAt(i)));
  else if (dynamic_cast<const TNormalStrain *>(&referenceAt(i)))
      pNormal = const_cast<TNormalStrain *>(static_cast<const TNormalStrain *>(&referenceAt(i)));
  else if (dynamic_cast<const TLateralStrain *>(&referenceAt(i)))
      pLateral = const_cast<TLateralStrain *>(static_cast<const TLateralStrain *>(&referenceAt(i)));
  }

  m_bEncounteredFullTensor = bIsStrainTensor = false;

  if (pTensor)
  {
  for (size_t i = 0; i < 6; ++i)
  {
      CStrainDistributedSetWrapper wrapper(*pTensor, DistributedOnly(), i);
      CConvexHullCheck<IValueDomainScalar::TValueVec>::ValueElement(wrapper, elm, CQuantity::SI_UNIT, tmpVal[i]);
      assert(tmpVal[i].size() == elm.NrOfNodes());

      for (size_t j = 0; j < tmpVal[i].size(); ++j)
      {
    if (tmpVal[i][j].Valid())
    {
          tmpTensor[j].values[i].Value(i > 2 ? tmpVal[i][j].Value() / 2 : tmpVal[i][j].Value());
          m_bEncounteredFullTensor = bIsStrainTensor = true;
    }
      }
  }
  }

  // if the strain tensor is undefined for this element, we look into normal/lateral strains and their respective defaults
  if (!bIsStrainTensor && pNormal)
  {
  CStrainDistributedSetWrapper wrapper(*pNormal, DistributedOnly());
  CConvexHullCheck<IValueDomainScalar::TValueVec>::ValueElement(wrapper, elm, CQuantity::SI_UNIT, tmpVal[2]);

  for (size_t j = 0; j < tmpVal[2].size(); ++j)
  {
      if (tmpVal[2][j].Valid())
      {
    tmpTensor[j].values[2].Value(tmpVal[2][j].Value());
      }
  }
  }

  if (!bIsStrainTensor && pLateral)
  {
  CStrainDistributedSetWrapper wrapper(*pLateral, DistributedOnly());
  CConvexHullCheck<IValueDomainScalar::TValueVec>::ValueElement(wrapper, elm, CQuantity::SI_UNIT, tmpVal[0]);

  for (size_t j = 0; j < tmpVal[0].size(); ++j)
  {
      if (tmpVal[0][j].Valid())
      {
    tmpTensor[j].values[0].Value(tmpVal[0][j].Value());
    tmpTensor[j].values[1].Value(tmpVal[0][j].Value()); // duplicate XX to YY
      }
  }
  }

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CQuantity::UNIT unit = CQuantity::SI_UNIT;

  if (!bIsStrainTensor && !DistributedOnly() && Type() == CStrainLoad::CONSTANT_MODE && Constant().UseInPlane())
  { // fill in empty slots

  for (size_t i = 0; i < 2; ++i)
  {
      for (size_t j = 0; j < elm.NrOfNodes(); ++j)
      {
    if (tmpTensor[j].values[i].empty)
    {
          double value = (Constant().ReferenceDepth().Convert(model.Depth(elm.Point(j)), unit, CQuantity::SI_UNIT)  - Constant().ReferenceDepth().Value(unit)) * Constant().Gradient().Value(unit) + Constant().ReferenceValueLateral().Value(unit);
          tmpTensor[j].values[i].Value(value);
    }
      }
  }
  for (size_t j = 0; j < elm.NrOfNodes(); ++j)
  {
      if (tmpTensor[j].values[2].empty)
      {
    double value = (Constant().ReferenceDepth().Convert(model.Depth(elm.Point(j)), unit, CQuantity::SI_UNIT)  - Constant().ReferenceDepth().Value(unit)) * Constant().Gradient().Value(unit) + Constant().ReferenceValueNormal().Value(unit);
    tmpTensor[j].values[2].Value(value);
      }
  }

  }


  for (size_t j = 0; j < elm.NrOfNodes(); ++j)
  {
  bool notEmpty = false;
  for (size_t i = 0; i < 3; ++i)
  {
      if (!tmpTensor[j].values[i].empty)
      {
    notEmpty = true;
    break;
      }
  }
  if (notEmpty)
  {
      for (size_t i = 0; i < 3; ++i)
  {
    if (tmpTensor[j].values[i].empty)
          tmpTensor[j].values[i].empty = false;
      }
      retVal[j].XX(tmpTensor[j].values[0].value);
      retVal[j].YY(tmpTensor[j].values[1].value);
      retVal[j].ZZ(tmpTensor[j].values[2].value);
      retVal[j].XY(tmpTensor[j].values[3].value);
      retVal[j].YZ(tmpTensor[j].values[4].value);
      retVal[j].XZ(tmpTensor[j].values[5].value);
  }
  }

  return retVal;

}


bool CStrainLoad::EncounteredFullTensor() const
{
  return m_bEncounteredFullTensor;
}

bool CStrainLoad::IsTensor() const
{
  switch (Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  if (Constant().UseInPlane())
      return true;
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if ((dynamic_cast <const TLateralStrain*> (&referenceAt(i))) ||
    (dynamic_cast <const TNormalStrain*> (&referenceAt(i))))
    return true;
  }
  break;
  case CStrainLoad::REPEAT_MODE:
  {
  for (size_t distributedNode = 0; distributedNode < DistributedSize();
      ++distributedNode)
  {
      if ((dynamic_cast <const TLateralStrain*> (
          &DistributedValue(distributedNode)) != 0) ||
    (dynamic_cast <const TNormalStrain*> (
          &DistributedValue(distributedNode)) != 0))
      {
    return true;
      }
  }

  const CDepletionStage *pPrev = &DepletionStage().Previous();
  assert(pPrev);

  // D1 can be REPEAT_MODE (unused) and we need to catch that here, so that we won't try to access strains for D0
  if (pPrev->Initial())
      return false;

  const CFormationBase *pFormation = dynamic_cast<CFormationBase*>(parent());
  assert(pFormation);
  const CStrainLoad *pPrevStrain = &pFormation->Strain(*pPrev);
  assert(pPrevStrain);

  return pPrevStrain->IsTensor();
  }
  default:
  assert(false);
  };

  return false;
}

bool CStrainLoad::IsVolumetric() const
{
  switch(Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  if (Constant().UseVolumetric())
      return true;
  for (size_t i = 0; i < referenceSize(); ++i)
  {
      if (dynamic_cast<const TVolumetricStrain *>(&referenceAt(i)))
    return true;
  }
  break;
  case CStrainLoad::REPEAT_MODE:
  {
      for (size_t distributedNode = 0; distributedNode < DistributedSize();
    ++distributedNode)
      {
    if (dynamic_cast <const TVolumetricStrain*> (
          &DistributedValue(distributedNode)) != 0)
    {
          return true;
    }
      }

      const CDepletionStage *pPrev = &DepletionStage().Previous();
      assert(pPrev);

      // D1 can be REPEAT_MODE (unused) and we need to catch that here, so that we won't try to access strains for D0
      if (pPrev->Initial())
    return false;

      const CFormationBase *pFormation = dynamic_cast<CFormationBase*>(parent());
      assert(pFormation);
      const CStrainLoad *pPrevStrain = &pFormation->Strain(*pPrev);
      assert(pPrevStrain);

      return pPrevStrain->IsVolumetric();
  }
  default:
  assert(false);
  };

  return false;
}

bool CStrainLoad::IsPlanar() const
{
  bool dummy;
  return IsPlanar(dummy);
}

bool CStrainLoad::IsPlanar(bool& fullTensor) const
{
  fullTensor = false;
  switch(Type())
  {
  case CStrainLoad::CONSTANT_MODE:
  {
      bool haveStrain = false;

      for (size_t i = 0; i < referenceSize(); ++i)
      {
    if (dynamic_cast<const TStrainTensor *>(&referenceAt(i)))
    {
          fullTensor = true;
          return true;
    }
    else if (dynamic_cast<const TNormalStrain *>(&referenceAt(i)) || dynamic_cast<const TLateralStrain *>(&referenceAt(i)))
    {
          haveStrain = true;
    }
      }

      return haveStrain || Constant().UseInPlane();
  }
  case CStrainLoad::REPEAT_MODE:
  {
      const CDepletionStage *pPrev = &DepletionStage().Previous();
      assert(pPrev);

      // D1 can be REPEAT_MODE (unused) and we need to catch that here, so that we won't try to access strains for D0
      if (pPrev->Initial())
    return false;

      const CFormationBase *pFormation = dynamic_cast<CFormationBase*>(parent());
      assert(pFormation);
      const CStrainLoad *pPrevStrain = &pFormation->Strain(*pPrev);
      assert(pPrevStrain);

      return pPrevStrain->IsPlanar();
  }
  default:
  assert(false);
  };

  return false;
}


bool CStrainLoad::PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *)
{
  for (size_t i = 0; i < DistributedSize(); ++i)
  {
  const IValueComposite *pComposite = dynamic_cast<const IValueComposite *>(&DistributedValue(i));
  if (pComposite)
  {
      for (size_t j = 0; j < pComposite->ComponentSize(); ++j)
    if (!const_cast<IValueComponentBase&>(pComposite->Component(j)).PrepareMapping(pElementSet))
          return false;
  }
  }

  return true;
}


const CStrainLoad *CStrainLoad::ParentStrainLoad() const
{
  const CFormationBase *pFormation = Formation();
  const CStrainLoad *pParentStrainLoad = 0;

  if (pFormation && pFormation->ParentFormation())
  pParentStrainLoad = &pFormation->ParentFormation()->Strain(static_cast<const CModelBase&>(pFormation->Model()).ParentEquivalentDepletionStage(DepletionStage()));

  return pParentStrainLoad;
}