// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER


#include "CCementScenariosTable.h"


//##ModelId=3C0DDDAE01E6
CCementScenariosTable::CCementScenariosTable()
{
	NumberOfCementScenarios(1);

}

//##ModelId=3C2043F5002F
CCementScenario* CCementScenariosTable::CementScenarioAt(long Index)
{
	ASSERT( Index>=0 && Index<m_ScenarioTable.size());
	return &m_ScenarioTable[Index];
}

//##ModelId=3C0DDDAE01E7
long CCementScenariosTable::NumberOfCementScenarios() const
{
	return m_ScenarioTable.size();	
}

//##ModelId=3C0DDDAE01E9
void CCementScenariosTable::NumberOfCementScenarios(long iNumberOfCementScenarios)
{
	m_ScenarioTable.resize(iNumberOfCementScenarios);
}


//##ModelId=3C2043F403D9
bool CCementScenariosTable::RemoveAllScenarios()
{
	m_ScenarioTable.clear();
	return true;
}

//##ModelId=3C2043F5010A
bool CCementScenariosTable::CheckComplete() const
{
	if(NumberOfCementScenarios()<1)
		return false;
	
	return true;
}

//##ModelId=3C6A14BC0194
long CCementScenariosTable::GetScenarioIndex(CCementScenario *pScenario)
{

	for(long i=0; i<NumberOfCementScenarios();i++)
	{
		if (pScenario == CementScenarioAt(i))
			return i;
	}
	return -1;

}
