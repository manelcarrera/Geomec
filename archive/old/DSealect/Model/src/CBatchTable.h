// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBATCHTABLE_3B66B3280030_INCLUDED
#define _INC_CBATCHTABLE_3B66B3280030_INCLUDED




#include "CAnalysisPoint.h"
#include "DSResult.h"
#include "CementMaterial.h"
#include "FormationMaterial.h"
#include "CasingMaterial.h"
#include "InitialStress.h"
#include "IComponent.h"

#include<vector>
#include<list>

class CDataModel;

//##ModelId=3C8D9E0E039B
class CBatchTableRow : public IComponent
{
public:
	//##ModelId=3DDA0C4D01A9
	enum eIterationScheme
	{
		//##ModelId=3DDA0C4D01BA
		Constant=2000,
		//##ModelId=3DDA0C4D01C8
		RegNewtonRaphson,
		//##ModelId=3DDA0C4D01C9
		ModNewtonRaphson,
	};

	//##ModelId=3DDA0C4D01CA
	enum eConvergenceNorm
	{
		//##ModelId=3DDA0C4D01D9
		Force=3000,
		//##ModelId=3DDA0C4D01E8
		Displacement,
		//##ModelId=3DDA0C4D01E9
		Energy,
	};

private:
	//##ModelId=3DDA0C4C0284
	BOOL FillInterfaceList(LPCSTR FileName, std::list<long> &lstInterface);
	//##ModelId=3DDA0C4C02A3
	bool m_bDefaultSteppingScheme;
	//##ModelId=3DDA0C4C02B3
	bool m_bStopBatchRun;
	//##ModelId=3DDA0C4C02C2
	bool m_bFurtherPostProcessing;
	//bool m_bNoMonitoring;
	//##ModelId=3DDA0C4C02D2
	bool m_bSaveDiagnostic;
	//##ModelId=3DDA0C4C02D3
	long m_iTimeStepNumber;
	//##ModelId=3DDA0C4C02E2
	long m_iMaxNumberOfIterations;
	//##ModelId=3DDA0C4C02F1
	CString m_sDiagnosticFile;

	//##ModelId=3DDA0C4C0311
	eIterationScheme m_eIterationScheme;
	//##ModelId=3DDA0C4C0321
	eConvergenceNorm m_eConvergenceNorm;

	//##ModelId=3DDA0C4C0330
	void GetFemDbAndPhaseNb(int i1stDamLoadPhase,int iTable, int iMaxDamLONB, CString &s1stDamDbName, int &i1stDamPhNb);

	//##ModelId=3DDA0C4C0360
	BOOL WriteContourFemFile(LPCSTR sFileName, std::list<long> lstDebCasInterface,std::list<long> lstDebFormInterface, bool &bIsWrittenFem);
	//##ModelId=3DDA0C4C039D
	void WriteTmpContourFemStartupFile(LPCSTR sFileName);
	
	//##ModelId=3CA0119702BF
	double CalcCrackBand();
	//##ModelId=3CA01197035B
	double CalcFEMFracEnergy();
	//##ModelId=3C8D9E0E03AC
	double CalcInterfaceRelatFracEnergy(bool bInternal);
	//##ModelId=3C8D9E0E03BB
	double CalcInterfaceRelatStrength(bool bInternal);
	//##ModelId=3C8D9E0E03BD
	double CalcInterfaceConductivity(bool bInternal);
	//##ModelId=3C8D9E0E03CB
	double CalcInterfaceStiffness(bool bInternal);

	//##ModelId=3CA01198001F
	double CalcCementLinShrinkage();

	//##ModelId=3CAAA660011D
	CDataModel     *m_pDataModel;
	//##ModelId=3CAAA660014C
	mutable CAnalysisPoint *m_pAnalysisPoint;

	//##ModelId=3CAC3FED0235
	mutable CDSResult* m_pDSResult;

	//##ModelId=3CAAA660018A
	mutable CFormation     *m_pFormation;  
	//##ModelId=3CAAA66001B9
	mutable CInitialStress *m_pInitialStress;
	//##ModelId=3CAAA66001F8
	mutable CCementMaterial    *m_pCementMat;
	//##ModelId=3CAAA6600227
	mutable CFormationMaterial *m_pFormationMat;
	
	//##ModelId=3C8D9E0F010B
	int m_iScenarioNr;
	//##ModelId=3C8D9E0F011B
	BOOL m_bTail;
	//##ModelId=3C8D9E0F011C
	BOOL m_bUseForCalculation;

	//##ModelId=3DDA0C4C03BC
	BOOL m_bIsCalculated;
	//##ModelId=3DDA0C4C03CC
	BOOL m_bHasResults;
	//##ModelId=3DDA0C4C03DC
	BOOL m_bCalculationSucceeded;
	//##ModelId=3DDA0C4D0004
	std::vector< CString > m_vecContourPlotFile;

	//##ModelId=3DDA0C4D0014
	CByteArray m_ByteArray;
	
public:

	//##ModelId=3DDA0C4D0024
	CString GetDiagnosticFileName();
	//##ModelId=3DDA0C4D0032
	void ProcessDiagnosticFile(CString sWorkingDir);
	//##ModelId=3DDA0C4D0042
	void ProcessContourPlots(CString sWorkingDir);
	//##ModelId=3DDA0C4D0051
	CString GetContourFileName(int iTableNr);
	//##ModelId=3DDA0C4D0061
	void SetDiagnosticFileName(CString);

	//##ModelId=3CAC3FED0253
	void ClearResult();
	//##ModelId=3CA9A00302A2
	BOOL GetResultFromDSOFile(CString sFileName);

	//##ModelId=3DDA0C4D0071
	void SetByteArray(const CByteArray& bytearray);
	//##ModelId=3DDA0C4D0080
	void SetContourFileNames(const std::vector< CString >&);

	//##ModelId=3DDA0C4D0090
	const CByteArray& GetByteArray();
	
	//##ModelId=3CA9A0040070
	BOOL IsCalculated();
	//##ModelId=3DDA0C4D009F
	void IsCalculated(BOOL);

	//##ModelId=3DDA0C4D00AF
	BOOL CalculationSucceeded();
	//##ModelId=3DDA0C4D00B0
	void CalculationSucceeded(BOOL);

	//##ModelId=3DDA0C4D00BF
	BOOL HasResults();
	//##ModelId=3DDA0C4D00CE
	void HasResults(BOOL);
	
	//##ModelId=3C8D9E0F012A
	void UseForCalculation(BOOL newval);
	//##ModelId=3C8D9E0F013A
	BOOL UseForCalculation();
	//##ModelId=3C8D9E0F013B
	CBatchTableRow(CDataModel* pDataModel,
				   CAnalysisPoint* pAnalysisPoint,
				   int iScenarioNr);

	//##ModelId=3CA0119800BB
	void UpdateChanges();

	//##ModelId=3C8D9E0F0159
	~CBatchTableRow();

	//##ModelId=3C8D9E0F015A
	double GetDepth();

	//##ModelId=3C8D9E0F015B
	BOOL WriteMaterialFile(LPCSTR sFileName) ;
	//##ModelId=3C8D9E0F0169
	BOOL WriteAnalysisFile(LPCSTR sFileName) ;
	
	//##ModelId=3C8D9E0F016B
	BOOL IsTail() const;



	//##ModelId=3C8D9E0F0179
	int ScenarioNr();
	//##ModelId=3C8D9E0F017A
	CAnalysisPoint* AnalysisPoint() const;

	//##ModelId=3CA9A004033E
	CDSResult* DSResult() const;

	//##ModelId=3C8D9E0F017C
	CFormationMaterial *FormationMaterial() const;
	//##ModelId=3C8D9E0F017E
	CCementMaterial *CementMaterial() const;
	//##ModelId=3C8D9E0F0189
	CCasingMaterial *CasingMaterial() const;
	//##ModelId=3C8D9E0F018B
	CInitialStress *InitialStress() const; 
	//##ModelId=3C8D9E0F018D
	CFormation *Formation() const; 

	//From: Analysis
	//Label: Stop batch run if divergence
	//Descr: True if analysis is stopped after first 
	//encountered divergence.
	//##ModelId=3DDA0C4D00EE
	bool StopBatchRun() const;
	//##ModelId=3DDA0C4D00F0
	void StopBatchRun(bool bStopBatchRun);

	//From: Analysis
	//Label: Allow further post-processing
	//Descr: True if results file kept for further 
	//post-processing
	//##ModelId=3DDA0C4D00FD
	bool FurtherPostProcessing() const;
	//##ModelId=3DDA0C4D010D
	void FurtherPostProcessing(bool bFurtherPostProcessing);

	//From: Analysis
	//Label: No calculation monitoring
	//Descr: True if caculation monitor interface disabled.
	//bool NoMonitoring() const;
	//void NoMonitoring(bool bNoMonitoring);

	//From: Analysis
	//Label: Save diagnostic
	//Descr: True if diagnostid file is saved. Default=true
	//##ModelId=3DDA0C4D011C
	bool SaveDiagnostic() const;
	//##ModelId=3DDA0C4D011E
	void SaveDiagnostic(bool bSaveDiagnostic);

	//From: Stepping Scheme
	//Label: Number of time steps
	//Descr: number of time steps for application of one load
	//##ModelId=3DDA0C4D012C
	long TimeStepNumber() const;
	//##ModelId=3DDA0C4D013C
	void TimeStepNumber(long iTimeStepNumber);

	//From: Stepping Scheme
	//Label: Iteration scheme
	//Descr: iteration scheme for all non-linear analyses
	//##ModelId=3DDA0C4D014B
	eIterationScheme IterationScheme() const;
	//##ModelId=3DDA0C4D014D
	void IterationScheme(eIterationScheme iterationscheme);

	//From: Stepping Scheme
	//Label: Max. numb. iterations
	//Descr: maximum number of iterations
	//##ModelId=3DDA0C4D015B
	long MaxNumberOfIterations() const;
	//##ModelId=3DDA0C4D016B
	void MaxNumberOfIterations(long iMaxNumberOfIterations);

	//From: Stepping Scheme
	//Label: convergence norm
	//Descr: name of adopted convergence norm
	//##ModelId=3DDA0C4D017A
	eConvergenceNorm ConvergenceNorm() const;
	//##ModelId=3DDA0C4D017C
	void ConvergenceNorm(eConvergenceNorm convergencenorm);

	//From: Stepping Scheme
	//Label: Default stepping Scheme
	//Descr: True; default stepping scheme is adopted, false; 
	//access to advanced options
	//##ModelId=3DDA0C4D018A
	bool DefaultSteppingScheme() const;
	//##ModelId=3DDA0C4D0199
	void DefaultSteppingScheme(bool bDefaultSteppingScheme);

};


//##ModelId=3C0DDDAF0197
class CBatchTable : public IComponent
{
private:
	//##ModelId=3C8D9E0F01F6
	CDataModel *m_pDataModel;
	//##ModelId=3CAC3FED038E
	std::vector<CBatchTableRow*> m_BatchTable;
	
	//##ModelId=3DDA0C4D02D3
	std::vector<CBatchTableRow*> m_vecPointBatchRows;

public:

	//##ModelId=3C8D9E0F0263
	void DeleteBatchTable();

	//##ModelId=3DDA0C4D02E2
	BOOL IsCalculated() ;
	//##ModelId=3DDA0C4D02F1
	BOOL UseForCalculation() ;

	//##ModelId=3DDA0C4D02F2
	BOOL CalculationSucceeded() ;
	//##ModelId=3DDA0C4D0301
	BOOL HasResults() ;

	//##ModelId=3C8D9E0F02EF
	BOOL CanBuildBatchTable();
	//##ModelId=3C8D9E0F035D
	void Invalidate();
	//##ModelId=3C8D9E0F03CA
	BOOL IsBuild();
	//##ModelId=3C8D9E10004F
	long RowCount() {return m_BatchTable.size();}
	//##ModelId=3C8D9E1000BD
	CBatchTable(CDataModel *pDataModel);
	//##ModelId=3C8D9E100224
	~CBatchTable();
	//##ModelId=3C8D9E100292
	CBatchTableRow* GetRow(int iRow);

	//##ModelId=3DDA0C4D0310
	CBatchTableRow* GetRow(CAnalysisPoint* pAnalysisPoint, int iScenarioNr);
	//##ModelId=3DDA0C4D0321
	int GetIndex(CBatchTableRow*);

	//##ModelId=3CA011980261
	void UpdateChanges();

	//Uses combinations of Analysis Points and Cement 
	//specifications to build a matrix for presentation.
	//
	//Or, reformat table if relevant parameters have been 
	//changed (Cement scenario's, etc.).
	//
	//Returns TRUE if succeeded
	//##ModelId=3C0DDDAF01A7
	BOOL BuildBatchTable();

	//##ModelId=3DDA0C4D033F
	std::vector<CBatchTableRow*> GetResultRows(CAnalysisPoint* pAnalysisPoint);
	//##ModelId=3DDA0C4D034F
	std::vector<CBatchTableRow*> GetBatchRows(int iScenarioNr);
	
};


class CBatchTableSelection
{
public:
  CBatchTableSelection(CBatchTableRow* pSelectedRow = 0);

  CBatchTableRow* SelectedRow() const;
  void SelectRow(CBatchTableRow* pSelectedRow);

private:
  CBatchTableRow* m_pSelectedRow;
};


#endif /* _INC_CBATCHTABLE_3B66B3280030_INCLUDED */
