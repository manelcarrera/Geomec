/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMesh.cpp: implementation of the IMesh class.
//
//////////////////////////////////////////////////////////////////////
#include "IMesh.h"
#include "IElement.h"
#include "INode.h"
#include "ISupport.h"
#include "MeshNodeGroup.h"
#include "dimple.h"
// #include "MeshNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {
IMesh::IMesh() {}

IMesh::~IMesh() { Clear(); }

// Clear the mesh
void IMesh::Clear() {
  InvalidateCache();

  // Destroy supports
  ClearSupports();

  // Destroy those tyings
  ClearTyings();

  // Destroy all groups
  int i;
  for (i = 0; i < m_vcElementGroup.size(); i++)
    delete m_vcElementGroup[i];
  m_vcElementGroup.clear();

  for (i = 0; i < m_vcMeshNodeGroup.size(); i++)
    delete m_vcMeshNodeGroup[i];
  m_vcMeshNodeGroup.clear();
}

std::vector<int> IMesh::Nodes(const IElement &element) const {
  assert(element.IndexingElementSet() == this);
  std::vector<int> vcRet(element.NrOfPoints());
  for (int i = 0; i < element.NrOfPoints(); i++) {
    vcRet[i] = element.PointIndex(i);
  }

  return vcRet;
}

void IMesh::AssertValid() const {}

int IMesh::PointSize() const { return NodeSize(); }

const IPoint &IMesh::Point(int nIndex) const { return Node(nIndex); }

// Tyings in Diana
int IMesh::TyingSize() const { return (int)m_vcTying.size(); }

const CTying &IMesh::Tying(int nTyingIndex) const { return *m_vcTying[nTyingIndex]; }

// Supports in Diana
int IMesh::SupportSize() const { return (int)m_vcSupport.size(); }

const ISupport &IMesh::Support(int nSupportIndex) const {
  assert(nSupportIndex >= 0 && nSupportIndex < m_vcSupport.size());
  return *m_vcSupport[nSupportIndex];
}

bool IMesh::HasTranslationSupport(int nNodeNr) const // see if the specified node index is supported
{
  TSupportMap::const_iterator it = m_SupportMap.find(nNodeNr);
  if (it == m_SupportMap.end())
    return false;

  return (it->second.first != 0);
}

const CTranslationSupport &IMesh::TranslationSupport(int nNodeNr) const {
  assert(HasTranslationSupport(nNodeNr));
  TSupportMap::const_iterator it = m_SupportMap.find(nNodeNr);
  assert(it != m_SupportMap.end());
  assert(it->second.first != 0);

  return *it->second.first;
}

bool IMesh::HasRotationSupport(int nNodeNr) const {
  TSupportMap::const_iterator it = m_SupportMap.find(nNodeNr);
  if (it == m_SupportMap.end())
    return false;

  return (it->second.second != 0);
}

const CRotationSupport &IMesh::RotationSupport(int nNodeNr) const {
  assert(HasRotationSupport(nNodeNr));
  TSupportMap::const_iterator it = m_SupportMap.find(nNodeNr);
  assert(it != m_SupportMap.end());
  assert(it->second.second != 0);

  return *it->second.second;
}

const ISupport &IMesh::CreateTranslationSupport(int nNodeNr, const IVector &direction) {
  return CreateSupport(nNodeNr, direction, true);
}

const ISupport &IMesh::CreateRotationSupport(int nNodeNr, const IVector &direction) {
  return CreateSupport(nNodeNr, direction, false);
}

void IMesh::ClearSupports() {
  for (int i = 0; i < m_vcSupport.size(); i++)
    delete m_vcSupport[i];

  m_vcSupport.clear();
  m_SupportMap.clear();
}

// Tyings creation is controlled by the mesher
// single master
const CTying &IMesh::CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType, int nMaster,
                                 const IVector &vecMasterDir, CTying::TType nMasterType, double dFactor) {
  CTying *pTying = new CTying(*this, nSlave, vecSlaveDir, nSlaveType, nMaster, vecMasterDir, nMasterType, dFactor);
  m_vcTying.push_back(pTying);

  return *pTying;
}

// two masters
const CTying &IMesh::CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType, int nMaster0,
                                 const IVector &vecMaster0Dir, CTying::TType nMaster0Type, double dFactor0,
                                 int nMaster1, const IVector &vecMaster1Dir, CTying::TType nMaster1Type,
                                 double dFactor1) {
  CTying *pTying = new CTying(*this, nSlave, vecSlaveDir, nSlaveType, nMaster0, vecMaster0Dir, nMaster0Type, dFactor0,
                              nMaster1, vecMaster1Dir, nMaster1Type, dFactor1);
  m_vcTying.push_back(pTying);

  return *pTying;
}

// any number of masters (must be greater than 0 of course...)
const CTying &IMesh::CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType,
                                 const std::vector<CTying::TMasterDef> &vcMasters) {
  CTying *pTying = new CTying(*this, nSlave, vecSlaveDir, nSlaveType, vcMasters);

  m_vcTying.push_back(pTying);

  return *pTying;
}

void IMesh::ClearTyings() {
  for (int i = 0; i < m_vcTying.size(); i++)
    delete m_vcTying[i];
  m_vcTying.clear();
}

const ISupport &IMesh::CreateSupport(int nNodeNr, const IVector &direction, bool bTrans) {
  ISupport *pRet = 0;
  // see if a support for this node already exists
  TSupportMap::iterator it = m_SupportMap.find(nNodeNr);

  if (it == m_SupportMap.end()) {
    std::pair<TSupportMap::iterator, bool> ret;
    TSupportPair prNew;
    if (bTrans) {
      prNew.first = new CTranslationSupport(*this, nNodeNr, direction);
      prNew.second = 0;
      ret = m_SupportMap.insert(std::make_pair(nNodeNr, prNew));
      // store the new support for random access
      pRet = prNew.first;
    } else {
      prNew.first = 0;
      prNew.second = new CRotationSupport(*this, nNodeNr, direction);
      ret = m_SupportMap.insert(std::make_pair(nNodeNr, prNew));
      // store the new support for random access
      pRet = prNew.second;
    }

    m_vcSupport.push_back(pRet);
    assert(ret.second);
  } else {
    if (bTrans) {
      if (it->second.first != 0) {
        it->second.first->AddDirection(direction);
      } else {
        it->second.first = new CTranslationSupport(*this, nNodeNr, direction);
        m_vcSupport.push_back(it->second.first);
      }
      pRet = it->second.first;
    } else {
      if (it->second.second != 0) {
        it->second.second->AddDirection(direction);
      } else {
        it->second.second = new CRotationSupport(*this, nNodeNr, direction);
        m_vcSupport.push_back(it->second.second);
      }
      pRet = it->second.second;
    }
  }

  assert(pRet != 0);
  return *pRet;
}

int IMesh::ElementGroupSize() const { return (int)m_vcElementGroup.size(); }

const IElementSet &IMesh::ElementGroup(int nElementGroupIndex) const { return *m_vcElementGroup[nElementGroupIndex]; }

// Node grouping in Diana.
int IMesh::NodeGroupSize() const { return (int)m_vcMeshNodeGroup.size(); }

const CMeshNodeGroup &IMesh::NodeGroup(int nNodeGroupIndex) const {
  assert(nNodeGroupIndex >= 0);
  assert(nNodeGroupIndex < NodeGroupSize());

  return *m_vcMeshNodeGroup[nNodeGroupIndex];
}

void IMesh::AddElementGroup(IElementSet &elementset) { m_vcElementGroup.push_back(&elementset); }

void IMesh::RemoveElementGroup(IElementSet &elementset) {
  int i;
  for (i = 0; i < m_vcElementGroup.size(); i++) {
    if (m_vcElementGroup[i] == &elementset) {
      m_vcElementGroup.erase(m_vcElementGroup.begin() + i);
      break;
    }
  }
}

void IMesh::AddNodeGroup(CMeshNodeGroup &nodegroup) { m_vcMeshNodeGroup.push_back(&nodegroup); }

void IMesh::RemoveNodeGroup(CMeshNodeGroup &nodegroup) {
  int i;
  for (i = 0; i < m_vcMeshNodeGroup.size(); i++) {
    if (m_vcMeshNodeGroup[i] == &nodegroup) {
      m_vcMeshNodeGroup.erase(m_vcMeshNodeGroup.begin() + i);
      break;
    }
  }
}

void IMesh::GetElementTypeRanges(std::vector<std::pair<int, int>> &bodies, std::vector<std::pair<int, int>> &faults,
                                 int *pMaxBodyNodes, int *pMaxFaultNodes) const {
  if (ElementSize() == 0)
    return;

  const IElement &firstElm = Element(0);
  bool isFault = firstElm.IsInterfaceElement();

  int maxBodyNodes = 0;
  int maxFaultNodes = 0;

  if (isFault) {
    faults.push_back(std::make_pair(0, 1));
    maxFaultNodes = firstElm.NrOfNodes();
  } else {
    bodies.push_back(std::make_pair(0, 1));
    maxBodyNodes = firstElm.NrOfNodes();
  }

  for (int i = 1; i < ElementSize(); ++i) {
    const IElement &elm = Element(i);

    if (elm.IsInterfaceElement()) {
      int nodes = elm.NrOfNodes();
      if (nodes > maxFaultNodes)
        maxFaultNodes = nodes;

      if (isFault)
        ++faults.back().second;
      else {
        faults.push_back(std::make_pair(i, i + 1));
        isFault = true;
      }
    } else {
      int nodes = elm.NrOfNodes();
      if (nodes > maxBodyNodes)
        maxBodyNodes = nodes;

      if (!isFault)
        ++bodies.back().second;
      else {
        bodies.push_back(std::make_pair(i, i + 1));
        isFault = false;
      }
    }
  }

  if (pMaxBodyNodes)
    *pMaxBodyNodes = maxBodyNodes;
  if (pMaxFaultNodes)
    *pMaxFaultNodes = maxFaultNodes;
}

} // namespace geo
