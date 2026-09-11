#include "OIDIVisitor.h"
#include "Conus.h"
#include "ElementGroup.h"
#include "IBox.h"
#include "IFace.h"
#include "ILine.h"
#include "IMesh.h"
#include "ISurface.h"
#include "IVolume.h"
#include "OIDIMesh.h"
#include "Sphere.h"
#include "hexameshregion.h"

bool OIDIVisitor::HandleElementSet(const geo::IElementSet &ElementSet) {
  if (dynamic_cast<const geo::CElementGroup *>(&ElementSet)) {
    std::vector<OIDISurfaceCell> cells;
    cells.reserve(ElementSet.ElementSize());

    OIDISurfaceVisitor visitor(cells);

    for (int i = 0; i < ElementSet.ElementSize(); ++i) {
      const_cast<geo::IElement *>(&ElementSet.Element(i))->Visit(visitor);
    }

    OIDIMesh *wrapper = new OIDIMeshSU(m_OpenGLNode, &ElementSet, cells);
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::SURFACE_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);

    return true;
  }
  return false;
}

bool OIDIVisitor::HandleMesh(const geo::IMesh &Mesh) {
  if (Mesh.ElementSize() == 0)
    return false;

  OIDIMesh *wrapper = 0;

  // we can have different element types in the mesh, and we have no good way to detect them earlier
  if (Mesh.Element(0).ElementType() == 0) {
    std::vector<OIDIVolumeCell> cells;
    cells.reserve(Mesh.ElementSize());

    OIDIVolumeVisitor visitor(cells);

    for (int i = 0; i < Mesh.ElementSize(); ++i) {
      const_cast<geo::IElement *>(&Mesh.Element(i))->Visit(visitor);
    }

    wrapper = new OIDIMeshVU(m_OpenGLNode, &Mesh, cells);
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::VOLUME_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);
  } else if (Mesh.Element(0).ElementType() ==
             CElementSet::QUAD_3D) // at the moment we're treating them as QUAD_2D, as that is the example I have seen
  {
    std::vector<OIDISurfaceCell> cells;
    cells.reserve(Mesh.ElementSize());

    OIDISurfaceVisitor visitor(cells);

    for (int i = 0; i < Mesh.ElementSize(); ++i) {
      const_cast<geo::IElement *>(&Mesh.Element(i))->Visit(visitor);
    }

    wrapper = new OIDIMeshSU(m_OpenGLNode, &Mesh, cells);
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::SURFACE_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);
  } else if (Mesh.Element(0).ElementType() == CElementSet::LINE_2D) // untested, no example
  {
    std::vector<OIDILineCell> cells;
    cells.reserve(Mesh.ElementSize());

    OIDILineVisitor visitor(cells);

    for (int i = 0; i < Mesh.ElementSize(); ++i) {
      const_cast<geo::IElement *>(&Mesh.Element(i))->Visit(visitor);
    }

    wrapper = new OIDIMeshLU(m_OpenGLNode, &Mesh, cells);
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::LINE_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);
  }
  // TODO: add other element types

  return wrapper != 0;
}

bool OIDIVisitor::HandleVolume(const geo::IVolume &Volume) {
  if (Volume.ElementSize() == 0)
    return false;

  std::vector<OIDIVolumeCell> cells;
  cells.reserve(Volume.ElementSize());

  OIDIVolumeVisitor visitor(cells);

  for (int i = 0; i < Volume.ElementSize(); ++i) {
    const_cast<geo::IElement *>(&Volume.Element(i))->Visit(visitor);
  }

  OIDIMesh *wrapper = new OIDIMeshVU(m_OpenGLNode, &Volume, cells);
  assert(wrapper);

  m_Node->addHandle(OIDISet::Handle(OIDISet::VOLUME_UNSTRUCTURED, m_Set->NrOfMesh()));
  m_Set->addMesh(wrapper);

  return true;
}

bool OIDIVisitor::HandleSurface(const geo::ISurface &Surface) {
  if (Surface.ElementSize() == 0)
    return false;

  std::vector<OIDISurfaceCell> cells;
  cells.reserve(Surface.ElementSize());

  OIDISurfaceVisitor visitor(cells);

  for (int i = 0; i < Surface.ElementSize(); ++i) {
    const_cast<geo::IElement *>(&Surface.Element(i))->Visit(visitor);
  }

  OIDIMesh *wrapper = new OIDIMeshSU(m_OpenGLNode, &Surface, cells);
  assert(wrapper);

  m_Node->addHandle(OIDISet::Handle(OIDISet::SURFACE_UNSTRUCTURED, m_Set->NrOfMesh()));
  m_Set->addMesh(wrapper);

  return true;
}

bool OIDIVisitor::HandlePoint(const geo::IPoint &Point) {
  OIDIMesh *wrapper = OIDIPointMesh::Create(m_OpenGLNode, Point);
  assert(wrapper);

  m_Node->addHandle(OIDISet::Handle(OIDISet::VOLUME_UNSTRUCTURED, m_Set->NrOfMesh()));
  m_Set->addMesh(wrapper);

  return true;
}

bool OIDIVisitor::HandlePolyLine(const geo::IPolyLine &PolyLine) {
  if (PolyLine.LineSize() == 0)
    return false;

  std::vector<OIDILineCell> cells;
  cells.reserve(PolyLine.LineSize());

  OIDILineVisitor visitor(cells);

  for (int i = 0; i < PolyLine.LineSize(); ++i) {
    const_cast<geo::ILine *>(&PolyLine.Line(i))->Visit(visitor);
  }

  OIDIMesh *wrapper = new OIDIMeshLU(m_OpenGLNode, &PolyLine, cells);
  assert(wrapper);

  m_Node->addHandle(OIDISet::Handle(OIDISet::LINE_UNSTRUCTURED, m_Set->NrOfMesh()));
  m_Set->addMesh(wrapper);

  return true;
}

bool OIDIVisitor::HandleArray(const geo::IArray &Array) {
  if (Array.Size() == 0)
    return false;

  if (dynamic_cast<const geo::IArray *>(&Array.ObjectBase(0))) {
    // hexameshregions
    bool ret = true;
    for (size_t i = 0; i < Array.Size(); ++i)
      ret = ret && HandleArray(static_cast<const geo::IArray &>(Array.ObjectBase(i)));
    return ret;
  } else if ((dynamic_cast<const IPointSet *>(m_OpenGLNode) ||
              dynamic_cast<const CHexaMeshRegionBase *>(m_OpenGLNode)) &&
             dynamic_cast<const geo::IPoint *>(&Array.ObjectBase(0))) {
    if (((dynamic_cast<const IPointSet *>(m_OpenGLNode) != 0) &&
         ((dynamic_cast<const IPointSet *>(m_OpenGLNode))->isDrawable())) ||
        (dynamic_cast<const CHexaMeshRegionBase *>(m_OpenGLNode) != 0)) {
      OIDIMesh *wrapper = new OIDIPointSetMesh(m_OpenGLNode, &Array);
      assert(wrapper);

      m_Node->addHandle(OIDISet::Handle(OIDISet::VOLUME_UNSTRUCTURED, m_Set->NrOfMesh()));
      m_Set->addMesh(wrapper);

      return true;
    } else {
      return false;
    }
  } else if (dynamic_cast<const geo::IInterfaceElement *>(&Array.ObjectBase(0))) {
    std::vector<OIDISurfaceCell> cells;
    cells.reserve(Array.Size());

    OIDISurfaceVisitor visitor(cells);

    for (int i = 0; i < Array.Size(); ++i) {
      const_cast<geo::IObject *>(&Array.ObjectBase(i))->Visit(visitor);
    }

    OIDIMesh *wrapper = new OIDIMeshSU(
        m_OpenGLNode, &Array, cells); // IArray not subclass of IElementSet, so TODO: find geometry based on IArray
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::SURFACE_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);

    return true;
  } else if (dynamic_cast<const geo::CSphere *>(&Array.ObjectBase(0))) {
    // and now?
    return false;
  } else {
    std::vector<OIDILineCell> cells;
    cells.reserve(Array.Size());

    OIDILineVisitor visitor(cells);

    bool arrayFound = false; // to ignore lines after the first array as we cannot handle this (or has not been tested)
                             // further on in the code.
    for (int i = 0; i < Array.Size(); ++i) {
      if (!arrayFound && dynamic_cast<const geo::ILine *>(&Array.ObjectBase(i))) {
        const_cast<geo::IObject *>(&Array.ObjectBase(i))->Visit(visitor);
      } else if (dynamic_cast<const geo::IArray *>(&Array.ObjectBase(i))) {
        arrayFound = true;
        HandleArray(static_cast<const geo::IArray &>(Array.ObjectBase(i)));
      }
      // const_cast<geo::IObject *>(&Array.ObjectBase(i))->Visit(visitor);
    }

    // this function will fail in the OIDMeshLU constructor
    // return false;

    OIDIMesh *wrapper = new OIDIMeshLU(
        m_OpenGLNode, &Array, cells); // IArray not subclass of IElementSet, so TODO: find geometry based on IArray
    assert(wrapper);

    m_Node->addHandle(OIDISet::Handle(OIDISet::LINE_UNSTRUCTURED, m_Set->NrOfMesh()));
    m_Set->addMesh(wrapper);

    return true;
  }
}

bool OIDIVisitor::HandleBox(const geo::IBox &Box) {
  std::vector<OIDILineCell> cells;
  cells.reserve(Box.NrOfLines());

  for (size_t i = 0; i < Box.NrOfLines(); ++i) {
    OIDILineCell cell;
    cell.setParent(const_cast<geo::ILine *>(&Box.Line(i)));
    cell.setIndex(cells.size());

    cells.push_back(cell);
  }

  OIDIMesh *wrapper = new OIDIMeshLU(m_OpenGLNode, &Box, cells);
  assert(wrapper);

  m_Node->addHandle(OIDISet::Handle(OIDISet::LINE_UNSTRUCTURED, m_Set->NrOfMesh()));
  m_Set->addMesh(wrapper);

  return false;
}

// these two are called in wellpaths; probably the best/a workable approach is to add data to some temporary
// vectors in the visitor and handle them when OIDIMeshLU is created. Not sure what the correlation with
// the path itself is, but presumable some one-on-one or other well-defined relationship is going on.
bool OIDIVisitor::HandleLabelPoint(const CLabelPoint &LabelPoint) { return false; }

bool OIDIVisitor::HandleSymbol(const ISymbol &Symbol) { return false; }

bool OIDIVisitor::OIDIVolumeVisitor::HandleBody(const geo::IBody &body) {
  OIDIVolumeCell cell;
  cell.setParent(const_cast<geo::IBody *>(&body));
  m_Cells.push_back(cell);
  return true;
}

bool OIDIVisitor::OIDISurfaceVisitor::HandleFace(const geo::IFace &face) {
  OIDISurfaceCell cell;
  cell.setParent(const_cast<geo::IFace *>(&face));
  m_Cells.push_back(cell);
  return true;
}

bool OIDIVisitor::OIDISurfaceVisitor::HandleInterface(const geo::IInterfaceElement &element) {
  OIDISurfaceCell cell;
  cell.setParent(dynamic_cast<geo::IFace *>(const_cast<geo::IElement *>(&element.Front())));
  cell.setInterfaceElement(const_cast<geo::IInterfaceElement *>(&element));
  m_Cells.push_back(cell);
  return true;
}

bool OIDIVisitor::OIDILineVisitor::HandleLine(const geo::ILine &line) {
  OIDILineCell cell;
  cell.setParent(const_cast<geo::ILine *>(&line));
  cell.setIndex(m_Cells.size());
  m_Cells.push_back(cell);
  return true;
}