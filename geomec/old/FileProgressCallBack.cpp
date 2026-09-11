// FileProgressCallBack.cpp: implementation of the CFileProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "FileProgressCallBack.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileProgressCallBack::CFileProgressCallBack(CProgressDlg &dlg)
: m_pProgressDlg(&dlg)
{

}

CFileProgressCallBack::~CFileProgressCallBack()
{

}

void CFileProgressCallBack::OnOpenFile(const CTextFile &file)
{
  m_pProgressDlg->NextJob(CString("Loading ") + CString(file.FileName().c_str())); 
}

void CFileProgressCallBack::OnSaveFile(const CTextFile &file)
{
  m_pProgressDlg->NextJob(CString("Saving ") + CString(file.FileName().c_str())); 
}

void CFileProgressCallBack::OnProgressUpdate(const int nProgress)
{
  if(m_pProgressDlg->OnProgress((WPARAM)(SET_PROGRESS), (LPARAM)(nProgress)) == 0)
    OnCancel();
}
