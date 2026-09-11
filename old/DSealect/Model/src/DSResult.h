// DSResult.h: interface for the CDSResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSRESULT_H__8C4516FE_A3EE_4D99_9DDC_491858106489__INCLUDED_)
#define AFX_DSRESULT_H__8C4516FE_A3EE_4D99_9DDC_491858106489__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern const char* LoadPhaseNames[];
extern const char* TableDamageType[];
extern const char* TypeOfDamage[];
extern const char* PlotFile[];
extern const char* FemViewDb[];

#define FORMATION_PLASTICITY_TABLE		0
#define CASING_PLASTICITY_TABLE			1
#define FORMATION_DEBONDING_TABLE		2
#define CASING_DEBONDING_TABLE			3
#define CEMENT_PLASTICITY_TABLE			4
#define CEMENT_RADIAL_CRACKING_TABLE	5

#define DRILLING		0
#define CURING			1
#define PRESSURE_TEST	2
#define COMPLETION		3
#define SHUT_IN			4
#define PRODUCTION		5
#define INJECTION		6
#define FRAC_KILLING	7
#define EVAC_UNLOAD		8

#define DRILN	1
#define CEM		2
#define CASIN	3
#define COMPL	4
#define PRODU	5
#define INJEC	6
#define FRACT	7
#define EVAC	8

#define NR_OF_TABLES 6
#define NR_OF_LOADPHASES 9
#define NR_OF_APPLIEDLOADS 24

#define CONTOUR_FILE_EXT ".dat"

class CDSResult;

//##ModelId=3CAC3FFA00EC
class CResultItem
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA00FC
	BOOL m_bExists;
	//##ModelId=3CAC3FFA010B
	BOOL m_bFailed;
	//##ModelId=3CAC3FFA010C
	double m_dDamageLevel;

public:
	//##ModelId=3CAC3FFA010D
	CResultItem();
	//##ModelId=3CAC3FFA010E
	virtual ~CResultItem();
	
	//##ModelId=3CAC3FFA011B
	BOOL Exists();
	//##ModelId=3CAC3FFA011C
	BOOL Failed();
	//##ModelId=3CAC3FFA011D
	double DamageLevel();

};


//##ModelId=3CAC3FFA014A
class CSpaceAndTimeItem : public CResultItem
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA016A
	double m_dCapacity;
public:
	//##ModelId=3CAC3FFA016B
	CSpaceAndTimeItem();
	//##ModelId=3CAC3FFA0179
	virtual ~CSpaceAndTimeItem();
	
	//##ModelId=3CAC3FFA017B
	double Capacity();
};

//##ModelId=3CAC3FFA01A8
class CSpaceItem : public CResultItem
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA01B8
	long m_lLONB;
	//##ModelId=3CAC3FFA01C7
	long m_lSTNB;
	//##ModelId=3CAC3FFA01C8
	double m_dLoadLevel;
	//##ModelId=3CAC3FFA01C9
	double m_dTime;

public:
	//##ModelId=3CAC3FFA01D6
	CSpaceItem();
	//##ModelId=3CAC3FFA01D7
	virtual ~CSpaceItem();
	
	//##ModelId=3CAC3FFA01D9
	long LONB();
	//##ModelId=3CAC3FFA01DA
	long STNB();
	//##ModelId=3CAC3FFA01E6
	double LoadLevel();
	//##ModelId=3CAC3FFA01E7
	double Time();
};


//##ModelId=3CAC3FFA0215
class CResultTable
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA0225
	BOOL m_bHasResult;
public:
	//##ModelId=3CAC3FFA0226
	CResultTable();
	//##ModelId=3CAC3FFA0227
	virtual ~CResultTable();
	
	//##ModelId=3CAC3FFA0234
	BOOL HasResults();
};


//##ModelId=3CAC3FFA0253
class CSpaceAndTimeTable: public CResultTable
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA0284
	std::vector<CSpaceAndTimeItem*> m_vecSpaceAndTimeItem;
	
public:
	//##ModelId=3CAC3FFA0292
	CSpaceAndTimeTable();
	//##ModelId=3CAC3FFA0293
	virtual ~CSpaceAndTimeTable();
	
	
	//##ModelId=3CAC3FFA02A2
	CSpaceAndTimeItem* GetLoadPhaseItem(int LoadPhase); 
};


//##ModelId=3CAC3FFA02E0
class CAppliedLoads 
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFA02F0
	CString m_sLabel;
	//##ModelId=3CAC3FFA0321
	std::vector<CSpaceItem*> m_vecSpaceItem;
	//##ModelId=3CAC3FFA032E
	int m_iAppliedLoadIndex;


public:
	

	//##ModelId=3CAC3FFA034D
	CAppliedLoads();
	//##ModelId=3CAC3FFA035D
	virtual ~CAppliedLoads();


	//##ModelId=3CAC3FFA035F
	long GetNrOfItems() {return m_vecSpaceItem.size();}
	//##ModelId=3CAC3FFA0360
	CString GetLoadLabel();
	
	//##ModelId=3CAC3FFA036D
	CSpaceItem* GetAppliedLoadItem(int Index);

	//##ModelId=3CAC3FFA036F
	int GetAppliedLoadIndex(){return m_iAppliedLoadIndex;}

	//##ModelId=3DDA0C550284
	BOOL IsOneStepLoad();

	//##ModelId=3DDA0C550293
	BOOL IsAppliedTime();

	//##ModelId=3DDA0C550294
	double GetMinValPlotX();

	//##ModelId=3DDA0C550295
	double GetMaxValPlotX();

	
	
};

//##ModelId=3CAC3FFA039C
class CLoadPhaseItems 
{
friend CDSResult;
private:
	//##ModelId=3CAC3FFA03DC
	std::vector<CAppliedLoads*> m_vecAppliedLoads;
	//##ModelId=3CAC3FFB0002
	int m_iLoadPhaseIndex;

	//##ModelId=3DDA0C550310
	int m_iMaxDamageSTNB;
	//##ModelId=3DDA0C550320
	int m_iMaxDamageLONB;
	//##ModelId=3DDA0C55032F
	double m_dMaxDamageLevel;
	


public:
	
	//##ModelId=3CAC3FFB0003
	CLoadPhaseItems();
	//##ModelId=3CAC3FFB0004
	virtual ~CLoadPhaseItems();
	
	//##ModelId=3CAC3FFB0006
	CAppliedLoads* GetAppliedLoad(int AppliedLoadNr);
	
	//##ModelId=3CAC3FFB0011
	long GetNrOfAppliedLoads(){return m_vecAppliedLoads.size();}
	//##ModelId=3CAC3FFB0012
	int GetLoadPhaseIndex(){return m_iLoadPhaseIndex;}

	//##ModelId=3DDA0C55033F
	int GetMaxDamageLONB(){return m_iMaxDamageLONB;}
	//##ModelId=3DDA0C55034F
	int GetMaxDamageSTNB(){return m_iMaxDamageSTNB;}

	//##ModelId=3DDA0C550350
	long GetNrOfLoadsToPlot();

	
};

//##ModelId=3CAC3FFB0040
class CSpaceTable : public CResultTable 
{
friend CDSResult;
private:
	//##ModelId=3CAC3FFB0071
	std::vector<CLoadPhaseItems*> m_vecLoadPhases;
	//##ModelId=3CAAB2AD01D8
	
public:
	//##ModelId=3DDA0C5503AC
	int GetFirstLoadPhaseItemIndex();

	//##ModelId=3CAC3FFB007F
	CSpaceTable();
	//##ModelId=3CAC3FFB0080
	virtual ~CSpaceTable();
	
	//##ModelId=3CAC3FFB0082
	CLoadPhaseItems* GetLoadPhaseItem(int LoadPhase);
	//##ModelId=3CAC3FFB0084
	long GetNrOfLoadPhaseItems(){return m_vecLoadPhases.size();}

};


//##ModelId=3CAC3FFB00AE
class CSpace
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFB010D
	std::vector<CSpaceTable*> m_vecSpaceTable;
	
public:
	//##ModelId=3CAC3FFB011B
	CSpace();
	//##ModelId=3CAC3FFB011C
	virtual ~CSpace();

	//##ModelId=3CAC3FFB011E
	CSpaceTable* GetSpaceTable(int eTableID);
	//##ModelId=3DDA0C560022
	BOOL AllTablesEmpty();
	
};

//##ModelId=3CAC3FFB0169
class CSpaceAndTime  
{
friend CDSResult;
private: 
	//##ModelId=3CAC3FFB019A
	std::vector<CSpaceAndTimeTable*> m_vecSpaceAndTimeTable;
	
public:
	//##ModelId=3CAC3FFB01A8
	CSpaceAndTime();
	//##ModelId=3CAC3FFB01A9
	virtual ~CSpaceAndTime();
	
	//##ModelId=3CAC3FFB01B7
	CSpaceAndTimeTable* GetSpaceAndTimeTable(int eTableID);
};

//##ModelId=3CAC3FFB01F6
class CDSResult  
{
private:
	//##ModelId=3DDA0C56014B
	long m_iNRadCrack;
	//##ModelId=3DDA0C56015B
	void FindDataForOverview();
	//##ModelId=3DDA0C56016A
	void FindSpaceDataForOverview();
	
	//##ModelId=3DDA0C56016B
	void FindCapForRisk();
	//##ModelId=3CAC3FFB0216
	CSpaceAndTime m_SpaceAndTime;
	//##ModelId=3CAC3FFB0226
	CSpace m_Space;

	//##ModelId=3DDA0C56018D
	std::vector<double> m_vecMaxCapAtLoadPhase;
			
	//##ModelId=3DDA0C56019A
	std::vector< std::vector<BOOL> > m_vecFailedTableLoadphase;
	//##ModelId=3DDA0C5601B9
	std::vector< std::vector<double> > m_vecCapacityTableLoadphase;
	//##ModelId=3DDA0C5601BE
	std::vector< std::vector<double> > m_vecDamageLevelTableLoadphase;

	//##ModelId=3DDA0C5601D9
	std::vector< std::vector<int> > m_vecSpaceLONBTableLoadphase;
	//##ModelId=3DDA0C5601E8
	std::vector< std::vector<int> > m_vecSpaceSTNBTableLoadphase;
	//##ModelId=3DDA0C5601F8
	std::vector< std::vector<double> > m_vecSpaceLoadLevelTableLoadphase;
	//##ModelId=3DDA0C560208
	std::vector< std::vector<double> > m_vecSpaceTimeTableLoadphase;
	//##ModelId=3DDA0C560217
	std::vector< std::vector<BOOL> > m_vecSpaceFailedTableLoadphase;
	//##ModelId=3DDA0C560227
	std::vector< std::vector<double> > m_vecSpaceDamageLevelTableLoadphase;

public:
	//##ModelId=3DDA0C560235
	long NRadCrack();

	//##ModelId=3CAC3FFB0234
	CDSResult();
	//##ModelId=3CAC3FFB0235
	virtual ~CDSResult();
	//##ModelId=3CAC3FFB0237
	CSpaceAndTime*  SpaceAndTime(){return &m_SpaceAndTime;}
	//##ModelId=3CAC3FFB0238
	CSpace*         Space(){return &m_Space;}
	//##ModelId=3CAC3FFB0244
	BOOL			ReadDSOFile(LPCSTR FileName);
	//##ModelId=3CAC3FFB0246
	void			ReadSpaceAndTimeData(CStdioFile* sfMST);
	//##ModelId=3CAC3FFB0248
	void			ReadSpaceData(CStdioFile* sfMS);

	//##ModelId=3DDA0C560245
	double			GetSealantRiskCapacityAtLoadPhase(int iLoadPhase);
	
	//##ModelId=3DDA0C560255
	BOOL			GetFailedAtTableAndLoadphase(int iTable, int iLoadphase);
	//##ModelId=3DDA0C560284
	double			GetDamageLevelAtTableAndLoadphase(int iTable, int iLoadphase);
	//##ModelId=3DDA0C5602A3
	double			GetCapacityAtTableAndLoadphase(int iTable, int iLoadphase);

	
};

#endif // !defined(AFX_DSRESULT_H__8C4516FE_A3EE_4D99_9DDC_491858106489__INCLUDED_)
