#include "GetWellPathInfo.h"
#include "NewWellPath.h"

namespace cora
{

// static

CGetWellPathInfo& CGetWellPathInfo::instance(const CModelBase* modelBase)
{
  if (m_getWellPathInfo == 0)
  {
  m_getWellPathInfo = new CGetWellPathInfo(modelBase);
  }

  return *m_getWellPathInfo;
}

// non-static

const TObjects CGetWellPathInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetWellPathInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetWellPathInfo::CGetWellPathInfo(const CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetWellPathInfo::~CGetWellPathInfo()
{
}

// static

TObjects CGetWellPathInfo::getObjects(const CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
  const CNewWellPathEntry* newWellPathEntry =
      dynamic_cast <const CNewWellPathEntry*> (
    modelBase->GraphEntry(MD_NEW_WELLPATH));
  const CNewWellPathEntry::TSortedNodeSet entryNodes =
      newWellPathEntry->SortedEntryNodes();

  for (CNewWellPathEntry::TSortedNodeSet::const_iterator
      entryNode = entryNodes.begin(); entryNode != entryNodes.end();
      ++entryNode)
  {
      objects.push_back(TObject(new CObject(CObject::wellObject, *entryNode)));
  }
  }

  return objects;
}

void CGetWellPathInfo::cleanup()
{
  delete m_getWellPathInfo;
  m_getWellPathInfo = 0;
}

CGetWellPathInfo* CGetWellPathInfo::m_getWellPathInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetWellPathInfo& i)
{
  return i(os);
}
