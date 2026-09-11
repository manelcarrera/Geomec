#include "stdafx.h"

#include "OIVWellPath.h"

#include "NewWellPath.h"
#include "WellDefinitionPointList.h"
#include "WellZoomInModel.h"

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoExtrusion.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>

#include <MeshViz/graph/PoArrow3.h>

SoNode *OIVWellPath::createWellPathNode(const std::vector<SbVec3f> &points, SbColor color, float radius) {
  size_t n = points.size();

  SoExtrusion *model = new SoExtrusion;

  // set cross section
  size_t m = 16;
  if (radius == 0)
    radius = 5;

  model->crossSection.setNum(m + 1);

  SbVec2f *cptr = model->crossSection.startEditing();
  for (size_t i = 0; i <= m; ++i, ++cptr) {
    float angle = i * 2 * M_PI / m;
    float x = radius * cosf(angle);
    float y = radius * sinf(angle);

    *cptr = SbVec2f(x, y);
  }
  model->crossSection.finishEditing();

  // set spine
  model->spine.setValues(0, (int)points.size(), &points[0]);

  // set color
  SoBaseColor *colorNode = new SoBaseColor;
  colorNode->rgb = color;

  SoSeparator *sep = new SoSeparator;
  sep->addChild(colorNode);
  sep->addChild(model);

  return sep;
}

SoNode *OIVWellPath::createWellPathNode(const CNewWellPath &wellPath) {
  std::vector<SbVec3f> coords;

  const geo::CPolyLine line = static_cast<const geo::CPolyLine &>(wellPath.DisplayList(0));

  COpenGLNodeBase::TColor color = wellPath.Color();
  SbColor sbcolor(qRed(color) / 255.0f, qGreen(color) / 255.0f, qBlue(color) / 255.0f);

  coords.push_back(SbVec3f(wellPath.GlobalNorthing() - m_translation[0], wellPath.GlobalEasting() - m_translation[1],
                           wellPath.DerElev() - m_translation[2])); // derrick floor

  for (size_t i = 0; i < line.PointSize(); ++i) {
    const geo::IPoint &p = line.Point(i);

    SbVec3f n(p.X() - m_translation[0], p.Y() - m_translation[1], p.Z() - m_translation[2]);

    if (coords.back() != n)
      coords.push_back(n);
  }

  float radius = 0;
  if (wellPath.hasZoomInModel())
    radius = static_cast<float>(wellPath.ZoomInModel().Radius());

  return createWellPathNode(coords, sbcolor, radius);
}

SoNode *OIVWellPath::createLabelNode(const char *labelText, const SbVec3f &position) {
  SoFont *pFont1 = new SoFont();
  pFont1->name = "Arial";
  pFont1->size = 12;
  pFont1->renderStyle = SoFont::TEXTURE;
  // SoMaterial *pMatl1 = new SoMaterial();
  // pMatl1->diffuseColor = SbColor(1,1,1);
  SoText2 *text = new SoText2;
  text->string = labelText;

  SoTranslation *textTranslation = new SoTranslation;
  textTranslation->translation = position;

  SoSeparator *textSeparator = new SoSeparator;
  textSeparator->addChild(pFont1);
  // textSeparator->addChild(pMatl1);
  textSeparator->addChild(textTranslation);
  textSeparator->addChild(text);

  return textSeparator;
}

SoNode *OIVWellPath::createSectionNode(const SbVec3f &top, const SbVec3f &bottom, const char *name) {
  const float width = 600.0f;
  const SbVec3f offset(width, 0.0f, 0.0f);

  SbVec3f vertices[] = {top, top + offset, bottom, bottom + offset};
  SoVertexProperty *vertexProperty = new SoVertexProperty;
  vertexProperty->vertex.setValues(0, 4, vertices);

  SoLineSet *lineSet = new SoLineSet;
  int lineSizes[] = {2, 2};
  lineSet->numVertices.setValues(0, 2, lineSizes);
  lineSet->vertexProperty = vertexProperty;

  // Create arrows
  const SbVec3f arrowOffset(500.0f, 0.0f, 0.0f);

  float verticalDistance = std::abs(top[2] - bottom[2]);
  const float arrowHeadWidth = std::min(100.0f, verticalDistance / 3.0f);
  const float arrowHeadHeight = .6f * arrowHeadWidth;

  SbVec3f points[2];
  points[0] = top + arrowOffset;
  points[1] = bottom + arrowOffset;
  PoArrow3 *arrow =
      new PoArrow3(2, points, PoArrow3::DIRECT_TRIANGLE, PoArrow3::DIRECT_TRIANGLE, arrowHeadWidth, arrowHeadHeight);

  // Label
  SbVec3f position = .5f * (points[0] + points[1]);
  SoNode *labelNode = createLabelNode(name, position);

  SoGroup *sectionGroup = new SoGroup;
  sectionGroup->addChild(lineSet);
  sectionGroup->addChild(arrow);
  sectionGroup->addChild(labelNode);

  return sectionGroup;
}

SoNode *OIVWellPath::createInfoNode(const CNewWellPath &wellPath) {
  SoGroup *infoGroup = new SoGroup;
  SoFont *pFont1 = new SoFont();
  pFont1->name = "Arial";
  pFont1->size = 1;
  pFont1->renderStyle = SoFont::TEXTURE;
  SoMaterial *pMatl1 = new SoMaterial();
  pMatl1->diffuseColor = SbColor(1, 1, 1);

  infoGroup->addChild(pFont1);
  infoGroup->addChild(pMatl1);

  // well sections
  const std::list<CNewFormationSection> &sections = wellPath.FormationSections();
  std::list<CNewFormationSection>::const_iterator i = sections.begin();
  std::list<CNewFormationSection>::const_iterator e = sections.end();
  for (; i != e; ++i) {
    const CNewWellPoint &top = i->Top();
    const CNewWellPoint &bottom = i->Bottom();
    std::string desc = i->GetDescription();
    const char *name = desc.c_str();

    infoGroup->addChild(createSectionNode(
        SbVec3f(top.X() - m_translation[0], top.Y() - m_translation[1], top.Z() - m_translation[2]),
        SbVec3f(bottom.X() - m_translation[0], bottom.Y() - m_translation[1], bottom.Z() - m_translation[2]), name));
  }
  return infoGroup;
}

OIVWellPath::OIVWellPath(const CNewWellPath &wellPath, const SbVec3d &translation) {
  m_translation = translation;
  m_infoSwitch = new SoSwitch;
  m_infoSwitch->addChild(createInfoNode(wellPath));
  m_infoSwitch->whichChild = wellPath.DrawWellInfo() ? SO_SWITCH_ALL : SO_SWITCH_NONE;

  addChild(createWellPathNode(wellPath));
  addChild(m_infoSwitch);

  setName("OIVWellPath");
}

void OIVWellPath::showInfo(bool enable) { m_infoSwitch->whichChild = enable ? SO_SWITCH_ALL : SO_SWITCH_NONE; }
