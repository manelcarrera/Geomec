#include "stdafx.h"

#include "LegendScene.h"
#include "PixelCamera.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/nodes/SoMaterial.h>

#include "Inventor/nodes/SoMarkerSet.h"
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/fields/SoSFVec2f.h>

#include "Inventor/nodes/SoCoordinate3.h"
#include "openglnode.h"
#include <MeshViz/graph/PoLinearValueLegend.h>
#include <MeshVizXLM/mapping/nodes/MoLegend.h>

static unsigned char GeologyMarkerBits[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define GEOLOGYMARKERINDEX 90

LegendScene::LegendScene()
    : m_font(0), m_material(0), m_valuesTextSwitch(0), m_valuesLegendSwitch(0), m_valuesAdornments(0),
      m_valuesAdornmentCount(0), m_valuesLegendOffsetX(10), m_valuesLegendOffsetY(25), m_valuesLegendWidth(20),
      m_valuesLegendHeight(400), m_valuesText(0), m_textSize(12), m_textSpacing(1.5f), m_valuesTextOffset(3),
      m_valuesTextTranslation(0), m_valuesCount(11), m_geologySwitch(0), m_geologyHeadersText(0),
      m_geologyItemsText(0) {
  setName("LegendScene");

  // Font settings and material for 2D annotations.
  m_font = new SoFont();
  m_font->name = "Arial";
  m_font->size = m_textSize;
  m_font->renderStyle = SoFont::TEXTURE;
  m_material = new SoMaterial();
  m_material->diffuseColor = SbColor(1, 1, 1);

  addChild(m_font);
  addChild(m_material);

  m_valuesLegendSwitch = new SoSwitch;
  addChild(m_valuesLegendSwitch);
  m_valuesLegendSwitch->whichChild = SO_SWITCH_NONE;
  m_valuesLegendSwitch->setName("LegendSceneLegendSwitch");

  // Not subject to pixel space camera
  m_valuesLegend = new MoLegend;
  m_valuesLegendSwitch->addChild(m_valuesLegend);

  initLegend();

  addPixelSpaceCamera(this, FALSE, TRUE); // top left corner

  m_valuesTextSwitch = new SoSwitch;
  addChild(m_valuesTextSwitch);
  m_valuesTextSwitch->whichChild = SO_SWITCH_NONE;
  m_valuesTextSwitch->setName("LegendSceneValuesTextSwitch");

  SoTranslation *baseTranslation = new SoTranslation();
  baseTranslation->setName("BaseTranslation");
  baseTranslation->translation = SbVec3f(1, -20, 0);
  m_valuesTextSwitch->addChild(baseTranslation);

  SoSeparator *valuesTextSeparator = new SoSeparator;
  m_valuesTextSwitch->addChild(valuesTextSeparator);

  m_valuesTextTranslation = new SoTranslation();
  m_valuesTextTranslation->setName("ValuesTextTranslation");
  m_valuesTextTranslation->translation = SbVec3f(0, 0, 0);
  valuesTextSeparator->addChild(m_valuesTextTranslation);

  m_valuesText = new SoText2();
  m_valuesText->setName("ValuesText");
  m_valuesText->justification = SoText2::LEFT;

  m_valuesText->spacing = float(m_valuesLegendHeight) / float(m_valuesCount - 1) / float(m_textSize);

  valuesTextSeparator->addChild(m_valuesText);

  m_geologySwitch = new SoSwitch;
  addChild(m_geologySwitch);
  m_geologySwitch->whichChild = SO_SWITCH_ALL;
  m_geologySwitch->setName("LegendSceneGeologyTextSwitch");

  SoResetTransform *pResetBbox = new SoResetTransform();
  pResetBbox->whatToReset = SoResetTransform::BBOX;
  addChild(pResetBbox);

  SoMarkerSet::addMarker(GEOLOGYMARKERINDEX, SbVec2s(16, 16), GeologyMarkerBits);

  m_geologySwitch->addChild(baseTranslation);

  SoSeparator *geologyTextSeparator = new SoSeparator;
  m_geologySwitch->addChild(geologyTextSeparator);

  m_geologyHeadersText = new SoText2();
  m_geologyHeadersText->setName("ValuesText");
  m_geologyHeadersText->justification = SoText2::LEFT;

  m_geologyHeadersText->spacing = m_textSpacing;
  geologyTextSeparator->addChild(m_geologyHeadersText);

  SoTranslation *geologyItemsTranslation = new SoTranslation();
  geologyItemsTranslation->setName("geologyItemsTranslation");
  geologyItemsTranslation->translation = SbVec3f(30, 0, 0);
  geologyTextSeparator->addChild(geologyItemsTranslation);

  m_geologyItemsText = new SoText2();
  m_geologyItemsText->setName("GeologyItemsText");
  m_geologyItemsText->justification = SoText2::LEFT;
  m_geologyItemsText->spacing = m_textSpacing;

  geologyTextSeparator->addChild(m_geologyItemsText);

  SoSeparator *geologyMarkersSeparator = new SoSeparator;
  m_geologySwitch->addChild(geologyMarkersSeparator);

  m_geologyMarkersSet = new SoMarkerSet;
#if SO_INVENTOR_VERSION >= 9300
  m_geologyMarkersSet->markerGlobalScale = 1.0f;
  m_geologyMarkersSet->markerScale = 1.0f;
#endif
  m_geologyMarkersCoordinates = new SoVertexProperty;
  m_geologyMarkersSet->vertexProperty.setValue(m_geologyMarkersCoordinates);

  m_geologyMarkersMaterial = new SoMaterial;
  m_geologyMarkersMaterialBinding = new SoMaterialBinding;

  geologyMarkersSeparator->addChild(m_geologyMarkersMaterialBinding);
  geologyMarkersSeparator->addChild(m_geologyMarkersMaterial);

  geologyMarkersSeparator->addChild(m_geologyMarkersSet);

  displayGeologyLegend();
}

/**
 * Update the position and size of the legend. This needs to be called any time the
 * size of the viewport changes. According to the documentation for MoLegend (see
 * http://oivdoc92.vsg3d.com/APIS/RefManCpp/class_mo_legend.html) the bottomLeft and
 * topRight points are given in Normalized Device Coordinates [0..1]. In practice it
 * seems that the following rule holds:
 *   When width < height:
 *     - x runs from -1.0 to 1.0
 *     - y runs from -height/width to height/width
 *   When width > height:
 *     - x runs from -width/height to width/height
 *     - y runs from -1.0 to 1.0
 * So, in order to keep the legend in the topleft corner of the screen, this function
 * needs to be called on size changes.
 */
void LegendScene::updateValuesLegend(float widthScreen, float heightScreen, double minimum, double maximum) {
  bool undefined = (maximum < minimum || minimum == DBL_UNDEFINED || maximum == DBL_UNDEFINED);

  if (m_valuesLegend == 0)
    return;

  // update legend values
  double legendIncrement = (maximum - minimum) / (m_valuesCount - 1);

  SbString valueString;
  if (undefined) {
    char *strUndefined = " UNDEFINED ";
    int undefinedLength = strlen(strUndefined);

    for (int i = 0; i < m_valuesCount; ++i) {
      char c = (i < undefinedLength) ? strUndefined[i] : ' ';
      valueString.sprintf("%c", c);
      m_valuesText->string.set1Value(i, valueString);
    }
  } else {
    for (int i = 0; i < m_valuesCount; ++i) {

      valueString.sprintf("%-7.7G", minimum + i * legendIncrement);

      // remove redundant zeros
      for (int j = 0; j < 2; ++j) {
        valueString.replace("E+0", "E+");
        valueString.replace("E-0", "E-");
      }
      m_valuesText->string.set1Value(i, valueString);
    }
  }

  // convert pixel coordinates to normalized device coordinates for the MoLegend object
  float top;
  float left;

  float scaleX;
  float scaleY;

  if (widthScreen < heightScreen) {
    top = heightScreen / widthScreen;
    left = -1.0f;

    scaleY = 2.0f * heightScreen / widthScreen;
    scaleX = 2.0f;
  } else {
    left = -widthScreen / heightScreen;
    top = 1.0f;

    scaleY = 2.0f;
    scaleX = 2.0f * widthScreen / heightScreen;
  }

  float offsetX = (m_valuesLegendOffsetX / widthScreen) * scaleX;
  float offsetY = (getFullValuesLegendOffsetY() / heightScreen) * scaleY;
  float width = (m_valuesLegendWidth / widthScreen) * scaleX;
  float height = (m_valuesLegendHeight / heightScreen) * scaleY;

  m_valuesLegend->topRight =
      SbVec2f(left + offsetX, top - offsetY - height); // topRight is actually coordinates of bottomLeft on screen
  m_valuesLegend->bottomLeft =
      SbVec2f(left + offsetX + width, top - offsetY); // bottomLeft is actually coordinates of topRight on screen

  m_valuesTextTranslation->translation =
      SbVec3f(m_valuesLegendOffsetX + m_valuesLegendWidth + m_valuesTextOffset,
              -(m_valuesLegendOffsetY + (m_valuesAdornmentCount - 1) * m_textSize * m_textSpacing), 0);

  SoNode *fontSeparatorNode = m_valuesLegend->getByName("Values");
  SoSeparator *fontSeparator = dynamic_cast<SoSeparator *>(fontSeparatorNode);
}

int LegendScene::getFullValuesLegendOffsetY() {
  return m_valuesLegendOffsetY + m_valuesAdornmentCount * m_textSize * m_textSpacing;
}

void LegendScene::SetTextColor(float *rgb) { m_material->diffuseColor.setValue(rgb); }

void LegendScene::GetTextColor(float *rgb) {

  const SbColor &color = m_material->diffuseColor[0];
  color.getValue(rgb[0], rgb[1], rgb[2]);
}

void LegendScene::updateGeologyLegend() {
  m_geologyHeadersText->string.setValue("");
  m_geologyItemsText->string.setValue("");
  m_geologyMarkersSet->markerIndex.setValue(GEOLOGYMARKERINDEX);
  m_geologyMarkersMaterial->diffuseColor.setValue(0.0f, 0.0f, 0.0f);
  m_geologyMarkersMaterial->transparency.setValue(0.0f);
  m_geologyMarkersMaterialBinding->value = SoMaterialBinding::PER_VERTEX;
  m_geologyMarkersCoordinates->vertex.setValue(-100.0f, 100.0f, 0.0f);

  size_t itemIndex;
  m_listOfGeologyLegendItemLists.listLegendItems(itemIndex, m_geologyHeadersText->string, m_geologyItemsText->string,
                                                 m_geologyMarkersCoordinates->vertex,
                                                 m_geologyMarkersMaterial->diffuseColor);

  for (size_t i = 0; i < itemIndex; ++i) {
#if SO_INVENTOR_VERSION >= 9300
    m_geologyMarkersSet->markerScale.set1Value(i, 1.0f);
#endif
    m_geologyMarkersSet->markerIndex.set1Value(i, GEOLOGYMARKERINDEX);
    m_geologyMarkersMaterial->transparency.set1Value(i, 0.0);
  }
}

void LegendScene::displayGeologyLegend() {
  m_valuesTextSwitch->whichChild = SO_SWITCH_NONE;
  m_valuesLegendSwitch->whichChild = SO_SWITCH_NONE;
  m_geologySwitch->whichChild = SO_SWITCH_ALL;
}

void LegendScene::displayValuesLegend() {
  m_valuesTextSwitch->whichChild = SO_SWITCH_ALL;
  m_valuesLegendSwitch->whichChild = SO_SWITCH_ALL;
  m_geologySwitch->whichChild = SO_SWITCH_NONE;
}

bool LegendScene::isDisplayingValueLegend() { return m_valuesLegendSwitch->whichChild.getValue() == SO_SWITCH_ALL; }

void LegendScene::setTitle(const std::string &title) {
  // m_legend->title = title;
}

const std::string LegendScene::getTitle() const { return m_valuesLegend->title.getValue().getString(); }

void LegendScene::addValuesAdornment(const std::string &adornment) {
  if (m_valuesAdornments == 0) {
    m_valuesAdornments = new SoText2();
    m_valuesAdornments->justification = SoText2::LEFT;
    m_valuesAdornments->spacing = m_textSpacing;
    m_valuesTextSwitch->addChild(m_valuesAdornments);
  }
  m_valuesAdornments->string.set1Value(m_valuesAdornmentCount, SbString(adornment));
  ++m_valuesAdornmentCount;
}

void LegendScene::clearValuesAdornments() {
  if (m_valuesAdornments != 0) {
    m_valuesTextSwitch->removeChild(m_valuesAdornments);
    m_valuesAdornments = 0;
    m_valuesAdornmentCount = 0;
  }
}

void LegendScene::initLegend() {
  m_valuesLegend->setName("Legend");
  m_valuesLegend->vertical = true;
  m_valuesLegend->title = "";
  m_valuesLegend->titleColor.setValue(1.0f, 1.0f, 1.0f);
  m_valuesLegend->titleFontSize = 10;
  m_valuesLegend->displayValues =
      false; // Not using the legend values because we cannot set font or display side of these values
  m_valuesLegend->numValues = m_valuesCount;

  m_valuesLegend->valuesColor.setValue(1.0f, 1.0f, 1.0f);
  m_valuesLegend->valuesFontSize = m_textSize;
  m_valuesLegend->setName("Legend");

  SoSFVec2f bottomLeft;
  bottomLeft.setValue(0.0f, 0.0f);
  m_valuesLegend->bottomLeft = bottomLeft;

  SoSFVec2f topRight;
  topRight.setValue(0.1f, 0.1f);
  m_valuesLegend->topRight = topRight;
}

void LegendScene::addGeologyLegendItem(const COpenGLNode *reference) {
  m_listOfGeologyLegendItemLists.addLegendItem(reference);
}

void LegendScene::refreshGeologyLegendItem(const COpenGLNode *reference) {
  m_listOfGeologyLegendItemLists.refreshLegendItem(reference);
}

void LegendScene::removeGeologyLegendItem(const COpenGLNode *reference) {
  m_listOfGeologyLegendItemLists.removeLegendItem(reference);
}

SbBox2i32 LegendScene::getValuesLegendBoundingBox() {
  // slightly bigger than the actual values legend
  const int extension = 30;
  SbBox2i32 bbox(m_valuesLegendOffsetX - extension, getFullValuesLegendOffsetY() - extension,
                 m_valuesLegendOffsetX + m_valuesLegendWidth + extension,
                 getFullValuesLegendOffsetY() + m_valuesLegendHeight + extension);

  return bbox;
}

GeologyLegendItemList *ListOfGeologyLegendItemLists::getGeologyLegendItemList(const std::string &listName) {
  GeologyLegendItemList *itemListFound = 0;
  size_t insertionIndex = 0;
  for (auto itemList : m_listOfGeologyLegendItemLists) {
    if (itemList->name() == listName) {
      itemListFound = itemList;
      break;
    }
    if (itemList->name() > listName)
      break;

    ++insertionIndex;
  }

  if (!itemListFound) {
    auto itemListNew = new GeologyLegendItemList(listName);
    m_listOfGeologyLegendItemLists.insert(m_listOfGeologyLegendItemLists.begin() + insertionIndex, itemListNew);
    itemListFound = itemListNew;
  }

  return itemListFound;
}

void ListOfGeologyLegendItemLists::addLegendItem(const COpenGLNode *reference) {
  std::string listName = reference->TypeName().toStdString();

  GeologyLegendItemList *itemList = getGeologyLegendItemList(listName);
  assert(itemList != 0);

  itemList->addGeologyLegendItem(reference);
}

void ListOfGeologyLegendItemLists::refreshLegendItem(const COpenGLNode *reference) {
  std::string listName = reference->TypeName().toStdString();
  GeologyLegendItemList *itemList = getGeologyLegendItemList(listName);
  assert(itemList != 0);

  itemList->refreshGeologyLegendItem(reference);
}

void ListOfGeologyLegendItemLists::removeLegendItem(const COpenGLNode *reference) {
  std::string listName = reference->TypeName().toStdString();
  GeologyLegendItemList *itemList = getGeologyLegendItemList(listName);
  if (itemList == 0)
    return;
  assert(itemList != 0);
  itemList->removeGeologyLegendItem(reference);
}

void ListOfGeologyLegendItemLists::listLegendItems(size_t &itemIndex, SoMFString &geologyHeaders,
                                                   SoMFString &geologyItems, SoMFVec3f &point,
                                                   SoMFColor &diffuseColor) {
  itemIndex = 0;
  for (auto itemList : m_listOfGeologyLegendItemLists) {
    if (itemList->empty())
      continue;

    geologyHeaders.set1Value(itemIndex, itemList->name());
    geologyItems.set1Value(itemIndex, "");
    point.set1Value(itemIndex, -100.0f, 100.0f, 0.0f); // outside viewport
    diffuseColor.set1Value(itemIndex, 0.0f, 0.0f, 0.0f);
    ++itemIndex;

    itemList->listItems(itemIndex, geologyHeaders, geologyItems, point, diffuseColor);

    // empty line
    geologyHeaders.set1Value(itemIndex, "");
    geologyItems.set1Value(itemIndex, "");
    point.set1Value(itemIndex, -100.0f, 100.0f, 0.0f); // outside viewport
    diffuseColor.set1Value(itemIndex, 0.0f, 0.0f, 0.0f);
    ++itemIndex;
  }
}

void GeologyLegendItemList::addGeologyLegendItem(const COpenGLNode *reference) {

  GeologyLegendItem *itemFound = 0;
  int insertionIndex = 0;
  for (auto item : m_geologyLegendItemList) {
    if (item->reference() == reference) {
      itemFound = item;
      ++(itemFound->referenceCount());
      break;
    } else if (reference->Less(*item->reference())) {
      // to keep things sorted
      break;
    }
    ++insertionIndex;
  }
  if (!itemFound) {
    std::string name = reference->Name().toStdString();
    auto newItem = new GeologyLegendItem(reference, name);
    m_geologyLegendItemList.insert(m_geologyLegendItemList.begin() + insertionIndex, newItem);
    ++(newItem->referenceCount());
  }
}

void GeologyLegendItemList::refreshGeologyLegendItem(const COpenGLNode *reference) {
  for (auto item : m_geologyLegendItemList) {
    if (item->reference() == reference) {
      item->refreshContents();
      break;
    }
  }
}

void GeologyLegendItemList::removeGeologyLegendItem(const COpenGLNode *reference) {
  size_t itemIndex = 0;
  for (auto item : m_geologyLegendItemList) {
    if (item->reference() == reference) {
      if (--(item->referenceCount()) == 0) {
        m_geologyLegendItemList.erase(m_geologyLegendItemList.begin() + itemIndex);
        delete item;
      }
      break;
    }
    ++itemIndex;
  }
}

void GeologyLegendItemList::listItems(size_t &itemIndex, SoMFString &geologyHeaders, SoMFString &geologyItems,
                                      SoMFVec3f &point, SoMFColor &diffuseColor) {
  for (auto item : m_geologyLegendItemList) {
    geologyHeaders.set1Value(itemIndex, "");
    geologyItems.set1Value(itemIndex, item->name());
    point.set1Value(itemIndex, 14.0f, 4.0f - itemIndex * 18.0f, 0.0f);
    diffuseColor.set1Value(itemIndex, item->color());
    ++itemIndex;
  }
}

bool GeologyLegendItemList::empty() { return m_geologyLegendItemList.empty(); }

size_t GeologyLegendItemList::size() { return m_geologyLegendItemList.size(); }

extern void DecomposeQColor(QRgb color, float &red, float &green, float &blue);

void GeologyLegendItem::refreshContents() {
  float red, green, blue;
  DecomposeQColor(m_reference->Color(), red, green, blue);
  m_color[0] = red, m_color[1] = green, m_color[2] = blue;

  m_name = m_reference->Name().toStdString();
}
