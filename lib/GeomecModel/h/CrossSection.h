// CrossSection.h: interface for the CCrossSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CROSSSECTION_H__1B63715F_8AB9_4BA7_A0E7_ED876978D075__INCLUDED_)
#define AFX_CROSSSECTION_H__1B63715F_8AB9_4BA7_A0E7_ED876978D075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CModelBase;
class CTSSurface;
class C3DFormation;
class CNewGeoWellPoint;
class CNewWellPath;
class CNewWellPathEntry;

namespace geo {
  class ISurface;
}

#include "StorageNode.h"
#include "openglnode.h"
#include "Plane.h"
#include "colornode.h"
#include "NewWellPath.h"
#include "GeomecModelVisitor.h"

class CCrossSection : public CStorageNode
{
  // The cross section create CCSObject object for every formation or wellpath. The CCSObject is closely
  // related to the formation or wellpath it uses the name, color, icon, less-function and typename id of 
  // the original formation or wellpath. 
public:

  enum OrientationType
  {
    ORIENTATION_DEPTH = 0,
    ORIENTATION_NE = 1,
    ORIENTATION_ANY = 2,
    ORIENTATION_NORTHING = 3,
    ORIENTATION_EASTING = 4,
    ORIENTATION_CURRENT = 5
  };

  class CCSObject : public COpenGLNode
  {
    CCrossSection& m_cross_section;
    COpenGLNode& m_base_node;
  public:
    class CCSObjectDrawDef : public COpenGLNode::CDrawDef
    {
    public:
      CCSObjectDrawDef(const COpenGLNode& node)
        :CColorNode::CDrawDef(node) {}
      DrawDecisionBool PolyFillFront() const { return std::make_pair(true, true); }
      DrawDecisionBool PolyFillBack() const { return std::make_pair(true, true); }
    };
    CCSObject(CCrossSection& cross_section, COpenGLNode& base_node);
    const CCrossSection &CrossSection() const;
    CCrossSection &CrossSection();
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual const QString& Name() const;
    virtual TColor Color() const;
    virtual bool Less(const CGraphNode &node) const;
    virtual bool Empty() const { return false; }
    virtual long SavedItems() const { return 0; }
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    virtual void OnNeighbourDeleted(const CGraphNode &node);
    virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;
  };

  class CCSFormation : public CCSObject
  {
    mutable std::vector<geo::ISurface*> m_vcSurface;
    mutable bool m_bDirty;
    C3DFormation &m_BaseFormation;
    void Invalidate(); 
    bool IsValid() const;
  public:
    CCSFormation(CCrossSection &cross_section, C3DFormation &base_formation);
    virtual ~CCSFormation();
    void CalculateIntersection() const;
    virtual int DisplayListSize() const;
    virtual const geo::IObject &DisplayList(int nIndex) const;
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    int SurfaceSize() const;
    const geo::ISurface& Surface(int nIndex) const;
    const C3DFormation &BaseObject() const;
    C3DFormation &BaseObject();
    virtual bool Less(const CGraphNode &node) const;

  };

  class CCSWellPath : public CCSObject
  {
    CNewWellPath &m_WellPath;
    mutable geo::CPtrArray<CNewGeoWellPoint> m_DisplayList;	
  public:
    CCSWellPath(CCrossSection &csect, CNewWellPath &wellpath);
    virtual ~CCSWellPath();
    void CalculateIntersection() const;
    virtual const geo::IObject &DisplayList(int nIndex) const;
    virtual int DisplayListSize() const;
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

    const CNewWellPath &WellPath() const;
    CNewWellPath &WellPath();

  };
  friend class CCSWellPath;

  enum TCrossSectionType
  {
      VERTICAL = 0,
      HORIZONTAL,
      MANUAL
  };

private:
  CNewWellPathEntry* m_pWellPathEntry;

  geo::CVector m_NormalBookmarked;
  geo::CVector m_NormalScreen;

  geo::CPlane m_IntersectPlaneBookmarked;
  geo::CPlane m_IntersectPlaneScreen;

  OrientationType m_orientationBookmarked;
  OrientationType m_orientationScreen;

  CMouseListener* m_pModifier;
  mutable CTSSurface *m_pExportSurface;

  std::vector<CCSObject*> m_CSObjects;
  bool m_bDirty;

  TCrossSectionType m_crossSectionType;


  void Recalculate();
public:
  CCrossSection(CFemAppModel &model, const geo::IPoint &basePoint, const geo::IVector &normal);
  CCrossSection(const QString &sName, CFemAppModel &model, const geo::IPoint &basePoint, const geo::IVector &normal);
  CCrossSection(CFemAppModel &model, const geo::IPoint &RefPoint1, const geo::IPoint &RefPoint2);
  CCrossSection(const QString &sName, CFemAppModel &model, const geo::IPoint &RefPoint1, const geo::IPoint &RefPoint2);
  CCrossSection(CFemAppModel& model, const double& dDepth);
  CCrossSection(const QString& sName, CFemAppModel& model, const double& dDepth);
  CCrossSection(const QString &sName, CFemAppModel &model);
  CCrossSection(const CCrossSection &Xsec);

  // constructor for loading a CrossSection
  CCrossSection( CFemAppModel &model );

  ~CCrossSection();
  virtual bool Destroy()
  {
    UnLinkAll();
    return CStorageNode::Destroy();
  }

  const std::vector<CCSObject*>& CSObjects() const;
  void Clear(bool bSendModified = true);

  virtual void OnDeselect(CGraphNode* NewNode);

  CCrossSection& operator =(const CCrossSection &rhs);
  bool operator==(const CCrossSection &rhs) const;

  int ElementSize() const;

  // Neighbour messaging
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  const geo::IVector& NormalBookmarked() const { return m_NormalBookmarked; }
  const geo::IVector& NormalScreen() const { return m_NormalScreen; }

  const geo::IPlane& IntersectPlaneBookmarked() const {return m_IntersectPlaneBookmarked;}
  const geo::IPlane& IntersectPlaneScreen() const {return m_IntersectPlaneScreen;}

  OrientationType OrientationBookmarked() const {return m_orientationBookmarked;}
  OrientationType OrientationScreen() const {return m_orientationScreen;}

  void SetBookmarked(const geo::IPoint& basePoint, const geo::IVector& normal, OrientationType orientation, bool bSendModified=true);
  void SetScreen(const geo::IPoint& basePoint, const geo::IVector& normal, OrientationType orientation, bool bSendModified=true);
  void SetScreen(const geo::IPoint& basePoint, const geo::IVector& normal, bool bSendModified=true);
  void SetBookmarked(const geo::IPoint& first, const geo::IPoint& second, bool bSendModified = true);
  void SetBookmarked(const double& dDepth, bool bSendModified = true);

  void CopyFromScreen();
  void SetScreenFromBookmarked();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;
  virtual bool Empty() const {return (m_CSObjects.size() == 0);} // no formations, no cross section..

  bool CanFlip() const;
  void Flip();

  OrientationType Orientation() const;
  bool Horizontal() const;

  // This function supplies a single surface from the different surfaces in the cross section.
  // The surface is stored in m_pExportSurface. For export purposes.
  CTSSurface *ExportSurface() const;
  geo::CPoint First() const;
  geo::CPoint Second() const;

  // for creating a copy without using the copy constructor

  const geo::IPoint& basePoint() const;
  const geo::IVector& normal() const;

  TCrossSectionType crossSectionType() const;
  void crossSectionType(TCrossSectionType crossSectionType);

  static bool calculateTwoArbitraryPointsInPlane(geo::IPoint& first,
      geo::IPoint& second, const geo::IPlane& plane,
      const CModelBase& modelBase);

  ACCEPT_GEOMECMODELVISITORS(VisitCrossSection);
};

typedef CStorageNodeEntry<CCrossSection> TCrossSectionEntry;
class CCrossSectionEntry : public CStorageNodeEntry<CCrossSection>
{
  typedef CGraphEntryTemp<CCrossSection> BASE_CLASS;
public:
  typedef CSingleCommandTemplate<CCrossSectionEntry> TCrossSectionCommand;
  CCrossSectionEntry(CModelBase& model);

  ACCEPT_GEOMECMODELVISITORS(VisitCrossSectionEntry);
};

#endif // !defined(AFX_CROSSSECTION_H__1B63715F_8AB9_4BA7_A0E7_ED876978D075__INCLUDED_)
