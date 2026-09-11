#if !defined(AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_)
#define AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellEditFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceFrame frame

#include "QuantityInclude.h"
#include "CutSurfaceDlg.h"
#include "NamedSurfaceMap.h"


namespace geo
{
class CBox;
};

class COpenGLView;
class COpenGLScene;
class CGeoObjectHandlerBase;
class CDisplayPair;
class CTreeView;
class CTriaIntersecBox;
class CCutsurfaceTreeView;


class CCutSurfaceFrame : public CFrameWnd
{
  friend class CCutSurfaceDlg;

  DECLARE_DYNCREATE(CCutSurfaceFrame)
protected:

public:
  CCutSurfaceFrame();        

public:
  void  UpdateScene();
  void Init(
        CCutsurfaceTreeView*		listview,
        COpenGLView*	view,
        CCutSurfaceDlg* dlg,
        CDoubleQuantity::UNIT unit);


  //called from EditForm..
  void OnBBChanged(const geo::CBox* pBox);
  void OnCutSurface(const geo::CBox* pBox, int nodesX, int nodesY, int nodesZ, bool bShowMinMaxPoints, const CString& basename="base_");
  void OnNodesNrChanged(const geo::CBox* pBox,int nodesX,int nodesY,int nodesZ);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCutSurfaceFrame)
  //virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CCutSurfaceFrame();

  // Generated message map functions
  //{{AFX_MSG(CCutSurfaceFrame)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  


  COpenGLScene*  CurrentScene();
  void		  On3dvLockDepth();
  void		  OnUpdate3DViewButtons(CCmdUI* pCmdUI);
  void		  On3dvLockEast();
  void		  On3dvLockNorth();
  void		  On3dvLockReset();
  void		  On3dvZoomReset();
  void		  On3dvZoomWindow();
  void		  On3dvZoomin();
  void		  On3dvZoomout();
  void		  On3dvCopypicture();
  void		  On3dvViewAxis();
  void		  On3dvViewBeDirec();
  void		  On3dvViewBoundary();
  void		  On3dvViewEDirec();
  void		  On3dvViewLighting();
  void		  On3dvViewNDirec();
  void		  On3dvViewPerspective();
  void		  On3dvViewZDirec();




private:
  void UpdateTree();
  void  BuildBox(const geo::CBox* pBox);
  std::map<CString,CGeoObjectHandlerBase*> m_handlermap;
  void BuildNodes(const geo::CBox* pBox,int nodesX,int nodesY,int nodesZ);

  CGeoObjectHandlerBase* GetHandler(const char* name);
  CGeoObjectHandlerBase* CreateHandler(const char* name,bool autodelete);
  void RemoveHandlerFromScene(CGeoObjectHandlerBase* handler);
  void ShowHandlerInScene(CGeoObjectHandlerBase* handler);

  void  GetSideSurfaceMap(TNamedSurfMap& surfmap );
  void  GetCutSurfaceMap(TNamedSurfMap& surfmap);

  void GetSurfaceMap(const char* name, TNamedSurfMap& surfmap);

  CCutsurfaceTreeView* m_pTreeView;	
  COpenGLView* m_pView;
  CCutSurfaceDlg* m_pDlg;
  CDoubleQuantity::UNIT m_unit;
  bool m_bShowMinMaxPoints;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_)
