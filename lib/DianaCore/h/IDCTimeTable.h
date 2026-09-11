#ifndef _IDCTIMETABLE_H_
#define _IDCTIMETABLE_H_

#include <map>

namespace dia {

class CLoadManager;
class ICase;

class ITimeTable {
  ICase &m_case;

  typedef std::map<double, double> TTimeFactorMap;
  TTimeFactorMap m_mpTable;

protected:
  ITimeTable(ICase &icase);
  virtual void ChangeToTableDir() const = 0;

public:
  virtual ~ITimeTable();

  void Insert(double dTime, double dFactor);
  const ICase &Case() const;

  bool WriteFilos() const;
};

} // namespace dia

#endif // _IDCTIMETABLE_H_
