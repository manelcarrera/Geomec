#ifndef _OIVWELLPATH_H_
#define _OIVWELLPATH_H_

class SoNode;
class SoSwitch;
class SoFont;

class CNewWellPath;

namespace well {
class IWellSection;
}

#include <Inventor/nodes/SoGroup.h>

class OIVWellPath : public SoGroup {
  SoNode *createWellPathNode(const std::vector<SbVec3f> &points, SbColor color, float radius);

  SoNode *createWellPathNode(const CNewWellPath &wellPath);

  SoNode *createLabelNode(const char *labelText, const SbVec3f &position);

  SoNode *createSectionNode(const SbVec3f &top, const SbVec3f &bottom, const char *name);

  SoNode *createInfoNode(const CNewWellPath &wellPath);

  SoSwitch *m_infoSwitch;

  SbVec3d m_translation;

public:
  OIVWellPath(const CNewWellPath &wellPath, const SbVec3d &translation);

  void showInfo(bool enable);
};

#endif
