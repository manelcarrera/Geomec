#if !defined(AFX_MODELVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_)
#define AFX_MODELVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelView view

class CModelBase;

#include "FemAppOpenGLView.h"
#include "OpenInventorSceneNode.h"
#include "MeshMode.h"

class ISceneWrapper;

class CModelView : public CFemAppOpenGLView
{
private:
  BOOL MeshSelectionMode();
  CModelBase* Model();
  void SwitchToTopView();

  COpenGLView* m_pLegend;

  ISceneWrapper* m_pSceneWrapper;

  UINT_PTR m_timerId;

protected:
  //##ModelId=3BC55D610110
  CModelView();           // protected constructor used by dynamic creation

  DECLARE_DYNCREATE(CModelView)

protected:

  virtual void OnDraw(CDC* pDC);  

  virtual void EnterView();
  virtual void LeaveView();

  // Attributes
public:
  // Overrides
  virtual COpenGLView *LegendView() { return m_pLegend; }
  void AttachLegendView( COpenGLView* pView ) { m_pLegend = pView; }
  const ISceneWrapper& SceneWrapper() const;

  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  typedef Qt::Key TKey;
  bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point);
  bool KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  bool KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount);
  void OnTimerCalled();

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CModelView)
protected:
  //##ModelId=3BC55D610111
  virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

  //}}AFX_VIRTUAL

  // Implementation
protected:
  //##ModelId=3BC55D610116
  virtual ~CModelView();
#ifdef _DEBUG
  //##ModelId=3BC55D610120
  virtual void AssertValid() const;
  //##ModelId=3BC55D610122
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CModelView)

  void PerformInvalidatingOIVSceneWrapperFunction (void (ISceneWrapper::*func_ptr)());

  template <class T>
  void PerformInvalidatingOIVSceneWrapperFunction (void (ISceneWrapper::*func_ptr)(T), T value)
  {
    if(m_pSceneWrapper)
    {
      (m_pSceneWrapper->*func_ptr)(value);
      if (m_pSceneWrapper->OpenInventorScene())
      {
        Invalidate();
      }
    }
  }

  afx_msg void OnZoomWindow();
  afx_msg void OnUpdateZoomWindow(CCmdUI* pCmdUI);
  afx_msg void OnInsertPolygonalMeshRegion();
  afx_msg void OnUpdateInsertMeshRegion(CCmdUI* pCmdUI);
  afx_msg void OnInsertRectangularMeshRegion();
  afx_msg void OnInsertXSection();
  afx_msg void OnUpdateInsertXSection(CCmdUI* pCmdUI);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnUpdateCopyPicture(CCmdUI* pCmdUI);
  afx_msg void OnCopyPicture();
  afx_msg void OnSaveGraphics() ;
  //}}AFX_MSG
  afx_msg void OnUpdateViewMenu(CCmdUI* pCmdUI);
  afx_msg void OnViewColor();
  afx_msg void OnViewFill();
  afx_msg void OnViewBeachBalls();
  afx_msg void OnViewOptions();
  afx_msg void OnViewValue();
  afx_msg void OnViewRotate();
  afx_msg void OnViewAxis();
  afx_msg void OnViewCoordinateAxis();
  afx_msg void OnFormationFull();

  void SelectMeshMode( MeshMode mode );

  afx_msg void OnFormationSkin();
  afx_msg void OnFormationCenter();
  afx_msg void OnSelectDraggerTracker();
  afx_msg void OnSelectMouseTracker();
  afx_msg void OnSetDraggerConfined();
  afx_msg void OnSelectHideLegend();
  afx_msg void OnSelectClipPlaneMode();
  afx_msg void OnSelectContourLineMode();
  afx_msg void OnSelectMeshLinesDisplayed();
  afx_msg void OnSelectFormationOutlinesDisplayed();
  afx_msg void OnSelectHandMode();
  afx_msg void OnShowValuesPerElement();
  afx_msg void OnViewBoundary();
  afx_msg void OnViewGlobal();
  afx_msg void OnViewLocal();
  afx_msg void OnViewHotspot();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnZoomReset();
  afx_msg void OnLockX();
  afx_msg void OnLockY();
  afx_msg void OnLockZ();
  afx_msg void OnUnlockAxes();
  afx_msg void OnViewTensorVectorFull();
  afx_msg void OnViewTensorVectorNormal();
  afx_msg void OnViewTensorVectorShear();
  afx_msg void OnBaseScaleRefresh();
  afx_msg void OnViewLighting();
  afx_msg void OnViewNDirection();
  afx_msg void OnViewZDirection();
  afx_msg void OnViewBirdEyeDirection();
  afx_msg void OnViewEDirection();
  afx_msg void OnGoHome();
  afx_msg void OnSaveHome();
  afx_msg void OnViewColorscale();
  afx_msg void OnViewSquare();
  afx_msg void OnViewPerspective();
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

protected:
  afx_msg LRESULT OnScaleVectors(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetVectorVisibleFraction(WPARAM wParam, LPARAM lParam);
public:
  afx_msg void OnUpdateScaleSlider(CCmdUI *pCmdUI);
  afx_msg void OnUpdateDecimateSlider(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_)
