/* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#if !defined(COLORSCALEDRAWDEF_H)
#define COLORSCALEDRAWDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorScale.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CColorScaleDrawDef : public IDrawDef {
public:
  enum eColorScaleDirection { CS_X_DIRECTION, CS_Y_DIRECTION, CS_Z_DIRECTION };

private:
  CColorScale m_colorscale;
  void Init();

  bool m_bPolyFillFront;
  bool m_bPolyFillBack;
  bool m_bVisible;
  bool m_bLineStipple;
  float m_fLineWidth;
  float m_fPointSize;
  bool m_bPolyDrawFront;
  bool m_bPolyDrawBack;
  int m_iDepthFunc;
  bool m_bCurvedDraw;

  TColor tmpColor;

  eColorScaleDirection m_eDirection;

public:
  virtual bool Visible() const { return m_bVisible; }
  virtual void Visible(bool visible) { m_bVisible = visible; }
  virtual TColor TextColor() const { return qRgb(80, 80, 80); }

  virtual bool PolyFillFront() const { return m_bPolyFillFront; }
  virtual void PolyFillFront(bool fillfront) { m_bPolyFillFront = fillfront; }

  virtual bool PolyFillBack() const { return m_bPolyFillBack; }
  virtual void PolyFillBack(bool polyfillback) { m_bPolyFillBack = polyfillback; }

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

  virtual bool CurvedDraw() const { return m_bCurvedDraw; };
  virtual void CurvedDraw(bool curvedDraw) { m_bCurvedDraw = curvedDraw; };

  CColorScaleDrawDef(const geo::IObject &object);
  CColorScaleDrawDef();
  CColorScaleDrawDef(double val1, double val2, int color1, int color2);

  eColorScaleDirection ColorScaleDirection() { return m_eDirection; }
  void ColorScaleDirection(eColorScaleDirection direction) { m_eDirection = direction; }

  void AddPair(double value, TColor color);
  void Clear();
  virtual std::vector<TColor> Color(const geo::IObject &object) const;
  ~CColorScaleDrawDef();
};

#endif
