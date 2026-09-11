
#include "VolumetricStrainLoad.h"
#include "FormationBase.h"
#include "ValueTypes.h"
#include "DepletionStage.h"

///// CVolumetricStrainLoadGradientComponent

CVolumetricStrainLoadGradientComponent::CVolumetricStrainLoadGradientComponent(CFormationLoadBase<TVolumetricStrain> &volstrain, bool bEmpty)
: CFormationLoadGradientComponent<TVolumetricStrain, CStrainQuantity, CStrainQuantity>(volstrain, bEmpty)
{
}

CVolumetricStrainLoadGradientComponent::CVolumetricStrainLoadGradientComponent(const CVolumetricStrainLoadGradientComponent& rhs)
: CFormationLoadGradientComponent<TVolumetricStrain, CStrainQuantity, CStrainQuantity>(rhs)
{
}

unsigned int CVolumetricStrainLoadGradientComponent::TypeId() const
{
	return IDT_COMPONENT_VOLSTRAIN_GRADIENT;
}

unsigned int CVolumetricStrainLoadGradientComponent::IconId() const
{
	return IDI_COMPONENT_VOLSTRAIN_GRADIENT;
}

QString CVolumetricStrainLoadGradientComponent::TypeName() const
{
	return getStringTableEntry(IDS_COMPONENT_VOLSTRAIN_GRADIENT);
}

QString CVolumetricStrainLoadGradientComponent::UnitName(const CQuantity::UNIT /*unit*/) const
{
	return QString("-");
}

QString CVolumetricStrainLoadGradientComponent::ExportLabel() const
{
	 return getStringTableEntry(IDS_ET_VOLUMETRICSTRAIN);
}


///// CVolumetricStrainLoadGWCDummyComponent

CVolumetricStrainLoadGWCDummyComponent::CVolumetricStrainLoadGWCDummyComponent()
: IValueComponentBase(*this)
{
	assert(false);
}

CVolumetricStrainLoadGWCDummyComponent::CVolumetricStrainLoadGWCDummyComponent(CVolumetricStrainLoadGWCDummyComponent& rhs)
: IValueComponentBase(rhs)
{
	assert(false);
}

const CStrainQuantity &CVolumetricStrainLoadGWCDummyComponent::ContactPressure() const
{
	assert(false);
	CStrainQuantity* p = 0;
	return *p;
}

const CStrainQuantity &CVolumetricStrainLoadGWCDummyComponent::TransitionPressure() const
{
	assert(false);
	CStrainQuantity* p = 0;
	return *p;
}

const CLengthQuantity &CVolumetricStrainLoadGWCDummyComponent::ContactDepth() const
{
	assert(false);
	CLengthQuantity* p = 0;
	return *p;
}

const CLengthQuantity &CVolumetricStrainLoadGWCDummyComponent::TransitionDepth() const
{
	assert(false);
	CLengthQuantity* p = 0;
	return *p;
}

const CStrainQuantity &CVolumetricStrainLoadGWCDummyComponent::UpperGradient() const
{
	assert(false);
	CStrainQuantity* p = 0;
	return *p;
}

const CStrainQuantity &CVolumetricStrainLoadGWCDummyComponent::LowerGradient() const
{
	assert(false);
	CStrainQuantity* p = 0;
	return *p;
}

bool CVolumetricStrainLoadGWCDummyComponent::TransitionFromPrevious() const
{
	assert(false);
	return false;
}

void CVolumetricStrainLoadGWCDummyComponent::Set(const double &/*dContactPressure*/,
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


///// CVolumetricStrainLoadRepeaterComponent

CVolumetricStrainLoadRepeaterComponent::CVolumetricStrainLoadRepeaterComponent(CVolumetricStrainLoad& volstrain)
: CFormationLoadRepeaterComponent<TVolumetricStrain, CVolumetricStrainLoadGradientComponent, CVolumetricStrainLoadRepeaterComponent, CVolumetricStrainLoadGWCDummyComponent>(volstrain),
  m_GlobalValuesComponent(volstrain)
{
}

CVolumetricStrainLoadRepeaterComponent::CVolumetricStrainLoadRepeaterComponent(const CVolumetricStrainLoadRepeaterComponent& rhs)
: CFormationLoadRepeaterComponent<TVolumetricStrain, CVolumetricStrainLoadGradientComponent, CVolumetricStrainLoadRepeaterComponent, CVolumetricStrainLoadGWCDummyComponent>(rhs),
  m_GlobalValuesComponent(rhs.m_GlobalValuesComponent)
{
}

unsigned int CVolumetricStrainLoadRepeaterComponent::TypeId() const
{
	const CVolumetricStrainLoad *pVolStrain = dynamic_cast<const CVolumetricStrainLoad*> (&FormationLoadBase());
	assert(pVolStrain);
	switch(pVolStrain->Type())
	{
	case CVolumetricStrainLoad::CONSTANT_MODE:
		return pVolStrain->Constant().TypeId();
	case CVolumetricStrainLoad::REPEAT_MODE:
		return IDT_COMPONENT_VOLSTRAIN_REPEATER;
	default:
		assert(false); // Bogus
	};

	return 0;
}

unsigned int CVolumetricStrainLoadRepeaterComponent::IconId() const
{
	return 0;
}

QString CVolumetricStrainLoadRepeaterComponent::TypeName() const
{
	return QString();
}

QString CVolumetricStrainLoadRepeaterComponent::UnitName(const CQuantity::UNIT /*unit*/) const
{
  return QString("-");
}

QString CVolumetricStrainLoadRepeaterComponent::ExportLabel() const
{
	 return getStringTableEntry(IDS_ET_VOLUMETRICSTRAIN);
}

const CVolumetricStrainLoadRepeaterComponent::TFormationLoad& CVolumetricStrainLoadRepeaterComponent::PreviousFormationLoad() const
{
  assert(false);
  TFormationLoad* pBogus = 0;
  return *pBogus;
}

const IValueComponentBase& CVolumetricStrainLoadRepeaterComponent::GlobalValues() const
{
  // return the global values dummy component
  return m_GlobalValuesComponent;
}


///// CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent
CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::CGlobalValuesComponent(IValueComposite& parent)
: IValueComponentBase(parent), m_Data(*this)
{
}

IValueComponentBase::TValueComponentType CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::ScalarData()
{
  return m_Data;
}

QString CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::ExportLabel() const
{
  assert(false);
  return QString("");
}

QString CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::UnitName(const UNIT /*unit*/) const
{
  return QString("-");
}

bool CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::Defined() const
{
  return true;
}

IValueDomainScalar::TMinMax CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::MinMax(IProgressBase& /*progressBase*/, const UNIT /*unit*/) const
{
  return IValueDomainScalar::TMinMax(0, 0);
}

IValueDomainScalar::TValue CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::ValuePoint(const geo::IPoint& /*pt*/, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  return geo::CValue(0);
}

IValueDomainScalar::TValueVec CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::ValueElement(const geo::IElement& elm, const UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  IValueDomainScalar::TValueVec vcValues(elm.NrOfNodes(), 0);
  return vcValues;
}

IValueDomainScalar::TValue CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::Average(IProgressBase&, const UNIT) const
{
  return IValueDomainScalar::TValue();
}

unsigned int CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::IconId() const
{
  return 0;
}

unsigned int CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::TypeId() const
{
  return 0;
}

bool CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::Empty() const
{
  return true;
}

long CVolumetricStrainLoadRepeaterComponent::CGlobalValuesComponent::SavedItems() const
{
  return 0;
}


///// CVolumetricStrainLoad
CVolumetricStrainLoad::CVolumetricStrainLoad(CFormationBase& f, CDepletionStage& d, bool bEmpty)
: TBase(f, d)
{
  LinkTo(d);
  reParent(&f);

  if(!bEmpty)
  {
    new CVolumetricStrainLoadRepeaterComponent(*this);
    new CVolumetricStrainLoadGradientComponent(*this);
  }
}

CVolumetricStrainLoad::CVolumetricStrainLoad(CFormationBase &f, CDepletionStage &d, const double& dVolStrain, const CQuantity::UNIT unit)
: TBase(f, d)
{
  LinkTo(d);
  reParent(&f);

  new CVolumetricStrainLoadRepeaterComponent(*this);
  new CVolumetricStrainLoadGradientComponent(*this);

	Constant().Set(dVolStrain,
				   0,
				   0,
				   unit);

	assert(Type() == CONSTANT_MODE);
}

CVolumetricStrainLoad::CVolumetricStrainLoad(const CVolumetricStrainLoad& rhs)
: TBase(rhs)
{
}

CVolumetricStrainLoad& CVolumetricStrainLoad::operator=(const CVolumetricStrainLoad& rhs)
{
  TBase::operator=(rhs);

  CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearAll();

  return *this;
}

void CVolumetricStrainLoad::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if(version < CStreamVersion(3, 7, 3))
  {
    // for backward compatibility
    CFormationLoadBase<TVolumetricStrain>::LoadStream(stream, version, progress);
    CVolumetricStrainLoadComponent comp(*this);
    comp.LoadStream(stream, version, progress);
    new CVolumetricStrainLoadRepeaterComponent(*this);
    new CVolumetricStrainLoadGradientComponent(*this);
  }
  else
  {
    TBase::LoadStream(stream, version, progress);
  }
}

bool CVolumetricStrainLoad::IsFirstValidDepletionStage() const
{
  assert(!DepletionStage().Initial());
  return (DepletionStage().Previous().Initial());
}

unsigned int CVolumetricStrainLoad::TypeId() const
{
  return IDT_VALUETYPE_VOLUMETRICSTRAIN;
}

QString CVolumetricStrainLoad::TypeName() const
{
  return getStringTableEntry(IDS_VALUENAME_VOLUMETRICSTRAIN);
}

void CVolumetricStrainLoad::OnNewNeighbour(const CGraphNode& node)
{
  const TVolumetricStrain* pVolStrain = dynamic_cast<const TVolumetricStrain*>(&node);
  if(pVolStrain)
  {
    CModelBase& model = static_cast<CModelBase&>(Model());
    model.ResultRegister().ClearLinear(false);
    model.ResultRegister().ClearNonLinear(false);
    model.ResultRegister().ClearMixture();
  }

  TBase::OnNewNeighbour(node);
}

void CVolumetricStrainLoad::OnNeighbourDeleted(const CGraphNode& node)
{
  const TVolumetricStrain* pVolStrain = dynamic_cast<const TVolumetricStrain*>(&node);
  if(pVolStrain)
  {
    CModelBase& model = static_cast<CModelBase&>(Model());
    if(!model.getDeleting())
    {
      model.ResultRegister().ClearLinear(false);
      model.ResultRegister().ClearNonLinear(false);
      model.ResultRegister().ClearMixture();
    }
  }

  TBase::OnNeighbourDeleted(node);
}

void CVolumetricStrainLoad::LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  new CVolumetricStrainLoadRepeaterComponent(*this);

	switch(Type())
	{
	case REPEAT_MODE:
		new CVolumetricStrainLoadGradientComponent(*this);
		break;
	case CONSTANT_MODE:
		{
			CVolumetricStrainLoadGradientComponent* pConstant = new CVolumetricStrainLoadGradientComponent(*this, true);
			pConstant->LoadStream(stream, version, progress);
		}
		break;
	default:
		assert(false);
		break;
	};
}

void CVolumetricStrainLoad::SaveComponents(TSTREAM& stream, TPROGRESS& progress)
{
	if(Type() == CONSTANT_MODE)
		Constant().SaveStream(stream, progress);
}

TVolumetricStrainLoadBase& CVolumetricStrainLoad::FormationStageLoad(CFormationBase& /*formation*/, CDepletionStage& /*stage*/)
{
  assert(false);
  TVolumetricStrainLoadBase* pBogus = 0;
  return *pBogus;
}


/////

CVolumetricStrainLoad::CVolumetricStrainLoadComponent::CVolumetricStrainLoadComponent(CVolumetricStrainLoad& parent)
: TBase(parent, 0)
{
}

CVolumetricStrainLoad::CVolumetricStrainLoadComponent::CVolumetricStrainLoadComponent(const CVolumetricStrainLoadComponent& rhs)
: TBase(rhs)
{
}

unsigned int CVolumetricStrainLoad::CVolumetricStrainLoadComponent::TypeId() const
{
  return Parent().TypeId();
}

unsigned int CVolumetricStrainLoad::CVolumetricStrainLoadComponent::IconId() const
{
  return Parent().IconId();
}

QString CVolumetricStrainLoad::CVolumetricStrainLoadComponent::TypeName() const
{
  return Parent().TypeName();
}

QString CVolumetricStrainLoad::CVolumetricStrainLoadComponent::UnitName(const CQuantity::UNIT /*unit*/) const
{
  return getStringTableEntry(IDS_UNIT_DIMENSIONLESS);
}

QString CVolumetricStrainLoad::CVolumetricStrainLoadComponent::ExportLabel() const
{
  return Parent().ExportLabel();
}

bool CVolumetricStrainLoad::CVolumetricStrainLoadComponent::Defined() const
{
  const CVolumetricStrainLoad& parent = static_cast<const CVolumetricStrainLoad&>(Parent());
  return parent.DistributedSize() > 0;
}

long CVolumetricStrainLoad::CVolumetricStrainLoadComponent::SavedItems() const
{
  return TBase::SavedItems();
}

bool CVolumetricStrainLoad::CVolumetricStrainLoadComponent::PrepareMapping(const geo::IElementSet* /*pElementSet*/)
{
  return false;
}