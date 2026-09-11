// BoundaryCondition.h: interface for the CBoundaryCondition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDARYCONDITION_H__58E2AD93_2846_409E_8F95_5124D97169F8__INCLUDED_)
#define AFX_BOUNDARYCONDITION_H__58E2AD93_2846_409E_8F95_5124D97169F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTetSurface;
class CTetraModel;
class CDisplacementSupportNode;
class CTetraBoundary;

namespace geo {
  class CTetSurface;
}

class CBoundaryCondition  
{
public:
  class CSupport
  {
  public:
    CSupport(int nStages); // generates full support, displacements need to be defined for each stage
    CSupport(const geo::IVector &vecDir); // generates normal support, no displacements defined

    // generates normal support, with displacements, vectors are projected on vecDir
    CSupport(const geo::IVector &vecDir, std::vector<geo::CVector> vcDisplacements);
    CSupport(const CSupport &first, int distfirst, const CSupport &second, int distsecond, bool bFull,
      const geo::IVector &vecDir); // interpolated

    CSupport(const CSupport &rhs);

    // derive displacement vector from global strain tensor and reference vector
    void Displacement(const ITensor &GlobalTensor, const geo::IVector &vecReference, const double &dRefAzimuth, int nStage);
    // direct displacement vector input
    void Displacement(const geo::IVector &vecDisplacement, int nStage);
    // get vector
    const geo::IVector &Displacement(int nStage) const;

    // the number of support directions (0, 1, 2 or 3)
    int SupportDirSize() const;
    // the vector with the given support direction (0 <= nDir < SupportDirSize())
    const geo::IVector &SupportDir(int nDir) const;

    // combine two supports to one new
    CSupport operator+(const CSupport &rhs);

    // make sure displacements are in line with the support directions
    void ConformDisplacementsToDirections();

  private:
    // for operator+
    CSupport();

    class CIndependentVectorSet
    {
    public:
      bool Insert(const geo::IVector &vector);
      int Size();
      const geo::IVector &Vector(int nIndex);

    private:
      std::vector<geo::CVector> m_vcVectors;
    };

    std::vector<geo::CVector> m_vcDisplacements; // for each stage
    bool m_bFull; // whether support is full or normal
    std::vector<geo::CVector> m_vcDirections; // normal support directions (max. 2, otherwise full)
  };

  class CPressure
  {
  public:
    CPressure(int nStages);

    // derive pressure vector from global stress tensor
    void Pressure(const ITensor &GlobalTensor, const geo::IVector &vecNormal, int nStage);
    // direct pressure vector input
    void Pressure(const geo::IVector &vecPressure, int nStage);

  private:
    std::vector<geo::CVector> m_vcPressures; // for each stage
  };

  CBoundaryCondition(const geo::CTetSurface &tetsurf, const CTetraModel &model, const geo::IPoint &ptReference);
  virtual ~CBoundaryCondition();

  typedef std::map<geo::CPoint, CSupport> TSupportMap;
  typedef std::map<const geo::CBodyTriangle*, CPressure> TPressureMap;

  void AddSupports(TSupportMap &supportmap, TPressureMap &pressuremap);

private:
  const geo::CTetSurface &m_Surface;
  geo::CPolyLine m_SortedEdge;
  const CTetraModel &m_Model;
  const geo::IPoint &m_ptReference;

  typedef std::pair<geo::CLine, geo::CPoint> TLinePointPair;
  typedef std::pair<TLinePointPair, TLinePointPair> TLinePointPairPair;

  const CTetraBoundary &Boundary();
  void InsertSupport(TSupportMap &supportmap, const geo::IPoint &pt, CSupport &support,
    const CDisplacementSupportNode::CNodalSupportDef &NodeDef, const geo::IVector &vecReference);
  void InterpolateAndInsertEdgeNodes(TSupportMap &supportmap, TSupportMap &localmap, bool bFull);
  double Interpolate(double dVal1, double dFac1, double dVal2, double dFac2);
  geo::CVector InterpolateVector(const geo::IPoint &pt, const geo::IVector &vec1, const geo::IPoint &pt1, const geo::IVector &vec2, const geo::IPoint &pt2);
  TLinePointPairPair GetBestEdgePoints(const geo::IPoint &pt, const std::vector<TLinePointPair> &vcEdgePoints);
  void InterpolateSurfaceNode(const geo::IPoint &pt,
    const std::vector<std::pair<geo::CLine, geo::CPoint> > &vcEdgePoints, TSupportMap &supportmap);
  void InterpolateSurfaceNodes(TSupportMap &supportmap, TSupportMap &localmap, bool bFull);
  void ProcessNormal(TSupportMap &supportmap);
  void ProcessFull(TSupportMap &supportmap);
  void ProcessStress(const CPressureSupportNode::CFaceSupportDef &def, TPressureMap &pressuremap);
};

#endif // !defined(AFX_BOUNDARYCONDITION_H__58E2AD93_2846_409E_8F95_5124D97169F8__INCLUDED_)
