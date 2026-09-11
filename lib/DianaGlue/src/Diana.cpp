// Diana.cpp: implementation of the CDiana class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "resource.h"
#include "Diana.h"

#include "idianax.h"

#include "lbcp.h" //diana_info_t

//#include "sharedResource.h"

//#include "Utils.h"

#include <QWidget>

static diana_info_t di_info;

typedef int BOOL;

//#define FALSE false
//#define TRUE true


#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//#define NUM_DIA_MESSAGES 19+4*(NUMSTEPS+1)
#define NUM_DIA_MESSAGES 1

#include "idianax.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiana::CDiana( IDianaX* diana_x )
{
  m_pDianaX = diana_x;
  m_bCalcResult = FALSE;
  m_pParentWnd = NULL;
}

CDiana::~CDiana()
{
  if( m_pDianaX )
  {
    delete m_pDianaX;
    m_pDianaX = NULL;
  }
}

BOOL CDiana::Calculate()
{
  if( !StartDiana() )
  {
    //TRACE0( "Failed to start Diana kernel\n" );
    UnInitializeControl();
    return FALSE;
  }

  UnInitializeControl();
  return TRUE;
}

BOOL CDiana::SetEnv()
{
  if( !InitializeControl() )
  {
    //TRACE0( "Failed to initialize the DianaX control\n" );
    return FALSE;
  }

  if( !SetEnvironment() )
  {
    //TRACE0( "Failed to set environment for Diana\n" );
    UnInitializeControl();
    return FALSE;
  }

  return TRUE;
}


BOOL CDiana::InitializeControl()
{
  //m_pDianaX = diana_x;

  /*m_pDianaX = new CDianaX();
  if( !m_pDianaX ) 
    return FALSE;*/

  assert( m_pParentWnd != NULL ); // must be set for creation of control
  if( !m_pParentWnd ) 
    return FALSE;

  //FIXME: MCR
  //BOOL bResult = m_pDianaX->Create( NULL, WS_CHILD, CRect( 0, 0, 1, 1 ), m_pParentWnd, IDC_DIANAX );

  m_bCalcResult = FALSE;

  //FIXME: MCR
  //if( bResult )
  {
    m_pDianaX->SetEnvironmentVar( "APP_SPC_LOGGING", "YES" );
    m_pDianaX->SetProgressIsLifesign( TRUE );
  }

  //FIXME: MCR
  //return bResult;
  return true;
}

void CDiana::UnInitializeControl()
{
  if( m_pDianaX )
  {
    //FIXME: MCR
    /*if( ::IsWindow( m_pDianaX->GetSafeHwnd() ) )
      m_pDianaX->DestroyWindow();*/

    delete m_pDianaX;
    m_pDianaX = NULL;
  }
}

BOOL CDiana::SetEnvironment()
{
  assert( m_pDianaX ); // call InitializeControl first
  if( !m_pDianaX ) 
    return FALSE;

  // use DIASHARE if available, otherwise DIAPATH/Share
  //CString strDiaShare;
  QString strDiaShare;
  if(m_strDiaShare.isEmpty()) 
    strDiaShare = m_strDiaPath + "\\Share";
  else                        
    strDiaShare = m_strDiaShare;

  if( m_strDiaPath.isEmpty() || 
    m_strTempPath.isEmpty() || 
    m_strDiaExeName.isEmpty() ) 

      return FALSE; // set these properties first

  m_pDianaX->SetEnvironmentVar( "DIASHARE",        strDiaShare);
  m_pDianaX->SetEnvironmentVar( "DIALIB",         m_strDiaPath + "\\Lib" );
  
  m_pDianaX->SetEnvironmentVar( "FFDIR",           "." );

  m_pDianaX->SetEnvironmentVar( "STP0", m_strDiaPath + "\\binseg\\ap\\mc41.exe");
  m_pDianaX->SetEnvironmentVar( "DIAPATH", m_strDiaPath );
  m_pDianaX->SetEnvironmentVar( "DIASLIB", strDiaShare + "\\Lib" );
  m_pDianaX->SetEnvironmentVar( "FF", m_strTempPath + "calib.ff" );
  m_pDianaX->SetEnvironmentVar( "DIAERRPATH", strDiaShare + "\\Src" );
  
  

    const char *argv[1] = { "MatLibX" };
    int argc = 1;
    char **nargv;
    int nargc;

    vDiSetenv("%s=%s", "DIAPATH", m_strDiaPath);
    vDiSetenv("%s=%s", "DIASHARE", strDiaShare);
    vDiSetenv("%s=%s", "DIALIB", m_strDiaPath + "\\Lib");
    vDiSetenv("%s=%s", "DIASLIB", strDiaShare + "\\Lib");

    nargc = DianaArguments( argc, (char **)argv, &di_info, &nargv );

  return TRUE;
}

BOOL CDiana::StartDiana()
{
  assert( m_pDianaX ); // call InitializeControl first
  if( !m_pDianaX ) return FALSE;

  m_pDianaX->SetNumberOfMessages( NUM_DIA_MESSAGES );

  if( m_strDiaPath.isEmpty() || 
    m_strTempPath.isEmpty() || 
    m_strDiaExeName.isEmpty() || 
    m_strDataFile.isEmpty() ) 

      return FALSE;

  short res = m_pDianaX->ExecuteDiana( m_strDiaExeName, m_strTempPath, "", "", "", m_strBaseName );

  m_bCalcResult = m_pDianaX->GetCalculationResult();

  return ( res == 0 );
}

bool CDiana::GetCalculationResult()
{
  return m_bCalcResult;
}
