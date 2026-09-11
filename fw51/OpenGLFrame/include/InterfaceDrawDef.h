 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(INTERFACE_DRAW_DEF__INCLUDED_)
#define INTERFACE_DRAW_DEF__INCLUDED_

#include "IDrawDef.h"

namespace geo {
  class IInterfaceElement;
}

class CInterfaceDrawDef  : public IDrawDef
{
  const geo::IInterfaceElement& m_interface;
  const IDrawDef&               m_drawdef;
public:
  CInterfaceDrawDef( const geo::IInterfaceElement& interface_element, const IDrawDef& drawdef );
  virtual std::vector<TColor> Color(const geo::IObject &object) const;
  virtual TColor TextColor() const;
  virtual bool Visible() const;
  virtual bool PolyFillFront() const;
  virtual bool PolyFillBack() const;
  virtual bool LineStipple() const;
  virtual GLint LineStippleFactor() const;
  virtual GLushort LineStipplePattern() const;

  virtual float LineWidth() const;
  virtual float PointSize() const;
  virtual bool PolyDrawFront() const;
  virtual bool PolyDrawBack() const;
  virtual int DepthFunc() const;
  virtual bool RequireCompile() const;
  virtual long CircleNrOfSegments() const;
  virtual bool CurvedDraw() const;

  //for showing a border on a label
  virtual bool LabelBorder() const;
  virtual TColor LabelBorderOutlineColor() const;
  virtual TColor LabelBorderInteriorColor() const;

  //these are used for stenciling
  virtual bool UseStencilBuffer() const;
  virtual GLenum StencilFuncFunc() const;
  virtual GLint StencilFuncRef() const;
  virtual GLuint StencilFuncMask() const;
  virtual GLenum StencilOpFail() const;
  virtual GLenum StencilOpZFail() const;
  virtual GLenum StencilOpZPass() const;

  //see if objects can be selected
  virtual bool IsSelectable(const geo::IObject &object) const;

};

#endif // !defined(INTERFACE_DRAW_DEF__INCLUDED_)
