#include "IDCCase.h"

#include "DCLoadManager.h"
#include "IDCLoad.h"
#include "IDCTimeTable.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

ICase::ICase(CLoadManager &manager, int index)
:	m_Manager(manager), m_Index(index), m_bActive(true)
{
}

ICase::~ICase()
{
  for(size_t i = 0; i < m_vcTimeTables.size(); ++i)
  delete m_vcTimeTables[i];
}

int ICase::Index() const
{
  return m_Index;
}

CLoadManager &ICase::Manager()
{
  return m_Manager;
}

const CLoadManager &ICase::Manager() const
{
  return m_Manager;
}

int ICase::LoadSize() const
{
  return m_vcLoads.size();
}

const ILoad &ICase::Load(int nLoad) const
{
  return *m_vcLoads[nLoad];
}

void ICase::AddLoad(ILoad &load)
{
  m_vcLoads.push_back(&load);
}

void ICase::AddTimeTable(const ITimeTable& timetable)
{
  m_vcTimeTables.push_back(&timetable);
}

bool ICase::WriteFilos() const
{
  bool bSuccess = true;

  if(Active())
  {
//		assert(LoadSize());
    ftn_int_t idx = (ftn_int_t) (Index() + 1);

  std::string sDirName = std::string("/") + FilosDirName() + "/";

    if(XistIndexed(sDirName.c_str(), &idx))
      RemoveIndexedItem(sDirName.c_str(), &idx);

    PushDir();

    ChangeIndexedDir(sDirName.c_str(), &idx);

    for(int i = 0; i < LoadSize(); i++)
    {
      if(!Load(i).WriteFilos())
      {
        bSuccess = false;
        break;
      }
    }

    PopDir();
  }

  if(bSuccess)
  {
  for(size_t i = 0; i < m_vcTimeTables.size(); ++i)
  {
      if(!m_vcTimeTables[i]->WriteFilos())
      {
    bSuccess = false;
    break;
      }
  }
  }

  return bSuccess;
}

bool ICase::Active() const
{
  return m_bActive;
}

void ICase::SetActive(bool bActive)
{
  m_bActive = bActive;
}

} // namespace dia
