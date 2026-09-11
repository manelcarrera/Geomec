// DSealectDoc.cpp : implementation of the CDSealectDoc class
//

#include "stdafx.h"
#include "DSealect.h"
#include "FormationMaterialDatabase.h"
#include "CasingMaterialDatabase.h"
#include "CementMaterialDatabase.h"
#include "DSModelDatabase.h"
#include "DSealectDoc.h"
#include "Global.h"
#include "CalculationWindowDlg.h"
#include "IRegistry.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


 
/////////////////////////////////////////////////////////////////////////////
// CDSealectDoc



IMPLEMENT_DYNCREATE(CDSealectDoc, CDocument)

BEGIN_MESSAGE_MAP(CDSealectDoc, CDocument)
	//{{AFX_MSG_MAP(CDSealectDoc)
	ON_COMMAND(ID_APP_RUN, OnAppRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSealectDoc construction/destruction

static CDSealectDoc *pDoc=NULL;


//////////////////
// Override SetModified flag to notify view when doc has changed,
//
//##ModelId=3DDA0C57018A
void CDSealectDoc::SetModifiedFlag(BOOL bModified)
{
   if(IsModified() != bModified)
   {
		CDocument::SetModifiedFlag(bModified);
		UpdateAllViews(0);

		CString title = GetTitle();
		int asterisk = title.GetLength()-1;

		if(asterisk >=0 && title.GetAt(asterisk)=='*')
		{
			if (!bModified) //remove the asterix
				title.SetAt(asterisk,0);
		}
		else if (bModified)
			title += '*';//add the asterix

		SetTitle(title);
	}
}

//##ModelId=3C20440202C0
CDSealectDoc *CDSealectDoc::GetMe()
{
	return pDoc;
}

//##ModelId=3DDA0C5701E9
void CDSealectDoc::OnAppRun() 
{
	StartAnalysis();
}

//##ModelId=3DDA0C57019B
void CDSealectDoc::StartAnalysis()
{
	if(GetPathName().IsEmpty())
	{
		AfxMessageBox("First save this project");
		return;
	}

	CCalculationWindowDlg dlg(DSealectModel()->BatchTable());
	dlg.DoModal();
	UpdateAllViews(0);

}

//##ModelId=3C0DC15C0246
CDSealectDoc::CDSealectDoc()
{
	m_pDSealectModel=0;
	pDoc=this;
			
	m_eUnitSystem = QU::UNIT(GetApp()->RegistryX()->getProfileInt("paths", "UnitSystem",long(QU::SI_USER_UNIT)));
	SetUnitSystem(m_eUnitSystem,FALSE);

	m_sTempDir = GetApp()->RegistryX()->getProfileString("paths","TempDir",GetHomeDirectory() + "TempDir").toStdString().c_str();
	SetTempDir(m_sTempDir);
		
	m_sFileWorkDir = GetApp()->RegistryX()->getProfileString("paths","FileWorkDir",GetHomeDirectory() + "FileWorkDir").toStdString().c_str();
	SetFileWorkDir(m_sFileWorkDir);

	m_sFormationMatDatabasePath = GetApp()->RegistryX()->getProfileString("paths","FormationMatDatabase",QString((LPCTSTR) GetAppPath()) + "FormationMatDatabase.mdb").toStdString().c_str();
	SetFormationMatDatabasePath(m_sFormationMatDatabasePath);
	
	m_sCementMatDatabasePath = GetApp()->RegistryX()->getProfileString("paths","CementMatDatabase",QString((LPCTSTR) GetAppPath()) + "CementMatDatabase.mdb").toStdString().c_str();
	SetCementMatDatabasePath(m_sCementMatDatabasePath);

	m_sCasingMatDatabasePath = GetApp()->RegistryX()->getProfileString("paths","CasingMatDatabase",QString((LPCTSTR) GetAppPath()) + "CasingMatDatabase.mdb").toStdString().c_str();
	SetCasingMatDatabasePath(m_sCasingMatDatabasePath);

	m_bShowToolTip = FALSE;
		
}

//##ModelId=3C60D57803C9
BOOL CDSealectDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CString newName(lpszPathName);
	bool bSaveAs = !(newName == this->GetPathName());
	if(IsModified() && m_pDSealectModel->BatchTable()->HasResults())
	{
		if(bSaveAs)
		{
			m_pDSealectModel->InvalidateResults();

		}
		else
		{
			CString msg;
			msg = "Model inconsistent with existing results.\n";
			msg += "Saving will cause results to be lost.\n";
			msg += "Save as new project to keep old results.\n\n";
			msg += "Continue and lose results?";

			if(AfxMessageBox(msg,MB_YESNO) == IDYES)
			{
				m_pDSealectModel->InvalidateResults();
			}
			else
			{
				return TRUE;
			}

		}

	}

	if(FileExists(lpszPathName))
	{
		DeleteFile(lpszPathName);
	}

	m_pDSealectModel->SaveToDatabase(lpszPathName);
	
	SetModifiedFlag(FALSE);
	UpdateAllViews(NULL,UPDATE_DOC_SAVED);
	return TRUE;
}

//##ModelId=3DDA0C5701E8
BOOL CDSealectDoc::ContinueAndLoseResultsWarning()
{	
	if(m_pDSealectModel->BatchTable()->HasResults())
	{
		CString msg;
		msg = "Changing this parameter will be inconsistent with existing results.\n";
		msg += "Changing will cause results to be lost.\n";
		msg += "Save as new project to keep old results.\n\n";
		msg += "Continue and lose results?";

		if(AfxMessageBox(msg,MB_YESNO) == IDYES)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3C60D57803D9
BOOL CDSealectDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	try
	{
		if(m_pDSealectModel)
			delete m_pDSealectModel;

		m_pDSealectModel = new CDataModel(TempDir());// FileWorkDir());
		m_pDSealectModel->InitInstance();
		m_pDSealectModel->DependenciesManager.SetUnit(m_eUnitSystem);

		if(!m_pDSealectModel->LoadFromDatabase(lpszPathName))
		{
			delete m_pDSealectModel;
			m_pDSealectModel = new CDataModel(FileWorkDir());
			m_pDSealectModel->DependenciesManager.SetUnit(m_eUnitSystem);
			UpdateAllViews(NULL);
			return FALSE;
		}
/*
		std::vector<std::string> ret;
		ret= m_pDSealectModel->DependenciesManager.CheckAllDependencies();
		CString msg;

		if(ret.size())
		{
			for(int i=0 ;i<ret.size();i++)
			{
				msg=msg + ret[i].c_str() + "\n";
			}
			AfxMessageBox(msg,0,0);

			if(m_pDSealectModel)
				delete m_pDSealectModel;

			m_pDSealectModel = new CDataModel(FileWorkDir());
			m_pDSealectModel->DependenciesManager.SetUnit(m_eUnitSystem);
			UpdateAllViews(NULL);
			return FALSE;
		}
*/
	}
	catch(CDaoException* pErr)
	{
		TCHAR   szCause[255];
		CString strFormatted;

		pErr->GetErrorMessage(szCause, 255);

		strFormatted = _T("The data file could not be opened because of this error: ");
		strFormatted += szCause;

		AfxMessageBox(strFormatted);

		//delete pErr;	


		if(m_pDSealectModel)
			delete m_pDSealectModel;

		m_pDSealectModel = new CDataModel(FileWorkDir());
		m_pDSealectModel->DependenciesManager.SetUnit(m_eUnitSystem);
		return FALSE;
	}

	UpdateAllViews(NULL);
	return TRUE;

}


//##ModelId=3C0DC15C0261
CDSealectDoc::~CDSealectDoc()
{
	if(m_pDSealectModel)
		delete m_pDSealectModel;
	
	WriteToRegistry();

}

//##ModelId=3C0DC15C0253
BOOL CDSealectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if(m_pDSealectModel)
		delete m_pDSealectModel;
	

	m_pDSealectModel = new CDataModel(TempDir());//(FileWorkDir());
	
	m_pDSealectModel->DependenciesManager.SetUnit(m_eUnitSystem);

	UpdateAllViews(NULL);
	SetModifiedFlag(FALSE);
	

	// add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDSealectDoc serialization

//##ModelId=3C0DC15C0255
void CDSealectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDSealectDoc diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15C0263
void CDSealectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

//##ModelId=3C0DC15C0265
void CDSealectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDSealectDoc commands
//##ModelId=3C20440202DF
void CDSealectDoc::SetUnitSystem(QU::UNIT  newsystem,BOOL bUpdateViews /*=true*/)
{
	
	if(m_pDSealectModel)
	{
		m_pDSealectModel->DependenciesManager.SetUnit(newsystem);
	}
	m_eUnitSystem=newsystem;
	if(bUpdateViews)
		UpdateAllViews(NULL,UPDATE_UNITS);
}



//##ModelId=3DDA0C57017A
void CDSealectDoc::WriteToRegistry()
{
	GetApp()->RegistryX()->writeProfileString("paths","TempDir",(LPCTSTR) m_sTempDir);
	GetApp()->RegistryX()->writeProfileString("paths","FileWorkDir",(LPCTSTR) m_sFileWorkDir);
	GetApp()->RegistryX()->writeProfileInt("paths", "UnitSystem", long(UnitSystem()));
	GetApp()->RegistryX()->writeProfileString("paths","FormationMatDatabase",(LPCTSTR) m_sFormationMatDatabasePath);
	GetApp()->RegistryX()->writeProfileString("paths","CementMatDatabase",(LPCTSTR) m_sCementMatDatabasePath);	
	GetApp()->RegistryX()->writeProfileString("paths","CasingMatDatabase",(LPCTSTR) m_sCasingMatDatabasePath);

}

//##ModelId=3DDA0C5701E7
long CDSealectDoc::GetIndexUnitCombo()
{
	// because the order of the UnitSystem differs from the order in de unit-combobox
	if(long(UnitSystem()) == 0)	//Field
		return 2;
	else if(long(UnitSystem()) == 2) //US
		return 1;
	else if(long(UnitSystem()) == 3) //SI
		return 0;
	else
		return 0;

}

//##ModelId=3DDA0C5701D8
void CDSealectDoc::SetFormationMatDatabasePath(CString sPath)
{
	m_sFormationMatDatabasePath = sPath;
	if(!FileExists(m_sFormationMatDatabasePath))
	{
		CString sPath,sFileName;
		SplitPathAndFileName(m_sFormationMatDatabasePath,sPath,sFileName);
		CreateDirectory(sPath);
		CFormationMaterialDatabase fmd;
		fmd.Create(m_sFormationMatDatabasePath,CVersionInfo(1,0));
		fmd.Database().Close();
	}
	GetApp()->RegistryX()->writeProfileString("paths","FormationMatDatabase",(LPCTSTR) m_sFormationMatDatabasePath);
}

//##ModelId=3DDA0C5701C9
void CDSealectDoc::SetCementMatDatabasePath(CString sPath)
{
	m_sCementMatDatabasePath = sPath;
	if(!FileExists(m_sCementMatDatabasePath))
	{
		CString sPath,sFileName;
		SplitPathAndFileName(m_sCementMatDatabasePath,sPath,sFileName);
		CreateDirectory(sPath);
		CCementMaterialDatabase cemd;
		cemd.Create(m_sCementMatDatabasePath,CVersionInfo(1,0));
		cemd.Database().Close();
		
	}
	GetApp()->RegistryX()->writeProfileString("paths","CementMatDatabase",(LPCTSTR) m_sCementMatDatabasePath);	
}

//##ModelId=3DDA0C5701BA
void CDSealectDoc::SetCasingMatDatabasePath(CString sPath)
{
	m_sCasingMatDatabasePath = sPath;
	if(!FileExists(m_sCasingMatDatabasePath))
	{
		CString sPath,sFileName;
		SplitPathAndFileName(m_sCasingMatDatabasePath,sPath,sFileName);
		CreateDirectory(sPath);
		CCasingMaterialDatabase camd;
		camd.Create(m_sCasingMatDatabasePath,CVersionInfo(1,0));
		camd.Database().Close();
		
	}
	GetApp()->RegistryX()->writeProfileString("paths","CasingMatDatabase",(LPCTSTR) m_sCasingMatDatabasePath);
}

//##ModelId=3DDA0C5701A9
void CDSealectDoc::SetTempDir(CString sPath)
{
	m_sTempDir = sPath;
	if(!DirExists(m_sTempDir))
	{
		CreateDirectory(m_sTempDir);
	}
	GetApp()->RegistryX()->writeProfileString("paths","TempDir",(LPCTSTR) m_sTempDir);
}

//##ModelId=3DDA0C5701B8
void CDSealectDoc::SetFileWorkDir(CString sPath)
{
	m_sFileWorkDir = sPath;
	if(!DirExists(m_sFileWorkDir))
	{
		CreateDirectory(m_sFileWorkDir);
	}
	GetApp()->RegistryX()->writeProfileString("paths","FileWorkDir",(LPCTSTR) m_sFileWorkDir);
}
