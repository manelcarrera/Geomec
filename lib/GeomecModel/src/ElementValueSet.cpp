// ElementValueSet.cpp: implementation of the CElementValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "ElementValueSet.h"
#include "ElementSet.h"
#include "StreamVersion.h"
#include "resourceIDI.h"
#include "IInterfaceElement.h"
#include "IParallelInitializationCallback.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Constructor for loading purposes
CElementValueSet::CElementValueSet(IElementSet& set)
: IValueSet(set), m_value_set(set.ElementSet()), m_pElementSet(&set), m_bDirty(true)
{
  assert(!set.IsCopy() && !set.IsCopied());
  m_pElementSet->m_vcElementValueSet.push_back(this);	
}

CElementValueSet::CElementValueSet(const QString &sName, CQuantity::UNIT unit, IElementSet& set)
: IValueSet(sName, unit, set), m_value_set(set.ElementSet()), m_pElementSet(&set), m_bDirty(true)
{
  if(set.IsCopy())
    m_pElementSet = (IElementSet*) &set.Source();
  else
  {
    m_pElementSet->m_vcElementValueSet.push_back(this);
    assert(IsLinkedTo(ElementSet()));
  }
}

CElementValueSet::CElementValueSet(const CElementValueSet& rhs)
: IValueSet(rhs), m_value_set(rhs.m_value_set), m_pElementSet(rhs.m_pElementSet), m_bDirty(true)
{
  // Check or we're registered in the m_vcNodalValueSet 
  bool bFound = false;
  for(size_t i = 0; i < ElementSet().m_vcElementValueSet.size(); i++)
  {
    if(ElementSet().m_vcElementValueSet[i] == &rhs)
      bFound = true;
  }

  if(!bFound)
    ElementSet().m_vcElementValueSet.push_back((CElementValueSet*) &rhs);
}

CElementValueSet::~CElementValueSet()
{
}

CElementValueSet* CElementValueSet::clone()
{
  /*
     Do not use the copy-constructor.
     The base-class 'CGraphNode' introduces an algorithm that limits the
     number of copies.
  */

  CElementValueSet* elementValueSet = new CElementValueSet(ElementSet());

  elementValueSet->m_dMin = m_dMin;
  elementValueSet->m_dMax = m_dMax;

  elementValueSet->getValueSet().valueSetValues(getValueSet().valueSetValues());

  return elementValueSet;
}

void CElementValueSet::Clear()
{
  m_value_set.Empty();
  m_bDirty = true;
}

bool CElementValueSet::operator==(const CElementValueSet &rhs) const
{
  if(!IValueSet::operator ==(rhs))
    return false;
  return m_value_set == m_value_set;
}

void CElementValueSet::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  IValueSet::LoadStream(stream, version, progress);

  m_bDirty = true;

  // From version 3.0.28 we load weighting type
  if(CStreamVersion(3, 0, 27) < version)
  {
    int nType;
    stream >> nType;
    // We don't use the weighting here. It is moved to valuetype
  }

  int nSize;
  stream >> nSize;
  m_value_set.reserveSize(nSize);
  for(int i = 0; i < nSize; i++)
  {
    VERIFY(m_value_set.PushBack(LoadValueVec(stream, version)) == i);
    progress.Step();
  }
}

void CElementValueSet::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
  IValueSet::SaveStream(stream, progress);

  // We don't use value types here
  int nObsolete = 0;
  stream << nObsolete;

  int nSize = m_value_set.ElementSet().ElementSize();

  for(int i = 0; i < m_value_set.ElementSet().ElementSize(); i++)
  {
  // for geomec_rgi the element set is the mesh, including interface elements, which need to be ignored
  if (dynamic_cast<const geo::IInterfaceElement*>(&m_value_set.ElementSet().Element(i)))
      --nSize;
  }

  stream << nSize;
  for(int i = 0; i < m_value_set.ElementSet().ElementSize(); i++)
  {
  // interface elements should be at end, so we could use nSize, but just in case...
  if (dynamic_cast<const geo::IInterfaceElement*>(&m_value_set.ElementSet().Element(i)))
      continue;
    geo::IValueSet::TValueVec vcValues( m_value_set.ElementSet().Element(i).NrOfPoints() );
  m_value_set.ElementValues(vcValues, i, geo::IParallelInitializationCallback::Sequential);
    SaveValueVec(vcValues, stream);
    progress.Step();
  }

}

geo::IValueSet::TValueVec CElementValueSet::LoadValueVec(TSTREAM& stream, const CStreamVersion& version)
{
  int nSize;
  stream >> nSize;
  TValueVec ret(nSize);
  for(size_t i = 0; i < ret.size(); i++)
  {
    int bValid = true;
    if(CStreamVersion(3, 0, 18) < version)
      stream >> bValid;
    if(bValid)
    {
      double dValue;
      stream >> dValue;
      ret[i] = geo::CValue(dValue);
    }
    else
      ret[i] = geo::CValue();
  }

  return ret;
}

void CElementValueSet::SaveValueVec(const TValueVec& data, TSTREAM& stream)
{
  int nSize = data.size();
  stream << nSize;
  for(size_t i = 0; i < data.size(); i++)
  {
    int bValid = data[i].Valid();
    stream << bValid;
    if(bValid)
      stream << data[i].Value();
  }
}

CElementValueSet& CElementValueSet::operator=(const CElementValueSet& rhs)
{
  IValueSet::operator =(rhs);
  m_value_set = rhs.m_value_set;

  m_bDirty = true;
  m_pElementSet = rhs.m_pElementSet;

  return *this;
}

//=============================================================================
unsigned int CElementValueSet::TypeId() const
{
  return 0;
}

unsigned int CElementValueSet::IconId() const
{
  return IDI_ELEMENT_VALUESET;
}

int CElementValueSet::PushBack(const double &value)
{
  m_bDirty = true;
  TValueVec vcValues;
  vcValues.push_back(geo::CValue(value));
  return m_value_set.PushBack(vcValues);
}

int CElementValueSet::PushBack(const TValueVec& value)
{
  m_bDirty = true;
  return m_value_set.PushBack(value);
}

int CElementValueSet::PushBack(const std::vector<double>& value)
{
  m_bDirty = true;
  return m_value_set.PushBack(value);
}

//=============================================================================
geo::IValueSet::TValueVec CElementValueSet::ValueElement(const geo::IElement& element, geo::IParallelInitializationCallback *cb) const
{
  assert(element.IndexingElementSet() == &ElementSet().ElementSet());

  return ValueElement(element.Index(), cb);
}

int CElementValueSet::ValueSize() const
{
  if(m_value_set.IsEmpty())
    return 0;
  return m_value_set.ElementSet().ElementSize();
}

geo::IValueSet::TValueVec CElementValueSet::ValueAt(int nIndex, geo::IParallelInitializationCallback *cb) const
{
  return ValueElement(nIndex, cb);
}

geo::IValueSet::TValueVec CElementValueSet::ValueElement(int nIndex, geo::IParallelInitializationCallback *cb) const
{
  geo::IValueSet::TValueVec vcValues( m_value_set.ElementSet().Element(nIndex).NrOfPoints() );
  m_value_set.ElementValues(vcValues, nIndex, cb);
  int element_point_size = ElementSet().ElementSet().Element(nIndex).NrOfPoints();
  assert(vcValues.size() == 1 || vcValues.size() == element_point_size);

  if(vcValues.size() == 1)
  {
    // resize to the complete number of points first
    vcValues.resize(element_point_size, vcValues[0]);
  }

  return vcValues;
}

// Min, max enz
void CElementValueSet::CalculateProperties() const
{
  m_dMin = DBL_MAX;
  m_dMax = -DBL_MAX;

  for (int i = 0; i < m_value_set.ElementSet().ElementSize(); i++)
  {
  if (m_value_set.ElementSet().Element(i).IsInterfaceElement())
      continue;
    geo::IValueSet::TValueVec e_value( m_value_set.ElementSet().Element(i).NrOfPoints() );
  m_value_set.ElementValues(e_value, i, geo::IParallelInitializationCallback::Sequential);
    for(size_t j = 0; j < e_value.size(); j++)
    {
      m_dMin = m_dMin.Min(e_value[j]);
      m_dMax = m_dMax.Max(e_value[j]);
    }
  }

  // Set dirty false to prevent nested endless loops
  m_bDirty = false;
}


CElementValueSet::TValue CElementValueSet::Min() const
{
  if (m_bDirty)
    CalculateProperties();

  return m_dMin;
}

CElementValueSet::TValue CElementValueSet::Max() const
{
  if (m_bDirty)
    CalculateProperties();

  return m_dMax;
}



//=============================================================================
// Present Point
CElementValueSet::TValue CElementValueSet::ValuePoint(const geo::IPoint& pt, geo::IParallelInitializationCallback *cb) const
{
  // wjrx mantis 3480
  //
  TValue val= m_value_set.ValuePoint(pt, cb);
  if ( ! val.Valid() ) 
  {
    std::vector< const geo::IPoint * > vec= ElementSet().ElementSet().NearestPoint(pt);
    for ( size_t ii= 0; ii< vec.size(); ++ii)
    {
      val= m_value_set.ValuePoint(*vec[ii], cb);
      // return with the first valid one
      if ( val.Valid() ) break;
    }
  }
  return val;
}

//=============================================================================
long CElementValueSet::SavedItems() const
{
  return IValueSet::SavedItems() + m_value_set.ElementSet().ElementSize();
}

//=============================================================================
const IElementSet& CElementValueSet::ElementSet() const
{
  assert(!m_pElementSet->IsCopy());
  if(IsCopy() && m_pElementSet->IsCopied())
    return (IElementSet&)(m_pElementSet->Copy());

  return *m_pElementSet;
}

IElementSet& CElementValueSet::ElementSet()
{
  assert(!m_pElementSet->IsCopy());
  if(IsCopy() && m_pElementSet->IsCopied())
    return (IElementSet&)(m_pElementSet->Copy());

  return *m_pElementSet;
}

const geo::CValueSet& CElementValueSet::getValueSet() const
{
  return m_value_set;
}

geo::CValueSet& CElementValueSet::getValueSet()
{
  return m_value_set;
}
