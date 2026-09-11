// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER


#include "CAnalysisPoint.h"
#include "DSModelDatabase.h"
#include "BatchTableTable.h"
#include "CDataModel.h"
#include "StringUtils.h"
#include "Global.h"
#include "ReadWriteBlob.h"
#include "CBatchTable.h"
#include "MF.h"
#include "CAnalysis.h"
#include "CFormationTopography.h"
#include "MeshParam.h"

#define FGV_EXTENSION ".V72"
#define DIAGNOSTIC_FILE   "dsealect.out"

//##ModelId=3C8D9E100224
CBatchTable::~CBatchTable()
{
	DeleteBatchTable();
}

//##ModelId=3C8D9E1000BD
CBatchTable::CBatchTable(CDataModel *pDataModel)
{
	m_pDataModel=pDataModel;
}

//##ModelId=3C8D9E100292
CBatchTableRow* CBatchTable::GetRow(int iRow)
{
	return m_BatchTable[iRow];
}

//##ModelId=3DDA0C4D0310
CBatchTableRow* CBatchTable::GetRow(CAnalysisPoint* pAnalysisPoint, int iScenarioNr)
{
	int j = 0;
	for(; j < RowCount() ; j++)
	{
		if(GetRow(j)->AnalysisPoint() == pAnalysisPoint)
		{
			if(GetRow(j)->ScenarioNr() == iScenarioNr)
				return m_BatchTable[j];			
		}
	}
		
	return m_BatchTable[j];			
}

//##ModelId=3C8D9E0F0263
void CBatchTable::DeleteBatchTable()
{
	for(int i=0;i<m_BatchTable.size();i++)
	{
		delete m_BatchTable[i];
	}
	m_BatchTable.clear();
}

//##ModelId=3C8D9E0F035D
void CBatchTable::Invalidate()
{
	DeleteBatchTable();
}

//##ModelId=3C8D9E0F03CA
BOOL CBatchTable::IsBuild()
{
	return m_BatchTable.size()>0 ? TRUE:FALSE;
}

//##ModelId=3DDA0C4D02F1
BOOL CBatchTable::UseForCalculation() 
{
	for(int i = 0 ; i < RowCount() ; i++)
	{
		if(GetRow(i)->UseForCalculation())
			return TRUE;
	}
	
	return FALSE;
}

//##ModelId=3DDA0C4D02E2
BOOL CBatchTable::IsCalculated() 
{
	if(IsBuild()) 
	{
		for(int i = 0 ; i < RowCount() ; i++)
		{
			if(GetRow(i)->IsCalculated())
				return TRUE;
		}
	}
	return FALSE;
}

//##ModelId=3DDA0C4D02F2
BOOL CBatchTable::CalculationSucceeded() 
{
	if(IsBuild()) 
	{
		for(int i = 0 ; i < RowCount() ; i++)
		{
			if(GetRow(i)->CalculationSucceeded())
				return TRUE;
		}
	}
	return FALSE;
}

//##ModelId=3DDA0C4D0301
BOOL CBatchTable::HasResults() 
{
	if(IsBuild()) 
	{
		for(int i = 0 ; i < RowCount() ; i++)
		{
			if(GetRow(i)->HasResults())
				return TRUE;
		}
	}
	return FALSE;
}

//##ModelId=3DDA0C4D0321
int CBatchTable::GetIndex(CBatchTableRow* pBatchRow)
{
	if(IsBuild()) 
	{
		for(int i = 0 ; i < RowCount() ; i++)
		{
			if(GetRow(i) == pBatchRow)
				return i;
		}
	}
	return -1;
}

//##ModelId=3C0DDDAF01A7
BOOL CBatchTable::BuildBatchTable()
{
	DeleteBatchTable();

	for(int j=0;j<m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();j++)
	{
		for(int i=0;i<m_pDataModel->Analysis()->NrOfAnalysisPoints();i++)
		{
			CBatchTableRow *pRow = new CBatchTableRow(m_pDataModel,m_pDataModel->Analysis()->GetAnalysisPointAt(i),j);
			m_BatchTable.push_back(pRow);
		}
	}
	return true;
}

//##ModelId=3CA011980261
void CBatchTable::UpdateChanges()
{
	for(int i = 0 ; i< RowCount(); i++)
	{
		GetRow(i)->UpdateChanges();
	}
}

//##ModelId=3CA0119800BB
void CBatchTableRow::UpdateChanges()
{
	CDrillingOperation *pDO = m_pDataModel->DrillingScheme()->GetDrillingOperation(0);

	m_bTail = m_pDataModel->IsInTail(AnalysisPoint()->TMD()->Value());

	CCementScenariosTable* pCST = pDO->CementScenariosTable();
	CString strCementName;

	if(m_bTail)
	{
		strCementName = pCST->CementScenarioAt(m_iScenarioNr)->TailMaterialName();
	}
	else
	{
		strCementName = pCST->CementScenarioAt(m_iScenarioNr)->HeadMaterialName();
	}
	m_pCementMat = m_pDataModel->CementMatList()->GetMaterial(strCementName);


  if(m_pDataModel->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).second)
  {
	  double depth = m_pDataModel->TMDtoTVD( m_pAnalysisPoint->TMD()->Value() ).first;
	  m_pFormation=m_pDataModel->FormationTopography()->GetFormationAtDepth(depth);

	  ASSERT(m_pFormation);

	  CString strMatName = m_pFormation->MaterialName();
	  m_pFormationMat = m_pDataModel->FormationMatList()->GetMaterial(strMatName);
  }
}


//##ModelId=3C8D9E0F013B
CBatchTableRow::CBatchTableRow(CDataModel* pDataModel,CAnalysisPoint* pAnalysisPoint, int iScenarioNr)
:
m_bDefaultSteppingScheme	(true),
m_bStopBatchRun				(false),
m_bFurtherPostProcessing	(false),
//m_bNoMonitoring				(false),
m_bSaveDiagnostic			(true)
{
	
	IterationScheme(RegNewtonRaphson);
	ConvergenceNorm(Energy);
	m_iTimeStepNumber			= 6;
	m_iMaxNumberOfIterations	= 10;

	m_pDSResult = NULL;
	m_pFormation=NULL;
	m_pDataModel=pDataModel;
	m_pAnalysisPoint=pAnalysisPoint;
	m_bCalculationSucceeded = FALSE;
	m_bIsCalculated = FALSE;
	m_bHasResults=FALSE;

	m_iScenarioNr=iScenarioNr;
	m_bUseForCalculation=TRUE;
	m_pInitialStress = new CInitialStress(pDataModel,this);

	m_vecContourPlotFile.clear();
	m_vecContourPlotFile.resize(NR_OF_TABLES);
	m_sDiagnosticFile="";	
	ClearResult();

	UpdateChanges();	
	
}

//##ModelId=3C8D9E0F0159
CBatchTableRow::~CBatchTableRow()
{
  if(m_pDataModel->BatchTableSelection().SelectedRow() == this)
    m_pDataModel->BatchTableSelection().SelectRow(0);

	delete m_pInitialStress;

	if(m_pDSResult)
		delete m_pDSResult;
}

//##ModelId=3C8D9E0F017E
CCementMaterial *CBatchTableRow::CementMaterial() const
{
	return m_pCementMat;
}

//##ModelId=3C8D9E0F0189
CCasingMaterial *CBatchTableRow::CasingMaterial() const
{
	//just one global casing material
	CString strCementName;
	strCementName=m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	return m_pDataModel->CasingMatList()->GetMaterial(strCementName);
}

//##ModelId=3C8D9E0F016B
BOOL CBatchTableRow::IsTail() const
{
	return m_bTail;
}

//##ModelId=3C8D9E0F0179
int CBatchTableRow::ScenarioNr()
{
	return m_iScenarioNr;
}


//##ModelId=3C8D9E0F017A
CAnalysisPoint *CBatchTableRow::AnalysisPoint() const
{
	return m_pAnalysisPoint;
}

//##ModelId=3CA9A004033E
CDSResult *CBatchTableRow::DSResult() const
{
	return m_pDSResult;
}

//##ModelId=3C8D9E0F017C
CFormationMaterial *CBatchTableRow::FormationMaterial() const
{
	return m_pFormationMat;
}

//##ModelId=3C8D9E0F018B
CInitialStress *CBatchTableRow::InitialStress() const
{
	return m_pInitialStress;
}

//##ModelId=3C8D9E0F018D
CFormation *CBatchTableRow::Formation() const
{
	return m_pFormation;
}

//##ModelId=3C8D9E0F013A
BOOL CBatchTableRow::UseForCalculation()
{
	return m_bUseForCalculation;
}

//##ModelId=3C8D9E0F012A
void CBatchTableRow::UseForCalculation(BOOL newval)
{
	m_bUseForCalculation=newval;
}

	//From: Stepping Scheme
	//Label: Default stepping Scheme
	//Descr: True; default stepping scheme is adopted, false; 
	//access to advanced options
//##ModelId=3DDA0C4D018A
	bool CBatchTableRow::DefaultSteppingScheme() const
	{
		return m_bDefaultSteppingScheme;	
	}

//##ModelId=3DDA0C4D0199
	void CBatchTableRow::DefaultSteppingScheme(bool bDefaultSteppingScheme)
	{
		m_bDefaultSteppingScheme= bDefaultSteppingScheme;
	}

	//From: Stepping Scheme
	//Label: Number of time steps
	//Descr: number of time steps for application of one load
//##ModelId=3DDA0C4D012C
	long CBatchTableRow::TimeStepNumber() const
	{
		return m_iTimeStepNumber;	
	}

//##ModelId=3DDA0C4D013C
	void CBatchTableRow::TimeStepNumber(long iTimeStepNumber)
	{
		m_iTimeStepNumber= iTimeStepNumber;
	}

	//From: Stepping Scheme
	//Label: Iteration scheme
	//Descr: iteration scheme for all non-linear analyses
//##ModelId=3DDA0C4D014B
	CBatchTableRow::eIterationScheme CBatchTableRow::IterationScheme() const
	{
		return m_eIterationScheme;	
	}

	void CBatchTableRow::IterationScheme(CBatchTableRow::eIterationScheme iterationscheme)
	{
		m_eIterationScheme= iterationscheme;
	}

	//From: Stepping Scheme
	//Label: Max. numb. iterations
	//Descr: maximum number of iterations
//##ModelId=3DDA0C4D015B
	long CBatchTableRow::MaxNumberOfIterations() const
	{
		return m_iMaxNumberOfIterations;	
	}

//##ModelId=3DDA0C4D016B
	void CBatchTableRow::MaxNumberOfIterations(long iMaxNumberOfIterations)
	{
		m_iMaxNumberOfIterations= iMaxNumberOfIterations;
	}

	//From: Stepping Scheme
	//Label: convergence norm
	//Descr: name of adopted convergence norm
//##ModelId=3DDA0C4D017A
	CBatchTableRow::eConvergenceNorm CBatchTableRow::ConvergenceNorm() const
	{
		return m_eConvergenceNorm;	
	}

	void CBatchTableRow::ConvergenceNorm(CBatchTableRow::eConvergenceNorm convergencenorm)
	{
		m_eConvergenceNorm= convergencenorm;
	}

	//From: Analysis
	//Label: Stop batch run if divergence
	//Descr: True if analysis is stopped after first 
	//encountered divergence.
//##ModelId=3DDA0C4D00EE
	bool CBatchTableRow::StopBatchRun() const
	{
		return m_bStopBatchRun;	
	}

//##ModelId=3DDA0C4D00F0
	void CBatchTableRow::StopBatchRun(bool bStopBatchRun)
	{
		m_bStopBatchRun= bStopBatchRun;
	}

	//From: Analysis
	//Label: Allow further post-processing
	//Descr: True if results file kept for further 
	//post-processing
//##ModelId=3DDA0C4D00FD
	bool CBatchTableRow::FurtherPostProcessing() const
	{
		return m_bFurtherPostProcessing;	
	}

//##ModelId=3DDA0C4D010D
	void CBatchTableRow::FurtherPostProcessing(bool bFurtherPostProcessing)
	{
		m_bFurtherPostProcessing= bFurtherPostProcessing;
	}

	//From: Analysis
	//Label: No calculation monitoring
	//Descr: True if caculation monitor interface disabled.
	//bool CBatchTableRow::NoMonitoring() const
	//{
	//	return m_bNoMonitoring;	
	//}

	//void CBatchTableRow::NoMonitoring(bool bNoMonitoring)
	//{
	//	m_bNoMonitoring= bNoMonitoring;
	//}

	//From: Analysis
	//Label: Save diagnostic
	//Descr: True if diagnostid file is saved. Default=true
//##ModelId=3DDA0C4D011C
	bool CBatchTableRow::SaveDiagnostic() const
	{
		return m_bSaveDiagnostic;	
	}

	//##ModelId=3C0DDDAB03CE
	void CBatchTableRow::SaveDiagnostic(bool bSaveDiagnostic)
	{
		m_bSaveDiagnostic= bSaveDiagnostic;
	}

//##ModelId=3C8D9E0F02EF
BOOL CBatchTable::CanBuildBatchTable()
{
	if(!m_pDataModel->Analysis()->CheckComplete())
		return FALSE;
	
	if(!m_pDataModel->FormationTopography()->CheckComplete())
		return FALSE;

	if(!m_pDataModel->DrillingScheme()->CheckComplete())
		return FALSE;

	if(!m_pDataModel->CheckAllMaterials())
		return FALSE;

	return TRUE;
}

//##ModelId=3DDA0C4D033F
std::vector<CBatchTableRow*> CBatchTable::GetResultRows(CAnalysisPoint* pAnalysisPoint)
{
	m_vecPointBatchRows.clear();
	
	for(int j = 0 ; j < RowCount() ; j++)
	{
		if(GetRow(j)->AnalysisPoint() == pAnalysisPoint)
		{
			if(GetRow(j)->HasResults())
				m_vecPointBatchRows.push_back(GetRow(j));
		}
	}

	return m_vecPointBatchRows;
}

static void WriteLine(FILE* file,const CString &name, const double &val,const CString &sUnitName)
{
	fprintf(file,"%-45s%20g %s\n",name,val,sUnitName);
	
}

static void WriteLine(FILE* file,const CString &name, const CString &name2)
{
	fprintf(file,"%-45s%20s\n",name,name2);
}

static void WriteLine(FILE* file,const CString &name, long iVal)
{
	fprintf(file,"%-45s%20d\n",name,iVal);
}

static void WriteLine(FILE* file,const CString &name)
{

	fprintf(file,"%-45s\n",name);
	FlushFileBuffers(file);
}

static void WriteLine(FILE* file)
{
	fprintf(file,"\n");
}

static void WriteLine(FILE* file,const CString &name, bool bYes)
{
	if(bYes)
	{
		fprintf(file,"%-45s%20s\n",name,"YES");
	}
	else
	{
		fprintf(file,"%-45s%20s\n",name,"NO");
	}
}

static void WriteLine(FILE* file,const CString &name, double dVal,int iVal=-1);

static void WriteLine(FILE* file,const CString &name, double dVal,int iVal)
{
	if(iVal>0)
		fprintf(file,"%-5d%s%f\n",iVal ,name,dVal);
	else
		fprintf(file,"%s%f\n",name,dVal);
}

static void WriteLine2(FILE* file,const CString &name1, const CString &name2)
{
	fprintf(file,"%s%s\n",name1,name2);
}

//******************

static void WriteLineMat(FILE* file,const CString &name, const double &val,const CString &sUnitName)
{
	fprintf(file,"%-7s%-68g %s\n",name,val,sUnitName);
}

static void WriteLineMat(FILE* file,const CString &name, const CString &name2)
{
	fprintf(file,"%-7s%-68s\n",name,name2);
}

static void WriteLineMat(FILE* file,const CString &name, long lVal)
{
	fprintf(file,"%-7s%-68d\n",name,lVal);
}

static void WriteLineMat(FILE* file,const CString &name, int iVal)
{
	fprintf(file,"%5s%-7s%-68i\n","", name,iVal);
}

static void WriteLineMat(FILE* file,const CString &name)
{
	fprintf(file,"%-7s\n",name);
}

static void WriteLineMat(FILE* file)
{
	fprintf(file,"\n");
}

static void WriteLineMat(FILE* file,const CString &name, bool bYes)
{
	if(bYes)
	{
		fprintf(file,"%-7s%-68s\n",name,"YES");
	}
	else
	{
		fprintf(file,"%-7s%-68s\n",name,"NO");
	}
}

static void WriteLineMat(FILE* file,const CString &name, double dVal,int iVal=-1);

static void WriteLineMat(FILE* file,const CString &name, double dVal,int iVal)
{
	CString space = "";

	if(iVal>0)
		fprintf(file,"%4d%s%-7s%-68g\n",iVal," " ,name,dVal);
	else
		fprintf(file,"%5s%-7s%-68g\n",space,name,dVal);
}

static void WriteLineMat2(FILE* file,const CString &name1, const CString &name2)
{
	CString space = "";
	fprintf(file,"%5s%-7s%-68s\n",space,name1,name2);
}


//*******************


static void PrintHardening(FILE* file,const geo::CMatrix &HardeningMatrix)
{
	for(int row=0;row< HardeningMatrix.RowSize();row++)
	{
		if(row==0)
		{
			fprintf(file,"%5s%-7s%-15f%-24f\n","", "HARDIA",HardeningMatrix.Value(row,1),HardeningMatrix.Value(row,0));
		}
		else
		{
			fprintf(file,"%12s%-15f%-24f\n" ,"",HardeningMatrix.Value(row,1),HardeningMatrix.Value(row,0));
		}
	}
	
}

static MF::HARD_TYPE GetHardeningType(eHardeningType HardeningType)
{
		if(HardeningType == LinearHardening)
			return MF::LINEAR_HARD;
		else if(HardeningType == LinearSoftening) 
			return MF::LINEAR_SOFT;
		else if(HardeningType == ParabolicSoftening) 
			return MF::PARABOLIC_HARD;
		else
			ASSERT(FALSE);
			return MF::LINEAR_HARD; //dummy 

}

//##ModelId=3C8D9E0F015B
BOOL CBatchTableRow::WriteMaterialFile(LPCSTR sFileName) 
{

	CString space = "           ";

	FILE* file = fopen(sFileName,"w");
	if(!file)
		return FALSE;
	
	WriteLineMat(file,"'MATERIALS'");
	WriteLineMat(file,"");

// 1 *****	
	WriteLineMat(file,":" + space + "RockYoungModulus");
	WriteLineMat(file,"YOUNG",FormationMaterial()->YoungsModulus()->Value(),1);

	WriteLineMat(file,":" + space + "RockPoissonRatio");
	WriteLineMat(file,"POISON",FormationMaterial()->PoissonRatio()->Value());
	
	WriteLineMat(file,":" + space + "RockThermalExpansion");
	WriteLineMat(file,"THERMX",FormationMaterial()->ThermalExpansion()->Value());

	WriteLineMat(file,":" + space + "RockThermalConductivity");
	WriteLineMat(file,"CONDUC",FormationMaterial()->ThermalConductivity()->Value());

	WriteLineMat(file,":" + space + "RockVolumetricSpecificHeat");
	WriteLineMat(file,"CAPACI",FormationMaterial()->VolumetricSpecificHeat()->Value());

	if(FormationMaterial()->Plasticity())
	{
		double cohesion          = m_pDataModel->GetCohesionAtDepth(GetDepth());
		double frictionangle     = m_pDataModel->GetFrictionAtDepth(GetDepth());
		double dilatationangle   = m_pDataModel->GetDilatationAtDepth(GetDepth());
		
		double SinFric = sin(frictionangle*PI/180.0);
		double SinDilat = sin(dilatationangle*PI/180.0);
				

		WriteLineMat(file,":" + space + "fixed keyword");
		WriteLineMat2(file,"YIELD","MOHRCO");
		fprintf(file,"%-5s%-7s%-15s%-24s%-30s\n" ,":", "","RockCohesion", "RockSinFrictionAngle", "RockDilatation(0)");
		fprintf(file,"%5s%-7s%-15f%-24f%-30f\n" ,"", "YLDVAL",cohesion,SinFric,SinDilat);

		if(FormationMaterial()->HardeningType() != None)
		{
			double hardeninggradient = 0.0;
			double fractureenergie   = 0.0;
			double youngsmodulus     = FormationMaterial()->YoungsModulus()->Value();

			if(FormationMaterial()->HardeningType() == LinearHardening)
			{
				hardeninggradient = FormationMaterial()->HardeningGradient()->Value();
			}
			else
			{
				fractureenergie   = FormationMaterial()->FractureEnergy()->Value();
			}
			
			MF::HARD_TYPE HardeningType = GetHardeningType(FormationMaterial()->HardeningType());
			geo::CMatrix HardeningMatrix = MF::GenerateHardeningCurve(cohesion,
											 frictionangle,
											 dilatationangle,
											 hardeninggradient,
											 fractureenergie,
											 youngsmodulus,
											 HardeningType,
											 MF::DEG);

			fprintf(file,"%-5s%-7s%-15s%-24s\n" ,":", "","RockHardSig", "RockHardEps");
			PrintHardening(file ,HardeningMatrix);
		}
	}
	WriteLineMat(file,"");

// 2 *****
	WriteLineMat(file,":" + space + "CementYoungModulus");	
	WriteLineMat(file,"YOUNG",CementMaterial()->YoungsModulus()->Value(),2);
	
	WriteLineMat(file,":" + space + "CementPoissonRatio");	
	WriteLineMat(file,"POISON",CementMaterial()->PoissonRatio()->Value());

	WriteLineMat(file,":" + space + "CementThermalExpansion");	
	WriteLineMat(file,"THERMX",CementMaterial()->ThermalExpansion()->Value());

	WriteLineMat(file,":" + space + "CementThermalConductivity");	
	WriteLineMat(file,"CONDUC",CementMaterial()->ThermalConductivity()->Value());

	WriteLineMat(file,":" + space + "CementVolumetricSpecificHeat");	
	WriteLineMat(file,"CAPACI",CementMaterial()->VolumetricSpecificHeat()->Value());
		
	WriteLineMat(file,":" + space + "fixed keyword");
	WriteLineMat2(file,"YIELD","MOHRCO");

	double poisson           = CementMaterial()->PoissonRatio()->Value();
	double cohesion          = CementMaterial()->Cohesion()->Value();
	double frictionangle     = CementMaterial()->FrictionAngle()->Value();
	double dilatationangle   = CementMaterial()->FrictionAngle()->Value(); // looks strange but is ok!

	fprintf(file,"%-5s%-7s%-15s%-24s%-30s\n" ,":", "","CementCohesion", "CementSinFrictionAngle", "CementDilatation(0)");
	fprintf(file,"%5s%-7s%-15f%-24f%-30f\n" ,"", "YLDVAL",cohesion,sin(frictionangle*PI/180.0),sin(dilatationangle*PI/180.0));

	if(CementMaterial()->HardeningType() != None)
	{
		double hardeninggradient = 0.0;
		double fractureenergie   = 0.0;
		double youngsmodulus     = CementMaterial()->YoungsModulus()->Value();
		
		if(CementMaterial()->HardeningType() == LinearHardening)
		{
			hardeninggradient = CementMaterial()->CompHardeningGradient()->Value();
		}
		else
		{
			fractureenergie   = CementMaterial()->CompFractureEnergy()->Value();
		}
		
		MF::HARD_TYPE HardeningType = GetHardeningType(CementMaterial()->HardeningType());
		geo::CMatrix HardeningMatrix = MF::GenerateHardeningCurve(cohesion,
										 frictionangle,
										 dilatationangle,
										 hardeninggradient,
										 fractureenergie,
										 youngsmodulus,
										 HardeningType,
										 MF::DEG);

		fprintf(file,"%-5s%-7s%-15s%-24s\n" ,":", "","CementHardSig", "CementHardEps");
		PrintHardening(file ,HardeningMatrix);
	}
	
	WriteLineMat(file,":" + space + "fixed value");	
	WriteLineMat(file,"CRACK", 1 );
	
	WriteLineMat(file,":" + space + "CementTensileStrength");	
	WriteLineMat(file,"CRKVAL",CementMaterial()->TensileStrength()->Value());

	WriteLineMat(file,":" + space + "fixed value");	
	WriteLineMat(file,"TENSIO",1);

	WriteLineMat(file,":" + space + "CementTenFracEnergy");	
	WriteLineMat(file,"TENVAL", CalcFEMFracEnergy());

	WriteLineMat(file,":" + space + "fixed value");	
	WriteLineMat(file,"TAUCRI",0);
	
	fprintf(file,"%-5s%-7s%-15s%-24s\n" ,":", "","0", "CementLinShrinkage");
	fprintf(file,"%5s%-7s%-15f%-24f\n" ,"", "SHRINF",0.0 , CalcCementLinShrinkage()); 

	WriteLineMat(file,":" + space + "fixed values");	
	fprintf(file,"%5s%-7s%-15f%-24f\n" ,"", "SHTIME",0.0,1.0); 

	WriteLineMat(file,"");

// 3 *****	
	WriteLineMat(file,":" + space + "CasingYoungModulus");
	WriteLineMat(file,"YOUNG",CasingMaterial()->YoungsModulus()->Value(),3);

	WriteLineMat(file,":" + space + "CasingPoissonRatio");
	WriteLineMat(file,"POISON",CasingMaterial()->PoissonRatio()->Value());
	
	WriteLineMat(file,":" + space + "CasingTherExpansion");
	WriteLineMat(file,"THERMX",CasingMaterial()->ThermalExpansion()->Value());

	WriteLineMat(file,":" + space + "CasingTherConduct");
	WriteLineMat(file,"CONDUC",CasingMaterial()->ThermalConductivity()->Value());

	WriteLineMat(file,":" + space + "CasingTherCapacity");
	WriteLineMat(file,"CAPACI",CasingMaterial()->VolumetricSpecificHeat()->Value());

	if(CasingMaterial()->Plasticity())
	{
		WriteLineMat(file,":" + space + "fixed keyword");
		WriteLineMat2(file,"YIELD","VMISES");

		WriteLineMat(file,":" + space + "CasingYieldStrength");
		WriteLineMat(file,"YLDVAL",CasingMaterial()->YieldStrength()->Value());
	}

	WriteLineMat(file,"");

// 4 *****
	fprintf(file,"%-5s%-7s%-20s%-20s\n" ,":", "","InterfaceStiffness", "InterfaceStiffness");
	fprintf(file,"%4d%s%-7s%-20f%-20f\n" ,4," ", "DSTIF", CalcInterfaceStiffness(true) ,CalcInterfaceStiffness(true)); // see page 30/59

	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"DISCRA",1);

	WriteLineMat(file,":" + space + "ExtInterfaceStrength");
	WriteLineMat(file,"DCRVAL",CalcInterfaceRelatStrength(false)); //see page 30/59
	
	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"MODE1",1);

	WriteLineMat(file,":" + space + "ExtInterfaceEnergy");
	WriteLineMat(file,"MO1VAL",CalcInterfaceRelatFracEnergy(false)); //see page 30/59

	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"MODE2",1);

	WriteLineMat(file,":" + space + "InterfaceStiffness");
	WriteLineMat(file,"MO2VAL",CalcInterfaceStiffness(true));//see page 30/59

	WriteLineMat(file,":" + space + "ExtInterfaceFlux");
	WriteLineMat(file,"DFLUX",CalcInterfaceConductivity(false));//see page 30/59

	WriteLineMat(file,"");

// 5 *****
	fprintf(file,"%-5s%-7s%-20s%-20s\n" ,":", "","InterfaceStiffness", "InterfaceStiffness");
	fprintf(file,"%4d%s%-7s%-20f%-20f\n" ,5," ", "DSTIF",CalcInterfaceStiffness(true),CalcInterfaceStiffness(true)); // see page 30/59

	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"DISCRA",1);

	WriteLineMat(file,":" + space + "IntInterfaceStrength");
	WriteLineMat(file,"DCRVAL",CalcInterfaceRelatStrength(true)); //see page 30/59
	
	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"MODE1",1);

	WriteLineMat(file,":" + space + "IntInterfaceEnergy");
	WriteLineMat(file,"MO1VAL",CalcInterfaceRelatFracEnergy(true)); //see page 30/59

	WriteLineMat(file,":" + space + "fixed value");
	WriteLineMat(file,"MODE2",1);

	WriteLineMat(file,":" + space + "InterfaceStiffness");
	WriteLineMat(file,"MO2VAL",CalcInterfaceStiffness(true));//see page 30/59

	WriteLineMat(file,":" + space + "IntInterfaceFlux");
	WriteLineMat(file,"DFLUX",CalcInterfaceConductivity(true));//see page 30/59
	
	fclose(file);

	return TRUE;
}


//##ModelId=3C8D9E0F0169
BOOL CBatchTableRow::WriteAnalysisFile(LPCSTR sFileName)
{
	FILE* file = fopen(sFileName,"w");
	
	if(!file)
		return FALSE;

	WriteLine(file , sFileName);
	WriteLine(file);


//################################################################################################################################
	WriteLine(file , "VERSION INFO");
	WriteLine(file , "Version"  , long(DIANA_KERNEL_VERSION));
	WriteLine(file , "Revision" , long(DIANA_KERNEL_REVISION));
	WriteLine(file);
//################################################################################################################################




//################################################################################################################################
	WriteLine(file , "OPTIONAL LOADS" );
	//WriteLine(file , "Unbalanced completion", !m_pDataModel->Control()->BalancedCompletion());
	WriteLine(file , "Unbalanced completion", m_pDataModel->Control()->DoCompletion());
	WriteLine(file , "Shut-in"              , m_pDataModel->Control()->ShutIn());
	WriteLine(file , "Production"           , m_pDataModel->Control()->IsProduction());
	WriteLine(file , "Injection"            , m_pDataModel->Control()->IsInjection());
	WriteLine(file , "Fracturing"           , m_pDataModel->Control()->Fracturing());
	WriteLine(file , "Evacuation"           , m_pDataModel->Control()->Evacuation());
	WriteLine(file);
//################################################################################################################################	

	



//################################################################################################################################
	WriteLine(file , "INITIAL STRESSES" );
	InitialStress()->CalculateStress();
	WriteLine(file , "Insitu effective stress xx", InitialStress()->EffStressXX() ,"MPa");
	WriteLine(file , "Insitu effective stress yy", InitialStress()->EffStressYY() ,"MPa");
	WriteLine(file , "Insitu effective stress zz", InitialStress()->EffStressZZ() ,"MPa");
	WriteLine(file , "Insitu effective stress xy", InitialStress()->EffStressXY() ,"MPa");
	WriteLine(file , "Insitu effective stress yz", InitialStress()->EffStressYZ() ,"MPa");
	WriteLine(file , "Insitu effective stress zx", InitialStress()->EffStressZX() ,"MPa");
	WriteLine(file , "Insitu pore pressure", InitialStress()->PorePressure(),"MPa");
	WriteLine(file , "Drilling mud pressure", InitialStress()->DrillingMudPressure(),"MPa");
	WriteLine(file , "Cement slurry pressure", InitialStress()->CementSlurryPressure(),"MPa");
	WriteLine(file , "Casing axial stress", InitialStress()->CasingAxialStress(),"MPa");
	WriteLine(file , "Waiting on cement fluid pressure", InitialStress()->WaitingOnCementFluidPressure(),"MPa");
	WriteLine(file);
//################################################################################################################################





//################################################################################################################################
	WriteLine(file , "APPLIED LOADS" );
	WriteLine(file , "Pressure test", AnalysisPoint()->TestPressure(), "MPa");

	//if(!m_pDataModel->Control()->BalancedCompletion())
	if(m_pDataModel->Control()->DoCompletion())
	{
		double press;
		if(m_pDataModel->Control()->UnderBalancedCompletion())
		{
			
			press =  InitialStress()->UndrainedPorePressure() - AnalysisPoint()->PerforationLoads()->CompletionPressureDiff()->Value();
			if(press < 0.0) //TODO : give preanalysis WARNING
				press =0.0;
		}
		else
		{
			press =  InitialStress()->UndrainedPorePressure() + AnalysisPoint()->PerforationLoads()->CompletionPressureDiff()->Value();
		}
		WriteLine(file , "Completion   ", press ,"MPa");
	}

	if(m_pDataModel->Control()->IsProduction())
	{
		double press = AnalysisPoint()->ProductionLoads()->ProductionPorePressure().Value();
		WriteLine(file , "Production pore pressure", press,"MPa");
		press = AnalysisPoint()->ProductionLoads()->BottomHoleFlowingPressure()->Value();
		WriteLine(file , "Production", press,"MPa");
	}

	if(m_pDataModel->Control()->IsInjection())
	{
		double press = AnalysisPoint()->InjectionLoads()->MaximumInjectionPressure()->Value();
		WriteLine(file , "Injection", press,"MPa");
	}

	if(m_pDataModel->Control()->Fracturing())
		WriteLine(file , "Fracturing", AnalysisPoint()->FracturingLoads()->MaxInjectingFracPressure()->Value(),"MPa");

	if(m_pDataModel->Control()->Evacuation())
		WriteLine(file , "Evacuation", AnalysisPoint()->EvacuationLoads()->BottomHoleTreatingPressure()->Value(),"MPa");

	WriteLine(file);
//################################################################################################################################



//################################################################################################################################
	WriteLine(file , "TEMPERATURES" );
	WriteLine(file , "Insitu",m_pDataModel->GetGeoThermalTemperatureAtDepth(GetDepth()),  "C");
	WriteLine(file , "Drilling",AnalysisPoint()->PriorToCompletionLoads()->DrillingTemperature()->Value() ,  "C");
	if(m_pDataModel->Control()->IsProduction())
		WriteLine(file , "Production",AnalysisPoint()->ProductionLoads()->BottomHoleFlowingTemp()->Value(),  "C");
	if(m_pDataModel->Control()->IsInjection())
		WriteLine(file , "Injection",AnalysisPoint()->InjectionLoads()->InjectionTemperature()->Value(),  "C");
	if(m_pDataModel->Control()->Fracturing())
		WriteLine(file , "Fracturing",AnalysisPoint()->FracturingLoads()->TreatingFluidTemp()->Value(),  "C");
	if(m_pDataModel->Control()->Evacuation())
		WriteLine(file , "Evacuation",AnalysisPoint()->EvacuationLoads()->TreatingFluidTempEvac()->Value(),  "C");
	WriteLine(file);
//################################################################################################################################



//################################################################################################################################
	WriteLine(file , "EXPOSURE TIMES" );
	WriteLine(file , "Drilling",AnalysisPoint()->PriorToCompletionLoads()->TimeOfExposureToDrillingFluid()->Value(),  "s");
	if(m_pDataModel->Control()->IsProduction())
		WriteLine(file , "Production",AnalysisPoint()->ProductionLoads()->TimeOfContinuousProduction()->Value(),  "s");
	if(m_pDataModel->Control()->IsInjection())
		WriteLine(file , "Injection",AnalysisPoint()->InjectionLoads()->TimeOfContinuousInjection()->Value(),  "s");
	if(m_pDataModel->Control()->Fracturing())
		WriteLine(file , "Fracturing",AnalysisPoint()->FracturingLoads()->TimeOfExposureToTreatingFluid()->Value(),  "s");
	if(m_pDataModel->Control()->Evacuation())
		WriteLine(file , "Evacuation",AnalysisPoint()->EvacuationLoads()->TimeOfExposureToTreatingFluidEvac()->Value(),  "s");
	WriteLine(file);
//################################################################################################################################


//################################################################################################################################
	WriteLine(file , "ANALYSIS CONTROL" );
	WriteLine(file , "Plasticity in formation" ,FormationMaterial()->Plasticity()?true:false);
	WriteLine(file , "Plasticity in casing"    ,CasingMaterial()->Plasticity());
	
	WriteLine(file , "Number of time steps"     ,TimeStepNumber());
	

	CString sIterationScheme;
	if(IterationScheme()== CBatchTableRow::Constant)
		sIterationScheme="CONSTANT";
	else if(IterationScheme()==CBatchTableRow::RegNewtonRaphson)
		sIterationScheme="REGULAR";
	else if(IterationScheme()==CBatchTableRow::ModNewtonRaphson)
		sIterationScheme="MODIFIED";
	else
		ASSERT(FALSE);

	WriteLine(file , "Iteration Scheme"    ,sIterationScheme);

	WriteLine(file , "Max number of iterations"    ,MaxNumberOfIterations());

	CString sConvergenceNorm;
	if(ConvergenceNorm()==CBatchTableRow::Force)
		sConvergenceNorm="FORCE";
	else if(ConvergenceNorm()==CBatchTableRow::Displacement)
		sConvergenceNorm="DISPLACEMENT";
	else if(ConvergenceNorm()==CBatchTableRow::Energy)
		sConvergenceNorm="ENERGY";
	else
		ASSERT(FALSE);

	WriteLine(file,"Convergence norm",sConvergenceNorm);

	
	WriteLine(file , "Output level"            ,m_pDataModel->Control()->OutputLevel());
	WriteLine(file , "Expected number of radial cracks" ,long(1) );
	WriteLine(file , "Sealant sheath inner radius"  ,CasingMaterial()->CasingOuterRadius() , "m");
	WriteLine(file , "Sealant sheath outer radius"  ,m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->BoreholeRadius(),"m" );
	WriteLine(file);
//################################################################################################################################


	fclose(file);

	return TRUE;

}


//##ModelId=3C8D9E0F015A
double CBatchTableRow::GetDepth() 
{
  assert(m_pDataModel->TMDtoTVD(AnalysisPoint()->TMD()->Value()).second);
	return m_pDataModel->TMDtoTVD(AnalysisPoint()->TMD()->Value()).first;
}

//##ModelId=3C8D9E0E03CB
double CBatchTableRow::CalcInterfaceStiffness(bool bInternal)
{
	double CasingYoung = CasingMaterial()->YoungsModulus()->Value();
	double OuterRadius =  CasingMaterial()->CasingOuterDiameter()->Value() / 2.0 ;
	double BoreholeRadius = m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->BoreholeRadius();

	return (CasingYoung / (BoreholeRadius - OuterRadius) * 1000.0);
	

}

//##ModelId=3C8D9E0E03BD
double CBatchTableRow::CalcInterfaceConductivity(bool bInternal)
{
	double OuterRadius =  CasingMaterial()->CasingOuterDiameter()->Value() / 2.0 ;
	double BoreholeRadius = m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->BoreholeRadius();
	double VolumSpecHeat = CasingMaterial()->VolumetricSpecificHeat()->Value();
	double ExtConduct;

	ExtConduct = ((pow(BoreholeRadius,2.0) - pow(OuterRadius,2.0)) * VolumSpecHeat * 100.0) / (2.0 * BoreholeRadius);
	
	if(bInternal)
	{
		return (BoreholeRadius / OuterRadius) * ExtConduct;
	}
	else
	{
		return ExtConduct;
	}
}


//##ModelId=3C8D9E0E03BB
double CBatchTableRow::CalcInterfaceRelatStrength(bool bInternal)
{
	if(bInternal)
	{
		return 0.7 * CementMaterial()->TensileStrength()->Value();
	}
	else
	{
		return 0.1 * CementMaterial()->TensileStrength()->Value();
	}
}

//##ModelId=3C8D9E0E03AC
double CBatchTableRow::CalcInterfaceRelatFracEnergy(bool bInternal)
{
	if(bInternal)
	{
		return 0.7 * CementMaterial()->TensFractureEnergy()->Value();
	}
	else
	{
		return 0.1 * CementMaterial()->TensFractureEnergy()->Value();
	}
}

//##ModelId=3CA01198001F
double CBatchTableRow::CalcCementLinShrinkage()
{
	double Shrinkage = CementMaterial()->VolumetricShrinkage()->Value();
	
	if(Shrinkage > 0)
	{
		return Shrinkage / 6.0;
	}
	else 
	{
		return Shrinkage / 3.0;
	}
}

//##ModelId=3CA01197035B
double CBatchTableRow::CalcFEMFracEnergy()
{
	double dGft = CementMaterial()->TensFractureEnergy()->Value();
	
	return (dGft / CalcCrackBand());
}

//##ModelId=3CA0119702BF
double CBatchTableRow::CalcCrackBand()
{	
	double dBorehole = m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->BoreholeRadius();
	double dNrOfTangents  = 2 * m_pDataModel->MeshParam()->NDivTangent();
	double dNP = 2.0;  //number of integration point. see 4.2.4 page 29/59

	return (2 * PI * dBorehole) / (dNrOfTangents * dNP);

}

//##ModelId=3CA9A0040070
BOOL CBatchTableRow::IsCalculated()
{
	return m_bIsCalculated;
}

//##ModelId=3DDA0C4D009F
void CBatchTableRow::IsCalculated(BOOL bIscalculated)
{
	m_bIsCalculated = bIscalculated;
}

//##ModelId=3CA9A004013B
BOOL CBatchTableRow::CalculationSucceeded()
{
	return m_bCalculationSucceeded;
}

//##ModelId=3DDA0C4D00B0
void CBatchTableRow::CalculationSucceeded(BOOL bSucceeded)
{
	m_bCalculationSucceeded = bSucceeded;
	
}



//##ModelId=3DDA0C4D00BF
BOOL CBatchTableRow::HasResults()
{
	return m_bHasResults;
}

//##ModelId=3DDA0C4D00CE
void CBatchTableRow::HasResults(BOOL bHasResults)
{
	m_bHasResults = bHasResults;
	
}


//##ModelId=3DDA0C4D0080
void CBatchTableRow::SetContourFileNames(const std::vector< CString >& vecContourFileNames)
{
	m_vecContourPlotFile=vecContourFileNames;
}

//##ModelId=3DDA0C4D0090
const CByteArray& CBatchTableRow::GetByteArray()
{
	return m_ByteArray;
}

//##ModelId=3DDA0C4D0071
void CBatchTableRow::SetByteArray(const CByteArray& bytearray)
{
	m_ByteArray.RemoveAll();
	m_ByteArray.Copy(bytearray);

	CString sTmpPath;
	CString sTmpFileName;
	LPTSTR lptstrFN = sTmpFileName.GetBufferSetLength(1024);
	LPTSTR lptstr = sTmpPath.GetBufferSetLength(1024);
	GetTempPath(1024,lptstr);								
	GetTempFileName(lptstr,"~DSO",0,lptstrFN);			// get temporary file-name; 0 means unique
	ByteArrayToFile((LPCSTR)sTmpFileName,m_ByteArray);	// copy byte-array to tempfile
	GetResultFromDSOFile(sTmpFileName);					// read results from tempfile
	DeleteFile(sTmpFileName);							// delete tempfile

}

//##ModelId=3CA9A00302A2
BOOL CBatchTableRow::GetResultFromDSOFile(CString sFileName)
{
	ClearResult();
	return DSResult()->ReadDSOFile(sFileName);
	
//show results in mssgbox as test
	if(DSResult()->ReadDSOFile(sFileName))
	{
		CString sTotal = "SPACE AND TIME\n\n";
		CString sStr;
		
		for(int a=0 ; a<NR_OF_TABLES; a++)
		{
			if(DSResult()->SpaceAndTime()->GetSpaceAndTimeTable(a)->HasResults())
			{
				for(int b = 0 ; b < NR_OF_LOADPHASES ; b++)
				{
					if(DSResult()->SpaceAndTime()->GetSpaceAndTimeTable(a)->GetLoadPhaseItem(b)->Exists())
					{
						sStr.Format("%i",DSResult()->SpaceAndTime()->GetSpaceAndTimeTable(a)->GetLoadPhaseItem(b)->Failed());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%#4.1f",DSResult()->SpaceAndTime()->GetSpaceAndTimeTable(a)->GetLoadPhaseItem(b)->Capacity());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%#4.4g",DSResult()->SpaceAndTime()->GetSpaceAndTimeTable(a)->GetLoadPhaseItem(b)->DamageLevel());
						sTotal = sTotal + sStr + "\n";
					}
					
				}
				sTotal = sTotal +  "\n";
			}
		}
		AfxMessageBox(sTotal);
		sTotal="SPACE\n\n";
		
		for(int i=0 ; i<NR_OF_TABLES ; i++)
		{
			for(int j = 0 ; j < DSResult()->Space()->GetSpaceTable(i)->GetNrOfLoadPhaseItems() ; j++)
			{
				sStr.Format("%i",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetMaxDamageSTNB());
				sTotal = sTotal + "MaxDamSTNB " + sStr + "\t";
				sStr.Format("%i",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetMaxDamageLONB());
				sTotal = sTotal + "MaxDamLONB " + sStr + "\t\n";

				for(int k = 0 ; k < DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetNrOfAppliedLoads() ; k++)
				{
																						
					for(int l = 0 ; l < DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetNrOfItems() ; l++)
					{
												
						sTotal = sTotal + DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetLoadLabel() + "\t\t";
						sStr.Format("%i",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->LONB());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%i",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->STNB());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%#4.2f",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->LoadLevel());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%#4.3f",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->Time());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%i",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->Failed());
						sTotal = sTotal + sStr + "\t";
						sStr.Format("%#4.4g",DSResult()->Space()->GetSpaceTable(i)->GetLoadPhaseItem(j)->GetAppliedLoad(k)->GetAppliedLoadItem(l)->DamageLevel());
						sTotal = sTotal + sStr + "\n";
					}
				}
			}
			sTotal = sTotal +  "\n";
		}
		AfxMessageBox(sTotal);
	}
	
	return TRUE;
}

//##ModelId=3CAC3FED0253
void CBatchTableRow::ClearResult()
{
	if(m_pDSResult)
		delete m_pDSResult;

	m_pDSResult = new CDSResult;
}

//##ModelId=3DDA0C4D0061
void CBatchTableRow::SetDiagnosticFileName(CString sFilename)
{
	m_sDiagnosticFile=sFilename;
}

//##ModelId=3DDA0C4D0024
CString CBatchTableRow::GetDiagnosticFileName()
{
	return 	m_sDiagnosticFile; 
}

//##ModelId=3DDA0C4D0032
void CBatchTableRow::ProcessDiagnosticFile(CString sWorkingDir)
{
	if(!SaveDiagnostic())
		return;

	CString sDiagnosticFile=EnsureBackslash(sWorkingDir) + DIAGNOSTIC_FILE;
	if(!FileExists(sDiagnosticFile))
		return;
	
	
	CString sTblNr;
	CString sTmpFileName;
	LPTSTR lptstrFN = sTmpFileName.GetBufferSetLength(1024);
	GetTempFileName(m_pDataModel->GetWorkingDir(),"DIAG",0,lptstrFN);
	CopyFile(
				  sDiagnosticFile, // name of an existing file
				  lptstrFN,         // name of new file
				  FALSE                     // operation fail if file exists
				);

	m_sDiagnosticFile=lptstrFN;
}


//##ModelId=3DDA0C4D0042
void CBatchTableRow::ProcessContourPlots(CString sWorkingDir)
{
	if(DSResult()->Space()->AllTablesEmpty())
		return;

	for(int j = 0 ; j < NR_OF_TABLES ; j++) // remove if already exists
	{
		if(FileExists(EnsureBackslash(sWorkingDir) + PlotFile[j] + CONTOUR_FILE_EXT))
			DeleteFile(EnsureBackslash(sWorkingDir) + PlotFile[j] + CONTOUR_FILE_EXT);
	}
	
	CString sFgvStartUpFile=EnsureBackslash(sWorkingDir) + FGV_STR;
	CString sTmpOldPath;
	
	std::list<long> lstDebCasInterface;
	std::list<long> lstDebFormInterface;
	
	DeleteFile(sFgvStartUpFile);


	m_pDataModel->GetFgvRunControl().SetMonitorCaption("Creating contours ...");

	sTmpOldPath = 	m_pDataModel->GetFgvRunControl().GetWorkingDir(); //remember old WorkingDir
	m_pDataModel->GetFgvRunControl().SetWorkingDir(sWorkingDir); 

	BOOL bFHasResults = DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->HasResults();
	BOOL bCHasResults = DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->HasResults();
	
	if(bFHasResults || bCHasResults)
	{
		WriteTmpContourFemStartupFile(sFgvStartUpFile);
		m_pDataModel->GetFgvRunControl().RunFemView(); //creates 2 files in working dir: cecasi + rocemi
		DeleteFile(sFgvStartUpFile);
	}
	
	if(bCHasResults)
		FillInterfaceList(EnsureBackslash(sWorkingDir) + "cecasi.fva", lstDebCasInterface);
	
	if(bFHasResults )
		FillInterfaceList(EnsureBackslash(sWorkingDir) + "rocemi.fva", lstDebFormInterface);

	bool bIsWrittenFem;
	WriteContourFemFile(sFgvStartUpFile,lstDebCasInterface,lstDebFormInterface, bIsWrittenFem);

	if(bIsWrittenFem)
	{
		m_pDataModel->GetFgvRunControl().RunFemView(); //creates (max) 6 contour files in working dir
	}

	DeleteFile(sFgvStartUpFile);
		
	
	for(int iTableNr = 0 ; iTableNr < NR_OF_TABLES ; iTableNr++)
	{
		CString sContourFileName = EnsureBackslash(sWorkingDir) + PlotFile[iTableNr] + CONTOUR_FILE_EXT;
		
		if(FileExists(sContourFileName))
		{
			CString sTmpFileName;
			CString sTblNr;
			sTblNr.Format("~CPF%d_",iTableNr);
			LPTSTR lptstrFN = sTmpFileName.GetBufferSetLength(1024);
			GetTempFileName(m_pDataModel->GetWorkingDir(),sTblNr,0,lptstrFN);
			CopyFile(
						  sContourFileName, // name of an existing file
						  lptstrFN,         // name of new file
						  FALSE                     // operation fail if file exists
						);

			m_vecContourPlotFile[iTableNr] = lptstrFN;
		}
		else
		{
			m_vecContourPlotFile[iTableNr] = "";
		}
	}

	m_pDataModel->GetFgvRunControl().SetWorkingDir(sTmpOldPath); //return to old WorkingDir
}


//##ModelId=3DDA0C4D0051
CString CBatchTableRow::GetContourFileName(int iTableNr)
{
	return m_vecContourPlotFile[iTableNr];
}


//##ModelId=3DDA0C4C039D
void CBatchTableRow::WriteTmpContourFemStartupFile(LPCSTR sFileName)
{
	FILE* file = fopen(sFileName,"w");
	
	if(!file)
		return ;

	BOOL bFHasResults = DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->HasResults();
	BOOL bCHasResults = DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->HasResults();

	if(bFHasResults)
	{
		WriteLine(file , "FEMVIEW COMPL");
		WriteLine(file , "VIEW MESH ROCEMI");
		WriteLine(file , "UTILITY WRITE MESH rocemi");
		WriteLine(file , "YES");
		WriteLine(file , "");
	}
	
	if(bCHasResults)
	{	
		WriteLine(file , "FEMVIEW COMPL");
		WriteLine(file , "VIEW MESH CECASI");
		WriteLine(file , "UTILITY WRITE MESH cecasi");
		WriteLine(file , "YES");
		WriteLine(file , "");
	}

	WriteLine(file,"STOP");
	WriteLine(file,"YES");
	WriteLine(file,"YES");

	fclose(file);
		
}

//##ModelId=3DDA0C4C0360
BOOL CBatchTableRow::WriteContourFemFile(LPCSTR sFileName, std::list<long> lstDebCasInterface,std::list<long> lstDebFormInterface, bool &bIsWrittenFem)
{
	FILE* file = fopen(sFileName,"w");
	
	if(!file)
		return FALSE;

	int iFstLoadPhaseIndex;
	CString sFirstDamageDbName;
	int iFirstDamagePhNb;
	int iMaxDamSTNB;
	int iMaxDamLONB;
	
	bIsWrittenFem =false;

	WriteLine(file , "UTILITY SETUP COLOURS INVERT"); 
	WriteLine(file , "UTILITY SETUP PLOTTER FORMAT NEUTRAL");

	if(DSResult()->Space()->GetSpaceTable(CEMENT_PLASTICITY_TABLE)->HasResults())
	{
		//shear deterioration in cement (Table 4 )
		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(CEMENT_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(CEMENT_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(CEMENT_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,CEMENT_PLASTICITY_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);
			
			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) ); 
			WriteLine(file , "VIEW MESH CEMENT");
			WriteLine(file , "EYE FRAME");
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			WriteLine(file , "RESULTS GAUSSIAN EL.EPEQS EPEQ");
			WriteLine(file , "PRESENT CONTOUR LEVELS");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[CEMENT_PLASTICITY_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}

	if(DSResult()->Space()->GetSpaceTable(CEMENT_RADIAL_CRACKING_TABLE)->HasResults())
	{
		//radial cracks (Table 5)
		long lNRadCrack = DSResult()->NRadCrack();
		if(lNRadCrack == -1)
			lNRadCrack = 2;

		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(CEMENT_RADIAL_CRACKING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(CEMENT_RADIAL_CRACKING_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(CEMENT_RADIAL_CRACKING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,CEMENT_RADIAL_CRACKING_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);
			CString sNRadCrack;
			sNRadCrack.Format("%d",lNRadCrack);
	
			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) ); 
			WriteLine(file , "VIEW MESH CEMENT");
			WriteLine(file , "EYE FRAME");
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			//WriteLine(file , "RESULTS GAUSSIAN EL.EKNN2 EKNN");
			WriteLine(file , "RESULTS GAUSSIAN EL.EKNN" + sNRadCrack + " EKNN");
			WriteLine(file , "PRESENT DISC");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[CEMENT_RADIAL_CRACKING_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}

	if(DSResult()->Space()->GetSpaceTable(CASING_PLASTICITY_TABLE)->HasResults())
	{
		//plastic deformation in casing (Table 1)
		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(CASING_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(CASING_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(CASING_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,CASING_PLASTICITY_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);

			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) ); 
			WriteLine(file , "VIEW MESH CASING");
			WriteLine(file , "EYE FRAME");
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			WriteLine(file , "RESULTS GAUSSIAN EL.EPEQS EPEQ");
			WriteLine(file , "PRESENT CONTOUR LEVELS");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[CASING_PLASTICITY_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}

	if(DSResult()->Space()->GetSpaceTable(FORMATION_PLASTICITY_TABLE)->HasResults())
	{
		//shear deterioration in formation (Table 0)
		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(FORMATION_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(FORMATION_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(FORMATION_PLASTICITY_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,FORMATION_PLASTICITY_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);

			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) );
			WriteLine(file , "VIEW MESH ROCKI");
			WriteLine(file , "EYE FRAME");
			WriteLine(file , "VIEW MESH ROCK");
			WriteLine(file , "EYE ZOOM FACTOR 0.3");
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			WriteLine(file , "RESULTS GAUSSIAN EL.EPEQS EPEQ");
			WriteLine(file , "PRESENT CONTOUR LEVELS");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[FORMATION_PLASTICITY_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}

	std::list<long> lstInterface;
	CString sTmpList;
	if(DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->HasResults())
	{
		
		//shear debonding at formation (Table 2)
		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(FORMATION_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,FORMATION_DEBONDING_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
		
			for(std::list<long>::iterator i = lstDebFormInterface.begin() ; i != lstDebFormInterface.end() ; i++)
			{	
				CString sTmp;
				sTmp.Format("%d",*i);

				sTmpList = sTmpList + sTmp + " ";
			}
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);

			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) );
			WriteLine(file , "CONSTRUCT LINE ELEMENTS LIST " + CString(sTmpList) ); 
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			WriteLine(file , "RESULTS GAUSSIAN EL.PTX.L PTX");
			WriteLine(file , "PRESENT OPTIONS GRAPH AXES YMIN 0");
			WriteLine(file , "PRESENT GRAPH LINE OLD");
			WriteLine(file , "PRESENT OPTIONS GRAPH RESIZE 0 0.4 0.6 0");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[FORMATION_DEBONDING_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}
	
	if(DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->HasResults())
	{
		
		//debonding at casing (Table 3)
		iMaxDamSTNB = DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageSTNB();
		iFstLoadPhaseIndex = DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetLoadPhaseIndex();
		iMaxDamLONB = DSResult()->Space()->GetSpaceTable(CASING_DEBONDING_TABLE)->GetLoadPhaseItem(0)->GetMaxDamageLONB();
		GetFemDbAndPhaseNb(iFstLoadPhaseIndex,CASING_DEBONDING_TABLE, iMaxDamLONB, sFirstDamageDbName,iFirstDamagePhNb);
		CString sFullDbName =EnsureBackslash(m_pDataModel->GetFgvRunControl().GetWorkingDir()) + sFirstDamageDbName + FGV_EXTENSION;
		if(FileExists(sFullDbName))
		{
			sTmpList.Empty();
			for(std::list<long>::iterator j = lstDebCasInterface.begin() ; j != lstDebCasInterface.end() ; j++)
			{
				CString sTmp;
				sTmp.Format("%d",*j);
				sTmpList = sTmpList + sTmp + " ";
			}
			
			CString sFirstDamagePhNb;
			sFirstDamagePhNb.Format("%d",iFirstDamagePhNb);
			CString sMaxDamSTNB;
			sMaxDamSTNB.Format("%d",iMaxDamSTNB);

			WriteLine(file , "FEMVIEW "  + CString(sFirstDamageDbName) );
			WriteLine(file , "CONSTRUCT LINE ELEMENTS LIST " + CString(sTmpList) ); 
			WriteLine(file , "RESULTS LOADCASE LC" + sFirstDamagePhNb + " " + sMaxDamSTNB);
			WriteLine(file , "RESULTS GAUSSIAN EL.PTX.L PTX");
			WriteLine(file , "PRESENT OPTIONS GRAPH AXES YMIN 0");
			WriteLine(file , "PRESENT GRAPH LINE OLD");
			WriteLine(file , "PRESENT OPTIONS GRAPH RESIZE 0 0.4 0.6 0");
			WriteLine(file , "DRAWING SAVE PLOTFILE " + CString(PlotFile[CASING_DEBONDING_TABLE]));
			WriteLine(file , "YES");
			WriteLine(file , "");
			bIsWrittenFem = true;
		}
	}
	
	if(bIsWrittenFem)
	{
		WriteLine(file,"STOP");
		WriteLine(file,"YES");
		WriteLine(file,"YES");
	}
	fclose(file);

	return TRUE;

}

//##ModelId=3DDA0C4C0330
void CBatchTableRow::GetFemDbAndPhaseNb(int i1stDamLoadPhase,int iTable, int iMaxDamLONB, CString &s1stDamDbName, int &i1stDamPhNb)
{
	s1stDamDbName = "";
	i1stDamPhNb = -1;

	switch(i1stDamLoadPhase)
	{

		case DRILLING: //drilling
			if(iTable == 0)
			{
				s1stDamDbName = FemViewDb[DRILN];
				i1stDamPhNb = 1;
			}
			break;
		case CURING: //curing
			switch(iTable)
			{
				case FORMATION_PLASTICITY_TABLE: //formation deterioration
					if(iMaxDamLONB == 3)
					{
						s1stDamDbName = FemViewDb[DRILN];
						i1stDamPhNb = 1;
					}
					else
					{
						s1stDamDbName = FemViewDb[COMPL];
						i1stDamPhNb = 4;
					}
					break;
				case CASING_PLASTICITY_TABLE: //casing plasticity
					if(iMaxDamLONB == 7 || iMaxDamLONB ==8)
					{
						s1stDamDbName = FemViewDb[CASIN];
						i1stDamPhNb = 3;

					}
					else
					{
						s1stDamDbName = FemViewDb[COMPL];
						i1stDamPhNb = 4;

					}
					break;
					

				case FORMATION_DEBONDING_TABLE:	//debonding at formation
				case CASING_DEBONDING_TABLE: //debonding at casing
				case CEMENT_PLASTICITY_TABLE: //cement deterioration
				case CEMENT_RADIAL_CRACKING_TABLE: //radial cracking
					s1stDamDbName = FemViewDb[COMPL];
					i1stDamPhNb = 4;
					break;

			}
			break;

		case PRESSURE_TEST: //pressure test
		case COMPLETION: //completion
		case SHUT_IN: //shut in
			s1stDamDbName = FemViewDb[COMPL];
			i1stDamPhNb = 4;
			break;

		case PRODUCTION: //production
			s1stDamDbName = FemViewDb[PRODU];
			i1stDamPhNb = 5;
			break;

		case INJECTION: //injection
			s1stDamDbName = FemViewDb[INJEC];
			i1stDamPhNb = 5;
			break;

		case FRAC_KILLING: //frac killing
			s1stDamDbName = FemViewDb[FRACT];
			i1stDamPhNb = 5;
			break;
		case EVAC_UNLOAD: //evac unload
			s1stDamDbName = FemViewDb[EVAC];
			i1stDamPhNb = 5;
			break;
				
	}
	
}

//##ModelId=3DDA0C4C0284
BOOL CBatchTableRow::FillInterfaceList(LPCSTR FileName, std::list<long> &lstInterface)
{
	
	CStdioFile vFile;
	bool b3CFound= false;
	//std::list<long> lstInterface;
	lstInterface.clear();

	// When the given file can be opened
	if(vFile.Open(FileName, CFile::modeRead | CFile::shareDenyWrite))
	{
		// Read all the lines (one by one)
		CString sLine;
		while(vFile.ReadString(sLine))
		{
			sLine.TrimLeft();
			sLine.TrimRight();

			if(sLine.Find("20C") != -1) //found
			{
				b3CFound = false;
				break;
			}

			if(sLine.Find("3C") != -1) //found
			{
				b3CFound = true;
				continue;
			}

			if(b3CFound)
			{
				std::vector<CString> results;
				if(StringUtils::SplitString(sLine, " ", results)) 
				{
					if(results[0] == "-1")
					{
						char* sDum;
						lstInterface.push_back(strtol(results[1],&sDum,0));
					}
				}
			}
		}		
		return TRUE;
	}
	else
      return FALSE;
}


/////
CBatchTableSelection::CBatchTableSelection(CBatchTableRow* pSelectedRow)
: m_pSelectedRow(pSelectedRow)
{
}

CBatchTableRow* CBatchTableSelection::SelectedRow() const
{
  return m_pSelectedRow;
}

void CBatchTableSelection::SelectRow(CBatchTableRow* pSelectedRow)
{
  m_pSelectedRow = pSelectedRow;
}
