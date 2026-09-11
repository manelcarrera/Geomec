// Copyright (C) 2001 -2002 TNO-BOUW

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "FormationTable.h"
#include "DrillingDirectionTable.h"
#include "DrillingOperationTable.h"
#include "ModelOverburdenTable.h"
#include "AnalysisPointTable.h"
#include "BatchTableTable.h"
#include "ControlTable.h"
#include "CementTable.h"
#include "PorePressureProfileTable.h"
#include "DensityProfileTable.h"
#include "TemperatureProfileTable.h"
#include "ModelTable.h"
#include "CementMaterialDatabase.h"
#include "CasingMaterialDatabase.h"
//#include "DSealectDoc.h"
#include "DSModelDatabase.h"
#include "FormationMaterialDatabase.h"
#include "RockRefinement.h"
#include "MeshParam.h"
#include "ReadWriteBlob.h"

#include "CDataModel.h"
#include "Global.h"
#include "MF.h"
#include "FgvProgressDlg.h"
#include "CFormationTopography.h"
#include "CAnalysis.h"
#include "DSealectGlobal.h"
#include "DSealectDoc.h"

///// CFgvRunner

CFgvRunner::CFgvRunner()
: m_bLog(false),
  m_bAutoClose(false),
  m_strExeName("idiana.exe")
{
}

void CFgvRunner::SetFgvPath(const CString& strFgvPath)
{
  m_strFgvPath = strFgvPath;
}

void CFgvRunner::SetExeName(const CString& strExeName)
{
  m_strExeName = strExeName;
}

void CFgvRunner::SetWorkingDir(const CString& strWorkingDir)
{
  m_strWorkingDir = strWorkingDir;
}

void CFgvRunner::SetMonitorLogfile(bool bLog)
{
  m_bLog = bLog;
}

void CFgvRunner::SetMonitorCaption(const CString& strCaption)
{
  m_strCaption = strCaption;
}

void CFgvRunner::SetAutoCloseDialog(bool bAuto)
{
  m_bAutoClose = bAuto;
}

const CString& CFgvRunner::GetWorkingDir() const
{
  return m_strWorkingDir;
}

bool CFgvRunner::RunFemView()
{
  bool bRet = true;

  // go to working dir
  char cwd[_MAX_PATH];
  getcwd(cwd, _MAX_PATH);
  chdir(m_strWorkingDir);

  putenv("FGVSTR=.");

  int hProcess = spawnl(_P_NOWAIT, EnsureBackslash(m_strFgvPath) + m_strExeName, "idiana", "-Alpha", "-NoConsoleWindow", 0);
  if(hProcess == -1)
    bRet = false;

  if(bRet)
  {
    CFgvProgressDlg dlg(hProcess, m_strCaption, AfxGetMainWnd());
    if(dlg.DoModal() != IDOK)
      bRet = false;
  }

  chdir(cwd);

  return bRet;
}



//file names
#define  MATERIAL_FILE "materi.dat"   //for each calculation (in sub dir)
#define  ANALYSIS_FILE "Analysis.dsi" //for each calculation (in sub dir)
#define  MESH_FILE     "mesh.dat"     //global (in root)
#define  MAXIMA_DSO    "Maxima.dso"

#define DEPWARNING "Not allowed : "

//version info
#define MODEL_VERSION  1
#define MODEL_REVISION 5

//##ModelId=3DDA0C480274
CDataModel::CDataModel(CString sTempDir)
: m_DrillingScheme(*this)
{
	InitModel();
	m_sWorkingDir = CreateSubWorkDir(sTempDir);
}

//##ModelId=3C0DDDAE0189
CDataModel::CDataModel()
: m_DrillingScheme(*this)
{
	InitModel();

	CString sTmpPath;
	LPTSTR lptstr = sTmpPath.GetBufferSetLength(1024);
	GetTempPath(1024,lptstr);	
	m_sWorkingDir = CreateSubWorkDir(lptstr);
	
	
	
}

//##ModelId=3DDA0C4800A0
void CDataModel::InitModel()
{
	m_VersionInfo = CVersionInfo(MODEL_VERSION,MODEL_REVISION);

	m_pFormationTopography=new CFormationTopography(this);
	m_Analysis = new CAnalysis(this);

	CDep1_1* pDep1_1 = new CDataModel::CDep1_1(this);
	m_DependencyVec.push_back(pDep1_1);
	DependenciesManager.AddDependency(pDep1_1);
	
	CDep1_2* pDep1_2 = new CDataModel::CDep1_2(this);
	m_DependencyVec.push_back(pDep1_2);
	DependenciesManager.AddDependency(pDep1_2);

	CDep1_3* pDep1_3 = new CDataModel::CDep1_3(this);
	m_DependencyVec.push_back(pDep1_3);
	DependenciesManager.AddDependency(pDep1_3);

	CDep2_1* pDep2_1 = new CDataModel::CDep2_1(this);
	m_DependencyVec.push_back(pDep2_1);
	DependenciesManager.AddDependency(pDep2_1);
/*
	CDep2_2* pDep2_2 = new CDataModel::CDep2_2(this);
	m_DependencyVec.push_back(pDep2_2);
	DependenciesManager.AddDependency(pDep2_2);

	CDep2_3* pDep2_3 = new CDataModel::CDep2_3(this);
	m_DependencyVec.push_back(pDep2_3);
	DependenciesManager.AddDependency(pDep2_3);
*/
	CDep3_1* pDep3_1 = new CDataModel::CDep3_1(this);
	m_DependencyVec.push_back(pDep3_1);
	DependenciesManager.AddDependency(pDep3_1);

	CDep4_1* pDep4_1 = new CDataModel::CDep4_1(this);
	m_DependencyVec.push_back(pDep4_1);
	DependenciesManager.AddDependency(pDep4_1);
/*	
	CDep4_2* pDep4_2 = new CDataModel::CDep4_2(this);
	m_DependencyVec.push_back(pDep4_2);
	DependenciesManager.AddDependency(pDep4_2);
*/
	CDep1* pDep1 = new CDataModel::CDep1(this);
	m_DependencyVec.push_back(pDep1);
	DependenciesManager.AddDependency(pDep1);

	CDep2* pDep2 = new CDataModel::CDep2(this);
	m_DependencyVec.push_back(pDep2);
	DependenciesManager.AddDependency(pDep2);

	CDep3* pDep3 = new CDataModel::CDep3(this);
	m_DependencyVec.push_back(pDep3);
	DependenciesManager.AddDependency(pDep3);

	CDep4* pDep4 = new CDataModel::CDep4(this);
	m_DependencyVec.push_back(pDep4);
	DependenciesManager.AddDependency(pDep4);

	CDep5* pDep5 = new CDataModel::CDep5(this);
	m_DependencyVec.push_back(pDep5);
	DependenciesManager.AddDependency(pDep5);

	m_pBoreHoleDep = new CDataModel::CDep6(this->DrillingScheme()->GetDrillingOperation(0)->BoreholeDiameter());
	DependenciesManager.AddDependency(m_pBoreHoleDep);

	m_pMeshParam  = new CMeshParam(this); 
	m_pBatchTable = new CBatchTable(this);
	m_pDianaX     = new CDianaX;

#if USE_SE40
  m_pFgvRunCtr  = new CFgvRunCtr;
#else
	m_pFgvRunCtr  = new CFgvRunner;
#endif


	m_pDianaXEventWnd = new CDianaXEventWnd;
	m_pDianaXEventWnd->Create();

	m_pDianaXEventWnd->AddSuccesMessageString("SUCCESSFUL STRUCTURAL ANALYSIS");  //success =0
	m_pDianaXEventWnd->AddSuccesMessageString("POST-PROCESSING MAXIMUM RESULTS"); //has results=1


	m_bIsCalculated = FALSE;
	m_bPolyLineCreated = FALSE;

	BOOL btemp;

	btemp = m_pDianaX->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),m_pDianaXEventWnd,IDC_DIANAX);
	m_pDianaX->SetShowDefaultMessages(TRUE);

}

BOOL CDataModel::InitInstance()
{
#if USE_SE40
	return m_pFgvRunCtr->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),AfxGetMainWnd(),IDC_FGVRUNCTR);
#else
  return TRUE;
#endif
}

//##ModelId=3C2043F402CF
CDataModel::~CDataModel()
{
	m_pDianaXEventWnd->DestroyWindow();
	delete m_pDianaXEventWnd;

	//m_pDianaX->CloseWindow();
	//m_pFgvRunCtr->CloseWindow();

	delete m_pMeshParam;
	delete m_Analysis	;
	delete m_pFormationTopography;
	delete m_pBatchTable;
	delete m_pDianaX;
	delete m_pFgvRunCtr;

	if(DirExists(m_sWorkingDir))
	{
		EmptyDirectory(m_sWorkingDir);
		RemoveDirectory(m_sWorkingDir);
	}

	DependenciesManager.RemoveDependency(m_pBoreHoleDep);
	delete m_pBoreHoleDep;
	

	for(int i=0 ; i< m_DependencyVec.size();i++)
	{
		DependenciesManager.RemoveDependency(m_DependencyVec[i]);
		delete m_DependencyVec[i];
	}
}

//##ModelId=3C60D56A011A
CMaterialListTemplate<CFormationMaterial>* CDataModel::FormationMatList()
{
	return & m_FormationMatList;
}

const CMaterialListTemplate<CFormationMaterial>* CDataModel::FormationMatList() const
{
	return & m_FormationMatList;
}

//##ModelId=3C60D56A0158
CMaterialListTemplate<CCasingMaterial>* CDataModel::CasingMatList()
{
	return & m_CasingMatList;
}

//##ModelId=3C60D56A01A6
CMaterialListTemplate<CCementMaterial>* CDataModel::CementMatList()
{
	return & m_CementMatList;
}

//##ModelId=3C0DDDAE018A
CFormationTopography *CDataModel::FormationTopography()
{
	return m_pFormationTopography;
}

const CFormationTopography *CDataModel::FormationTopography() const
{
	return m_pFormationTopography;
}

//##ModelId=3C0DDDAE0197
CControl *CDataModel::Control()
{
	return &m_Control;
}

//##ModelId=3C8D9E08034D
CBatchTable *CDataModel::BatchTable()
{
	return m_pBatchTable;
}

//##ModelId=3C0DDDAE0198
CDrillingScheme *CDataModel::DrillingScheme()
{
	return &m_DrillingScheme;
}

//##ModelId=3C0DDDAE0199
CAnalysis *CDataModel::Analysis()
{
	return m_Analysis;
}

//##ModelId=3C0DDDAE019A
bool CDataModel::CheckComplete() const
{
	return m_bComplete;
	
}

//##ModelId=3C0DDDAE019B
CString CDataModel::GetInfo()
{
	return m_strInfo;
}

//##ModelId=3C8D9E02038C
CVersionInfo CDataModel::VersionInfo()
{
	return m_VersionInfo;
}

//##ModelId=3DDA0C480071
CString CDataModel::CreateSubWorkDir(CString sWorkDir)
{
	for(int i = 0 ; i < 1000 ; i++)
	{
		CString sFrmt;
		sFrmt.Format("DS%d",i);
		CString sDir = EnsureBackslash(sWorkDir) + sFrmt;
		if(!DirExists( sDir))
		{
			CreateDirectory(sDir);
			return sDir;
		}
	}
	assert(FALSE);
	return"";

}

//##ModelId=3C2043F4007D
CLengthQuantity& CDataModel::GetModelBottomTVD()
{
	
	if(FormationTopography()->NrOfFormations())
	{
		m_qModelBottomTVD = *FormationTopography()->GetFormationAt(FormationTopography()->NrOfFormations()-1)->BottomTVD();
	}
	else
	{
		m_qModelBottomTVD.Invalidate();
	}
	return m_qModelBottomTVD;
}

//##ModelId=3C2043F4003F
CLengthQuantity& CDataModel::GetModelBottomTMD()
{
	
	if(!GetModelBottomTVD().Undefined())
	{
		m_qModelBottomTMD.Value(m_PolyLine.GetLength()/*+DrillingScheme()->OverburdenBottomTMD()->Value()*/,CDoubleQuantity::SI_UNIT);
	}
	else
	{
		m_qModelBottomTMD.Invalidate();
	}

	return m_qModelBottomTMD;
}

//##ModelId=3C2043F303D9
CLengthQuantity& CDataModel::GetModelTopTVD()
{
	m_qModelTopTVD=*FormationTopography()->Overburden()->OverburdenBottomTVD();
	return m_qModelTopTVD;
}

//##ModelId=3C2043F3038B
CLengthQuantity& CDataModel::GetModelTopTMD()
{	
	//ModelTopTMD= Max(OverburdenBottomTMD, TopOfCementTMD)
	CLengthQuantity ret1;
	CLengthQuantity ret2;
	ret1= *DrillingScheme()->OverburdenBottomTMD();
	ret2= *DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD();
	
	if((!ret1.Undefined())&&(!ret2.Undefined()))
	{
		if(ret1.Value() > ret2.Value()) // find maximum
		{	
			m_qModelTopTMD=ret1;
			return m_qModelTopTMD;
		}
		else
		{
			m_qModelTopTMD= ret2;
			return m_qModelTopTMD;
		}
	}
	else
	{	
		if((ret1.Undefined())&&(!ret2.Undefined()))
		{
			m_qModelTopTMD= ret2;
			return m_qModelTopTMD;
		}
		if((!ret1.Undefined())&&(ret2.Undefined()))
		{
			m_qModelTopTMD= ret1;
			return m_qModelTopTMD;
		}
	}
	m_qModelTopTMD.Invalidate();
	return m_qModelTopTMD;
		
}

//##ModelId=3C2043F40168
// second is false (and first undefined) if length > wellpath length or length < 0
std::pair<double, bool> CDataModel::TMDtoTVD(const double &length)
{
  std::pair<double, bool> prRet;

  if(length <= m_PolyLine.GetLength())
  {
	  geo::CPoint tmp=m_PolyLine.GetPointAtLength(length);
	  assert(!tmp.Empty());
    prRet = std::make_pair(tmp.Z(), true);
  }
  else
    prRet.second = false;

  return prRet;
}

//##ModelId=3CA0119103D8
bool CDataModel::IsInTail(double dDepthTMD)
{
	CDrillingOperation *pDO = DrillingScheme()->GetDrillingOperation(0);
	
	bool bLTC = pDO->CementingOperation()->LeadTailCements();

	if(bLTC)
	{
		if(dDepthTMD < pDO->CementingOperation()->TopOfTailCement()->Value())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

//##ModelId=3DDA0C4801C9
void CDataModel::InvalidatePolyLine()
{
	m_bPolyLineCreated = FALSE;
	
}

//##ModelId=3DDA0C4801D8
BOOL CDataModel::IsPolyLineCreated()
{
	return m_bPolyLineCreated;
	
}

//##ModelId=3CA0118F0242
void CDataModel::InvalidateResults()
{
	m_pBatchTable->DeleteBatchTable();
}

//##ModelId=3C2043F4011A
BOOL CDataModel::CanCreatePolyLine()
{
	if(!FormationTopography()->NrOfFormations())
		return FALSE;

	int i;
	for(i=0;i< DrillingScheme()->NrOfDrillingDirections();i++)
	{
		if(! DrillingScheme()->GetDrillingDirectionSection(i)->CheckComplete())
			return FALSE;
	}

	if( FormationTopography()->Overburden()->OverburdenBottomTVD()->Undefined())
		return FALSE;

	if( GetModelBottomTVD().Undefined())
		return FALSE;

	return TRUE;
}

//##ModelId=3C8D9E0802B1
CMeshParam *CDataModel::MeshParam()
{
	m_pMeshParam->UpdateData();
	return m_pMeshParam;
}
//##ModelId=3C2043F400CC
BOOL CDataModel::CreatePolyLine()
{
	ASSERT(CanCreatePolyLine());
	
	if(m_PolyLine.PointSize() > 0)
		m_PolyLine.RemoveAll();

  double md = 0;
  double x = 0;
  double y = 0;
  double z = 0;

  geo::CPoint ptPrev = geo::CPoint(x, y, z);
  m_PolyLine.PushBack(ptPrev);

  for(int i = 0; i < DrillingScheme()->NrOfDrillingDirections(); ++i)
  {
    double nextmd = DrillingScheme()->GetDrillingDirectionSection(i)->DrillingSectionBottomTMD()->Value();
    double dtmd = nextmd - md;
    double inc = DrillingScheme()->GetDrillingDirectionSection(i)->WellInclination()->Value() * PI / 180.;
    double azi = DrillingScheme()->GetDrillingDirectionSection(i)->AzimuthWithNorth()->Value() * PI / 180.;

    double l = dtmd * sin(inc);
    double dz = dtmd * cos(inc);

    double dx = l * sin(azi);
    double dy = l * cos(azi);

    x += dx;
    y += dy;
    z += dz;

    geo::CPoint pt(x, y, z);
    if(!(pt == ptPrev))
    {
      m_PolyLine.PushBack(pt);
      ptPrev = pt;
    }

    md = nextmd;
  }

  m_bPolyLineCreated = TRUE;
	
	return TRUE;
}

//##ModelId=3DDA0C4802D2

BOOL CDataModel::LoadFromDatabase(CString strDatabaseName)
{
	m_sDatabaseName = strDatabaseName;

	BOOL bPorousIsChanged = FALSE;

	CDSModelDatabase database;
	database.Open(strDatabaseName,TRUE,TRUE);
	
	if(database.VersionInfo() < VersionInfo())
	{
		if(database.VersionInfo() < CVersionInfo(0,3))
		{
			CString msg = "version ";
			msg += database.VersionInfo().GetVersionString();
			msg += " is unsupported";
			AfxMessageBox(msg,0,0);
			database.Close();
			return FALSE;
		}
		else if(database.VersionInfo() == CVersionInfo(0,3))
		{
			CString msg = "version ";
			msg += database.VersionInfo().GetVersionString();
			msg += " could be unconsistent with current version, check: \n";
			msg += " percentage eccentricity\n";
			msg += " cement hydration volume change\n";
			msg += " hook load";
			
			AfxMessageBox(msg,0,0);
		
		}
		else if(database.VersionInfo() == CVersionInfo(0,4))
		{
			CString msg = "version ";
			msg += database.VersionInfo().GetVersionString();
			msg += " could be unconsistent with current version \n";
			AfxMessageBox(msg,0,0);
		}
		else if(database.VersionInfo() < CVersionInfo(1,2))
		{
			CString msg = "version ";
			msg += database.VersionInfo().GetVersionString();
			msg += " could be inconsistent with current version \n\n";
			msg += "Undrained option is no longer supported and is automatically changed to drained option.\n\n";
			msg += "For the US- and Field-unit system the thermal gradient unit is changed!";
			AfxMessageBox(msg,0,0);
		}
    else if(database.VersionInfo() < CVersionInfo(1, 3))
    {
      CString msg = "Version ";
      msg += database.VersionInfo().GetVersionString();
      msg += " contains test pressure values per analysis point. ";
      msg += "These values are ignored in this version. Instead a test pressure value ";
      msg += "at surface level and the fluid pressure gradient are used to ";
      msg += "calculate the test pressure in each point.\n\n";
      msg += "Please check your input.";
      AfxMessageBox(msg);
    }
	}
	else if(database.VersionInfo() > VersionInfo())
	{
		CString msg = "version ";
		msg += database.VersionInfo().GetVersionString();
		msg += " is not yet supported";
		AfxMessageBox(msg,0,0);
		database.Close();
		return FALSE;
	}

		
	//read model overburden from database
	database.ModelOverburden().Open();
	if(!database.ModelOverburden().RS().IsBOF())
	{
		database.ModelOverburden().RS().MoveFirst();
		
		BOOL bTemp;
		database.ModelOverburden().GetField(MODEL_OVERBURDEN_OFFSHORE_DEFINITION,bTemp);
		FormationTopography()->Offshore(bTemp==TRUE);
		database.ModelOverburden().GetField(MODEL_OVERBURDEN_ELEVATION,	*(FormationTopography()->ElevationTVD()));
		database.ModelOverburden().GetField(MODEL_OVERBURDEN_OVERBURDEN_BOTTOM_TVD,		*(FormationTopography()->Overburden()->OverburdenBottomTVD()));
		database.ModelOverburden().GetField(MODEL_OVERBURDEN_SURFACE_UNDIST_TEMP,		*(FormationTopography()->Overburden()->SurfaceTemperature()));
		database.ModelOverburden().GetField(MODEL_OVERBURDEN_OB_GEOTHERMAL_GRAD,		*(FormationTopography()->Overburden()->OBGeothermalGradient()));

    if(database.VersionInfo() >= CVersionInfo(1, 5))
    {
      // load average density
      database.ModelOverburden().GetField(MODEL_OVERBURDEN_AVERAGE_DENSITY, *FormationTopography()->Overburden()->OBAverageDensity());
    }
    else
    {
      // load average stress gradient and convert to density
      CStressGradientQuantity qOBGradient;
  		database.ModelOverburden().GetField(MODEL_OVERBURDEN_OB_AVER_VERT_STRESS_GRAD, qOBGradient);
      if(!qOBGradient.Undefined())
        FormationTopography()->Overburden()->OBAverageDensity()->SetValue(1./9.81*qOBGradient.Value()); // MPa/m -> Mkg/m3
    }

    if(database.VersionInfo() <= CVersionInfo(1, 2))
    {
      CLengthQuantity qnSeaBedTVD;
		  database.ModelOverburden().GetField(MODEL_OVERBURDEN_SEABED_TVD,	qnSeaBedTVD);
      if(!qnSeaBedTVD.Undefined() && !FormationTopography()->ElevationTVD()->Undefined())
        FormationTopography()->Seawater()->WaterDepth()->SetValue(qnSeaBedTVD.Value() - FormationTopography()->ElevationTVD()->Value());
    }
    else
    {
      database.ModelOverburden().GetField(MODEL_OVERBURDEN_WATERDEPTH, *(FormationTopography()->Seawater()->WaterDepth()));
    }

		database.ModelOverburden().GetField(MODEL_OVERBURDEN_WATER_DENSITY,				*(FormationTopography()->Seawater()->Waterdensity()));
		
	}	
	database.ModelOverburden().Close();


	FormationTopography()->RemoveAllFormations();

  // when (old) database contains stress gradients instead of densities convert these gradients
  // to densities and map them to the material names, so they can be imported into the materials later on
  std::map<CString, double> mpConvertedDensities;

	//read formation data from database
	database.Formation().Open();
	if (!database.Formation().RS().IsBOF())
	{
		database.Formation().RS().MoveFirst();
		if(FormationTopography()->CanAddFormations())
		{
			int i=0;
			while(!database.Formation().RS().IsEOF())
			{
				
				CString strName,strErr;
				double dblTVD;
				database.Formation().GetField(FORMATION_NAME,strName);
				
				database.Formation().GetField(FORMATION_BOTTOM_TVD,dblTVD);
				FormationTopography()->AddFormation(strName,dblTVD,strErr);

				database.Formation().GetField(FORMATION_MATERIAL_NAME,strName);
				FormationTopography()->GetFormationAt(i)->MaterialName(strName);

        if(database.VersionInfo() < CVersionInfo(1, 5))
        {
          CStressGradientQuantity qsg;
				  database.Formation().GetField(FORMATION_VERT_STRESS_GRAD, qsg);
          if(!qsg.Undefined())
            mpConvertedDensities.insert(std::map<CString, double>::value_type(strName, 1./9.81*qsg.Value()));
        }
				database.Formation().GetField(FORMATION_GEOTHERMAL_GRAD,		*(FormationTopography()->GetFormationAt(i)->GeothermalGradient()));
				
				database.Formation().GetField(FORMATION_MAX_HOR_STRESS_RATIO,	*(FormationTopography()->GetFormationAt(i)->MaxHorizontalStressRatio()));
				database.Formation().GetField(FORMATION_MIN_HOR_STRESS_RATIO,	*(FormationTopography()->GetFormationAt(i)->MinHorizontalStressRatio()));
				database.Formation().GetField(FORMATION_AZIMUTH_SH_WITH_NORTH,	*(FormationTopography()->GetFormationAt(i)->AzimuthOfSHWithNorth()));
				
				BOOL bTemp;
				database.Formation().GetField(FORMATION_POROUS,bTemp);	
				if (bTemp == FALSE)
				{
					bTemp = TRUE; // porous = true because the drained option is automatically set
					bPorousIsChanged = TRUE;
				}
				FormationTopography()->GetFormationAt(i)->Porous(bTemp==TRUE);
				
				database.Formation().GetField(FORMATION_TOP_PORE_PRESSURE,		*(FormationTopography()->GetFormationAt(i)->TopPorePressure()));
				database.Formation().GetField(FORMATION_BOTTOM_PORE_PRESSURE,	*(FormationTopography()->GetFormationAt(i)->BottomPorePressure()));
				
				i++;
				database.Formation().RS().MoveNext();
			}
		}
	}
	database.Formation().Close();

	CDrillingOperation        *pDO   = DrillingScheme()->GetDrillingOperation(0);
	CCasingOperation          *pCaO  = pDO->CasingOperation();
	CCementingOperation       *pCeO  = pDO->CementingOperation();

	//read drilling operation from database
	database.DrillingOperation().Open();
	if(!database.DrillingOperation().RS().IsBOF())
	{
		database.DrillingOperation().RS().MoveFirst();
		
		BOOL bTemp;
		database.DrillingOperation().GetField(DRILLING_OPERATION_BOTTOM_TMD,					*(pDO->OperationBottomTMD()));					
		database.DrillingOperation().GetField(DRILLING_OPERATION_BOREHOLE_DIAMETER,				*(pDO->BoreholeDiameter()));			
		database.DrillingOperation().GetField(DRILLING_OPERATION_MUD_DENSITY_AT_SURFACE,		*(pDO->MudDensityAtSurface()));					
		database.DrillingOperation().GetField(DRILLING_OPERATION_MUD_COMPRESSIBILITY,			*(pDO->MudCompressibility()));						
		database.DrillingOperation().GetField(DRILLING_OPERATION_CASING_AXIAL_STRESSES,bTemp);
		pCaO->AdvancedAxialCasingStresses(bTemp==TRUE);
		
		database.DrillingOperation().GetField(DRILLING_OPERATION_HOOK_LOAD,						*(pCaO->HookLoad()));					
		database.DrillingOperation().GetField(DRILLING_OPERATION_WORK_STRING_USED, bTemp);
		pCaO->WorkStringUsed(bTemp==TRUE);
		
		database.DrillingOperation().GetField(DRILLING_OPERATION_DISP_FLUID_STRESS_GRAD,		*(pCaO->DisplacementFluidStressGradient()));		
		database.DrillingOperation().GetField(DRILLING_OPERATION_DISP_FLUID_COMPRESSIBILITY,	*(pCaO->DispFluidCompressibility()));	
		database.DrillingOperation().GetField(DRILLING_OPERATION_WORK_STRING_PACKERS_TMD,		*(pCaO->WorkStringPackersTMD()));		
		database.DrillingOperation().GetField(DRILLING_OPERATION_NEW_FLUID_IN_ANNULUS,bTemp);
		pCaO->NewFluidInCasWSAnnulus(bTemp==TRUE);			

		database.DrillingOperation().GetField(DRILLING_OPERATION_NEW_FLUID_STRESS_GRAD,			*(pCaO->NewFluidStressGradient()));				
		database.DrillingOperation().GetField(DRILLING_OPERATION_NEW_FLUID_OVERPRESSURE,		*(pCaO->NewFluidOverpressure()));						
		database.DrillingOperation().GetField(DRILLING_OPERATION_NEW_FLUID_COMPRESSIBILITY,		*(pCaO->NewFluidCompressibility()));					
		database.DrillingOperation().GetField(DRILLING_OPERATION_PERCENTAGE_ECCENTRICITY,		*(pCaO->PercentageEccentricity()));							
		database.DrillingOperation().GetField(DRILLING_OPERATION_GUIDE_SHOE_DEPTH_ALONG_HOLE,	*(pCaO->GuideShoeTMD()));						
		database.DrillingOperation().GetField(DRILLING_OPERATION_TOP_OF_CEMENT_TMD,				*(pCeO->TopOfCementTMD()));			
		database.DrillingOperation().GetField(DRILLING_OPERATION_PRESSURE_FROM_FLUIDS_ABOVE,	*(pCeO->PressureFromFluidsAbove()));	
		database.DrillingOperation().GetField(DRILLING_OPERATION_LEAD_AND_TAIL_CEMENTS,bTemp);
		pCeO->LeadTailCements(bTemp==TRUE);
		
		database.DrillingOperation().GetField(DRILLING_OPERATION_TOP_OF_TAIL_CEMENT_TMD,		*(pCeO->TopOfTailCement()));		
		database.DrillingOperation().GetField(DRILLING_OPERATION_LEAD_CEMENT_STRESS_GRAD,		*(pCeO->LeadCementStressGradient()));		
		database.DrillingOperation().GetField(DRILLING_OPERATION_TAIL_CEMENT_STRESS_GRAD,		*(pCeO->TailCementStressGradient()));		

		CString CasingMatName;
		database.DrillingOperation().GetField(DRILLING_OPERATION_CASING_MATERIAL_NAME,		    CasingMatName);
		pCaO->CasingMaterialName(CasingMatName);
	}	
	database.DrillingOperation().Close();

	//read drilling direction from database
	database.DrillingDirection().Open();
	if(!database.DrillingDirection().RS().IsBOF())
	{
		database.DrillingDirection().RS().MoveFirst();

    int iSection = 0;
    double dTMD = 0;
    while(!database.DrillingDirection().RS().IsEOF())
    {
      if(iSection >= DrillingScheme()->NrOfDrillingDirections())
        DrillingScheme()->AddDrillingDirectionSection();

      CDrillingDirectionSection *pDDS = DrillingScheme()->GetDrillingDirectionSection(iSection);

      pDDS->DrillingSectionTopTMD()->SetValue(dTMD);
		  database.DrillingDirection().GetField(DRILLING_DIR_BOTTOM_TMD, *pDDS->DrillingSectionBottomTMD());

      if(!pDDS->DrillingSectionBottomTMD()->Undefined())
        dTMD = pDDS->DrillingSectionBottomTMD()->Value();

		  database.DrillingDirection().GetField(DRILLING_DIR_WELL_INCLINATION	,	*(pDDS->WellInclination()));
		  database.DrillingDirection().GetField(DRILLING_DIR_AZIMUTH_WITH_NORTH,	*(pDDS->AzimuthWithNorth()));

      ++iSection;
      database.DrillingDirection().RS().MoveNext();
    }

    if(database.VersionInfo() <= CVersionInfo(1, 2))
    {
      // only a single drilling section, bottom TMD = model bottom TMD
      ASSERT(DrillingScheme()->NrOfDrillingDirections() == 1);
      CDrillingDirectionSection* pDDS = DrillingScheme()->GetDrillingDirectionSection(0);
      if(pDDS->DrillingSectionBottomTMD()->Undefined())
      {
        double inc = pDDS->WellInclination()->Value() * PI / 180.;
        double dBottomTMD = GetModelBottomTVD().Value() / cos(inc);
        pDDS->DrillingSectionBottomTMD()->SetValue(dBottomTMD);
      }
    }

	}	
	database.DrillingDirection().Close();


	//read cement scenario material names
	database.Cement().Open();
	long iNrOfCementScenarios=0;
	if (!database.Cement().RS().IsBOF())
	{
		database.Cement().RS().MoveFirst();
		while(!database.Cement().RS().IsEOF())
		{
			pDO->CementScenariosTable()->NumberOfCementScenarios(iNrOfCementScenarios+1);
			CString CementMatNameHead;
			CString CementMatNameTail;
			CCementScenario *pCS=pDO->CementScenariosTable()->CementScenarioAt(iNrOfCementScenarios);
			database.Cement().GetField(CEMENT_HEAD_MATERIAL_NAME,CementMatNameHead);
			database.Cement().GetField(CEMENT_TAIL_MATERIAL_NAME,CementMatNameTail);
			pCS->HeadMaterialName(CementMatNameHead);
			pCS->TailMaterialName(CementMatNameTail);
			iNrOfCementScenarios++;
			database.Cement().RS().MoveNext();
		}
	}

	//read analysis points from database
	Analysis()->RemoveAllAnalysisPoints();
	
	database.AnalysisPoint().Open();
	if (!database.AnalysisPoint().RS().IsBOF())
	{
		database.AnalysisPoint().RS().MoveFirst();
		//if(Analysis()->FormationTopography()->CanAddFormations())
		{
			int j=0;
			while(!database.AnalysisPoint().RS().IsEOF())
			{
				
				CString strErr;
				double dblTMD;
				database.AnalysisPoint().GetField(ANALYSIS_POINT_TMD,dblTMD);
				Analysis()->AddAnalysisPoint(dblTMD,strErr);

				database.AnalysisPoint().GetField(ANALYSIS_POINT_DRILLING_FLUID_TEMP,					*(Analysis()->GetAnalysisPointAt(j)->PriorToCompletionLoads()->DrillingTemperature()));					
				database.AnalysisPoint().GetField(ANALYSIS_POINT_TIME_OF_EXPOS_TO_DRIL_FLUID,			*(Analysis()->GetAnalysisPointAt(j)->PriorToCompletionLoads()->TimeOfExposureToDrillingFluid()));								
				//database.AnalysisPoint().GetField(ANALYSIS_POINT_TEST_PRESSURE,							*(Analysis()->GetAnalysisPointAt(j)->TestLoads()->TestPressure()));						
				database.AnalysisPoint().GetField(ANALYSIS_POINT_COMPLETION_PRESSURE_DIFF,				*(Analysis()->GetAnalysisPointAt(j)->PerforationLoads()->CompletionPressureDiff()));				
				//database.AnalysisPoint().GetField(ANALYSIS_POINT_PRODUCTION_PORE_PRESSURE,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->ProductionPorePressure()));				
				database.AnalysisPoint().GetField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->BottomHoleFlowingPressure()));			
				database.AnalysisPoint().GetField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_TEMP,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->BottomHoleFlowingTemp()));				
				database.AnalysisPoint().GetField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_PROD,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->TimeOfContinuousProduction()));				
				database.AnalysisPoint().GetField(ANALYSIS_POINT_MAXIMUM_INJECTION_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->MaximumInjectionPressure()));							
				database.AnalysisPoint().GetField(ANALYSIS_POINT_INJECTION_TEMPERATURE,					*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->InjectionTemperature()));											
				database.AnalysisPoint().GetField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_INJECTION,			*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->TimeOfContinuousInjection()));										
				database.AnalysisPoint().GetField(ANALYSIS_POINT_MAX_INJECTING_FRAC_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->MaxInjectingFracPressure()));
				database.AnalysisPoint().GetField(ANALYSIS_POINT_INJECTION_FRAC_TREATING_FLUID_TEMP,	*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->TreatingFluidTemp()));	
				database.AnalysisPoint().GetField(ANALYSIS_POINT_INJECTION_FRAC_TIME_OF_EXPOSURE,		*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->TimeOfExposureToTreatingFluid()));		
				database.AnalysisPoint().GetField(ANALYSIS_POINT_BOTTOM_HOLE_EVAC_TREATING_PRESSURE,	*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->BottomHoleTreatingPressure()));			
				database.AnalysisPoint().GetField(ANALYSIS_POINT_EVAC_TREATING_FLUID_TEMP,				*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->TreatingFluidTempEvac()));						
				database.AnalysisPoint().GetField(ANALYSIS_POINT_EVAC_TIME_OF_EXPOSURE,					*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->TimeOfExposureToTreatingFluidEvac()));						
		
				j++;
				database.AnalysisPoint().RS().MoveNext();
			}
		}
	}
	 
	database.AnalysisPoint().Close();


	//read model overburden from database
	database.Control().Open();
	if(!database.Control().RS().IsBOF())
	{
		database.Control().RS().MoveFirst();
		
		BOOL bTemp;
		long lngTemp;
		database.Control().GetField(CONTROL_WELL_TYPE, lngTemp);
		Control()->WellType(CControl::eWellType(lngTemp));						
		database.Control().GetField(CONTROL_BALANCED_COMPLETION,bTemp);
		Control()->BalancedCompletion(bTemp==TRUE);				
		database.Control().GetField(CONTROL_UNDERBALANCED_COMPLETION,bTemp);
		Control()->UnderBalancedCompletion(bTemp==TRUE);						
		database.Control().GetField(CONTROL_OVERBALANCED_COMPLETION,bTemp);
		Control()->OverBalancedCompletion(bTemp==TRUE);							
		database.Control().GetField(CONTROL_CONVERSION,bTemp);
		Control()->Conversion(bTemp==TRUE);										
		database.Control().GetField(CONTROL_FRACTURING,bTemp);
		Control()->Fracturing(bTemp==TRUE);				
		database.Control().GetField(CONTROL_EVACUATION,bTemp);
		Control()->Evacuation(bTemp==TRUE);				
		database.Control().GetField(CONTROL_DEFAULT_MESH_REFINEMENT,bTemp);
		Control()->DefaultMeshRefinement(bTemp==TRUE);				
		database.Control().GetField(CONTROL_ROCK_REFINEMENT	,lngTemp);
		Control()->RockRefinement(lngTemp);				
		database.Control().GetField(CONTROL_CASING_REFINEMENT,lngTemp);
		Control()->CasingRefinement(lngTemp);				
		database.Control().GetField(CONTROL_CEMENT_REFINEMENT,lngTemp);
		Control()->CementRefinement(lngTemp);				
		database.Control().GetField(CONTROL_OUTPUT_LEVEL,lngTemp);
		Control()->OutputLevel(lngTemp);

    if(database.VersionInfo() >= CVersionInfo(1, 3))
    {
      database.Control().GetField(CONTROL_TEST_PRESSURE, *m_TestLoads.TestPressure());
      database.Control().GetField(CONTROL_FLUID_PRESSURE_GRADIENT, *m_TestLoads.FluidPressureGradient());
    }

		if(database.FieldExistsInTable(CONTROL_TABLE, CONTROL_SHUT_IN))
		{
			database.Control().GetField( CONTROL_SHUT_IN,bTemp);
			Control()->ShutIn(bTemp==TRUE);
		}
		else
		{
			Control()->ShutIn(true);
		}

	}	
	database.Control().Close();
	database.Close();

	if(CanCreatePolyLine())
		CreatePolyLine();
		
	//load formation material	
	CFormationMaterialDatabase fmd;
	fmd.Database().Open(strDatabaseName,TRUE,TRUE,NULL);
	fmd.MaterialTable().Open();
	if(fmd.MaterialTable().RS().GetRecordCount()>0)
	{
	fmd.MaterialTable().RS().MoveFirst();

		while(!fmd.MaterialTable().RS().IsEOF())
		{
			CString strName;
			CFormationMaterial fm;
			fmd.MaterialTable().GetField(FORMATION_MATERIAL_NAME,strName);
			fmd.GetMaterial(strName,fm);

      // try to set converted density from older database
      std::map<CString, double>::const_iterator it = mpConvertedDensities.find(strName);
      if(it != mpConvertedDensities.end())
        fm.Density()->SetValue(it->second);

			FormationMatList()->Add(fm);
			fmd.MaterialTable().RS().MoveNext();
		}
	}
	fmd.Database().Close();
	
	//load casing material
	CasingMatList()->RemoveAll();
	CCasingMaterialDatabase cmd;
	cmd.Database().Open(strDatabaseName,TRUE,TRUE,NULL);
	cmd.MaterialTable().Open();
	if(cmd.MaterialTable().RS().GetRecordCount()>0)
	{
	cmd.MaterialTable().RS().MoveFirst();

		while(!cmd.MaterialTable().RS().IsEOF())
		{
			CString strName;
			CCasingMaterial cm;
			cmd.MaterialTable().GetField(CASING_MATERIAL_NAME,strName);
			cmd.GetMaterial(strName,cm);
			CasingMatList()->Add(cm);
			cmd.MaterialTable().RS().MoveNext();
		}
	}
	cmd.Database().Close();


	//load cement material
	CementMatList()->RemoveAll();
	CCementMaterialDatabase cemd;
	cemd.Database().Open(strDatabaseName,TRUE,TRUE,NULL);
	cemd.MaterialTable().Open();
	if(cemd.MaterialTable().RS().GetRecordCount()>0)
	{
	cemd.MaterialTable().RS().MoveFirst();

		while(!cemd.MaterialTable().RS().IsEOF())
		{
			CString strName;
			CCementMaterial cem;
			cemd.MaterialTable().GetField(CEMENT_MATERIAL_NAME,strName);
			cemd.GetMaterial(strName,cem);
			CementMatList()->Add(cem);
			cemd.MaterialTable().RS().MoveNext();
		}
	}
	cemd.Database().Close();
	
	
	database.Open(strDatabaseName,TRUE,TRUE);
	//read Batchtable rows from database
	BatchTable()->Invalidate();
	
	if(BatchTable()->CanBuildBatchTable())
	{
		BatchTable()->BuildBatchTable();
	}
	
	
	if(BatchTable()->IsBuild() && 
	   database.TableExists(BATCHTABLE_TABLE) &&
	   database.VersionInfo() >= CVersionInfo(1,0) )
	{
	
		database.BatchTable().Open();
		if (!database.BatchTable().RS().IsBOF())
		{
			BOOL bTemp;
			long lngTemp;
			database.BatchTable().RS().MoveFirst();
			{
				int j=0;
				while(!database.BatchTable().RS().IsEOF())
				{
					database.BatchTable().GetField(BATCHTABLE_USE_FOR_CALCULATION,bTemp);
					BatchTable()->GetRow(j)->UseForCalculation(bTemp == TRUE);
					database.BatchTable().GetField(BATCHTABLE_IS_CALCULATED,bTemp);
					BatchTable()->GetRow(j)->IsCalculated(bTemp == TRUE);				
					database.BatchTable().GetField(BATCHTABLE_CALCULATION_SUCCEEDED,bTemp);	
					BatchTable()->GetRow(j)->CalculationSucceeded(bTemp == TRUE);
					
					if(database.FieldExistsInTable(BATCHTABLE_TABLE,BATCHTABLE_HASRESULTS))
						if(bPorousIsChanged) // if porous is automatically changed; invalidate results
						{
							bTemp = FALSE;
						}
						else
						{
							database.BatchTable().GetField(BATCHTABLE_HASRESULTS,bTemp);	
						}
					else
						bTemp=FALSE;

					BatchTable()->GetRow(j)->HasResults(bTemp == TRUE);
					if(BatchTable()->GetRow(j)->HasResults())
					{
						CByteArray bytearray;
						database.BatchTable().GetField(BATCHTABLE_DSO_FILE,bytearray);
						BatchTable()->GetRow(j)->SetByteArray(bytearray);
						
						std::vector<CString> vecContourFilenames;
						for(int iTable = 0 ; iTable < NR_OF_TABLES ; iTable++)
						{
							switch(iTable)
							{
								//get bytearrays from database
								case FORMATION_PLASTICITY_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_FOREQPLAST,	bytearray);
									break;
								case CASING_PLASTICITY_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_CASEQPLAST,	bytearray);
									break;
								case FORMATION_DEBONDING_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_FORDEB,		bytearray);
									break;
								case CASING_DEBONDING_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_CASDEB,		bytearray);
									break;
								case CEMENT_PLASTICITY_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_CEMEQPLAST,	bytearray);
									break;
								case CEMENT_RADIAL_CRACKING_TABLE:
									database.BatchTable().GetField(BATCHTABLE_PLOT_RADCRACK,	bytearray);
									break;
							}
							

							if(bytearray.GetSize())
							{
								CString sTmpFileName;
								CString sTblNr;
								sTblNr.Format("~CPF%d_",iTable);
								LPTSTR lptstrFN = sTmpFileName.GetBufferSetLength(1024);
								GetTempFileName(GetWorkingDir(),sTblNr,0,lptstrFN);
								ByteArrayToFile(lptstrFN,bytearray);
								vecContourFilenames.push_back(lptstrFN);
							}
							else
							{
								vecContourFilenames.push_back(CString());
							}

						}
						BatchTable()->GetRow(j)->SetContourFileNames(vecContourFilenames);
					}


					database.BatchTable().GetField(BATCHTABLE_DEFAULT_STEPPING_SCHEME,bTemp);
					BatchTable()->GetRow(j)->DefaultSteppingScheme(bTemp==TRUE);				
					database.BatchTable().GetField(BATCHTABLE_TIME_STEP_NUMBER,lngTemp);
					BatchTable()->GetRow(j)->TimeStepNumber(lngTemp);				
					database.BatchTable().GetField(BATCHTABLE_ITERATION_SCHEME,lngTemp);
					BatchTable()->GetRow(j)->IterationScheme(CBatchTableRow::eIterationScheme(lngTemp));				
					database.BatchTable().GetField(BATCHTABLE_MAX_NUMBER_OF_ITERATIONS,lngTemp);
					BatchTable()->GetRow(j)->MaxNumberOfIterations(lngTemp);				
					database.BatchTable().GetField(BATCHTABLE_CONVERGENCE_NORM,lngTemp);
					BatchTable()->GetRow(j)->ConvergenceNorm(CBatchTableRow::eConvergenceNorm(lngTemp));				
					database.BatchTable().GetField(BATCHTABLE_STOP_BATCH_RUN,bTemp);
					BatchTable()->GetRow(j)->StopBatchRun(bTemp==TRUE);				
					database.BatchTable().GetField(BATCHTABLE_FURTHER_POST_PROCESSING,bTemp);
					BatchTable()->GetRow(j)->FurtherPostProcessing(bTemp==TRUE);				
					//database.BatchTable().GetField(BATCHTABLE_NO_MONITORING,bTemp);
					//BatchTable()->GetRow(j)->NoMonitoring(bTemp==TRUE);				
					database.BatchTable().GetField(BATCHTABLE_SAVE_DIAGNOSTIC,bTemp);
					BatchTable()->GetRow(j)->SaveDiagnostic(bTemp==TRUE);
					if(database.FieldExistsInTable(BATCHTABLE_TABLE,BATCHTABLE_DIAGNOSTIC_FILE))
					{
						if(BatchTable()->GetRow(j)->IsCalculated() && BatchTable()->GetRow(j)->SaveDiagnostic())
						{
							CByteArray bytearray;
							database.BatchTable().GetField(BATCHTABLE_DIAGNOSTIC_FILE,	bytearray);

							if(bytearray.GetSize())
							{
								CString sTmpFileName;
								LPTSTR lptstrFN = sTmpFileName.GetBufferSetLength(1024);
								GetTempFileName(GetWorkingDir(),"DIAG",0,lptstrFN);
								ByteArrayToFile(lptstrFN,bytearray);
								BatchTable()->GetRow(j)->SetDiagnosticFileName(lptstrFN);
							}
							else
							{
								BatchTable()->GetRow(j)->SetDiagnosticFileName(CString());
							}
						}
					}
					else
					{
						BatchTable()->GetRow(j)->SetDiagnosticFileName(CString());
					}
					
					j++;
					database.BatchTable().RS().MoveNext();
				}
			}
		}
		database.BatchTable().Close();
	}

  PorePressureProfile().clear();
  if(database.TableExists(PPP_TABLE) && database.VersionInfo() >= CVersionInfo(1, 3))
  {
    database.PorePressureProfileTable().Open();
    if(!database.PorePressureProfileTable().RS().IsBOF())
    {
      database.PorePressureProfileTable().RS().MoveFirst();
      while(!database.PorePressureProfileTable().RS().IsEOF())
      {
        double dDepth;
        double dValue;
        database.PorePressureProfileTable().GetField(PPP_DEPTH, dDepth);
        database.PorePressureProfileTable().GetField(PPP_VALUE, dValue);
        PorePressureProfile().AddValues(dDepth, dValue);
        database.PorePressureProfileTable().RS().MoveNext();
      }
    }
    database.PorePressureProfileTable().Close();
  }

  DensityProfile().clear();
  if(database.TableExists(DENSITY_PROFILE_TABLE) && database.VersionInfo() >= CVersionInfo(1, 5))
  {
    database.DensityProfileTable().Open();
    if(!database.DensityProfileTable().RS().IsBOF())
    {
      database.DensityProfileTable().RS().MoveFirst();
      while(!database.DensityProfileTable().RS().IsEOF())
      {
        double dDepth;
        double dValue;
        database.DensityProfileTable().GetField(DENSITY_PROFILE_DEPTH, dDepth);
        database.DensityProfileTable().GetField(DENSITY_PROFILE_VALUE, dValue);
        DensityProfile().AddValues(dDepth, dValue);
        database.DensityProfileTable().RS().MoveNext();
      }
    }
    database.DensityProfileTable().Close();
  }

  TemperatureProfile().clear();
  if(database.TableExists(TEMPERATURE_PROFILE_TABLE) && database.VersionInfo() >= CVersionInfo(1, 5))
  {
    database.TemperatureProfileTable().Open();
    if(!database.TemperatureProfileTable().RS().IsBOF())
    {
      database.TemperatureProfileTable().RS().MoveFirst();
      while(!database.TemperatureProfileTable().RS().IsEOF())
      {
        double dDepth;
        double dValue;
        database.TemperatureProfileTable().GetField(TEMPERATURE_PROFILE_DEPTH, dDepth);
        database.TemperatureProfileTable().GetField(TEMPERATURE_PROFILE_VALUE, dValue);
        TemperatureProfile().AddValues(dDepth, dValue);
        database.TemperatureProfileTable().RS().MoveNext();
      }
    }
    database.TemperatureProfileTable().Close();
  }

  if(database.TableExists(MODEL_TABLE) && database.VersionInfo() >= CVersionInfo(1, 3))
  {
    database.ModelTable().Open();
    if(!database.ModelTable().RS().IsBOF())
    {
      database.ModelTable().RS().MoveFirst();
      database.ModelTable().GetField(MODEL_PPP_FILENAME, m_strPorePressureProfileFileName);

      if(database.VersionInfo() >= CVersionInfo(1, 5))
      {
        database.ModelTable().GetField(MODEL_DENSITY_PROFILE_FILENAME, m_strDensityProfileFileName);
        database.ModelTable().GetField(MODEL_TEMPERATURE_PROFILE_FILENAME, m_strTemperatureProfileFileName);
      }

      CString strSurveyFileName;
      database.ModelTable().GetField(MODEL_SURVEY_FILENAME, strSurveyFileName);
      SurveyFileName(strSurveyFileName);
    }
    database.ModelTable().Close();
  }
	
	database.Close();

	if(bPorousIsChanged)
		SaveToDatabase(strDatabaseName);

  if(database.VersionInfo() < CVersionInfo(1, 3))
    InvalidateResults();

  return TRUE;
}

//##ModelId=3DDA0C4802F1
BOOL CDataModel::SaveToDatabase(CString strDatabaseName)
{
	m_sDatabaseName = strDatabaseName;

	CDSModelDatabase database;
	if(FileExists(strDatabaseName))
	{
		DeleteFile(strDatabaseName);
	}
	database.Create(strDatabaseName, VersionInfo());

	//Store formations in database
	database.Formation().Open();
	for(int i=0 ; i< FormationTopography()->NrOfFormations();i++)
	{
		database.Formation().RS().AddNew();
		database.Formation().SetField(FORMATION_NAME,					  FormationTopography()->GetFormationAt(i)->FormationName());
		database.Formation().SetField(FORMATION_MATERIAL_NAME,			  FormationTopography()->GetFormationAt(i)->MaterialName());
		database.Formation().SetField(FORMATION_BOTTOM_TVD,				*(FormationTopography()->GetFormationAt(i)->BottomTVD()));
		database.Formation().SetField(FORMATION_GEOTHERMAL_GRAD,		*(FormationTopography()->GetFormationAt(i)->GeothermalGradient()));
		database.Formation().SetField(FORMATION_MAX_HOR_STRESS_RATIO,	*(FormationTopography()->GetFormationAt(i)->MaxHorizontalStressRatio()));
		database.Formation().SetField(FORMATION_MIN_HOR_STRESS_RATIO,	*(FormationTopography()->GetFormationAt(i)->MinHorizontalStressRatio()));
		database.Formation().SetField(FORMATION_AZIMUTH_SH_WITH_NORTH,	*(FormationTopography()->GetFormationAt(i)->AzimuthOfSHWithNorth()));
		database.Formation().SetField(FORMATION_POROUS,					  FormationTopography()->GetFormationAt(i)->Porous());
		database.Formation().SetField(FORMATION_TOP_PORE_PRESSURE,		*(FormationTopography()->GetFormationAt(i)->TopPorePressure()));
		database.Formation().SetField(FORMATION_BOTTOM_PORE_PRESSURE,	*(FormationTopography()->GetFormationAt(i)->BottomPorePressure()));

		database.Formation().RS().Update();
	}
	database.Formation().Close();

	//Store ModelOverburden in database		
	database.ModelOverburden().Open();
	database.ModelOverburden().RS().AddNew();

	database.ModelOverburden().SetField(MODEL_OVERBURDEN_OFFSHORE_DEFINITION,		  FormationTopography()->Offshore());
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_ELEVATION,					*(FormationTopography()->ElevationTVD()));
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_OVERBURDEN_BOTTOM_TVD,		*(FormationTopography()->Overburden()->OverburdenBottomTVD()));
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_SURFACE_UNDIST_TEMP,		*(FormationTopography()->Overburden()->SurfaceTemperature()));
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_OB_GEOTHERMAL_GRAD,		*(FormationTopography()->Overburden()->OBGeothermalGradient()));
//	database.ModelOverburden().SetField(MODEL_OVERBURDEN_OB_AVER_VERT_STRESS_GRAD,	*(FormationTopography()->Overburden()->OBAverageStressGradient()));
  database.ModelOverburden().SetField(MODEL_OVERBURDEN_AVERAGE_DENSITY, *FormationTopography()->Overburden()->OBAverageDensity());
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_WATERDEPTH,				*(FormationTopography()->Seawater()->WaterDepth()));
	database.ModelOverburden().SetField(MODEL_OVERBURDEN_WATER_DENSITY,				*(FormationTopography()->Seawater()->Waterdensity()));
	
	database.ModelOverburden().RS().Update();
	database.ModelOverburden().Close();

	//Store DrillingDirection in database		
	database.DrillingDirection().Open();

  for(int i = 0; i < DrillingScheme()->NrOfDrillingDirections(); ++i)
  {
	  database.DrillingDirection().RS().AddNew();
  		
	  database.DrillingDirection().SetField(DRILLING_DIR_BOTTOM_TMD,			*(DrillingScheme()->GetDrillingDirectionSection(i)->DrillingSectionBottomTMD()));			
	  database.DrillingDirection().SetField(DRILLING_DIR_WELL_INCLINATION	,	*(DrillingScheme()->GetDrillingDirectionSection(i)->WellInclination()));
	  database.DrillingDirection().SetField(DRILLING_DIR_AZIMUTH_WITH_NORTH,	*(DrillingScheme()->GetDrillingDirectionSection(i)->AzimuthWithNorth()));

	  database.DrillingDirection().RS().Update();
  }
	database.DrillingDirection().Close();

	//Store DrillingOperation in database		
	database.DrillingOperation().Open();
	database.DrillingOperation().RS().AddNew();

	CDrillingOperation  *pDO   = DrillingScheme()->GetDrillingOperation(0);
	CCasingOperation    *pCaO  = pDO->CasingOperation();
	CCementingOperation *pCeO  = pDO->CementingOperation();


	database.DrillingOperation().SetField(DRILLING_OPERATION_BOTTOM_TMD,					*(pDO->OperationBottomTMD()));					
	database.DrillingOperation().SetField(DRILLING_OPERATION_BOREHOLE_DIAMETER,				*(pDO->BoreholeDiameter()));			
	database.DrillingOperation().SetField(DRILLING_OPERATION_MUD_DENSITY_AT_SURFACE,		*(pDO->MudDensityAtSurface()));					
	database.DrillingOperation().SetField(DRILLING_OPERATION_MUD_COMPRESSIBILITY,			*(pDO->MudCompressibility()));						
	database.DrillingOperation().SetField(DRILLING_OPERATION_CASING_AXIAL_STRESSES,			  pCaO->AdvancedAxialCasingStresses());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_HOOK_LOAD,						*pCaO->HookLoad());					
	database.DrillingOperation().SetField(DRILLING_OPERATION_WORK_STRING_USED,				 pCaO->WorkStringUsed());				
	database.DrillingOperation().SetField(DRILLING_OPERATION_DISP_FLUID_STRESS_GRAD,		*pCaO->DisplacementFluidStressGradient());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_DISP_FLUID_COMPRESSIBILITY,	*pCaO->DispFluidCompressibility());	
	database.DrillingOperation().SetField(DRILLING_OPERATION_WORK_STRING_PACKERS_TMD,		*pCaO->WorkStringPackersTMD());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_NEW_FLUID_IN_ANNULUS,			 pCaO->NewFluidInCasWSAnnulus());			
	database.DrillingOperation().SetField(DRILLING_OPERATION_NEW_FLUID_STRESS_GRAD,			*pCaO->NewFluidStressGradient());				
	database.DrillingOperation().SetField(DRILLING_OPERATION_NEW_FLUID_OVERPRESSURE,		*pCaO->NewFluidOverpressure());						
	database.DrillingOperation().SetField(DRILLING_OPERATION_NEW_FLUID_COMPRESSIBILITY,		*pCaO->NewFluidCompressibility());					
	database.DrillingOperation().SetField(DRILLING_OPERATION_PERCENTAGE_ECCENTRICITY,		*pCaO->PercentageEccentricity());							
	database.DrillingOperation().SetField(DRILLING_OPERATION_GUIDE_SHOE_DEPTH_ALONG_HOLE,	*pCaO->GuideShoeTMD());						
	database.DrillingOperation().SetField(DRILLING_OPERATION_CASING_MATERIAL_NAME,		    pCaO->CasingMaterialName());


	database.DrillingOperation().SetField(DRILLING_OPERATION_TOP_OF_CEMENT_TMD,				*pCeO->TopOfCementTMD());			
	database.DrillingOperation().SetField(DRILLING_OPERATION_PRESSURE_FROM_FLUIDS_ABOVE,	*pCeO->PressureFromFluidsAbove());	
	database.DrillingOperation().SetField(DRILLING_OPERATION_LEAD_AND_TAIL_CEMENTS,			 pCeO->LeadTailCements());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_TOP_OF_TAIL_CEMENT_TMD,		*pCeO->TopOfTailCement());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_LEAD_CEMENT_STRESS_GRAD,		*pCeO->LeadCementStressGradient());		
	database.DrillingOperation().SetField(DRILLING_OPERATION_TAIL_CEMENT_STRESS_GRAD,		*pCeO->TailCementStressGradient());		
	
	
	
	database.DrillingOperation().RS().Update();
	database.DrillingOperation().Close();


	//store cement scenario material names
	database.Cement().Open();
	
	for(int j=0;j<pDO->CementScenariosTable()->NumberOfCementScenarios();j++)
	{
		database.Cement().RS().AddNew();
		CCementScenario *pCS=pDO->CementScenariosTable()->CementScenarioAt(j);
		database.Cement().SetField(CEMENT_HEAD_MATERIAL_NAME,pCS->HeadMaterialName());
		database.Cement().SetField(CEMENT_TAIL_MATERIAL_NAME,pCS->TailMaterialName());
		database.Cement().RS().Update();	
	}
	

	//Store AnalysisPoint in database		
	database.AnalysisPoint().Open();
	for(int j=0 ; j< Analysis()->NrOfAnalysisPoints();j++)
	{
		CAnalysisPoint *pPoint;
		pPoint=Analysis()->GetAnalysisPointAt(j);
		database.AnalysisPoint().RS().AddNew();
	
		database.AnalysisPoint().SetField(ANALYSIS_POINT_TMD,									*(Analysis()->GetAnalysisPointAt(j)->TMD()));									
		database.AnalysisPoint().SetField(ANALYSIS_POINT_DRILLING_FLUID_TEMP,					*(Analysis()->GetAnalysisPointAt(j)->PriorToCompletionLoads()->DrillingTemperature()));					
		database.AnalysisPoint().SetField(ANALYSIS_POINT_TIME_OF_EXPOS_TO_DRIL_FLUID,			*(Analysis()->GetAnalysisPointAt(j)->PriorToCompletionLoads()->TimeOfExposureToDrillingFluid()));								
//		database.AnalysisPoint().SetField(ANALYSIS_POINT_TEST_PRESSURE,							*(Analysis()->GetAnalysisPointAt(j)->TestLoads()->TestPressure()));						
		database.AnalysisPoint().SetField(ANALYSIS_POINT_COMPLETION_PRESSURE_DIFF,				*(Analysis()->GetAnalysisPointAt(j)->PerforationLoads()->CompletionPressureDiff()));				
//		database.AnalysisPoint().SetField(ANALYSIS_POINT_PRODUCTION_PORE_PRESSURE,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->ProductionPorePressure()));				
		database.AnalysisPoint().SetField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->BottomHoleFlowingPressure()));			
		database.AnalysisPoint().SetField(ANALYSIS_POINT_BOTTOM_HOLE_FLOWING_TEMP,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->BottomHoleFlowingTemp()));				
		database.AnalysisPoint().SetField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_PROD,				*(Analysis()->GetAnalysisPointAt(j)->ProductionLoads()->TimeOfContinuousProduction()));				
		database.AnalysisPoint().SetField(ANALYSIS_POINT_MAXIMUM_INJECTION_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->MaximumInjectionPressure()));							
		database.AnalysisPoint().SetField(ANALYSIS_POINT_INJECTION_TEMPERATURE,					*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->InjectionTemperature()));											
		database.AnalysisPoint().SetField(ANALYSIS_POINT_TIME_OF_CONTINUOUS_INJECTION,			*(Analysis()->GetAnalysisPointAt(j)->InjectionLoads()->TimeOfContinuousInjection()));										
		database.AnalysisPoint().SetField(ANALYSIS_POINT_MAX_INJECTING_FRAC_PRESSURE,			*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->MaxInjectingFracPressure()));
		database.AnalysisPoint().SetField(ANALYSIS_POINT_INJECTION_FRAC_TREATING_FLUID_TEMP,	*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->TreatingFluidTemp()));	
		database.AnalysisPoint().SetField(ANALYSIS_POINT_INJECTION_FRAC_TIME_OF_EXPOSURE,		*(Analysis()->GetAnalysisPointAt(j)->FracturingLoads()->TimeOfExposureToTreatingFluid()));		
		database.AnalysisPoint().SetField(ANALYSIS_POINT_BOTTOM_HOLE_EVAC_TREATING_PRESSURE,	*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->BottomHoleTreatingPressure()));			
		database.AnalysisPoint().SetField(ANALYSIS_POINT_EVAC_TREATING_FLUID_TEMP,				*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->TreatingFluidTempEvac()));						
		database.AnalysisPoint().SetField(ANALYSIS_POINT_EVAC_TIME_OF_EXPOSURE,					*(Analysis()->GetAnalysisPointAt(j)->EvacuationLoads()->TimeOfExposureToTreatingFluidEvac()));						
		
		database.AnalysisPoint().RS().Update();
	}
	database.AnalysisPoint().Close();

	//Store Control in database		
	database.Control().Open();
	database.Control().RS().AddNew();

	database.Control().SetField(CONTROL_WELL_TYPE,					(long) Control()->WellType());						
	database.Control().SetField(CONTROL_BALANCED_COMPLETION,		Control()->BalancedCompletion());				
	database.Control().SetField(CONTROL_UNDERBALANCED_COMPLETION,	Control()->UnderBalancedCompletion());						
	database.Control().SetField(CONTROL_OVERBALANCED_COMPLETION,	Control()->OverBalancedCompletion());							
	database.Control().SetField(CONTROL_CONVERSION,					Control()->Conversion());										
	database.Control().SetField(CONTROL_FRACTURING,					Control()->Fracturing());				
	database.Control().SetField(CONTROL_EVACUATION,					Control()->Evacuation());				
	database.Control().SetField(CONTROL_DEFAULT_MESH_REFINEMENT,	Control()->DefaultMeshRefinement());				
	database.Control().SetField(CONTROL_ROCK_REFINEMENT	,			Control()->RockRefinement());				
	database.Control().SetField(CONTROL_CASING_REFINEMENT,			Control()->CasingRefinement());				
	database.Control().SetField(CONTROL_CEMENT_REFINEMENT,			Control()->CementRefinement());				
	database.Control().SetField(CONTROL_OUTPUT_LEVEL,				Control()->OutputLevel());
  database.Control().SetField(CONTROL_TEST_PRESSURE, *m_TestLoads.TestPressure());
  database.Control().SetField(CONTROL_FLUID_PRESSURE_GRADIENT, *m_TestLoads.FluidPressureGradient());
	database.Control().SetField(CONTROL_SHUT_IN,					Control()->ShutIn());				

	database.Control().RS().Update();
	database.Control().Close();

	//Store BatchTable in database		
	database.BatchTable().Open();
	for(int k = 0 ; k < BatchTable()->RowCount() ; k++)
	{
		CBatchTableRow* pBatchRow = BatchTable()->GetRow(k);
		database.BatchTable().RS().AddNew();

		database.BatchTable().SetField(BATCHTABLE_USE_FOR_CALCULATION,		pBatchRow->UseForCalculation());						
		database.BatchTable().SetField(BATCHTABLE_IS_CALCULATED,			pBatchRow->IsCalculated());				
		database.BatchTable().SetField(BATCHTABLE_CALCULATION_SUCCEEDED,	pBatchRow->CalculationSucceeded());						
		database.BatchTable().SetField(BATCHTABLE_HASRESULTS,				pBatchRow->HasResults());						

		if(pBatchRow->HasResults())
		{
			database.BatchTable().SetField(BATCHTABLE_DSO_FILE,				pBatchRow->GetByteArray());	

			for(int iTableNr = 0 ; iTableNr < NR_OF_TABLES ; iTableNr++) // remove 
			{
				CString sContourFileName = pBatchRow->GetContourFileName(iTableNr);
				if(!sContourFileName.IsEmpty())
				{
					CByteArray arr;
					ByteArrayFromFile((LPCSTR)sContourFileName,arr);
			
					if(iTableNr == FORMATION_PLASTICITY_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_FOREQPLAST, 	arr);
				
					else if(iTableNr == CASING_PLASTICITY_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_CASEQPLAST, 	arr);

					else if(iTableNr == FORMATION_DEBONDING_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_FORDEB, 	arr);

					else if(iTableNr == CASING_DEBONDING_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_CASDEB, 	arr);

					else if(iTableNr == CEMENT_PLASTICITY_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_CEMEQPLAST, 	arr);

					else if(iTableNr == CEMENT_RADIAL_CRACKING_TABLE)
						database.BatchTable().SetField(BATCHTABLE_PLOT_RADCRACK, 	arr);

				}
			}
		}

		if(pBatchRow->SaveDiagnostic())
		{
			if(!pBatchRow->GetDiagnosticFileName().IsEmpty())
			{
				CByteArray arr;
				ByteArrayFromFile((LPCSTR)pBatchRow->GetDiagnosticFileName(),arr);
				database.BatchTable().SetField(BATCHTABLE_DIAGNOSTIC_FILE, 	arr);
			}
		}
		

		database.BatchTable().SetField(BATCHTABLE_DEFAULT_STEPPING_SCHEME,	pBatchRow->DefaultSteppingScheme());				
		database.BatchTable().SetField(BATCHTABLE_TIME_STEP_NUMBER,			pBatchRow->TimeStepNumber());				
		database.BatchTable().SetField(BATCHTABLE_ITERATION_SCHEME,			(long) pBatchRow->IterationScheme());				
		database.BatchTable().SetField(BATCHTABLE_MAX_NUMBER_OF_ITERATIONS,	pBatchRow->MaxNumberOfIterations());				
		database.BatchTable().SetField(BATCHTABLE_CONVERGENCE_NORM,			(long) pBatchRow->ConvergenceNorm());				
						
		database.BatchTable().SetField(BATCHTABLE_STOP_BATCH_RUN,			pBatchRow->StopBatchRun());				
		database.BatchTable().SetField(BATCHTABLE_FURTHER_POST_PROCESSING,	pBatchRow->FurtherPostProcessing());				
		//database.BatchTable().SetField(BATCHTABLE_NO_MONITORING,			pBatchRow->NoMonitoring());				
		database.BatchTable().SetField(BATCHTABLE_SAVE_DIAGNOSTIC,			pBatchRow->SaveDiagnostic());
	
		database.BatchTable().RS().Update();
	}
	database.BatchTable().Close();

	database.PorePressureProfileTable().Open();
  for(CValueProfile::const_iterator it = PorePressureProfile().begin(); it != PorePressureProfile().end(); ++it)
  {
		database.PorePressureProfileTable().RS().AddNew();
		database.PorePressureProfileTable().SetField(PPP_DEPTH,	it->first);
    database.PorePressureProfileTable().SetField(PPP_VALUE, it->second);
    database.PorePressureProfileTable().RS().Update();
  }

  database.PorePressureProfileTable().Close();

  database.DensityProfileTable().Open();
  for(CValueProfile::const_iterator it = DensityProfile().begin(); it != DensityProfile().end(); ++it)
  {
    database.DensityProfileTable().RS().AddNew();
    database.DensityProfileTable().SetField(DENSITY_PROFILE_DEPTH, it->first);
    database.DensityProfileTable().SetField(DENSITY_PROFILE_VALUE, it->second);
    database.DensityProfileTable().RS().Update();
  }

  database.DensityProfileTable().Close();

  database.TemperatureProfileTable().Open();
  for(CValueProfile::const_iterator it = TemperatureProfile().begin(); it != TemperatureProfile().end(); ++it)
  {
    database.TemperatureProfileTable().RS().AddNew();
    database.TemperatureProfileTable().SetField(TEMPERATURE_PROFILE_DEPTH, it->first);
    database.TemperatureProfileTable().SetField(TEMPERATURE_PROFILE_VALUE, it->second);
    database.TemperatureProfileTable().RS().Update();
  }

  database.TemperatureProfileTable().Close();

  database.ModelTable().Open();
  database.ModelTable().RS().AddNew();
  database.ModelTable().SetField(MODEL_PPP_FILENAME, m_strPorePressureProfileFileName);
  database.ModelTable().SetField(MODEL_DENSITY_PROFILE_FILENAME, m_strDensityProfileFileName);
  database.ModelTable().SetField(MODEL_TEMPERATURE_PROFILE_FILENAME, m_strTemperatureProfileFileName);
  database.ModelTable().SetField(MODEL_SURVEY_FILENAME, SurveyFileName());
  database.ModelTable().RS().Update();
  database.ModelTable().Close();

	database.Close();

	CString MatName;
	
	//Store formation materials in datbase
	CFormationMaterialDatabase fmd;
	fmd.Database().Open(strDatabaseName,TRUE,FALSE,NULL);
	for(int i=0;i<FormationMatList()->NrOfMaterials();i++)
	{
		fmd.AddMaterial(*FormationMatList()->GetMaterialAt(i));
	}
	fmd.Database().Close();

	//Store the casing material in datbase
	CCasingMaterialDatabase cmd;
	cmd.Database().Open(strDatabaseName,TRUE,FALSE,NULL);
	MatName=DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	CCasingMaterial *pCasingMaterial;
	pCasingMaterial=CasingMatList()->GetMaterial(MatName);
	if(pCasingMaterial)
		cmd.AddMaterial(*pCasingMaterial);
	cmd.Database().Close();
	

	//Store cement materials in database
	CCementMaterialDatabase cemd;
	cemd.Database().Open(strDatabaseName,TRUE,FALSE,NULL);
	for(int i=0;i<CementMatList()->NrOfMaterials();i++)
	{
		cemd.AddMaterial(*CementMatList()->GetMaterialAt(i));
	}
	cemd.Database().Close();

	return true;
}

//##ModelId=3DDA0C48014B
double CDataModel::GetCohesionAtDepth(double dDepth)
{
	CFormation* pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
	CString name = pFormation->MaterialName();
		
	CFormationMaterial * pFM = FormationMatList()->GetMaterial(name);
	
	double cohesion          = pFM->Cohesion()->Value();
	double frictionangle     = pFM->FrictionAngle()->Value();
	
	if(pFormation->Porous())
	{
		return cohesion;
	}
	else
	{
		CTensor EffStress = GetLocalEffectiveStressesAtDepth(dDepth);
		double dStressXX = EffStress.XX();
		double dStressYY = EffStress.YY();

		double dEffStressXX = dStressXX - GetUndrainedPorepressureAtDepth(dDepth); 
		double dEffStressYY = dStressYY - GetUndrainedPorepressureAtDepth(dDepth);
					
		return cohesion * cos(frictionangle*PI/180.0) + sin(frictionangle*PI/180.0) / 2 * (dEffStressXX + dEffStressYY) ;
		
	}
}

//##ModelId=3DDA0C48017A
double CDataModel::GetFrictionAtDepth(double dDepth)
{	
	CFormation* pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
	CString name = pFormation->MaterialName();
		
	CFormationMaterial * pFM = FormationMatList()->GetMaterial(name);
	
	double frictionangle     = pFM->FrictionAngle()->Value();
	
	if(pFormation->Porous())
	{
		return frictionangle;
	}
	else
	{
		return 0.0;
	}
}

//##ModelId=3DDA0C48019A
double CDataModel::GetDilatationAtDepth(double dDepth)
{
	double dilatationangle; 
	
	CFormation* pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
	CString name = pFormation->MaterialName();
		
	CFormationMaterial * pFM = FormationMatList()->GetMaterial(name);
	double frictionangle     = pFM->FrictionAngle()->Value();

	if(pFormation->Porous())
	{
		dilatationangle = frictionangle - 30.0;
		if(dilatationangle < 0)
			dilatationangle = 0.0;

		return dilatationangle;
	}
	else
	{
		return 0.0;
	}
}
//##ModelId=3C8D9E0300CC
double CDataModel::GetGeoThermalTemperatureAtDepth(double dDepth)
{
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
  return pFormation->TemperatureAtDepth(dDepth);
}

//##ModelId=3C8D9E03031E
double CDataModel::GetVertStressAtDepth(double dDepth)
{	
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
  assert(pFormation);
  return pFormation->VerticalStressAtDepth(dDepth);
}

//##ModelId=3C8D9E040188
double CDataModel::GetMaxHorizontalStressAtDepth(double dDepth)
{	
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
	return pFormation->MaxHorizontalStressAtDepth(dDepth);
}

//##ModelId=3C8D9E050030
double CDataModel::GetMinHorizontalStressAtDepth(double dDepth)
{	
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
	return pFormation->MinHorizontalStressAtDepth(dDepth);
}

//##ModelId=3C8D9E050272
double CDataModel::GetPorepressureAtDepth(double dDepth)
{	
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);

	if(pFormation->Porous())
	{
		return GetUndrainedPorepressureAtDepth(dDepth);
	}
	else
	{
		return 0.0;
	}
}


//##ModelId=3DDA0C480217
double CDataModel::GetUndrainedPorepressureAtDepth(double dDepth)
{	
	CFormation *pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
  return pFormation->GetPorePressureAt(dDepth);
	
}
//##ModelId=3C8D9E0600FB
double CDataModel::GetEffectiveVertStressAtDepth(double dDepth)
{	
	return GetVertStressAtDepth(dDepth)-GetPorepressureAtDepth(dDepth);
}

//##ModelId=3C8D9E06035D
double CDataModel::GetEffectiveMaxHorizontalStressAtDepth(double dDepth)
{	
	return GetMaxHorizontalStressAtDepth(dDepth)-GetPorepressureAtDepth(dDepth);
}

//##ModelId=3C8D9E0701E6
double CDataModel::GetEffectiveMinHorizontalStressAtDepth(double dDepth)
{	
	return GetMinHorizontalStressAtDepth(dDepth)-GetPorepressureAtDepth(dDepth);
}

//##ModelId=3CA011900167
double CDataModel::GetTauRatioAtDepth(double dDepth)
{
	CTensor Strss = GetLocalEffectiveStressesAtDepth(dDepth);
	double SigXX = Strss.XX();
	double SigYY = Strss.YY();
	double SigZZ = Strss.ZZ();
	double TauXY = Strss.XY();
	double TauZX = Strss.XZ();
	double TauYZ = Strss.YZ();
	int NrOfSteps = 360;
	double Delta=(2 * PI / NrOfSteps);
	double teta=0.0;
	CString name = FormationTopography()->GetFormationAtDepth(dDepth)->MaterialName();
	CFormationMaterial * pFM = FormationMatList()->GetMaterial(name);
	if(!pFM->Plasticity())
		return 0.5; // always elastic

	double dCoh = GetCohesionAtDepth(dDepth);//	 pFM->Cohesion()->Value();
	double dFi = GetFrictionAtDepth(dDepth) * PI / 180.0;//pFM->FrictionAngle()->Value() * PI / 180.0;
	double nu = pFM->PoissonRatio()->Value();
	double dTauRatioMax;
	
	for(int i = 0; i < NrOfSteps ; i++)
	{
		double dSigRoRo = DrillingMudPressureAtDepth(dDepth) - GetPorepressureAtDepth(dDepth);
		double dSigtt = SigXX + SigYY - dSigRoRo - 2.0 * (SigXX - SigYY) * cos(2.0 * teta) - 4.0 * TauXY * sin(2.0 * teta);
		double dSigZZ = SigZZ - 2.0 * nu * ( SigXX - SigYY) * cos(2.0 * teta) - 4.0 * nu * TauXY * sin(2.0 * teta);
		double dSigTZ = 2.0 * (TauYZ * cos(teta) - TauZX * sin (teta));
		double dSigZR = 0.0;
		double dSigRT = 0.0;

		double d_Sig = (dSigRoRo + dSigtt + SigZZ)/3.0 ;
		double dJ2	= -(1.0 / 6.0) * pow((dSigRoRo - dSigtt),2.0) - (1.0/6.0) * pow((dSigtt - SigZZ),2.0) - (1.0 / 6.0) * pow((SigZZ - dSigRoRo),2.0) - pow(dSigTZ,2.0);
		double dJ3 = (dSigRoRo - d_Sig) * (dSigtt - d_Sig) * ( SigZZ - d_Sig) - (dSigRoRo - d_Sig) * pow(dSigTZ, 2.0);
		double dPsi = (1.0 / 3.0) * asin( (-dJ3 / 2.0) * pow((-dJ2 / 3.0),(-3.0 / 2.0)) );

		double dTau = pow( (-2.0 * dJ2), 0.5);
		double dShearStrength = pow(6.0, 0.5) * (dCoh * cos(dFi) - d_Sig * sin(dFi)) / (pow(3.0, 0.5) * cos(dPsi) - sin(dPsi) * sin(dFi));
		
		double dTauRatio = fabs(dTau / dShearStrength);
		if(i == 0)
		{
			dTauRatioMax = dTauRatio;
			
		}
		else
		{
			if(dTauRatioMax < dTauRatio)
			{
				dTauRatioMax = dTauRatio;
			}
		}

		teta=teta+Delta;
	}
	return dTauRatioMax;

}

//##ModelId=3C8D9E08004F
CTensor CDataModel::GetLocalEffectiveStressesAtDepth(double dDepth)
{
	CTensor PrincipalEffectiveStresses(0);
	CTensor RotatedInNEg(0);
	CTensor RotatedInWellAzimuth(0);
	
	PrincipalEffectiveStresses.XX(GetEffectiveMaxHorizontalStressAtDepth(dDepth));
	PrincipalEffectiveStresses.YY(GetEffectiveMinHorizontalStressAtDepth(dDepth));
	PrincipalEffectiveStresses.ZZ(GetEffectiveVertStressAtDepth(dDepth));

	CFormation* pFormation = FormationTopography()->GetFormationAtDepth(dDepth);
  CDrillingDirectionSection* pDDS = DrillingScheme()->GetDrillingDirectionSectionAt(dDepth);
  assert(pDDS);
	double azimuth_well   = pDDS->AzimuthWithNorth()->Value();
	double inclination    = pDDS->WellInclination()->Value();
	double azimuth_stress = pFormation->AzimuthOfSHWithNorth()->Value();

	RotatedInNEg = PrincipalEffectiveStresses.RotateTensorDeg(geo::CVector::Zaxis,-azimuth_stress);

	RotatedInWellAzimuth= RotatedInNEg.RotateTensorDeg(geo::CVector::Zaxis,azimuth_well);

	return RotatedInWellAzimuth.RotateTensorDeg(geo::CVector::Yaxis,inclination);


}

//##ModelId=3DDA0C4801B9
#if USE_SE40
CFgvRunCtr&
#else
CFgvRunner& 
#endif
            CDataModel::GetFgvRunControl()
{
	return *m_pFgvRunCtr;
}

//##ModelId=3CA0119101C5
double CDataModel::DrillingMudPressureAtDepth(double dDepth)
{
	//double Depth = m_pBatchTableRow->GetDepth();
	double FluidStressGrad = DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface()->Value()*ACCELERATION_OF_GRAVITY;
	double FluidCompressibility = DrillingScheme()->GetDrillingOperation(0)->MudCompressibility()->Value();
	double FluidPressure = 0.0;

	return MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);
}

//##ModelId=3CA011900399
double CDataModel::CementSlurryPressureAtTMD(double dTMD) 
{
	CCementingOperation *pCO= DrillingScheme()->GetDrillingOperation(0)->CementingOperation();

	double RelativeDepth;
	double TopTailTVD;
  assert(TMDtoTVD(pCO->TopOfCementTMD()->Value()).second);
	double TopLeadTVD= TMDtoTVD(pCO->TopOfCementTMD()->Value()).first;
	double FluidStressGrad; 
	double FluidCompressibility;
	double FluidPressure;
	double BottomLeadPressure;

  assert(TMDtoTVD(dTMD).second);
	double dDepth = TMDtoTVD(dTMD).first;
	
	if(IsInTail(dTMD))
	{

		TopTailTVD = pCO->TopOfTailCement()->Value();
    assert(TMDtoTVD(TopTailTVD).second);
		TopTailTVD = TMDtoTVD(TopTailTVD).first;
		
		RelativeDepth = TopTailTVD-TopLeadTVD;
		FluidStressGrad = pCO->LeadCementStressGradient()->Value();
		FluidCompressibility = 0.0;
		FluidPressure = pCO->PressureFromFluidsAbove()->Value();	
		
		BottomLeadPressure = MF::CalcCompFluidPressure(RelativeDepth, FluidStressGrad,FluidCompressibility,FluidPressure);

		RelativeDepth = dDepth - TopTailTVD;
		FluidStressGrad = pCO->TailCementStressGradient()->Value();
		FluidCompressibility = 0.0;
		FluidPressure = BottomLeadPressure;

		return MF::CalcCompFluidPressure(RelativeDepth, FluidStressGrad,FluidCompressibility,FluidPressure);

	}
	else
	{
		RelativeDepth = dDepth - TopLeadTVD;
		FluidStressGrad = pCO->LeadCementStressGradient()->Value();
		FluidCompressibility =0.0;
		FluidPressure = pCO->PressureFromFluidsAbove()->Value();	
		
		return MF::CalcCompFluidPressure(RelativeDepth, FluidStressGrad,FluidCompressibility,FluidPressure);
	}

}

//##ModelId=3DDA0C48012C
void CDataModel::CalculateNEgMatrix(geo::CMatrix &NEgRot, double dTMD)
{
  CDrillingDirectionSection* pDDS = DrillingScheme()->GetDrillingDirectionSectionAt(dTMD);
  assert(pDDS);
	double dAzimuthWN = pDDS->AzimuthWithNorth()->Value();
	double dInclination = pDDS->WellInclination()->Value();
	
	geo::CMatrix AziRot;
	AziRot.CreateRotationMatrixDeg(geo::CVector::Zaxis,dAzimuthWN);
	
	geo::CMatrix IncliRot;
	IncliRot.CreateRotationMatrixDeg(geo::CVector::Yaxis,dInclination);
	
	geo::CMatrix NEgRot1;
	NEgRot1 = AziRot * IncliRot;
	NEgRot = NEgRot1.GetTransposed();

}

//##ModelId=3DDA0C4801C8
CString CDataModel::GetWorkingDir()
{
	return m_sWorkingDir;
}

//##ModelId=3C60D56A03AA
void CDataModel::RemoveUnusedMaterial()
{

	BOOL bUsed=FALSE;
	std::vector<CString> IndexToRemove;

	for(int i =0 ;i<m_FormationMatList.NrOfMaterials();i++)
	{
		bUsed=FALSE;
		for(int j=0; j< FormationTopography()->NrOfFormations();j++)
		{
			if(FormationTopography()->GetFormationAt(j)->MaterialName()==m_FormationMatList.GetMaterialAt(i)->Name())
			{
				bUsed=TRUE;
				break;
			}
		}
		if(!bUsed)
		{
			IndexToRemove.push_back(m_FormationMatList.GetMaterialAt(i)->Name());
		}
	}

	for(int i=0;i<IndexToRemove.size();i++)
	{
		m_FormationMatList.Remove(IndexToRemove[i]);
	}
//******
	IndexToRemove.clear();
	for(int i =0 ;i<m_CasingMatList.NrOfMaterials();i++)
	{
		bUsed=FALSE;
		if(DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName()==m_CasingMatList.GetMaterialAt(i)->Name())
		{
			bUsed=TRUE;
			break;
		}
		
		if(!bUsed)
		{
			IndexToRemove.push_back(m_CasingMatList.GetMaterialAt(i)->Name());
		}
	}

	for(int i=0;i<IndexToRemove.size();i++)
	{
		m_CasingMatList.Remove(IndexToRemove[i]);
	}

//*********

	IndexToRemove.clear();
	for(int i =0 ;i<m_CementMatList.NrOfMaterials();i++)
	{
		CString MatName = m_CementMatList.GetMaterialAt(i)->Name();
		bUsed=FALSE;
		for(int j =0 ;j<DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();j++)
		{
			CCementScenario *pCS = DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->CementScenarioAt(j);
			if(pCS->HeadMaterialName() == MatName || pCS->TailMaterialName() == MatName)
			{
				bUsed=TRUE;
				break;
			}
		}
		if(!bUsed)
		{
			IndexToRemove.push_back(MatName);
		}
	}

	for(int i=0;i<IndexToRemove.size();i++)
	{
		m_CementMatList.Remove(IndexToRemove[i]);
	}
}


///////////DEPENDENCIES////////////

//##ModelId=3C60D56B009D
CDataModel::CDep1_1::CDep1_1(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->OverburdenBottomTMD());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->WorkStringPackersTMD());
	
}

//##ModelId=3C60D56B009F
bool CDataModel::CDep1_1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	if(m_pDM->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->WorkStringUsed())
	{
		return m_qVec[0]->Value()<m_qVec[1]->Value();
	}
	else
	{
		return TRUE;
	}

}

//##ModelId=3C60D56B00AD
std::string CDataModel::CDep1_1::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nOverburden Bottom TMD < WorkStringPackersTMD"));
}

//##ModelId=3C60D56B0158
CDataModel::CDep1_2::CDep1_2(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->WorkStringPackersTMD());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
}

//##ModelId=3C60D56B015A
bool CDataModel::CDep1_2::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	if(m_pDM->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->WorkStringUsed())
	{
		return m_qVec[0]->Value()<m_qVec[1]->Value();
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D56B0168
std::string CDataModel::CDep1_2::GetDependencyString() const
{
	
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nWorkStringPackersTMD < Guide Shoe depth"));
}



//##ModelId=3C60D56B0204
CDataModel::CDep1_3::CDep1_3(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->OverburdenBottomTMD());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
	
}

//##ModelId=3C60D56B0206
bool CDataModel::CDep1_3::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	return m_qVec[0]->Value()<m_qVec[1]->Value();
}

//##ModelId=3C60D56B0209
std::string CDataModel::CDep1_3::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nOverburden Bottom TMD < Guide Shoe depth "));
}

//##ModelId=3C60D56B02C0
CDataModel::CDep2_1::CDep2_1(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfTailCement());
	
}

//##ModelId=3C60D56B02C2
bool CDataModel::CDep2_1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	if(m_pDM->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
	{
		return m_qVec[0]->Value()<m_qVec[1]->Value();
	}
	else
	{
		return TRUE;
	}

}

//##ModelId=3C60D56B02C5
std::string CDataModel::CDep2_1::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nTop of Cement TMD < Top of Tail Cement TMD"));
}

//##ModelId=3C60D56B0370
CDataModel::CDep2_2::CDep2_2(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfTailCement());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
}

//##ModelId=3C60D56B037C
bool CDataModel::CDep2_2::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	if(m_pDM->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
	{
		return m_qVec[0]->Value()<m_qVec[1]->Value();
	}
	else
	{
		return TRUE;
	}
}


//##ModelId=3C60D56B037F
std::string CDataModel::CDep2_2::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nTop of Tail Cement TMD < Guide Shoe depth"));
}

//##ModelId=3C60D56C0063
CDataModel::CDep2_3::CDep2_3(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD());
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
}

//##ModelId=3C60D56C006F
bool CDataModel::CDep2_3::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	return m_qVec[0]->Value()<m_qVec[1]->Value();
}

//##ModelId=3C60D56C0072
std::string CDataModel::CDep2_3::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nTop of Cement TMD < Guide Shoe depth "));
}

//##ModelId=3C60D56C011A
CDataModel::CDep3_1::CDep3_1(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->FormationTopography()->Overburden()->OverburdenBottomTVD());
	m_qVec.push_back(dm->DrillingScheme()->OverburdenBottomTMD());
	
}

//##ModelId=3C60D56C011C
bool CDataModel::CDep3_1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if(RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	return m_qVec[0]->Value()<= m_qVec[1]->Value();
}


//##ModelId=3C60D56C0129
std::string CDataModel::CDep3_1::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec0;
	sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


	return (LPCTSTR) (CString(DEPWARNING) + sVec1 + CString(" >= ") + sVec0 + CString("\nOverburden bottom TMD >= Model top TVD"));
}


/////////DEPENDENCIES/////////////

//##ModelId=3C60D56C01F4
CDataModel::CDep4_1::CDep4_1(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD());
	m_qVec.push_back(dm->DrillingScheme()->OverburdenBottomTMD());
}

//##ModelId=3C60D56C01FB
bool CDataModel::CDep4_1::RemainingsUndefined(const CDoubleQuantity *pQuantity ) const
{
	return FALSE;
}

//##ModelId=3C60D56C01F6
bool CDataModel::CDep4_1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	m_DependencyString.clear();

	double dblMax;

	if(!m_qVec[0]->Undefined() && !m_qVec[1]->Undefined())
	{
		if(m_qVec[0]->Value() > m_qVec[1]->Value())
		{
			m_DependencyString= "Top of Cement TMD < Analysis point TMD";
			dblMax = m_qVec[0]->Value();
		}
		else
		{
			m_DependencyString = "Overburden bottom TMD < Analysis point TMD";
			dblMax = m_qVec[1]->Value();
		}
	}
	else if(!m_qVec[0]->Undefined())
	{
		m_DependencyString= "Top of Cement TMD < Analysis point TMD";
		dblMax = m_qVec[0]->Value();
	}
	else if(!m_qVec[1]->Undefined())
	{
		m_DependencyString = "Overburden bottom TMD < Analysis point TMD";
		dblMax = m_qVec[1]->Value();
	}
	else
	{
		return TRUE;
	}

	if(m_pDM->Analysis()->NrOfAnalysisPoints() > 0)
	{
		return m_pDM->Analysis()->GetAnalysisPointAt(0)->TMD()->Value() > dblMax;
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D56C01F9
std::string CDataModel::CDep4_1::GetDependencyString() const
{
	return m_DependencyString;
}

//##ModelId=3C60D56C02C0
CDataModel::CDep4_2::CDep4_2(CDataModel* dm)
{
	m_pDM=dm;
	
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
}

//##ModelId=3C60D56C02C2
bool CDataModel::CDep4_2::CheckQuantity(const CDoubleQuantity* pQuantity) const
{

	if(m_pDM->Analysis()->NrOfAnalysisPoints() > 0)
	{
		int iNrAP=m_pDM->Analysis()->NrOfAnalysisPoints();
		return m_pDM->Analysis()->GetAnalysisPointAt(iNrAP-1)->TMD()->Value() < m_qVec[0]->Value();
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D56C02CF
std::string CDataModel::CDep4_2::GetDependencyString() const
{
	return "Analysis point TMD < Guide Shoe depth";
}

//##ModelId=3C60D56C037B
CDataModel::CDep1::CDep1(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->FormationTopography()->ElevationTVD());
	m_qVec.push_back(dm->FormationTopography()->Seawater()->WaterDepth());
}

//##ModelId=3C60D56C037D
bool CDataModel::CDep1::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;
	//THE RULE !!!
	if(m_pDM->FormationTopography()->Offshore())
		return m_qVec[1]->Value() > 0;
	else
		return TRUE;
}

//##ModelId=3C60D56C0380
std::string CDataModel::CDep1::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec1;
	sVec1.Format("%#1.2f",m_qVec[1]->Value(us));

  return (LPCTSTR)(CString(sVec1 + " > 0\nWater Depth > 0"));
}

//##ModelId=3C60D56D005E
CDataModel::CDep2::CDep2(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->FormationTopography()->ElevationTVD());
	m_qVec.push_back(dm->FormationTopography()->Overburden()->OverburdenBottomTVD());
}

//##ModelId=3C60D56D0060
bool CDataModel::CDep2::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	return m_qVec[0]->Value() < m_qVec[1]->Value();
}

//##ModelId=3C60D56D0063
std::string CDataModel::CDep2::GetDependencyString() const
{
	if(m_pDM->FormationTopography()->Offshore())
	{
		QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

		CString sVec0;
		sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
		CString sVec1;
		sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


		return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nMean sea level < Overburden TVD"));
	}
	else
	{
		QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

		CString sVec0;
		sVec0.Format("%#1.2f",m_qVec[0]->Value(us));
		CString sVec1;
		sVec1.Format("%#1.2f",m_qVec[1]->Value(us));


		return (LPCTSTR) (CString(DEPWARNING) + sVec0 + CString(" < ") + sVec1 + CString("\nMean ground level < Overburden TVD"));
	}
}

//##ModelId=3C60D56D011F
CDataModel::CDep3::CDep3(CDataModel* dm)
{
	m_pDM=dm;
  m_qVec.push_back(dm->FormationTopography()->ElevationTVD());
	m_qVec.push_back(dm->FormationTopography()->Seawater()->WaterDepth());
	m_qVec.push_back(dm->FormationTopography()->Overburden()->OverburdenBottomTVD());
}

//##ModelId=3C60D56D0129
bool CDataModel::CDep3::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	if(m_pDM->FormationTopography()->Offshore())
	{
		return m_qVec[0]->Value() + m_qVec[1]->Value() < m_qVec[2]->Value();
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D56D012C
std::string CDataModel::CDep3::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();

	CString sVec01;
	sVec01.Format("%#1.2f",m_qVec[0]->Value(us) + m_qVec[1]->Value(us));
	CString sVec2;
	sVec2.Format("%#1.2f",m_qVec[2]->Value(us));

	return (LPCTSTR) (CString(DEPWARNING) + sVec01 + CString(" < ") + sVec2 + CString("\nAir gap + Water depth < Overburden TVD"));
}

//##ModelId=3C60D56D01E5
CDataModel::CDep4::CDep4(CDataModel* dm)
{
	m_pDM=dm;
	//m_qVec.push_back((dm->FormationTopography()->GetFormationAt(0)->BottomTVD())); //>m_qBottomOfTopFormation));
	m_qVec.push_back(dm->FormationTopography()->Overburden()->OverburdenBottomTVD());
}

//##ModelId=3C60D56D01E7
bool CDataModel::CDep4::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
	if(m_pDM->FormationTopography()->NrOfFormations() > 0)
	{
		return m_pDM->FormationTopography()->GetFormationAt(0)->BottomTVD()->Value() > m_qVec[0]->Value();
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D56D01F4
std::string CDataModel::CDep4::GetDependencyString() const
{
	return "Overburden bottom TVD < Bottom of Top-Formation";
}


//##ModelId=3C60D56D02B0
CDataModel::CDep5::CDep5(CDataModel* dm)
{
	m_pDM=dm;
	m_qVec.push_back(dm->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD());
}

//##ModelId=3C60D56D02B2
bool CDataModel::CDep5::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
		if (RemainingsUndefined(pQuantity))
		return TRUE;

    int nSections = m_pDM->DrillingScheme()->NrOfDrillingDirections();
    CDrillingDirectionSection* pLastSection = m_pDM->DrillingScheme()->GetDrillingDirectionSection(nSections - 1);
    double dBottomTMD = pLastSection->DrillingSectionBottomTMD()->Value();
		return m_qVec[0]->Value() <= dBottomTMD;
}

//##ModelId=3C60D56D02C0
std::string CDataModel::CDep5::GetDependencyString() const
{
	QU::UNIT us = m_pDM->DependenciesManager.GetUnit();
	CLengthQuantity tmpLQ;

  int nSections = m_pDM->DrillingScheme()->NrOfDrillingDirections();
  CDrillingDirectionSection* pLastSection = m_pDM->DrillingScheme()->GetDrillingDirectionSection(nSections - 1);
  double dBottomTMD = pLastSection->DrillingSectionBottomTMD()->Value();
	tmpLQ.Value(dBottomTMD, CDoubleQuantity::SI_UNIT);

	CString sTotalValue;
	sTotalValue.Format("%#1.2f",tmpLQ.Value(us));
	
	return (LPCTSTR) (CString("Guide shoe depth <= bottom TMD\nGuide shoe depth <= ") + sTotalValue);
}

//##ModelId=3CA0119300CB
CDataModel::CDep6::CDep6(const CDoubleQuantity* pBoreholeQ)
{
	m_qVec.push_back(pBoreholeQ);
}

//##ModelId=3CA0119300DD
CDoubleQuantity* CDataModel::CDep6::GetCasingOuter()
{
	if(m_qVec.size()==1)
		return 0;

	return const_cast<CDoubleQuantity*>(m_qVec[1]);
}

//##ModelId=3CA0119300DB
void CDataModel::CDep6::SetCasingOuter(CDoubleQuantity* pCasingOuterDiameter)
{
	if(pCasingOuterDiameter==0)
	{
		if(m_qVec.size() == 2)
			m_qVec.erase(m_qVec.begin()+1);
		return;
	}

	if(m_qVec.size() == 2)
	{
		m_qVec.erase(m_qVec.begin()+1);
	}
	m_qVec.push_back(pCasingOuterDiameter);

}


//##ModelId=3CA0119300CD
bool CDataModel::CDep6::CheckQuantity(const CDoubleQuantity* pQuantity) const 

{
	m_DepString.Empty();

	if(m_qVec.size() == 1) 
		return TRUE;

	if (RemainingsUndefined(pQuantity))
		return TRUE;

	/*
	if(pQuantity == m_qVec[0])
		m_DepString;
	else
		m_DepString;
*/
	
	//THE RULE !!!
	return m_qVec[0]->Value() > m_qVec[1]->Value();
}


//##ModelId=3CA0119300D0
std::string CDataModel::CDep6::GetDependencyString() const
{
	//if(m_DepString.IsEmpty())
		return  "Borehole diameter > Casing Outer Diameter";
	//else
		//return m_DepString;
}

//##ModelId=3C8D9E00032E
void CDataModel::InitDianaX()
{
	CString strDiaPath = EnsureBackslash(m_sDianaPath);
  CString strDiaSharePath = EnsureBackslash(m_sDianaSharePath);

#if USE_SE40
    m_pDianaX->SetEnvironmentVar( "STP0", strDiaPath + "binseg\\ap\\se40.exe");
#else
    m_pDianaX->SetEnvironmentVar( "STP0", strDiaPath + "binseg\\ap\\se51.exe");
#endif
    m_pDianaX->SetEnvironmentVar( "DIAPATH", strDiaPath);
    m_pDianaX->SetEnvironmentVar( "DIASHARE", strDiaSharePath);
    m_pDianaX->SetEnvironmentVar( "DIAERRPATH", strDiaSharePath + "src");
    m_pDianaX->SetEnvironmentVar( "DIASLIB", strDiaSharePath + "lib");
    m_pDianaX->SetEnvironmentVar( "APP_SPC_LOGGING", "YES");
    m_pDianaX->SetEnvironmentVar( "DIALIB", strDiaPath + "lib");

#if USE_SE40
	m_pDianaX->SetSuccessMessageString("/DIANA/AP/SE40");
#else
	m_pDianaX->SetSuccessMessageString("*** SUCCESSFUL STRUCTURAL ANALYSIS");
#endif
	
			
}

//##ModelId=3C8D9E0003BA
void CDataModel::InitFgvRunCtr()
{
	CString strDiaPath = EnsureBackslash(m_sDianaPath);
  CString strDiaSharePath = EnsureBackslash(m_sDianaSharePath);
	m_pFgvRunCtr->SetAutoCloseDialog(TRUE);
	m_pFgvRunCtr->SetFgvPath(strDiaPath + "bin");
  //m_pFgvRunCtr->SetFgvPath("C:\\Windows\\System32");
#if USE_SE40
  m_pFgvRunCtr->SetFgvKey(strDiaPath + "lib");
#endif
	m_pFgvRunCtr->SetExeName("idiana.exe");
  //m_pFgvRunCtr->SetExeName("cmd.exe");
}

//##ModelId=3DDA0C480320
BOOL CDataModel::CanStartAnalysis()
{
	return BatchTable()->CanBuildBatchTable();
}


//##ModelId=3DDA0C480321
BOOL CDataModel::StartAnalysis(CStartAnalysisData& sud)
{
	CString sWorkDir = EnsureNoBackslash(sud.Workingdir);
	CString sTempDir = EnsureNoBackslash(sud.TempDir);
#if USE_SE40
  m_sDianaPath  = "D:\\users\\wedx\\svn_trunk\\DSealect\\diana";
  m_sDianaSharePath = "D:\\users\\wedx\\svn_trunk\\DSealect\\diana\\share";
#else
	m_sDianaPath  = EnsureNoBackslash(sud.DianaPath);
  m_sDianaSharePath = EnsureNoBackslash(sud.DianaSharePath);
#endif
	
	sWorkDir.TrimRight();
	sWorkDir.TrimLeft();
	
	m_sDianaPath.TrimRight();
	m_sDianaPath.TrimLeft();

	InitDianaX();
	InitFgvRunCtr();

	//Create a directory(sBasename) in the working directory
	CString sBaseDirectory = EnsureBackslash(sWorkDir) + sud.Basename;
	CreateDirectory(sBaseDirectory);
	DeleteFilesInDir(sBaseDirectory,"*.*");

	//Create FemGv startup file in BaseDirectory
	CString sFgvStr = EnsureBackslash(sBaseDirectory) + FGV_STR;
	if(!WriteMeshFile(sFgvStr))
		return FALSE;

	//Let FemGv create a diana-dat file
	m_pFgvRunCtr->SetWorkingDir(sBaseDirectory);
	m_pFgvRunCtr->SetMonitorLogfile(TRUE);
	m_pFgvRunCtr->SetMonitorCaption("Creating mesh ...");

	//run FemGv
/*
  // go to working dir
  char cwd[_MAX_PATH];
  getcwd(cwd, _MAX_PATH);
  chdir(sBaseDirectory);

  putenv("FGVSTR=.");

  int nRet;
  {
    CWaitCursor cur;
    nRet = spawnl(_P_WAIT, EnsureBackslash(m_sDianaPath) + "bin\\idiana.exe", "idiana", "-Alpha", "-NoConsoleWindow", 0);
  }

  // and change back to original directory
  chdir(cwd);
*/
	if(!m_pFgvRunCtr->RunFemView())
	{
		DeleteFilesInDir(sBaseDirectory,"*.*");
		if(IsDirEmpty(sBaseDirectory))
			RemoveDirectory(LPCSTR(sBaseDirectory));

		AfxMessageBox("Calculation aborted");
		return FALSE;
	}

	CString sMeshFileName = EnsureBackslash(sBaseDirectory) + MESH_FILE;

	for(int i=0;i<m_pBatchTable->RowCount();i++)
	{
		if(!m_pBatchTable->GetRow(i)->UseForCalculation())
			continue;
		
		m_bIsCalculated=TRUE;

		CString sPath;
		sPath.Format("%s%i", EnsureBackslash(sBaseDirectory) + sud.Basename,i+1);

		//create a sub directory in the base directory
		CreateDirectory(sPath);
		EmptyDirectory(sPath);
		CString sFileName;
		
		//copy the by femgen created "mesh.dat" file to the current subdirectory
		CopyFile(
			  LPCTSTR(sMeshFileName),                           // name of an existing file
			  LPCTSTR(EnsureBackslash(sPath) + MESH_FILE),      // name of new file
			  FALSE                                             // fail if exist?
		);

		//write analysis file, read by Diana (se40.exe)
		sFileName = EnsureBackslash(sPath)+ANALYSIS_FILE;
		if(!m_pBatchTable->GetRow(i)->WriteAnalysisFile(sFileName))
			return FALSE;

		//write material file, read by Diana (se40.exe)
		sFileName=EnsureBackslash(sPath)+MATERIAL_FILE; 
		if(!m_pBatchTable->GetRow(i)->WriteMaterialFile(sFileName))
			return FALSE;
		
		if(i < (m_pBatchTable->RowCount()-1))
			m_pDianaX->SetAutoCloseDialog(TRUE); //automatic close the dialog
		else
			m_pDianaX->SetAutoCloseDialog(FALSE);//last run, wait for user to close
		
		m_pBatchTable->GetRow(i)->IsCalculated(TRUE); 
		m_pBatchTable->GetRow(i)->ClearResult();
		m_pBatchTable->GetRow(i)->CalculationSucceeded(FALSE);
		m_pBatchTable->GetRow(i)->HasResults(FALSE);
		m_pDianaXEventWnd->ResetFlags();

		//start DIANA !!!!!!!!!!!!!!!!
#if USE_SE40
		short DianaRet = m_pDianaX->ExecuteDiana("se40.exe",sPath,"","","dsealect.ff","dsealect");
#else
		short DianaRet = m_pDianaX->ExecuteDiana("se51.exe",sPath,"","","dsealect.ff","dsealect");
#endif
						
		

		if(m_pBatchTable->GetRow(i)->SaveDiagnostic())
			m_pBatchTable->GetRow(i)->ProcessDiagnosticFile(sPath);

		if( m_pDianaXEventWnd->StopedByUser() )
		{
			if(i < (m_pBatchTable->RowCount()-1))
			{
				if(MessageBox(0,"Abort further calculation ?","Abort",MB_YESNO)==IDYES)
				{
					break;
				}
			}
		}
		
		m_pBatchTable->GetRow(i)->CalculationSucceeded(m_pDianaXEventWnd->IsSucceded(0)); //"SUCCESSFUL STRUCTURAL ANALYSIS" //success
		m_pBatchTable->GetRow(i)->HasResults(m_pDianaXEventWnd->IsSucceded(1));           //"POST-PROCESSING MAXIMUM RESULTS"  //has results

		if(m_pBatchTable->GetRow(i)->StopBatchRun() && !m_pBatchTable->GetRow(i)->CalculationSucceeded() )
		{
			break;
		}

		CString sDSOFileName = EnsureBackslash(sPath) + MAXIMA_DSO;
		if(m_pBatchTable->GetRow(i)->HasResults())
		{
			CByteArray Bytes;
			ByteArrayFromFile((LPCSTR)sDSOFileName, Bytes);
			m_pBatchTable->GetRow(i)->SetByteArray(Bytes);
			m_pBatchTable->GetRow(i)->ProcessContourPlots(sPath);
		}
		

		if(m_pBatchTable->GetRow(i)->FurtherPostProcessing())
		{
			//delete all filos files, out files and temp files
			DeleteFilesInDir(sPath,"*.ff");
			DeleteFilesInDir(sPath,"*.out");
			DeleteFilesInDir(sPath,"*temp.dat");
		}
		else
		{
			EmptyDirectory(sPath);
			RemoveDirectory(LPCSTR(sPath));
		}
	}
	
	SaveToDatabase(sud.DatabaseName);
	DeleteFilesInDir(sBaseDirectory,"*.*");
	if(IsDirEmpty(sBaseDirectory))
		RemoveDirectory(LPCSTR(sBaseDirectory));

	return TRUE;
}

bool CDataModel::IsDeviated() const
{
  if(m_PolyLine.PointSize() < 2)
    return false;

  double eps = 1e-4;
  bool bVertical = (m_PolyLine.PointSize() == 2 &&
                    fabs(m_PolyLine.Point(0).X() - m_PolyLine.Point(1).X()) < eps &&
                    fabs(m_PolyLine.Point(0).Y() - m_PolyLine.Point(1).Y()) < eps);

  return !bVertical;
}

void CDataModel::SurveyFileName(const CString& name)
{
  m_strSurveyFileName = name;
}

const CString& CDataModel::SurveyFileName() const
{
  return m_strSurveyFileName;
}

//##ModelId=3C0DDDAF02CF
CTestLoads *CDataModel::TestLoads()
{
	return &m_TestLoads;
}

const geo::CPolyLine& CDataModel::PolyLine() const
{
  return m_PolyLine;
}

CBatchTableSelection& CDataModel::BatchTableSelection()
{
  return m_BatchTableSelection;
}

const CValueProfile& CDataModel::PorePressureProfile() const
{
  return m_PorePressureProfile;
}

CValueProfile& CDataModel::PorePressureProfile()
{
  return m_PorePressureProfile;
}

const CString& CDataModel::PorePressureProfileFileName() const
{
  return m_strPorePressureProfileFileName;
}

void CDataModel::PorePressureProfileFileName(const CString& strName)
{
  m_strPorePressureProfileFileName = strName;
  GetDoc()->SetModifiedFlag();
}

const CValueProfile& CDataModel::DensityProfile() const
{
  return m_DensityProfile;
}

CValueProfile& CDataModel::DensityProfile()
{
  return m_DensityProfile;
}

const CString& CDataModel::DensityProfileFileName() const
{
  return m_strDensityProfileFileName;
}

void CDataModel::DensityProfileFileName(const CString& strName)
{
  m_strDensityProfileFileName = strName;
  GetDoc()->SetModifiedFlag();
}

const CValueProfile& CDataModel::TemperatureProfile() const
{
  return m_TemperatureProfile;
}

CValueProfile& CDataModel::TemperatureProfile()
{
  return m_TemperatureProfile;
}

const CString& CDataModel::TemperatureProfileFileName() const
{
  return m_strTemperatureProfileFileName;
}

void CDataModel::TemperatureProfileFileName(const CString& strName)
{
  m_strTemperatureProfileFileName = strName;
  GetDoc()->SetModifiedFlag();
}


//##ModelId=3C8D9E0A0205
BOOL CDataModel::WriteMeshFile(LPCTSTR sFileName)
{

	FILE* file = fopen(sFileName,"w");

	if(!file)
		return FALSE;

	//some shortcuts
	CDrillingOperation  *pDO        = DrillingScheme()->GetDrillingOperation(0);
	CCementingOperation *pCEO       = pDO->CementingOperation();
	CCasingOperation    *pCAO       = pDO->CasingOperation();
	CCasingMaterial     *pCasingMat = CasingMatList()->GetMaterial(pCAO->CasingMaterialName());
	CMeshParam          *pMP        = MeshParam();


	fprintf(file,"\n");
	fprintf(file,"\n");
	fprintf(file,"!===================================================\n");
	fprintf(file,"!    FEMGV Startup file\n");
	fprintf(file,"!    Generated by Wellbore Cement Integrity Model version %s\n",VersionInfo().GetVersionString());
	fprintf(file,"!===================================================\n");
	fprintf(file,"\n");
	fprintf(file,"\n");


	fprintf(file,"FEMGEN MESH\n");
#if USE_SE40
	fprintf(file,"PROPERTY FE-PROG DIANA HTSTAG_PE\n");
#else
	fprintf(file,"PROPERTY FE-PROG DIANA HTSTAG_GE\n");
#endif
	fprintf(file,"YES\n");
	fprintf(file,"CONSTRUCT SPACE TOLERANCE OFF\n");
	fprintf(file,"UTILITY SETUP UNITS LENGTH METER\n");
	fprintf(file,"UTILITY SETUP UNITS MASS KILOGRAM\n");
	fprintf(file,"UTILITY SETUP UNITS TEMPERATURE CELSIUS\n");

	fprintf(file,"GEOMETRY POINT COORD 0\n");
	


	fprintf(file,"!     CasingExentricityShift\n");
	fprintf(file,"GEOMETRY POINT COORD 0 %f\n",-pMP->CasingExentricityShift());

	
	
	fprintf(file,"!     NDivTangent\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n",pMP->NDivTangent());
	fprintf(file,"CONSTRUCT SET OPEN ROCKO\n");

	fprintf(file,"!     Ring3Radius\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P1 %f\n",pMP->Ring3Radius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");

	fprintf(file,"!     Ring2Radius\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P1 %f\n",pMP->Ring2Radius());

	fprintf(file,"!     Ring1Radius\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P1 %f\n",pMP->Ring1Radius());

	fprintf(file,"CONSTRUCT SET OPEN ROCKI\n");
	fprintf(file,"!     BoreholeRadius (outer line of interface)\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P1 %f\n", pDO->BoreholeRadius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");

	fprintf(file,"CONSTRUCT SET OPEN CEMO\n");
	fprintf(file,"!     BoreholeRadius (inner line of interface)\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P1 %f\n", pDO->BoreholeRadius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");


	
	fprintf(file,"CONSTRUCT SET OPEN CEMI\n");
	fprintf(file,"!     CasingOuterRadius (outer line of interface)\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P2 %f\n", pCasingMat->CasingOuterRadius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");


	fprintf(file,"CONSTRUCT SET OPEN CASO\n");
	fprintf(file,"!     CasingOuterRadius (inner line of interface)\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P2 %f\n", pCasingMat->CasingOuterRadius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");


	fprintf(file,"CONSTRUCT SET OPEN CASI\n");
	fprintf(file,"!     CasingInnerRadius\n");
	fprintf(file,"GEOMETRY LINE CIRCLE P2 %f\n", pCasingMat->CasingInnerRadius());
	fprintf(file,"CONSTRUCT SET CLOSE\n");

	fprintf(file,"CONSTRUCT SET OPEN ROCK\n");

	fprintf(file,"!     -Ring3Factor\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n",-pMP->Ring3Factor());

	fprintf(file,"GEOMETRY SURFACE 4POINTS P3 P4 P8 P7\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P4 P5 P9 P8\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P5 P6 P10 P9\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P6 P3 P7 P10\n");

	fprintf(file,"!     Ring3Factor\n");
	fprintf(file,"MESHING DIVISION LINE L34 %i\n",pMP->Ring3Factor());

	fprintf(file,"!     -Ring2Factor\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n",-pMP->Ring2Factor());


	fprintf(file,"GEOMETRY SURFACE 4POINTS P7 P8 P12 P11\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P8 P9 P13 P12\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P9 P10 P14 P13\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P10 P7 P11 P14\n");


	fprintf(file,"!     Ring2Factor\n");
	fprintf(file,"MESHING DIVISION LINE L38 %i\n",pMP->Ring2Factor());


	fprintf(file,"!     -Ring1Factor\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n",-pMP->Ring1Factor());

	
	fprintf(file,"GEOMETRY SURFACE 4POINTS P11 P12 P16 P15\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P12 P13 P17 P16\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P13 P14 P18 P17\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P14 P11 P15 P18\n");


	fprintf(file,"!     Ring1Factor\n");
	fprintf(file,"MESHING DIVISION LINE L42 %i\n",pMP->Ring1Factor());

	fprintf(file,"CONSTRUCT SET REMOVE POINTS ALL\n");
	fprintf(file,"CONSTRUCT SET REMOVE LINES ALL\n");
	fprintf(file,"CONSTRUCT SET CLOSE\n");
	fprintf(file,"CONSTRUCT SET OPEN CEMENT\n");

	fprintf(file,"!     NDivCement\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n", pMP->NDivCement());

	fprintf(file,"GEOMETRY SURFACE 4POINTS P19 P20 P24 P23\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P20 P21 P25 P24\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P21 P22 P26 P25\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P22 P19 P23 P26\n");

	fprintf(file,"CONSTRUCT SET REMOVE POINTS ALL\n");
	fprintf(file,"CONSTRUCT SET CLOSE\n");
	fprintf(file,"CONSTRUCT SET OPEN CASING\n");

	fprintf(file,"!     NDivCasing\n");
	fprintf(file,"MESHING DIVISION DEFAULT %i\n", pMP->NDivCasing());
	fprintf(file,"GEOMETRY SURFACE 4POINTS P27 P28 P32 P31\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P28 P29 P33 P32\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P29 P30 P34 P33\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P30 P27 P31 P34\n");
	fprintf(file,"CONSTRUCT SET REMOVE POINTS ALL\n");
	fprintf(file,"CONSTRUCT SET CLOSE\n");
	

	fprintf(file,"MESHING DIVISION DEFAULT 1\n");
	fprintf(file,"CONSTRUCT SET OPEN INTERF\n");
	fprintf(file,"CONSTRUCT SET OPEN ROCEMI\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P15 P16 P20 P19\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P16 P17 P21 P20\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P17 P18 P22 P21\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P18 P15 P19 P22\n");
	fprintf(file,"CONSTRUCT SET REMOVE POINTS ALL\n");
	fprintf(file,"CONSTRUCT SET CLOSE ROCEMI\n");
	fprintf(file,"CONSTRUCT SET OPEN CECASI\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P23 P24 P28 P27\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P24 P25 P29 P28\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P25 P26 P30 P29\n");
	fprintf(file,"GEOMETRY SURFACE 4POINTS P26 P23 P27 P30\n");
	fprintf(file,"CONSTRUCT SET REMOVE POINTS ALL\n");
	fprintf(file,"CONSTRUCT SET CLOSE CECASI\n");
	fprintf(file,"CONSTRUCT SET CLOSE INTERF\n");

	fprintf(file,"CONSTRUCT SET OPEN SUPCEX\n");
	fprintf(file,"CONSTRUCT SET APPEND POINTS P19\n");
	fprintf(file,"CONSTRUCT SET CLOSE SUPCEX\n");
		
	fprintf(file,"CONSTRUCT SET OPEN SUPCEY\n");
	fprintf(file,"CONSTRUCT SET APPEND POINTS P19 P21\n");
	fprintf(file,"CONSTRUCT SET CLOSE SUPCEY\n");

	fprintf(file,"CONSTRUCT SET OPEN SUPCAX\n");
	fprintf(file,"CONSTRUCT SET APPEND POINTS P27\n");
	fprintf(file,"CONSTRUCT SET CLOSE SUPCAX\n");

	fprintf(file,"CONSTRUCT SET OPEN SUPCAY\n");
	fprintf(file,"CONSTRUCT SET APPEND POINTS P27 P29\n");
	fprintf(file,"CONSTRUCT SET CLOSE SUPCAY\n");

#if USE_SE40
	fprintf(file,"MESHING TYPES ALL QU8 CQ16E\n");
#else
	fprintf(file,"MESHING TYPES ALL QU8 CQ24GE\n");
#endif
	fprintf(file,"MESHING TYPES INTERF IL33 CL12I\n");
	fprintf(file,"MESHING GENERATE\n");

	fprintf(file,"!     dummy materials\n");
	fprintf(file,"PROPERTY MATERIAL MA1 ELASTIC ISOTROP 10 0.2\n");
	fprintf(file,"PROPERTY MATERIAL MA2 ELASTIC ISOTROP 10 0.2\n");
	fprintf(file,"PROPERTY MATERIAL MA3 ELASTIC ISOTROP 10 0.2\n");
	fprintf(file,"PROPERTY MATERIAL MA4 ELASTIC INTERFAC 10. 10.\n");
	fprintf(file,"PROPERTY MATERIAL MA5 ELASTIC INTERFAC 10. 10.\n");
	fprintf(file,"PROPERTY PHYSICAL PH1 GEOMETRY INTERFAC LINE\n");
	fprintf(file,"PROPERTY ATTACH ROCK MA1\n");
	fprintf(file,"PROPERTY ATTACH CEMENT MA2\n");
	fprintf(file,"PROPERTY ATTACH CASING MA3\n");
	fprintf(file,"PROPERTY ATTACH ROCEMI MA4 PH1\n");
	fprintf(file,"PROPERTY ATTACH CECASI MA5 PH1\n");

	fprintf(file,"!     at borehole wall compressive\n");
	fprintf(file,"PROPERTY LOADS PRESSURE BORHOL 1 ROCKI ROCK -1. NORMAL\n");

	fprintf(file,"!     at outer wall of cement compressive\n");
	fprintf(file,"PROPERTY LOADS PRESSURE 2 CEMO CEMENT -1. NORMAL\n");

	fprintf(file,"!     at inner wall of cement compressive\n");
	fprintf(file,"PROPERTY LOADS PRESSURE 3 CEMI CEMENT -1. NORMAL\n");

	fprintf(file,"!     at outer wall of casing compressive\n");
	fprintf(file,"PROPERTY LOADS PRESSURE 4 CASO CASING -1. NORMAL\n");

	fprintf(file,"!     at inner wall of casing compressive\n");
	fprintf(file,"PROPERTY LOADS PRESSURE 5 CASI CASING -1. NORMAL\n");

	fprintf(file,"!     save diana datfile\n");
	fprintf(file,"UTILITY WRITE DIANA %s\n",MESH_FILE);
	fprintf(file,"YES\n");

	fprintf(file,"STOP\n");
	fprintf(file,"YES\n");
	fprintf(file,"YES\n\n");


	fclose(file);


	return TRUE;
}


//##ModelId=3C8D9E030040
BOOL CDataModel::CheckAllMaterials()
{
	//check formation material /////////////////////////////////////////////////
	if(FormationTopography()->NrOfFormations()<1)
		return FALSE;


	CString sName;
	for(int i=0;i<FormationTopography()->NrOfFormations();i++)
	{
		sName=FormationTopography()->GetFormationAt(i)->MaterialName();
		if(sName.IsEmpty())
		{
			return FALSE;
		}
		else
		{
			if(!FormationMatList()->GetMaterial(sName))
				return FALSE;
		}

	}

	//check casing material /////////////////////////////////////////////////
	sName = DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	if(sName.IsEmpty())
	{
		return FALSE;
	}
	else
	{
		if(!CasingMatList()->GetMaterial(sName))
			return FALSE;
	}

	//check cement material /////////////////////////////////////////////////
	bool bLeadAndTail= DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements();

	

	long iNumberOfCementScenarios = DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();
	if(iNumberOfCementScenarios<1)
		return FALSE;

	CString sCementMatName;
	for(int i=0;i< iNumberOfCementScenarios; i++)
	{
		CCementScenario* pCC = DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->CementScenarioAt(i);
		sCementMatName = pCC->HeadMaterialName();
		bool bCheckTail = bLeadAndTail;

		CheckName:
		if(sCementMatName.IsEmpty())
		{
			return FALSE;
		}
		else
		{
			if(!CementMatList()->GetMaterial(sCementMatName))
				return FALSE;
		}

		if(bCheckTail)
		{
			sCementMatName = pCC->TailMaterialName();
			bCheckTail=false;
			goto CheckName;
		}


	}

	return TRUE;

}

//##ModelId=3CA0118F030D
double CDataModel::WaitingOnCementFluidPressureAtDepth(double dDepth) 
{

	CCasingOperation *pCaO = DrillingScheme()->GetDrillingOperation(0)->CasingOperation();
	CDrillingOperation * pDO = DrillingScheme()->GetDrillingOperation(0);
		

	if(pCaO->WorkStringUsed())
	{
    assert(TMDtoTVD(pCaO->WorkStringPackersTMD()->Value()).second);
		double WorkStringPackersTVD = TMDtoTVD(pCaO->WorkStringPackersTMD()->Value()).first;
		
		if(dDepth > WorkStringPackersTVD)
		{
			double FluidStressGrad = pCaO->DisplacementFluidStressGradient()->Value();
			double FluidCompressibility = pCaO->DispFluidCompressibility()->Value();
			double FluidPressure = 0.0 ; //DisplacementFluidOverpressure is removed

			return MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);
		}
		else
		{	
			if(pCaO->NewFluidInCasWSAnnulus())
			{
				double FluidStressGrad = pCaO->NewFluidStressGradient()->Value();
				double FluidCompressibility = pCaO->NewFluidCompressibility()->Value();
				double FluidPressure = pCaO->NewFluidOverpressure()->Value();

				return MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);
			}
			else
			{
				double FluidStressGrad = pDO->MudDensityAtSurface()->Value()/ACCELERATION_OF_GRAVITY;
				double FluidCompressibility = pDO->MudCompressibility()->Value();
				double FluidPressure = 0.0;

				return MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);
			}
		}
	}
	else
	{
		double FluidStressGrad = pCaO->DisplacementFluidStressGradient()->Value();
		double FluidCompressibility = pCaO->DispFluidCompressibility()->Value();
		double FluidPressure = 0.0;

		return MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);		

	}

}
