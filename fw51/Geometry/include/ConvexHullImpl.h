#ifndef _CONVEX_HULL_IMPL_H_
#define _CONVEX_HULL_IMPL_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4290) // see comment in ConvexHull.h
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {

//==============================================================================
// This object (CConvexHull) might need an update to get rid of the structures.
// These structures were introduced by the original code for calculating
// convexhull from the book "Computational geometry in C" by Joseph O'Rourke.
//==============================================================================

// Define structures for vertices, edges and faces
typedef class tVertexStructure tsVertex;
typedef tsVertex *tVertex;

typedef struct tEdgeStructure tsEdge;
typedef tsEdge *tEdge;

typedef struct tFaceStructure tsFace;
typedef tsFace *tFace;

typedef struct tBoxStructure tsBox;
typedef tsBox *tBox;

class GEOMETRY_EXPORT tVertexStructure {
public:
  // Attributes
  CPoint Point;
  tEdge pDuplicate; // Pointer to incident cone edge (or NULL).
  bool bOnhull;     // Set if point on hull.
  int nIndex;       // for saving and loading
  bool bProcessed;
  bool bDeleted;
  int nNext, nPrev;
  double dWeight;
  // Construction
  tVertexStructure();
  ~tVertexStructure();

  bool operator<(const tVertexStructure &rhs) const;
};

struct GEOMETRY_EXPORT tEdgeStructure {
  tEdgeStructure();

  tFace adjface[2];
  tVertex endpts[2];
  tFace newface; // Pointer to incident cone face.
  bool bDelete;  // Set if edge should be delete.
  tEdge pNext, pPrev;
  int nIndex; // for saving and loading

  void Add(tEdge nEdge);
  void Delete();
};

struct GEOMETRY_EXPORT tFaceStructure {
  tFaceStructure();

  tEdge edge[3];
  tVertex vertex[3];
  CVector normal;
  bool visible; // Set if face visible from new point.
  tFace pNext, pPrev;
  int nIndex; // for saving and loading

  void Add(tFace nFace);
  void Delete();
};

struct GEOMETRY_EXPORT tBoxStructure {
  int m_halfLengthX;
  int m_halfLengthY;
  int m_halfLengthZ;

  int m_strideX;
  int m_strideY;

  std::vector<int> m_vcBoxes;

  tBoxStructure();

  void Set(int x, int y, int z);
  void Add(int x, int y, int z, int v);
};

class CConvexHullImpl : public IConvexHullImpl {
  friend class CTaskFilterVertices;

public:
  CConvexHullImpl(const CConvexHull &parent);
  virtual ~CConvexHullImpl();

  virtual void SetPoints(IProgressBase *progress, const std::vector<const IPoint *> &points);

  virtual void SetValidDigits(double digits);
  virtual double GetValidDigits() const;

  virtual CVector MidPoint() const;

  virtual void GetPointsAndFaces(std::vector<CPoint> &points, std::vector<CConvexHull::CConvexHullFace> &faces) const;

  virtual bool Calculate(IProgressBase *progress);

private:
  void CalcNormals();
  double RoundNumber(const double &d) const;
  IPoint &RoundPoint(IPoint &p) const;
  bool CheckValidDigits(const double &testValue, double &digits);

  int AddVertex(int nVertex);
  void DeleteVertex(int nVertex);
  int VertexSize() const;

  void sortExtremities();

  bool DoubleTriangle();
  void ConstructHull(IProgressBase *progress);
  bool AddOne(tVertex p);
  // int VolumeSign(tFace f, const IPoint &p) const;
  int VolumeSign(tFace f, tVertex p) const;
  tFace MakeConeFace(tEdge e, tVertex p) throw(const char *);
  void MakeCcw(tFace f, tEdge e, tVertex p);
  tEdge MakeNullEdge();
  tFace MakeNullFace();
  tFace MakeFace(tVertex v0, tVertex v1, tVertex v2, tFace f);

  void CleanUp();
  void CleanEdges();
  void CleanFaces();
  void CleanVertices();

  bool Collinear(const IPoint &a, const IPoint &b, const IPoint &c);
  bool CheckEuler(int V, int E, int F);
  bool Consistency();
  bool NonDegenerate();
  bool Convexity();
  bool Checks();

  void ClearVertices();
  void ClearEdges();
  void ClearFaces();

  void ClearAll();

  int FindPointIndex(const std::vector<CPoint> &points, const IPoint &pnt) const;

  double m_maxDist;          // largest value of X, Y or Z in pointSet
  double m_dHullIncrement;   // add this length to the hull, in all directions
  geo::CPoint m_centrePoint; // middle point in pointSet
  double m_validDigits;

  mutable CPoint m_min;
  mutable CPoint m_max;
  mutable bool m_invalid;

  // typedef std::set<tVertex> TVertexSet;
  // TVertexSet m_tVertices;
  tEdge m_tEdges;
  tFace m_tFaces;

  int nVertexRoot;
  int nVertexCount;

  std::vector<tsVertex> m_vcVertices;

  tsBox m_box;

  const CConvexHull &m_parent;
};

#endif