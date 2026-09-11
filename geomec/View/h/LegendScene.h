#ifndef _LEGEND_SCENE_H__
#define _LEGEND_SCENE_H__

#include <string>
#include <vector>

#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoSeparator.h>
#include <MeshVizXLM/MbVec3.h>

class SoSwitch;
class SoText2;
class SoCoordinate3;
class SoMarkerSet;
class SoFont;
class SoMaterial;
class MoLegend;
class SoTranslation;
class SoMaterial;
class SoMFVec3f;
class SoMFColor;
class SoMFFloat;
class SoMaterialBinding;
class SoVertexProperty;

struct GeologyLegendItem {
public:
  GeologyLegendItem(const COpenGLNode *reference, const std::string &name)
      : m_reference(reference), m_name(name), m_referenceCount(0) {
    refreshContents();
  }

  const COpenGLNode *reference() { return m_reference; }
  const float *color() { return m_color; }
  const std::string &name() { return m_name; }

  int &referenceCount() { return m_referenceCount; }

  void refreshContents();

private:
  const COpenGLNode *m_reference;
  std::string m_name;
  float m_color[3];
  int m_referenceCount;
};

class GeologyLegendItemList {
public:
  GeologyLegendItemList(const std::string &name) : m_name(name) {}

  const std::string &name() { return m_name; }

  void addGeologyLegendItem(const COpenGLNode *reference);
  void refreshGeologyLegendItem(const COpenGLNode *reference);
  void removeGeologyLegendItem(const COpenGLNode *reference);
  void listItems(size_t &itemIndex, SoMFString &geologyHeaders, SoMFString &geologyItems, SoMFVec3f &point,
                 SoMFColor &diffuseColor);
  bool empty();
  size_t size();

private:
  std::string m_name;
  std::vector<GeologyLegendItem *> m_geologyLegendItemList;
};

class ListOfGeologyLegendItemLists {
public:
  GeologyLegendItemList *getGeologyLegendItemList(const std::string &listName);
  void addLegendItem(const COpenGLNode *reference);
  void refreshLegendItem(const COpenGLNode *reference);
  void removeLegendItem(const COpenGLNode *reference);
  void listLegendItems(size_t &itemIndex, SoMFString &geologyHeaders, SoMFString &geologyItems, SoMFVec3f &point,
                       SoMFColor &diffuseColor);

private:
  std::vector<GeologyLegendItemList *> m_listOfGeologyLegendItemLists;
};

class LegendScene : public SoSeparator {
public:
  LegendScene();

  void initLegend();

  void setTitle(const std::string &title);
  const std::string getTitle() const;

  void addValuesAdornment(const std::string &adornment);
  void clearValuesAdornments();

  void displayValuesLegend();
  void displayGeologyLegend();
  bool isDisplayingValueLegend();
  void updateValuesLegend(float w, float h, double minimum, double maximum);

  void addGeologyLegendItem(const COpenGLNode *reference);
  void refreshGeologyLegendItem(const COpenGLNode *reference);
  void removeGeologyLegendItem(const COpenGLNode *reference);
  void updateGeologyLegend();

  void SetTextColor(float *rgb);
  void GetTextColor(float *rgb);
  SbBox2i32 getValuesLegendBoundingBox();

  int getFullValuesLegendOffsetY();

private:
  SoFont *m_font;
  SoMaterial *m_material;
  SoSwitch *m_valuesTextSwitch;
  SoSwitch *m_valuesLegendSwitch;
  SoSwitch *m_geologySwitch;

  MoLegend *m_valuesLegend;

  SoText2 *m_valuesAdornments;
  int m_valuesAdornmentCount;

  int m_textSize;
  float m_textSpacing;

  SoText2 *m_valuesText;
  int m_valuesTextOffset; // offset from legend bar
  SoTranslation *m_valuesTextTranslation;

  int m_valuesCount;

  SoText2 *m_geologyHeadersText;
  SoText2 *m_geologyItemsText;
  SoVertexProperty *m_geologyMarkersCoordinates;
  SoMarkerSet *m_geologyMarkersSet;
  SoMaterial *m_geologyMarkersMaterial;
  SoMaterialBinding *m_geologyMarkersMaterialBinding;

  ListOfGeologyLegendItemLists m_listOfGeologyLegendItemLists;

  // all in pixels
  int m_valuesLegendOffsetX;
  int m_valuesLegendOffsetY;
  int m_valuesLegendWidth;
  int m_valuesLegendHeight;
};

#endif // _LEGEND_SCENE_H__