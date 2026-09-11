/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IOpenGLFrame.h: interface for the IOpenGLFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOPENGLFRAME_H__A2D18C1F_0209_4B93_9EF5_41ADD2B7230B__INCLUDED_)
#define AFX_IOPENGLFRAME_H__A2D18C1F_0209_4B93_9EF5_41ADD2B7230B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <qgl.h>
#include <stack>
#include <vector>
// Some qgl.h seem's to define a max, we don't want that!
#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

#include "IObject.h"
#include "MouseListener.h"

#include "OpenGLFrameExports.h"

class COpenGLDraw;
class IDrawDef;
class OPENGLFRAME_EXPORT IOpenGLFrame {
public:
  typedef std::vector<const geo::IObject *> TObjectVec;

private:
  friend class CMouseListener;
  IOpenGLFrame *m_pParent;
  std::stack<CMouseListener *> m_skMouseListener;
  std::vector<IOpenGLFrame *> m_vcChild;
  COpenGLDraw *m_ptDraw;
  int *m_ViewPort;
  bool m_bLightingEnabled;
  int m_updateCount; // ignore updates occuring during an update

  GLuint m_CurrenNameID;
  // maps containing selectable objects used for LookUp
  typedef std::map<const geo::IObject *, GLuint> TSelectionMap;
  typedef std::map<GLuint, const geo::IObject *> TSelectionMapRev;
  mutable TSelectionMap m_SelectionMap;
  mutable TSelectionMapRev m_SelectionMapRev;

  // cash the opengl matices for speed
  // used in Unproject(..)
  mutable double m_ModelMatrix[16];
  mutable double m_ProjectionMatrix[16];
  mutable double m_PickMatrix[16];
  mutable int m_ViewPortMatrix[4];

  const geo::IObject *GetSelectableObject(GLuint selectionID) const;
  // return a selection id for this object used for lookup later in ProcessSelection
  // called by COpenGLDraw()
  // no need for clients to call this
  GLuint GetSelectionID(const geo::IObject &object) const;
  TObjectVec ProcessHits(GLint nHitSize, GLuint *pSelectBuff);
  void OnUpdateFrame();

protected:
  // Overrides
  virtual void OnAttachChild(IOpenGLFrame &child);
  virtual void OnDetachChild(IOpenGLFrame &child);
  virtual void SetupProjectionMatrix() const;
  virtual void SetupViewPortMatrix() const;
  virtual void SetupModelMatrix() const;
  virtual void FlipBuffers();
  virtual bool DeviceContext() const;
  virtual int OnCountDrawSteps() const;
  virtual void OnBeginDraw(int nSteps);
  virtual void OnDrawStep(int nSteps);
  virtual void OnEndDraw();

public:
  IOpenGLFrame();
  virtual ~IOpenGLFrame();

  // Parent / Child
  IOpenGLFrame *Parent();
  const IOpenGLFrame *Parent() const;
  virtual bool ReParent(IOpenGLFrame *pParent);
  const IOpenGLFrame &Root() const;
  IOpenGLFrame &Root();

  int ChildSize() const;
  const IOpenGLFrame &Child(int nIndex) const;
  IOpenGLFrame &Child(int nIndex);

  // View port function
  typedef QPoint TScreenPoint;
  bool HitTest(const TScreenPoint &point) const;
  virtual TScreenPoint GlobalScreenPoint(const TScreenPoint &point) const;
  TScreenPoint RootToChild(const TScreenPoint &root_point) const;
  TScreenPoint ChildToRoot(const TScreenPoint &child_point) const;
  virtual int ViewPortX() const;
  virtual int ViewPortY() const;
  virtual int ViewPortWidth() const;
  virtual int ViewPortHeight() const;
  bool AlignToParent() const;
  void RestoreAlignToParent();
  virtual void SetViewPort(int lX, int lY, int lWidth, int lHeight);

  virtual void OnResizeParent();
  virtual void OnResizeChild(IOpenGLFrame &child);

  // Device context and draw functions
  virtual bool MakeCurrent() const;
  void ClearFrame();
  void UpdateFrame();
  typedef Qt::CursorShape TCursor;
  virtual void SetCursor(TCursor cursor);
  virtual void DrawScene();
  virtual long CountScene();
  virtual void DrawObject(const geo::IObject &object, const IDrawDef &DrawDef, bool bDrawSteps = false);
  virtual long CountObject(const geo::IObject &object, const IDrawDef &DrawDef) const;
  virtual void DrawList(int nList, const IDrawDef &DrawDef);
  virtual int CompileList(const geo::IObject &object, const IDrawDef &DrawDef, bool bDrawSteps = false);
  virtual void DeleteList(int nList);
  virtual void FlushDisplayList();
  virtual void RenderText(const double &dX, const double &dY, const double &dZ, const QString &text,
                          const QFont &fnt = QFont());
  // Step function to perform one or more steps
  void DrawStep(int nSteps = 0) const;

  // Do not call this function direct, this is called by the framework. (see COpenGLDraw).
  // Use IDrawDef::IsSelectable(..) to allow selection of objects
  virtual bool RegisterSelectableObject(const geo::IObject &object);

  int GLRenderMode() const;

  // Mouse messages return true when handled.
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  virtual bool MousePress(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseDblClk(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers modifiers, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint &point);
  typedef Qt::Key TKey;
  virtual bool KeyPress(TKeyboardModifiers modifiers, int nAscii, TKey key, int nRepCount);
  virtual bool KeyRelease(TKeyboardModifiers modifiers, int nAscii, TKey key, int nRepCount);

  // contains the screen point and 'screen_depth'
  typedef std::pair<TScreenPoint, double> TScreenPointDepth;

  // convert screen ccordinate to world coordinate
  geo::CPoint UnProject(const TScreenPoint &screen_point, const double &dDepth = 0.0) const;
  geo::CPoint UnProject(const IOpenGLFrame::TScreenPointDepth &screen_point) const;

  // convert world ccordinate to screen coordinate
  TScreenPointDepth Project(const geo::IPoint &world_point) const;

  // returns the selected objects for the given screen-point
  TObjectVec ProcessSelection(const TScreenPoint &point);

  void EnableLighting(bool lighting);
  bool LightingEnabled() const { return m_bLightingEnabled; }
  typedef QRgb TColor;
  virtual TColor BackgroundColor() const;
  virtual TColor BackgroundColor2() const;
  virtual TColor TextColor() const;

  enum DIMENSION { DIM_2D = 2, DIM_3D = 3 };
  virtual DIMENSION Dimension() const = 0;

  // return the top-listener on the stack.
  // only the toplistener can handle mouse events
  CMouseListener *MouseListener();
  const CMouseListener *MouseListener() const;

  void CancelAllMouseListeners();
  void ValidateAllMouseListeners();
};

#endif // !defined(AFX_IOPENGLFRAME_H__A2D18C1F_0209_4B93_9EF5_41ADD2B7230B__INCLUDED_)
