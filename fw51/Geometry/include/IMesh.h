 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMesh.h: interface for the IMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMESH_H__E1218DC8_E4D1_459F_B3B4_65D70FC65905__INCLUDED_)
#define AFX_IMESH_H__E1218DC8_E4D1_459F_B3B4_65D70FC65905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IElementSet.h"
#include "NearestNeighbour.h"
#include "Tying.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

// IMesh is main class of all meshes. One can walk over the mesh elements
class CMeshNodeGroup;
class CBodyGroup;
class CFaceGroup;
class CElementGroup;
class ISupport;
class CTranslationSupport;
class CRotationSupport;
class CTying;
class INode;
class CTiedBodyTriangle;

class GEOMETRY_EXPORT  IMesh : public IElementSet
{
  std::vector<CTying*> m_vcTying;

  typedef std::pair<CTranslationSupport *, CRotationSupport *> TSupportPair;
  typedef std::map<int, TSupportPair> TSupportMap;
  TSupportMap m_SupportMap;

  std::vector<ISupport*> m_vcSupport; // for random access support
  const ISupport &CreateSupport(int nNodeNr, const IVector &direction, bool bTrans);

  std::vector<IElementSet*> m_vcElementGroup;
  std::vector<CMeshNodeGroup*> m_vcMeshNodeGroup;

public:
  typedef std::vector<int> TNodeVec;

  // Construction / Destruction ...
  IMesh();
  virtual ~IMesh();

  virtual bool IsMeshForEntry() const { return true; }

  // Registering elements and points in the mesher
  virtual int RegisterNode(const IPoint& point, bool bUnique = true, int nElements = 0) = 0;

  void GetElementTypeRanges(std::vector<std::pair<int, int> >& bodies, std::vector<std::pair<int, int> >& faults, int *pMaxBodyNodes = 0, int *pMaxFaultNodes = 0) const;
  virtual int RegisterElement(IElement& element) = 0;
  virtual void reserveElementSize(size_t size) = 0;

  virtual int RegisterTiedBodyTriangle(int nIndex, const CTiedBodyTriangle& tiedBodyTriangle) = 0;
  virtual const CTiedBodyTriangle& TiedBodyTriangle(int nIndex) = 0;

  virtual void AddElementGroup(IElementSet &elementset);
  virtual void RemoveElementGroup(IElementSet &elementset);

  virtual void AddNodeGroup(CMeshNodeGroup &nodegroup);
  virtual void RemoveNodeGroup(CMeshNodeGroup &nodegroup);

  // Element grouping in Diana. 
  virtual int ElementGroupSize() const;
  virtual const IElementSet& ElementGroup(int nElementGroupIndex) const;

  // Node grouping in Diana.
  virtual int NodeGroupSize() const;
  virtual const CMeshNodeGroup& NodeGroup(int nNodeGroupIndex) const;

  // Supports creation is controlled by the mesher
  virtual const ISupport &CreateTranslationSupport(int nNodeNr, const IVector &direction);
  virtual const ISupport &CreateRotationSupport(int nNodeNr, const IVector &direction);
  virtual void ClearSupports();

  // Supports in Diana
  virtual int SupportSize() const;
  virtual const ISupport& Support(int nSupportIndex) const;
  virtual bool HasTranslationSupport(int nNodeNr) const; // see if the specified node index is supported
  virtual const CTranslationSupport &TranslationSupport(int nNodeNr) const;
  virtual bool HasRotationSupport(int nNodeNr) const;
  virtual const CRotationSupport &RotationSupport(int nNodeNr) const;

  // Tyings in Diana
  virtual int TyingSize() const;
  virtual const CTying& Tying(int nTyingIndex) const;

  // Tyings creation is controlled by the mesher
  // single master
  const CTying &CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType,
    int nMaster, const IVector &vecMasterDir, CTying::TType nMasterType, double dFactor);

  // two masters
  const CTying &CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType,
    int nMaster0, const IVector &vecMaster0Dir, CTying::TType nMaster0Type, double dFactor0,
    int nMaster1, const IVector &vecMaster1Dir, CTying::TType nMaster1Type, double dFactor1);

  // any number of masters (must be greater than 0 of course...)
  const CTying &CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType,
    const std::vector<CTying::TMasterDef> &vcMasters);

  void ClearTyings();

  // mesh nodes
  virtual int NodeSize() const = 0;
  virtual const INode& Node(int nIndex) const = 0;
  virtual void Node(int nIndex, const IPoint& node) = 0;

  // IElementSet interface
  virtual int PointSize() const;
  virtual const IPoint& Point(int nIndex) const;

  // Clear the mesh
  virtual void Clear();

  // get the indices of the node(s) located at the given point
  virtual TNodeVec NodeIndex(const IPoint& point) = 0;

  // get the node indices connected to the given element
  virtual std::vector<int> Nodes(const IElement &element) const;

  virtual void AssertValid() const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleMesh(*this); }
};

}

#endif // !defined(AFX_IMESH_H__E1218DC8_E4D1_459F_B3B4_65D70FC65905__INCLUDED_)
