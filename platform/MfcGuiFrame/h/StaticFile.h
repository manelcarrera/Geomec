// StaticFile.h: interface for the CStaticFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATICFILE_H__B116BD67_F23D_45DB_A5C9_23D07544A7BA__INCLUDED_)
#define AFX_STATICFILE_H__B116BD67_F23D_45DB_A5C9_23D07544A7BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStaticFile : public CStatic  
{
  BOOL    m_bPathEllipsis;    // flag: draw text as path
  DWORD   m_dwFormat;         // text format
public:
  CStaticFile(DWORD dwFormat = DT_LEFT | DT_NOPREFIX | DT_VCENTER,
          BOOL bPathEllipsis = FALSE);
  BOOL PathEllipsis() const;
  void PathEllipsis(const BOOL bEnable);
  DWORD TextFormat() const;
  void TextFormat(const DWORD dwFormat); 


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CStaticFilespec)
  //}}AFX_VIRTUAL
  // Generated message map functions
protected:
  //{{AFX_MSG(CStaticFilespec)
  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_STATICFILE_H__B116BD67_F23D_45DB_A5C9_23D07544A7BA__INCLUDED_)
