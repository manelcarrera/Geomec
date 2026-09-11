// Dependency.cpp: implementation of the CDependency class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "Dependency.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C0DDDAA012C
CDependency::CDependency()
{

}

//##ModelId=3C0DDDAA012D
CDependency::~CDependency()
{

}

//##ModelId=3C0DDDAA0129
CDependencyVec CDependency::GetRemainings(const CDoubleQuantity *pQuantity ) const
{

	ASSERT(IsMember(pQuantity));

	CDependencyVec vec;
	for(int i=0 ; i<m_qVec.size(); i++)
	{
		if(!(pQuantity==m_qVec.at(i)))
		{
			vec.push_back(m_qVec.at(i));
		}
	}

	return vec;
}

//##ModelId=3C0DDDAA013B
bool CDependency::RemainingsUndefined(const CDoubleQuantity *pQuantity ) const
{
	CDependencyVec vec=GetRemainings(pQuantity);
	for(int i=0 ;i<vec.size();i++)
	{
		if(vec[i]->Undefined())
			return TRUE;
	}
	return FALSE;
}

//##ModelId=3C60D5640000
bool CDependency::Check() const
{
	for(int i=0 ;i<m_qVec.size();i++)
	{
		if(!m_qVec[i]->Undefined())
		{
			if(!CheckQuantity(m_qVec[i]))
		 		return FALSE;
		}
	}
	return TRUE;
}

//##ModelId=3C0DDDAA012F
bool CDependency::IsMember( const CDoubleQuantity *pQuantity) const
{
	for(int i=0 ;i<m_qVec.size();i++)
	{
		if(m_qVec[i] == pQuantity)
			return TRUE;
	}
	return FALSE;
}


