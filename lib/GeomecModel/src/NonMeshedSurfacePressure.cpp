#include "NonMeshedSurfacePressure.h"

#include "NonMeshedSurface.h"
#include "SinglePressure.h"
#include "resourceIDS.h"
#include "ValueTypeFactory.h"
#include "FieldFactor.h"
#include "ModelBase.h"
#include "FormationBase.h"
#include "Pressure.h"
#include "ivaluecomponent.h"

CNonMeshedSurfacePressureComponent::CNonMeshedSurfacePressureComponent(const QString& sName, CNonMeshedSurfacePressure& pressure)
: IValueComponentBase(sName, pressure),
  m_Data(*this),
  m_pressure(pressure)
{
}

CNonMeshedSurfacePressureComponent::CNonMeshedSurfacePressureComponent(const CNonMeshedSurfacePressureComponent& rhs)
: IValueComponentBase(rhs),
  m_Data(*this),
  m_pressure(rhs.m_pressure)
{
}

CNonMeshedSurfacePressureComponent::~CNonMeshedSurfacePressureComponent()
{
}

IValueComponentBase::TValueComponentType CNonMeshedSurfacePressureComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CNonMeshedSurfacePressureComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CNonMeshedSurfacePressureComponent::ScalarData()
{
  return m_Data;
}

const CNonMeshedSurfacePressure& CNonMeshedSurfacePressureComponent::Pressure() const
{
  return m_pressure;
}

CNonMeshedSurfacePressure& CNonMeshedSurfacePressureComponent::Pressure()
{
  return m_pressure;
}

QString CNonMeshedSurfacePressureComponent::UnitName(const CQuantity::UNIT unit) const
{
  CSinglePressure p;
  return p.UnitName(unit).c_str();
}

QString CNonMeshedSurfacePressureComponent::ExportLabel() const
{
  return getStringTableEntry(IDS_ET_PRESSURE);
}

bool CNonMeshedSurfacePressureComponent::Empty() const
{
  return false;
}

long CNonMeshedSurfacePressureComponent::SavedItems() const
{//TODO
  return 0;
}

geo::CValue CNonMeshedSurfacePressureComponent::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue val;
  int nValues = 0;

  for(size_t i = 0; i < m_pressure.DistributedSize(); ++i)
  {
  if(m_pressure.DistributedOnly() || m_pressure.DistributedValue(i).PointSet().PointInConvexHull(pt))
  {
      geo::CValue valpt = m_pressure.DistributedValue(i).Component().ScalarData().ValuePoint(pt, unit, cb);
      if(valpt.Valid())
      {
    if(val.Valid())
          val += valpt.Value();
    else
          val = valpt.Value();
    ++nValues;
      }
  }
  }

  if(val.Valid())
  {
  assert(nValues > 0);
  val /= nValues;
  }

  if(!val.Valid() && m_pressure.ManualValues())
  {
  val = (pt.Z() - m_pressure.ReferenceDepth()) * m_pressure.Gradient() + m_pressure.ReferencePressure();
  if(unit == CQuantity::FIELD_UNIT)
      val *= FF_FACTOR_PRESSURE;
  }

  return val;
}

IValueDomainScalar::TValueVec CNonMeshedSurfacePressureComponent::ValueElement(const geo::IElement& elm, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec values(elm.NrOfPoints());

  std::vector<int> vcNumValues(elm.NrOfPoints(), 0);

  for(size_t i = 0; i < m_pressure.DistributedSize(); ++i)
  {
  IValueDomainScalar::TValueVec vcPressureValues = m_pressure.DistributedValue(i).Component().ScalarData().ValueElement(elm, unit, cb);
  for(int j = 0; j < elm.NrOfPoints(); ++j)
  {
      if(m_pressure.DistributedOnly() || m_pressure.DistributedValue(i).PointSet().PointInConvexHull(elm.Point(j)))
      {
    if(vcPressureValues[j].Valid())
    {
          if(values[j].Valid())
      values[j] += vcPressureValues[j];
          else
      values[j] = vcPressureValues[j];
          ++vcNumValues[j];
    }
      }
  }
  }

  for(int i = 0; i < elm.NrOfPoints(); ++i)
  {
  if(values[i].Valid())
  {
      assert(vcNumValues[i] > 0);
      values[i] /= vcNumValues[i];
  }
  }

  for(int i = 0; i < elm.NrOfPoints(); ++i)
  {
  if(!values[i].Valid())
  {
      if(m_pressure.ManualValues())
      {
    values[i] = (elm.Point(i).Z() - m_pressure.ReferenceDepth()) * m_pressure.Gradient() + m_pressure.ReferencePressure();
    if(unit == CQuantity::FIELD_UNIT)
          values[i] *= FF_FACTOR_PRESSURE;
      }
      else
      {
      }
  }
  }

  return values;
}

bool CNonMeshedSurfacePressureComponent::CanMap(const COpenGLNode& node) const
{
  return (&node == &m_pressure.Surface());
}

unsigned int CNonMeshedSurfacePressureComponent::IconId() const
{
  return 0;
}

unsigned int CNonMeshedSurfacePressureComponent::TypeId() const
{
  return 0;
}

bool CNonMeshedSurfacePressureComponent::Defined() const
{
  //return m_pressure.ManualValues();
  return true;
}

IValueDomainScalar::TMinMax CNonMeshedSurfacePressureComponent::MinMax(IProgressBase& progressBase, const UNIT unit) const
{
  geo::CBox box(m_pressure.Surface().Min(), m_pressure.Surface().Max());
  return ScalarData().MinMax(progressBase, box, unit);
}

IValueDomainScalar::TValue CNonMeshedSurfacePressureComponent::Average(IProgressBase& progressBase, const UNIT unit) const
{
  geo::CBox box(m_pressure.Surface().Min(), m_pressure.Surface().Max());
  return ScalarData().Average(progressBase, box, unit);
}


/////

CNonMeshedSurfacePressure::CNonMeshedSurfacePressure(CNonMeshedSurface& surface)
: IValueComposite("Pressure", surface.Model()),
  m_surface(surface),
  m_bDistributedOnly(false),
  m_bManual(false),
  m_dRefPressure(0),
  m_dRefDepth(0),
  m_dGradient(0)
{
  new CNonMeshedSurfacePressureComponent("Pressure", *this);
  reParent(&surface);
}

CNonMeshedSurfacePressure::CNonMeshedSurfacePressure(const CNonMeshedSurfacePressure& rhs)
: IValueComposite(rhs),
  m_surface(rhs.m_surface),
  m_vcDistributed(rhs.m_vcDistributed),
  m_bDistributedOnly(rhs.m_bDistributedOnly),
  m_bManual(rhs.m_bManual),
  m_dRefPressure(rhs.m_dRefPressure),
  m_dRefDepth(rhs.m_dRefDepth),
  m_dGradient(rhs.m_dGradient)
{
}

CNonMeshedSurfacePressure& CNonMeshedSurfacePressure::operator=(const CNonMeshedSurfacePressure& rhs)
{
  m_vcDistributed = rhs.m_vcDistributed;
  m_bDistributedOnly = rhs.m_bDistributedOnly;
  m_bManual = rhs.m_bManual;
  m_dRefPressure = rhs.m_dRefPressure;
  m_dRefDepth = rhs.m_dRefDepth;
  m_dGradient = rhs.m_dGradient;

//  Modified();
  Component().Modified();

  return *this;
}

bool CNonMeshedSurfacePressure::operator==(const CNonMeshedSurfacePressure& rhs) const
{
  return (
  m_vcDistributed == rhs.m_vcDistributed &&
  m_bDistributedOnly == rhs.m_bDistributedOnly &&
  m_bManual == rhs.m_bManual &&
  m_dRefPressure == rhs.m_dRefPressure &&
  m_dRefDepth == rhs.m_dRefDepth &&
  m_dGradient == rhs.m_dGradient);
}

size_t CNonMeshedSurfacePressure::DistributedSize() const
{
  return m_vcDistributed.size();
}

const TPressure& CNonMeshedSurfacePressure::DistributedValue(size_t nIndex) const
{
  return *m_vcDistributed[nIndex];
}

bool CNonMeshedSurfacePressure::DistributedOnly() const
{
  return m_bDistributedOnly;
}

void CNonMeshedSurfacePressure::DistributedOnly(bool bDistributedOnly)
{
  m_bDistributedOnly = bDistributedOnly;
}

bool CNonMeshedSurfacePressure::ManualValues() const
{
  return m_bManual;
}

void CNonMeshedSurfacePressure::ManualValues(bool bManual)
{
  m_bManual = bManual;
}

double CNonMeshedSurfacePressure::ReferencePressure() const
{
  return m_dRefPressure;
}

double CNonMeshedSurfacePressure::ReferenceDepth() const
{
  return m_dRefDepth;
}

double CNonMeshedSurfacePressure::Gradient() const
{
  return m_dGradient;
}

void CNonMeshedSurfacePressure::ReferencePressure(double dPressure)
{
  m_dRefPressure = dPressure;
}

void CNonMeshedSurfacePressure::ReferenceDepth(double dDepth)
{
  m_dRefDepth = dDepth;
}

void CNonMeshedSurfacePressure::Gradient(double dGradient)
{
  m_dGradient = dGradient;
}

void CNonMeshedSurfacePressure::OnNewNeighbour(const CGraphNode &node)
{
  const TPressure* pPressure = dynamic_cast<const TPressure*>(&node);
  if(pPressure)
  m_vcDistributed.push_back(const_cast<TPressure*>(pPressure));

  IValueComposite::OnNewNeighbour(node);
}

void CNonMeshedSurfacePressure::OnNeighbourDeleted(const CGraphNode &node)
{
  for(int i = 0; i < m_vcDistributed.size(); ++i)
  {
  if(&node == m_vcDistributed[i])
  {
      m_vcDistributed.erase(m_vcDistributed.begin() + i);
      break;
  }
  }

  IValueComposite::OnNeighbourDeleted(node);
}

bool CNonMeshedSurfacePressure::CanConnectItem(const CGraphNode &item) const
{
  // allow only once
  for(int i = 0; i < m_vcDistributed.size(); ++i)
  {
  if(&item == m_vcDistributed[i])
      return false;
  }

  return (dynamic_cast<const TPressure*>(&item) != 0);
}

long CNonMeshedSurfacePressure::SavedItems() const
{
  return 0;
}

void CNonMeshedSurfacePressure::LoadStream(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& /*progress*/)
{
  TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*(static_cast<CModelBase&>(Model())).RootModel().GraphEntry(MD_BASE_VALUE_COMPOSITE);

  int val;
  stream >> val;
  for(int i = 0; i < val; ++i)
  {
  int idx;
  stream >> idx;
  composite_entry.LinkNodeToIndex(*this, idx);
  }

  stream >> val;
  m_bDistributedOnly = (val != 0);
  stream >> val;
  m_bManual = (val != 0);
  stream >> m_dRefPressure;
  stream >> m_dRefDepth;
  stream >> m_dGradient;
}

void CNonMeshedSurfacePressure::SaveStream(TSTREAM& stream, TPROGRESS& /*progress*/)
{
  stream << int(m_vcDistributed.size());
  for(int i = 0; i < m_vcDistributed.size(); ++i)
  stream << m_vcDistributed[i]->Index();

  stream << (m_bDistributedOnly ? 1 : 0);
  stream << (m_bManual ? 1 : 0);
  stream << m_dRefPressure;
  stream << m_dRefDepth;
  stream << m_dGradient;
}

const CNonMeshedSurface& CNonMeshedSurfacePressure::Surface() const
{
  return m_surface;
}

CNonMeshedSurface& CNonMeshedSurfacePressure::Surface()
{
  return m_surface;
}

QString CNonMeshedSurfacePressure::ExportLabel(int /*nComponent*/) const
{//TOTO
  return QString();
}

bool CNonMeshedSurfacePressure::CanMap(const COpenGLNode& node, int /*nRegister*/) const
{
  return (&node == &m_surface);
}

unsigned int CNonMeshedSurfacePressure::IconId() const
{
  return IDI_PRESSURES;
}

unsigned int CNonMeshedSurfacePressure::TypeId() const
{
  return 0;
}
