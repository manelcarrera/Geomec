#include "NonMeshedSurfaceParametersNode.h"

#include "DepletionStage.h"
#include "NonMeshedSurface.h"
#include "ModelBase.h"
#include "ValueType.h"
#include "ValueTypes.h"
#include "IPointSet.h"
#include "ValueComponent.h"
#include "FemAppEntryTypes.h"
#include "resourceIDI.h"

CNonMeshedSurfaceParametersNode::CNonMeshedSurfaceParametersNode(CNonMeshedSurface& surface)
: CStorageNode("Parameters", surface.Model()),
  m_surface(surface),
  m_bCohesionExtrapolated(true),
  m_bFrictionExtrapolated(true),
  m_Cohesion(10),
  m_FrictionAngle(15),
  m_pValueTypeCohesion(0),
  m_pValueTypeFriction(0)
{
  reParent(&surface);
}

CNonMeshedSurfaceParametersNode::CNonMeshedSurfaceParametersNode(const CNonMeshedSurfaceParametersNode& rhs)
: CStorageNode(rhs),
  m_surface(rhs.m_surface),
  m_bCohesionExtrapolated(rhs.m_bCohesionExtrapolated),
  m_bFrictionExtrapolated(rhs.m_bFrictionExtrapolated),
	m_Cohesion(rhs.m_Cohesion),
	m_FrictionAngle(rhs.m_FrictionAngle),
  m_pValueTypeCohesion(rhs.m_pValueTypeCohesion),
  m_pValueTypeFriction(rhs.m_pValueTypeFriction)
{
}

CNonMeshedSurfaceParametersNode& CNonMeshedSurfaceParametersNode::operator=(const CNonMeshedSurfaceParametersNode& rhs)
{
  assert(!IsCopy());
  assert(rhs.IsCopy());

  CStorageNode::operator=(rhs);

  assert(&m_surface == &rhs.m_surface);

  m_bCohesionExtrapolated   = rhs.m_bCohesionExtrapolated;
  m_bFrictionExtrapolated   = rhs.m_bFrictionExtrapolated;
  m_Cohesion                = rhs.m_Cohesion;
  m_FrictionAngle           = rhs.m_FrictionAngle;
  m_pValueTypeCohesion      = rhs.m_pValueTypeCohesion;
  m_pValueTypeFriction      = rhs.m_pValueTypeFriction;

  return *this;
}

bool CNonMeshedSurfaceParametersNode::operator==(const CNonMeshedSurfaceParametersNode& rhs) const
{
  return (
    &m_surface                == &rhs.m_surface                &&
    m_bCohesionExtrapolated   == rhs.m_bCohesionExtrapolated   &&
    m_bFrictionExtrapolated   == rhs.m_bFrictionExtrapolated   &&
    m_Cohesion                == rhs.m_Cohesion                &&
    m_FrictionAngle           == rhs.m_FrictionAngle           &&
    m_pValueTypeCohesion      == rhs.m_pValueTypeCohesion      &&
    m_pValueTypeFriction      == rhs.m_pValueTypeFriction);
}

void CNonMeshedSurfaceParametersNode::OnNewNeighbour(const CGraphNode& node)
{
	const CValueType* pValueType = dynamic_cast<const CValueType*>(&node);
	if(pValueType)
	{
		assert((pValueType->TypeId() == IDT_VALUETYPE_COHESION)         ||
           (pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE));
		
		if(pValueType->TypeId() == IDT_VALUETYPE_COHESION)
		{
			assert(m_pValueTypeCohesion == 0);
			m_pValueTypeCohesion = pValueType;
		}
		else if(pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE)
		{
			assert(m_pValueTypeFriction == 0);
			m_pValueTypeFriction = pValueType;
		}

		Modified();
	}

  CStorageNode::OnNewNeighbour(node);
}

void CNonMeshedSurfaceParametersNode::OnNeighbourDeleted(const CGraphNode& node)
{
	if(m_pValueTypeCohesion == &node)
	{
		m_pValueTypeCohesion = 0;
		Modified();
	}
	else if(m_pValueTypeFriction == &node)
	{
		m_pValueTypeFriction = 0;
		Modified();
	}

  CStorageNode::OnNeighbourDeleted(node);
}

void CNonMeshedSurfaceParametersNode::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
	if(m_pValueTypeCohesion == &node || m_pValueTypeFriction == &node)
		Modified();
}

bool CNonMeshedSurfaceParametersNode::CanConnectItem(const CGraphNode& item) const
{
	if(IsLinkedTo(item))
		return false;

	const CValueType* pValueType = dynamic_cast<const CValueType*>(&item);
	if(pValueType)
	{
		return ((pValueType->TypeId() == IDT_VALUETYPE_COHESION       && !m_pValueTypeCohesion) || 
            (pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE && !m_pValueTypeFriction));
	}

	return CStorageNode::CanConnectItem(item);
}

unsigned int CNonMeshedSurfaceParametersNode::IconId() const
{
  return IDI_NONMESHEDSURFACEPARAMETERS;
}

unsigned int CNonMeshedSurfaceParametersNode::TypeId() const
{
  return IDT_VALUETYPE_NONMESHEDSURFACEPARAMETERS;
}

bool CNonMeshedSurfaceParametersNode::Empty() const
{
  return false;
}

long CNonMeshedSurfaceParametersNode::SavedItems() const
{
  return 1;
}

void CNonMeshedSurfaceParametersNode::LoadStream(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& progress)
{
  int n;

  stream >> n;
  m_bCohesionExtrapolated = (n != 0);

  stream >> n;
  m_bFrictionExtrapolated = (n != 0);

  double dCohesion;
  stream >> dCohesion;
  m_Cohesion = dCohesion;

  double dFriction;
  stream >> dFriction;
  m_FrictionAngle = dFriction;

  int i;
  for(i = 0; i < 2; ++i)
  {
    TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);
    stream >> n;
    if(n >= 0)
    {
      composite_entry.LinkNodeToIndex(*this, n);
    }
  }

  progress.Step();
}

void CNonMeshedSurfaceParametersNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  int n;

  n = m_bCohesionExtrapolated ? 1 : 0;
  stream << n;

  n = m_bFrictionExtrapolated ? 1 : 0;
  stream << n;

  double dCohesion = m_Cohesion.Value();
  stream << dCohesion;

  double dFriction = m_FrictionAngle.Value();
  stream << dFriction;

  if(m_pValueTypeCohesion)
    stream << m_pValueTypeCohesion->Index();
  else
    stream << -1;

  if(m_pValueTypeFriction)
    stream << m_pValueTypeFriction->Index();
  else
    stream << -1;

  progress.Step();
}

const CCohesionQuantity& CNonMeshedSurfaceParametersNode::Cohesion() const
{
  return m_Cohesion;
}

const CAngleQuantity& CNonMeshedSurfaceParametersNode::FrictionAngle() const
{
  return m_FrictionAngle;
}

CCohesionQuantity& CNonMeshedSurfaceParametersNode::Cohesion()
{
  return m_Cohesion;
}

CAngleQuantity& CNonMeshedSurfaceParametersNode::FrictionAngle()
{
  return m_FrictionAngle;
}

bool CNonMeshedSurfaceParametersNode::DistributedCohesion() const
{
  return (m_pValueTypeCohesion != 0);
}

bool CNonMeshedSurfaceParametersNode::DistributedFriction() const
{
  return (m_pValueTypeFriction != 0);
}

bool CNonMeshedSurfaceParametersNode::CohesionExtrapolated() const
{
  return m_bCohesionExtrapolated;
}

bool CNonMeshedSurfaceParametersNode::FrictionExtrapolated() const
{
  return m_bFrictionExtrapolated;
}

void CNonMeshedSurfaceParametersNode::CohesionExtrapolated(bool b)
{
  m_bCohesionExtrapolated = b;
}

void CNonMeshedSurfaceParametersNode::FrictionExtrapolated(bool b)
{
  m_bFrictionExtrapolated = b;
}

double CNonMeshedSurfaceParametersNode::Cohesion(const geo::IPoint& point, const CQuantity::UNIT unit) const
{
	if(m_pValueTypeCohesion)
	{
		if(m_bCohesionExtrapolated || m_pValueTypeCohesion->PointSet().PointInConvexHull(point))
		{
			geo::CValue value = m_pValueTypeCohesion->Component().ScalarData().ValuePoint(point, unit);
			if(value.Valid())
        return value.Value();
		}
	}

	return Cohesion().Value(unit);
}

std::vector<double> CNonMeshedSurfaceParametersNode::Cohesion(const geo::IElement& element, const CQuantity::UNIT unit) const
{
	std::vector<double> vcResult(element.NrOfPoints());

	if(m_pValueTypeCohesion)
	{
		if(m_bCohesionExtrapolated || m_pValueTypeCohesion->PointSet().ElementInConvexHull(element))
		{
			IValueDomainScalar::TValueVec vcValue = m_pValueTypeCohesion->Component().ScalarData().ValueElement(element, unit);
			assert(vcValue.size() == vcResult.size());
			for(size_t j = 0; j < vcResult.size(); ++j)
			{
				if(vcValue[j].Valid())
					vcResult[j] = vcValue[j].Value();
				else
					vcResult[j] = Cohesion().Value(unit);
			}

			return vcResult;
		}
	}

	for(size_t i = 0; i < vcResult.size(); ++i)
    vcResult[i] = Cohesion().Value(unit);

	return vcResult;
}

double CNonMeshedSurfaceParametersNode::FrictionAngle(const geo::IPoint& point, const CQuantity::UNIT unit) const
{
	if(m_pValueTypeFriction)
	{
		if(m_bFrictionExtrapolated || m_pValueTypeFriction->PointSet().PointInConvexHull(point))
		{
			geo::CValue value = m_pValueTypeFriction->Component().ScalarData().ValuePoint(point, unit);
			if(value.Valid())
        return value.Value();
		}
	}

	return FrictionAngle().Value(unit);
}

std::vector<double> CNonMeshedSurfaceParametersNode::FrictionAngle(const geo::IElement& element, const CQuantity::UNIT unit) const
{
	std::vector<double> vcResult(element.NrOfPoints());

	if(m_pValueTypeFriction)
	{
		if(m_bFrictionExtrapolated || m_pValueTypeFriction->PointSet().ElementInConvexHull(element))
		{
			IValueDomainScalar::TValueVec vcValue = m_pValueTypeFriction->Component().ScalarData().ValueElement(element, unit);
			assert(vcValue.size() == vcResult.size());
			for(size_t j = 0; j < vcResult.size(); j++)
			{
				if(vcValue[j].Valid())
					vcResult[j] = vcValue[j].Value();
				else
					vcResult[j] = FrictionAngle().Value(unit);
			}

			return vcResult;
		}
	}

	for(size_t i = 0; i < vcResult.size(); i++)
    vcResult[i] = FrictionAngle().Value(unit);

	return vcResult;
}

const CNonMeshedSurface& CNonMeshedSurfaceParametersNode::Surface() const
{
  return m_surface;
}
