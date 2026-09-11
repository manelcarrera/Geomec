#ifndef _VALUEPROFILE_H_
#define _VALUEPROFILE_H_

#include <map>

class CValueProfile
{
public:
  typedef std::map<double, double> TValueMap;
  typedef TValueMap::const_iterator const_iterator;
  typedef TValueMap::const_reverse_iterator const_reverse_iterator;
  typedef std::vector<std::pair<double, double> > TDepthValueVector;

public:
  CValueProfile();
  CValueProfile(const CValueProfile& rhs);

  CValueProfile& operator=(const CValueProfile& rhs);
  bool operator==(const CValueProfile& rhs) const;

  double MinDepth() const;
  double MaxDepth() const;

  double ValueAt(double dDepth) const;
  TDepthValueVector ValuesFromTo(double dDepthFrom, double dDepthTo) const;

  const_iterator begin() const;
  const_iterator end() const;

  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  bool empty() const;

  bool AddValues(double dDepth, double dValue);
  void clear();

private:
  TValueMap m_mpValues;
};

#endif // _VALUEPROFILE_H_
