/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/* Confidential Source Code                 Copyright (c) 2008 TNO DIANA B.V. */
#if !defined(AFX_CONVEXHULL_H__21267E27_37BC_4446_B0D2_5A9CEEE56EFD__INCLUDED_)
#define AFX_CONVEXHULL_H__21267E27_37BC_4446_B0D2_5A9CEEE56EFD__INCLUDED_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4290) // warning "throw(...)" only acts as not __declspec(nothrow)
// Microsoft doesn't understand throw(type X) and gcc doesn't understand throw(...).
// Since we can't disable it in the property sheets, we do it here.
// (And since we have several in this file, we do it at the top, but still with a push/pop.)
// Once Visual C++ can compile like a man, this pragma can be removed.
// More occurrences are in ConvexHull.cpp and ModelFrame's IModelFile.cpp.
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IBody.h"
#include "IFace.h"

#include "GeometryExports.h"
#include "IProgressBase.h"

namespace geo {


class GEOMETRY_EXPORT  CConvexHull : public IBody
{
public:
  // Convexhull faces
  class GEOMETRY_EXPORT CConvexHullFace : public IFace
  {
    IBody::TIndexVec   m_vcPoint;
    CVector            m_normal;
    const CConvexHull *m_parent;
  public:
    CConvexHullFace();
    CConvexHullFace& operator=(const CConvexHullFace& rhs);

    void Set(const CConvexHull &Parent, int n1, int n2, int n3, double x, double y, double z);

    virtual CVector Normal() const;

    const IBody::TIndexVec& PointIndices() const;
    virtual const IPoint &Point(int nIndex) const throw (const char *);
    virtual void Point(int nIndex, const IPoint &pt);
    virtual int NrOfPoints() const;

    int PointIndex(int nLocalIndex) const throw (const char *);

    virtual size_t Order() const;
  };

  friend class CConvexHullFace;
public:
  CConvexHull(const std::vector<const IPoint*> &points);
  CConvexHull(CPtrArray<IPoint> &points);
  virtual ~CConvexHull();
  void SetValidDigits(const double &digits= -1.0);//<0: calculate
  void ResizeHull(const double& increment) { m_dHullIncrement= increment; }
  bool Calculate(IProgressBase *progress = 0);
  CPoint GlobalPoint(int nIndex) const;

  static double KahanSummation(const double *input, int size);

  virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;

  virtual int NrOfPoints() const;
  virtual int NrOfFaces() const;
  virtual const IFace &Face(int nIndex) const throw (const char *);

  virtual CElementPoint MidPoint() const;

  long SaveProgressSize() const;

  template <class STREAM>
  void Save(STREAM& stream, IProgressBase& progress);

  template <class STREAM>
  void Load(STREAM& stream, IProgressBase& progress);

private:

  double RoundNumber(double d) const;
  int VolumeSign(const CConvexHullFace& face, const IPoint &point) const;

  virtual const IPoint &Point(int nIndex) const throw (const char *);
  virtual void Point(int nIndex, const IPoint &pt);

  using IBody::FacePointIndices;
  using IBody::FaceNodeIndices;
  virtual const TIndexVec& FacePointIndices(int nIndex) const throw (const char *);
  virtual const TIndexVec& FaceNodeIndices(int nIndex) const throw (const char *);

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  using IBody::LinePointIndices;
  using IBody::LineNodeIndices;
  virtual const TIndexVec& LinePointIndices(int nIndex) const;
  virtual const TIndexVec& LineNodeIndices(int nIndex) const;

  virtual double Size() const;

  virtual size_t Order() const;
  virtual const char* FaceName(int /*nIndex*/) const { return 0; }


private:

  double m_dHullIncrement; //add this length to the hull, in all directions
  geo::CPoint m_centrePoint; //middle point in pointSet
  double m_validDigits;

  mutable CPoint m_min; // IF we have an invalid hull (because it was stored before we prevented that) then we calculate the bounding box
  mutable CPoint m_max;
  mutable bool   m_invalid;


  std::vector<const IPoint *>  m_vcInputPoints;
  std::vector<CPoint>          m_vcHullPoints;
  std::vector<CConvexHullFace> m_vcHullFaces;
};


class IConvexHullImpl
{
public:
  IConvexHullImpl(const CConvexHull& /*parent*/) {}
  virtual ~IConvexHullImpl() {}

  virtual void SetPoints(IProgressBase *progress, const std::vector<const IPoint *> &points) = 0;

  virtual void SetValidDigits(double digits) = 0;
  virtual double GetValidDigits() const = 0;

  virtual CVector MidPoint() const = 0;

  virtual void GetPointsAndFaces(std::vector<CPoint> &points, std::vector<CConvexHull::CConvexHullFace> &faces) const = 0;

  virtual bool Calculate(IProgressBase *progress) = 0;
};

extern IConvexHullImpl *createConvexHullImpl(const CConvexHull& parent);



template <class STREAM>
void CConvexHull::Save(STREAM& stream, IProgressBase& progress)
{
  // version number
  //  1: initial
  //  2: no saving of implementation datastructures
  stream << int(2);

  stream << m_dHullIncrement;
  stream << m_centrePoint.X() << m_centrePoint.Y() << m_centrePoint.Z();
  stream << m_validDigits;

  // stream vertices
  stream << int(m_vcHullPoints.size());
  for(size_t i = 0; i < m_vcHullPoints.size(); ++i)
  {
    stream << m_vcHullPoints[i].X() << m_vcHullPoints[i].Y() << m_vcHullPoints[i].Z();

    progress.Step();
  }

  // stream faces
  stream << int(m_vcHullFaces.size());
  for(size_t i = 0; i < m_vcHullFaces.size(); ++i)
  {
    const IBody::TIndexVec& indices = m_vcHullFaces[i].PointIndices();

    for (size_t j = 0; j < indices.size(); ++j)
    {
      stream << indices[j];
    }

    const CVector& normal = m_vcHullFaces[i].Normal();
    stream << normal.X() << normal.Y() << normal.Z();

    progress.Step();
  }

}


template <class STREAM>
void CConvexHull::Load(STREAM& stream, IProgressBase& progress)
{
  int nVersion;
  stream >> nVersion;

  int    iDummy;
  double dDummy;

  if (nVersion < 2)
  {
    stream >> dDummy;
  }

  stream >> m_dHullIncrement;
  double x, y, z;
  stream >> x >> y >> z;
  m_centrePoint.Set(x, y, z);
  stream >> m_validDigits;

  int nVertices;
  stream >> nVertices;

  m_vcHullPoints.resize(nVertices);

  if (nVersion < 2) // we have some duplicate code below, but eventually we want all the if's outside the read loops, so we split it up like this
  {
    for(int i = 0; i < nVertices; ++i)
    {
      stream >> x >> y >> z;
      m_vcHullPoints[i].Set(x, y, z);

      stream >> iDummy;
      stream >> iDummy;

      progress.Step();
    }

    int nEdges;
    stream >> nEdges;

    for(int i = 0; i < nEdges; ++i)
    {
      for (int j = 0; j < 8; ++j)
      {
        stream >> iDummy;
      }
    }

    int nFaces;
    stream >> nFaces;

    m_vcHullFaces.resize(nFaces);

    for(int i = 0; i < nFaces; ++i)
    {
      int nVertex[3];

      for(int j = 0; j < 3; ++j)
      {
        stream >> iDummy;
      }

      for(int j = 0; j < 3; ++j)
      {
        stream >> nVertex[j];
      }

      stream >> x >> y >> z;

      for(int j = 0; j < 3; ++j)
      {
        stream >> iDummy;
      }

      m_vcHullFaces[i].Set(*this, nVertex[0], nVertex[1], nVertex[2], x, y, z);

      progress.Step();
    }

    return;
  }

  // nVersion > 1

  for(int i = 0; i < nVertices; ++i)
  {
    stream >> x >> y >> z;
    m_vcHullPoints[i].Set(x, y, z);

    progress.Step();
  }

  int nFaces;
  stream >> nFaces;

  m_vcHullFaces.resize(nFaces);

  for(int i = 0; i < nFaces; ++i)
  {
    int nVertex[3];

    for(int j = 0; j < 3; ++j)
    {
      stream >> nVertex[j];
    }

    stream >> x >> y >> z;

    m_vcHullFaces[i].Set(*this, nVertex[0], nVertex[1], nVertex[2], x, y, z);

    progress.Step();
  }

}


}//end namespace geo

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // !defined(AFX_CONVEXHULL_H__21267E27_37BC_4446_B0D2_5A9CEEE56EFD__INCLUDED_)
