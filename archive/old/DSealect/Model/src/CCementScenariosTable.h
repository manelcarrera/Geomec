// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCEMENTSCENARIOSTABLE_3B68E8D3037B_INCLUDED
#define _INC_CCEMENTSCENARIOSTABLE_3B68E8D3037B_INCLUDED




#include "IComponent.h"
#include "CementScenario.h"
#include <vector>



//##ModelId=3C0DDDAE01C6
class CCementScenariosTable 
: public IComponent
{
private:


public:
	
	//##ModelId=3C0DDDAE01E6
	CCementScenariosTable();
	//##ModelId=3C0DDDAE01E7
	long NumberOfCementScenarios() const;
	//##ModelId=3C0DDDAE01E9
	void NumberOfCementScenarios(long iNumberOfCementScenarios);
	//##ModelId=3C2043F403D9
	bool RemoveAllScenarios();
	//##ModelId=3C2043F5002F
	CCementScenario* CementScenarioAt(long Index);
	//##ModelId=3C6A14BC0194
	long GetScenarioIndex(CCementScenario *pScenario);

	//##ModelId=3C2043F5010A
	bool CheckComplete() const;

private:
	
	
	//##ModelId=3CAC3FEC0013
	std::vector<CCementScenario> m_ScenarioTable;


};

#endif /* _INC_CCEMENTSCENARIOSTABLE_3B68E8D3037B_INCLUDED */
