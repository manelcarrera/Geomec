#ifndef _OIDI_CELL_H_
#define _OIDI_CELL_H_

#include <MeshVizXLM/extractors/MxHexahedronCellExtract.h>
#include <MeshVizXLM/extractors/MxTetrahedronCellExtract.h>
#include <MeshVizXLM/mesh/cell/MiLineCell.h>
#include <MeshVizXLM/mesh/cell/MiSurfaceCell.h>
#include <MeshVizXLM/mesh/cell/MiVolumeCell.h>

/// template base class for all OIDI...Cell classes
template <class IElementType, class MiCellType> class OIDICell : public MiCellType {
protected:
  IElementType *m_Parent;

public:
  OIDICell() : m_Parent(0) {}

  void setParent(IElementType *parent) { m_Parent = parent; }

  const IElementType *getParent() const { return m_Parent; }

  size_t getNumNodes() const {
    assert(m_Parent);
    return m_Parent->NrOfPoints();
  }

  virtual size_t getNodeIndex(size_t node) const {
    assert(m_Parent);
    return m_Parent->PointIndex(node);
  }

  virtual bool isPointInsideCell(const MiGeometryI &meshGeometry, const MbVec3d &point,
                                 std::vector<double> &weights) const = 0;
};

class OIDIVolumeCell : public OIDICell<geo::IBody, MiVolumeCell> {

public:
  OIDIVolumeCell() : OIDICell() {}

  size_t getNumFacets() const { return m_Parent->NrOfFaces(); }

  size_t appendNodesIndexOfFacet(size_t facet, std::vector<size_t> &facetNodes) const {
    assert(m_Parent);

    const geo::IFace *face = &getParent()->Face(facet);
    assert(face);

    for (int i = 0; i < face->NrOfPoints(); ++i) {
      facetNodes.push_back(face->PointIndex(i));
    }

    return face->NrOfPoints();
  }

  void getIsosurfTopology(unsigned char caseId, std::vector<std::pair<size_t, size_t>> &edgeList) const {
    assert(m_Parent);

    if (getNumFacets() == 4)
      MxTetrahedronCellExtract::getIsosurfTopology(caseId, edgeList);
    else if (getNumFacets() == 6)
      MxHexahedronCellExtract::getIsosurfTopology(caseId, edgeList);
    else
      throw MiAbstractMethodError("OIDIVolumeCell::getIsosurfTopology(...)");
  }

  bool isPointInsideCell(const MiGeometryI &meshGeometry, const MbVec3d &point, std::vector<double> &weights) const {

    if (getNumFacets() == 4)
      return MxTetrahedronCellExtract::isPointInsideCell(meshGeometry, this, point, weights);
    else if (getNumFacets() == 6)
      return MxHexahedronCellExtract::isPointInsideCell(meshGeometry, this, point, weights);
    else
      throw MiAbstractMethodError("OIDIVolumeCell::isPointInsideCell(const MbVec3d &point)");
  }
  bool hasSameNodeIndices(const OIDIVolumeCell *cell) const;
  bool findNodeIndex(size_t nodeIndex) const;
};

class OIDISurfaceCell : public OIDICell<geo::IFace, MiSurfaceCell> {
  geo::IInterfaceElement *m_InterfaceElt;

public:
  OIDISurfaceCell() : OIDICell(), m_InterfaceElt(0) {}

  bool isPointInsideCell(const MiGeometryI &meshGeometry, const MbVec3d &point, std::vector<double> &weights) const {
    return false;
  }

  void setInterfaceElement(geo::IInterfaceElement *element) { m_InterfaceElt = element; }

  size_t appendNodeIndices(std::vector<size_t> &facetNodes) const {
    for (int i = 0; i < getNumNodes(); ++i) {
      facetNodes.push_back(getNodeIndex(i));
    }

    return getNumNodes();
  }

  const geo::IInterfaceElement *getInterfaceElement() const { return m_InterfaceElt; }
};

template <class IElementType, class MiCellType, int NodesPerCell>
class OIDIVectorIndexingCell : public OIDICell<IElementType, MiCellType> {
  size_t m_VectorIndex;

public:
  OIDIVectorIndexingCell() : m_VectorIndex(0), OIDICell() {}

  void setIndex(size_t index) { m_VectorIndex = index * NodesPerCell; }

  virtual size_t getNodeIndex(size_t node) const { return m_VectorIndex + node; }

  bool isPointInsideCell(const MiGeometryI &meshGeometry, const MbVec3d &point, std::vector<double> &weights) const {
    return false;
  }
};

class OIDILineCell : public OIDIVectorIndexingCell<geo::ILine, MiLineCell, 2> {

public:
  OIDILineCell() : OIDIVectorIndexingCell() {}
};

class OIDIPointSetCell : public MiSurfaceCell {
  const geo::IArray *m_pointSet;

public:
  OIDIPointSetCell(const geo::IArray *pointSet) : m_pointSet(pointSet) {}

#ifdef ISMIVOLUMECELL
  virtual size_t getNumFacets() const {
    // return 1;
    return m_pointSet->Size() > 2 ? m_pointSet->Size() - 2 : 0;
  }
#endif

  virtual size_t getNumNodes() const {
    size_t size = m_pointSet->Size();
    return size;
  }

  virtual size_t getNodeIndex(size_t node) const { return node; }

#ifdef ISMIVOLUMECELL
  virtual size_t appendNodesIndexOfFacet(size_t facet, std::vector<size_t> &facetNodes) const {
#if 0
  for (int i = 0; i < m_pointSet->Size(); ++i)
  {
      facetNodes.push_back(i);
  }
  return m_pointSet->Size();
#endif
    facetNodes.push_back(facet);
    facetNodes.push_back(facet + 1);
    facetNodes.push_back(facet + 2);

    return 3;
  }

  virtual unsigned char getMarchingCaseId(std::vector<bool> &nodesSign, size_t beginNodeId = 0) const { return 0; }

  virtual void getIsosurfTopology(unsigned char caseId, std::vector<std::pair<size_t, size_t>> &edgeList) const {
    return;
  }
#endif

  virtual bool isPointInsideCell(const MiGeometryI &meshGeometry, const MbVec3d &point,
                                 std::vector<double> &weights) const {
    return false;
  }
};

#endif