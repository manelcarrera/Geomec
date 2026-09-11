#ifndef CROSSSECTIONGROUP_H_INCLUDED
#define CROSSSECTIONGROUP_H_INCLUDED

class CCrossSection;
class MeshCrossSection;
class OIDIMeshNodeManager;

#include <Inventor/nodes/SoGroup.h>

#include <map>

class CrossSectionGroup : public SoGroup {
  typedef std::map<const CCrossSection *, MeshCrossSection *> XSecMap;
  XSecMap m_xsecMap;

  OIDIMeshNodeManager *m_meshNodeMgr;

  MeshCrossSection *buildCrossSectionNode(CCrossSection &xsec) const;

public:
  explicit CrossSectionGroup(OIDIMeshNodeManager *mgr);

  void enableCrossSection(const CCrossSection *xsec);

  void disableCrossSection(const CCrossSection *xsec);

  void setActiveCrossSection(const CCrossSection *xsec);

  MeshCrossSection *getCrossSectionNode(const CCrossSection *xsec) const;

  void SelectClipPlaneMode(bool enable);
};

#endif
