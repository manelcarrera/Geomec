#include "stdafx.h"

#include "Curve.h"
#include "ColorManager.h"
#include "vcbfunctions.h"

Curve::Curve(const IValueComponentBase* valueComponent, CUnitNode::TUnitType unit, int colorIndex)
  : m_valueComponent(valueComponent)
  , m_unit(unit)
  , m_curveColor(this, colorIndex)
{
  setLabel (getFullResultPath()); 
  setValueComponentLabel(getFullResultPath());
}

QString Curve::getResultPath(int exceptFirst) const
{
  return vcbfunctions::getResultPath(*m_valueComponent, exceptFirst);
}

QString Curve::getFullResultPath(int exceptFirst) const
{
  return QString("%1 [%2]")
    .arg(getResultPath(exceptFirst))
    .arg(getUnitName());
}

QString Curve::getUnitName() const
{
  return m_valueComponent->UnitName(m_unit);
}

const IValueComponentBase* Curve::valueComponent() const
{
  return m_valueComponent;
}

void Curve::setColor(QColor curveColor, QColor markerColor)
{
  m_curveColor.setColor(curveColor, markerColor);
}

const QColor & Curve::getCurveColor() const
{
  return m_curveColor.getCurveColor();
}


int Curve::getNumPoints()
{
  return point.getNum();
}

const SbVec2f & Curve::getPoint(int n)
{
  return * point.getValues(n);
}


const QColor & Curve::getMarkerColor() const
{
  return m_curveColor.getMarkerColor();
}

void Curve::setMarkerSize(float size)
{
  SbString sizeStr;
  sizeStr.sprintf("pointSize %f", size);
  set("markerApp.drawStyle", sizeStr);
}

void Curve::setLineWidth(float width)
{
  SbString sizeStr;
  sizeStr.sprintf("lineWidth %f", width);
  set("curvePointApp.drawStyle", sizeStr);
}

QString Curve::getLabel() const
{
  return m_label;
}

void Curve::setLabel( const QString & label ) const
{
  m_label = label;
}

QString Curve::getValueComponentLabel() const
{
  return m_valueComponentLabel;
}

void Curve::setValueComponentLabel(const QString& valueComponentLabel)
{
  m_valueComponentLabel = valueComponentLabel;
}

CUnitNode::TUnitType Curve::unit() const
{
  return m_unit;
}

void Curve::setUnit(CUnitNode::TUnitType unit)
{
  if(unit != m_unit)
  {
    m_unit = unit;
    onUnitChanged();
  }
}

void Curve::getMinMax(float& minVal, float& maxVal, int index) const
{
  assert (index == 0 /* x */ || index == 1 /* y */);
  int n = point.getNum();
  if(n == 0)
  {
    minVal = 0.0f;
    maxVal = 0.0f;
  }
  else
  {
    minVal = point[0][index];
    maxVal = minVal;

    for(int i=1; i < n; ++i)
    { 
      float val = point[i][index];

      minVal = std::min(val, minVal);
      maxVal = std::max(val, maxVal);
    }
  }
}

void Curve::getMinMaxBound(float minX, float maxX, float& minVal, float& maxVal) const
{
  int n = point.getNum();

  minVal = FLT_MAX;
  maxVal = -FLT_MAX;

  for (int i = 1; i < n; ++i)
  {
  float x = -point[i][1];
  if (x >= minX && x <= maxX)
  {
      float val = point[i][0];

      minVal = std::min(val, minVal);
      maxVal = std::max(val, maxVal);
  }
  }
}


void Curve::refresh()
{
}

int Curve::getColorIndex() const
{
  return m_curveColor.getColorIndex();
}

void Curve::setLinePattern( int linePattern )
{
  SbString sizeStr;
  sizeStr.sprintf("linePattern %#.4x", linePattern);
  set("curvePointApp.drawStyle", sizeStr);
}
