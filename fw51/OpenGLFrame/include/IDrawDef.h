/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(OPENGL_DRAWDEF_INTERFACE__INCLUDED_)
#define OPENGL_DRAWDEF_INTERFACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IObject.h"
#include <qcolor.h>
#include <qgl.h>
#include <vector>

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT IDrawDef {
protected:
  IDrawDef() {}

public:
  typedef QRgb TColor;
  virtual ~IDrawDef() {}

  virtual std::vector<TColor> Color(const geo::IObject &object) const = 0;
  virtual TColor TextColor() const { return 0; }
  virtual bool Visible() const = 0;
  virtual bool PolyFillFront() const = 0;
  virtual bool PolyFillBack() const = 0;
  virtual bool LineStipple() const = 0;
  virtual GLint LineStippleFactor() const { return 6; }
  virtual GLushort LineStipplePattern() const { return 0x5555; }

  virtual float LineWidth() const = 0;
  virtual float PointSize() const = 0;
  virtual bool PolyDrawFront() const = 0;
  virtual bool PolyDrawBack() const = 0;
  virtual int DepthFunc() const = 0;
  virtual bool RequireCompile() const { return true; }
  virtual long CircleNrOfSegments() const { return 100; }

  // drawing curved or straight
  virtual bool CurvedDraw() const = 0;

  // for showing a border on a label
  virtual bool LabelBorder() const { return false; }
  virtual TColor LabelBorderOutlineColor() const { return 0; }
  virtual TColor LabelBorderInteriorColor() const { return 0; }

  // these are used for stenciling
  virtual bool UseStencilBuffer() const { return false; }
  virtual GLenum StencilFuncFunc() const { return GL_ALWAYS; }
  virtual GLint StencilFuncRef() const { return 1; }
  virtual GLuint StencilFuncMask() const { return 1; }
  virtual GLenum StencilOpFail() const { return GL_REPLACE; }
  virtual GLenum StencilOpZFail() const { return GL_KEEP; }
  virtual GLenum StencilOpZPass() const { return GL_KEEP; }

  // see if objects can be selected
  virtual bool IsSelectable(const geo::IObject & /*object*/) const { return false; }
};

#endif // !defined(OPENGL_DRAWDEF_INTERFACE__INCLUDED_)
