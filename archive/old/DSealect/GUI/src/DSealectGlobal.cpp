

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <direct.h>
#include "DSealect.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include <vector>
#include "IRegistry.h"

#define USE_UNITBRACKETS 1

CString GetInstallLocation()
{
	QString strLoc;

	strLoc = GetApp()->RegistryX()->getSystemString( "Paths", "InstallLocation", "" );

	if( strLoc.isEmpty() )
	{
		AfxMessageBox( IDP_INSTALLLOCATIONNOTFOUND, MB_OK | MB_HELP | MB_ICONSTOP );
	}

	return strLoc.toStdString().c_str();
}

CString GetTempReg()
{
	QString sTempInRegistry;
	sTempInRegistry = GetApp()->RegistryX()->getProfileString( "Paths", "Temp", "" );
	if( sTempInRegistry.isEmpty() )
	{
		CString strInstallLoc = GetInstallLocation();
		sTempInRegistry = QString((LPCTSTR) strInstallLoc) + "\\Temp";

		if( !DirExists( strInstallLoc + "\\Temp" ) )
		{
			if( mkdir( strInstallLoc + "\\Temp" ) != 0 )
			{
				AfxMessageBox( IDP_CANNOTCREATETEMPDIR, MB_OK | MB_HELP | MB_ICONSTOP );
			}
		}
	}

//	ASSERT (sTempInRegistry!="");
	return sTempInRegistry.toStdString().c_str();
}

CString GetTempParent()
{
	QString sTempSelected;
	sTempSelected=GetApp()->RegistryX()->getProfileString( "Paths", "TempSpec", "" );

	if( sTempSelected.isEmpty() ) sTempSelected = "Manual";

	ASSERT (sTempSelected!="");

	CString sTemp;

	if (sTempSelected=="Manual")
		sTemp=GetTempReg();
	else
		if (sTempSelected=="EnvVar")
			sTemp=GetTempEnv();
		else
			sTemp="";

	return sTemp;
}

CString FindValidTempParent()
{
	//Temp in registry was not valid, try alternative in registry
	QString sTempSelected;
	sTempSelected=GetApp()->RegistryX()->getProfileString("Paths","TempSpec", "");

	CString sTemp;
	if (sTempSelected=="EnvVar")
		sTemp=GetTempEnv(); //try alternative
	else
		sTemp=GetTempReg();

	//Check if this one DOES exist
	CFileFind finder;
	BOOL bTempExists=finder.FindFile(sTemp);
	if (bTempExists)
	{
		//Store this alternative in registry
		if (sTempSelected=="Manual")
			GetApp()->RegistryX()->writeProfileString("Paths","TempSpec","EnvVar");
		else
			GetApp()->RegistryX()->writeProfileString("Paths","TempSpec","Manual");
	}
	else
	{
		//Notify user that no valid tempfolder was found
		CString sMessage;
		sMessage.LoadString(IDP_MSG_SELECTNEWTEMP);
		sTemp=BrowseFolder(NULL,sMessage,"");
		if (sTemp=="")
		{
			//User cancelled the selection, so quit program
			AfxMessageBox(IDP_MSG_NOTEMPFOLDER, MB_OK | MB_HELP);
			return "";
		}
		//Store this alternative in registry
		GetApp()->RegistryX()->writeProfileString("Paths","TempSpec","Manual");
		GetApp()->RegistryX()->writeProfileString("Paths","Temp",(LPCTSTR) sTemp);
	}


	return sTemp;
	
}

CString GetFemGVPath()
{
	QString sFGVPath;
//	sFGVPath=AfxGetApp()->GetProfileString("Paths","FemGV");
	sFGVPath=GetApp()->RegistryX()->getSystemString("Paths","FemGV", "");

	if( sFGVPath.isEmpty() )
	{
		AfxMessageBox( IDP_FGVPATHNOTFOUND, MB_OK | MB_HELP | MB_ICONSTOP );
	}


	return sFGVPath.toStdString().c_str();
}

CString GetDianaPath()
{
	QString sDianaPath;
	sDianaPath=GetApp()->RegistryX()->getSystemString("Paths","DiaPath", "");

	if( sDianaPath.isEmpty() )
	{
		AfxMessageBox( IDP_DIANAPATHNOTFOUND, MB_OK | MB_HELP | MB_ICONSTOP );
	}

	return sDianaPath.toStdString().c_str();
}

CString GetDianaSharePath()
{
	QString sDianaSharePath;
	sDianaSharePath=GetApp()->RegistryX()->getSystemString("Paths","DiaShare", "");

	if( sDianaSharePath.isEmpty() )
	{
		AfxMessageBox( "Could not find the location of the Diana share files in the registry", MB_OK | MB_HELP | MB_ICONSTOP );
	}

	return sDianaSharePath.toStdString().c_str();
}

CString GetMatLibPath()
{

	QString sLibPath;
	sLibPath=GetApp()->RegistryX()->getProfileString("Paths","MaterialLibrary", "");
/*
	if (sLibPath=="")
	{
		//Not found, try to find setting for GeoMec 3D in registry
		HKEY hkResult1;
		HKEY hkResult2;
		lRet=RegOpenKeyEx(HKEY_CURRENT_USER,"Software",0,KEY_READ,&hkResult1);
		if (lRet!=ERROR_SUCCESS)
		{
			return "";
		}
		lRet=RegOpenKeyEx(hkResult1,"VB and VBA Program Settings\\GEOMEC\\MaterialLibrary",0,KEY_READ,&hkResult2);
		if (lRet!=ERROR_SUCCESS)
		{
			lRet=RegCloseKey(hkResult1);
			return "";
		}
	
		DWORD lpByte;
		lpByte=REG_SZ;

		unsigned char *szDir;		//Resulting Value
		szDir=(unsigned char *)malloc(MAX_PATH);
		DWORD dwLength = MAX_PATH;	//Maximum length of value

		lRet=RegQueryValueEx(hkResult2,"DefaultDatabase",NULL,&lpByte,szDir,&dwLength);
		if (lRet==ERROR_SUCCESS)
			sLibPath=szDir;
		free(szDir);

		lRet=RegCloseKey(hkResult2);
		lRet=RegCloseKey(hkResult1);
		
	}
*/
	if( sLibPath.isEmpty() )
	{
		// get default location from registry
		// CString strInstallLoc = GetInstallLocation();
		QString strMatLibLoc = GetApp()->RegistryX()->getSystemString( "Paths", "DefaultMatLibLocation", "" );
		sLibPath = strMatLibLoc + "\\DefaultMat.gml";

		if( !DirExists( strMatLibLoc.toStdString().c_str() ) )
		{
			if( mkdir( strMatLibLoc.toStdString().c_str() ) != 0 )
			{
				AfxMessageBox( IDP_CANNOTCREATELIBRARYDIR, MB_OK | MB_HELP | MB_ICONSTOP );
			}
		}
	}
	return sLibPath.toStdString().c_str();;
}

CDSealectDoc *GetDoc()
{
	return CDSealectDoc::GetMe();
}

CDataModel* GetModel()
{
  return GetDoc()->DSealectModel();
}

void SetUnitlabel(CStatic *pUnitLabel,CDoubleQuantity* pQ)
{
	ASSERT(pUnitLabel!=0);
	pUnitLabel->SetWindowText(GetUnitString(pQ));	
}

CString GetUnitString(CDoubleQuantity* pQ)
{
	CDSealectDoc* pDoc=GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
			
	if (USE_UNITBRACKETS == 0)
	{
		return CString(pQ->UnitName(us).c_str());	
	}
	else //with unit brackets
	{
		std::stringstream str;
		str << "[" << pQ->UnitName(us).c_str() << "]"  << std::ends;
	    return CString(str.str().c_str());
	}
}

/*
FileDeleter(CFileFinder* ff = 0, const CString& FileSpec ="")
{
	if(ff)
	{
		// We Have a FileFinder
		if(ff->GetNextFile())
		{
			CString sDelName = ff->GetFileName();
			FileDeleter(ff);
			DeleteFile(sDelName);
		}
	} else {
		// We Need to create the FileFinder 
		CFileFind ff;
		if(ff.FindFile(FileSpec))
		{
			if(ff.GetNextFile())
			{
				CString sDelName = ff.GetFileName();
				FileDeleter(&ff);
				DeleteFile(sDelName);
			}
		}
	}
}
*/
