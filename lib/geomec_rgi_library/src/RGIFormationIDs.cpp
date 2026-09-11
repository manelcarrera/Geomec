
#include <limits>

#include "RGIFormationIDs.h"
#include "RGInterface.h"
#include "propertyValues.h"

namespace
{

std::vector <int> loadRGIFormationIDs(RGInterface& rgi)
{
  std::vector <int> rgiFormationIDs;

  rgi.loadProperty(RGPropertyType::propElementFormation, rgiFormationIDs);
  GeomecRGI::propertyValuesMayNotContainNull(
  RGPropertyType::propElementFormation, rgiFormationIDs);

  return rgiFormationIDs;
}

std::map <int, std::size_t> loadRGIFormation2RGIElementMap(
  const std::set <int>& distinctRGIFormationIDs,
  const std::vector <int>& rgiFormationIDs)
{
  std::map <int, std::size_t> rgiFormation2RGIElementMap;

  for (std::set <int>::const_iterator id = distinctRGIFormationIDs.begin();
  id != distinctRGIFormationIDs.end(); ++id)
  {
  const std::vector <int>::const_iterator element =
      std::find(rgiFormationIDs.begin(), rgiFormationIDs.end(), *id);

  assert(element != rgiFormationIDs.end());

  size_t offset = std::distance(rgiFormationIDs.begin(), element);

  rgiFormation2RGIElementMap.insert(
      std::map <int, std::size_t>::value_type(*id, offset));
  }

  return rgiFormation2RGIElementMap;
}

} // anonymous namespace

namespace GeomecRGI
{

CRGIFormationIDs::CRGIFormationIDs(RGInterface& rgi)
: m_RGIFormationIDs(loadRGIFormationIDs(rgi))
, m_DistinctRGIFormationIDs(getDistinctFormationIDs(m_RGIFormationIDs))
, m_RGIFormation2RGIElementMap(loadRGIFormation2RGIElementMap(
  m_DistinctRGIFormationIDs, m_RGIFormationIDs))
{
}

CRGIFormationIDs::~CRGIFormationIDs()
{
}

std::vector <int> CRGIFormationIDs::getRGIFormationIDs() const
{
  return m_RGIFormationIDs;
}

std::set <int> CRGIFormationIDs::getDistinctRGIFormationIDs() const
{
  return m_DistinctRGIFormationIDs;
}

std::size_t CRGIFormationIDs::mapRGIFormation2RGIElement(int rgiFormation) const
{
  if (m_RGIFormation2RGIElementMap.find(rgiFormation) !=
  m_RGIFormation2RGIElementMap.end())
  {
  return m_RGIFormation2RGIElementMap.at(rgiFormation);
  }

  assert(false);

  return std::numeric_limits <std::size_t>::max();
}

// static

std::set <int> CRGIFormationIDs::getDistinctFormationIDs(
  const std::vector <int>& formationIDs)
{
  std::set <int> distinctFormationIDs;

  for (size_t id = 0; id < formationIDs.size(); ++id)
  {
  distinctFormationIDs.insert(formationIDs[id]);
  }

  return distinctFormationIDs;
}

} // namespace GeomecRGI
