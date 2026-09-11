#include "stdafx.h"

#include "CoordinateAxis.h"
#include "OIDIMeshNodeManager.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>

#include <MeshViz/graph/PoGroup3Axis3.h>
#include <MeshViz/graph/PoLinearAxis.h>
#include <MeshViz/nodes/PoMiscTextAttr.h>
#include <Inventor/nodes/SoAnnoText3Property.h>

CoordinateAxis::CoordinateAxis(OIDIMeshNodeManager * meshNodeManager)
{
  setName("CoordinateAxis");
  SoSeparator* coordAxisSeparator = new SoSeparator;

  m_coordAxisMaterial = new SoMaterial;
  coordAxisSeparator->addChild(m_coordAxisMaterial);
  m_coordAxisMaterial->diffuseColor.setValue(1.0f, 1.0f, 0.0f);

  SoScale * scale = meshNodeManager->getScale();
  assert (scale != 0);
  coordAxisSeparator->addChild(scale);

  m_translation = new SoTranslation;
  coordAxisSeparator->addChild(m_translation);

  m_coordAxisGroup = new PoGroup3Axis3;

  m_coordAxisGroup->xTitle = "N";
  m_coordAxisGroup->yTitle = "E";
  m_coordAxisGroup->zTitle = "D";

  PoLinearAxis * xAxis = (SO_GET_PART (m_coordAxisGroup, "xAxis", PoLinearAxis));
  PoLinearAxis * yAxis = (SO_GET_PART (m_coordAxisGroup, "yAxis", PoLinearAxis));
  PoLinearAxis * zAxis = (SO_GET_PART (m_coordAxisGroup, "zAxis", PoLinearAxis));

  xAxis->set("mainGradTextApp.drawStyle", "style FILLED") ;
  yAxis->set("mainGradTextApp.drawStyle", "style FILLED") ;
  zAxis->set("mainGradTextApp.drawStyle", "style FILLED") ;

  xAxis->set("titleApp.drawStyle", "style FILLED") ;
  yAxis->set("titleApp.drawStyle", "style FILLED") ;
  zAxis->set("titleApp.drawStyle", "style FILLED") ;


  m_textProperty = new SoAnnoText3Property;
  

  m_textProperty->renderPrintType = SoAnnoText3Property::RENDER2D_PRINT_RASTER;
  m_textProperty->fontSizeHint = SoAnnoText3Property::ANNOTATION;

  PoMiscTextAttr *MyTextAttr = new PoMiscTextAttr;
#ifdef _WIN32
  MyTextAttr->fontName = "Courier New"  ;
#else
  MyTextAttr->fontName = "Courier"  ;
#endif

  PbNumericDisplayFormat * numFormat = new PbNumericDisplayFormat (PbNumericDisplayFormat::INTEGER_FORMAT);
  xAxis->setFormat (numFormat);
  yAxis->setFormat (numFormat);
  zAxis->setFormat (numFormat);

  coordAxisSeparator->addChild(m_textProperty);
  coordAxisSeparator->addChild(MyTextAttr);
  coordAxisSeparator->addChild(m_coordAxisGroup);
  addChild(coordAxisSeparator);
}

void CoordinateAxis::reset(const SbBox3f & bbox, const SbVec3d& translation)
{
  float sizeX, sizeY, sizeZ;
  float originX, originY, originZ;

  bbox.getOrigin(originX, originY, originZ);
  bbox.getSize(sizeX, sizeY, sizeZ);

  originX += translation[0];
  originY += translation[1];
  originZ += translation[2];

  const float extensionFactor = 0.05f;
  float maximumSize = std::max(sizeX, sizeY);
  maximumSize = std::max(maximumSize, sizeZ);

  originX -= sizeX * extensionFactor;
  originY -= sizeY * extensionFactor;
  originZ -= sizeZ * extensionFactor;

  sizeX *= (1 + 2 * extensionFactor);
  sizeY *= (1 + 2 * extensionFactor);
  sizeZ *= (1 + 2 * extensionFactor);

  PoLinearAxis * axisX = (SO_GET_PART (m_coordAxisGroup, "xAxis", PoLinearAxis));
  PoLinearAxis * axisY = (SO_GET_PART (m_coordAxisGroup, "yAxis", PoLinearAxis));
  PoLinearAxis * axisZ = (SO_GET_PART (m_coordAxisGroup, "zAxis", PoLinearAxis));

  adjustAxis(axisX, sizeX, originX);
  adjustAxis(axisY, sizeY, originY);
  adjustAxis(axisZ, sizeZ, originZ);

  m_translation->translation.setValue(-translation);

  m_coordAxisGroup->start.setValue(SbVec3f (originX, originY, originZ));
  m_coordAxisGroup->end.setValue (SbVec3f (originX + sizeX, originY + sizeY, originZ + sizeZ));

  const float fontSizeScale = 0.02f;
  const float arrowLengthScale = 0.025f;
  const float arrowHeightScale = 0.0167f;
  const float tickLenghtScale = 0.01f;

  float fontSize = maximumSize * fontSizeScale;
  axisX->gradFontSize = fontSize;
  axisY->gradFontSize = fontSize;
  axisZ->gradFontSize = fontSize;

  axisX->titleFontSize = fontSize;
  axisY->titleFontSize = fontSize;
  axisZ->titleFontSize = fontSize;

  float arrowLength = maximumSize * arrowLengthScale;
  axisX->arrowLength = arrowLength;
  axisY->arrowLength = arrowLength;
  axisZ->arrowLength = arrowLength;

  float arrowHeight = maximumSize * arrowHeightScale;
  axisX->arrowHeight = arrowHeight;
  axisY->arrowHeight = arrowHeight;
  axisZ->arrowHeight = arrowHeight;

  float tickMainLength = maximumSize * tickLenghtScale;
  axisX->tickMainLength = tickMainLength;
  axisY->tickMainLength = tickMainLength;
  axisZ->tickMainLength = tickMainLength;
}

void CoordinateAxis::adjustAxis( PoLinearAxis * axis, float &size, float &origin )
{
  axis->step = 0;
  if (size == 0)
  {
    origin -= 10;
    size = 20;
    axis->step = 10;
  }
}

void CoordinateAxis::SetAxisColor( float * rgb )
{
  m_coordAxisMaterial->diffuseColor.setValue(rgb);
}

void CoordinateAxis::GetAxisColor( float * rgb ) const
{

  const SbColor & color = m_coordAxisMaterial->diffuseColor[0];
  color.getValue(rgb[0], rgb[1], rgb[2]);
}

void CoordinateAxis::PointSize( const double & size )
{
  m_textProperty->setMaxViewingFontSize((unsigned int) size);
}

