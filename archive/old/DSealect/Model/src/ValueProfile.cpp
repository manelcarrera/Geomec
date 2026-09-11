#include "StdAfx.h"

#include "ValueProfile.h"

CValueProfile::CValueProfile()
{
}

CValueProfile::CValueProfile(const CValueProfile& rhs)
: m_mpValues(rhs.m_mpValues)
{
}

CValueProfile& CValueProfile::operator=(const CValueProfile& rhs)
{
  m_mpValues = rhs.m_mpValues;
  return *this;
}

bool CValueProfile::operator==(const CValueProfile& rhs) const
{
  return m_mpValues == rhs.m_mpValues;
}

double CValueProfile::MinDepth() const
{
  assert(!empty());
  return begin()->first;
}

double CValueProfile::MaxDepth() const
{
  assert(!empty());
  return rbegin()->first;
}

double CValueProfile::ValueAt(double dDepth) const
{
  assert(!empty());

  TValueMap::const_iterator itUpper = m_mpValues.upper_bound(dDepth);

  // get reverse_iterator from iterator, points to previous value (or rend())
  TValueMap::const_reverse_iterator ritUpper(itUpper);

  if(itUpper == m_mpValues.end()) // too deep
    return rbegin()->second; // return deepest value (assume constant with depth)

  if(ritUpper == m_mpValues.rend()) // too shallow
    return begin()->second; // return shallowest value (assume constant with depth)

  double d1 = ritUpper->first;  // shallower depth
  double d2 = itUpper->first;   // deeper depth
  double v1 = ritUpper->second; // shallower value
  double v2 = itUpper->second;  // deeper value

  return v1 + (dDepth - d1) * (v2 - v1) / (d2 - d1);
}

std::vector<std::pair<double, double> > CValueProfile::ValuesFromTo(double dDepthFrom, double dDepthTo) const
{
  std::vector<std::pair<double, double> > vcValues;

  // add from value
  vcValues.push_back(std::make_pair(dDepthFrom, ValueAt(dDepthFrom)));

  const_iterator itUpper = m_mpValues.upper_bound(dDepthFrom);
  for(const_iterator it = itUpper; it != end() && it->first < dDepthTo; ++it)
    vcValues.push_back(std::make_pair(it->first, it->second));

  // add to value
  vcValues.push_back(std::make_pair(dDepthTo, ValueAt(dDepthTo)));

  return vcValues;
}

CValueProfile::const_iterator CValueProfile::begin() const
{
  return m_mpValues.begin();
}

CValueProfile::const_iterator CValueProfile::end() const
{
  return m_mpValues.end();
}

CValueProfile::const_reverse_iterator CValueProfile::rbegin() const
{
  return m_mpValues.rbegin();
}

CValueProfile::const_reverse_iterator CValueProfile::rend() const
{
  return m_mpValues.rend();
}

bool CValueProfile::empty() const
{
  return m_mpValues.empty();
}

bool CValueProfile::AddValues(double dDepth, double dValue)
{
  return m_mpValues.insert(TValueMap::value_type(dDepth, dValue)).second;
}

void CValueProfile::clear()
{
  m_mpValues.clear();
}
