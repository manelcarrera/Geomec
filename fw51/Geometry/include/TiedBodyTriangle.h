#ifndef _TIED_BODYTRIANGLE_H_
#define _TIED_BODYTRIANGLE_H_

#include "ITriangle.h"

#include "GeometryExports.h"

namespace geo {
class IMesh;
class IPoint;
class CBodyTriangle;

class GEOMETRY_EXPORT  CTiedBodyTriangle : public ITriangle
{
  IMesh& m_mesh;
  int    m_nIndex;
  int    m_nNodes[3];
  const CBodyTriangle *m_pTriangles[3];

public:
  CTiedBodyTriangle(IMesh& mesh, std::vector<int> vcNodes, std::vector<const CBodyTriangle *> vcTriangles);
  CTiedBodyTriangle(IMesh& mesh, int nIndex, std::vector<int> vcNodes, std::vector<const CBodyTriangle *> vcTriangles);
  ~CTiedBodyTriangle();

  int Index() const { return m_nIndex; }
  void Index(int nIndex) { m_nIndex = nIndex; }

  const IBody *Parent() const;
  const CBodyTriangle *BodyTriangle(int nIndex) const;

  virtual int PointIndex(int nIndex) const;
  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint& point);

  virtual size_t Order() const;

};

} // namespace geo

#endif
