#ifndef _OIDI_GEOMETRY_H_
#define _OIDI_GEOMETRY_H_

#include <assert.h>

#include <Inventor/SbVec.h>

#include <MeshVizXLM/MxTimeStamp.h>
#include <MeshVizXLM/mesh/geometry/MiGeometryI.h>

#include "BodyGroup.h"
#include "IBody.h"
#include "IBox.h"
#include "IElementSet.h"
#include "IFace.h"
#include "IObject.h"
#include "ITensorGroup.h"
#include "IVectorResult.h"
#include "InterfaceElement.h"
#include "Line.h"
#include "MeshSurface.h"
#include "Point.h"

class OIDIGeometryI : public MiGeometryI {

private:
  const geo::IElementSet *m_Parent;
  const geo::IArray *m_PointSet;
  const geo::IBox *m_Box;

  enum TFLAG { HAS_ELEMENTS = 0, HAS_POINTS, HAS_LINES };

  const TFLAG m_Flag;

  std::size_t m_TimeStamp;

  SbVec3d m_translation;

  struct TDeformation {
    bool is_deformation;
    std::vector<MbVec3d> points;
    std::pair<MbVec3d, MbVec3d> min_max;
  } m_deformation;

  static const TDeformation NO_DEFORMATION;

public:
  OIDIGeometryI(const geo::IElementSet *parent)
      : m_Flag(HAS_ELEMENTS), m_Parent(0), m_PointSet(0), m_Box(0), m_TimeStamp(MxTimeStamp::getTimeStamp()),
        m_translation(0, 0, 0) {
    // It looks like about every other class in Geometry has a different method to determine the indexing element set
    // and this constructor should eventually reflect that (even better: handle in visitor, but not every class has its
    // own handler yet)
    // Assume for now that the set contains at least one Body/Element/etc
    assert(parent);
    const geo::CBodyGroup::CSideSurface *surface = dynamic_cast<const geo::CBodyGroup::CSideSurface *>(parent);
    const CMeshSurface *meshSurface = dynamic_cast<const CMeshSurface *>(parent);
    if (surface && surface->BodyGroup().Body(0).IndexingElementSet()) {
      m_Parent = surface->BodyGroup().Body(0).IndexingElementSet();
    } else if (parent->Element(0).IndexingElementSet()) {
      m_Parent = parent->Element(0).IndexingElementSet();
    } else if (meshSurface && meshSurface->Face(0).Parent() && meshSurface->Face(0).Parent()->IndexingElementSet()) {
      m_Parent = meshSurface->Face(0).Parent()->IndexingElementSet();
    } else {
      m_Parent = parent;
    }
    m_deformation = NO_DEFORMATION;
  }

  OIDIGeometryI(const geo::IArray *parent)
      : m_Parent(0), m_PointSet(parent), m_Box(0), m_TimeStamp(MxTimeStamp::getTimeStamp()),
        m_Flag(
            parent && parent->Size() > 0 && dynamic_cast<const geo::ILine *>(&parent->ObjectBase(0))
                ? HAS_LINES
                : (dynamic_cast<const geo::IInterfaceElement *>(&parent->ObjectBase(0)) ? HAS_ELEMENTS : HAS_POINTS)),
        m_translation(0, 0, 0) {
    if (m_Flag == HAS_ELEMENTS) {
      const geo::IInterfaceElement *elt = static_cast<const geo::IInterfaceElement *>(&parent->ObjectBase(0));
      assert(elt);
      m_deformation = NO_DEFORMATION;

      m_Parent = elt->IndexingElementSet();

      if (m_Parent) // we can just use the underlying element set as our geometry
      {
        m_PointSet = 0;
      }

      assert(
          m_Parent); // actually, for now, we only deal with the situation where we have one (may change in the future)
    }
  }

  OIDIGeometryI(const geo::IBox *parent)
      : m_Parent(0), m_PointSet(0), m_Box(parent), m_TimeStamp(MxTimeStamp::getTimeStamp()), m_Flag(HAS_LINES),
        m_translation(0, 0, 0) {}

  const geo::IElementSet *Parent() const { return m_Parent; }
  const geo::IArray *PointSet() const { return m_PointSet; }

  const geo::IPoint *getPoint(size_t i) const {
    if (m_Parent) {
      const geo::IPoint *point = &m_Parent->Point(i);
      assert(point);
      return point;
    } else if (m_PointSet) {
      if (m_Flag == HAS_ELEMENTS) {
        assert(false); // see ctor
        return 0;
      } else if (m_Flag == HAS_LINES) {
        const geo::ILine *line = &static_cast<const geo::ILine &>(m_PointSet->ObjectBase(i / 2));
        assert(line);

        const geo::IPoint *point = &static_cast<const geo::IPoint &>(line->Point(i % 2));
        assert(point);

        return point;
      } else {
        const geo::IPoint *point = &static_cast<const geo::IPoint &>(m_PointSet->ObjectBase(i));
        assert(point);

        return point;
      }
    } else {
      assert(m_Box);
      assert(m_Flag == HAS_LINES);

      const geo::ILine *line = &m_Box->Line(i / 2);
      assert(line);

      const geo::IPoint *point = &static_cast<const geo::IPoint &>(line->Point(i % 2));
      assert(point);

      return point;
    }
  }

  MbVec3d getCoord(size_t i) const {
    assert(i < getSize());

    const geo::IPoint *point = getPoint(i);
    assert(point);

    if (m_deformation.is_deformation && i < m_deformation.points.size()) {
      return MbVec3d(m_deformation.points[i][0] - m_translation[0], m_deformation.points[i][1] - m_translation[1],
                     m_deformation.points[i][2] - m_translation[2]);
    } else
      return MbVec3d(point->X() - m_translation[0], point->Y() - m_translation[1], point->Z() - m_translation[2]);
  }

  MbVec3d getMax() const // FIXME: case 'm_is_deformation == true'
  {
    if (m_deformation.is_deformation) {
      MbVec3d translation = MbVec3d(m_translation[0], m_translation[1], m_translation[2]);
      return (m_deformation.min_max.second - translation);
    } else {
      geo::CPoint point = m_Parent ? m_Parent->Max() : m_PointSet->Max();
      return MbVec3d(point.X() - m_translation[0], point.Y() - m_translation[1], point.Z() - m_translation[2]);
    }
  }

  MbVec3d getMin() const {
    if (m_deformation.is_deformation) {
      MbVec3d translation = MbVec3d(m_translation[0], m_translation[1], m_translation[2]);
      return (m_deformation.min_max.first - translation);
    } else {
      geo::CPoint point = m_Parent ? m_Parent->Min() : m_PointSet->Min();
      return MbVec3d(point.X() - m_translation[0], point.Y() - m_translation[1], point.Z() - m_translation[2]);
    }
  }

  void Translate(const SbVec3d &translation) {
    m_translation.setValue(translation[0], translation[1], translation[2]);
    m_TimeStamp = MxTimeStamp::getTimeStamp();
  }

  MbVec3d getTranslation() const { return MbVec3d(m_translation[0], m_translation[1], m_translation[2]); }

  void set_deformation(const std::vector<MbVec3d> &points_mod_v, std::pair<MbVec3d, MbVec3d> min_max) {
    m_deformation = TDeformation{true, points_mod_v, std::make_pair(min_max.first, min_max.second)};

    m_TimeStamp = MxTimeStamp::getTimeStamp();
  }

  void set_deformation(bool val) {
    m_deformation = NO_DEFORMATION;
    m_TimeStamp = MxTimeStamp::getTimeStamp();
  }

  void update_timestamp() { m_TimeStamp = MxTimeStamp::getTimeStamp(); }

  size_t getSize() const {
    if (m_Parent) {
      return m_deformation.is_deformation ? m_deformation.points.size() : m_Parent->PointSize();
    } else if (m_PointSet) {
      switch (m_Flag) {
      case HAS_ELEMENTS:
        assert(false); // see ctor
        return 0;
      case HAS_LINES:
        return m_PointSet->Size() * 2;
      default:
      case HAS_POINTS:
        return m_PointSet->Size();
      }
    } else
      return m_Box->NrOfLines() * 2;
  }

  size_t getTimeStamp() const { return m_TimeStamp; }
};

#endif