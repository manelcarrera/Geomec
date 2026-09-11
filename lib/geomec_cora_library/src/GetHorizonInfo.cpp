#include "GetHorizonInfo.h"
#include "3DHorizon.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"

namespace cora
{

// static

CGetHorizonInfo& CGetHorizonInfo::instance(const CModelBase* modelBase)
{
  if (m_getHorizonInfo == 0)
  {
    m_getHorizonInfo = new CGetHorizonInfo(modelBase);
  }

  return *m_getHorizonInfo;
}

// non-static

const TObjects CGetHorizonInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetHorizonInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetHorizonInfo::CGetHorizonInfo(const CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetHorizonInfo::~CGetHorizonInfo()
{
}

// static

TObjects CGetHorizonInfo::getObjects(const CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
    const THorizonBaseEntry *horizonBaseEntry =
      dynamic_cast <const THorizonBaseEntry*> (
        modelBase->GraphEntry(MD_BASE_HORIZON));

    // faults are not sorted on depth (hence
    // 'EntryNodes()' instead of 'SortedEntryNodes()')

    const THorizonBaseEntry::TNodeSet entryNodes =
      horizonBaseEntry->EntryNodes();

    for (THorizonBaseEntry::TNodeSet::const_iterator
      entryNode = entryNodes.begin(); entryNode != entryNodes.end();
      ++entryNode)
    {
      // TODO SURFACE-HORIZON
      // for now only add the top-horizon

      if ((dynamic_cast <C3DHorizon*> (*entryNode))->IsTopHorizon())
      {
        objects.push_back(
          TObject(new CObject(CObject::horizonObject, *entryNode)));
      }
    }
  }

  return objects;
}

void CGetHorizonInfo::cleanup()
{
  delete m_getHorizonInfo;
  m_getHorizonInfo = 0;
}

CGetHorizonInfo* CGetHorizonInfo::m_getHorizonInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetHorizonInfo& i)
{
  return i(os);
}
