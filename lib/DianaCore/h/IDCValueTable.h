#ifndef _IDCVALUETABLE_H_
#define _IDCVALUETABLE_H_

#include <string>
#include <vector>

class IProgressBase;

namespace geo {
class IElement;
class IMesh;
}

namespace dia {

class IDianaRunner;

class IValueTable
{
public:
  class CTimePoint
  {
    double m_time;

  public:
    CTimePoint(double time);

    const double& Time() const;
  };

public:
  IValueTable(const IDianaRunner &runner);
  virtual ~IValueTable();

  const CTimePoint& AddTime(double dTime);

  int Size() const;
  const CTimePoint& Time(int nIndex) const;

  void Write(IProgressBase &progress);

  void CleanUp(bool cleanup);
  virtual void SetRunner(IDianaRunner *runner);

protected:
  virtual bool HasValue(const geo::IElement &element) const = 0;
  virtual void ValueAt(const geo::IElement &element, const CTimePoint& tpoint, std::vector<double>& vcValues) const = 0;
  virtual std::string TableName() const = 0;
  virtual std::string TimeTableName() const = 0;
  virtual bool CompressTable() const;

private:
  void WriteTable();
  void DoCleanUp();

private:
  const geo::IMesh &m_mesh;
  std::vector<const CTimePoint*> m_vcTimes;

  std::vector<std::vector<std::vector<double> > > m_vcElementNodeBasedValues;
  double m_dMinVal;
  double m_dMaxVal;
  bool   m_bCleanUp;
};

}

#endif // _IDCVALUETABLE_H_
