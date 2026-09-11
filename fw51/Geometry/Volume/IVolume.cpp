/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IVolume.cpp: implementation of the IVolume class.
//
//////////////////////////////////////////////////////////////////////
#include "IVolume.h"
#include "BucketKernel.h"
#include "IBody.h"
#include "IProgressBase.h"
#include "PolyLine.h"
#include "Polygon.h"
#include "dimple.h"
#include <cmath>
#include <map>

#include <tbb/spin_rw_mutex.h>

namespace {

tbb::spin_rw_mutex myGlobalIVolumeMutex[16];

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

IVolume::IVolume() {}

IVolume::~IVolume() {}

const IFace &IVolume::EdgeFace(int nIndex) const {
  if (m_vcEdgeFace.size() == 0)
    GenerateEdgeFaces();

  return *m_vcEdgeFace[nIndex];
}

int IVolume::EdgeFaceSize() const {
  if (m_vcEdgeFace.size() == 0)
    GenerateEdgeFaces();

  return (int)m_vcEdgeFace.size();
}

void IVolume::GenerateEdgeFaces() const {
  int index = (intptr_t)this >> 6 & 0xf;

  tbb::spin_rw_mutex::scoped_lock lock(myGlobalIVolumeMutex[index]);

  if (m_vcEdgeFace.size() != 0)
    return; // some other thread beat us to it

  typedef std::set<int> TFace;
  typedef std::map<TFace, std::pair<const IFace *, int>> TFaceMap;
  TFaceMap mpFace;
  // Walk over bodies
  for (int i = 0; i < ElementSize(); i++) {
    const IBody *pBody = dynamic_cast<const IBody *>(&Element(i));
    if (pBody) {
      // Walk over body faces
      for (int j = 0; j < pBody->NrOfFaces(); j++) {
        TFace face_index;
        const IFace &face = pBody->Face(j);
        std::vector<int> vcIndices = pBody->FaceNodeIndices(j);

        for (int k = 0; k < face.NrOfPoints(); k++) {
          face_index.insert(pBody->PointIndex(vcIndices[k]));
        }
        std::pair<TFaceMap::iterator, bool> ret =
            mpFace.insert(TFaceMap::value_type(face_index, std::pair<const IFace *, int>(&face, 1)));
        if (!ret.second)
          ret.first->second.second++;
      }
    }
  }

  // Walk over face map and push_back edge face in a map
  for (TFaceMap::iterator it = mpFace.begin(); it != mpFace.end(); it++) {
    if (it->second.second == 1)
      m_vcEdgeFace.push_back(it->second.first);
  }
}

void IVolume::Intersection(const ILine &Line, std::set<CPoint> &PointSet) const {
  for (int i = 0; i < BodySize(); i++) {
    Body(i).Intersection(Line, PointSet);
  }
}

void IVolume::Intersection(const IPolyLine &Line, std::set<CPoint> &PointSet) const {
  for (int i = 0; i < Line.LineSize(); i++) {
    Intersection(Line.Line(i), PointSet);
  }
}

/*!
  Returns the intersection points with the line on the skin of the volume.
*/
void IVolume::IntersectionWithEdgeFaces(const ILine &Line, std::set<CPoint> &PointSet) const {
  typedef std::set<int> TPointSet;                  // Represents face points of the body face.
  typedef std::pair<const geo::IFace *, int> TFace; // Represents the face and number of faces counted.
  typedef std::map<TPointSet, TFace> TFaceMap;
  TFaceMap mpFace;
  CBucketKernel::TBucket bucket = BucketKernel().Buckets(Line);
  CBucketKernel::TBucket::const_iterator it;
  for (it = bucket.begin(); it != bucket.end(); it++) {
    // Walk over the bodies in the bucket
    const IBody *pBody = dynamic_cast<const IBody *>(&Element(*it));
    if (pBody) {
      // Walk over body faces
      for (int j = 0; j < pBody->NrOfFaces(); j++) {
        TPointSet face_points;
        const IFace &face = pBody->Face(j);
        for (int k = 0; k < face.NrOfNodes(); k++) {
          VERIFY(face_points.insert(face.Node(k).Index()).second);
        }

        // Insert in the map
        TFace face_pair(&face, 0);
        TFaceMap::iterator ret = mpFace.insert(TFaceMap::value_type(face_points, face_pair)).first;
        ret->second.second++;
      }
    }
  }

  // Intersect with skin and line
  for (TFaceMap::iterator i = mpFace.begin(); i != mpFace.end(); i++) {
    if (i->second.second == 1) {
      // Skin face
      const IFace &face = *i->second.first;
      CPoint point = face.Intersection(Line);
      if (!point.Empty())
        PointSet.insert(point);
    }
  }
}

/*!
 This function will calculate the resulting polygons of the intersected bodies
 by a plane. The progress indicator will make Candidate(<Plane>).size() steps.
*/
std::vector<CPolygon> IVolume::Intersection(const IPlane &Plane, IProgressBase *pProgress) const {
  // We create a list of relevant bodies for intersection just by intersecting layers of the bucket structure.

  // Mean direction
  std::set<int> stBody = Candidates(Plane);

  std::vector<CPolygon> vcRet;
  for (std::set<int>::const_iterator it = stBody.begin(); it != stBody.end(); it++) {
    CPolygon polygon = Body(*it).IntersectionConv(Plane);
    if (!polygon.Empty())
      vcRet.push_back(polygon);
    if (pProgress)
      pProgress->Step();
  }

  return vcRet;
}

// This function will calculate the resulting polygons and points of an intersection of a volme by a plane.
void IVolume::Intersection(const IPlane &Plane, CArray<CPolygon> &PolArr, std::set<CPoint> *PointSet /* = 0 */,
                           IProgressBase *pProgress /* = 0 */) const {
  CArray<CLine> PolyLineArr;
  std::set<CLine, ILine::CLineLess> TmpLineSet;  // Is passed to IBody::Intersection(), is cleared each time
  std::set<CLine, ILine::CLineLess> InLineSet;   // Is used to store all the inner lines (non polygon lines)
  std::set<CLine, ILine::CLineLess> PolyLineSet; // In the end this should only contain the lines making up the polygons
  std::set<CLine, ILine::CLineLess>::iterator it;
  std::pair<std::set<CLine, ILine::CLineLess>::iterator, bool> pair;

  // We create a list of relevant bodies for intersection just by intersecting layers of the bucket structure.

  // Mean direction
  std::set<int> stBody = Candidates(Plane);

  for (std::set<int>::const_iterator itt = stBody.begin(); itt != stBody.end(); itt++) {
    Body(*itt).Intersection(Plane, TmpLineSet, PointSet);

    if (TmpLineSet.size() < 2) { // plane crosses an 'edge' of a body or doesn't intersect at all.
      TmpLineSet.clear();
      continue;
    }
    for (it = TmpLineSet.begin(); it != TmpLineSet.end(); it++) {
      pair = PolyLineSet.insert(*it);
      if (!(pair.second)) // line was already present in LineSet, must be an inner line
        InLineSet.insert(*it);
      // LineSet.erase(pair.first);
    }
    TmpLineSet.clear();
    if (pProgress)
      pProgress->Step();
  }

  // Now, to remove all the inner lines (non polygon lines) from the PolyLineSet...
  for (it = InLineSet.begin(); it != InLineSet.end(); it++) {
    pair = PolyLineSet.insert(*it); // find the iterator
    PolyLineSet.erase(pair.first);  // remove the line
  }

  for (it = PolyLineSet.begin(); it != PolyLineSet.end();
       it++) { // create an array from the set because I need random acces in ExtractPolyLine()....
    CLine *l = const_cast<CLine *>(&(*it));
    PolyLineArr.PushBack(*l);
    //		PolyLineArr.PushBack(*it);
  }

  CArray<CPolyLine> PolyArr = ExtractPolyLine(PolyLineArr);

  for (size_t i = 0; i < PolyArr.Size(); i++) {
    CPolygon PGon(PolyArr.Object(i));
    PolArr.PushBack(PGon);
  }
}

// This function will create a number of polygons from the incoming array of lines.
// Make sure that the array has unique lines (put them in a std::set before you put them in an
// array). You also have to make sure that it is possible to construct a number of polygons from the lines.
// This function is called in Intersection(), take a look there to see how the lines are gathered.
CArray<CPolyLine> IVolume::ExtractPolyLine(CArray<CLine> &LineArr) const {
  typedef std::map<CPoint, int> TPtIndexMap; // map with all the points in the polylines and an index
                                             // describing how many lines are connected to each point.

  TPtIndexMap mpPtInd;
  CArray<CLine>::iterator LineIt;
  std::set<CPoint>::iterator PointIt;
  TPtIndexMap::iterator PtIndIt;

  // Build the map *************************************************
  for (LineIt = LineArr.begin(); LineIt != LineArr.end(); LineIt++) {
    CPoint PtFirst(LineIt->First());
    CPoint PtSecond(LineIt->Second());

    PtIndIt = mpPtInd.find(PtFirst);
    if (PtIndIt == mpPtInd.end()) {
      mpPtInd.insert(TPtIndexMap::value_type(PtFirst, 1));
    } else {
      PtIndIt->second += 1;
    }

    PtIndIt = mpPtInd.find(PtSecond);
    if (PtIndIt == mpPtInd.end()) {
      mpPtInd.insert(TPtIndexMap::value_type(PtSecond, 1));
    } else {
      PtIndIt->second += 1;
    }
  }
  // ****************************************************************

  int i = 0;
  int j = 0;
  CPolyLine PLine;
  bool bUseLastPoint;

  CArray<CPolyLine> TmpLineArr;
  CArray<CPolyLine> PLineArr;

  // Extract the "obvious" polygons *********************************
  while (LineArr.Size() > 0) {
    bUseLastPoint = false;

    if (j == 0) {
      PLine.PushBack(LineArr.Object(j).First());
      PLine.PushBack(LineArr.Object(j).Second());
      LineArr.Remove(j);
      j++;
      continue;
    }

    if (mpPtInd[PLine.LastPoint()] < 3)
      bUseLastPoint = true;
    else if (!(mpPtInd[PLine.FirstPoint()] < 3)) {
      TmpLineArr.PushBack(PLine);
      PLine.RemoveAll();
      j = 0;
      continue;
    }

    CPoint ptSample;
    ptSample = (bUseLastPoint) ? PLine.LastPoint() : PLine.FirstPoint();

    if (LineArr.Object(i).First() == ptSample) {
      bool ptPresent;
      ptPresent =
          (bUseLastPoint) ? PLine.PushBack(LineArr.Object(i).Second()) : PLine.PushFront(LineArr.Object(i).Second());
      if (!ptPresent) { // apparently the second point is also already present in the polyline -> the line must be
                        // closed, we found a polygon!
        PLineArr.PushBack(PLine);
        PLine.RemoveAll();
        j = 0;
      }
      LineArr.Remove(i);
      i = 0;
      continue;
    }

    if (LineArr.Object(i).Second() == ptSample) {
      bool ptPresent;
      ptPresent =
          (bUseLastPoint) ? PLine.PushBack(LineArr.Object(i).First()) : PLine.PushFront(LineArr.Object(i).First());
      if (!ptPresent) { // apparently the first point is also already present in the polyline -> the line must be
                        // closed, we found a polygon!
        PLineArr.PushBack(PLine);
        PLine.RemoveAll();
        j = 0;
      }
      LineArr.Remove(i);
      i = 0;
      continue;
    }

    i++;
  }
  // *******************************************************************

  PLine.RemoveAll();
  i = 0;
  j = 0;
  int k;

  // Extract the remaining polygons ***********************************
  // The polylines left in TmpLineArr should create the remaining polygons
  while (TmpLineArr.Size() > 0) {
    if (j == 0) {
      PLine = TmpLineArr.Object(j);
      TmpLineArr.Remove(j);
      j++;
      continue;
    }

    if (TmpLineArr.Object(i).LastPoint() == PLine.LastPoint()) {
      int NrPoints = TmpLineArr.Object(i).PointSize();
      for (k = NrPoints - 2; k >= 0; k--) // don't use the last point, is already present...
      {
        if (!(PLine.PushBack(TmpLineArr.Object(i).Point(k)))) { // closed a polyline with another
          assert(k == 0);
          PLineArr.PushBack(PLine);
          PLine.RemoveAll();
          j = 0;
        }
      }
      TmpLineArr.Remove(i);
      i = 0;
      continue;
    }
    if (TmpLineArr.Object(i).FirstPoint() == PLine.LastPoint()) {
      int NrPoints = TmpLineArr.Object(i).PointSize();
      for (k = 1; k < NrPoints; k++) {
        if ((!PLine.PushBack(TmpLineArr.Object(i).Point(k)))) {
          assert(k == NrPoints - 1);
          PLineArr.PushBack(PLine);
          PLine.RemoveAll();
          j = 0;
        }
      }
      TmpLineArr.Remove(i);
      i = 0;
      continue;
    }
    i++;
  }
  // ******************************************************************

  return PLineArr;
}

const IElement &IVolume::Element(int nIndex) const { return Body(nIndex); }

int IVolume::ElementSize() const { return BodySize(); }

std::vector<const IBody *> IVolume::PointInVolume(const IPoint &point) const {
  std::vector<const IBody *> vcRet;
  std::vector<int> vcIndex = ElementsAt(point);

  for (size_t i = 0; i < vcIndex.size(); i++) {
    vcRet.push_back(&Body(vcIndex[i]));
  }

  return vcRet;
}

void IVolume::InvalidateCache() { m_vcEdgeFace.clear(); }

bool IVolume::Contains(const IPoint &point, bool bIncludeEdge) const {
  if (!InBoundingBox(point))
    return false;

  if (!bIncludeEdge) {
    for (int i = 0; i < EdgeFaceSize(); i++) {
      if (EdgeFace(i).Contains(point, true))
        return false;
    }
  }

  return IElementSet::Contains(point, true);
}

} // namespace geo
