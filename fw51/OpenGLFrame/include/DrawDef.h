/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DrawDef.h: interface for the CDrawDef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWDEF_H__FF9AF00D_C434_482B_96B0_16B23C619826__INCLUDED_)
#define AFX_DRAWDEF_H__FF9AF00D_C434_482B_96B0_16B23C619826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDrawDef.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CDrawDef : public IDrawDef {
  bool m_bPolyFillFront;
  bool m_bPolyFillBack;
  bool m_bVisible;
  bool m_bLineStipple;
  float m_fLineWidth;
  float m_fPointSize;
  bool m_bPolyDrawFront;
  bool m_bPolyDrawBack;
  int m_iDepthFunc;
  bool m_bRequireCompile;
  TColor m_Color;
  void Init();
  TColor m_TextColor;
  long m_lCircleNrOfSegments;

  bool m_LabelBorder;
  TColor m_LabelBorderOutlineColor;
  TColor m_LabelBorderInteriorColor;

  bool m_UseStencilBuffer;
  GLenum m_StencilFuncFunc;
  GLint m_StencilFuncRef;
  GLuint m_StencilFuncMask;
  GLenum m_StencilOpFail;
  GLenum m_StencilOpZFail;
  GLenum m_StencilOpZPass;
  bool m_IsSelectable;
  GLint m_LineStippleFactor;
  GLushort m_LineStipplePattern;

  bool m_curvedDraw;

public:
  virtual std::vector<TColor> Color(const geo::IObject &object) const;
  virtual void TextColor(const TColor color);
  virtual TColor TextColor() const;

  virtual bool CurvedDraw() const { return m_curvedDraw; };
  virtual void CurvedDraw(bool curvedDraw) { m_curvedDraw = curvedDraw; };

  virtual void SetFixedColor(const TColor color);
  virtual TColor GetFixedColor() const;

  virtual bool Visible() const { return m_bVisible; }
  virtual void Visible(bool visible) { m_bVisible = visible; }

  virtual bool PolyFillFront() const { return m_bPolyFillFront; }
  virtual void PolyFillFront(bool fillfront) { m_bPolyFillFront = fillfront; }

  virtual bool PolyFillBack() const { return m_bPolyFillBack; }
  virtual void PolyFillBack(bool fillback) { m_bPolyFillBack = fillback; }

  virtual bool LineStipple() const { return m_bLineStipple; }
  virtual void LineStipple(bool linestipple) { m_bLineStipple = linestipple; }

  virtual float LineWidth() const { return m_fLineWidth; }
  virtual void LineWidth(float linewidth) { m_fLineWidth = linewidth; }

  virtual float PointSize() const { return m_fPointSize; }
  virtual void PointSize(float pointsize) { m_fPointSize = pointsize; }

  virtual bool PolyDrawFront() const { return m_bPolyDrawFront; }
  virtual void PolyDrawFront(bool polydrawfront) { m_bPolyDrawFront = polydrawfront; }

  virtual bool PolyDrawBack() const { return m_bPolyDrawBack; }
  virtual void PolyDrawBack(bool polydrawback) { m_bPolyDrawBack = polydrawback; }

  virtual int DepthFunc() const { return m_iDepthFunc; }
  virtual void DepthFunc(int depthfunc) { m_iDepthFunc = depthfunc; }

  virtual bool RequireCompile() const { return m_bRequireCompile; }
  virtual void RequireCompile(bool NewVal) { m_bRequireCompile = NewVal; }

  virtual long CircleNrOfSegments() const { return m_lCircleNrOfSegments; }
  virtual void CircleNrOfSegments(long newval) { m_lCircleNrOfSegments = newval; }

  virtual bool UseStencilBuffer() const { return m_UseStencilBuffer; }
  virtual GLenum StencilFuncFunc() const { return m_StencilFuncFunc; }
  virtual GLint StencilFuncRef() const { return m_StencilFuncRef; }
  virtual GLuint StencilFuncMask() const { return m_StencilFuncMask; }
  virtual GLenum StencilOpFail() const { return m_StencilOpFail; }
  virtual GLenum StencilOpZFail() const { return m_StencilOpZFail; }
  virtual GLenum StencilOpZPass() const { return m_StencilOpZPass; }

  virtual void SetDepthLayer2D(int LayerNr);

  // for showing a border on a label
  virtual bool LabelBorder() const { return m_LabelBorder; }
  virtual TColor LabelBorderOutlineColor() const { return m_LabelBorderOutlineColor; }
  virtual TColor LabelBorderInteriorColor() const { return m_LabelBorderInteriorColor; }

  void LabelBorder(bool show) { m_LabelBorder = show; }
  void LabelBorderOutlineColor(const TColor &color) { m_LabelBorderOutlineColor = color; }
  void LabelBorderInteriorColor(const TColor &color) { m_LabelBorderInteriorColor = color; }

  virtual bool IsSelectable(const geo::IObject & /*object*/) const { return m_IsSelectable; }
  virtual void IsSelectable(bool IsSelectable) { m_IsSelectable = IsSelectable; }

  virtual GLint LineStippleFactor() const { return m_LineStippleFactor; }
  virtual void LineStippleFactor(GLint val) { m_LineStippleFactor = val; }

  virtual GLushort LineStipplePattern() const { return m_LineStipplePattern; }
  virtual void LineStipplePattern(GLushort val) { m_LineStipplePattern = val; }

  CDrawDef(TColor FixedColor);
  CDrawDef();
  // ##ModelId=3B653BCA03BC
  virtual ~CDrawDef();
};

#endif // !defined(AFX_DRAWDEF_H__FF9AF00D_C434_482B_96B0_16B23C619826__INCLUDED_)
