/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "InterfaceDrawDef.h"
#include "IInterfaceElement.h"

CInterfaceDrawDef::CInterfaceDrawDef(const geo::IInterfaceElement &interface_element, const IDrawDef &drawdef)
    : m_interface(interface_element), m_drawdef(drawdef) {}

std::vector<CInterfaceDrawDef::TColor> CInterfaceDrawDef::Color(const geo::IObject &object) const {
  std::vector<TColor> vcColor = m_drawdef.Color(m_interface);
  assert(vcColor.size() == m_interface.NrOfNodes());
  if (&m_interface.Front() == &object) {
    std::vector<TColor> vcRet(m_interface.Front().NrOfNodes());
    for (size_t i = 0; i < vcRet.size(); i++) {
      vcRet[i] = vcColor[i];
    }
    return vcRet;
  }
  assert(&m_interface.Back() == &object);
  std::vector<TColor> vcRet(m_interface.Back().NrOfNodes());
  for (size_t i = 0; i < vcRet.size(); i++) {
    vcRet[i] = vcColor[m_interface.Front().NrOfNodes() + i];
  }
  return vcRet;
}

CInterfaceDrawDef::TColor CInterfaceDrawDef::TextColor() const { return m_drawdef.TextColor(); }

bool CInterfaceDrawDef::Visible() const { return m_drawdef.Visible(); }

bool CInterfaceDrawDef::PolyFillFront() const { return m_drawdef.PolyFillFront(); }

bool CInterfaceDrawDef::PolyFillBack() const { return m_drawdef.PolyFillBack(); }

bool CInterfaceDrawDef::LineStipple() const { return m_drawdef.LineStipple(); }

GLint CInterfaceDrawDef::LineStippleFactor() const { return m_drawdef.LineStippleFactor(); }

GLushort CInterfaceDrawDef::LineStipplePattern() const { return m_drawdef.LineStipplePattern(); }

float CInterfaceDrawDef::LineWidth() const { return m_drawdef.LineWidth(); }

float CInterfaceDrawDef::PointSize() const { return m_drawdef.PointSize(); }

bool CInterfaceDrawDef::PolyDrawFront() const { return m_drawdef.PolyDrawFront(); }

bool CInterfaceDrawDef::PolyDrawBack() const { return m_drawdef.PolyDrawFront(); }

int CInterfaceDrawDef::DepthFunc() const { return m_drawdef.DepthFunc(); }

bool CInterfaceDrawDef::RequireCompile() const { return m_drawdef.RequireCompile(); }

long CInterfaceDrawDef::CircleNrOfSegments() const { return m_drawdef.CircleNrOfSegments(); }

bool CInterfaceDrawDef::CurvedDraw() const { return m_drawdef.CurvedDraw(); }

bool CInterfaceDrawDef::LabelBorder() const { return m_drawdef.LabelBorder(); }

CInterfaceDrawDef::TColor CInterfaceDrawDef::LabelBorderOutlineColor() const {
  return m_drawdef.LabelBorderOutlineColor();
}

CInterfaceDrawDef::TColor CInterfaceDrawDef::LabelBorderInteriorColor() const {
  return m_drawdef.LabelBorderInteriorColor();
}

bool CInterfaceDrawDef::UseStencilBuffer() const { return m_drawdef.UseStencilBuffer(); }

GLenum CInterfaceDrawDef::StencilFuncFunc() const { return m_drawdef.StencilFuncFunc(); }

GLint CInterfaceDrawDef::StencilFuncRef() const { return m_drawdef.StencilFuncRef(); }

GLuint CInterfaceDrawDef::StencilFuncMask() const { return m_drawdef.StencilFuncMask(); }

GLenum CInterfaceDrawDef::StencilOpFail() const { return m_drawdef.StencilOpFail(); }

GLenum CInterfaceDrawDef::StencilOpZFail() const { return m_drawdef.StencilOpZFail(); }

GLenum CInterfaceDrawDef::StencilOpZPass() const { return m_drawdef.StencilOpZPass(); }

bool CInterfaceDrawDef::IsSelectable(const geo::IObject &object) const { return m_drawdef.IsSelectable(object); }
