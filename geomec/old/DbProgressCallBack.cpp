// DbProgressCallBack.cpp: implementation of the CDbProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "DbProgressCallBack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbProgressCallBack::CDbProgressCallBack(CProgressDlg &dlg)
: m_dlg(dlg)
{

}

CDbProgressCallBack::~CDbProgressCallBack()
{

}

void CDbProgressCallBack::OnNewRecord()
{
	if (m_dlg.OnProgress(SET_PROGRESS, (LPARAM)((CurrentRecord() * 100) / TotalRecords())) == 0)
		OnCancel();
	//gm3::
	CProgressCallBack::OnNewRecord();
}


void CDbProgressCallBack::OnNewTable(const CString &strTableName)
{	
	m_dlg.JobComment(strTableName);
}