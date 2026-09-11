/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/*                                         Copyright (c) 2007 TNO DIANA BV */
// OpenGLScene.h: interface for the COpenGLScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLSCENE_H__98ADD4BE_C466_4009_84AC_43DC48386429__INCLUDED_)
#define AFX_OPENGLSCENE_H__98ADD4BE_C466_4009_84AC_43DC48386429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include "Box.h"
#include "DisplayPair.h"
#include "IOpenGLFrame.h"
#include <QObject>
#include <map>

#include "OpenGLFrameExports.h"

OPENGLFRAME_EXPORT void OutputGlError(const char *label);

class COpenGLView;
class COpenGLAxis;
// ##ModelId=3B653BCA031E
class OPENGLFRAME_EXPORT COpenGLScene : public QObject, public IOpenGLFrame {

  Q_OBJECT

protected:
  double m_dPointSize; // Point size of the drawed points ...
  TColor m_BackgroundColor;
  bool m_bDisplayBoundingBox; // Display bounding box or not
  bool m_bDisplayPerspective; //
  bool m_bAllowDynamicRotate;
  bool m_bAllowDynamicZoom;
  bool m_bAllowDynamicPan;
  bool m_bProportional;
  bool m_bUseFixedBoundingBox;
  geo::CBox m_FixedBoundingBox;

  DIMENSION m_dimension;

  geo::CVector m_ScaleVector;
  geo::CVector m_DisplacementVector;
  geo::CVector m_ViewVector;
  geo::CVector m_UpVector;
  geo::CVector m_RotateLockVector;
  geo::CVector m_ModelScaleVector;

  COpenGLAxis *m_pAxis;

  // m_RotateSmoothFactor indicates the sensitivity of rotation.
  // if factor = 1 then the bounding-box-corner follows the mouse pointer
  double m_RotateSmoothFactor;

  // the near and far clipping planes
  mutable double m_dNear;
  mutable double m_dFar;

  virtual int OnCountDrawSteps() const;

  virtual bool DynamicZoomMouseState(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) const;
  virtual bool DynamicPanMouseState(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) const;
  virtual bool DynamicRotateMouseState(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) const;

private:
  mutable bool m_bRecalcMaxMin;
  mutable geo::CPoint m_ptMax;
  mutable geo::CPoint m_ptMin;
  mutable geo::CPoint m_ptMid;

  void CalcMaxMin() const; // Views ...

  // internal flags for rotating,zoom,pan
  bool m_bDuringDynamicRotate;
  bool m_bDuringDynamicZoom;
  bool m_bDuringDynamicPan;

  // internal points for roatating,zooming,pan
  TScreenPoint m_ptSecondSelectedScreenPoint;
  TScreenPoint m_ptFirstSelectedScreenPoint;

  geo::CPoint m_ptSecondSelectedWorldPoint;
  geo::CPoint m_ptFirstSelectedWorldPoint;

  TScreenPoint m_ptFirstZoomPoint;  // Screenpoint!!
  TScreenPoint m_ptSecondZoomPoint; // Screenpoint!!
  TScreenPoint m_ptZoomRefPoint;    // Screenpoint!!

  // Internal rotating,zoom,pan functions
  void DoStartDynamicPan(const TScreenPoint &point);
  void DoDuringDynamicPan(const TScreenPoint &point);
  void DoEndDynamicPan();

  void DoStartDynamicZoom(const TScreenPoint &point);
  void DoDuringDynamicZoom(const TScreenPoint &point);
  void DoEndDynamicZoom();

  void DoStartDynamicRotate(const TScreenPoint &point);
  void DoDuringDynamicRotate(const TScreenPoint &point);
  void DoEndDynamicRotate();

  // Return the intersection of a Line and the modelsphere on the bounding box
  // returns the point nearest by the First point of the line
  geo::CPoint GetSelectedPointOnModelSphere(const geo::CLine &line);
  void DrawBoundingBox();

public:
  typedef std::map<const geo::IObject *, const IDrawDef *> TDisplayMap;
  typedef TDisplayMap::iterator iterator;
  typedef TDisplayMap::const_iterator const_iterator;

private:
  // Mapping of compiled display lists ...
  // if a displaylist is not in the map it need to be recompiled
  // See Compile() and InvalidateDisplayList()
  typedef std::map<const geo::IObject *, GLuint> TDisplayListMap;
  typedef std::map<GLuint, const geo::IObject *> TDisplayListMapRev;
  TDisplayListMap m_mpDisplayList;       // IObject -> ID in opengl
  TDisplayListMapRev m_mpDisplayListRev; // ID in opengl -> IObject
  TDisplayMap m_mpDisplay;               // The display list as it appears to the user
public:
  // Construction
  COpenGLScene(bool bDisplayAxis = true);
  COpenGLScene(const COpenGLScene &rhs);
  virtual ~COpenGLScene();

  COpenGLScene &operator=(const COpenGLScene &rhs);
  bool operator==(const COpenGLScene &rhs) const;

  // Supporting the IOpenGLFrame
  virtual void OnResizeParent();

  virtual void SetupProjectionMatrix() const;
  virtual void SetupModelMatrix() const;

  virtual void DrawScene();

  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);

  virtual bool ReParent(IOpenGLFrame *pParent);
  virtual void FlushDisplayList();

  // get a line in world coordinates perpendicular to the clipping planes at screenpoint
  geo::CLine GetViewingLine(const TScreenPoint &screenpoint);

  // Create a line normal to the viewingplane trhough the given screenpoint
  // and then calls GetSelectedPointOnModelSphere()
  geo::CPoint GetSelectedModelSpherePoint(const TScreenPoint &screenpoint);

  void ResetPan();
  void ResetScale();
  virtual void ResetRotate();
  void AllowDynamicPan(bool newval);
  bool AllowDynamicPan() const;

  void DisplacementVector(const geo::CVector &newval);
  const geo::CVector &DisplacementVector() const;

  bool AllowDynamicZoom() const;
  void AllowDynamicZoom(bool newval);

  const double &PointSize() const;
  void PointSize(const double &dNewSize);

  const geo::CVector &ScaleVector() const;
  void ScaleVector(const geo::CVector &newval);

  const geo::CVector &ModelScaleVector() const;
  void ModelScaleVector(const geo::CVector &newval);

  void DisplayPerspective(bool newval);
  bool DisplayPerspective() const;

  virtual DIMENSION Dimension() const;
  void Dimension(DIMENSION dim);

  bool UseFixedBoundingBox() const { return m_bUseFixedBoundingBox; }
  void UseFixedBoundingBox(bool bUseFixedBB);

  void SetFixedBoundingBox(geo::CBox BoundingBox) { m_FixedBoundingBox = BoundingBox; }
  geo::CBox GetFixedBoundingBox() const { return m_FixedBoundingBox; }

  bool Proportional() const { return m_bProportional; }
  void Proportional(bool bProportional) { m_bProportional = bProportional; }

  TColor AxisColor() const;
  void AxisColor(TColor color);

  virtual TColor BackgroundColor() const;
  virtual void BackgroundColor(TColor color);

  const COpenGLAxis &Axis() const;
  COpenGLAxis &Axis();

  void DisplayAxis(bool);
  bool DisplayAxis() const;

  void DisplayBoundingBox(bool);
  bool DisplayBoundingBox() const;

  bool AllowDynamicRotate() const { return m_bAllowDynamicRotate; }
  void AllowDynamicRotate(const bool newval) { m_bAllowDynamicRotate = newval; }

  virtual void RotateLockVector(const geo::CVector &vec);
  virtual const geo::CVector &RotateLockVector() const;

  virtual void ViewVector(const geo::IVector &vec);
  virtual const geo::CVector &ViewVector() const;

  virtual void UpVector(const geo::CVector &vec);
  virtual const geo::CVector &UpVector() const;

  void InvalidateMaxMin();

  const_iterator begin() const { return m_mpDisplay.begin(); }
  iterator begin() { return m_mpDisplay.begin(); }

  const_iterator end() const { return m_mpDisplay.end(); }
  iterator end() { return m_mpDisplay.end(); }

  bool IsDisplayList(const geo::IObject &object) const;
  bool insert(const geo::IObject &object, const IDrawDef &def);
  bool erase(const geo::IObject &list, bool bUpdate = true);
  void invalidate(const geo::IObject &list, bool bUpdate = true);
  const IDrawDef &DrawDef(const geo::IObject &list) const;
  void DrawDef(const IDrawDef &drawdef, const geo::IObject &list);
  void EraseAllDisplayLists();
  void InvalidateAllDisplayLists(bool bUpdate = true);

  double NearClippingPlane() const { return m_dNear; }
  double FarClippingPlane() const { return m_dFar; }

  void ZoomWindow(const TScreenPoint &ScreenPoint1, const TScreenPoint &ScreenPoint2);

  // Max, min and midpoint
  const geo::IPoint &Max() const;
  const geo::IPoint &Min() const;
  const geo::IPoint &Mid() const;

  int Size() const { return (int)m_mpDisplay.size(); }

  void SceneToClipboard();

public slots:
  void GeometryObjectUpdated(const geo::IObject *pObject, IDrawDef *pDrawDef);
  void GeometryObjectRemoved(const geo::IObject *pObject);

signals:
  void OnDestroy(const COpenGLScene &scene);
};

#endif // !defined(AFX_OPENGLSCENE_H__98ADD4BE_C466_4009_84AC_43DC48386429__INCLUDED_)
