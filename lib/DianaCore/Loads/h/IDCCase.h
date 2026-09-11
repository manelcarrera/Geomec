#ifndef _IDCCASE_H_
#define _IDCCASE_H_

#include <vector>
#include "Value.h"

namespace dia {

class CLoadManager;
class ILoad;
class ITimeTable;

class ICase
{
  CLoadManager &m_Manager;
  std::vector<const ILoad *> m_vcLoads;
  int m_Index;
  bool m_bActive;
  std::vector<const ITimeTable*> m_vcTimeTables;

protected:
  ICase(CLoadManager &manager, int index);
  virtual std::string FilosDirName() const = 0;

public:
  virtual ~ICase();

  CLoadManager &Manager();

  int Index() const;

  const CLoadManager &Manager() const;

  int LoadSize() const;
  const ILoad &Load(int nLoad) const;

  void AddLoad(ILoad &load);
  void AddTimeTable(const ITimeTable& timetable);

  bool WriteFilos() const;

  bool Active() const;
  void SetActive(bool bActive);
};

}

#endif // _IDCCASE_H_
