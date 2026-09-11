// Geomec.h : main header file for the GEOMEC application
//

#if !defined(AFX_GEOMEC_H__9C2309E0_4FE5_47F3_9FD2_B2B86AE4E8F3__INCLUDED_)
#define AFX_GEOMEC_H__9C2309E0_4FE5_47F3_9FD2_B2B86AE4E8F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if BUILD_LICENSE_OPTION == -1
#define LICENSING 0
#define LICENSE_BORROWING 0
#else
#define LICENSING 1
#define LICENSE_BORROWING 1
#endif

#include "geomecdoc.h"
#include "TNOWinApp.h"
#include "TempPath.h"
//#include "KeyFile.h"

/////////////////////////////////////////////////////////////////////////////
// CGeomecApp:
// See Geomec.cpp for the implementation of this class
//

typedef CStorageNode::TSTREAM TSTREAM;
typedef CStorageNode::TPROGRESS TPROGRESS;

class CKeyFile;
class QApplication;
class ISettings;
class IPlatform;

class ChangelogDlg;

//
//
//
namespace gm{

namespace settings_{
	void init();
} //namespace settings

namespace global{
//
// read config file (geomec.ini) and assign values to CGlobals vars
//
void read();
} //namespace global

} //namespace gm
//
//
//

//##ModelId=3BC55D63015D
class CGeomecApp : public CTnoWinApp
{
	QApplication *m_pQapp;
public:
	//##ModelId=3BE7AEC3039F
	CGeomecDoc& GetDoc() const;
	//##ModelId=3BC55D63016E
	CGeomecApp();
	virtual ~CGeomecApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeomecApp)
public:
	//##ModelId=3BC55D63016F
	virtual BOOL InitInstance();
  virtual int ExitInstance();
	//}}AFX_VIRTUAL
	BOOL OnCmdMsg(unsigned int nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

// Implementation
	//{{AFX_MSG(CGeomecApp)
	//##ModelId=3BC55D630171
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CTempPath* TempPath();
	const CKeyFile &KeyFile() const;

private:
	CTempPath* m_TempPath;
	CKeyFile *m_pKeyFile;

	void ShowTipAtStartup(void);
	void ShowTipOfTheDay(void);
  BOOL ProcessShellOpenCommand(CCommandLineInfo& cmdInfo);
};

// Global function to retrieve a pointer to the global WinApp derived object
CGeomecApp *GetGeomecApp();


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOMEC_H__9C2309E0_4FE5_47F3_9FD2_B2B86AE4E8F3__INCLUDED_)
