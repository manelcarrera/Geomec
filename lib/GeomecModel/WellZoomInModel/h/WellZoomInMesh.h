#ifndef _WELLZOOMINMESH_H_
#define _WELLZOOMINMESH_H_

#include "IWellMesh.h"

class CWellZoomInModel;

class CWellZoomInMesh : public IWellMesh {
public:
  CWellZoomInMesh(const QString &sName, CWellZoomInModel &zoominmodel);
  CWellZoomInMesh(const CWellZoomInMesh &rhs);
  virtual ~CWellZoomInMesh();

  CWellZoomInMesh &operator=(const CWellZoomInMesh &rhs);
  bool operator==(const CWellZoomInMesh &rhs) const;

  // Rendering and default color
  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;
  virtual TColor Color() const;

  bool CanCreateMesh() const;
  bool IsMesh() const;
  void CreateMesh();
  void InvalidateMesh();

  const CWellZoomInModel &ZoomInModel() const;
  CWellZoomInModel &ZoomInModel();

  int NrOfRadialElements() const;
  void NrOfRadialElements(int nRadialElements);

  double TargetLongitudinalElementSize() const;
  void TargetLongitudinalElementSize(double dSize);

  void setHexaMesh(bool hexa);
  bool isHexaMesh() const;
  bool isTetraMesh() const;

  double TetraRadius() const;
  void TetraRadius(double radius);

  double TetraIdealElementSize() const;
  void TetraIdealElementSize(double size);

  int MeshNodeToBoundaryNode(int nGridNodeIndex) const;     // returns -1 if no boundary node is associated
  int BoundaryNodeToMeshNode(int nBoundaryNodeIndex) const; // returns -1 if no mesh node is associated

  virtual bool Empty() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  virtual geo::CPoint SamplePosition(const geo::IElement &element) const;
  virtual geo::CPoint SamplePosition(const geo::INode &node) const;

  bool getInvalidateAfterDlg() const;
  void setInvalidateAfterDlg(bool invalidateAfterDlg);

  ACCEPT_GEOMECMODELVISITORS(VisitWellZoomInMesh);

protected:
  virtual void RegisterGridNodes(const T3DGrid &vc3DGrid, std::vector<int> &vcGridNodeIndices, bool bModelStart,
                                 bool bModelEnd);
  virtual void CreateSupports(const std::vector<int> &vcGrid, bool bModelStart, bool bModelEnd);
  virtual void CreateHexaElements(const std::vector<int> &vcGrid1, const std::vector<int> &vcGrid2, double uppertmd,
                                  double lowertmd, TBoundaryElementVec &vcBoundaryElements,
                                  const SegmentInfo &segmentinfo);
  virtual void Create3DGridAt(const geo::IPoint &point, const geo::IVector &vecNormal, T3DGrid &vcGrid3D) const;
  virtual void RegisterTetraBoundaryNodes(const std::map<int, int> &boundaryNodes);

private:
  virtual bool Create2DGrid(IProgressBase &prog);
  void Clear2DGrid();
  geo::CPoint Grid2DPointAt(int iXIndex, int iYIndex) const;
  void CreateSupportAt(int iNode);
  bool CheckCurvature();
  double MaximumSquaredRadius(const geo::IVector &vec1, const geo::IVector &vec2, double dDist) const;
  void InsertNode2MidPoint(const geo::INode &node, const geo::IPoint &midpoint);
  void InsertBody2MidPoint(const geo::IBody &body, const geo::IPoint &midpoint);

private:
  CWellZoomInModel &m_zoominmodel;
  int m_nRadialElements;     // the number of elements in radial direction (and along the outer edge)
  double m_dLongElementSize; // target element size in longitudinal (along hole) direction

  bool m_isHexaMesh;
  double m_tetraRadius;
  double m_tetraIdealElementSize;

  bool m_bInvalidateAfterDlg;

  typedef std::vector<std::vector<geo::IPoint *>> T2DGrid;
  T2DGrid m_vc2DGrid;

  typedef std::map<int, int> TBoundaryNodeMap;
  TBoundaryNodeMap
      m_mpMesh2Boundary; // maps mesh node indices to boundary node indices (not all mesh nodes will be found)
  TBoundaryNodeMap
      m_mpBoundary2Mesh; // maps boundary node indices to mesh node indices (all boundary nodes should be found)

  typedef std::map<const geo::IBody *, geo::CPoint> TBody2MidPointMap;
  TBody2MidPointMap m_mpBody2MidPoint;

  typedef std::map<const geo::INode *, geo::CPoint> TNode2MidPointMap;
  TNode2MidPointMap m_mpNode2MidPoint;
  /*
    // for debugging
    mutable std::vector<geo::CPoint> m_vcDbg;
  */
};

#endif // _WELLZOOMINMESH_H_