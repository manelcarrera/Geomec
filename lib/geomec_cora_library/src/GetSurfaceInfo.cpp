#include "GetSurfaceInfo.h"
#include "GeoSurface.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"

namespace cora
{

// static

CGetSurfaceInfo& CGetSurfaceInfo::instance(const CModelBase* modelBase)
{
  if (m_getSurfaceInfo == 0)
  {
    m_getSurfaceInfo = new CGetSurfaceInfo(modelBase);
  }

  return *m_getSurfaceInfo;
}

// non-static

const TObjects CGetSurfaceInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetSurfaceInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetSurfaceInfo::CGetSurfaceInfo(const CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetSurfaceInfo::~CGetSurfaceInfo()
{
}

// static

TObjects CGetSurfaceInfo::getObjects(const CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
    const CSurfaceEntry* surfaceEntry =
      dynamic_cast <const CSurfaceEntry*> (
        modelBase->GraphEntry(MD_BASE_SURFACE));
    const CSurfaceEntry::TSortedNodeSet entryNodes =
      surfaceEntry->SortedEntryNodes();

    for (CSurfaceEntry::TSortedNodeSet::const_iterator
      entryNode = entryNodes.begin(); entryNode != entryNodes.end();
      ++entryNode)
    {
      objects.push_back(TObject(new CObject(CObject::surfaceObject, *entryNode)));
    }
  }

  return objects;
}

void CGetSurfaceInfo::cleanup()
{
  delete m_getSurfaceInfo;
  m_getSurfaceInfo = 0;
}

CGetSurfaceInfo* CGetSurfaceInfo::m_getSurfaceInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetSurfaceInfo& i)
{
  return i(os);
}
