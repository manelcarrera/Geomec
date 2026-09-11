#include "stdafx.h"
#include "cplb.h"
#include "DianaX.h"
#include "ProgressDlg.h"
#include "DianaXCtl.h"
#include <afxtempl.h>

diana_info_t di_info;

extern "C"
{
	int DisplayDianaErrorMessage( char * );
	int UserMessageHandler( char * );
	int ApplicationSpecLogging( char * );
	void DefMsgHandler( char *buf, int size );
	int UserFunc();
}

CDianaXCtrl *GetControl()
{
	return CDianaXCtrl::GetMe();
}

int RunDiana( CString Exe, CString Dat, CString Com, CString Filos, CString Base, BOOL DisplayDefMessages,
			  CString UserLeader, CString UserTrailer )
{
	const char *argv[7];
	int argc = 0;
	char **nargv;
	int nargc;

	if( Exe.IsEmpty() )
	{
		if( !DiGetenv( "STP0" ) ) return -2;
	}

	if( Com.IsEmpty() )
	{
		di_info.commandfile = vDiStrsave( "", "", NULL );
	}

	argv[argc++] = Exe;
	if( DisplayDefMessages ) argv[argc++] = "-m";
	if( !Base.IsEmpty() ) argv[argc++] = Base;
	if( !Dat.IsEmpty() ) argv[argc++] = Dat;
	if( !Com.IsEmpty() ) argv[argc++] = Com;
	if( !Filos.IsEmpty() ) argv[argc++] = Filos;
	argv[argc] = NULL;

	nargc = DianaArguments( argc, (char **)argv, &di_info, &nargv );

	/* Check for Diana installation (baa, 29-12-98) */
	char* env;
	if (!(env = DiGetenv("DIAPATH")))
	{
		return -3;
	}

	InstallDCMessageHandler(
		ASK_DC_HANDLE_ERRMSG,
		ASK_DC_TERMINATE_ERRMSG,
		DisplayDianaErrorMessage );

	InstallDCMessageHandler(
		ASK_DC_APPSPC_MSG,
		NULL,
		UserMessageHandler );

	if( !UserLeader.IsEmpty() && !UserTrailer.IsEmpty() )
	{
		InstallDCMessageHandler(
			UserLeader.GetBuffer( UserLeader.GetLength() ),
			UserTrailer.GetBuffer( UserTrailer.GetLength() ),
			UserMessageHandler );
		UserTrailer.ReleaseBuffer();
		UserLeader.ReleaseBuffer();
	}

	InstallDCDefaultMessageHandler( DefMsgHandler );

	InstallDCUserFunction( UserFunc );

	SetDCReadNoDelay();

	di_info.size = sizeof( diana_info_t );
	di_info.windows = 1;
	int iret = DianaExecute( &di_info );
	if( iret != 0 )
	{
		if( GetControl()->m_pDlg ) GetControl()->DeleteDlg();
		ResetDiInfo( &di_info );
		return -5;
	}

	ResetDiInfo( &di_info );

	if( GetControl()->m_pDlg ) GetControl()->m_pDlg->Finish();

	return 0;
}

static void GetSeparateStrings( CString txt, CArray <CString, CString> &RetVal )
{
	int prev = 0;

	for( int i=0; i<txt.GetLength(); ++i )
	{
		if( txt[i] == '\n' || txt[i] == '\r' )
		{
			if( i == prev )
			{
				++prev;
				continue;
			}
			RetVal.Add( txt.Mid( prev, i - prev ) );
			prev = i + 1;
		}
	}
	int l = txt.GetLength();
	while ( l && ( (txt[l-1] == '\r') || (txt[l-1] == '\n') ) )l--;
	RetVal.Add( txt.Mid( prev, l - prev ) );
}

static int
UserMessageHandler( char *txt )
{
	CProgressDlg *pDlg = GetControl()->m_pDlg;
	CArray <CString, CString> Lines;
	GetSeparateStrings( txt, Lines );
	for( int i=0; i<Lines.GetSize(); ++i )
	{
		if( pDlg ) pDlg->AddMessage( Lines[i] );
		GetControl()->DoFireOnDiaMessage( Lines[i] );
		if( Lines[i].Find( GetControl()->m_strSucceedMessage ) != -1 ) GetControl()->m_bCalculationSucceeded = TRUE;
	}

	return 0;
}

static int
//DEFUN( DisplayDianaErrorMessage, (txt), char *txt )
DisplayDianaErrorMessage( char *txt )
{
	CProgressDlg *pDlg = GetControl()->m_pDlg;
	char *p = strstr( txt, " ERRORMSG." );
	if ( p )
	{
		p += 10;
		if ( p[0] && (p[1] == ':') )
		{
			CArray <CString, CString> Lines;
			int i;
			switch ( p[0] )
			{
			case 'A':
			case 'F':
				if( pDlg ) pDlg->m_bStop = TRUE;
				if( pDlg ) pDlg->DisplayError( txt );
				GetSeparateStrings( txt, Lines );
				for( i=0; i<Lines.GetSize(); ++i )
				{
					GetControl()->DoFireOnDiaError( Lines[i] );
				}
				break;
			case 'W':
				GetSeparateStrings( txt, Lines );
				for( i=0; i<Lines.GetSize(); ++i )
				{
					if( pDlg ) pDlg->AddWarning( Lines[i] );
					GetControl()->DoFireOnDiaWarning( Lines[i] );
				}
				break;
			default:
				if( pDlg ) pDlg->AddMessage( txt );
				GetControl()->DoFireOnDiaMessage( txt );
				break;
			}
		}
	}

	return 0;
}

static int
ApplicationSpecLogging( char *txt )
{
	return 0;
}

static int
UserFunc()
{
	int rv = 0;

	if( GetControl()->m_pDlg )
	{
		rv = GetControl()->m_pDlg->CallBack();
	}

	if( !rv ) GetControl()->DoFireOnCallback( &rv );

	if( rv )
	{
		StopExecutingSegments();
		ResetDiInfo( &di_info );
	}

	return rv;
}


static void
DefMsgHandler( char *buf, int size )
{
	if ( size <= 0 ) return;
	char *p = (char *)malloc( size+1 );
	if ( !p ) return;
	strncpy( p, buf, size );
	p[size] = '\0';
	CString Buf = p;
	free( p );

	CArray <CString, CString> Lines;
	GetSeparateStrings( Buf, Lines );

	for( int i=0; i<Lines.GetSize(); ++i )
	{
		CProgressDlg *pDlg = GetControl()->m_pDlg;
		if( pDlg ) pDlg->AddMessage( Lines[i] );
		GetControl()->DoFireOnDiaMessage( Lines[i] );
		if( Lines[i].Find( GetControl()->m_strSucceedMessage ) != -1 ) GetControl()->m_bCalculationSucceeded = TRUE;
	}
}
