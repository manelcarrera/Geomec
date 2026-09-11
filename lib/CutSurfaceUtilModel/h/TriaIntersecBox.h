// TriaIntersecBox.h: interface for the CTriaIntersecBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIAINTERSECBOX_H__AB5C4BF2_10AD_45C3_9A9A_079BF3003ADD__INCLUDED_)
#define AFX_TRIAINTERSECBOX_H__AB5C4BF2_10AD_45C3_9A9A_079BF3003ADD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Box.h"
#include "TriSurface.h"

//***************** bounding box definitions for cutting of surfaces begin ***********************

// sides of a orthogonal box
enum SIDE_ID { SIDE_ID_FRONT = 0, SIDE_ID_BACK, SIDE_ID_LEFT, SIDE_ID_RIGHT, SIDE_ID_TOP, SIDE_ID_BOTTOM };

struct STNamedSurface {
  STNamedSurface() {
    Name = "empty";
    Surface = 0;
  }
  STNamedSurface(const std::string &name, const geo::ISurface *surface) : Name(name) {
    // Name		=	name;
    Surface = surface;
  }
  STNamedSurface(const STNamedSurface &rhs) {
    Name = rhs.Name;
    Surface = rhs.Surface;
  }
  bool operator<(const STNamedSurface & /*rhs*/) const { return true; }
  bool operator==(const STNamedSurface & /*rhs*/) const { return true; }

  std::string Name;
  const geo::ISurface *Surface;
};

// these indices refers to an segnment on the surface that are on a side of the box
class CSegmentOnSide {
public:
  CSegmentOnSide(const geo::IPoint *ap1, const geo::IPoint *ap2) : p1(ap1), p2(ap2) {
    assert(p1 != p2);
    assert(*p1 != *p2);
  }
  CSegmentOnSide(const CSegmentOnSide &rhs) {
    p1 = rhs.p1;
    p2 = rhs.p2;
  }

  const geo::IPoint *p1;
  const geo::IPoint *p2;
};

// definition of ribs of an ortho box.
// a rib is represented by its two sideID
typedef std::pair<SIDE_ID, SIDE_ID> TRib;
typedef std::vector<TRib> TRibs;

// get the ribs of a ortho box
const TRibs &GetRibs();

TRib GetRib(SIDE_ID SideID1, SIDE_ID SideID2);

// the line represents the rib
geo::CLine GetLine(const geo::IBox &box, TRib rib);

// the plane represents the side
geo::CPlane GetPlane(const geo::IBox &box, SIDE_ID SideID);

//***************** bounding box definitions  end ***********************

class TriaSurfImpl;

// a class for storing intersection information with a Trisurface and a boundong box
class CTriaIntersecBox {
private:
  // for each rib on the box a vector of points is kept
  typedef std::map<TRib, std::vector<const geo::IPoint *>> TPointRibMap;

  // for each side on the box that is used a vector of CSegmentOnSide is kept
  typedef std::map<SIDE_ID, std::vector<CSegmentOnSide>> TSegmentMap;

  TRibs GetRibsOnSide(SIDE_ID SideID);

  void FilterSurfaceElements(const geo::ISurface &surf, geo::ISurface::TElementSet &elements_on_the_side,
                             geo::ISurface::TElementSet &elements_strict_inside,
                             geo::ISurface::TElementSet &elements_strict_outside);

public:
  friend class TriaSurfImpl; // implementation stuff..

  virtual ~CTriaIntersecBox();

  // constructor with bounding box
  CTriaIntersecBox(const geo::CBox &box) : m_box(box) {}

  // add a surface to cut ( every surface with 3 point faces )
  bool AddSurfaceToCut(const std::string &name, const geo::ISurface &surf);

  // get the generated surfaces
  // these surfaces are not deleted if this object is deleted!!!!!
  int CutSurfacesCount() const;
  const STNamedSurface &GetCutSurfaces(int index) const;
  const std::vector<STNamedSurface> &getCutSurfaces() const;

  // for each side a surface is created
  geo::CTriSurface *GetSideSurface(SIDE_ID) const;
  const std::map<SIDE_ID, geo::CTriSurface *> &getSideSurfaces() const;

  // sides of the bounding box to use (see AllSidesExeptTopAndBottom and AllSides)
  const std::vector<SIDE_ID> &GetSides() const;

  // set extra points per rib (user defined points)
  void SetExtraPointsOnRib(const TRib &rib, int nrOfPoints = 10);

  // helper to set extra points for all ribs
  void SetExtraPointsOnAllRibs(int nrOfPoints = 10);

  // get points per rib ( intersection points + userdefined)
  std::vector<const geo::IPoint *> GetPointsOnRib(const TRib &rib) const;

  // get segments per side
  const std::vector<CSegmentOnSide> &GetSegmentsOnSide(SIDE_ID id) const;

  // check for isolated edges after call GenerateSideSurfaces()
  typedef std::vector<std::pair<const geo::IPoint *, const geo::IPoint *>> TEdgeVector;

  bool LookForIsolatedEdges(geo::CPtrArray<geo::IPoint> &points, TEdgeVector &edges) const;

#if 0
  /////*************************** to be removed !!!! BEGIN ****************************************
        //temporary arrays for visualisation, debugging etc..
        std::map<std::string,geo::CPtrArray<geo::IObject>* > test_arrays;
        void AddToTestArray(const char* name , const geo::IObject* obj)
        {
          std::map<std::string,geo::CPtrArray<geo::IObject>* >::iterator it = test_arrays.find(std::string(name));

          if(it == test_arrays.end())
          {
            it = test_arrays.insert(std::make_pair(std::string(name) , new geo::CPtrArray<geo::IObject>)).first;
          }
          it->second->PushBack((geo::IObject&)*obj);
        }
  /////*************************** to be removed !!!! END ****************************************
#endif

  // helper to select all sides exept top and bottom
  // call this before adding surfaces
  void AllSidesExeptTopAndBottom();

  // helper to select all sides
  // call this before adding surfaces
  void AllSides();

  void GenerateSideSurfaces();

  const geo::CBox &Box() const;

  // just a helper function to fill a vector with points on a rib
  static void GetPointsOnRib(const geo::IBox &box, std::vector<const geo::IPoint *> &vec, const TRib &rib,
                             int nrOfPoints);

private:
  // helper funck.
  void GetIntersecSurfaceWithPlane(const geo::CPlane &plane, TriaSurfImpl &mimic);
  void GenerateSideSurface(SIDE_ID side);

  // get intersection points per rib
  const std::vector<const geo::IPoint *> &GetIntersecPointsOnRib(const TRib &rib) const;

  // generated surfaces, not deleted at destroy, user should dlete the generated surfaces!!!
  std::vector<STNamedSurface> m_CutSurfaces;
  std::map<SIDE_ID, geo::CTriSurface *> m_SideSurfaces;

  typedef std::map<TRib, std::vector<const geo::IPoint *>> TUserPointMap;

  // is allocated ( freed by destruction )
  TUserPointMap m_UserPointsOnRib;

  // sides to be used
  std::vector<SIDE_ID> m_Sides;

  // store points per rib
  // is allocated ( freed by destruction )
  TPointRibMap m_PointRibMap;

  // storing segments per side
  // is allocated ( freed by destruction )
  TSegmentMap m_SegmentMap;

  // bb definition
  geo::CBox m_box;
};

#endif // !defined(AFX_TRIAINTERSECBOX_H__AB5C4BF2_10AD_45C3_9A9A_079BF3003ADD__INCLUDED_)
