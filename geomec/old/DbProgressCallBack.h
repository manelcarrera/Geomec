// DbProgressCallBack.h: interface for the CDbProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBPROGRESSCALLBACK_H__75D73B95_AB02_44F9_A5F4_24EE2F7D0D41__INCLUDED_)
#define AFX_DBPROGRESSCALLBACK_H__75D73B95_AB02_44F9_A5F4_24EE2F7D0D41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProgressDlg.h"

class CDbProgressCallBack  : public gm3::CProgressCallBack
{
	CProgressDlg &m_dlg;
public:
	CDbProgressCallBack(CProgressDlg &dlg);
	virtual ~CDbProgressCallBack();
	virtual void OnNewRecord();
	virtual void OnNewTable(const CString &strTableName);
};

#endif // !defined(AFX_DBPROGRESSCALLBACK_H__75D73B95_AB02_44F9_A5F4_24EE2F7D0D41__INCLUDED_)
