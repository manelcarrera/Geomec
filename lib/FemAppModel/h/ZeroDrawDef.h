#ifndef _ZeroDrawDef_h_
#define _ZeroDrawDef_h_

#include "IDrawDef.h"

class CZeroDrawDef : public IDrawDef
{
public:
  typedef QRgb TColor;

  CZeroDrawDef();
  virtual ~CZeroDrawDef();

  virtual std::vector<TColor> Color(const geo::IObject &object) const;
  virtual bool Visible() const;
  virtual bool PolyFillFront() const;
  virtual bool PolyFillBack() const;
  virtual bool LineStipple() const;

  virtual float LineWidth() const;
  virtual float PointSize() const;
  virtual bool PolyDrawFront() const;
  virtual bool PolyDrawBack() const;
  virtual int DepthFunc() const;

  virtual bool CurvedDraw() const;

private:
  CZeroDrawDef(const CZeroDrawDef& rhs);
  CZeroDrawDef& operator = (const CZeroDrawDef& rhs);
};

#endif  // _ZeroDrawDef_h_
