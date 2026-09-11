// DistributedQuantity.cpp: implementation of the CDistributedQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "DistributedQuantity.h"
#include "AttriDistributedQuantitydlg.h"
#include "PointSet.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3BE7AEC401F9
CDistributedQuantity::CDistributedQuantity(CPointSet &point_set, const int nComponentSize)
: CQuantityNode(point_set.Name()), m_point_set(point_set)
{
  ASSERT(nComponentSize > 0);
  m_vcValueSet.resize(nComponentSize);
  LinkTo(point_set);
}

CDistributedQuantity::CDistributedQuantity(const CDistributedQuantity &rhs)
: CQuantityNode(rhs), m_point_set(rhs.m_point_set), m_vcValueSet(rhs.m_vcValueSet)
{
  ASSERT(PointSet().IsCopy());
  ASSERT(rhs == (*this));
}
 
//##ModelId=3BE7AEC40208
CDistributedQuantity::~CDistributedQuantity()
{

}

//##ModelId=3BE7AEC40256
int CDistributedQuantity::ComponentSize() const
{
  return ValueSetSize();
}

//##ModelId=3BE7AEC40258
double CDistributedQuantity::MaxValue(const int nComponent, const UNIT unit) const
{
  ASSERT(nComponent < ComponentSize());
  ASSERT((unit == FIELD_UNIT) || (unit == SI_UNIT));

  return Convert(ValueSet(nComponent).Max(), unit, SI_UNIT);
}


//##ModelId=3BE7AEC40266
double CDistributedQuantity::MinValue(const int nComponent, const UNIT unit) const
{
  ASSERT(nComponent < ComponentSize());
  ASSERT((unit == FIELD_UNIT) || (unit == SI_UNIT));

  return Convert(ValueSet(nComponent).Min(), unit, SI_UNIT);
}
  
//##ModelId=3BE7AEC4026A
double CDistributedQuantity::Min(const geo::IBox &box, const int nComponent, const UNIT unit) const
{
  ASSERT(nComponent < ComponentSize());
  ASSERT((unit == FIELD_UNIT) || (unit == SI_UNIT));

  return Convert(ValueSet(nComponent).Min(box), unit, SI_UNIT);
}

//##ModelId=3BE7AEC40278
double CDistributedQuantity::Max(const geo::IBox &box, const int nComponent, const UNIT unit) const
{
  ASSERT(nComponent < ComponentSize());
  ASSERT((unit == FIELD_UNIT) || (unit == SI_UNIT));

  return Convert(ValueSet(nComponent).Max(box), unit, SI_UNIT);
}

//##ModelId=3BE7AEC40285
double CDistributedQuantity::Value(const geo::IPoint& pt, 
                   const int nComponent, 
                   const UNIT unit) const
{
  ASSERT(nComponent < ComponentSize());
  ASSERT((unit == FIELD_UNIT) || (unit == SI_UNIT));

  return Convert(ValueSet(nComponent).Value(pt), unit, SI_UNIT);
}
//##ModelId=3BE7AEC4020A
const CValueSet& CDistributedQuantity::ValueSet(const int nIndex) const
{
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < ValueSetSize());
  ASSERT(m_vcValueSet[nIndex] != 0);

  if(IsCopy())
  {
    m_vcValueSet[nIndex]->IsCopied();
    return (const CValueSet&)(m_vcValueSet[nIndex]->Copy());
  }

  return *m_vcValueSet[nIndex];
}

//##ModelId=3BE7AEC40218
CValueSet& CDistributedQuantity::ValueSet(const int nIndex)
{
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < ValueSetSize());
  ASSERT(m_vcValueSet[nIndex] != 0);

  if(IsCopy())
  {
    m_vcValueSet[nIndex]->IsCopied();
    return (CValueSet&)(m_vcValueSet[nIndex]->Copy());
  }

  return *m_vcValueSet[nIndex];
}

//##ModelId=3BE7AEC4022B
void CDistributedQuantity::ValueSet(const int nIndex, CValueSet &value)
{
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < ValueSetSize());

  // Unlink the old stuff ...
  if(IsValueSet(nIndex))
    UnLink(*m_vcValueSet[nIndex]);
  
  // Link to new stuff ...
  if(value.IsCopy())
  {
    ASSERT(IsCopy());
    m_vcValueSet[nIndex] = (CValueSet*)(&value.Source());
  }
  else
    m_vcValueSet[nIndex] = &value;

  if(!IsLinkedTo(value))
    LinkTo(value);
}

//##ModelId=3BE7AEC40228
BOOL CDistributedQuantity::IsValueSet(const int nIndex) const
{
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < ValueSetSize());
  return m_vcValueSet[nIndex] != 0;
}

//##ModelId=3BE7AEC40238
int CDistributedQuantity::ValueSetSize() const
{
  return m_vcValueSet.size();
}

//##ModelId=3BE7AEC4023C
void CDistributedQuantity::OnNewNeighbour(const CGraphNode &node)
{
  // An newly assigned valueset must be unique in the vector ...
#ifdef _DEBUG
  const CValueSet *pValue = dynamic_cast<const CValueSet*>(&node);
  if(pValue)
  {
    BOOL bFound = FALSE;
    for(std::vector<CValueSet*>::iterator it = m_vcValueSet.begin(); it != m_vcValueSet.end(); it++)
    {
      if(*it == &node)
      {
        ASSERT(!bFound);	// Unique
        bFound = TRUE;
      }
    }

    ASSERT(bFound);		
  }
#endif			

  CGraphNode::OnNewNeighbour(node);
}

//##ModelId=3BE7AEC40249
void CDistributedQuantity::OnNeighbourDeleted(const CGraphNode &node)
{
  for(std::vector<CValueSet*>::iterator it = m_vcValueSet.begin(); it != m_vcValueSet.end(); it++)
  {
    if(*it == &node)
      *it = 0;
  }

  if(&node == &m_point_set)
  {
    CQuantityNode::OnNeighbourDeleted(node);
    delete this;
    return;
  }

  CQuantityNode::OnNeighbourDeleted(node);
}



//##ModelId=3BE7AEC4023A
const CPointSet& CDistributedQuantity::PointSet() const
{
  ASSERT(IsLinkedTo(m_point_set));
  if(IsCopy())
  {
    ASSERT(m_point_set.IsCopied());
    return (const CPointSet&)(m_point_set.Copy());
  }

  return m_point_set;
}

//##ModelId=3BE7AEC4024C
UINT CDistributedQuantity::TypeNameId() const
{
  return 0;
}

bool CDistributedQuantity::Undefined() const
{
  return ValueSetSize() == 0;
}

double CDistributedQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->Convert(value, out, in);
}

const IQuantityEntry& CDistributedQuantity::QuantityEntry() const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->QuantityEntry();
}

UINT CDistributedQuantity::IconId() const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->IconId();
}
  
const COpenGLNode* CDistributedQuantity::ProjectOn() const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->ProjectOn();
}

std::string CDistributedQuantity::QuantityName() const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->QuantityName();
}

std::string CDistributedQuantity::UnitName(const UNIT unit) const
{
  ASSERT(IsCopy());
  CDistributedQuantity *pQuantity = (CDistributedQuantity*)(&Source());
  return pQuantity->UnitName(unit);
}

BOOL CDistributedQuantity::operator==(const CDistributedQuantity &rhs) const
{
  ASSERT(IsCopied());
  ASSERT(rhs.IsCopy());
  ASSERT(ValueSetSize() == rhs.ValueSetSize());

  if(!CQuantityNode::operator ==(rhs))
    return FALSE;

  for(int i = 0; i < ValueSetSize(); i++)
  {
    if(m_vcValueSet[i] != rhs.m_vcValueSet[i])
      return FALSE;
  }

  return TRUE;
}

CDistributedQuantity& CDistributedQuantity::operator=(const CDistributedQuantity &rhs)
{
  ASSERT(IsCopied());
  ASSERT(rhs.IsCopy());
  ASSERT(ValueSetSize() == rhs.ValueSetSize());

  // Assign ....
  m_vcValueSet = rhs.m_vcValueSet;

  CQuantityNode::operator=(rhs);

  return *this;
}

void CDistributedQuantity::Properties()
{
  CAttriDistributedQuantityDlg dlg(*this);
  dlg.DoModal();
}