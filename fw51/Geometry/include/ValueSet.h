 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ValueSet.h: interface for the CValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUESET_H__42A3CB37_8778_437F_B9F5_34508E4FA0F5__INCLUDED_)
#define AFX_VALUESET_H__42A3CB37_8778_437F_B9F5_34508E4FA0F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValueSet.h"

#include "GeometryExports.h"

namespace geo {
class GEOMETRY_EXPORT  CValueSet : public IValueSet  
{
public:
  CValueSet(const IElementSet &elementset);
  CValueSet(const IElementSet &elementset, const std::vector<TValueVec> &values);
  CValueSet(const CValueSet& rhs);
  virtual ~CValueSet();

  // get the elementset this valueset belongs to (get the size there as well)
  virtual const IElementSet &ElementSet() const;

  // get the element values for the element with the given index in the elementset
  virtual void ElementValues(TValueVec& values, int iElementIndex, IParallelInitializationCallback *cb) const;

  // empty the set
  virtual void Empty();
  // check for emptyness
  virtual bool IsEmpty() const;

  // protected interface to set the values for the given element, returns set index
  virtual int PushBack(const TValueVec &values);
  int PushBack(const std::vector<double> &value);
  virtual void reserveSize(size_t size);

  // set function for modification
  void ElementValues(int iElementIndex, const TValueVec& values, IParallelInitializationCallback *cb);

  // get and set m_Values
  const std::vector <TValueVec>& valueSetValues() const;
  void valueSetValues(const std::vector <TValueVec>& values);

  // operators
  bool operator==(const CValueSet& rhs) const;
  CValueSet& operator=(const CValueSet& rhs);
private:
  const IElementSet &m_ElementSet;
  std::vector<TValueVec> m_Values;
};

}

#endif // !defined(AFX_VALUESET_H__42A3CB37_8778_437F_B9F5_34508E4FA0F5__INCLUDED_)
