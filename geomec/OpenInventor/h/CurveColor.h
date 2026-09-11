#ifndef _CurveColor_h_
#define _CurveColor_h_

class Curve;

class CCurveColor
{
  public:
  CCurveColor(Curve* curve, int colorIndex);

  void setColor(QColor curveColor, QColor markerColor);
  const QColor& getCurveColor() const;
  const QColor& getMarkerColor() const;
  int getColorIndex() const;

  private:
  CCurveColor(const CCurveColor& rhs);
  CCurveColor(CCurveColor&& rhs);
  CCurveColor& operator = (CCurveColor rhs);
  CCurveColor& operator = (CCurveColor&& rhs);

  static int setColor(Curve* curve, int colorIndex, QColor curveColor,
      QColor markerColor);

  Curve* m_curve;
  QColor m_curveColor;
  QColor m_markerColor;
  int m_colorIndex;
};

#endif  // _CurveColor_h_
