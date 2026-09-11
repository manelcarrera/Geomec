// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
 

#include "CDependenciesManager.h"



//##ModelId=3C0DDDAE0100
CDependenciesManager::CDependenciesManager()
{
	m_UnitSystem = QU::SI_USER_UNIT;
}

//##ModelId=3C0DDDAE010A
CDependenciesManager::~CDependenciesManager()
{
}

//##ModelId=3C0DDDAE010B
void CDependenciesManager::AddDependency(const CDependency* pDependency)
{
	m_DependencyVec.push_back(pDependency);
}

//##ModelId=3C0DDDAE010D
void CDependenciesManager::RemoveDependency(const CDependency* pDependency)
{
	for(int i=0 ; i<m_DependencyVec.size() ;i++)
	{
		if(m_DependencyVec[i] == pDependency)
		{
			RemoveDependency(i);
			return;
		}
	}

	ASSERT(FALSE);
	return;
}

//##ModelId=3C0DDDAE010F
void CDependenciesManager::RemoveDependency(int Index)
{
	ASSERT(Index >=0 && Index < m_DependencyVec.size());
	m_DependencyVec.erase(m_DependencyVec.begin() + Index);
}


//##ModelId=3C0DDDAE011A
bool CDependenciesManager::CheckQuantity(const CDoubleQuantity* pQuantity, std::string &strDependency) const
{
	for(int i=0 ; i<m_DependencyVec.size() ;i++)
	{
		if(m_DependencyVec[i]->IsMember(pQuantity))
		{
			if(!(m_DependencyVec[i]->CheckQuantity(pQuantity)))
			{
				strDependency=m_DependencyVec[i]->GetDependencyString();
				return FALSE;
			}
		}
	}

	return TRUE;
}

//##ModelId=3C60D56900EB
std::vector<std::string> CDependenciesManager::CheckAllDependencies()
{
	std::vector<std::string> strVec;
	for(int i=0 ; i<m_DependencyVec.size() ;i++)
	{
		if(!m_DependencyVec[i]->Check())
		{
			strVec.push_back(m_DependencyVec[i]->GetDependencyString());
		}
	}
	return strVec;
}

//##ModelId=3DDA0C4602F1
QU::UNIT CDependenciesManager::GetUnit()
{
	
	return m_UnitSystem;

}

//##ModelId=3DDA0C4602F2
void CDependenciesManager::SetUnit(QU::UNIT us)
{
	m_UnitSystem = us;
}
