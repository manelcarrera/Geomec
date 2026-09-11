/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// GoCadSurface.h: interface for the CGoCadSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADSURFACE_H__146A88CF_F5E2_4034_BBE6_72ED4ED69546__INCLUDED_)
#define AFX_GOCADSURFACE_H__146A88CF_F5E2_4034_BBE6_72ED4ED69546__INCLUDED_

#include "FaceGroup.h"
#include "FvGocadHeader.h"
#include "ITriangle.h"

#include "FieldValueFileExports.h"

// Header for surface
class FIELDVALUEFILE_EXPORT CSurfaceHeader : public CGoCadHeader {
public:
  CSurfaceHeader(const QString &sName);
};

class CGoCadSurface;

class FIELDVALUEFILE_EXPORT CGoCadSurface : public geo::CFaceGroup {
  // The face of a gocad surface
  class FIELDVALUEFILE_EXPORT CTriGoCadFace : public geo::ITriangle {
    int m_nodes[3];
    CGoCadSurface &m_surface;

  public:
    CTriGoCadFace(CGoCadSurface &surface, int nPoint1, int nPoint2, int nPoint3);
    using geo::ITriangle::InterpolateValue;
    virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
    virtual size_t Order() const { return 1; }
    virtual int PointIndex(int nIndex) const;
    virtual size_t NrOfNodes() const;
    virtual const geo::INode &Node(int nIndex) const;
    virtual void Node(int nIndex, const geo::IPoint &point);
    virtual const geo::IElementSet *IndexingElementSet() const;
    virtual std::string Type() const;
  };

  typedef std::map<int, int> TPointMap;
  TPointMap m_mpPoint;
  CSurfaceHeader *m_pHeader;

public:
  CGoCadSurface(geo::IMesh &mesh);
  CGoCadSurface(geo::IMesh &mesh, const geo::ISurface &surface, const QString &sName);
  virtual ~CGoCadSurface();

  bool isPoint(int nIndex) const;

  // Insertion of points ...
  bool InsertPoint(int nIndex, const geo::IPoint &pt);
  bool InsertPoint(int nIndex, const double &x, const double &y, const double &z);

  // Insertion of triangles
  bool InsertTriangle(int nV1, int nV2, int nV3);

  // Insertion of
  const CSurfaceHeader &Header() const;
  CSurfaceHeader &Header();

  void Header(const QString &sName);
  void MergeSurface(CGoCadSurface &surf);
};

#endif // !defined(AFX_GOCADSURFACE_H__146A88CF_F5E2_4034_BBE6_72ED4ED69546__INCLUDED_)
