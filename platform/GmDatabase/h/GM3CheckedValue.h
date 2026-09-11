// CheckedValue.h: interface for the CCheckedValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKEDVALUE_H__2CF8E037_30AD_4079_9CA3_413A48C2CCF1__INCLUDED_)
#define AFX_CHECKEDVALUE_H__2CF8E037_30AD_4079_9CA3_413A48C2CCF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

//##ModelId=3BA8A1C802D3
template<class T>
class CCheckedValue  
{
  //##ModelId=3BA8A1C80303
  typedef std::pair<T, bool> TValue;
  //##ModelId=3BA8A1C802E3
  TValue m_value;
public:
  //##ModelId=3BA8A1C802F2
  CCheckedValue(const T& value) : m_value(value, true) {}
  //##ModelId=3BA8A1C802F4
  CCheckedValue() : m_value(T(), false) {}
  //##ModelId=3BA8A1C802F5
  bool Valid() const { return m_value.second; }
  //##ModelId=3BA8A1C802F7
  void Invalidate() { m_value.second = false; }
  //##ModelId=3BA8A1C802F8
  const T& Value() const { assert(Valid()); return m_value.first; }
  //##ModelId=3BA8A1C80301
  void Value(const T& value) { m_value.first = value; m_value.second = true; }
};

//##ModelId=3BA8A1C80311
typedef CCheckedValue<double>	TCheckedDouble;
//##ModelId=3BA8A1C80321
typedef CCheckedValue<int>		TCheckedInt;


#endif // !defined(AFX_CHECKEDVALUE_H__2CF8E037_30AD_4079_9CA3_413A48C2CCF1__INCLUDED_)
