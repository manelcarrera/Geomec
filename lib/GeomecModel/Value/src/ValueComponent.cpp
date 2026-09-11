// ValueComponent.cpp: implementation of the CValueComponent class.
//
//////////////////////////////////////////////////////////////////////

#include "ValueComponent.h"
#include "ValueType.h"
#include "ElementSet.h"
#include "RpnValueSet.h"

#include "MeshBase.h"
#include "resourceIDS.h"
#include "resourceIDI.h"
#include "FieldFactor.h"
#include "ModelBase.h"


const Ranges::CRangeType Ranges::NoRange(false, 0., false, false, 0., false);

const Units::CUnitType Units::LengthUnit         (IDS_UNIT_SI_LENGTH,         IDS_UNIT_FIELD_LENGTH,         FF_FACTOR_LENGTH        );
const Units::CUnitType Units::PressureUnit       (IDS_UNIT_SI_PRESSURE,       IDS_UNIT_FIELD_PRESSURE,       FF_FACTOR_PRESSURE      );
const Units::CUnitType Units::TimeUnit           (IDS_UNIT_SI_TIME,           IDS_UNIT_FIELD_TIME,           FF_FACTOR_TIME);
const Units::CUnitType Units::PerTimeUnit        (IDS_UNIT_SI_PERTIME,        IDS_UNIT_FIELD_PERTIME,        FF_FACTOR_PERTIME       );
const Units::CUnitType Units::StressUnit         (IDS_UNIT_SI_STRESS,         IDS_UNIT_FIELD_STRESS,         FF_FACTOR_STRESS        );
const Units::CUnitType Units::AngleUnit          (IDS_UNIT_SI_ANGLE,          IDS_UNIT_FIELD_ANGLE,          FF_FACTOR_DIMENSIONLESS );
const Units::CUnitType Units::DimensionLessUnit  (IDS_UNIT_DIMENSIONLESS,     IDS_UNIT_DIMENSIONLESS,        FF_FACTOR_DIMENSIONLESS );
const Units::CUnitType Units::DensityUnit        (IDS_UNIT_SI_DENSITY,        IDS_UNIT_FIELD_DENSITY,        FF_FACTOR_DENSITY       );
const Units::CUnitType Units::PerTemperUnit      (IDS_UNIT_SI_PERTEMPER,      IDS_UNIT_FIELD_PERTEMPER,      FF_FACTOR_PERTEMPER	   );
const Units::CUnitType Units::TemperatureUnit    (IDS_UNIT_TEMPERATURE,       IDS_UNIT_TEMPERATURE,          FF_FACTOR_TEMPERATURE   );
const Units::CUnitType Units::VelocityUnit       (IDS_UNIT_SI_VELOCITY,       IDS_UNIT_FIELD_VELOCITY,       FF_FACTOR_VELOCITY      );
const Units::CUnitType Units::PerStressUnit      (IDS_UNIT_SI_PERSTRESS,      IDS_UNIT_FIELD_PERSTRESS,      FF_FACTOR_PERSTRESS     );
const Units::CUnitType Units::StressGradientUnit (IDS_UNIT_SI_STRESSGRADIENT, IDS_UNIT_FIELD_STRESSGRADIENT, FF_FACTOR_STRESSGRADIENT);
const Units::CUnitType Units::ThermalConductUnit (IDS_UNIT_SI_THERM_CONDUCT,  IDS_UNIT_FIELD_THERM_CONDUCT,  FF_FACTOR_THERM_CONDUCT );
const Units::CUnitType Units::ThermalCapaciUnit  (IDS_UNIT_SI_THERM_CAPACI,   IDS_UNIT_FIELD_THERM_CAPACI,   FF_FACTOR_THERM_CAPACI  );
const Units::CUnitType Units::PerLengthUnit      (IDS_UNIT_SI_PERLENGTH,      IDS_UNIT_FIELD_PERLENGTH,      FF_FACTOR_PERLENGTH     );
const Units::CUnitType Units::PermeaUnit         (IDS_UNIT_SI_PERMEA,         IDS_UNIT_FIELD_PERMEA,         FF_FACTOR_PERMEA        );
const Units::CUnitType Units::ViscosityUnit      (IDS_UNIT_SI_VISCOSITY,      IDS_UNIT_FIELD_VISCOSITY,      FF_FACTOR_VISCOSITY     );

//////////////////////////////////////////////////////////////////////
// CValueComponent::CValueSet
//////////////////////////////////////////////////////////////////////
CValueComponent::CValueSet::CValueSet(const CValueComponent& component)
: m_component(component)
{
}

const geo::IElementSet &CValueComponent::CValueSet::ElementSet() const
{
  const IElementSet* pElementSet = dynamic_cast<const IElementSet*>(&m_component.ValueSet().PointSet());
  assert(pElementSet);
  return pElementSet->ElementSet();
}

CQuantity::UNIT CValueComponent::CValueSet::Unit() const
{
  const geo::IValueSet* pThis = this;
  if(m_component.m_pSIValueSet == pThis)
    return CQuantity::SI_UNIT;
  return CQuantity::FIELD_UNIT;
}

void CValueComponent::CValueSet::ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const
{
  const CValueType& value_type = dynamic_cast<const CValueType&>(m_component.Parent());
  // Get values
  values = m_component.ValueSet().ValueElement(ElementSet().Element(iElementIndex), cb);
  
  // Adapt unit if nescecarry
  if (Unit() == CQuantity::FIELD_UNIT || value_type.PointSet().ValueUnit() == CQuantity::FIELD_UNIT)
  {
  double factor = 1;
  if (Unit() == CQuantity::FIELD_UNIT)
      factor = m_component.FieldFactor();
  if (value_type.PointSet().ValueUnit() == CQuantity::FIELD_UNIT)
      factor *= m_component.FieldFactor();

  for (size_t i = 0; i < values.size(); ++i)
  {
      values[i] *= factor;
  }

  }
}

void CValueComponent::CValueSet::Empty()
{
  assert(false);
}

bool CValueComponent::CValueSet::IsEmpty() const
{
  return false;
}

int CValueComponent::CValueSet::PushBack(const TValueVec &/*values*/)
{
  assert(false);
  return - 1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CValueComponent::CValueComponent(unsigned int uName, CValueType &value_type, IValueSet &value_set, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(uName, value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
  // Assign value set
  LinkTo(value_set);
  assert(m_pValueSet == &value_set);
}

CValueComponent::CValueComponent(const QString& sName, CValueType &value_type, IValueSet &value_set, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(sName, value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
  // Assign value set
  LinkTo(value_set);
  assert(m_pValueSet == &value_set);
}

CValueComponent::CValueComponent(unsigned int uName, CValueType &value_type, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(uName, value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
}

CValueComponent::CValueComponent(const QString& sName, CValueType &value_type, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(sName, value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
}

CValueComponent::CValueComponent(CValueType &value_type, IValueSet &value_set, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
  // Assign value set
  LinkTo(value_set);
  assert(m_pValueSet == &value_set);
}

CValueComponent::CValueComponent(CValueType &value_type, unsigned int uComponent, unsigned int uMode)
: IValueComponentBase(value_type, uComponent, uMode), m_pValueSet(0), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
}

CValueComponent::CValueComponent(const CValueComponent& rhs)
: IValueComponentBase(rhs), m_pValueSet(rhs.m_pValueSet), m_pSIValueSet(0), m_pFieldValueSet(0)
, m_Data(*this)  
{
}

CValueComponent::~CValueComponent()
{
  delete m_pSIValueSet;
  delete m_pFieldValueSet;
}

IValueComponentBase::TValueComponentType CValueComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CValueComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CValueComponent::ScalarData()
{
  return m_Data;
}

const geo::IValueSet& CValueComponent::MapValueSet(const CQuantity::UNIT unit) const
{
  if(unit == CQuantity::SI_UNIT)
  {
    if(m_pSIValueSet == 0)
      m_pSIValueSet = new CValueSet(*this);
    return *m_pSIValueSet;
  }
  if(m_pFieldValueSet == 0)
    m_pFieldValueSet = new CValueSet(*this);
  return *m_pFieldValueSet;
}
  
bool CValueComponent::operator==(const CValueComponent& rhs) const
{
  assert(rhs.IsCopy());
  if(m_pValueSet != rhs.m_pValueSet)
    return false;

  // Check of the unit is changed ...
  CValueType *pType = (CValueType*) &rhs.Parent();
  if(pType->PointSet().IsCopy())
  {
    IPointSet *pSource = (IPointSet*)&pType->PointSet().Source();
    if(pSource->ValueUnit() != pType->PointSet().ValueUnit())
      return false;
  }

  return CGraphNode::operator==(rhs);
}

CValueComponent& CValueComponent::operator=(const CValueComponent& rhs)
{
  bool bModified = !operator==(rhs);

  CGraphNode::operator=(rhs);
  m_pValueSet = rhs.m_pValueSet;
  assert(rhs.IsCopy());

  if(bModified)
    Modified();

  return *this;
}

bool CValueComponent::Defined() const
{
  return m_pValueSet != 0;
}

IValueDomainScalar::TMinMax CValueComponent::MinMax(IProgressBase& /*progressBase*/, const CQuantity::UNIT unit) const
{
  assert(Defined());
  IValueDomainScalar::TMinMax ret;
  // Get the point set and walk over the points
  const CValueType *pType = dynamic_cast<const CValueType*>(&Parent());
  assert(pType);

  const IElementSet *pElSet = dynamic_cast<const IElementSet *> (&pType->PointSet());

  if(pElSet)
  {
    for(int i = 0; i < pElSet->ElementSet().ElementSize(); i++)
    {
      const geo::IElement &el = pElSet->ElementSet().Element(i);
      IValueDomainScalar::TValueVec vcVal = ValueElement(el);
      for(int j = 0; j < el.NrOfPoints(); j++)
      {
        ret.first = ret.first.Min(vcVal[j]);
        ret.second = ret.second.Max(vcVal[j]);
      }
    }
  }
  else
  {
    for(int i = 0; i < pType->PointSet().PointSize(); i++)
    {
      geo::CValue value = ValuePoint(pType->PointSet().PointAt(i), unit);
      ret.first = ret.first.Min(value);
      ret.second = ret.second.Max(value);
    }
  }

  return ret;
}

IValueDomainScalar::TValue CValueComponent::Average(IProgressBase& /*progressBase*/, const CQuantity::UNIT unit) const
{
  assert(Defined());
  IValueDomainScalar::TValue ret;
  // Get the point set and walk over the points
  const CValueType *pType = dynamic_cast<const CValueType*>(&Parent());
  assert(pType);

  const IElementSet *pElSet = dynamic_cast<const IElementSet *> (&pType->PointSet());

  if(pElSet)
  {
  if(pElSet->ElementSet().ElementSize() > 0)
  {
      double dSum = 0;
      int nValues = 0;
      for(int i = 0; i < pElSet->ElementSet().ElementSize(); i++)
      {
        const geo::IElement &el = pElSet->ElementSet().Element(i);
        IValueDomainScalar::TValueVec vcVal = ValueElement(el);
        for(int j = 0; j < el.NrOfPoints(); j++)
        {
          ++nValues;
          if(vcVal[j].Valid())
      dSum += vcVal[j].Value();
          else
      return IValueDomainScalar::TValue();
        }
      }
      ret = dSum / nValues;
  }
  }
  else if(pType->PointSet().PointSize() > 0)
  {
  double dSum = 0;
    for(int i = 0; i < pType->PointSet().PointSize(); i++)
    {
      geo::CValue value = ValuePoint(pType->PointSet().PointAt(i), unit);
      if(value.Valid())
    dSum += value.Value();
      else
    return IValueDomainScalar::TValue();
    }

  ret = dSum / pType->PointSet().PointSize();
  }

  return ret;
}

IValueDomainScalar::TValue CValueComponent::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  assert(Defined());		// We can only access value types
  // Try to obtain the value of the value set

  const CValueType *pType = dynamic_cast<const CValueType*>(&Parent());
  assert(pType);

  return InterpolateValuePoint(pt, unit, cb);
}

IValueDomainScalar::TValue CValueComponent::InterpolateValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const CValueType *pType = dynamic_cast<const CValueType*>(&Parent());
  assert(pType);
  geo::CValue value;

  value = ValueSet().ValuePoint(pt, cb);

  if(value.Valid())
  {
  // don't convert rpn value sets
  if(dynamic_cast<CRpnValueSet*>(m_pValueSet) != 0)
  {
      if(unit == CQuantity::FIELD_UNIT)
    value *= FieldFactor();
  }
  else
  {
  		// We need the point set to get the unit
      if (pType->PointSet().ValueUnit() == CQuantity::FIELD_UNIT)
    value *= FieldFactor();
      if (unit == CQuantity::FIELD_UNIT)
    value *= FieldFactor();
  }

    return value;	// Just return the value
  }

  return geo::CValue();
}

IValueDomainScalar::TValueVec CValueComponent::ValueElement(const geo::IElement &elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet(elm.NrOfPoints());
  const CValueType &value_type = dynamic_cast<const CValueType&>(Parent());

  const IElementSet *pElSet = dynamic_cast<const IElementSet*>(&value_type.PointSet());

  if(pElSet == 0)
  {
    // We dealing with a pointset which supplies only nodal values
    for(int i = 0; i < elm.NrOfPoints(); i++)
      vcRet[i] = InterpolateValuePoint(elm.Point(i), unit, cb);
  }
  else if((value_type.MapType() == CValueType::MT_POINT) || (value_type.PointSet().Dimension() == IPointSet::DIM_1D)) 
  {
    if(!MapValueSet(unit).GetSmoothedNodalValues(elm, vcRet, cb) || (value_type.PointSet().Dimension() == IPointSet::DIM_1D))
    {
      for(int i = 0; i < elm.NrOfPoints(); i++)
        vcRet[i] = InterpolateValuePoint(elm.Point(i), unit, cb);
    }
  }
  else
  {
    // When element mapping is needed we do it here and the value set are supplied with "normal elements"
    // by using the value mapper

    // when a body or a bodyface is given we can use the value-mapper,
    // otherwise we use scoremap
    const geo::IBody *pBody = dynamic_cast<const geo::IBody *> (&elm);

    const geo::CBodyTriangle *pBodyTriangle = dynamic_cast<const geo::CBodyTriangle *> (&elm);
    if(pBodyTriangle) pBody = pBodyTriangle->Parent();

    const geo::CBodyQuadrilateral *pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral *> (&elm);
    if(pBodyQuad) pBody = pBodyQuad->Parent();

    if(pBody)
    {
      IValueDomainScalar::TValueVec vcInterm(pBody->NrOfPoints());
      bool bProcessed = false;
      switch(value_type.MapType())
      {
      case CValueType::MT_NONE:
        bProcessed = MapValueSet(unit).MapValues(*pBody, vcInterm, geo::WT_NONE, cb);
        break;
      case CValueType::MT_VOIGHT:
        bProcessed = MapValueSet(unit).MapValues(*pBody, vcInterm, geo::WT_VOIGHT, cb);
        break;
      case CValueType::MT_REUSS:
        bProcessed = MapValueSet(unit).MapValues(*pBody, vcInterm, geo::WT_REUSS, cb);
        break;
      default:
        assert(false);	// Something gone very wrong
      }

      if(!bProcessed)
      {
        for(int i = 0; i < pBody->NrOfPoints(); i++)
          vcInterm[i] = InterpolateValuePoint(pBody->Point(i), unit, cb);
      }

      if(pBodyTriangle)
      {
        std::vector<int> vcFaceNodeIndices = pBody->FaceNodeIndices(pBodyTriangle->Index());
        for(int i = 0; i < pBodyTriangle->NrOfPoints(); i++)
          vcRet[i] = vcInterm[vcFaceNodeIndices[i]];
      }
      else if(pBodyQuad)
      {
        std::vector<int> vcFaceNodeIndices = pBody->FaceNodeIndices(pBodyQuad->Index());
        for(int i = 0; i < pBodyQuad->NrOfPoints(); i++)
          vcRet[i] = vcInterm[vcFaceNodeIndices[i]];
      }
      else
      {
        return vcInterm;
      }
    }
    else
    {
      MapValueSet(unit).ElementValue(elm, vcRet, cb);
    }
  }

  const CMeshBase& mesh = ((const CModelBase&)Model()).Mesh();

  if(value_type.SingleElementValue() && &mesh.Mesh() == elm.IndexingElementSet() && !vcRet.empty())
  {
    double dValue = 0;
    int sz = 0;
  size_t i;
    for(i = 0; i < vcRet.size(); ++i)
    {
      if(vcRet[i].Valid())
      {
        dValue += vcRet[i].Value();
        ++sz;
      }
    }

    if(sz > 0)
    {
      dValue /= sz;
      for(i = 0; i < vcRet.size(); ++i)
      {
        if(vcRet[i].Valid())
          vcRet[i] = dValue;
      }
    }
  }

  return vcRet;
}

const IValueSet& CValueComponent::ValueSet() const
{
  assert(m_pValueSet);
  assert(!m_pValueSet->IsCopy());
  if(IsCopy() && m_pValueSet->IsCopied())
    return (const IValueSet&) m_pValueSet->Copy();

  return *m_pValueSet;
}

IValueSet& CValueComponent::ValueSet() 
{
  assert(m_pValueSet);
  assert(!m_pValueSet->IsCopy());
  if(IsCopy() && m_pValueSet->IsCopied())
    return (IValueSet&) m_pValueSet->Copy();

  return *m_pValueSet;
}

void CValueComponent::OnNewNeighbour(const CGraphNode &node)
{
  const IValueSet *pValue = dynamic_cast<const IValueSet*>(&node);
  if(pValue)
  {
    // UnLink current value set if any ...
    if(Defined())
      UnLink(ValueSet());

    m_pValueSet = const_cast<IValueSet*> (pValue);
  }

  IValueComponentBase::OnNewNeighbour(node);
}

void CValueComponent::OnNeighbourDeleted(const CGraphNode &node)
{
  // Is value set decoupled ...
  if(&node == m_pValueSet)
    m_pValueSet = 0;

  // Unlink from point set result in self destruction ...
  CValueType *pType = (CValueType*) &Parent();
  if((CGraphNode*)(&node) == (CGraphNode*)(&pType->PointSet()))
  {
    IValueComponentBase::OnNeighbourDeleted(node);
    delete this;
    return;
  }

  IValueComponentBase::OnNeighbourDeleted(node);
}

unsigned int CValueComponent::IconId() const
{
  if(m_pValueSet)
    return m_pValueSet->IconId();

  return IDI_UNDEFINED;
}

const QString& CValueComponent::Name() const
{
  if(!Defined())
    return IValueComponentBase::Name();

  return ValueSet().Name();
}

unsigned int CValueComponent::TypeId() const
{
  assert(IsCopy());
  return Source().TypeId();
}

QString CValueComponent::TypeName() const
{
  assert(IsCopy());
  return Source().TypeName();
}

double CValueComponent::FieldFactor() const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->FieldFactor();
}

QString CValueComponent::UnitName(const CQuantity::UNIT unit) const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->UnitName(unit);
}

bool CValueComponent::Empty() const
{
  assert(false);
  return false;
}

long CValueComponent::SavedItems() const
{
  assert(false);
  return 0;
}

QString CValueComponent::ExportLabel() const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->ExportLabel();
}

bool CValueComponent::IsInRange(const IValueSet& value_set) const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->IsInRange(value_set);
}

geo::CValue CValueComponent::RangeMin(const CQuantity::UNIT unit) const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->RangeMin(unit);
}

geo::CValue CValueComponent::RangeMax(const CQuantity::UNIT unit) const
{
  assert(IsCopy());
  const CValueComponent *pComponent = (const CValueComponent*)(&Source());
  return pComponent->RangeMax(unit);
}


bool CValueComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  bool retval = pElementSet != 0;

  const CValueType &value_type = dynamic_cast<const CValueType&>(Parent());
  const IElementSet *pElSet = dynamic_cast<const IElementSet*>(&value_type.PointSet());

  if (pElSet != 0)
  {
  if (!const_cast<IElementSet *>(pElSet)->PrepareMapping(pElementSet))
      retval = false;
  }

  if (!ValueSet().PrepareMapping(pElementSet))
  retval = false;

  if (pElementSet)
  {
  for (int i = 0; i < pElementSet->PointSize(); ++i)
  {
      value_type.PointSet().PointAt(pElementSet->Point(i));
  }
  }

  return retval;
}