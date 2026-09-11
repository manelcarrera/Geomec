#include "GetFormationInfo.h"
#include "FormationBase.h"

namespace cora
{

// static

CGetFormationInfo& CGetFormationInfo::instance(CModelBase* modelBase)
{
  if (m_getFormationInfo == 0)
  {
    m_getFormationInfo = new CGetFormationInfo(modelBase);
  }

  return *m_getFormationInfo;
}

// non-static

const TObjects CGetFormationInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetFormationInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetFormationInfo::CGetFormationInfo(CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetFormationInfo::~CGetFormationInfo()
{
}

// static

TObjects CGetFormationInfo::getObjects(CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
    const TFormationBaseEntry* formationBaseEntry =
      dynamic_cast <const TFormationBaseEntry*> (
        modelBase->GraphEntry(MD_BASE_FORMATION));
    const TFormationBaseEntry::TSortedNodeSet entryNodes =
      formationBaseEntry->SortedEntryNodes();

    for (TFormationBaseEntry::TSortedNodeSet::const_iterator
      entryNode = entryNodes.begin(); entryNode != entryNodes.end();
      ++entryNode)
    {
      getObjects(objects, modelBase, *entryNode);
    }
  }

  return objects;
}

void CGetFormationInfo::getObjects(TObjects& objects, CModelBase* modelBase,
  CFormationBase* formationBase)
{
  if (hasElementSet(formationBase))
  {
    objects.push_back(TObject(new CObject(CObject::formationObject,
      formationBase, modelBase)));
  }
}

bool CGetFormationInfo::hasElementSet(const CFormationBase* formationBase)
{
  return (formationBase->ElementSetSize() > 0);
}

void CGetFormationInfo::cleanup()
{
  delete m_getFormationInfo;
  m_getFormationInfo = 0;
}

CGetFormationInfo* CGetFormationInfo::m_getFormationInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetFormationInfo& i)
{
  return i(os);
}
