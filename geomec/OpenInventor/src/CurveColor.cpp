#include "stdafx.h"

#include "CurveColor.h"
#include "ColorManager.h"
#include "Curve.h"

CCurveColor::CCurveColor(Curve* curve, int colorIndex)
: m_curve(curve)
, m_curveColor(ColorManager::MakeCurveColor(colorIndex))
, m_markerColor(ColorManager::MakeMarkerColor(colorIndex))
, m_colorIndex(setColor(curve, colorIndex, m_curveColor, m_markerColor))
{
}

void CCurveColor::setColor(QColor curveColor, QColor markerColor)
{
  m_curveColor = curveColor;
  m_markerColor = markerColor;

  setColor(m_curve, m_colorIndex, curveColor, markerColor);
}

const QColor& CCurveColor::getCurveColor() const
{
  return m_curveColor;
}

const QColor& CCurveColor::getMarkerColor() const
{
  return m_markerColor;
}

int CCurveColor::getColorIndex() const
{
  return m_colorIndex;
}

// private

// static

int CCurveColor::setColor(Curve* curve, int colorIndex, QColor curveColor,
  QColor markerColor)
{
  SbString curveColorStr;

  curveColorStr.sprintf("diffuseColor [%f %f %f]",
  curveColor.redF(), curveColor.greenF(), curveColor.blueF());
  curve->set("curvePointApp.material", curveColorStr);

  SbString markerColorStr;

  markerColorStr.sprintf("diffuseColor [%f %f %f]",
  markerColor.redF(), markerColor.greenF(), markerColor.blueF());
  curve->set("markerApp.material", markerColorStr);

  return colorIndex;
}
