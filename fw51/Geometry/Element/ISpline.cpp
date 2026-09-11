/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ISpline.h"
#include "INode.h"

namespace geo {

/*!
  \class CSplineFace
  \brief A private IQuadrilateral implementation for the implementation of the Face function.

  \sa ISpline
  \sa IQuadrilateral
*/

/*!
  Constructor

  The parent ISpline object and this face's index in the parent ISpline object are required here.
*/
ISpline::CSplineFace::CSplineFace(const ISpline &parent, int nIndex) : m_parent(parent), m_nIndex(nIndex) {
  assert(nIndex < parent.NrOfFaces());
}

ISpline::CSplineFace::~CSplineFace() {}

/*!
  Returns the parent ISpline object.
*/
const ISpline &ISpline::CSplineFace::Spline() const { return m_parent; }

/*!
  Returns the index of this face in the parent ISpline object.
*/
int ISpline::CSplineFace::Index() const { return m_nIndex; }

size_t ISpline::CSplineFace::NrOfNodes() const { return 4; }

const INode &ISpline::CSplineFace::Node(int nIndex) const { return Spline().Node(ParentNodeIndex(nIndex)); }

/*!
  Convert a local node index (0-3) to the parent's node index.
*/
int ISpline::CSplineFace::ParentNodeIndex(int nIndex) const {
  assert(nIndex >= 0 && nIndex < 4);

  // bottom side
  if (nIndex < 2)
    return Index() + nIndex;

  // top side
  int nf = Spline().NrOfFaces();
  nIndex -= 2;
  return 2 * nf + nIndex - Index();
}

/*!
  Modification of nodes is not allowed, this should be done on the parent ISpline object. This function asserts false.
*/
void ISpline::CSplineFace::Node(int /*nIndex*/, const IPoint & /*pt*/) { assert(false); }

size_t ISpline::CSplineFace::Order() const { return 1; }

///////////////////////////////////////////////////////////////////////

/*!
  \class CSplineLine
  \brief A private ILine implementation for the implementation of the Line function.

  \sa ISpline
  \sa ILine
*/

/*!
  Constructor

  The parent ISpline object and this line's node indices in the parent ISpline object are required here.
*/
ISpline::CSplineLine::CSplineLine(const ISpline &parent, int nFirst, int nSecond) : m_parent(parent) {
  m_Nodes[0] = nFirst;
  m_Nodes[1] = nSecond;
}

ISpline::CSplineLine::~CSplineLine() {}

/*!
  Returns the parent ISpline object.
*/
const ISpline &ISpline::CSplineLine::localParent() const { return m_parent; }

size_t ISpline::CSplineLine::NrOfNodes() const { return 2; }

const INode &ISpline::CSplineLine::Node(int nIndex) const { return localParent().Node(ParentNodeIndex(nIndex)); }

/*!
  Modification of nodes is not allowed, this should be done on the parent ISpline object. This function asserts false.
*/
void ISpline::CSplineLine::Node(int /*nIdex*/, const IPoint & /*pt*/) { assert(false); }

size_t ISpline::CSplineLine::Order() const { return 1; }

/*!
  Convert a local node index (0 or 1) to the parent's node index.
*/
int ISpline::CSplineLine::ParentNodeIndex(int nIndex) const {
  assert(nIndex == 0 || nIndex == 1);
  return m_Nodes[nIndex];
}

//////////////////////////////////////////////////////////////////////

/*!
  \class ISpline
  \brief Baseclass for the spline elements in DIANA.

  The ISpline class is the general base for all spline based elements in DIANA. The elements are planar and rectangular,
  and can contain a arbitrary number of faces along its length (i.e. in the element's x direction).

  Spline elements have no unique concept of isoparametric coordinates. Each rectangular field has its own isoparametric
  axis system. Therefore, the functions based on isoparametric coordinates can not be used. All these functions
  implemented in IElement are overridden in ISpline.
*/
ISpline::ISpline() {}

ISpline::~ISpline() {
  DestroyFaceCache();
  DestroyLineCache();
}

void ISpline::DestroyFaceCache() const {
  int i;
  for (i = 0; i < m_vcFaces.size(); i++)
    delete m_vcFaces[i];
  m_vcFaces.clear();
}

void ISpline::DestroyLineCache() const {
  int i;
  for (i = 0; i < m_vcLines.size(); i++)
    delete m_vcLines[i];
  m_vcLines.clear();
}

int ISpline::NrOfPoints() const { return NrOfNodes(); }

const IPoint &ISpline::Point(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return Node(nIndex);
}

void ISpline::Point(int nIndex, const IPoint &pt) {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  Node(nIndex, pt);
}

int ISpline::NrOfFaces() const {
  // must be an even number of nodes
  assert(NrOfNodes() % 2 == 0);

  return (NrOfNodes() / 2 - 1);
}

void ISpline::BuildFaceCache() const {
  assert(m_vcFaces.empty());
  int i;
  int nf = NrOfFaces();
  for (i = 0; i < nf; i++)
    m_vcFaces.push_back(new CSplineFace(*this, i));
}

const IQuadrilateral &ISpline::Face(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfFaces());
  if (m_vcFaces.empty())
    BuildFaceCache();

  assert(m_vcFaces.size() == NrOfFaces());
  return *m_vcFaces[nIndex];
}

std::vector<int> ISpline::FaceNodeIndices(int nIndex) const {
  const CSplineFace &face = dynamic_cast<const CSplineFace &>(Face(nIndex));
  std::vector<int> ret;
  int i;
  for (i = 0; i < 4; i++) {
    ret.push_back(face.ParentNodeIndex(i));
  }

  return ret;
}

std::vector<int> ISpline::FacePointIndices(int nIndex) const {
  assert(Order() == 1);
  return FaceNodeIndices(nIndex);
}

int ISpline::NrOfLines() const { return 3 * NrOfFaces() + 1; }

void ISpline::BuildLineCache() const {
  assert(m_vcLines.empty());
  // follow the node indices first along the bottom and top
  int i;
  int nf = NrOfFaces();

  // bottom (left to right)
  for (i = 0; i < nf; i++)
    m_vcLines.push_back(new CSplineLine(*this, i, i + 1));

  // top (right to left)
  for (i = 0; i < nf; i++)
    m_vcLines.push_back(new CSplineLine(*this, nf + i + 1, nf + i + 2));

  // vertical lines
  for (i = 0; i < nf + 1; i++)
    m_vcLines.push_back(new CSplineLine(*this, i, 2 * nf + 1 - i));
}

const ILine &ISpline::Line(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfLines());
  if (m_vcLines.empty())
    BuildLineCache();

  assert(m_vcLines.size() == NrOfLines());
  return *m_vcLines[nIndex];
}

std::vector<int> ISpline::LinePointIndices(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfLines());
  const CSplineLine &line = dynamic_cast<const CSplineLine &>(Line(nIndex));

  std::vector<int> ret;
  int i;
  for (i = 0; i < 2; i++) {
    ret.push_back(line.ParentNodeIndex(i));
  }

  return ret;
}

std::vector<int> ISpline::LineNodeIndices(int nIndex) const {
  assert(Order() == 1);
  return LinePointIndices(nIndex);
}

IElement::TDoubleVec ISpline::ShapeFunction(const IElement::TDoubleVec & /*isocoords*/) const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  return IElement::TDoubleVec();
}

CMatrix ISpline::ShapeFunctionDerived(const IElement::TDoubleVec & /*isocoords*/) const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  return CMatrix();
}

std::vector<IElement::TDoubleVec> ISpline::IsoCoordinates() const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  return std::vector<IElement::TDoubleVec>();
}

int ISpline::IntegrationPointSize() const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  return 0;
}

const IElement::TDoubleVec &ISpline::IntegrationPointCoords(int /*nIndex*/) const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  IElement::TDoubleVec *pBogus = 0;
  return *pBogus;
}

const double &ISpline::IntegrationPointWeight(int /*nIndex*/) const {
  // Can not be implemented because there is no unique definition
  // of isoparametric coordinates.
  // The calling function should be overridden for spline elements.
  assert(false);
  double *pBogus = 0;
  return *pBogus;
}

double ISpline::Size() const {
  double length = 0;
  assert(NrOfNodes() % 2 == 0);

  int i;
  for (i = 1; i < NrOfNodes() / 2; i++) {
    length += Node(i - 1).Distance(Node(i));
  }

  double width = Node(0).Distance(Node(NrOfNodes() - 1));

  return length * width;
}

CValue ISpline::InterpolateValue(const IPoint & /*point*/, const std::vector<CValue> & /*values*/,
                                 int /*iField*/) const {
  assert(false); // TODO base it on BSPF routine from DIANA
  return CValue();
}

/*!
  This function can not be used because there is no unique concept of isoparametric coordinates in a spline element.
*/
CValue ISpline::InterpolateValue(const TDoubleVec & /*isocoords*/, const std::vector<CValue> & /*values*/) const {
  assert(false);
  return CValue();
}

CValue ISpline::InterpolateValue(const IPoint &point, const std::vector<CValue> &values) const {
  assert(values.size() == NrOfNodes());

  int i;
  for (i = 0; i < NrOfNodes(); i++) {
    if (!values[i].Valid())
      return CValue();
  }

  int nf = NrOfFaces();
  for (i = 0; i < nf; i++) {
    if (Face(i).Contains(point, true)) {
      return InterpolateValue(point, values, i);
    }
  }

  return CValue();
}

double ISpline::RepresentativeLength() const { return Node(0).Distance(Node(NrOfNodes() - 1)); }

} // namespace geo
