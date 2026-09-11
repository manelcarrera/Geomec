#ifndef _Curve_h_
#define _Curve_h_

#include "MeshViz\graph\PoCurve.h"
#include "unitnode.h"
#include "CurveColor.h"

class Curve : public PoCurve
{
  const IValueComponentBase* m_valueComponent;
  CUnitNode::TUnitType m_unit;
  mutable QString m_label;
  QString m_valueComponentLabel;
  CCurveColor m_curveColor;

protected:

  virtual void onUnitChanged() = 0;

public:
  Curve(const IValueComponentBase* valueComponent, CUnitNode::TUnitType unit, int colorIndex);

  const IValueComponentBase* valueComponent() const;

  void setColor(QColor curveColor, QColor markerColor);
  const QColor & getCurveColor() const;
  const QColor & getMarkerColor() const;
  int getColorIndex () const;

  void setMarkerSize(float size);

  void setLineWidth(float width);

  QString getResultPath(int exceptFirst = 1) const;
  QString getFullResultPath(int exceptFirst = 1) const;
  QString getUnitName() const;
  QString getLabel() const;
  void setLabel (const QString & label) const;
  QString getValueComponentLabel() const;
  void setValueComponentLabel(const QString& valueComponentLabel);

  CUnitNode::TUnitType unit() const;

  void setUnit(CUnitNode::TUnitType unit);

  virtual void getMinMax(float& minVal, float& maxVal, int index = 0) const;
  virtual void getMinMaxBound(float minX, float maxX, float& minVal, float& maxVal) const;

  virtual QString getTextRepresentation() const = 0;

  virtual void refresh();

  int getNumPoints();
  const SbVec2f & getPoint(int n);
  void setLinePattern( int linePattern );
};

#endif  // _Curve_h_
