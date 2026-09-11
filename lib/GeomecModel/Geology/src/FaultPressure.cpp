
#include "FaultPressure.h"
#include "HorizonBase.h"
#include "Pressure.h"
#include "InterfaceElement.h"
#include "LineInterface.h"
#include "3dmodel.h"
#include "FormationBase.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "ValueTypeFactory.h"
#include "BranchState.h"
#include "StreamVersion.h"
#include "FemAppEntryTypes.h"
#include "Box.h"
#include "IPointSet.h"
#include "MeshBase.h"
#include "ElementGroup.h"
#include "TiedBodyTriangle.h"

///////////////////////////////////////////////////////
// CFaultPressure::CComponentForwarder
///////////////////////////////////////////////////////

CFaultPressure::CComponentForwarder::CComponentForwarder(CFaultPressure& faultpressure)
: CFaultPressureComponent(faultpressure, MT_FORWARDER)
{
}

CFaultPressure::CComponentForwarder::CComponentForwarder(const CComponentForwarder& rhs)
: CFaultPressureComponent(rhs)
{
}

const IValueComponentBase& CFaultPressure::CComponentForwarder::ActiveComponent() const
{
	return FaultPressure().IValueComposite::Component(0, FaultPressure().Mode());
}

unsigned int CFaultPressure::CComponentForwarder::TypeId() const
{
	return ActiveComponent().TypeId();
}

unsigned int CFaultPressure::CComponentForwarder::IconId() const
{
	return ActiveComponent().IconId();
}

QString CFaultPressure::CComponentForwarder::TypeName() const
{
	return ActiveComponent().TypeName();
}

geo::CValue CFaultPressure::CComponentForwarder::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	return ActiveComponent().ScalarData().ValuePoint(pt, unit, cb);
}

IValueDomainScalar::TValueVec CFaultPressure::CComponentForwarder::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	return ActiveComponent().ScalarData().ValueElement(elm, unit, cb);
}


///////////////////////////////////////////////////////
// CFaultPressure
///////////////////////////////////////////////////////

CFaultPressure::CFaultPressure(CHorizonBase& fault, CDepletionStage& dstage, TModeType mode)
: IValueComposite("Fault Pressure", fault.Model()),
  m_fault(fault),
  m_stage(dstage),
  m_mode(mode),
  m_bDistributedOnly(false)
{
	LinkTo(dstage);
	reParent(&fault);

	new CComponentForwarder(*this);
	new CFaultPressureGradientComponent(*this);
	new CFaultPressureMinComponent(*this);
	new CFaultPressureMaxComponent(*this);
  new CFaultPressureMaxGradientComponent(*this);
}

CFaultPressure::CFaultPressure(const CFaultPressure& rhs)
: IValueComposite(rhs),
  m_fault(rhs.m_fault),
  m_stage(rhs.m_stage),
  m_mode(rhs.m_mode),
  m_vcDistributed(rhs.m_vcDistributed),
  m_bDistributedOnly(rhs.m_bDistributedOnly)
{
  new CFaultPressureMaxGradientComponent(rhs.MaxGradientComponent());
	new CComponentForwarder(rhs.Forwarder());
	new CFaultPressureGradientComponent(rhs.Gradient());
	new CFaultPressureMinComponent(rhs.MinComponent());
	new CFaultPressureMaxComponent(rhs.MaxComponent());
}

CFaultPressure::~CFaultPressure()
{
	assert(!IsCopied());
}

CFaultPressure& CFaultPressure::operator=(const CFaultPressure& rhs)
{
	if(!(*this == rhs))
	{
		CModelBase& model = static_cast<CModelBase&>(Model());
	  model.ResultRegister().ClearLinear(false);
	  model.ResultRegister().ClearNonLinear(false);
	  model.ResultRegister().ClearMixture();
	}

	IValueComposite::operator=(rhs);

	assert(&m_fault == &rhs.m_fault);
	assert(&m_stage == &rhs.m_stage);
	m_mode = rhs.m_mode;
	m_bDistributedOnly = rhs.m_bDistributedOnly;
	m_vcDistributed = rhs.m_vcDistributed;

	Gradient() = rhs.Gradient();
	MinComponent() = rhs.MinComponent();
	MaxComponent() = rhs.MaxComponent();
  MaxGradientComponent() = rhs.MaxGradientComponent();

	return *this;
}

bool CFaultPressure::operator==(const CFaultPressure& rhs) const
{
	return (
		IValueComposite::operator ==(rhs)			           &&
		&m_fault               == &rhs.m_fault           &&
		&m_stage               == &rhs.m_stage           &&
		m_mode                 == rhs.m_mode             &&
		m_bDistributedOnly     == rhs.m_bDistributedOnly &&
		m_vcDistributed        == rhs.m_vcDistributed    &&
		Gradient()             == rhs.Gradient()         &&
		MinComponent()         == rhs.MinComponent()     &&
		MaxComponent()         == rhs.MaxComponent()     &&
    MaxGradientComponent() == rhs.MaxGradientComponent());
}

CFaultPressure::TModeType CFaultPressure::Mode() const
{
	return m_mode;
}

void CFaultPressure::Mode(TModeType mode)
{
	m_mode = mode;
	Modified();
}

const CFaultPressure::CComponentForwarder& CFaultPressure::Forwarder() const
{
	return (const CComponentForwarder&)IValueComposite::Component(0, MT_FORWARDER);
}

CFaultPressure::CComponentForwarder& CFaultPressure::Forwarder()
{
	return (CComponentForwarder&)IValueComposite::Component(0, MT_FORWARDER);
}

const CFaultPressureGradientComponent& CFaultPressure::Gradient() const
{
	return (const CFaultPressureGradientComponent&)IValueComposite::Component(0, MT_GRADIENT);
}

CFaultPressureGradientComponent& CFaultPressure::Gradient()
{
	return (CFaultPressureGradientComponent&)IValueComposite::Component(0, MT_GRADIENT);
}

const CFaultPressureMinComponent& CFaultPressure::MinComponent() const
{
	return (const CFaultPressureMinComponent&)IValueComposite::Component(0, MT_MINIMUM);
}

CFaultPressureMinComponent& CFaultPressure::MinComponent()
{
	return (CFaultPressureMinComponent&)IValueComposite::Component(0, MT_MINIMUM);
}

const CFaultPressureMaxComponent& CFaultPressure::MaxComponent() const
{
	return (const CFaultPressureMaxComponent&)IValueComposite::Component(0, MT_MAXIMUM);
}

CFaultPressureMaxComponent& CFaultPressure::MaxComponent()
{
	return (CFaultPressureMaxComponent&)IValueComposite::Component(0, MT_MAXIMUM);
}

const CFaultPressureMaxGradientComponent& CFaultPressure::MaxGradientComponent() const
{
	return (const CFaultPressureMaxGradientComponent&)IValueComposite::Component(0, MT_MAXGRADIENT);
}

CFaultPressureMaxGradientComponent& CFaultPressure::MaxGradientComponent()
{
	return (CFaultPressureMaxGradientComponent&)IValueComposite::Component(0, MT_MAXGRADIENT);
}

size_t CFaultPressure::DistributedSize() const
{
	return m_vcDistributed.size();
}

const TPressure& CFaultPressure::DistributedValue(size_t nIndex) const
{
	assert(nIndex < m_vcDistributed.size());
	return *m_vcDistributed[nIndex];
}

bool CFaultPressure::DistributedOnly() const
{
	return m_bDistributedOnly;
}

void CFaultPressure::DistributedOnly(bool bDistributedOnly)
{
	m_bDistributedOnly = bDistributedOnly;
	Modified();
}

void CFaultPressure::OnNewNeighbour(const CGraphNode &node)
{
	const TPressure* pDistriPressure = dynamic_cast<const TPressure*>(&node);
	if(pDistriPressure)
	{
		m_vcDistributed.push_back(pDistriPressure);
		Modified();
		Component().Modified();
	}
}

void CFaultPressure::OnNeighbourDeleted(const CGraphNode &node)
{
	for(size_t i = 0; i < m_vcDistributed.size(); ++i)
	{
		if(m_vcDistributed[i] == &node)
		{
			m_vcDistributed.erase(m_vcDistributed.begin() + i);
			Modified();
			Component().Modified();
			break;
		}
	}

	bool bDelete = false;

	if(&node == &m_stage)
		bDelete = true;

	IValueComposite::OnNeighbourDeleted(node);

	if(bDelete)
		delete this;
}

bool CFaultPressure::CanConnectItem(const CGraphNode &item) const
{
	const TPressure* pDistriPressure = dynamic_cast<const TPressure*>(&item);
  const CModelBase& model = static_cast<const CModelBase&>(Model());
	return (!model.BranchState().IsFixedStage(m_stage) && pDistriPressure != 0 && pDistriPressure->Component().Defined() && !IsLinkedTo(*pDistriPressure));
}

bool CFaultPressure::CanDisconnectItem(const CGraphNode& item) const
{
	const TPressure* pDistriPressure = dynamic_cast<const TPressure*>(&item);
  if(pDistriPressure)
    return !(static_cast<const CModelBase&>(Model())).BranchState().IsFixedStage(m_stage);

  return IValueComposite::CanDisconnectItem(item);
}

long CFaultPressure::SavedItems() const
{
  return DistributedSize() + MaxComponent().SavedItems() + MinComponent().SavedItems() + Gradient().SavedItems();
}

void CFaultPressure::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	assert(CStreamVersion(3, 0, 87) < version);

	TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);

	int distriOnly;
	stream >> distriOnly;
	DistributedOnly(distriOnly);

	// Load the size of the distributed values
	int distriSize;
	stream >> distriSize;

	// Load and link indices
	for (int i = 0; i < distriSize; i++)
	{

		int nIndex;
		stream >> nIndex;
		assert(composite_entry.FindIndex(nIndex));
		TPressure *pValue = (TPressure*)composite_entry.FindIndex(nIndex);
		assert(pValue);
		LinkTo(*pValue);
	}

	int iMode;
	stream >> iMode;
	m_mode = (TModeType)iMode;

	MaxComponent().LoadStream(stream, version, progress);
	MinComponent().LoadStream(stream, version, progress);
	Gradient().LoadStream(stream, version, progress);

  if(version >= CStreamVersion(4, 1, 17))
    MaxGradientComponent().LoadStream(stream, version, progress);
}

void CFaultPressure::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	int distriOnly = DistributedOnly();
	stream << distriOnly;

	// Save size of distributed values
  int distributedSize = DistributedSize();
  stream << distributedSize;
	for(size_t i = 0; i < DistributedSize(); i++)
	{
		stream << DistributedValue(i).Index();
		progress.Step();
	}

	stream << (int)m_mode;

	// save components
	MaxComponent().SaveStream(stream, progress);
	MinComponent().SaveStream(stream, progress);
	Gradient().SaveStream(stream, progress);
  MaxGradientComponent().SaveStream(stream, progress);
}

unsigned int CFaultPressure::ModeSize() const
{
	return 1;
}

QString CFaultPressure::ModeName(unsigned int /*uMode*/) const
{
	return QString();
}

unsigned int CFaultPressure::ComponentSize(unsigned int /*uMode*/) const
{
	return 1;
}

IValueComponentBase& CFaultPressure::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/)
{
	return IValueComposite::Component(0, MT_FORWARDER);
}

const IValueComponentBase& CFaultPressure::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/) const
{
	return IValueComposite::Component(0, MT_FORWARDER);
}

unsigned int CFaultPressure::TypeId() const
{
	return IDT_FAULT_PRESSURE;
}

QString CFaultPressure::TypeName() const
{
	return Component().TypeName();
}

unsigned int CFaultPressure::IconId() const
{
	return Component().IconId();
}

const QString &CFaultPressure::Name() const
{
	return m_stage.Name();
}

bool CFaultPressure::Less(const CGraphNode &node) const
{
	const CFaultPressure* pFaultPressure = dynamic_cast<const CFaultPressure*>(&node);
	if(pFaultPressure)
		return m_stage.Less(pFaultPressure->m_stage);

	return IValueComposite::Less(node);
}

const CHorizonBase& CFaultPressure::Fault() const
{
	return m_fault;
}

CHorizonBase& CFaultPressure::Fault()
{
	return m_fault;
}

const CDepletionStage& CFaultPressure::DepletionStage() const
{
	return m_stage;
}

CDepletionStage& CFaultPressure::DepletionStage()
{
	return m_stage;
}

///////////////////////////////////////////////////////
// CFaultPressureComponent
///////////////////////////////////////////////////////

CFaultPressureComponent::CFaultPressureComponent(CFaultPressure& faultpressure, unsigned int uMode)
: IValueComponentBase(faultpressure, 0, uMode), m_Data(*this)
{
}

CFaultPressureComponent::CFaultPressureComponent(const CFaultPressureComponent& rhs)
: IValueComponentBase(rhs), m_Data(rhs.m_Data)
{
}

CFaultPressureComponent& CFaultPressureComponent::operator=(const CFaultPressureComponent& rhs)
{
  IValueComponentBase::operator=(rhs);

  m_Data = rhs.m_Data;

  return *this;
}

IValueComponentBase::TValueComponentType CFaultPressureComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CFaultPressureComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CFaultPressureComponent::ScalarData()
{
  return m_Data;
}

const CFaultPressure& CFaultPressureComponent::FaultPressure() const
{
	return (const CFaultPressure&)(Parent());
}

CFaultPressure& CFaultPressureComponent::FaultPressure()
{
	return (CFaultPressure&)(Parent());
}

QString CFaultPressureComponent::UnitName(const CQuantity::UNIT unit) const
{
	QString sRet;
	if(unit == CQuantity::SI_UNIT)
		sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
	else
		sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

	return sRet;
}

QString CFaultPressureComponent::ExportLabel() const
{
	 return getStringTableEntry(IDS_ET_PRESSURE);
}

IValueDomainScalar::TMinMax CFaultPressureComponent::MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
	geo::CBox box(FaultPressure().Fault().Min(), FaultPressure().Fault().Max());
	return ScalarData().MinMax(progressBase, box, unit);
}

IValueDomainScalar::TValue CFaultPressureComponent::Average(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
	geo::CBox box(FaultPressure().Fault().Min(), FaultPressure().Fault().Max());
	return ScalarData().Average(progressBase, box, unit);
}

bool CFaultPressureComponent::Empty() const
{
	return false;
}

long CFaultPressureComponent::SavedItems() const
{
	return 0;
}

bool CFaultPressureComponent::Defined() const
{
	return true;
}

geo::CValue CFaultPressureComponent::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	if(FaultPressure().DistributedSize() > 0)
	{
		int n = 0;
		double dResult = 0;
		for(size_t i = 0; i < FaultPressure().DistributedSize(); i++)
		{
			const TPressure& pressure = FaultPressure().DistributedValue(i);
			if(FaultPressure().DistributedOnly() || pressure.PointSet().PointInConvexHull(pt))
			{
				geo::CValue value = pressure.Component().ScalarData().ValuePoint(pt, unit, cb);
				if(!value.Valid())
					return geo::CValue();
				dResult += value.Value();
				n++;
			}
		}

		if(n > 0)
			return geo::CValue(dResult / n);
	}

	return geo::CValue();
}

IValueDomainScalar::TValueVec CFaultPressureComponent::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	IValueDomainScalar::TValueVec vcRet(elm.NrOfPoints());
	std::vector<int> vcNrOfValue(elm.NrOfPoints(), 0);
	if(FaultPressure().DistributedSize() > 0)
	{
		for(size_t i = 0; i < FaultPressure().DistributedSize(); i++)
		{
			const TPressure& pressure = FaultPressure().DistributedValue(i);
			IValueDomainScalar::TValueVec vcValue = pressure.Component().ScalarData().ValueElement(elm, unit, cb);
			for(int n = 0; n < elm.NrOfPoints(); ++n)
			{
				if((FaultPressure().DistributedOnly() || pressure.PointSet().PointInConvexHull(elm.Point(n))) && vcValue[n].Valid())
				{
					if(vcNrOfValue[n] == 0)
						vcRet[n] = vcValue[n];
					else
						vcRet[n] = vcRet[n] + vcValue[n];
					++vcNrOfValue[n];
				}
			}
		}

		for(int n = 0; n < elm.NrOfPoints(); ++n)
		{
			if(vcNrOfValue[n] > 1)
				vcRet[n] = vcRet[n] / vcNrOfValue[n];
		}
	}
	
	return vcRet;
}

bool CFaultPressureComponent::CanMap(const COpenGLNode& node) const
{
	const CHorizonBase* pFault = dynamic_cast<const CHorizonBase*>(&node);
	if(pFault)
	{
		const CModelBase& model = static_cast<const CModelBase&>(Model());
		return pFault->Slip() && model.Mesh().IsMesh();
	}

	return false;
}

const CFormationBase& CFaultPressureComponent::Formation(const geo::IElement& elm) const
{
	const CModelBase& model = static_cast<const CModelBase&>(Model());
	assert(model.Mesh().IsMesh());

	const geo::CBodyQuadrilateral* pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral*>(&elm);
	if(pBodyQuad)
		return *model.Mesh().Formation(*pBodyQuad->Parent());

	const geo::CBodyTriangle* pBodyTriangle = dynamic_cast<const geo::CBodyTriangle*>(&elm);
	if(pBodyTriangle)
		return *model.Mesh().Formation(*pBodyTriangle->Parent());

  const geo::CTiedBodyTriangle *pTiedBodyTriangle = dynamic_cast<const geo::CTiedBodyTriangle *>(&elm);
  if (pTiedBodyTriangle)
  {
    assert(false); // it's probably wrong to find the formation for the tied body triangle directly, but since all body triangles *should* lie in the same formation, we can return it
    const CFormationBase *pFormationBase = model.Mesh().Formation(*pTiedBodyTriangle->BodyTriangle(0));
#ifdef _DEBUG
    for (int i = 1; i < 3; ++i)
    {
      const CFormationBase *pOtherFormationBase = model.Mesh().Formation(*pTiedBodyTriangle->BodyTriangle(i));
      assert(pOtherFormationBase == pFormationBase);
    }
#endif
    return *pFormationBase;
  }

	return *model.Mesh().Formation(elm);
}

IValueDomainScalar::TValueVec CFaultPressureComponent::InterfaceValues3D(const geo::CInterfaceElement& iface, bool bMax, const CQuantity::UNIT unit) const
{
	IValueDomainScalar::TValueVec vcRet(iface.NrOfNodes());

	const CModelBase& model = static_cast<const CModelBase&>(Model());

	if(!model.Mesh().IsMesh())
		return vcRet;

	// the two formation elements attached at either side of the interface
	const geo::IElement& front = iface.Front();
	const geo::IElement& back  = iface.Back();

	// the pressure values in both formations
  IValueDomainScalar::TValueVec vcFrontValues = Formation(front).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(front, unit);
  IValueDomainScalar::TValueVec vcBackValues(vcFrontValues.size(), 0);
  
  if (dynamic_cast<const geo::CTiedBodyTriangle *>(iface.BackFace()))
  {
    const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(iface.BackFace());
    for (int i = 0; i < 3; ++i)
    {
      IValueDomainScalar::TValueVec vcTmpValues = Formation(*pTiedBodyTriangle->BodyTriangle(i)).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(*pTiedBodyTriangle->BodyTriangle(i), unit);
      assert(vcTmpValues.size() == vcBackValues.size());
      for (int j = 0; j < vcTmpValues.size(); ++j)
        vcBackValues[j] += vcTmpValues[j];
    }
    for (int j = 0; j < vcBackValues.size(); ++j)
      vcBackValues[j] /= 3;
  }
  else
  {
    vcBackValues = Formation(back).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(back, unit);
  }

	assert(vcFrontValues.size() == front.NrOfNodes());
	assert(vcBackValues.size() == back.NrOfNodes());

	int i;
	int numpoints = iface.NrOfNodes() / 2; // the number of physical (non-conciding) points
	for(i = 0; i < numpoints; ++i)
	{
		// get the formation element (front or back) and the body node index for this interface node index i

		int iFrontNode;
		int iBackNode;

		iFrontNode = iface.BodyFaceNode(i);
		iBackNode = iface.BodyFaceNode(i + numpoints);
    assert(iFrontNode >= 0);
    assert(iBackNode >= 0);

    assert(iface.Node(i) == iface.Node(i + numpoints)); // should coincide
    assert(front.Node(iFrontNode) == back.Node(iBackNode));

		assert(iFrontNode >= 0 && iFrontNode < vcFrontValues.size());
		assert(iBackNode >= 0 && iBackNode < vcBackValues.size());

		if(vcFrontValues[iFrontNode].Valid() && vcBackValues[iBackNode].Valid())
		{
			if(bMax)
				vcRet[i] = std::max(vcFrontValues[iFrontNode].Value(), vcBackValues[iBackNode].Value());
			else
				vcRet[i] = std::min(vcFrontValues[iFrontNode].Value(), vcBackValues[iBackNode].Value());
		}
		else if(vcFrontValues[iFrontNode].Valid())
			vcRet[i] = vcFrontValues[iFrontNode];
		else if(vcBackValues[iBackNode].Valid())
			vcRet[i] = vcBackValues[iBackNode];
		// otherwise it remains invalid

		// copy to other side
		vcRet[i + numpoints] = vcRet[i];
	}

	return vcRet;
}

// first front, second back
std::pair<const geo::IElement*, const geo::IElement*> CFaultPressureComponent::GetFrontAndBack(const geo::IInterfaceElement& iface) const
{
	assert(dynamic_cast<const C3DModel*>(&Model()));
	const geo::CInterfaceElement* p3DInterface = dynamic_cast<const geo::CInterfaceElement*>(&iface);
	assert(p3DInterface);
  return std::make_pair(&p3DInterface->Front(), &p3DInterface->Back());
}

IValueDomainScalar::TValueVec CFaultPressureComponent::InterfaceValues(const geo::IInterfaceElement& iface, bool bMax, const CQuantity::UNIT unit) const
{
	assert(dynamic_cast<const C3DModel*>(&Model()));
	const geo::CInterfaceElement* p3DInterface = dynamic_cast<const geo::CInterfaceElement*>(&iface);
	assert(p3DInterface);

	return InterfaceValues3D(*p3DInterface, bMax, unit);
}


///////////////////////////////////////////////////////
// CFaultPressureGradientComponent
///////////////////////////////////////////////////////

CFaultPressureGradientComponent::CFaultPressureGradientComponent(CFaultPressure& faultpressure)
: CFaultPressureComponent(faultpressure, CFaultPressure::MT_GRADIENT),
  m_depth(0),
  m_refpressure(0),
  m_gradient(0)
{
}

CFaultPressureGradientComponent::CFaultPressureGradientComponent(const CFaultPressureGradientComponent& rhs)
: CFaultPressureComponent(rhs),
  m_depth(rhs.m_depth),
  m_refpressure(rhs.m_refpressure),
  m_gradient(rhs.m_gradient)
{
}

CFaultPressureGradientComponent& CFaultPressureGradientComponent::operator=(const CFaultPressureGradientComponent& rhs)
{
	if(!(*this == rhs))
	{
		CModelBase& model = static_cast<CModelBase&>(Model());
	  model.ResultRegister().ClearLinear(false);
	  model.ResultRegister().ClearNonLinear(false);
	  model.ResultRegister().ClearMixture();
	}

	CFaultPressureComponent::operator=(rhs);

	m_depth = rhs.m_depth;
	m_refpressure = rhs.m_refpressure;
	m_gradient = rhs.m_gradient;

	return *this;
}

bool CFaultPressureGradientComponent::operator==(const CFaultPressureGradientComponent& rhs) const
{
	return (
		m_depth == rhs.m_depth &&
		m_refpressure == rhs.m_refpressure &&
		m_gradient == rhs.m_gradient);
}

const CLengthQuantity& CFaultPressureGradientComponent::ReferenceDepth() const
{
	return m_depth;
}

const CSinglePressure& CFaultPressureGradientComponent::ReferencePressure() const
{
	return m_refpressure;
}

const CPressureGradientQuantity& CFaultPressureGradientComponent::Gradient() const
{
	return m_gradient;
}

void CFaultPressureGradientComponent::ReferenceDepth(const double& value, const CQuantity::UNIT unit)
{
	m_depth.Value(value, unit);
}

void CFaultPressureGradientComponent::ReferencePressure(const double& value, const CQuantity::UNIT unit)
{
	m_refpressure.Value(value, unit);
}

void CFaultPressureGradientComponent::Gradient(const double& value, const CQuantity::UNIT unit)
{
	m_gradient.Value(value, unit);
}

unsigned int CFaultPressureGradientComponent::TypeId() const
{
	return IDT_COMPONENT_PRESSURE_CONSTANT;
}

unsigned int CFaultPressureGradientComponent::IconId() const
{
	return IDI_COMPONENT_PRESSURE_CONSTANT;
}

QString CFaultPressureGradientComponent::TypeName() const
{
	return getStringTableEntry(IDS_COMPONENT_PRESSURE_CONSTANT);
}

long CFaultPressureGradientComponent::SavedItems() const
{
	return 1;
}

void CFaultPressureGradientComponent::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 87) < version)
	{
		double d;
		stream >> d;
		m_depth.Value(d, CQuantity::SI_UNIT);
		stream >> d;
		m_refpressure.Value(d, CQuantity::SI_UNIT);
		stream >> d;
		m_gradient.Value(d, CQuantity::SI_UNIT);

		progress.Step();
	}
}

void CFaultPressureGradientComponent::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	stream << m_depth.Value(CQuantity::SI_UNIT);
	stream << m_refpressure.Value(CQuantity::SI_UNIT);
	stream << m_gradient.Value(CQuantity::SI_UNIT);

	progress.Step();
}

geo::CValue CFaultPressureGradientComponent::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const CModelBase& model = static_cast<const CModelBase&> (Model());

	geo::CValue value = CFaultPressureComponent::ValuePoint(pt, unit, cb);
	if(value.Valid())
		return value;

	return geo::CValue((ReferenceDepth().Convert(model.Depth(pt), unit, CQuantity::SI_UNIT) - ReferenceDepth().Value(unit)) * Gradient().Value(unit) + ReferencePressure().Value(unit));
}

IValueDomainScalar::TValueVec CFaultPressureGradientComponent::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const CModelBase& model = static_cast<const CModelBase&> (Model());

	IValueDomainScalar::TValueVec vcRet = CFaultPressureComponent::ValueElement(elm, unit, cb);

	for(int i = 0; i < vcRet.size(); i++)
	{
		if(!vcRet[i].Valid())
			vcRet[i] = geo::CValue((ReferenceDepth().Convert(model.Depth(elm.Point(i)), unit, CQuantity::SI_UNIT) - ReferenceDepth().Value(unit)) * Gradient().Value(unit) + ReferencePressure().Value(unit));
	}

	return vcRet;
}


///////////////////////////////////////////////////////
// CFaultPressureMinComponent
///////////////////////////////////////////////////////

CFaultPressureMinComponent::CFaultPressureMinComponent(CFaultPressure& faultpressure)
: CFaultPressureComponent(faultpressure, CFaultPressure::MT_MINIMUM)
{
}

CFaultPressureMinComponent::CFaultPressureMinComponent(const CFaultPressureMinComponent& rhs)
: CFaultPressureComponent(rhs)
{
}

unsigned int CFaultPressureMinComponent::TypeId() const
{
	return IDT_COMPONENT_PRESSURE_MINIMUM;
}

unsigned int CFaultPressureMinComponent::IconId() const
{
	return IDI_COMPONENT_PRESSURE_MINIMUM;
}

QString CFaultPressureMinComponent::TypeName() const
{
	return getStringTableEntry(IDS_COMPONENT_PRESSURE_MINIMUM);
}

geo::CValue CFaultPressureMinComponent::ValuePoint(const geo::IPoint& /*pt*/, const CQuantity::UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
	// this component needs an element, not possible
	assert(false);
	return geo::CValue();
}

IValueDomainScalar::TValueVec CFaultPressureMinComponent::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	IValueDomainScalar::TValueVec vcRet = CFaultPressureComponent::ValueElement(elm, unit, cb);

	const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
	if(pInterface)
	{
		IValueDomainScalar::TValueVec vcValues = InterfaceValues(*pInterface, false, unit);
		assert(vcValues.size() == vcRet.size());

		for(size_t i = 0; i < vcValues.size(); ++i)
		{
			if(!vcRet[i].Valid())
				vcRet[i] = vcValues[i];
		}
	}

	return vcRet;
}


///////////////////////////////////////////////////////
// CFaultPressureMaxComponent
///////////////////////////////////////////////////////

CFaultPressureMaxComponent::CFaultPressureMaxComponent(CFaultPressure& faultpressure)
: CFaultPressureComponent(faultpressure, CFaultPressure::MT_MAXIMUM)
{
}

CFaultPressureMaxComponent::CFaultPressureMaxComponent(const CFaultPressureMaxComponent& rhs)
: CFaultPressureComponent(rhs)
{
}

unsigned int CFaultPressureMaxComponent::TypeId() const
{
	return IDT_COMPONENT_PRESSURE_MAXIMUM;
}

unsigned int CFaultPressureMaxComponent::IconId() const
{
	return IDI_COMPONENT_PRESSURE_MAXIMUM;
}

QString CFaultPressureMaxComponent::TypeName() const
{
	return getStringTableEntry(IDS_COMPONENT_PRESSURE_MAXIMUM);
}

geo::CValue CFaultPressureMaxComponent::ValuePoint(const geo::IPoint& /*pt*/, const CQuantity::UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
	// this component needs an element, not possible
	assert(false);
	return geo::CValue();
}

IValueDomainScalar::TValueVec CFaultPressureMaxComponent::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	IValueDomainScalar::TValueVec vcRet = CFaultPressureComponent::ValueElement(elm, unit, cb);

	const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
	if(pInterface)
	{
		IValueDomainScalar::TValueVec vcValues = InterfaceValues(*pInterface, true, unit);
		assert(vcValues.size() == vcRet.size());

		for(size_t i = 0; i < vcValues.size(); ++i)
		{
			if(!vcRet[i].Valid())
				vcRet[i] = vcValues[i];
		}
	}

	return vcRet;
}


///////////////////////////////////////////////////////
// CFaultPressureMaxGradientComponent
///////////////////////////////////////////////////////

CFaultPressureMaxGradientComponent::CFaultPressureMaxGradientComponent(CFaultPressure& faultpressure)
: CFaultPressureComponent(faultpressure, CFaultPressure::MT_MAXGRADIENT)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  LinkTo(model.ResultRegister());
}

CFaultPressureMaxGradientComponent::CFaultPressureMaxGradientComponent(const CFaultPressureMaxGradientComponent& rhs)
: CFaultPressureComponent(rhs)
{
}

unsigned int CFaultPressureMaxGradientComponent::TypeId() const
{
	return IDT_COMPONENT_PRESSURE_MAXGRADIENT;
}

unsigned int CFaultPressureMaxGradientComponent::IconId() const
{
	return IDI_COMPONENT_PRESSURE_MAXGRADIENT;
}

QString CFaultPressureMaxGradientComponent::TypeName() const
{
	return getStringTableEntry(IDS_COMPONENT_PRESSURE_MAXGRADIENT);
}

geo::CValue CFaultPressureMaxGradientComponent::ValuePoint(const geo::IPoint& /*pt*/, const CQuantity::UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
	// this component needs an element, not possible
	assert(false);
	return geo::CValue();
}

IValueDomainScalar::TValueVec CFaultPressureMaxGradientComponent::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  if(m_refpressure.Undefined() || m_gradient.Undefined())
    DetermineValues();
	IValueDomainScalar::TValueVec vcRet = CFaultPressureComponent::ValueElement(elm, unit, cb);

	const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
	if(pInterface)
	{
    assert(pInterface->NrOfNodes() == vcRet.size());
		int i;
		for(i = 0; i < pInterface->NrOfNodes(); ++i)
		{
			if(!vcRet[i].Valid())
				vcRet[i] = m_refpressure.Value() + pInterface->Node(i).Z() * m_gradient.Value();
		}
	}

	return vcRet;
}

void CFaultPressureMaxGradientComponent::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  if(&node == &model.ResultRegister())
  {
    // invalidate values if something changed in the model that invalidated the results
    m_gradient.Invalidate();
    m_refpressure.Invalidate();
    FaultPressure().Modified();
  }

  CFaultPressureComponent::OnNeighbourModified(node, uHint);
}

void CFaultPressureMaxGradientComponent::DetermineValues() const
{
  // first determine the fluid density, take lowest value from adjacent formation elements' material properties
  const CHorizonBase& fault = FaultPressure().Fault();
  const geo::CElementGroup* pInterfaces = fault.InterfaceElementGroup();
  assert(pInterfaces);
  if(!pInterfaces)
    return;

  assert(pInterfaces->ElementSize());
  if(!pInterfaces->ElementSize())
    return;

  double dLowestDensity = 0;

  for(int i = 0; i < pInterfaces->ElementSize(); ++i)
  {
    const geo::IElement& elm = pInterfaces->Element(i);
    assert(dynamic_cast<const geo::IInterfaceElement*>(&elm));
    const geo::IInterfaceElement& iface = static_cast<const geo::IInterfaceElement&>(elm);

    // get front and back elements for this interface
    std::pair<const geo::IElement*, const geo::IElement*> prAdjacent = GetFrontAndBack(iface);
    assert(prAdjacent.first && prAdjacent.second);

    // get front fluid density
    const CFFMaterial& front_mat = Formation(*prAdjacent.first).Material(FaultPressure().DepletionStage()).Material(*prAdjacent.first);
    assert(front_mat.IsParameter(IDT_VALUETYPE_FLUID_DENSITY));
    double dFrontDensity = front_mat.ParameterValue(IDT_VALUETYPE_FLUID_DENSITY);

    // get back fluid density
    double dBackDensity = 0;

    if (dynamic_cast<const geo::CTiedBodyTriangle *>(prAdjacent.second))
    {
      const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(prAdjacent.second);

      for (int j = 0; j < 3; ++j)
      {
        const CFFMaterial& back_mat = Formation(*pTiedBodyTriangle->BodyTriangle(j)).Material(FaultPressure().DepletionStage()).Material(*pTiedBodyTriangle->BodyTriangle(j));

        assert(back_mat.IsParameter(IDT_VALUETYPE_FLUID_DENSITY));

        dBackDensity += back_mat.ParameterValue(IDT_VALUETYPE_FLUID_DENSITY);
      }

      dBackDensity /= 3;

    }
    else
    {
      const CFFMaterial& back_mat = Formation(*prAdjacent.second).Material(FaultPressure().DepletionStage()).Material(*prAdjacent.second);

      assert(back_mat.IsParameter(IDT_VALUETYPE_FLUID_DENSITY));

      dBackDensity = back_mat.ParameterValue(IDT_VALUETYPE_FLUID_DENSITY);
    }

    double dMinDensity = std::min(dFrontDensity, dBackDensity);
    if(!i || dMinDensity < dLowestDensity)
      dLowestDensity = dMinDensity;
  }

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  double dGravity = model.Gravity().Value();
  m_gradient.SetValue(dLowestDensity * dGravity * 1e-6); // from kg/m to MPa/m

  for(int i = 0; i < pInterfaces->ElementSize(); ++i)
  {
    const geo::IElement& elm = pInterfaces->Element(i);
    assert(dynamic_cast<const geo::IInterfaceElement*>(&elm));
    const geo::IInterfaceElement& iface = static_cast<const geo::IInterfaceElement&>(elm);

    // get front and back elements (2D: faces, 3D: bodies) for this interface
    std::pair<const geo::IElement*, const geo::IElement*> prAdjacent = GetFrontAndBack(iface);
    assert(prAdjacent.first && prAdjacent.second);

    // get pressure values
    IValueDomainScalar::TValueVec vcFrontValues = Formation(*prAdjacent.first).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(*prAdjacent.first);
    IValueDomainScalar::TValueVec vcBackValues(vcFrontValues.size(), 0);
    
    if (dynamic_cast<const geo::CTiedBodyTriangle *>(prAdjacent.second))
    {
      const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(prAdjacent.second);
      for (int j = 0; j < 3; ++j)
      {
        IValueDomainScalar::TValueVec vcTmpValues = Formation(*pTiedBodyTriangle->BodyTriangle(j)).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(*pTiedBodyTriangle->BodyTriangle(j));
        assert(vcTmpValues.size() == vcBackValues.size());
        for (int k = 0; k < vcTmpValues.size(); ++k)
          vcBackValues[k] += vcTmpValues[k];
      }
      for (int k = 0; k < vcBackValues.size(); ++k)
        vcBackValues[k] /= 3;
    }
    else
    {
      vcBackValues = Formation(*prAdjacent.second).Pressure(FaultPressure().DepletionStage()).Component().ScalarData().ValueElement(*prAdjacent.second);
    }

    assert(vcFrontValues.size() == vcBackValues.size());
    assert(vcFrontValues.size() + vcBackValues.size() == iface.NrOfNodes());
    assert(vcFrontValues.size() == prAdjacent.first->NrOfNodes());
    assert(vcBackValues.size() == prAdjacent.second->NrOfNodes());

    // find max value for each (colocated) interface element node
    for(int j = 0; j < iface.NrOfNodes() / 2; ++j)
    {
      int iFrontNodeID = iface.Node(j).Index();
      int iBackNodeID = iface.Node(j + iface.NrOfNodes() / 2).Index();

      double dMaxPressure = 0;

      for(int k = 0; k < prAdjacent.first->NrOfNodes(); ++k)
      {
        if(prAdjacent.first->Node(k).Index() == iFrontNodeID)
        {
          dMaxPressure = vcFrontValues[k].Value();
          break;
        }
      }

      for(int k = 0; k < prAdjacent.second->NrOfNodes(); ++k)
      {
        if(prAdjacent.second->Node(k).Index() == iBackNodeID)
        {
          if(vcBackValues[k].Value() > dMaxPressure)
            dMaxPressure = vcBackValues[k].Value();
          break;
        }
      }

      double dZ = iface.Node(j).Z();

      // reference value (mapped to Z=0 using gradient)
      double dRefValue = dMaxPressure - dZ * m_gradient.Value();

      if((!i && !j) || dRefValue > m_refpressure.Value())
        m_refpressure.SetValue(dRefValue);
    }
  }
}
