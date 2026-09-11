 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ValueSet.cpp: implementation of the CValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <cassert>
#include "IElementSet.h"
#include "IElement.h"
#include "ValueSet.h"

namespace geo {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueSet::CValueSet(const IElementSet &elementset) :
  m_ElementSet(elementset)
{
}

CValueSet::CValueSet(const IElementSet &elementset, const std::vector<TValueVec> &values) :
  m_ElementSet(elementset)
{
  int sz = (int)values.size();
  assert(sz == ElementSet().ElementSize()); // must provide same number of values as there are elements in the set

  m_Values.resize(sz);

  for(int i = 0; i < sz; i++)
  {
    int szel = (int)values[i].size();
    assert(szel == ElementSet().Element(i).NrOfPoints());

    m_Values[i].resize(szel);
    for(int j = 0; j < szel; j++) m_Values[i][j] = values[i][j];
  }
}

CValueSet::CValueSet(const CValueSet& rhs)
: m_ElementSet(rhs.m_ElementSet), m_Values(rhs.m_Values)
{
}

CValueSet::~CValueSet()
{
}

// get the elementset this valueset belongs to (get the size there as well)
const IElementSet &CValueSet::ElementSet() const
{
  return m_ElementSet;
}

// get the element values for the element with the given index in the elementset
void CValueSet::ElementValues(TValueVec& values, int iElementIndex, IParallelInitializationCallback* /*cb*/) const
{
  assert(m_Values.size() <= ElementSet().ElementSize()); // we should actually check that the difference equals number of interface elements, in case the elementset is the mesh
  assert(iElementIndex >= 0 && iElementIndex < ElementSet().ElementSize());
  assert(m_Values[iElementIndex].size() == values.size() || m_Values[iElementIndex].size() == 1);
  values = m_Values[iElementIndex];
}

void CValueSet::ElementValues(int iElementIndex, const TValueVec& values, IParallelInitializationCallback* /*cb*/)
{
  assert(m_Values.size() == ElementSet().ElementSize());
  assert(iElementIndex >= 0 && iElementIndex < ElementSet().ElementSize());

  m_Values[iElementIndex] = values;	
}

bool CValueSet::operator==(const CValueSet& rhs) const
{
  assert(&m_ElementSet == &rhs.m_ElementSet);
  return m_Values == rhs.m_Values;
}
  
CValueSet& CValueSet::operator=(const CValueSet& rhs)
{
  assert(&m_ElementSet == &rhs.m_ElementSet);
  m_Values = rhs.m_Values;
  return *this;
}

const std::vector <CValueSet::TValueVec>& CValueSet::valueSetValues() const
{
  return m_Values;
}

void CValueSet::valueSetValues(const std::vector <TValueVec>& values)
{
  m_Values = values;
}

// empty the set
void CValueSet::Empty()
{
  m_Values.clear();
}

// check for emptyness
bool CValueSet::IsEmpty() const
{
  return m_Values.empty();
}

// protected interface to set the values for the given element, returns set index
int CValueSet::PushBack(const TValueVec &values)
{
  int sz = (int)m_Values.size();
  assert(sz < ElementSet().ElementSize()); // may not be full yet (call Empty() first?)

  m_Values.push_back(values);

  return sz;
}

int CValueSet::PushBack(const std::vector<double> &value)
{
  int sz = (int)m_Values.size();
  assert(sz < ElementSet().ElementSize()); // may not be full yet (call Empty() first?)
  TValueVec vcValue(value.size());
  for(int i = 0; i < vcValue.size(); i++)
    vcValue[i] = CValue(value[i]);

  m_Values.push_back(vcValue);

  return sz;
}

void CValueSet::reserveSize(size_t size)
{
  m_Values.reserve(size);
}

}
