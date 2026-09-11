 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MouseListener.h: interface for the CMouseListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSELISTENER_H__597664CD_90FF_46B6_8974_594FF81D462F__INCLUDED_)
#define AFX_MOUSELISTENER_H__597664CD_90FF_46B6_8974_594FF81D462F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Polygon.h"
#include <qevent.h>

#include "OpenGLFrameExports.h"

// Every mouse listener can have one or more constraints. The class CConstraint is the base class for these constraints
class OPENGLFRAME_EXPORT CConstraint
{
public:
  // Validate must return true when the point and/or the line is accepted by the constraint
  // return false when the point and/or line is rejected by the constraint
  virtual bool ValidatePoint(const geo::IPoint &/*pt*/) const { return true;}
  virtual bool ValidateLine(const geo::ILine &/*line*/) const { return true;}
  virtual ~CConstraint(){}
};

// The crossingconstraint validates whether or not the given line crosses the constraining polygon
class OPENGLFRAME_EXPORT CCrossingConstraint : public CConstraint
{
protected:
  geo::CPolygon m_constraint;
public:
  CCrossingConstraint(const geo::CPolygon &crossing) : m_constraint(crossing){}
  virtual bool ValidateLine(const geo::ILine &line) const;
};

// The outerconstraint validates whether a point is outside a given polygon
class OPENGLFRAME_EXPORT COuterConstraint : public CCrossingConstraint
{
public:
  COuterConstraint(const geo::CPolygon &outer) : CCrossingConstraint(outer){}
  virtual bool ValidatePoint(const geo::IPoint &pt) const;
};

// The innerconstraint validates whether a point is inside a given polygon
class OPENGLFRAME_EXPORT CInnerConstraint : public CCrossingConstraint
{
public:
  CInnerConstraint(const geo::CPolygon &inner) : CCrossingConstraint(inner){}
  virtual bool ValidatePoint(const geo::IPoint &pt) const;
};

class IOpenGLFrame;
class OPENGLFRAME_EXPORT CMouseListener  
{
public:
  typedef IOpenGLFrame TFrame;
private:
  std::vector<CConstraint*> m_vcConstraints;
  TFrame& m_frame;
  bool m_bDeleting;
protected:
  // Construction
  CMouseListener(TFrame &frame);

  // Validation of the mouse listener
  virtual bool ValidatePoint(const geo::IPoint& point) const;
  virtual bool ValidateLine(const geo::ILine &line) const;
  int ConstraintSize() const;
  const CConstraint& Constraint(int nIndex) const;
  virtual void OnSetCursor(const geo::IPoint& point, bool bValidLocation);
  virtual geo::CPoint OnSetDragPoint(const geo::IPoint& ptDrag);

public:
  virtual ~CMouseListener();

  // The new mouse events base on qt.
  typedef QPoint TScreenPoint;
  typedef Qt::MouseButton TMouseButton;
  typedef Qt::KeyboardModifiers TKeyboardModifiers;
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  typedef Qt::Key TKey;
  virtual bool KeyPress(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount);
  virtual bool KeyRelease(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount);

  // Destruction // 
  void Cancel();
  void OK();
  void Delete();

  // Overrides
  virtual bool OnCancel();
  virtual bool OnOK();
  virtual bool OnDelete();

  // Constraints added to mouselistener are automaticly deleted ...
  void PushBack(CConstraint* pConstaint);

  virtual void DrawScene(){}

  // Frame of the current mouse listener
  const TFrame& Frame() const;
  TFrame& Frame();
};

#endif // !defined(AFX_MOUSELISTENER_H__597664CD_90FF_46B6_8974_594FF81D462F__INCLUDED_)
