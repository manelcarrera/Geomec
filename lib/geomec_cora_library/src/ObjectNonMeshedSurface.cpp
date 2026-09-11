#include "ObjectNonMeshedSurface.h"
#include "NonMeshedSurface.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "ValueTypes.h"
#include "resourceIDS.h"
#include "GetValueTypeInfo.h"
#include "ObjectParametersNodeValue.h"
#include "GeoSurface.h"
#include "Parameter.h"
#include "PointSet.h"

namespace cora
{

CObjectNonMeshedSurface::CObjectNonMeshedSurface(CModelBase* modelBase,
  CNonMeshedSurface* nonMeshedSurface)
: CObjectBase()
, m_nonMeshedSurface(nonMeshedSurface)
, m_parameters(getParameters(modelBase, nonMeshedSurface))
, m_owner(false)
, m_sequenceState(0)
{
}

CObjectNonMeshedSurface::~CObjectNonMeshedSurface()
{
}

const QString& CObjectNonMeshedSurface::name() const
{
  return m_nonMeshedSurface->Name();
}

const TParameters& CObjectNonMeshedSurface::getParameters() const
{
  return m_parameters;
}

namespace
{

// TODO SURFACE-HORIZON
// replace "surface:" with "non-meshed surface:"

const QString PREFIX = "surface:";

} // anonymous namespace

const QString& CObjectNonMeshedSurface::prefix() const
{
  return PREFIX;
}

const COpenGLNode* CObjectNonMeshedSurface::object() const
{
  return m_nonMeshedSurface;
}

namespace
{

const QString TYPE_NAME = "Surface";

} // anonymous namespace

const QString& CObjectNonMeshedSurface::typeName() const
{
  return TYPE_NAME;
}

const geo::IElement* CObjectNonMeshedSurface::getFirstElement()
{
  m_sequenceState = 0;

  return getNextElement();
}

const geo::IElement* CObjectNonMeshedSurface::getNextElement()
{
  if (m_sequenceState < m_nonMeshedSurface->FaceSize())
  {
    return &(m_nonMeshedSurface->Face(m_sequenceState++));
  }

  return 0;
}

std::ostream& CObjectNonMeshedSurface::operator () (std::ostream& os) const
{
  os << PREFIX.toStdString() << m_nonMeshedSurface->Name().toStdString() <<
    std::endl;
  os << m_parameters.size() << std::endl;

  for (size_t s = 0; s < m_parameters.size(); ++s)
  {
    os << m_parameters[s];
  }

  return os;
}

// private

// static

TParameters CObjectNonMeshedSurface::getParameters(CModelBase* modelBase,
  CNonMeshedSurface* nonMeshedSurface)
{
  const CDepletionStageEntry* depletionStages =
    dynamic_cast <const CDepletionStageEntry*> (
      modelBase->GraphEntry(MD_BASE_DEPLETION_STAGE));
  TParameters nonMeshedParameters;

  for (CDepletionStageEntry::const_iterator
    depletionStage = depletionStages->begin();
    depletionStage != depletionStages->LastStage(); ++depletionStage)
  {
    if ((*depletionStage).IsPhaseStartStage())
    {
      getParameters(nonMeshedParameters, modelBase, *depletionStage,
        nonMeshedSurface);
    }
  }

  return nonMeshedParameters;
}

void CObjectNonMeshedSurface::getParameters(TParameters& nonMeshedParameters,
  CModelBase* modelBase, const CDepletionStage& depletionStage,
  CNonMeshedSurface* nonMeshedSurface)
{
  // See CNonMeshedSurfaceEntry::CanConnectItem(...)!

  if (nonMeshedSurface != 0)
  {
    getParameters(nonMeshedParameters, modelBase, depletionStage,
      nonMeshedSurface, IDT_VALUETYPE_COHESION, IDS_VALUENAME_COHESION);
    getParameters(nonMeshedParameters, modelBase, depletionStage,
      nonMeshedSurface, IDT_VALUETYPE_FRICTION_ANGLE,
      IDS_VALUENAME_FRICTION_ANGLE);
  }
}

void CObjectNonMeshedSurface::getParameters(TParameters& nonMeshedParameters,
  CModelBase* modelBase, const CDepletionStage& depletionStage,
  CNonMeshedSurface* nonMeshedSurface, unsigned int valueTypeID,
  unsigned int /*valueTypeName*/)
{
  const CNonMeshedSurfaceParametersNode& nonMeshedSurfaceParametersNode =
    nonMeshedSurface->ParametersNode();
  CGetValueTypeInfo& getValueTypeInfo(CGetValueTypeInfo::instance(modelBase));
  std::set <CSurfaceBase*> surfaces = nonMeshedSurface->Links <CSurfaceBase> ();

  assert(surfaces.size() == 1);

  CSurfaceBase* surfaceBase = *(surfaces.begin());
  CObjectParametersNodeValue <geo::IElementSet, CNonMeshedSurfaceParametersNode>
    nonMeshedSurfaceParametersNodeValue(
    &(surfaceBase->ElementSet()),
      valueTypeID, nonMeshedSurfaceParametersNode);
  std::pair <geo::CValue, geo::CValue> range =
    nonMeshedSurfaceParametersNodeValue.getRange();
  double mean = nonMeshedSurfaceParametersNodeValue.getMean();

  nonMeshedParameters.push_back(TParameter(new CParameter(
    CParameter::nonMeshedSurfaceParameter, QString("%1_D%2").
      arg(getValueTypeInfo.getImportTag(valueTypeID)).
      arg(depletionStage.Index()),
    range.first, range.second, mean, valueTypeID, depletionStage.Index(),
    surfaceBase)));
}

} // namespace cora
