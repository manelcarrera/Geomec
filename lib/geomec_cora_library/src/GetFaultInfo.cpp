#include "GetFaultInfo.h"
#include "HorizonBase.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"

namespace cora
{

// static

CGetFaultInfo& CGetFaultInfo::instance(CModelBase* modelBase)
{
  if (m_getFaultInfo == 0)
  {
  m_getFaultInfo = new CGetFaultInfo(modelBase);
  }

  return *m_getFaultInfo;
}

// non-static

const TObjects CGetFaultInfo::getObjects() const
{
  return m_objects;
}

std::ostream& CGetFaultInfo::operator () (std::ostream& os) const
{
  os << m_objects;

  return os;
}

// private

CGetFaultInfo::CGetFaultInfo(CModelBase* modelBase)
: m_modelBase(modelBase)
, m_objects(getObjects(m_modelBase))
{
  atexit(&cleanup);
}

CGetFaultInfo::~CGetFaultInfo()
{
}

TObjects CGetFaultInfo::getObjects(CModelBase* modelBase)
{
  TObjects objects;

  if (modelBase != 0)
  {
  const THorizonBaseEntry* horizonBaseEntry =
      dynamic_cast <const THorizonBaseEntry*> (
    modelBase->GraphEntry(MD_BASE_HORIZON));
  const THorizonBaseEntry::TEntryNodeSet horizons =
      horizonBaseEntry->GraphEntryNodes();

  for (THorizonBaseEntry::TEntryNodeSet::const_iterator
      horizon = horizons.begin(); horizon != horizons.end(); ++horizon)
  {
      if (dynamic_cast <CHorizonBase*> (*horizon)->Slip())
      {
    objects.push_back(TObject(new CObject(CObject::faultObject, modelBase,
          dynamic_cast <CHorizonBase*> (*horizon))));
      }
  }
  }

  return objects;
}

void CGetFaultInfo::cleanup()
{
  delete m_getFaultInfo;
  m_getFaultInfo = 0;
}

CGetFaultInfo* CGetFaultInfo::m_getFaultInfo = 0;

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetFaultInfo& i)
{
  return i(os);
}
