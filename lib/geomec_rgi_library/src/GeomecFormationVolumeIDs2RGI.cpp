
#include <set>

#include "GeomecFormationVolumeIDs2RGI.h"
#include "RGInterface.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "MeshBase.h"
#include "FormationBase.h"
#include "3DFormation.h"
#include "FormationVolume.h"
#include "propertyValues.h"
#include "IInterfaceElement.h"

namespace
{

std::set <int> getDistinctFormationVolumeIDs(
  const std::vector <int>& formationVolumeIDs)
{
  std::set <int> distinctFormationVolumeIDs;

  for (size_t id = 0; id < formationVolumeIDs.size(); ++id)
  {
  distinctFormationVolumeIDs.insert(formationVolumeIDs[id]);
  }

  return distinctFormationVolumeIDs;
}

std::vector <CGraphNode*> getFormationVolumes(const CModelBase& modelBase)
{
  const CGraphEntry* graphEntry = modelBase.GraphEntry(MD_BASE_FORMATION);
  CGraphEntry::TEntryNodeSet entryNodeSet = graphEntry->GraphEntryNodes();
  std::vector <CGraphNode*> formationVolumes;

  for (CGraphEntry::TEntryNodeSet::const_iterator entryNode =
  entryNodeSet.begin(); entryNode != entryNodeSet.end(); ++entryNode)
  {
  C3DFormation* formation = dynamic_cast <C3DFormation*> (*entryNode);

  for (int volume = 0; volume < formation->VolumeSize(); ++volume)
  {
      formationVolumes.push_back(&formation->Volume(volume));
  }
  }

  return formationVolumes;
}

std::vector <int> getGeomecFormationVolumeIDs(const CModelBase& modelBase,
  const std::vector <CGraphNode*>& formationVolumes)
{
  std::vector <int>
  geomecFormationVolumeIDs(modelBase.Mesh().Mesh().ElementSize());

  for (int e = 0; e < modelBase.Mesh().Mesh().ElementSize(); ++e)
  {
  const geo::IElement& element = modelBase.Mesh().Mesh().Element(e);

  if (dynamic_cast<const geo::IInterfaceElement *>(&element))
      continue;

  const IFormationElementSet* formationElementSet =
      modelBase.Mesh().FormationElementSet(element);

  assert(formationElementSet != 0);

  std::vector <CGraphNode*> ::const_iterator formationVolume =
      std::find(formationVolumes.begin(), formationVolumes.end(),
    formationElementSet);

  assert(formationVolume != formationVolumes.end());

  geomecFormationVolumeIDs[e] =
      std::distance(formationVolumes.begin(), formationVolume);
  }

  return geomecFormationVolumeIDs;
}

} // anonymous namespace

namespace GeomecRGI
{

CGeomecFormationVolumeIDs2RGI::CGeomecFormationVolumeIDs2RGI(
  RGInterface& /*rgi*/, const CModelBase& modelBase)
: m_modelBase(modelBase)
, m_formationVolumes(getFormationVolumes(m_modelBase))
, m_GeomecFormationVolumeIDs(
  getGeomecFormationVolumeIDs(m_modelBase, m_formationVolumes))
, m_distinctGeomecFormationVolumeIDs(
  getDistinctFormationVolumeIDs(m_GeomecFormationVolumeIDs))
{
}

CGeomecFormationVolumeIDs2RGI::~CGeomecFormationVolumeIDs2RGI()
{
}

std::vector <int> CGeomecFormationVolumeIDs2RGI::getFormationVolumeIDs() const
{
  return m_GeomecFormationVolumeIDs;
}

} // namespace GeomecRGI
