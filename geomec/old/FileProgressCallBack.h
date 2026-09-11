// FileProgressCallBack.h: interface for the CFileProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPROGRESSCALLBACK_H__9E4D3E03_A87F_44B6_8ADC_95714C372C6D__INCLUDED_)
#define AFX_FILEPROGRESSCALLBACK_H__9E4D3E03_A87F_44B6_8ADC_95714C372C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "progressdlg.h"

class CFileProgressCallBack : public CProgressCallBack  
{
  CProgressDlg *m_pProgressDlg;
public:
  CFileProgressCallBack(CProgressDlg &dlg);
  virtual ~CFileProgressCallBack();
protected:
  virtual void OnOpenFile(const CTextFile &file);
  virtual void OnSaveFile(const CTextFile &file);
  virtual void OnProgressUpdate(const int nProgress);
};

#endif // !defined(AFX_FILEPROGRESSCALLBACK_H__9E4D3E03_A87F_44B6_8ADC_95714C372C6D__INCLUDED_)
