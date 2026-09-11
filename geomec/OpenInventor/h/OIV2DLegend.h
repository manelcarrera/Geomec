#ifndef _OIV2DLegend_h_
#define _OIV2DLegend_h_

class SoFont;
class SoMaterial;
class SoSwitch;

#include <Inventor/nodes/SoAnnotation.h>

class OIV2DLegendSection;
class CNewWellPath;

const int OIV2DMARKERINDEX = 91;

class OIV2DLegend : public SoAnnotation {
public:
  OIV2DLegend();
  void setForegroundColor(float textColor[3]);
  void clear();
  void addLegendSection();
  int numLegendSections();

  void addItem(std::size_t index, const std::string &name, float red, float green, float blue);

  void addLeader(std::size_t index, const std::string &leader);
  void updateSectionOffsets();

  int getNumLines(int i);

  void clear(std::size_t index);

private:
  OIV2DLegendSection *legendSection(std::size_t index) const;

  SoFont *m_font;
  SoMaterial *m_material;

  int m_textSize;
  float m_textSpacing;

  SoSwitch *m_switch;

  SoGroup *m_legendSections;
};

#endif // _OIV2DLegend_h_
