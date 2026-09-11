// DSResult.cpp: implementation of the CDSResult class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "StringUtils.h"
#include "Global.h"
#include "CDataModel.h"
#include "DSResult.h"


#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* TableNames[]=
{
	"FORMATION_PLASTICITY_TABLE",
	"CASING_PLASTICITY_TABLE",
	"FORMATION_DEBONDING_TABLE",
	"CASING_DEBONDING_TABLE",
	"CEMENT_PLASTICITY_TABLE",
	"CEMENT_RADIAL_CRACKING_TABLE"
	//if you add or remove; change define NR_OF_TABLES in header too
};

const char* TableDamageType[]=
{
	"Shear deterioration",
	"Plastic deformation",
	"Debonding at formation",
	"Debonding at casing",
	"Shear deterioration",
	"Radial cracks"
	//corresponds to table-names
};

const char* TypeOfDamage[]=
{
	"shear deterioration in formation",
	"plastic deformation in casing",
	"debonding at formation",
	"debonding at casing",
	"shear deterioration in cement",
	"radial cracks in cement"
};

const char* PlotFile[]=
{
	"foreqplast",
	"caseqplast",
	"fordeb",
	"casdeb",
	"cemeqplast",
	"radcrack"

};

const char* FemViewDb[]=
{
	"dummy",//not a 0 numbered Database corresponding to  phase number
	"DRILN",
	"CEMEN",
	"CASIN",
	"COMPL",
	"PRODU",
	"INJEC",
	"FRACT",
	"EVACU"
};

const char* LoadPhaseNames[]=
{
	"Drilling",
	"Curing",
	"Pressure_test",
	"Completion",
	"Shut-in",
	"Production",
	"Injection",
	"Frac-Killing",
	"Evac-Unload"
	//if you add or remove; change define NR_OF_LOADPHASES in header too

};

static const char* AppliedLoadNames[]=
{
	"Rock_insitu_stress",			//0
	"Mud_pressure_on_borehole",		//1
	"Slurry_press_on_borehole",		//2
	"Cement_initial_stress",		//3
	"Cement_confin_pressure",		//4
	"Casing_initial_stress",		//5
	"Mud_pressure_on_casing",		//6
	"Slurry_pressure_on_casing",	//7
	"Assembly",						//8
	"Test_pressure",				//9
	"Completion_pressure",			//10
	"Shut-in_pressure",				//11
	"Production_pressure",			//12
	"Injection_pressure",			//13
	"Fract-killing_pressure",		//14
	"Evac-unloading_pressure",		//15
	"End_of_shut-in",				//16

	"Drilling_fluid_temperat",		//17 Applied time
	"Cement_shrinkage",				//18 Applied time
	"Shut-in_time",					//19 Applied time
	"Production_flowing_temper",	//20 Applied time
	"Injection_flowing_temper",		//21 Applied time
	"Fract-killing_temperature",	//22 Applied time
	"Evac-unloading_temper"			//23 Applied time

	//if you add or remove; change define NR_OF_APPLIEDLOADS in header too

};


//##ModelId=3CAC3FFB0234
CDSResult::CDSResult()
{
	
}

//##ModelId=3CAC3FFB0235
CDSResult::~CDSResult()
{

}

//##ModelId=3CAC3FFB0244
BOOL CDSResult::ReadDSOFile(LPCSTR FileName)
{
	CStdioFile vFile;

	// When the given file can be opened
	if(vFile.Open(FileName, CFile::modeRead | CFile::shareDenyWrite))
	{
		// Read all the lines (one by one)
		m_iNRadCrack = -1; //default = -1
		CString sLine;
		while(vFile.ReadString(sLine))
		{
			if(sLine.Find("FEMGV_Radial_Crack") != -1) //found
			{
				std::vector<CString> results;
				sLine.TrimLeft(); //remove spaces from begin of string
				if(StringUtils::SplitString(sLine, " ", results)) 
				{
					//results[0] = "FEMGV_Radial_Crack:"
					char* sDum;
					m_iNRadCrack = strtol(results[1],&sDum,0);
				}
			}

			if(sLine.Find("MAX_SPACE_TIME_BEGIN") != -1) //found
			{
				ReadSpaceAndTimeData(&vFile);
			}

			if(sLine.Find("MAX_SPACE_BEGIN") != -1) //found
			{
				ReadSpaceData(&vFile);
			}
		}		
		FindCapForRisk();
		FindDataForOverview();
		return TRUE;
		
	}
	else
      return FALSE;
	
}

//##ModelId=3CAC3FFB0246
void CDSResult::ReadSpaceAndTimeData(CStdioFile* sfMST)
{
	CString sLine;
	while(sfMST->ReadString(sLine))
    {
		if(sLine.Find("MAX_SPACE_TIME_END") == -1) //not found
		{
			for (int i = 0; i < NR_OF_TABLES; i++)
			{
				if(sLine.Find(CString(TableNames[i]) + "_BEGIN") != -1) // "TableName"_BEGIN found
				{
					while(sfMST->ReadString(sLine))
					{
						if(sLine.Find("TABLE_END") == -1) //not found
						{
							for (int j = 0; j < NR_OF_LOADPHASES; j++) 
							{
								if(sLine.Find(LoadPhaseNames[j]) != -1) // LoadName found
								{
									CSpaceAndTimeTable* pSTT = SpaceAndTime()->GetSpaceAndTimeTable(i);
									pSTT->m_bHasResult = TRUE; 
									
									pSTT->GetLoadPhaseItem(j)->m_bExists = TRUE;

									std::vector<CString> results;
									sLine.TrimLeft(); //remove spaces from begin of string
									if(StringUtils::SplitString(sLine, " ", results)) 
									{
										//results[0] = loadphasename
										if(results[1] == "T")
										{
											pSTT->GetLoadPhaseItem(j)->m_bFailed = TRUE;
										}
										else
										{
											pSTT->GetLoadPhaseItem(j)->m_bFailed = FALSE;
										}
										
										char* sDum;
										pSTT->GetLoadPhaseItem(j)->m_dCapacity = strtod(results[2],&sDum);
										pSTT->GetLoadPhaseItem(j)->m_dDamageLevel = strtod(results[3],&sDum);

									}
								}
							}
							
						}
						else //break while-loop if TABLE_END found
						{
							break; 
						}
					}
				}
			}
		}
		else //return if MAX_SPACE_TIME_END found
		{
			return; 
		}
	}

}

//##ModelId=3DDA0C560255
BOOL CDSResult::GetFailedAtTableAndLoadphase(int iTable, int iLoadphase)
{
	//returns bool Failed for a loadphase in a table
	return m_vecFailedTableLoadphase[iTable][iLoadphase];
}

//##ModelId=3DDA0C5602A3
double CDSResult::GetCapacityAtTableAndLoadphase(int iTable, int iLoadphase)
{
	//returns double capacity for a loadphase in a table
	return m_vecCapacityTableLoadphase[iTable][iLoadphase];
	
}

//##ModelId=3DDA0C560284
double CDSResult::GetDamageLevelAtTableAndLoadphase(int iTable, int iLoadphase)
{
	//returns double damage level for a loadphase in a table
	return m_vecDamageLevelTableLoadphase[iTable][iLoadphase];
	
}

//##ModelId=3DDA0C56015B
void CDSResult::FindDataForOverview()
{
	//find data for each loadphase in each table and store in vectors

	m_vecFailedTableLoadphase.clear();
	m_vecCapacityTableLoadphase.clear();
	m_vecDamageLevelTableLoadphase.clear();

	m_vecFailedTableLoadphase.resize(NR_OF_TABLES);
	m_vecCapacityTableLoadphase.resize(NR_OF_TABLES);
	m_vecDamageLevelTableLoadphase.resize(NR_OF_TABLES);

	for(int b = 0; b < m_vecFailedTableLoadphase.size(); b++)
	{
		m_vecFailedTableLoadphase[b].resize(NR_OF_LOADPHASES);
		m_vecCapacityTableLoadphase[b].resize(NR_OF_LOADPHASES);
		m_vecDamageLevelTableLoadphase[b].resize(NR_OF_LOADPHASES);
	}


	for(int i = 0 ; i < NR_OF_TABLES ; i++)
	{
		for(int j = 0 ; j < NR_OF_LOADPHASES ; j++)
		{
			m_vecCapacityTableLoadphase[i][j] = -1; //default = -1
			m_vecDamageLevelTableLoadphase[i][j] = -1; //default=-1
			if(SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Exists())
			{
				m_vecFailedTableLoadphase[i][j] = SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Failed();
				m_vecCapacityTableLoadphase[i][j] = SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Capacity();
				m_vecDamageLevelTableLoadphase[i][j]= SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->DamageLevel();
			}
		}
	}
}

//##ModelId=3DDA0C56016A
void CDSResult::FindSpaceDataForOverview()
{
	//find data for each loadphase in each table and store in vectors
	m_vecSpaceLONBTableLoadphase.clear();
	m_vecSpaceSTNBTableLoadphase.clear();
	m_vecSpaceLoadLevelTableLoadphase.clear();
	m_vecSpaceTimeTableLoadphase.clear();
	m_vecSpaceFailedTableLoadphase.clear();
	m_vecSpaceDamageLevelTableLoadphase.clear();

	m_vecSpaceLONBTableLoadphase.resize(NR_OF_TABLES);
	m_vecSpaceSTNBTableLoadphase.resize(NR_OF_TABLES);
	m_vecSpaceLoadLevelTableLoadphase.resize(NR_OF_TABLES);
	m_vecSpaceTimeTableLoadphase.resize(NR_OF_TABLES);
	m_vecSpaceFailedTableLoadphase.resize(NR_OF_TABLES);
	m_vecSpaceDamageLevelTableLoadphase.resize(NR_OF_TABLES);

	
	int iNrApplLoads = Space()->GetSpaceTable(0)->GetLoadPhaseItem(0)->GetNrOfAppliedLoads();
	int iNrOfItems;
	for(int a = 0 ; a < iNrApplLoads ; a++)
	{
		iNrOfItems = Space()->GetSpaceTable(0)->GetLoadPhaseItem(0)->GetAppliedLoad(a)->GetNrOfItems();
		for(int b = 0 ; b < iNrOfItems ; b++)
		{
			//Space()->GetSpaceTable(0)->GetLoadPhaseItem(0)->GetAppliedLoad(a)->GetAppliedLoadItem(b)
		}
	}

	for(int b = 0; b < m_vecSpaceLONBTableLoadphase.size(); b++)
	{
	//	m_vecSpaceLONBTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);
	//	m_vecSpaceSTNBTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);
	//	m_vecSpaceLoadLevelTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);
	//	m_vecSpaceTimeTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);
	//	m_vecSpaceFailedTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);
	//	m_vecSpaceDamageLevelTableLoadphase[b].resize(TOTAL_NR_OF_ITEMS);

	}

	for(int i = 0 ; i < NR_OF_TABLES ; i++)
	{
		for(int j = 0 ; j < NR_OF_LOADPHASES ; j++)
		{
			m_vecCapacityTableLoadphase[i][j] = -1; //default = -1
			m_vecDamageLevelTableLoadphase[i][j] = -1; //default=-1
			if(SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Exists())
			{
				m_vecFailedTableLoadphase[i][j] = SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Failed();
				m_vecCapacityTableLoadphase[i][j] = SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Capacity();
				m_vecDamageLevelTableLoadphase[i][j]= SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->DamageLevel();
			}
		}
	}
}

//##ModelId=3DDA0C560245
double CDSResult::GetSealantRiskCapacityAtLoadPhase(int iLoadPhase)
{	
	//returns double max Capacity for a Loadphase in rest of (=4 latest) tables
	return m_vecMaxCapAtLoadPhase[iLoadPhase];
}


//##ModelId=3DDA0C56016B
void CDSResult::FindCapForRisk()
{
	//find in table 'form.debonding' to 'nrOfTables' the maximum capacity per Loadphase and store in vector
	//find in all tables the capacity per Loadphase and store in vector

	m_vecMaxCapAtLoadPhase.clear();
	
	m_vecMaxCapAtLoadPhase.resize(NR_OF_LOADPHASES);

	for(int h = 0 ; h < NR_OF_LOADPHASES ; h++)
	{
		m_vecMaxCapAtLoadPhase[h] = -1.0; //fill default with -1.0
	}
	
	for(int i = 0 ; i < NR_OF_TABLES ; i++)
	{
		if(SpaceAndTime()->GetSpaceAndTimeTable(i)->HasResults())
		{
			for(int j = 0 ; j < NR_OF_LOADPHASES ; j++)
			{
				if(SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Exists())
				{
					if(i != FORMATION_PLASTICITY_TABLE && i != CASING_PLASTICITY_TABLE)
					{
						if(m_vecMaxCapAtLoadPhase[j] < SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Capacity())
						{
							m_vecMaxCapAtLoadPhase[j] = SpaceAndTime()->GetSpaceAndTimeTable(i)->GetLoadPhaseItem(j)->Capacity();
						}
						
					}
				}
			}
		}
	}

}

//##ModelId=3CAC3FFB0248
void CDSResult::ReadSpaceData(CStdioFile* sfMS)
{
	int iTableNr;
	int iLoadPhaseNr;
	int iAppliedLoadNr;
	int iRememberAppLoad;
	
	//int iApplLoadCount=0;

	CString sLine;
	while(sfMS->ReadString(sLine))
    {
		sLine.TrimLeft();
		sLine.TrimRight();

		if(sLine.IsEmpty())
			continue;

		if(sLine.Find("*") != -1) //"*" found
			continue;

		if(sLine.Find("MAX_SPACE_END") !=-1) // "MAX_SPACE_END" found
			return;

		if(sLine.Find("TABLE_END") != -1) //"TABLE_END" found
			continue;
				
		if(sLine.Find("TABLE_BEGIN") != -1) // "TABLE_BEGIN" found
		{
			iTableNr = -1;
			for (int i = 0; i < NR_OF_TABLES; i++)
			{
				if(sLine.Find(CString(TableNames[i])) != -1) // TableName found
				{
					iTableNr = i;
					break;
				}
			}
			continue;
		}
		assert(iTableNr != -1);
		
		if(sLine.Find("NONE") != -1) //"NONE" found
		{
			Space()->GetSpaceTable(iTableNr)->m_bHasResult = FALSE;
			continue;
		}
			
		if(sLine.Find("LOAD_PHASE") != -1) //"LOAD_PHASE" found
		{
			Space()->GetSpaceTable(iTableNr)->m_bHasResult = TRUE;
			iLoadPhaseNr = -1;
			for (int j = 0; j < NR_OF_LOADPHASES; j++) 
			{
				if(sLine.Find(LoadPhaseNames[j]) != -1) // LoadPhaseName found
				{
					iRememberAppLoad = -1;
					iLoadPhaseNr = j;
					break;
				}
			}
			assert(iLoadPhaseNr != -1);
			CLoadPhaseItems* pLoadPhaseItem = new CLoadPhaseItems;
			pLoadPhaseItem->m_iLoadPhaseIndex = iLoadPhaseNr;
			Space()->GetSpaceTable(iTableNr)->m_vecLoadPhases.push_back(pLoadPhaseItem);
			continue;
		}
		
		//Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(iLoadPhaseNr)->m_iFirstDamageApplLoadIndex = -1;

		for (int k = 0; k < NR_OF_APPLIEDLOADS; k++) 
		{
			if(sLine.Find(AppliedLoadNames[k]) != -1) // AppliedLoadName found
			{
				iAppliedLoadNr = k;
			
				
				int a; //find LoadPhaseItem with right LoadPhaseIndex
				for(a = 0 ; a < Space()->GetSpaceTable(iTableNr)->m_vecLoadPhases.size() ; a++)
				{
					if(iLoadPhaseNr == Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->GetLoadPhaseIndex())
					{
						break; //keep a
					}
				}

				if(iRememberAppLoad != k) // not the same AppliedLoadName
				{
					CAppliedLoads* pAppLoads = new CAppliedLoads;
					pAppLoads->m_sLabel = AppliedLoadNames[k];
					pAppLoads->m_iAppliedLoadIndex = iAppliedLoadNr;

					Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->m_vecAppliedLoads.push_back(pAppLoads);
					
					iRememberAppLoad = k;
				}
				

				int b; //find AppliedLoad with right AppliedLoadIndex
				for(b = 0 ; b < Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->m_vecAppliedLoads.size() ; b++)
				{
					if(iAppliedLoadNr == Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->GetAppliedLoad(b)->GetAppliedLoadIndex())
					{
						break; //keep b
					}
				}

				CSpaceItem* pSpaceItem = new CSpaceItem;
				Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->GetAppliedLoad(b)->m_vecSpaceItem.push_back(pSpaceItem);
				
				int iALIndex = Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->GetAppliedLoad(b)->GetAppliedLoadIndex();

				std::vector<CString> results;
				sLine.TrimLeft(); //remove spaces from begin of string
				if(StringUtils::SplitString(sLine, " ", results)) 
				{
					//results[0] = appliedloadname
					char* sDum;
					pSpaceItem->m_lLONB = strtol(results[1],&sDum,0);
					pSpaceItem->m_lSTNB = strtol(results[2],&sDum,0);

					pSpaceItem->m_dLoadLevel = strtod(results[3],&sDum);
					pSpaceItem->m_dTime = strtod(results[4],&sDum);

					if(results[5] == "T")
					{
						pSpaceItem->m_bFailed = TRUE;
												
					}
					else
					{
						pSpaceItem->m_bFailed = FALSE;
					}
																																	
					pSpaceItem->m_dDamageLevel = strtod(results[6],&sDum);
					if(pSpaceItem->m_dDamageLevel > Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->m_dMaxDamageLevel)
					{
						Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->m_iMaxDamageLONB = pSpaceItem->m_lLONB;
						Space()->GetSpaceTable(iTableNr)->GetLoadPhaseItem(a)->m_iMaxDamageSTNB = pSpaceItem->m_lSTNB;
					}

				}

				break;
			}

		}
	}									
}

//##ModelId=3CAC3FFA010D
CResultItem::CResultItem()
{
	m_bExists = FALSE;
}

//##ModelId=3CAC3FFA010E
CResultItem::~CResultItem()
{
}

//##ModelId=3CAC3FFA011B
BOOL CResultItem::Exists()
{
	return m_bExists;
}

//##ModelId=3CAC3FFA011C
BOOL CResultItem::Failed()
{
	return m_bFailed; // TRUE;
}

//##ModelId=3CAC3FFA011D
double CResultItem::DamageLevel()
{
	return m_dDamageLevel;
}

//##ModelId=3CAC3FFA016B
CSpaceAndTimeItem::CSpaceAndTimeItem()
{

}

//##ModelId=3CAC3FFA0179
CSpaceAndTimeItem::~CSpaceAndTimeItem()
{

}
	
//##ModelId=3CAC3FFA017B
double CSpaceAndTimeItem::Capacity()
{
	return m_dCapacity;
}

//##ModelId=3CAC3FFA01D6
CSpaceItem::CSpaceItem()
{
	
}

//##ModelId=3CAC3FFA01D7
CSpaceItem::~CSpaceItem()
{
	
}

//##ModelId=3CAC3FFA01D9
long CSpaceItem::LONB()
{
	return m_lLONB;
}

//##ModelId=3CAC3FFA01DA
long CSpaceItem::STNB()
{
	return m_lSTNB;
}

//##ModelId=3CAC3FFA01E6
double CSpaceItem::LoadLevel()
{
	return m_dLoadLevel;
}

//##ModelId=3CAC3FFA01E7
double CSpaceItem::Time()
{
	return m_dTime;
}

//##ModelId=3CAC3FFA0226
CResultTable::CResultTable()
{
	m_bHasResult=FALSE;
}

//##ModelId=3CAC3FFA0227
CResultTable::~CResultTable()
{
}

//##ModelId=3CAC3FFA0234
BOOL CResultTable::HasResults()
{
	return m_bHasResult;
}


//##ModelId=3CAC3FFA02A2
CSpaceAndTimeItem* CSpaceAndTimeTable::GetLoadPhaseItem(int LoadPhase)
{
	return m_vecSpaceAndTimeItem[LoadPhase];
	
}

//##ModelId=3CAC3FFA0292
CSpaceAndTimeTable::CSpaceAndTimeTable()
{
	for(int i = 0 ; i<=NR_OF_LOADPHASES ;i++)
	{
		CSpaceAndTimeItem* pItem = new CSpaceAndTimeItem;
		m_vecSpaceAndTimeItem.push_back(pItem);
	}
}


//##ModelId=3CAC3FFA0293
CSpaceAndTimeTable::~CSpaceAndTimeTable()
{
	for(int i = 0 ; i<=NR_OF_LOADPHASES ;i++)
	{
		delete m_vecSpaceAndTimeItem[i];
	}
}

  

//##ModelId=3CAC3FFA034D
CAppliedLoads::CAppliedLoads()
{

}

//##ModelId=3CAC3FFA035D
CAppliedLoads::~CAppliedLoads()
{
	for(int i = 0 ; i < m_vecSpaceItem.size() ; i++)
	{
		delete m_vecSpaceItem[i];
	}
}


//##ModelId=3CAC3FFA0360
CString CAppliedLoads::GetLoadLabel()
{
	return m_sLabel;
}

//##ModelId=3DDA0C550284
BOOL CAppliedLoads::IsOneStepLoad()
{
	if(GetLoadLabel() == AppliedLoadNames[6])	//Mud_pressure_on_casing
		return TRUE;
	if(GetLoadLabel() == AppliedLoadNames[8])	//Assembly
		return TRUE;
	if(GetLoadLabel() == AppliedLoadNames[16])	//End_of_shut-in
		return TRUE;
	
	return FALSE;

}

//##ModelId=3DDA0C550293
BOOL CAppliedLoads::IsAppliedTime()
{
	if(GetAppliedLoadIndex() >= 17)
		return TRUE;

	return FALSE;
}

//##ModelId=3DDA0C550294
double CAppliedLoads::GetMinValPlotX()
{
	if(IsAppliedTime())
	{
		return 0.0;
	}
	else
	{
		return GetAppliedLoadItem(0)->LoadLevel();
	}
}

//##ModelId=3DDA0C550295
double CAppliedLoads::GetMaxValPlotX()
{
	if(IsAppliedTime())
	{
		return GetAppliedLoadItem(GetNrOfItems()-1)->Time();
	}
	else
	{
		return GetAppliedLoadItem(GetNrOfItems()-1)->LoadLevel();
	}
}

//##ModelId=3CAC3FFA036D
CSpaceItem* CAppliedLoads::GetAppliedLoadItem(int Index)
{
	return m_vecSpaceItem[Index];
}


//##ModelId=3CAC3FFB007F
CSpaceTable::CSpaceTable()
{

}

//##ModelId=3CAC3FFB0080
CSpaceTable::~CSpaceTable()
{
	for(int i = 0 ; i < m_vecLoadPhases.size() ; i++)
	{
		delete m_vecLoadPhases[i];
	}
}

//##ModelId=3CAC3FFB0003
CLoadPhaseItems::CLoadPhaseItems()
{
	m_iMaxDamageLONB = -1;
	m_iMaxDamageSTNB = -1;
	m_dMaxDamageLevel = -1.0;
	
}

//##ModelId=3CAC3FFB0004
CLoadPhaseItems::~CLoadPhaseItems()
{
	for(int i = 0 ; i < m_vecAppliedLoads.size() ; i++)
	{
		delete m_vecAppliedLoads[i];
	}
}

//##ModelId=3CAC3FFB0082
CLoadPhaseItems* CSpaceTable::GetLoadPhaseItem(int LoadPhase)
{
	return m_vecLoadPhases[LoadPhase]; 
}

//##ModelId=3CAC3FFB0006
CAppliedLoads* CLoadPhaseItems::GetAppliedLoad(int AppliedLoadNr)
{
	return m_vecAppliedLoads[AppliedLoadNr];
}

//##ModelId=3DDA0C550350
long CLoadPhaseItems::GetNrOfLoadsToPlot()
{
	//returns the number of appliedloads, except the specified 1step-loads 
	long lngCount = 0;
	for(int i = 0 ; i < GetNrOfAppliedLoads() ; i++)
	{
		if(!GetAppliedLoad(i)->IsOneStepLoad())
			lngCount++;
	}
	return lngCount;
}

//##ModelId=3CAC3FFB011B
CSpace::CSpace()
{
	for(int i = 0 ; i<=NR_OF_TABLES ;i++)
	{
		CSpaceTable* pTable = new CSpaceTable;
		m_vecSpaceTable.push_back(pTable);
	}

}

//##ModelId=3CAC3FFB011C
CSpace::~CSpace()
{
	for(int i = 0 ; i<=NR_OF_TABLES ;i++)
	{
		delete	m_vecSpaceTable[i];
	}
}

//##ModelId=3CAC3FFB011E
CSpaceTable* CSpace::GetSpaceTable(int eTableID)
{
	return m_vecSpaceTable[eTableID];
}

//##ModelId=3DDA0C560022
BOOL CSpace::AllTablesEmpty()
{
	for(int iTable = 0 ; iTable< NR_OF_TABLES ; iTable++)
	{
		if(m_vecSpaceTable[iTable]->HasResults())
			return FALSE;
	}
	return TRUE;

}
//##ModelId=3CAC3FFB01A8
CSpaceAndTime::CSpaceAndTime()  
{
	for(int i = 0 ; i<=NR_OF_TABLES ;i++)
	{
		CSpaceAndTimeTable* pTable = new CSpaceAndTimeTable;
		m_vecSpaceAndTimeTable.push_back(pTable);
	}
}

//##ModelId=3CAC3FFB01A9
CSpaceAndTime::~CSpaceAndTime()  
{
	for(int i = 0 ; i<= NR_OF_TABLES ;i++)
	{
		delete	m_vecSpaceAndTimeTable[i];
	}

}

//##ModelId=3CAC3FFB01B7
CSpaceAndTimeTable* CSpaceAndTime::GetSpaceAndTimeTable(int eTableID)
{
	return m_vecSpaceAndTimeTable[eTableID];
}


//##ModelId=3DDA0C5503AC
int CSpaceTable::GetFirstLoadPhaseItemIndex()
{
	if(HasResults())
	{
		return GetLoadPhaseItem(0)->GetLoadPhaseIndex();
	}
	else
	{
		return -1;
	}
	
}


//##ModelId=3DDA0C560235
long CDSResult::NRadCrack()
{
	return m_iNRadCrack;
}
