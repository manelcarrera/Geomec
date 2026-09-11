
#include <set>
#include <algorithm>

#include "GeomecFormationIDs2RGI.h"
#include "RGInterface.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "MeshBase.h"
#include "FormationBase.h"
#include "RGUtils.h"
#include "IInterfaceElement.h"

namespace
{

namespace {

struct compareFormations {
  bool operator()(CGraphNode *a, CGraphNode *b)
  {
  assert(dynamic_cast<CStorageNode *>(a) && dynamic_cast<CStorageNode *>(b));
  return static_cast<CStorageNode *>(a)->Index() < static_cast<CStorageNode *>(b)->Index();
  }
};

}

std::vector<CFormationBase *> getFormations(const CModelBase& modelBase)
{
  const TFormationBaseEntry *graphEntry = static_cast<const TFormationBaseEntry *>(modelBase.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TEntryNodeSet entryNodeSet = graphEntry->GraphEntryNodes();
  std::vector<CFormationBase *> formations;

  for (TFormationBaseEntry::TEntryNodeSet::const_iterator entryNode = entryNodeSet.begin(); entryNode != entryNodeSet.end(); ++entryNode)
  {
  formations.push_back(static_cast<CFormationBase *>(*entryNode));
  }

  std::sort(formations.begin(), formations.end(), compareFormations());

  return formations;
}

std::vector <int> getGeomecFormationIDs(const CModelBase& modelBase, const std::vector <CFormationBase *>& formations)
{
  size_t elementSize = modelBase.Mesh().Mesh().ElementSize();

  std::vector <int> geomecFormationIDs(elementSize);

  for (int e = 0; e < elementSize; ++e)
  {
  const geo::IElement& element = modelBase.Mesh().Mesh().Element(e);

  if (element.IsElement())
  {
      const CFormationBase* formationBase = modelBase.Mesh().Formation(element);
      std::vector<CFormationBase *>::const_iterator formation = std::find(formations.begin(), formations.end(), formationBase);

      assert(formation != formations.end());

      geomecFormationIDs[e] = std::distance(formations.begin(), formation);
  }
  }

  return geomecFormationIDs;
}

} // anonymous namespace

namespace GeomecRGI
{

CGeomecFormationIDs2RGI::CGeomecFormationIDs2RGI(RGInterface& rgi, const CModelBase& modelBase)
: m_modelBase(modelBase)
, m_rgiFormationIDs(rgi)
{
  m_formations         = getFormations(m_modelBase);
  m_GeomecFormationIDs = getGeomecFormationIDs(m_modelBase, m_formations);
  m_distinctGeomecFormationIDs     = CRGIFormationIDs::getDistinctFormationIDs(m_GeomecFormationIDs);
}

CGeomecFormationIDs2RGI::~CGeomecFormationIDs2RGI()
{
}

std::vector <int> CGeomecFormationIDs2RGI::getFormationIDs() const
{
  return m_GeomecFormationIDs;
}

CFormationBase* CGeomecFormationIDs2RGI::getFormation(int rgiFormationID) const
{
  return static_cast<CFormationBase *>(m_formations[rgiFormationID]);
}

void CGeomecFormationIDs2RGI::SetFormations(RGInterface& rgi, const CModelBase& modelBase)
{
  if (rgi.getFormationName(0).empty())
  {
  std::vector<CFormationBase *> formations = getFormations(modelBase);

  for (size_t i = 0; i < formations.size(); ++i)
  {
      rgi.setFormationName((int)i, formations[i]->Name().toStdString());
  }
  }
}

} // namespace GeomecRGI
