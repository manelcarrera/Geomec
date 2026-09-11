 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MeshNodeGroup.h: interface for the CMeshNodeGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHNODEGROUP_H__AA63D65C_C7B0_4C88_9477_A0AFC5D8D385__INCLUDED_)
#define AFX_MESHNODEGROUP_H__AA63D65C_C7B0_4C88_9477_A0AFC5D8D385__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "IObject.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

class IMesh;
class INode;
class GEOMETRY_EXPORT  CMeshNodeGroup : public IObject  
{
  IMesh& m_mesh;
  std::vector<const INode*> m_vcNode;
  int m_nIndex;

public:
  CMeshNodeGroup(IMesh& mesh);
  virtual ~CMeshNodeGroup();

  int Index() const;

  bool AddMeshNode(const INode& node);

  int NodeSize() const;
  const INode &Node(int nIndex) const;

  //Overrides of IObject
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  virtual bool Visit(IVisitor &visitor)
  {
    assert(dynamic_cast<CVisitorBase *>(&visitor));
    return static_cast<CVisitorBase &>(visitor).HandleMeshNodeGroup(*this);
  }
};

}

#endif // !defined(AFX_MESHNODEGROUP_H__AA63D65C_C7B0_4C88_9477_A0AFC5D8D385__INCLUDED_)
