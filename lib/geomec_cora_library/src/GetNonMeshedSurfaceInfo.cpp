#include "GetNonMeshedSurfaceInfo.h"
#include "NonMeshedSurface.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"

namespace cora
{

// static

CGetNonMeshedSurfaceInfo& CGetNonMeshedSurfaceInfo::instance(
  CModelBase* modelBase)
{
  if (m_getNonMeshedSurfaceInfo == 0)
  {
    m_getNonMeshedSurfaceInfo = new CGetNonMeshedSurfaceInfo(modelBase);
  }

  return *m_getNonMeshedSurfaceInfo;
}

// non-static

const TObjects CGetNonMeshedSurfaceInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetNonMeshedSurfaceInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetNonMeshedSurfaceInfo::CGetNonMeshedSurfaceInfo(CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetNonMeshedSurfaceInfo::~CGetNonMeshedSurfaceInfo()
{
}

TObjects CGetNonMeshedSurfaceInfo::getObjects(CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
    const CNonMeshedSurfaceEntry* nonMeshedSurfaceEntry =
      dynamic_cast <CNonMeshedSurfaceEntry*> (
        modelBase->GraphEntry(MD_BASE_NONMESHEDSURFACE));
    const CNonMeshedSurfaceEntry::TEntryNodeSet nonMeshedSurfaces =
      nonMeshedSurfaceEntry->GraphEntryNodes();

    for (CNonMeshedSurfaceEntry::TEntryNodeSet::const_iterator
      nonMeshedSurface = nonMeshedSurfaces.begin();
      nonMeshedSurface != nonMeshedSurfaces.end(); ++nonMeshedSurface)
    {
      objects.push_back(TObject(new CObject(CObject::nonMeshedSurfaceObject,
        modelBase, dynamic_cast <CNonMeshedSurface*> (*nonMeshedSurface))));
    }
  }

  return objects;
}

void CGetNonMeshedSurfaceInfo::cleanup()
{
  delete m_getNonMeshedSurfaceInfo;
  m_getNonMeshedSurfaceInfo = 0;
}

CGetNonMeshedSurfaceInfo*
  CGetNonMeshedSurfaceInfo::m_getNonMeshedSurfaceInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os,
  const cora::CGetNonMeshedSurfaceInfo& i)
{
  return i(os);
}
