/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ISurface.cpp: implementation of the ISurface class.
//
//////////////////////////////////////////////////////////////////////

#include "ISurface.h"
#include "Box.h"
#include "BucketKernel.h"
#include "IFace.h"
#include "IProgressBase.h"
#include "Line.h"
#include "NearestNeighbour.h"
#include "PolyLine.h"
#include "Vector.h"
#include "dimple.h"

#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

ISurface::ISurface() {}

ISurface::~ISurface() { InvalidateCache(); }

const IElement &ISurface::Element(int nIndex) const { return Face(nIndex); }

int ISurface::ElementSize() const { return FaceSize(); }

CVector const ISurface::AverageNormal() const {
  CVector AverageNormal(0, 0, 0);

  for (int i = 0; i < FaceSize(); i++) {
    AverageNormal.X(AverageNormal.X() + Face(i).Normal().X());
    AverageNormal.Y(AverageNormal.Y() + Face(i).Normal().Y());
    AverageNormal.Z(AverageNormal.Z() + Face(i).Normal().Z());
  }

  return AverageNormal;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Following function was implemented in order to use smooth shading in OpenGL. It works and is faster than previous
// attempts but still takes a lot of time. The point passed has to be a node in the surface. Returns the average normal
// in a node.
const CVector ISurface::AverageNormalInPoint(const IPoint &p) const {
  CPtrArray<IFace> facearr = FacesAtNode(p);
  CVector normal(0.0, 0.0, 0.0);

  CPtrArray<IFace>::const_iterator it;

  // we use a set for the normals, so equal normals in multiple faces are not weighted too heavy
  std::set<CVector> stVectors; // uses ICoordinate less operator

  for (it = facearr.begin(); it != facearr.end(); it++)
    stVectors.insert((*it)->NormalInPoint(p).UnitVector());

  for (std::set<CVector>::iterator itv = stVectors.begin(); itv != stVectors.end(); itv++)
    normal += *itv;

  if (normal.Length() == 0.0)
    return geo::CVector::Xaxis;

  return normal.UnitVector();
}

void ISurface::AssertValid() const { assert(Empty() || PointSize() != 0); }

void ISurface::InsertPointsIntoMapAndSet(TPointPairSet &stPointPairs, TPointMap &mpFaces, const IPoint &pt1,
                                         const IPoint &pt2, const IFace &face) const {
  assert(!pt1.Empty());
  assert(!pt2.Empty());
  CPointPair pp(&pt1, &pt2);
  TPointPairSet::iterator itset = stPointPairs.insert(pp).first;

  CPointPair *xpp = const_cast<CPointPair *>(&(*itset));
  std::vector<const IFace *> v;
  TPointMap::value_type p(xpp, v);
  TPointMap::iterator itmap = mpFaces.insert(p).first;
  //  TPointMap::iterator itmap = mpFaces.insert(std::make_pair(&(*itset), std::vector<const IFace*>())).first;

  itmap->second.push_back(&face);
}

void ISurface::CreateConnectivityPointMapAndSet(TPointPairSet &stPointPairs, TPointMap &mpFaces) const {
  int sz = FaceSize();

  for (int i = 0; i < sz; i++) {
    const IFace &face = Face(i);
    int ptsz = face.NrOfPoints();

    for (int j = 0; j < ptsz - 1; j++) {
      const IPoint &pt1 = face.Point(j);
      const IPoint &pt2 = face.Point(j + 1);
      InsertPointsIntoMapAndSet(stPointPairs, mpFaces, pt1, pt2, face);
    }

    // and the last and the first combined
    const IPoint &pt1 = face.Point(ptsz - 1);
    const IPoint &pt2 = face.Point(0);
    InsertPointsIntoMapAndSet(stPointPairs, mpFaces, pt1, pt2, face);
  }
}

const CArray<CLine> ISurface::Edge() const {
  TPointPairSet stPointPairs;
  TPointMap mpFaces;

  CreateConnectivityPointMapAndSet(stPointPairs, mpFaces);

  CArray<CLine> ret;

  for (TPointMap::iterator it = mpFaces.begin(); it != mpFaces.end(); it++) {
    std::vector<const IFace *> &vcFaces = it->second;
    assert(vcFaces.size() == 1 || vcFaces.size() == 2);
    if (vcFaces.size() == 1) {
      CLine l((*it->first).First(), (*it->first).Second());
      ret.PushBack(l);
    }
  }

  return ret;
}

CPolyLine ISurface::SortedEdge() const {
  const CArray<CLine> arLines = Edge();
  assert(arLines.Size() > 1);

  std::set<const ILine *> stLines;
  for (size_t i = 1; i < arLines.Size(); i++)
    stLines.insert(&arLines.Object(i));

  CPolyLine ret;
  ret.PushBack(arLines.Object(0).First());
  const IPoint *pLast = &arLines.Object(0).Second();
  ret.PushBack(*pLast);

  while (stLines.size() > 1) // don't do last line, so beginpoint != endpoint
  {
    bool bFound = false;
    for (std::set<const ILine *>::iterator it = stLines.begin(); it != stLines.end(); it++) {
      if ((*it)->First() == *pLast) {
        pLast = &(*it)->Second();
        bFound = true;
      }
      if ((*it)->Second() == *pLast) {
        pLast = &(*it)->First();
        bFound = true;
      }

      if (bFound) {
        ret.PushBack(*pLast);
        stLines.erase(it);
        break;
      }
    }

    assert(bFound);
  }

  assert(arLines.Size() == ret.PointSize());
  return ret;
}

namespace {

struct cmpUnitVector {
  static double epsilon;

  bool operator()(const CVector &lhs, const CVector &rhs) const {
    double dot = std::abs(lhs.DotProduct(rhs) - 1);

    if (dot > epsilon)
      return lhs < rhs;

    return false;
  }
};

double cmpUnitVector::epsilon = 0.0001;

} // namespace

// sorted edge nodes, actual pointers to the IPoints in the surface
std::vector<std::vector<const IPoint *>> ISurface::SortedEdgePoints(bool bRetry, double dEpsilon) const {
  TPointPairSet stPointPairs;
  TPointMap mpFaces;

  typedef std::map<const IPoint *, std::vector<const IPoint *>> TEdgePointMap;
  TEdgePointMap mpEdgePoints;

  CreateConnectivityPointMapAndSet(stPointPairs, mpFaces);

  for (TPointMap::iterator itfaces = mpFaces.begin(); itfaces != mpFaces.end(); itfaces++) {
    std::vector<const IFace *> &vcFaces = itfaces->second;
    assert(vcFaces.size() == 1 || vcFaces.size() == 2);
    if (vcFaces.size() == 1) {
      // this pair of points lies on the edge
      CPointPair *pPair = itfaces->first;

      TEdgePointMap::iterator it =
          mpEdgePoints.insert(std::make_pair(&pPair->First(), std::vector<const IPoint *>())).first;
      it->second.push_back(&pPair->Second());

      it = mpEdgePoints.insert(std::make_pair(&pPair->Second(), std::vector<const IPoint *>())).first;
      it->second.push_back(&pPair->First());
    }
  }

  if (bRetry) // If we failed to get any edge, this may be caused by mesh region touching the edge; we filter out points
              // that have edges in the exact same direction
  {
    cmpUnitVector::epsilon = dEpsilon;

    typedef std::map<CVector, std::vector<const IPoint *>, cmpUnitVector> TSameEdge;

    TEdgePointMap::iterator itTyings = mpEdgePoints.begin();
    while (itTyings != mpEdgePoints.end()) {
      if (itTyings->second.size() == 4) {
        TSameEdge mpSameEdge;

        for (int i = 0; i < itTyings->second.size(); ++i) {
          CVector p(*itTyings->first, *itTyings->second[i]);
          TSameEdge::iterator v =
              mpSameEdge.insert(std::make_pair(p.UnitVector(), std::vector<const IPoint *>())).first;
          v->second.push_back(itTyings->second[i]);
        }

        for (std::map<CVector, std::vector<const IPoint *>>::iterator v = mpSameEdge.begin(); v != mpSameEdge.end();
             ++v) {
          if (v->second.size() == 2) {
            itTyings->second.erase(std::find(itTyings->second.begin(), itTyings->second.end(), v->second[0]));
            itTyings->second.erase(std::find(itTyings->second.begin(), itTyings->second.end(), v->second[1]));
          }
        }

        if (itTyings->second.size() == 0)
          mpEdgePoints.erase(itTyings++);
        else
          ++itTyings;
      } else
        ++itTyings;
    }
  }

  // any point with more than two neighbours is not part of the edge
  // this happens in cases where two faces are adjacent to one (using tyings)
  //
  // +---+---+
  // |   |   |
  // +---+---+
  // |       |
  // |       |
  // +-------+
  //
  // these points are removed from the map
  TEdgePointMap::iterator it1 = mpEdgePoints.begin();
  while (it1 != mpEdgePoints.end()) {
    if (it1->second.size() > 2) {
      for (int i = 0; i < it1->second.size(); ++i) {
        // find and erase all (subsequently) connected points with 1 or 2 connections
        TEdgePointMap::iterator itn = mpEdgePoints.find(it1->second[i]);
        while (itn != mpEdgePoints.end()) {
          if (itn->second.size() == 1 || itn->second.size() == 2) {
            // fetch the vector with connected points
            std::vector<const IPoint *> vcPoints = itn->second;
            // use point 0 unless we just handled it
            const IPoint *p = vcPoints[0];
            if (vcPoints.size() == 2 && p == itn->first)
              p = vcPoints[1];
            // erase the current point and move on to the next
            mpEdgePoints.erase(itn);
            itn = mpEdgePoints.find(p);
          } else
            itn = mpEdgePoints.end(); // terminate loop
        }
      }

      // all connected points have been processed
      mpEdgePoints.erase(it1++);
    } else
      ++it1;
  }

  std::vector<std::vector<const IPoint *>> ret;

  while (mpEdgePoints.size() > 1) {
    // now mpEdgePoints contains links between all edge points and their 2 neighbours
    // make a trip around the surface to find all nodes in the right order
    std::vector<const IPoint *> vcRing;

    assert(mpEdgePoints.size() > 1);
    TEdgePointMap::iterator it2 = mpEdgePoints.begin();
    vcRing.push_back(it2->first);

    const IPoint *prev;
    int iNext = 0;

    bool bValidRing = true;

    do {
      prev = it2->first;
      it2 = mpEdgePoints.find(it2->second[iNext]);
      if (it2 == mpEdgePoints.end() || vcRing.size() > mpEdgePoints.size()) {
        bValidRing = false;
        break;
      }

      vcRing.push_back(it2->first);
      assert(it2 != mpEdgePoints.end());

      assert(it2->second[0] == prev || it2->second[1] == prev);
      if (it2->second[0] == prev)
        iNext = 1;
      else
        iNext = 0;
    } while (it2->second[iNext] != vcRing[0]);

    for (int i = 0; i < vcRing.size(); i++)
      mpEdgePoints.erase(vcRing[i]);

    if (bValidRing)
      ret.push_back(vcRing);
  }

  return ret;
}

bool ISurface::LocateRingIndex(const IPoint &point, std::set<const IPoint *> &stEncountered,
                               std::map<const IPoint *, int> &ringmap) const {
  // are we there?
  std::map<const IPoint *, int>::iterator itRing = ringmap.find(&point);
  if (itRing != ringmap.end()) {
    // put all encountered points into the map and bail out
    std::set<const IPoint *>::iterator it;
    for (it = stEncountered.begin(); it != stEncountered.end(); ++it)
      ringmap.insert(std::map<const IPoint *, int>::value_type(*it, itRing->second));
    return true;
  }

  if (!stEncountered.insert(&point).second)
    return false; // already processed

  // figure out a next point from the connectivity
  std::vector<int> vcElements = ElementsAt(point);
  assert(!vcElements.empty());
  int i;
  for (i = 0; i < vcElements.size(); ++i) {
    const IElement &e = Element(vcElements[i]);
    int n;
    for (n = 0; n < e.NrOfPoints(); ++n) {
      const IPoint &pt = e.Point(n);

      // did we not process this one already?
      if (stEncountered.find(&pt) == stEncountered.end()) {
        if (LocateRingIndex(pt, stEncountered, ringmap))
          return true; // found and done
      }
    }
  }

  return false;
}

// if a surface consists of several sub-surfaces (i.e. separate parts)
// you can get a map saying which point lies in which part
// first call SortedEdgePoints() and give the result of that to this function
// the returned map contains the full set of points mapped to the index
// of the outer vector returned by SortedEdgePoints()
std::map<const IPoint *, int> ISurface::SubSurfaceIndices(std::vector<std::vector<const IPoint *>> &edgepoints) const {
  int i;
  std::map<const IPoint *, int> ret;

  if (edgepoints.empty())
    return ret;

  // first check if there is only one part, because all points belong to that then
  if (edgepoints.size() == 1) {
    for (i = 0; i < PointSize(); i++) {
      VERIFY(ret.insert(std::map<const IPoint *, int>::value_type(&Point(i), 0)).second);
    }
    return ret;
  }

  // all points in the vector can be put in the return map already
  // because we know where they are
  for (i = 0; i < edgepoints.size(); i++) {
    int j;
    for (j = 0; j < edgepoints[i].size(); j++)
      VERIFY(ret.insert(std::map<const IPoint *, int>::value_type(edgepoints[i][j], i)).second);
  }

  // now walk over all points in the surface
  for (i = 0; i < PointSize(); i++) {
    // are we ready?
    if (ret.size() == PointSize())
      break; // all points have been added to the map already

    // walk over the attached elements to find an edge node

    // a set to store all points we encounter
    std::set<const IPoint *> stEncountered;

    VERIFY(LocateRingIndex(Point(i), stEncountered, ret));
  }

  assert(ret.size() == PointSize());

  return ret;
}

// CPointPair implementation
ISurface::CPointPair::CPointPair(const IPoint *p1, const IPoint *p2) {
  // pointers may not be equal
  assert(p1 != p2);
  // smallest pointer
  m_p1 = (p1 < p2 ? p1 : p2);
  // biggest pointer
  m_p2 = (p1 < p2 ? p2 : p1);
}

ISurface::CPointPair::~CPointPair() {}

bool ISurface::CPointPair::operator<(const CPointPair &rhs) const {
  if (m_p1 < rhs.m_p1)
    return true;
  if (m_p1 > rhs.m_p1)
    return false;

  return (m_p2 < rhs.m_p2);
}

const IPoint &ISurface::CPointPair::First() const { return *m_p1; }

const IPoint &ISurface::CPointPair::Second() const { return *m_p2; }

void ISurface::InvalidateCache() {
  ClearEdgeNearestNeighbours();

  IElementSet::InvalidateCache();
}

void ISurface::ClearEdgeNearestNeighbours() const {
  int i;
  for (i = 0; i < m_vcEdgeNearestNeighbour.size(); i++)
    delete m_vcEdgeNearestNeighbour[i];

  m_vcEdgeNearestNeighbour.clear();
}

void ISurface::CreateEdgeNearestNeighbours(std::vector<std::vector<const IPoint *>> &vcEdgePoints) const {
  assert(m_vcEdgeNearestNeighbour.empty());
  m_vcEdgeNearestNeighbour.resize(vcEdgePoints.size());

  int i;

  for (i = 0; i < vcEdgePoints.size(); i++) {
    std::vector<const IPoint *> &vcPoints = vcEdgePoints[i];
    m_vcEdgeNearestNeighbour[i] = new CNearestNeighbour(0.5, (int)vcPoints.size());

    // insert the points
    int j;
    for (j = 0; j < vcPoints.size(); j++)
      m_vcEdgeNearestNeighbour[i]->Object(j, const_cast<IPoint &>(*vcPoints[j]));
  }
}

CValue ISurface::InterpolateZ(const IPoint &point, bool bExtraPolate) const {
  // Find the right bucket
  //  int x = BucketKernel().BucketX(point.X());
  //  int y = BucketKernel().BucketY(point.Y());

  // Search through buckets
  CSetExpandCallBack<int> call_back;
  BucketKernel().ExpandXY(call_back, point, 0);
  CSetExpandCallBack<int>::TBucket::const_iterator it;
  for (it = call_back.Bucket().begin(); it != call_back.Bucket().end(); it++) {
    const IFace &face = Face(*it);

    if (face.ContainsInXYPlane(point, true)) {
      if (fabs(face.Normal().Z()) > 1e-8) {
        std::vector<CValue> vcZ(face.NrOfPoints());
        for (int i = 0; i < face.NrOfPoints(); i++)
          vcZ[i] = face.Point(i).Z();
        CValue zPoint = face.InterpolateValue(point, vcZ);
        assert(zPoint.Valid());
        return zPoint;
      }
    }
  }

  if ((ElementSize() == 0) || !bExtraPolate)
    return CValue();

  // create the nearest neighbour objects
  if (!m_vcEdgeNearestNeighbour.size()) {
    // retrieve all edge points, because they are the only ones we will use
    // for the extrapolation
    std::vector<std::vector<const IPoint *>> vcEdgePoints = SortedEdgePoints();
    CreateEdgeNearestNeighbours(vcEdgePoints);
    assert(m_vcEdgeNearestNeighbour.size() == vcEdgePoints.size());
  }

  // a vector with the first and second nearest points per ring
  std::vector<std::pair<const IPoint *, const IPoint *>> vcNearestPoints(m_vcEdgeNearestNeighbour.size());

  int i;
  for (i = 0; i < m_vcEdgeNearestNeighbour.size(); i++) {
    CNearestNeighbour::CPointVec vcNearest = m_vcEdgeNearestNeighbour[i]->NearestNeighbourXY(point, 2);
    assert(vcNearest.size() == 2);

    vcNearestPoints[i].first = vcNearest[0];
    vcNearestPoints[i].second = vcNearest[1];
  }

  // find the nearest ring, only look in 2D
  double dNearest = 0.;
  int iNearest = -1;

  for (i = 0; i < vcNearestPoints.size(); i++) {
    double dx = vcNearestPoints[i].first->X() - point.X();
    double dy = vcNearestPoints[i].first->Y() - point.Y();
    double dist = dx * dx + dy * dy;

    if (!i || dist < dNearest) {
      dNearest = dist;
      iNearest = i;
    }
  }

  assert(iNearest >= 0);

  const IPoint &first_point = *vcNearestPoints[iNearest].first;
  const IPoint &second_point = *vcNearestPoints[iNearest].second;

  geo::CVector v1(point.X() - first_point.X(), point.Y() - first_point.Y(), 0);
  geo::CVector v2(second_point.X() - first_point.X(), second_point.Y() - first_point.Y(), 0);

  double y1 = v1.DotProduct(v2) / v2.SquareLength();
  if (y1 <= 0)
    return geo::CValue(first_point.Z());
  else if (y1 >= 1)
    return geo::CValue(second_point.Z());

  return geo::CValue((1 - y1) * first_point.Z() + y1 * second_point.Z());
}

const IPoint &ISurface::NearestXYPoint(const IPoint &point) const {
  // Find the right bucket
  int x = BucketKernel().BucketX(point.X());
  int y = BucketKernel().BucketY(point.Y());

  return *NearestXYPoints(x, y, point.X(), point.Y()).begin()->second;
}

// At least tree points are given
std::multimap<double, const IPoint *> ISurface::NearestXYPoints(int size_x, int size_y, const double &dX,
                                                                const double &dY) const {
  assert(ElementSize() > 0);
  // Extend bucket till we find elements. Note that this can never
  // result in an infinite loop because there are elements in the
  // elementset.
  int nExpand = 0;
  CSetExpandCallBack<int> call_back;
  while (call_back.Bucket().size() == 0) {
    BucketKernel().ExpandXY(call_back, size_x, size_y, nExpand);
    nExpand++;
  }

  // Get all the nodes in a set
  typedef std::set<const IPoint *, ICoordinate::CCoordinateLess> TNodeSet;
  TNodeSet stPoint;
  CSetExpandCallBack<int>::TBucket::const_iterator it;
  for (it = call_back.Bucket().begin(); it != call_back.Bucket().end(); it++) {
    for (int nNode = 0; nNode < Element(*it).NrOfPoints(); nNode++)
      stPoint.insert(&Element(*it).Point(nNode));
  }

  assert(stPoint.size() > 0);

  // Fill the map with square distances
  typedef std::multimap<double, const IPoint *> TDistanceMap;
  TDistanceMap mpRet;
  for (TNodeSet::iterator it_point = stPoint.begin(); it_point != stPoint.end(); it_point++) {
    // Determine the square distance between X and Y
    double dDeltaX = dX - (*it_point)->X();
    double dDeltaY = dY - (*it_point)->Y();
    double dDistance = dDeltaX * dDeltaX + dDeltaY * dDeltaY;
    mpRet.insert(TDistanceMap::value_type(dDistance, *it_point));
  }

  return mpRet;
}

/*!
  Intersection function emits CandiatesPlane().size() steps on the progress indicator.
*/
ISurface::TPolyLineVec ISurface::Intersection(const IPlane &plane, IProgressBase *pProgress) const {
  std::set<int> stElementIndex = Candidates(plane);
  std::vector<const IFace *> vcCandidates;

  std::set<int>::iterator it;
  for (it = stElementIndex.begin(); it != stElementIndex.end(); it++) {
    const IFace &face = Face(*it);
    if (face.Intersects(plane))
      vcCandidates.push_back(&face);
    else if (pProgress)
      pProgress->Step();
  }

  std::vector<CLine *> vcLines;

  // create a set of pointers to these lines, so we can create
  // all the polylines
  std::set<const ILine *> stLines;
  int i;
  for (i = 0; i < vcCandidates.size(); i++) {
    IFace::TLineVec vcFaceLines = vcCandidates[i]->Intersection(plane);
    assert(!vcFaceLines.empty());
    int j;
    for (j = 0; j < vcFaceLines.size(); j++) {
      vcLines.push_back(new CLine(vcFaceLines[j]));
      stLines.insert(vcLines[vcLines.size() - 1]);
    }
    if (pProgress)
      pProgress->Step();
  }

  // create all polylines
  TPolyLineVec vcRet;

  while (!stLines.empty()) {
    CPolyLine pl;
    pl.CreateFromLines(stLines);
    vcRet.push_back(pl);
  }

  for (i = 0; i < vcLines.size(); i++)
    delete vcLines[i];

  return vcRet;
}

void ISurface::FilterElementsInOrthoBox(const geo::IPoint &ptMin, const geo::IPoint &ptMax,
                                        TElementSet &elements_on_the_side, TElementSet &elements_strict_inside,
                                        TElementSet &elements_strict_outside) const {
  geo::CBox box(ptMin, ptMax);

  for (int i = 0; i < ElementSize(); i++) {
    const IElement &element = Element(i);
    if (!box.BoundingBoxOutside(element)) {

      if (!box.BBRhsInside(element)) {
        elements_on_the_side.insert(&element);

      } else {
        elements_strict_inside.insert(&element);
      }
    } else
      elements_strict_outside.insert(&element);
  }
}

} // namespace geo
