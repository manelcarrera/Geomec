#if !defined(AFX_CUTSURFACEDLG_H__2C82DD42_13E3_449C_940F_6B797B5C1B71__INCLUDED_)
#define AFX_CUTSURFACEDLG_H__2C82DD42_13E3_449C_940F_6B797B5C1B71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutSurfaceDlg.h : header file
//

#include "NamedSurfaceMap.h"
#include "QuantityInclude.h"
#include "TriaIntersecBox.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceDlg dialog

class CCutSurfaceFrame;
class CTriaIntersecBox;
class CEditForm;

namespace geo
{
  class ISurface;
  class IPoint;
};


typedef std::vector <STNamedSurface> CNamedSurfaces;


class DECLSPECIFIER CCutSurfaceDlg : public CDialog
{
// Construction
public:
  

  friend class CCutSurfaceFrame;
  friend class CEditForm;

  CCutSurfaceDlg( CDoubleQuantity::UNIT unit=CDoubleQuantity::SI_UNIT,
          CWnd* pParent = NULL);
  

  CCutSurfaceDlg(	const geo::CBox& box,
          CDoubleQuantity::UNIT unit=CDoubleQuantity::SI_UNIT,
          CWnd* pParent = NULL);   

  void InsertSurface(const geo::ISurface* surf,const std::string& name );

  virtual ~CCutSurfaceDlg();
  const geo::CTriSurface* GetSideSurfaces(SIDE_ID SideID) const;
  const std::map <SIDE_ID, geo::CTriSurface*>& getSideSurfaces() const;

  const std::vector<SIDE_ID> GetSides() const;

  int CutSurfacesCount() const;
  const STNamedSurface& GetCutSurface(int index) const;
  const std::vector <STNamedSurface>& getCutSurfaces() const;

  const CString& BaseName() const;	

  const CNamedSurfaces& OrgSurfaces() const;

// Dialog Data
  //{{AFX_DATA(CCutSurfaceDlg)
  //enum { IDD = IDD_SURFACE_CUT_DLG };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCutSurfaceDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL


  


// Implementation
protected:



  // Generated message map functions
  //{{AFX_MSG(CCutSurfaceDlg)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CTriaIntersecBox*	CreateNewIntersec(const geo::CBox& box);

  CToolBar    m_3DViewToolBar;
  CSplitterWnd m_cSplitter;
  CCutSurfaceFrame* m_pFrameWnd;
  int m_Height;
  CTriaIntersecBox* m_pIntersec;
  CNamedSurfaces* m_OrgSurfaces;

  bool m_allow_edit_box;
  CString m_BaseName;
  CDoubleQuantity::UNIT m_unit;

  virtual void OnOK();
  virtual void OnCancel();

  const geo::CBox* m_pBox;
  const geo::IPoint* m_min;
  const geo::IPoint* m_max;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTSURFACEDLG_H__2C82DD42_13E3_449C_940F_6B797B5C1B71__INCLUDED_)
