#include "stdafx.h"

#include "SceneDrawDef.h"
#include "openglscenenode.h"

CSceneDrawDef::CSceneDrawDef(COpenGLSceneNode &openGLSceneNode, COpenGLNode::CDrawDef &drawDef)
    : IDrawDef(), m_openGLSceneNode(openGLSceneNode), m_drawDef(drawDef) {}

CSceneDrawDef::~CSceneDrawDef() {}

std::vector<CSceneDrawDef::TColor> CSceneDrawDef::Color(const geo::IObject &object) const {
  COpenGLNode::CDrawDef::DrawDecisionVector drawDecisionVector = m_drawDef.Color(object);

  if (drawDecisionVector.first) {
    return drawDecisionVector.second;
  }

  return m_openGLSceneNode.OnColor(m_drawDef.OpenGLNode(), object);
}

CSceneDrawDef::TColor CSceneDrawDef::TextColor() const {
  COpenGLNode::CDrawDef::DrawDecisionColor drawDecisionColor = m_drawDef.TextColor();

  assert(drawDecisionColor.first);

  return drawDecisionColor.second;
}

bool CSceneDrawDef::Visible() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.Visible();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

bool CSceneDrawDef::PolyFillFront() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.PolyFillFront();

  if (drawDecisionBool.first) {
    return drawDecisionBool.second;
  }

  return m_openGLSceneNode.Fill();
}

bool CSceneDrawDef::PolyFillBack() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.PolyFillBack();

  if (drawDecisionBool.first) {
    return drawDecisionBool.second;
  }

  return m_openGLSceneNode.Fill();
}

bool CSceneDrawDef::LineStipple() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.LineStipple();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

GLint CSceneDrawDef::LineStippleFactor() const {
  COpenGLNode::CDrawDef::DrawDecisionGLint drawDecisionGLint = m_drawDef.LineStippleFactor();

  assert(drawDecisionGLint.first);

  return drawDecisionGLint.second;
}

GLushort CSceneDrawDef::LineStipplePattern() const {
  COpenGLNode::CDrawDef::DrawDecisionGLushort drawDecisionGLushort = m_drawDef.LineStipplePattern();

  assert(drawDecisionGLushort.first);

  return drawDecisionGLushort.second;
}

float CSceneDrawDef::LineWidth() const {
  COpenGLNode::CDrawDef::DrawDecisionFloat drawDecisionFloat = m_drawDef.LineWidth();

  assert(drawDecisionFloat.first);

  return drawDecisionFloat.second;
}

float CSceneDrawDef::PointSize() const {
  COpenGLNode::CDrawDef::DrawDecisionFloat drawDecisionFloat = m_drawDef.PointSize();

  if (drawDecisionFloat.first) {
    return drawDecisionFloat.second;
  }

  return m_openGLSceneNode.PointSize();
}

bool CSceneDrawDef::PolyDrawFront() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.PolyDrawFront();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

bool CSceneDrawDef::PolyDrawBack() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.PolyDrawBack();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

int CSceneDrawDef::DepthFunc() const {
  COpenGLNode::CDrawDef::DrawDecisionInt drawDecisionInt = m_drawDef.DepthFunc();

  assert(drawDecisionInt.first);

  return drawDecisionInt.second;
}

bool CSceneDrawDef::RequireCompile() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.RequireCompile();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

long CSceneDrawDef::CircleNrOfSegments() const {
  COpenGLNode::CDrawDef::DrawDecisionLong drawDecisionLong = m_drawDef.CircleNrOfSegments();

  assert(drawDecisionLong.first);

  return drawDecisionLong.second;
}

bool CSceneDrawDef::CurvedDraw() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.CurvedDraw();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

bool CSceneDrawDef::LabelBorder() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.LabelBorder();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

CSceneDrawDef::TColor CSceneDrawDef::LabelBorderOutlineColor() const {
  COpenGLNode::CDrawDef::DrawDecisionColor drawDecisionColor = m_drawDef.LabelBorderOutlineColor();

  assert(drawDecisionColor.first);

  return drawDecisionColor.second;
}

CSceneDrawDef::TColor CSceneDrawDef::LabelBorderInteriorColor() const {
  COpenGLNode::CDrawDef::DrawDecisionColor drawDecisionColor = m_drawDef.LabelBorderInteriorColor();

  assert(drawDecisionColor.first);

  return drawDecisionColor.second;
}

bool CSceneDrawDef::UseStencilBuffer() const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.UseStencilBuffer();

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}

GLenum CSceneDrawDef::StencilFuncFunc() const {
  COpenGLNode::CDrawDef::DrawDecisionGLenum drawDecisionGLenum = m_drawDef.StencilFuncFunc();

  assert(drawDecisionGLenum.first);

  return drawDecisionGLenum.second;
}

GLint CSceneDrawDef::StencilFuncRef() const {
  COpenGLNode::CDrawDef::DrawDecisionGLint drawDecisionGLint = m_drawDef.StencilFuncRef();

  assert(drawDecisionGLint.first);

  return drawDecisionGLint.second;
}

GLuint CSceneDrawDef::StencilFuncMask() const {
  COpenGLNode::CDrawDef::DrawDecisionGLuint drawDecisionGLuint = m_drawDef.StencilFuncMask();

  assert(drawDecisionGLuint.first);

  return drawDecisionGLuint.second;
}

GLenum CSceneDrawDef::StencilOpFail() const {
  COpenGLNode::CDrawDef::DrawDecisionGLenum drawDecisionGLenum = m_drawDef.StencilOpFail();

  assert(drawDecisionGLenum.first);

  return drawDecisionGLenum.second;
}

GLenum CSceneDrawDef::StencilOpZFail() const {
  COpenGLNode::CDrawDef::DrawDecisionGLenum drawDecisionGLenum = m_drawDef.StencilOpZFail();

  assert(drawDecisionGLenum.first);

  return drawDecisionGLenum.second;
}

GLenum CSceneDrawDef::StencilOpZPass() const {
  COpenGLNode::CDrawDef::DrawDecisionGLenum drawDecisionGLenum = m_drawDef.StencilOpZPass();

  assert(drawDecisionGLenum.first);

  return drawDecisionGLenum.second;
}

bool CSceneDrawDef::IsSelectable(const geo::IObject &object) const {
  COpenGLNode::CDrawDef::DrawDecisionBool drawDecisionBool = m_drawDef.IsSelectable(object);

  assert(drawDecisionBool.first);

  return drawDecisionBool.second;
}
