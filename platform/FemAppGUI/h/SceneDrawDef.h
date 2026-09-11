#ifndef _SceneDrawDef_h_
#define _SceneDrawDef_h_

class COpenGLSceneNode;

#include "IDrawDef.h"
#include "openglnode.h"

class CSceneDrawDef : public IDrawDef {
public:
  CSceneDrawDef(COpenGLSceneNode &openGLSceneNode, COpenGLNode::CDrawDef &drawDef);
  virtual ~CSceneDrawDef();

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

  virtual bool LabelBorder() const;
  virtual TColor LabelBorderOutlineColor() const;
  virtual TColor LabelBorderInteriorColor() const;

  virtual bool UseStencilBuffer() const;
  virtual GLenum StencilFuncFunc() const;
  virtual GLint StencilFuncRef() const;
  virtual GLuint StencilFuncMask() const;
  virtual GLenum StencilOpFail() const;
  virtual GLenum StencilOpZFail() const;
  virtual GLenum StencilOpZPass() const;

  virtual bool IsSelectable(const geo::IObject &object) const;

private:
  CSceneDrawDef(const CSceneDrawDef &rhs);
  CSceneDrawDef &operator=(const CSceneDrawDef &rhs);

  COpenGLSceneNode &m_openGLSceneNode;
  COpenGLNode::CDrawDef &m_drawDef;
};

#endif // _SceneDrawDef_h_
