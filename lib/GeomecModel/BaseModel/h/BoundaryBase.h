// BoundaryBase.h: interface for the CBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDARYBASE_H__A839AC62_0920_46B6_AAFE_1FB5703ACD21__INCLUDED_)
#define AFX_BOUNDARYBASE_H__A839AC62_0920_46B6_AAFE_1FB5703ACD21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHorizonBase;
class IElementSet;

namespace geo {
  class CBox;
  class ISurface;
} // namespace geo

#include "colornode.h"
#include "Rectangle.h"
#include "LengthQuantity.h"
#include "GeomecModelVisitor.h"

class CBoundaryBase : public CColorNode  
{
public:
  class CBoundaryDrawDef : public CColorNode::CDrawDef
  {
  public:
    CBoundaryDrawDef(const COpenGLNode& node)
      :CColorNode::CDrawDef(node) {}
    DrawDecisionBool PolyFillFront() const { return std::make_pair(true, false); }
    DrawDecisionBool PolyFillBack() const { return std::make_pair(true, false); }
  };

  // Our min max device ...
  typedef std::pair<geo::CPoint, geo::CPoint> TMinMax;

  // when rotated
  typedef struct _Rotated
  {
  _Rotated() {}
  _Rotated(const _Rotated& rhs)
  : ptMid(rhs.ptMid),
      dAzimuth(rhs.dAzimuth),
      dLength(rhs.dLength),
      dWidth(rhs.dWidth),
      dMinDepth(rhs.dMinDepth),
      dMaxDepth(rhs.dMaxDepth)
  {}
  _Rotated(const geo::IPoint& mid, double azimuth, double length, double width, double minDepth, double maxDepth)
  : ptMid(mid),
      dAzimuth(azimuth),
      dLength(length),
      dWidth(width),
      dMinDepth(minDepth),
      dMaxDepth(maxDepth)
  {}

  geo::CPoint ptMid;
  double dAzimuth;
  double dLength;
  double dWidth;
  double dMinDepth;
  double dMaxDepth;
  } TRotated;

  // State enumeration ...
  enum BOUNDARY_STATE { DEFAULT_DEFINED, BEST_FIT, USER_DEFINED, ROTATED };

public:
  typedef CSingleCommandTemplate<CBoundaryBase> TBoundaryBaseCommand;
  CLengthQuantity R() const;

  virtual TMinMax BestFit() const;
  virtual TMinMax SnapToGrid(const TMinMax& minmax) const = 0;
  virtual TRotated SnapToGrid(const TRotated& rotated) const { return rotated; }
  void OnGridModified();

  // resulting info
  const geo::CBox &Box() const;
  virtual geo::CPoint Max() const;
  virtual geo::CPoint Min() const;
  double BoxAzimuth() const;

  // info when aligned
  const geo::CPoint& MaxPoint() const;
  const geo::CPoint& MinPoint() const;

  // info when rotated
  const geo::CPoint& Mid() const;
  double Azimuth() const; // rotation azimuth in degrees
  double Length() const;
  double Width() const;

  geo::CRectangle RectangleXY() const;
  void RectangleXY(const geo::CRectangle& RectangleXY);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  CBoundaryBase(const geo::IPoint& ptMin, 
          const geo::IPoint& ptMax,
          CFemAppModel& model,	  
          BOUNDARY_STATE state = DEFAULT_DEFINED);

  // Property dialog
  CBoundaryBase(const CBoundaryBase &boundary);
  bool operator==(const CBoundaryBase& rhs) const;
  CBoundaryBase& operator=(const CBoundaryBase& rhs);

  // Get state
  BOUNDARY_STATE State() const;
  void State(BOUNDARY_STATE state);

  // Manipulate by hand ...
  bool Set(const geo::IPoint& ptMin, const geo::IPoint& ptMax);
  bool Set(const geo::IPoint& ptMid, double dAzimuth, double dLength, double dWidth, double dMinDepth, double dMaxDepth);
  void Min(const geo::IPoint& ptMin);
  void Max(const geo::IPoint& ptMax);
  void CorrectMeshDepth(const double& dMin, const double& dMax);

  bool IsCube() const;

  // Manipulate by events ...
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // Interface of viewable node
  virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;
  const geo::IObject& DisplayList(int nIndex) const;
  int DisplayListSize() const;

  // Save and load
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  //	Define(const 
  virtual ~CBoundaryBase();

  // Top and bottom horizon
  virtual const CHorizonBase &GetTopHorizon() const { assert(false); CHorizonBase *pBogus = 0; return *pBogus; }
  virtual const CHorizonBase &GetBottomHorizon() const { assert(false); CHorizonBase *pBogus = 0; return *pBogus; }

  virtual std::vector<const geo::ISurface*> GetSideMeshSurfaces() const { assert(false); return std::vector<const geo::ISurface*>(); };

  // This function will return a vector of pointers with the items outside the new boundary defined by
  // rectangle in the x,y plane
  std::vector<CGraphNode*> ItemsOutsideBoundary(const geo::CRectangle &rectangle) const;

  // Interface elements
  virtual bool HasInterfaces() const;
  virtual int InterfaceNodeSize() const;
  virtual const geo::INode& InterfaceNode( int nIndex ) const;

  bool CanAssignElementSet(const IElementSet& elset) const;
  void AssignElementSet(IElementSet& elset);

  virtual bool CanRotate() const;

  ACCEPT_GEOMECMODELVISITORS(VisitBoundaryBase);

protected:
  virtual bool OnSet(const TMinMax& minmax);
  virtual bool OnSet(const TRotated& rotated);
  virtual bool InvalidateMeshOnSet() const { return true; }

private:
  geo::CBox* CreateRotatedBox(const TRotated& rotated) const;
  bool OnNewBox(geo::CBox* pNewBox);

private:
  // selected state
  BOUNDARY_STATE m_bsState;

  // resulting box
  geo::CBox* m_pBox;

  // USER_DEFINED parameters
  geo::CPoint m_ptMin;
  geo::CPoint m_ptMax;

  // ROTATED parameters
  geo::CPoint m_ptRotation;
  double m_dAzimuth;
  double m_dLength;
  double m_dWidth;
};

bool operator==(const geo::CBox& lhs, const geo::CBox& rhs);

typedef CGraphEntryTemp<CBoundaryBase> TBoundaryEntry;

#endif // !defined(AFX_BOUNDARYBASE_H__A839AC62_0920_46B6_AAFE_1FB5703ACD21__INCLUDED_)
