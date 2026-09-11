 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(OPENGL_DRAW_INTERFACE__INCLUDED_)
#define OPENGL_DRAW_INTERFACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IObject.h"

#include "OpenGLFrameExports.h"

class IDrawDef;
class OPENGLFRAME_EXPORT IOpenGLDraw
{
protected:
  IOpenGLDraw() {};
public:
  // the number of steps this object represents
  virtual int Count(const geo::IObject&, const IDrawDef&) const = 0;
  // draw the object
  virtual void Draw(const geo::IObject&, const IDrawDef&, bool bDrawSteps) = 0;
};

#endif // !defined(OPENGL_DRAW_INTERFACE__INCLUDED_)
