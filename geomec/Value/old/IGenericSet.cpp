// IGenericSet.cpp: implementation of the IGenericSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "IGenericSet.h"
#include "OpenGLNode.h"
#include "ValueSet.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IGenericSet::IGenericSet(CGraphModel& model)
: COpenGLNode(model)
{
}

IGenericSet::IGenericSet(const CString &strName, 
						 CGraphModel& model) 
: COpenGLNode(strName, model)
{
}

IGenericSet::IGenericSet(const CString &sName, 
						 CGraphModel& model,
						 int nValueSetSize)
: COpenGLNode(sName, model)
{
	for(int i = 0; i < nValueSetSize; i++)
		AddValueSet();
	ASSERT(nValueSetSize == ValueSetSize());
}

IGenericSet::IGenericSet(const IGenericSet &rhs)
: COpenGLNode(rhs), m_vcValueSet(rhs.m_vcValueSet)
{
	// Make value set copies ....
	for(const_iterator it = rhs.begin(); it != rhs.end(); it++)
	{
		CValueSet *pValueSet = dynamic_cast<CValueSet*>(*it);

		if(pValueSet)
			new CValueSet(*pValueSet);
	}
}

IGenericSet::~IGenericSet()
{
}

int IGenericSet::AddValueSet(CValueSet::eType type)
{
	int nIndex = ValueSetSize();

	// Create new name
	CString sName;
	sName.Format(IDS_PROPERTY_NAME, nIndex);
	new CValueSet(sName, CQuantity::SI_UNIT, type, *this);

	return nIndex;
}

int IGenericSet::ValueSetSize() const
{
	return m_vcValueSet.size();
}

//##ModelId=3C74E3EB00C0
const CValueSet& IGenericSet::ValueSet(int nIndex) const
{
	ASSERT(nIndex > -1);
	ASSERT(nIndex < m_vcValueSet.size());

	if(IsCopy())
		return (const CValueSet&) m_vcValueSet[nIndex]->Copy();
	
	return *m_vcValueSet[nIndex];
}

//##ModelId=3C74E3EB00CD
CValueSet& IGenericSet::ValueSet(int nIndex)
{
	if(IsCopy())
		return (CValueSet&) m_vcValueSet[nIndex]->Copy();
	
	return *m_vcValueSet[nIndex];
}

//##ModelId=3C74E3EB00AD
int IGenericSet::ValueSetIndex(const CValueSet& value_set) const
{ 
	ASSERT(&value_set.PointSet() == this);

	for(int nIndex = 0; nIndex < m_vcValueSet.size(); nIndex++)
	{
		if(value_set.IsCopy() && (m_vcValueSet[nIndex] == &value_set.Source()))
			return nIndex;
			
		if(m_vcValueSet[nIndex] == &value_set)
			return nIndex;
	}

	// Not found ....
	ASSERT(FALSE);
	return -1;
}

void IGenericSet::OnNeighbourDeleted(const CGraphNode &item) 
{
	COpenGLNode::OnNeighbourDeleted(item);

	// Search vector
	std::vector<CValueSet*>::iterator it = m_vcValueSet.begin();
	while((it != m_vcValueSet.end()) && ((*it) != &item))
		it++;

	if(it != m_vcValueSet.end())
		m_vcValueSet.erase(it);

	if(m_vcValueSet.size() > 0)
		return;
}



bool IGenericSet::operator==(const IGenericSet &rhs) const
{
	if(!(  COpenGLNode::operator==(rhs) ))
		return false;

	if(m_vcValueSet != rhs.m_vcValueSet)
		return false;

	// Validate valuesets
	for(int i = 0; i < ValueSetSize(); i++)
	{
		ASSERT(ValueSet(i).IsCopied());
		if(!((CValueSet&)ValueSet(i) == ((CValueSet&)ValueSet(i).Copy())))
			return false;
	}

	return true;
}

void IGenericSet::Load(TLOCATION& location, TPROGRESS& progress)
{

}

void IGenericSet::Save(TLOCATION& location, TPROGRESS& progress)
{

}

long IGenericSet::SavedItems() const
{
	return 1;
}
