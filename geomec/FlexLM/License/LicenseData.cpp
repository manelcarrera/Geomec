/*****************************************************************
 * FILENAME:
 *   $Workfile: LicenseData.cpp $
 *
 * PURPOSE:
 *   Contains the dialog used to show calculation progress
 *
 * COPYRIGHT 1999 Shell Internationale Research Mij. B.V.
 *
 * CONFIDENTIALITY:
 *   This is a CONFIDENTIAL document.
 *   Neither the  whole  or any  part of this document may
 *   be  reproduced,  stored in  any retrieval  system  or
 *   transmitted in any form or by any means  (electronic,
 *   mechanical,  reprographic,  recording  or  otherwise)
 *   without the  prior  consent of the  copyright  owner.
 *
 * VERSION INFORMATION:
 *   $Revision: 30 $
 *   $Author: Esseniuse $
 *   $Date: 9/18/06 3:57p $
 *
 * PART OF MODULE:
 *   License
 *
 * MODULE CONTROLLER:
 *   Logica
 *****************************************************************/

// LicenseData.cpp : implementation file
//

#include "stdafx.h"
#include "EPTFlexLm.h"
#include "LicenseData.h"
#include "MessageReporter.h"


#include <string>
#include <stdlib.h> 
#include <windows.h>
#include <sstream>
#include <string>

#include "GeomecUtils.h"
#include "WindowsUtils.h"

#include <QStringList>


#ifdef _DEBUG
////#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

#define ASSERT(x)


namespace 
{

std::string& replaceAll(std::string& context, const std::string& from, const std::string& to)
{
	size_t lookHere = 0;
    size_t foundHere;
    while((foundHere = context.find(from, lookHere)) != std::string::npos)
    {
          context.replace(foundHere, from.size(), to);
          lookHere = foundHere + to.size();
    }
    return context;
}

QString LMUtilBorrow(const QString& command)
{
  return QString("\"" + EnsureBackslash(GetAppPath()) + "\\lmutil.exe\" lmborrow " + command);
}

};

#define LMUtilBorrowStatus  LMUtilBorrow("-status")
#define LMUtilBorrowFeature LMUtilBorrow("mcslmd")
#define LMUtilBorrowClear   LMUtilBorrow("-clear")
#define LMUtilBorrowReturn  LMUtilBorrow("-return")


const char *COivLicenseData::m_FeatureNames[]= {"MeshVizXLM","MeshViz","OpenInventor"};

void COivLicenseData::setupBorrow(const int day, const int month, const int year)
{
  std::stringstream s_day; s_day<< day;
  std::stringstream s_year; s_year<< year;
  std::string cmd= LMUtilBorrowFeature.toStdString() + std::string(" ");
  cmd += std::string(s_day.str()) + std::string("-");

  switch(month)
  {
  case  1: cmd += std::string("jan"); break;
  case  2: cmd += std::string("feb"); break;
  case  3: cmd += std::string("mar"); break;
  case  4: cmd += std::string("apr"); break;
  case  5: cmd += std::string("may"); break;
  case  6: cmd += std::string("jun"); break;
  case  7: cmd += std::string("jul"); break;
  case  8: cmd += std::string("aug"); break;
  case  9: cmd += std::string("sep"); break;
  case 10: cmd += std::string("oct"); break;
  case 11: cmd += std::string("nov"); break;
  case 12: cmd += std::string("dec"); break;
  }
  cmd += std::string("-")+std::string(s_year.str());

  gm_windows_utils::Execute(cmd.c_str());
}

void COivLicenseData::returnBorrow()
{
 gm_windows_utils::Execute(LMUtilBorrowClear.toStdString().c_str());

 std::string cmd= LMUtilBorrowReturn.toStdString() + std::string(" ");
 for (int ii= 0; ii< sizeof(m_FeatureNames)/sizeof(m_FeatureNames[0]); ++ii)
 {
   std::string thisCmd= cmd + std::string(m_FeatureNames[ii]);
   gm_windows_utils::Execute(thisCmd.c_str());
 }
}

/////////////////////////////////////////////////////////////////////////////
// LicenseData
LicenseData* LicenseData::m_pLicenseInstance = NULL;

LicenseData::LicenseData(const char *  p_csAppName, const char *  p_csAppVersion)
  :	m_bInitialized(false),
    m_eLicenseType(NO_LICENSE),
	m_csAppVersion(_strdup(p_csAppVersion)),
	m_csAppName(_strdup(p_csAppName)),
	m_nLicenseCounter(0)
{
	m_pReporter = new MessageReporter;
}

LicenseData::~LicenseData()
{
	free((char *)m_csAppVersion);
	free((char *)m_csAppName);
	free((char *)m_csDemoLicense);
	free((char *)m_csFullLicense);

	delete m_pReporter;
}

LicenseData* LicenseData::CreateInstance (const char *  p_csAppName, const char *  p_csAppVersion)
{
	if (m_pLicenseInstance != NULL)
	{
		LicenseData::DestroyInstance();
	}
    
	m_pLicenseInstance = new LicenseData(p_csAppName, p_csAppVersion);

	bool bOK = m_pLicenseInstance->Initialize();

    if (!bOK)
    {
        DestroyInstance();
        return NULL;
    }
    
	m_pLicenseInstance->SetIntialized(bOK);
	
	
	
    
	return (LicenseData*)m_pLicenseInstance;
}

bool LicenseData::DestroyInstance ()
{
    if (m_pLicenseInstance != NULL)
    {
        m_pLicenseInstance->CheckIn();
    }

    EPTFlexLmTerminate();

	if(m_pLicenseInstance)
	{
		delete m_pLicenseInstance;
	}
	
	m_pLicenseInstance = NULL;

	return true;
}

LicenseData* LicenseData::GetInstance ()
{
	//m_pLicenseInstance shoud exist and it should have been initialized!
	ASSERT(m_pLicenseInstance != NULL);
	
	ASSERT(m_pLicenseInstance->IsInitialized());

	return m_pLicenseInstance;
}

void LicenseData::SetIntialized (bool p_bInitialized)
{
	m_bInitialized = p_bInitialized;
}

bool LicenseData::IsInitialized ()
{
	return m_bInitialized;
}

bool LicenseData::Initialize ()
{
	if (m_pLicenseInstance == NULL)
	{
		return false;
	}
	
	if (m_pLicenseInstance->IsInitialized())
    {
		return true;
    }

    char lv_cErrmessage[EPTFLEXLM_MAX_MESS_LEN];

    if (EPTFlexLmInit(lv_cErrmessage) != EPTFLEXLM_OK)
    {
        ASSERT(false);
        return false;
    }

    m_nLicenseCounter = 0;

	//intialise any new members
	std::string lv_sAppname = m_csAppName;
	if (lv_sAppname.length() == 0)
	{
		lv_sAppname = "APPLICATION";
	}
	std::string lv_sDemoLicense = lv_sAppname + "_DEMO";
	std::string lv_sFullLicense = lv_sAppname + "_FULL";

	m_csDemoLicense = _strdup(lv_sDemoLicense.c_str());
 	m_csFullLicense = _strdup(lv_sFullLicense.c_str());

	return true;
}

bool LicenseData::CheckOut()
{
    char lpVersionNumber[LICENSE_MAX_MSG_LEN+1];
	lpVersionNumber[LICENSE_MAX_MSG_LEN] = '\0';
	strncpy(lpVersionNumber, m_csAppVersion, LICENSE_MAX_MSG_LEN-1);

    char * lv_pType = GetFeature();

    return CheckOut(lv_pType, lpVersionNumber);
}

bool LicenseData::CheckOut(const char * p_pType, const char *p_pVersionNumber)
{
    //PTAStatusBarHandler lv_pStatusBar("Checking out license...");
    char lv_cErrmessage[EPTFLEXLM_MAX_MESS_LEN];

	ASSERT(m_nLicenseCounter == 0); //Dont checkout if there's one already checked out!

    //FlexLM only support integer or double values as version numbers. Duh...
    //So if we have a patch release, like 5.3.1 for example, we discard the rest when checking for the license.
	char versionNumber[EPTFLEXLM_MAX_MESS_LEN];
	strcpy(versionNumber, p_pVersionNumber);

	// Replace ", " with ","
	char *q = versionNumber;
	for (char *p = versionNumber; *p != 0; ++p)
	{
		if (!(p != versionNumber && *p == ' ' && *(p-1) == ','))
		{
			*q++ = *p;
		}
	}
	*q = 0;

	// Replace "," with "."
	for (char *p = versionNumber; *p != '\0'; ++p)
	{
		if (*p == ',') 
			*p = '.';
	}

	int count = 0;
	for (char *p = versionNumber; *p != 0; ++p)
	{
		count += *p == '.' ? 1 : 0;
		if (count == 2)
		{
			*p = 0;
			break;
		}
	}

 

	int lv_nRes = EPTFlexLmCheckOut((char*)p_pType, versionNumber, lv_cErrmessage);
    if (lv_nRes < 0)
    {
        return false;
    }
    else
    {
        m_nLicenseCounter++;
    }
    return true;
}

char* LicenseData::GetFeature()
{
    LicenseData* lv_pInstance = LicenseData::GetInstance();
    ASSERT(lv_pInstance != NULL);

    char * lv_pType = NULL;

    if (lv_pInstance != NULL)
    {
        //Determine correct feature...
        if (lv_pInstance->GetLicenseType() == DEMO_LICENSE)
        {
            lv_pType = (char*)lv_pInstance->GetDemoLicenseName();
        }
        else //FULL_LICENSE
        {
            lv_pType = (char*)lv_pInstance->GetFullLicenseName();
        }
    }
    return lv_pType;
}

void LicenseData::CheckIn()
{
    //PTAStatusBarHandler lv_pStatusBar("Checking in license...");
    
	char * lv_pType = GetFeature();
   
	if (m_nLicenseCounter == 1) //Only checkin if just 1 license is checked out!
	{
		m_nLicenseCounter--;
		EPTFlexLmCheckIn(lv_pType);
        
	}
}

void LicenseData::BorrowLicense(__time64_t p_ctEndDate)
{
	// TODO: Create callback for this functionality
    //CWaitCursor lv_cHourGlass; //lint !e1502
    
	
	//PTAStatusBarHandler lv_pStatusBar("Borrowing License...");

    //First return it before a borrow...
    ReturnBorrowLicense();

    CheckIn();
 
    char lv_szMsg[EPTFLEXLM_MAX_MESS_LEN];
  

	struct tm *tm = localtime(&p_ctEndDate);
    char buf[EPTFLEXLM_MAX_MESS_LEN];

	strftime(buf, sizeof(buf), "%d-%b-%Y", tm);

    int lv_nRes = EPTFlexLmSetBorrow(buf, lv_szMsg);
    if (!(lv_nRes == 0 && CheckOut()))
    {
        char lv_csMsg[EPTFLEXLM_MAX_MESS_LEN];
        sprintf(lv_csMsg, "License was not borrowed. Check that the selected period does not exceed %d days.", LICENSE_MAXIMUM_BORROW_DAYS);
		m_pReporter->ShowMessageBox(lv_csMsg,m_csAppName);
    }
	else
	{
		ReInitAndCheckoutLicense();
	}
}

char *LicenseData::GetBorrowList(char* p_pBorrow)
{
  // old overview, for fallback in case our new method fails

  char lv_cpList[EPTFLEXLM_MAX_MESS_LEN];
  memset(lv_cpList, 0, sizeof(lv_cpList));
  EPTFlexLmShowBorrow(lv_cpList);

	std::string lv_sList(  lv_cpList);
	lv_sList = replaceAll(lv_sList, "\n", "\r\n");
	lv_sList = replaceAll(lv_sList,",", "---");

  strcpy(p_pBorrow, lv_sList.c_str());


  // new overview
  QString lmutil = "\"" + EnsureBackslash(GetAppPath()) + "\\lmutil.exe\" lmborrow -status";
  QString output = gm_windows_utils::Execute(lmutil.toStdString().c_str());

  QStringList l = output.split("\n");

  if (l.size() > 0)
  {
    l.removeFirst();

    if (l.size() > 3)
    {
      l.removeAt(1);
    }
  }

  if (l.size() > 0)
  {
    memset(p_pBorrow, 0, EPTFLEXLM_MAX_MESS_LEN);
    strncpy(p_pBorrow, l.join("\r\n").toStdString().c_str(), EPTFLEXLM_MAX_MESS_LEN);
  }

  return p_pBorrow;
}

bool LicenseData::ReturnBorrowLicense()
{
//    CWaitCursor lv_cHourGlass;//lint !e1502
    
	//PTAStatusBarHandler lv_pStatusBar("Attempting to return borrowed license...");

    //checkin currently borrowed item...
    CheckIn();
	bool returnValue = true;

	if(HasLicenseBorrowed())
	{
		char returnMsg[EPTFLEXLM_MAX_MESS_LEN];

		//Return the license for the current feature
		returnValue = EPTFlexLmReturnBorrow(GetFeature(), returnMsg) == EPTFLEXLM_OK;

		if (returnValue)
 		{
			ReInitAndCheckoutLicense();
		}
	}

    return returnValue;
}

bool LicenseData::ReInitAndCheckoutLicense()
{
   bool returnValue = false;

	// free the license "job" handle, mv_job
	EPTFlexLmTerminate();
	char returnMsg[EPTFLEXLM_MAX_MESS_LEN];

	// Initializes FLEXlm 
	memset(returnMsg, 0, sizeof(returnMsg));
	returnValue = EPTFlexLmInit(returnMsg) == EPTFLEXLM_OK;
	if (returnValue)
	{
		m_nLicenseCounter = 0;
		//Borrow returned, checkout normal license...
		returnValue = CheckOut();
	}
	return returnValue;
}

bool LicenseData::HasLicenseBorrowed()
{
	char buffer[PTA_MAX_BORROWLIST_LEN];
	memset(buffer, 0, sizeof(buffer));
	GetBorrowList(buffer);

	std::string borrowList(buffer);

	char * featureType = GetFeature();
	std::string feature(featureType);

	return borrowList.find(feature) != -1;
}

bool LicenseData::SetupLicense(const bool p_bChecking, bool p_bLogginOnly, bool (*p_GracePerdiod)())
{
    //PTAStatusBarHandler lv_pStatusBar("Setting up license...");

	bool lv_bLoaded = false;
	bool lv_bChecking = p_bChecking;
	int lv_nAttempts = 0;

	//if not checking the license is automatically set to FULL
	if (!lv_bChecking)
	{
		SetLicenseType(LicenseData::FULL_LICENSE);
		lv_bLoaded = true;
	}
	
	std::string lv_csLicenseFileEdit;
	//This will only be done while the checking is not 
	while (lv_bChecking || (p_bLogginOnly && lv_nAttempts < 2))
	{
		lv_nAttempts++;

		
		std::string lv_csVersionNumber(  m_csAppVersion);
		lv_csVersionNumber = replaceAll(lv_csVersionNumber, ", ", ".");
		lv_csVersionNumber = replaceAll(lv_csVersionNumber, ",", ".");
		
        char lv_sVersionNumber[LICENSE_MAX_MSG_LEN+1];
		lv_sVersionNumber[LICENSE_MAX_MSG_LEN] = '\0';
		strncpy(lv_sVersionNumber, lv_csVersionNumber.c_str(), LICENSE_MAX_MSG_LEN-1);
        
		if (CheckOut((char*)GetFullLicenseName(),lv_sVersionNumber))
		{
			SetLicenseType(LicenseData::FULL_LICENSE);
			lv_bChecking = false;
			lv_bLoaded = true;
		}
		else
		{
            //Check demo license just in case...
            if (CheckOut((char*)GetDemoLicenseName(),lv_sVersionNumber))
		    {
			    SetLicenseType(LicenseData::DEMO_LICENSE);
			    lv_bChecking = false;
			    lv_bLoaded = true;
    		}
            else
            {
				if (p_GracePerdiod && p_GracePerdiod() == true)
				{
					SetLicenseType(LicenseData::DEMO_LICENSE);
				    lv_bChecking = false;
				    lv_bLoaded = true;
				}
				else if (p_bLogginOnly)
				{
					//For logging purposes only. Just in case we do a retry if the first
					//one fails to make sure we are not stopped by a timeout. 
					if (lv_nAttempts > 1) 
					{
						lv_bChecking   = false;
					}
					else
					{
						Sleep(2000);
						lv_bChecking   = true;
					}
				}
				else // if (!p_bLogginOnly)
				{
					// old fashioned way checking
					//
					lv_bChecking = 0 != m_pReporter->ShowRetryBox( "No available full or demo licenses found for this application. Please contact local support.",m_csAppName);
				}
            }
        }
    }
	return lv_bLoaded;
}


void LicenseData::SetReporter(MessageReporter *p_pReporter)
{
	delete m_pReporter;	
	m_pReporter = p_pReporter;
}
