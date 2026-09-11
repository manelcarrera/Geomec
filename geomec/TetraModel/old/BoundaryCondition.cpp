// BoundaryCondition.cpp: implementation of the CBoundaryCondition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <cmath>
#include "geomec.h"

#include "PressureSupportNode.h"
#include "DisplacementSupportNode.h"
#include "BoundaryCondition.h"

#include "TetraModel.h"
#include "TetraBoundary.h"
#include "BaseEntryTypes.h"
#include "DepletionStage.h"

#include "BodyTriangle.h"
#include "mfStrainTensor.h"

#include "TetSurface.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundaryCondition::CBoundaryCondition(const geo::CTetSurface &tetsurf, const CTetraModel &model, const geo::IPoint &ptReference)
:	m_Surface(tetsurf), m_Model(model), m_ptReference(ptReference)
{
}

CBoundaryCondition::~CBoundaryCondition()
{
}

void CBoundaryCondition::AddSupports(TSupportMap &supportmap, TPressureMap &pressuremap)
{
  const CPressureSupportNode::CFaceSupportDef def =
    Boundary().PressureSupportNode().FaceSupportDef(CPressureSupportNode::CSurfaceDef(m_Surface));

  switch(def.SupportType())
  {
  case CPressureSupportNode::CFaceSupportDef::ST_NORMAL:
    ProcessNormal(supportmap);
    break;
  case CPressureSupportNode::CFaceSupportDef::ST_FULL:
    ProcessFull(supportmap);
    break;
  case CPressureSupportNode::CFaceSupportDef::ST_STRESS:
    ProcessStress(def, pressuremap);
    break;
  default:
    ASSERT(FALSE);
  }
}

const CTetraBoundary &CBoundaryCondition::Boundary()
{
  const CTetraBoundary *pBoundary = dynamic_cast<const CTetraBoundary *> (&m_Model.Boundary());
  assert(pBoundary);
  return *pBoundary;
}

void CBoundaryCondition::InsertSupport(TSupportMap &supportmap, const geo::IPoint &pt, CSupport &support,
                     const CDisplacementSupportNode::CNodalSupportDef &NodeDef,
                     const geo::IVector &vecReference)
{
  // Insert boundary displacements from the nodal support def
  const CDepletionStage *pStage = &m_Model.InitialDepletionStage();
  ASSERT(!pStage->Last());
  pStage = &pStage->Next();
  int nStage = 0;

  while(pStage)
  {
    const CDisplacementSupportNode::CNodalSupportDef::CSupportDef &supdef = NodeDef.SupportDef(*pStage);
    const CDisplacementSupportNode &node = Boundary().DisplacementSupportNode();
    switch(supdef.SupportType())
    {
    case CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_NONE:
      // do nothing
      break;
    case CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_LOCAL:
      support.Displacement(supdef.LocalVector(), nStage);
      break;
    case CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_GLOBAL:
      support.Displacement(node.GlobalTensor(*pStage), vecReference, node.GlobalTensorReferenceAzimuth(*pStage), nStage);
      break;
    default:
      ASSERT(FALSE);
    }

    if(pStage->Last()) pStage = 0;
    else pStage = &pStage->Next();
    nStage++;
  }

  TSupportMap::iterator it = supportmap.find(pt);
  if(it == supportmap.end())
  {
    supportmap.insert(std::make_pair(geo::CPoint(pt), support));
  }
  else
  {
    it->second = it->second + support;
  }
}

void CBoundaryCondition::InterpolateAndInsertEdgeNodes(TSupportMap &supportmap, TSupportMap &localmap, bool bFull)
{
  // interpolate the displacement vectors along the lengths of the edge lines
  // the values are available in the corner nodes and will be interpolated for nodes in between
  // finally, the values will be added to the supportmap
  ASSERT(!m_SortedEdge.Empty());
  ASSERT(!localmap.empty());

  int pointsz = m_SortedEdge.PointSize();

  for(int i = 0; i < pointsz; i++)
  {
    const geo::IPoint &pt = m_SortedEdge.Point(i);
    if(localmap.find(geo::CPoint(pt)) != localmap.end()) continue; // already available

    CSupport *pSupportNext = 0;
    CSupport *pSupportPrev = 0;
    // find next point with support definition
    int j = i;
    double dLenNext = 0;
    while(!pSupportNext)
    {
      int iNext = j + 1;
      if(iNext == pointsz) iNext = 0;
      const geo::IPoint &ptCurr = m_SortedEdge.Point(j);
      const geo::IPoint &ptNext = m_SortedEdge.Point(iNext);

      dLenNext += ptCurr.Distance(ptNext);
      TSupportMap::iterator it = localmap.find(geo::CPoint(ptNext));
      if(it != localmap.end())
      {
        // found the next known point
        pSupportNext = &it->second;
      }
      j++;
    }

    j = i;

    double dLenPrev = 0;
    while(!pSupportPrev)
    {
      int iPrev = j - 1;
      if(iPrev == 0) iPrev = pointsz - 1;
      const geo::IPoint &ptCurr = m_SortedEdge.Point(j);
      const geo::IPoint &ptPrev = m_SortedEdge.Point(iPrev);

      dLenPrev += ptCurr.Distance(ptPrev);
      TSupportMap::iterator it = localmap.find(geo::CPoint(ptPrev));
      if(it != localmap.end())
      {
        // found the previous known point
        pSupportPrev = &it->second;
      }

      j++;
    }

    const geo::CVector vecNormal = m_Surface.AverageNormalInPoint(pt);
    if(!bFull) supportmap.insert(std::make_pair(geo::CPoint(pt), CSupport(*pSupportPrev, dLenPrev, *pSupportNext, dLenNext, bFull, vecNormal)));
    else supportmap.insert(std::make_pair(geo::CPoint(pt), CSupport(*pSupportPrev, dLenPrev, *pSupportNext, dLenNext, bFull, vecNormal)));
  }
}

double CBoundaryCondition::Interpolate(double dVal1, double dFac1, double dVal2, double dFac2)
{
  double dSum = dFac1 + dFac2;
  ASSERT(fabs(dSum) > EPS);
  return dVal1 + dFac1 / dSum * (dVal2 - dVal1);
}

geo::CVector CBoundaryCondition::InterpolateVector(const geo::IPoint &pt, const geo::IVector &vec1, const geo::IPoint &pt1, const geo::IVector &vec2, const geo::IPoint &pt2)
{
  double dDist1 = pt.Distance(pt1);
  double dDist2 = pt.Distance(pt2);

  return geo::CVector(
    Interpolate(vec1.X(), dDist1, vec2.X(), dDist2),
    Interpolate(vec1.Y(), dDist1, vec2.Y(), dDist2),
    Interpolate(vec1.Z(), dDist1, vec2.Z(), dDist2));
}

CBoundaryCondition::TLinePointPairPair CBoundaryCondition::GetBestEdgePoints(const geo::IPoint &pt, const std::vector<TLinePointPair> &vcEdgePoints)
{
  // get all possible lines from each point to each point in the vector
  // pick the line with the smallest perpendicular distance to the point pt
  std::vector<TLinePointPairPair> vcPossibleLines;
  int iSmallest;
  double dSmallest;
  int iCurrent = 0;

  for(int i = 0; i < vcEdgePoints.size() - 1; i++)
  {
    const TLinePointPair &pr1 = vcEdgePoints[i];
    for(int j = i + 1; j < vcEdgePoints.size(); j++)
    {
      const TLinePointPair &pr2 = vcEdgePoints[j];
      geo::CLine l(pr1.second, pr2.second);
      if(vcPossibleLines.empty())
      {
        // initialize
        ASSERT(!iCurrent);
        iSmallest = 0;
        dSmallest = l.Distance(pt);
      }
      else
      {
        double dDist = l.Distance(pt);
        if(dDist < dSmallest)
        {
          dSmallest = dDist;
          iSmallest = iCurrent;
        }
      }

      vcPossibleLines.push_back(std::make_pair(pr1, pr2));
    }

    iCurrent++;
  }

  ASSERT(iSmallest >= 0);
  ASSERT(iSmallest < vcPossibleLines.size());
  return vcPossibleLines[iSmallest];
}

void CBoundaryCondition::InterpolateSurfaceNode(const geo::IPoint &pt,
  const std::vector<TLinePointPair> &vcEdgePoints, TSupportMap &supportmap)
{
  TLinePointPairPair prEdgePoints = GetBestEdgePoints(pt, vcEdgePoints);

  const CDepletionStage *pStage = &m_Model.InitialDepletionStage();
  ASSERT(!pStage->Last());
  pStage = &pStage->Next();
  int nStage = 0;
  std::vector<geo::CVector> vcDisplacements;

  while(pStage)
  {
    // first point, interpolate vector
    TSupportMap::iterator itfirst = supportmap.find(prEdgePoints.first.first.First());
    ASSERT(itfirst != supportmap.end());
    TSupportMap::iterator itsecond = supportmap.find(prEdgePoints.first.first.Second());
    ASSERT(itsecond != supportmap.end());
    // the displacement vector interpolated in the first edge point
    geo::CVector vecfirst = InterpolateVector(prEdgePoints.first.second, itfirst->second.Displacement(nStage), prEdgePoints.first.first.First(), itsecond->second.Displacement(nStage), prEdgePoints.first.first.Second());

    // second point, interpolate vector
    itfirst = supportmap.find(prEdgePoints.second.first.First());
    ASSERT(itfirst != supportmap.end());
    itsecond = supportmap.find(prEdgePoints.second.first.Second());
    ASSERT(itsecond != supportmap.end());
    // the displacement vector interpolated in the second edge point
    geo::CVector vecsecond = InterpolateVector(prEdgePoints.second.second, itfirst->second.Displacement(nStage), prEdgePoints.second.first.First(), itsecond->second.Displacement(nStage), prEdgePoints.second.first.Second());

    // the vector interpolated from the the two vectors
    vcDisplacements.push_back(InterpolateVector(pt, vecfirst, prEdgePoints.first.second, vecsecond, prEdgePoints.second.second));

    if(pStage->Last()) pStage = 0;
    else pStage = &pStage->Next();
    nStage++;
  }
}

void CBoundaryCondition::InterpolateSurfaceNodes(TSupportMap &supportmap, TSupportMap &localmap, bool bFull)
{
  // create a plane through reference point spanned by average normal and z- or x-axis
  geo::CVector vecNormal = m_Surface.AverageNormal();
  const geo::IVector *pOtherVec = &geo::CVector::Zaxis;
  if(vecNormal.IsParallel(*pOtherVec)) pOtherVec = &geo::CVector::Xaxis;
  ASSERT(!vecNormal.IsParallel(*pOtherVec));
  geo::CPlane plCheck(m_ptReference, vecNormal, *pOtherVec);

  // walk through the surface's nodes, skip the edge nodes (we already processed them)
  for(int i = 0; i < m_Surface.PointSize(); i++)
  {
    const geo::IPoint &pt = m_Surface.Point(i);
    if(localmap.find(pt) == localmap.end())
    {
      // walk through the sorted edge and find the intersection points with the plane
      // we are in a point not on the edge, so we should find at least two points
      std::vector<std::pair<geo::CLine, geo::CPoint> > vcPoints;
      int edgepointsz = m_SortedEdge.PointSize();
      for(int j = 0; j < edgepointsz - 1; j++)
      {
        geo::CLine l(m_SortedEdge.Point(j), m_SortedEdge.Point(j + 1));
        // try to intersect the plane with this line, including the first, but
        // excluding the second line's point, so each point is tried only once
        geo::CPoint pt = plCheck.RealIntersection(l, true, false);
        if(!pt.Empty()) vcPoints.push_back(std::make_pair(l, pt));
      }
      // and try the last line
      geo::CLine l(m_SortedEdge.Point(edgepointsz - 1), m_SortedEdge.Point(0));
      geo::CPoint ptint = plCheck.RealIntersection(l, true, false);
      if(!ptint.Empty()) vcPoints.push_back(std::make_pair(l, ptint));

      ASSERT(vcPoints.size() >= 2);

      InterpolateSurfaceNode(pt, vcPoints, supportmap);
    }
  }
}

void CBoundaryCondition::ProcessNormal(TSupportMap &supportmap)
{
  if(m_SortedEdge.Empty()) m_SortedEdge = m_Surface.SortedEdge();

  const CDisplacementSupportNode &node = Boundary().DisplacementSupportNode();
  TSupportMap localmap; // for this surface only, needed for quicker edge node interpolation

  for(int i = 0; i < m_SortedEdge.PointSize(); i++)
  {
    const geo::IPoint &pt = m_SortedEdge.Point(i);
    const CDisplacementSupportNode::CNodalSupportDef *pDef = node.NodalSupportDef(pt);
    if(pDef != 0)
    {
      const geo::IPoint &pt = m_SortedEdge.Point(i);
      geo::CVector vecNormal = m_Surface.AverageNormalInPoint(pt);
      CSupport sup(vecNormal);
      InsertSupport(supportmap, pt, sup, *pDef, geo::CVector(m_ptReference, pt));
      InsertSupport(localmap, pt, sup, *pDef, geo::CVector(m_ptReference, pt));
    }
  }

  InterpolateAndInsertEdgeNodes(supportmap, localmap, false);

  InterpolateSurfaceNodes(supportmap, localmap, false);
}

void CBoundaryCondition::ProcessFull(TSupportMap &supportmap)
{
  if(m_SortedEdge.Empty()) m_SortedEdge = m_Surface.SortedEdge();

  const CDisplacementSupportNode &node = Boundary().DisplacementSupportNode();
  TSupportMap localmap; // for this surface only, needed for quicker edge node interpolation

  for(int i = 0; i < m_SortedEdge.PointSize(); i++)
  {
    const geo::IPoint &pt = m_SortedEdge.Point(i);
    const CDisplacementSupportNode::CNodalSupportDef *pDef = node.NodalSupportDef(pt);
    if(pDef != 0)
    {
      int nStages = m_Model.NrOfDepletionStages() - 1;
      CSupport sup(nStages);
      InsertSupport(supportmap, pt, sup, *pDef, geo::CVector(m_ptReference, pt));
      InsertSupport(localmap, pt, sup, *pDef, geo::CVector(m_ptReference, pt));
    }
  }

  InterpolateAndInsertEdgeNodes(supportmap, localmap, true);

  InterpolateSurfaceNodes(supportmap, localmap, true);
}

void CBoundaryCondition::ProcessStress(const CPressureSupportNode::CFaceSupportDef &def, TPressureMap &pressuremap)
{
  int sz = m_Surface.FaceSize();
  int nStages = m_Model.NrOfDepletionStages() - 1;

  for(int i = 0; i < sz; i++)
  {
    const geo::CBodyTriangle *pFace = dynamic_cast<const geo::CBodyTriangle *> (&m_Surface.Face(i));
    ASSERT(pFace != 0);

    std::pair<TPressureMap::iterator, bool> prInsert = pressuremap.insert(std::make_pair(pFace, CPressure(nStages)));
    ASSERT(prInsert.second);

    const CDepletionStage *pStage = &m_Model.InitialDepletionStage();
    ASSERT(pStage->Last());
    pStage = &pStage->Next();
    int iStage = 0;

    while(pStage)
    {
      const CPressureSupportNode::CFaceSupportDef::CSupportDef &supdef = def.SupportDef(*pStage);
      CPressureSupportNode::CFaceSupportDef::CSupportDef::TPressureType type = supdef.PressureType();

      switch(type)
      {
      case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL:
        prInsert.first->second.Pressure(supdef.LocalVector(), iStage);
        break;
      case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_GLOBAL:
        prInsert.first->second.Pressure(Boundary().PressureSupportNode().GlobalTensor(*pStage), pFace->Normal(), iStage);
        break;
      case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_UNLOADED:
        // do nothing
        break;
      default:
        ASSERT(FALSE);
      }

      if(pStage->Last()) pStage = 0;
      else pStage = &pStage->Next();
      iStage++;
    }
  }
}

//// CSupport
CBoundaryCondition::CSupport::CSupport(int nStages)
:	m_bFull(true), m_vcDisplacements(nStages)
{
}

CBoundaryCondition::CSupport::CSupport(const geo::IVector &vecDir)
:	m_bFull(false)
{
  m_vcDirections.push_back(geo::CVector(vecDir));
}

CBoundaryCondition::CSupport::CSupport(const CSupport &first, int distfirst, const CSupport &second, int distsecond,
                     bool bFull, const geo::IVector &vecDir)
:	m_bFull(bFull),
  m_vcDisplacements(first.m_vcDisplacements.size())
{
  ASSERT(distfirst > EPS);
  ASSERT(distsecond > EPS);
  ASSERT(first.m_vcDisplacements.size() == second.m_vcDisplacements.size());

  if(!bFull) m_vcDirections.push_back(geo::CVector(vecDir));

  double factor = distfirst / distfirst + distsecond;

  for(int i = 0; i < first.m_vcDisplacements.size(); i++)
  {
    const geo::IVector &vecFirst = first.m_vcDisplacements[i];
    const geo::IVector &vecSecond = second.m_vcDisplacements[i];

    m_vcDisplacements[i] = geo::CVector(vecFirst.X() + factor * (vecSecond.X() - vecFirst.X()),
                      vecFirst.Y() + factor * (vecSecond.Y() - vecFirst.Y()),
                      vecFirst.Z() + factor * (vecSecond.Z() - vecFirst.Z()));
  }
}

CBoundaryCondition::CSupport::CSupport(const CSupport &rhs)
:	m_bFull(rhs.m_bFull), m_vcDisplacements(rhs.m_vcDisplacements), m_vcDirections(rhs.m_vcDirections)
{
}

CBoundaryCondition::CSupport::CSupport()
:	m_bFull(false)
{
}

// derive displacement vector from global strain tensor and reference vector
void CBoundaryCondition::CSupport::Displacement(const ITensor &GlobalTensor, const geo::IVector &vecReference, const double &dRefAzimuth, int nStage)
{
  ASSERT(m_bFull);
  ASSERT(nStage >= 0 && nStage < m_vcDisplacements.size());

  CStrainTensor rotatedtensor = CStrainTensor(GlobalTensor.RotateTensorDeg(geo::CVector::Zaxis, dRefAzimuth));
  geo::CVector rotatedvector(vecReference);
  rotatedvector.Rotate(geo::CVector::Zaxis, -dRefAzimuth);

  m_vcDisplacements[nStage] = geo::CVector(
    rotatedtensor.XX() * rotatedvector.X() + rotatedtensor.EngineerZX() * rotatedvector.Z() + rotatedtensor.EngineerXY() * rotatedvector.Y(),
    rotatedtensor.YY() * rotatedvector.Y() + rotatedtensor.EngineerYZ() * rotatedvector.Z(),
    rotatedtensor.ZZ() * rotatedvector.Z());

  m_vcDisplacements[nStage].Rotate(geo::CVector::Zaxis, dRefAzimuth);
}

void CBoundaryCondition::CSupport::Displacement(const geo::IVector &vecDisplacement, int nStage)
{
  ASSERT(m_bFull);
  ASSERT(nStage >= 0 && nStage < m_vcDisplacements.size());

  m_vcDisplacements[nStage] = vecDisplacement;
}

// get vector
const geo::IVector &CBoundaryCondition::CSupport::Displacement(int nStage) const
{
  return m_vcDisplacements[nStage];
}

// the number of support directions (0, 1, 2 or 3)
int CBoundaryCondition::CSupport::SupportDirSize() const
{
  if(m_bFull) return 3;

  return m_vcDirections.size();
}

// the vector with the given support direction (0 <= nDir < SupportDirSize())
const geo::IVector &CBoundaryCondition::CSupport::SupportDir(int nDir) const
{
  ASSERT(nDir >= 0 && nDir < SupportDirSize());

  if(m_bFull)
  {
    switch(nDir)
    {
    case 0:
      return geo::CVector::Xaxis;
    case 1:
      return geo::CVector::Yaxis;
    case 2:
      return geo::CVector::Zaxis;
    default:
      ASSERT(FALSE);
    }
  }

  return m_vcDirections[nDir];
}

// combine two supports to one new
CBoundaryCondition::CSupport CBoundaryCondition::CSupport::operator+(const CSupport &rhs)
{
#ifdef _DEBUG
  // diagnostics
  if(m_bFull && rhs.m_bFull)
  {
    ASSERT(m_vcDisplacements.size() == rhs.m_vcDisplacements.size());
    for(int i = 0; i < m_vcDisplacements.size(); i++)
      ASSERT(m_vcDisplacements[i] == rhs.m_vcDisplacements[i]);
  }
#endif

  CSupport Sum;

  // take displacement vector of one of the two that is full
  // if both are full, vectors should be the same (see above check)
  if(m_bFull) Sum.m_vcDisplacements = m_vcDisplacements;
  else if(rhs.m_bFull) Sum.m_vcDisplacements = rhs.m_vcDisplacements;

  Sum.m_bFull = (m_bFull || rhs.m_bFull);

  if(!Sum.m_bFull) // none of input supports is already full
  {
    // support directions: options are
    // 1) single vector
    // 2) two independent vectors
    // 3) full support (3 vectors)
    ASSERT(m_vcDirections.size() == 1 || m_vcDirections.size() == 2);
    ASSERT(rhs.m_vcDirections.size() == 1 || rhs.m_vcDirections.size() == 2);

    CIndependentVectorSet vectors;
    for(int i = 0; i < m_vcDirections.size(); i++)
      vectors.Insert(m_vcDirections[i]);

    for(i = 0; i < rhs.m_vcDirections.size(); i++)
      vectors.Insert(rhs.m_vcDirections[i]);

    ASSERT(vectors.Size() > 0 && vectors.Size() <= 3);
    if(vectors.Size() == 3) Sum.m_bFull = true;
    else
    {
      for(i = 0; i < vectors.Size(); i++) Sum.m_vcDirections.push_back(geo::CVector(vectors.Vector(i)));
    }
  }

  return Sum;
}

// make sure displacements are in line with the support directions
void CBoundaryCondition::CSupport::ConformDisplacementsToDirections()
{
  ASSERT(SupportDirSize() > 0 || (SupportDirSize() == 0 && m_vcDisplacements.size() == 0));
  if(SupportDirSize() == 0 || SupportDirSize() == 3) return;

  for(int i = 0; i < m_vcDisplacements.size(); i++)
  {
    if(SupportDirSize() == 1)
    {
      // displacement vector should be parallel to the support direction vector
      // project the displacement vector on the support direction vector
      if(m_vcDisplacements[i].Length() > EPS)
        m_vcDisplacements[i] = m_vcDisplacements[i].GetNormal() * m_vcDisplacements[i].DotProduct(SupportDir(0));
    }
    else
    {
      // displacement vector should be in the plane spanned by the direction vectors
      ASSERT(SupportDirSize() == 2);
      // project the displacement vector on the plane spanned by the direction vectors
      if(m_vcDisplacements[i].Length() > EPS)
      {
        // the normal vector of the plane
        geo::CVector vecNormal = geo::CPlane(geo::CPoint(0, 0, 0), SupportDir(0), SupportDir(1)).Normal();
        // the projected vector on the plane (normalized)
        geo::CVector vecDir = (vecNormal.CrossProduct(m_vcDisplacements[i]).CrossProduct(vecNormal)).UnitVector();
        // project the vector on the normalized vector for the proper length
        m_vcDisplacements[i] = vecDir * m_vcDisplacements[i].DotProduct(vecDir);
      }
    }
  }
}

//// CSupport::CIndependentVectorSet
bool CBoundaryCondition::CSupport::CIndependentVectorSet::Insert(const geo::IVector &vector)
{
  if(Size() == 3) return false;

  if(Size() == 1 && Vector(0).IsParallel(vector)) return false;

  if(Size() == 2)
  {
    geo::CVector vecCross = Vector(0).CrossProduct(Vector(1));
    if(vecCross.DotProduct(vector) < EPS) return false; // in same plane
  }

  m_vcVectors.push_back(geo::CVector(vector));
  return true;
}

int CBoundaryCondition::CSupport::CIndependentVectorSet::Size()
{
  return m_vcVectors.size();
}

const geo::IVector &CBoundaryCondition::CSupport::CIndependentVectorSet::Vector(int nIndex)
{
  ASSERT(nIndex >= 0 && nIndex < m_vcVectors.size());
  return m_vcVectors[nIndex];
}

//// CPressure
CBoundaryCondition::CPressure::CPressure(int nStages)
:	m_vcPressures(nStages)
{
}

// derive pressure vector from global tensor
void CBoundaryCondition::CPressure::Pressure(const ITensor &GlobalTensor, const geo::IVector &vecNormal, int nStage)
{
  ASSERT(nStage >= 0 && nStage < m_vcPressures.size());

  m_vcPressures[nStage] = GlobalTensor * vecNormal.GetNormal();
}

// direct pressure vector input
void CBoundaryCondition::CPressure::Pressure(const geo::IVector &vecPressure, int nStage)
{
  ASSERT(nStage >= 0 && nStage < m_vcPressures.size());

  m_vcPressures[nStage] = vecPressure;
}
