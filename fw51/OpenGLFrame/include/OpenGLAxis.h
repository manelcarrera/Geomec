 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// OpenGLAxis.h: interface for the COpenGLAxis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLAXIS_H__AC19389E_C530_43A0_A8EC_F726FE27A515__INCLUDED_)
#define AFX_OPENGLAXIS_H__AC19389E_C530_43A0_A8EC_F726FE27A515__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLScene.h"
#include <qcolor.h>

#include "OpenGLFrameExports.h"
namespace geo{
  class CSphere;
}

class COpenGLArrow;
class CLabelPoint;
class OPENGLFRAME_EXPORT COpenGLAxis : public COpenGLScene
{
  //axis labels
  CLabelPoint* m_Label[3];
  COpenGLArrow* m_Arrow[3];
  geo::CSphere* m_pSphere;
  bool m_Visible;
  bool m_AllowRotateParent;

  COpenGLArrow* m_LockAxis;

  int Index(const geo::IVector& axis) const;
public:
  COpenGLAxis(TColor color = qRgb(255, 0, 0),
        const QString& sX = "X", 
        const QString& sY = "Y", 
        const QString& sZ = "Z");
  virtual ~COpenGLAxis();
  virtual void	DrawScene();

  // set text for axis labels
  void AxisText(const geo::IVector& axis, const QString& text); 
  const QString& AxisText(const geo::IVector& axis);

  void AxisColor(const geo::IVector& axis, TColor color);
  TColor AxisColor(const geo::IVector& axis) const;

  virtual TColor BackgroundColor() const;
  virtual void SetupModelMatrix() const;

  bool Visible() const;
  void Visible(bool);

  void AllowRotateParent(bool);
  bool AllowRotateParent() const ;


  virtual void ViewVector(const geo::IVector &vec);
  virtual void UpVector(const geo::CVector &vec);

  using COpenGLScene::BackgroundColor;
  using COpenGLScene::ViewVector;
  using COpenGLScene::UpVector;
};

#endif // !defined(AFX_OPENGLAXIS_H__AC19389E_C530_43A0_A8EC_F726FE27A515__INCLUDED_)
