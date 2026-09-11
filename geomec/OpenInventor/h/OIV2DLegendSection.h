#ifndef _OIV2DLegendSection_h_
#define _OIV2DLegendSection_h_

class SoTranslation;
class SoText2;
class SoVertexProperty;
class SoMarkerSet;
class SoMaterial;
class SoMaterialBinding;

#include <Inventor/nodes/SoSeparator.h>

class OIV2DLegendSection : public SoSeparator {
public:
  OIV2DLegendSection();
  void clear();
  void addItem(const std::string &name, float red, float green, float blue);
  void addLeader(const std::string &leader);
  void setOffset(int numberOfLines);

  int getNumLines();

private:
  float m_textSpacing;

  SoTranslation *m_translation;

  SoText2 *m_leadersText;
  SoText2 *m_colorItemsText;
  SoTranslation *m_colorItemsTranslation;
  SoVertexProperty *m_colorMarkersCoordinates;
  SoMarkerSet *m_colorMarkersSet;
  SoMaterial *m_colorMarkersMaterial;
  SoMaterialBinding *m_colorMarkersMaterialBinding;
};

#endif // _OIV2DLegendSection_h_
