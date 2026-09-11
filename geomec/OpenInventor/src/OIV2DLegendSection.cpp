#include "stdafx.h"

#include "Inventor\nodes\SoMarkerSet.h"
#include "Inventor\nodes\SoMaterial.h"
#include "Inventor\nodes\SoMaterialBinding.h"
#include "Inventor\nodes\SoText2.h"
#include "Inventor\nodes\SoTranslation.h"
#include "OIV2DLegend.h"
#include "OIV2DLegendSection.h"

OIV2DLegendSection::OIV2DLegendSection() : m_textSpacing(1.5f) {
  m_translation = new SoTranslation;
  m_translation->translation = SbVec3f(0.0f, 0.0f, 0.0f);
  addChild(m_translation);

  m_leadersText = new SoText2();
  m_leadersText->setName("LeadersText");
  m_leadersText->justification = SoText2::LEFT;
  m_leadersText->spacing = m_textSpacing;
  addChild(m_leadersText);

  m_colorItemsTranslation = new SoTranslation();
  m_colorItemsTranslation->setName("colorItemsTranslation");
  m_colorItemsTranslation->translation = SbVec3f(50, 0, 0);
  addChild(m_colorItemsTranslation);

  SoSeparator *colorTextSeparator = new SoSeparator;
  addChild(colorTextSeparator);

  SoTranslation *colorTextTranslation = new SoTranslation;
  colorTextTranslation->translation = SbVec3f(50, 0, 0);
  colorTextSeparator->addChild(colorTextTranslation);

  m_colorItemsText = new SoText2();
  m_colorItemsText->setName("ColorItemsText");
  m_colorItemsText->justification = SoText2::LEFT;
  m_colorItemsText->spacing = m_textSpacing;

  colorTextSeparator->addChild(m_colorItemsText);

  SoSeparator *colorMarkersSeparator = new SoSeparator;
  addChild(colorMarkersSeparator);

  m_colorMarkersMaterialBinding = new SoMaterialBinding;
  m_colorMarkersMaterialBinding->value = SoMaterialBinding::PER_VERTEX;

  colorMarkersSeparator->addChild(m_colorMarkersMaterialBinding);

  m_colorMarkersMaterial = new SoMaterial;
  m_colorMarkersMaterial->transparency.setValue(0.0f);
  colorMarkersSeparator->addChild(m_colorMarkersMaterial);

  m_colorMarkersSet = new SoMarkerSet;

#if SO_INVENTOR_VERSION >= 9300
  m_colorMarkersSet->markerGlobalScale = 1.0f;
  m_colorMarkersSet->markerScale = 1.0f;
#endif

  m_colorMarkersSet->markerIndex.setValue(OIV2DMARKERINDEX);
  m_colorMarkersCoordinates = new SoVertexProperty;
  m_colorMarkersSet->vertexProperty.setValue(m_colorMarkersCoordinates);
  colorMarkersSeparator->addChild(m_colorMarkersSet);
}

void OIV2DLegendSection::clear() {
  m_leadersText->string.deleteValues(0, -1);
  m_colorItemsText->string.deleteValues(0, -1);
  m_colorMarkersCoordinates->vertex.deleteValues(0, -1);
  m_colorMarkersMaterial->diffuseColor.deleteValues(0, -1);
  m_colorMarkersMaterial->transparency.deleteValues(0, -1);
  m_colorMarkersSet->markerIndex.deleteValues(0, -1);
#if SO_INVENTOR_VERSION >= 9300
  m_colorMarkersSet->markerScale.deleteValues(0, -1);
#endif

  int num = m_colorItemsText->string.getNum();
  assert(num == 0);
}

void OIV2DLegendSection::addItem(const std::string &name, float red, float green, float blue) {
  m_colorMarkersMaterialBinding->value = SoMaterialBinding::PER_VERTEX;

  int num = m_colorItemsText->string.getNum();
  m_colorItemsText->string.set1Value(num, name);
  m_colorMarkersCoordinates->vertex.set1Value(num, 14.0f, 4.0f - num * 18.0f, 0.0f);
  m_colorMarkersMaterial->diffuseColor.set1Value(num, red, green, blue);
  m_colorMarkersMaterial->transparency.set1Value(num, 0.0);
  m_colorMarkersSet->markerIndex.set1Value(num, OIV2DMARKERINDEX);
#if SO_INVENTOR_VERSION >= 9300
  m_colorMarkersSet->markerScale.set1Value(num, 1.0f);
#endif

  num = m_colorItemsText->string.getNum();
}

void OIV2DLegendSection::addLeader(const std::string &leader) {
  int num = m_leadersText->string.getNum();
  m_leadersText->string.set1Value(num, leader);
  m_colorItemsTranslation->translation = SbVec3f(0, -(num + 1) * 18.0f, 0.0f);

  num = m_leadersText->string.getNum();
}

int OIV2DLegendSection::getNumLines() { return m_leadersText->string.getNum() + m_colorItemsText->string.getNum(); }

void OIV2DLegendSection::setOffset(int numberOfLines) {
  m_translation->translation = SbVec3f(0.0f, -numberOfLines * 18.0f, 0.0f);
}
