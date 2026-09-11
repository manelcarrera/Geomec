/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_ISYMBOL_H__INCLUDED_)
#define AFX_ISYMBOL_H__INCLUDED_

#include "DrawVisitorBase.h"
#include "VectorTempl.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT ISymbol : public geo::IObject {
public:
  // ISymbol interface
  virtual int NrOfPositions() const = 0;
  virtual const geo::IPoint &Position(int Index) const = 0;
  virtual const geo::IArray &Geometry() const = 0;
  virtual const geo::IPoint &PivotPoint() const = 0;

  virtual bool Visit(geo::IVisitor &visitor) {
    assert(dynamic_cast<CDrawVisitorBase *>(&visitor));
    return static_cast<CDrawVisitorBase &>(visitor).HandleSymbol(*this);
  }
};

#endif // !defined(AFX_ISYMBOL_H__INCLUDED_)
