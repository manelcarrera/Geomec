#include "GocadData.h"

#include <cassert>

#include "StreamVersion.h"

#ifdef SKUA_NEW
#include "SKUAParseData.h"

#include <tbb/parallel_sort.h>
#endif

namespace {

template <typename V, typename R>
void loadVectorRef(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress, V &vector,
                   R &ref) {
  int size = 0;

  stream >> size;
  vector.resize(size);

  for (int i = 0; i < size; ++i) {
    typename V::value_type value(typename V::value_type(new typename V::value_type::value_type()));

    value->LoadStream(stream, version, progress, ref);
    vector[i] = value;
  }
}

template <typename V>
void loadBasicVector(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                     CStorageNode::TPROGRESS & /*progress*/, V &vector) {
  int size = 0;

  stream >> size;

  vector.resize(size);

  for (int i = 0; i < size; ++i) {
    stream >> vector[i];
  }
}

void loadVectorAtomRef(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                       std::vector<const CGocadData::CAtom *> &vector, CGocadData::IGoCadTypes &parent) {
  int size = 0;
  stream >> size;

  vector.reserve(size);

  if (version < CStreamVersion(4, 1, 12)) {
    // complete data instead of reference was erroneously written, so now we have to dummy read that data

    std::vector<std::vector<double>> dummy;
    int id;

    for (int i = 0; i < size; ++i) {
      stream >> id;

      int numberOfOuterProperties = 0;
      stream >> numberOfOuterProperties;

      if (numberOfOuterProperties > 0) {
        dummy.resize(numberOfOuterProperties);

        for (int j = 0; j < numberOfOuterProperties; ++j) {
          loadBasicVector(stream, version, progress, dummy[j]);
        }
      }

      int dummy_ref;
      stream >> dummy_ref;

      const CGocadData::CAtom *pAtom = parent.AtomID(id);

      assert(pAtom);
      dummy.clear();

      vector.push_back(pAtom);
    }
  } else {
    int id;

    for (int i = 0; i < size; ++i) {
      stream >> id;

      const CGocadData::CAtom *pAtom = parent.AtomID(id);

      assert(pAtom);

      vector.push_back(pAtom);
    }
  }
}

void loadVectorVertexRef(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                         std::vector<const CGocadData::CVertex *> &vector, CGocadData::IGoCadTypes &parent) {
  int size = 0;
  stream >> size;

  vector.reserve(size);

  if (version < CStreamVersion(4, 1, 12)) {
    // complete data instead of reference was erroneously written, so now we have to dummy read that data

    std::vector<std::vector<double>> dummy;
    int id;

    for (int i = 0; i < size; ++i) {
      stream >> id;

      int numberOfOuterProperties = 0;
      stream >> numberOfOuterProperties;

      if (numberOfOuterProperties > 0) {
        dummy.resize(numberOfOuterProperties);

        for (int j = 0; j < numberOfOuterProperties; ++j) {
          loadBasicVector(stream, version, progress, dummy[j]);
        }
      }

      int numberOfCoord = 0;
      stream >> numberOfCoord;

      double dummy_coord;

      for (int j = 0; j < numberOfCoord; ++j) {
        stream >> dummy_coord;
      }

      const CGocadData::CVertex *pVertex = parent.VertexID(id);

      assert(pVertex);
      dummy.clear();

      vector.push_back(pVertex);
    }
  } else {
    int id;

    for (int i = 0; i < size; ++i) {
      stream >> id;

      const CGocadData::CVertex *pVertex = parent.VertexID(id);

      assert(pVertex);

      vector.push_back(pVertex);
    }
  }
}

template <typename N, typename R>
void loadNodesRef(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress, N &nodes,
                  R &ref) {
  int numberOfNodes = 0;
  stream >> numberOfNodes;

  if (version < CStreamVersion(4, 1, 12)) {
    // complete data instead of reference was erroneously written, so now we have to dummy read that data
    // Note: LoadStream of base class CNode was called, so we miss the CAtom/CVertex dummies

    std::vector<std::vector<double>> dummy;

    const CGocadData::CNode *node = 0;
    int what, id;

    for (int i = 0; i < numberOfNodes; ++i) {
      stream >> what;
      stream >> id;

      int numberOfOuterProperties = 0;
      stream >> numberOfOuterProperties;

      if (numberOfOuterProperties > 0) {
        dummy.resize(numberOfOuterProperties);

        for (int j = 0; j < numberOfOuterProperties; ++j) {
          loadBasicVector(stream, version, progress, dummy[j]);
        }
      }

      if (what == CGocadData::IS_VERTEX) {
        node = ref.VertexID(id);
      } else if (what == CGocadData::IS_ATOM) {
        node = ref.AtomID(id);
      } else {
        assert(false);
      }

      assert(node);
      dummy.clear();

      nodes[i] = node;
    }
  } else {
    const CGocadData::CNode *node;
    int id;

    for (int i = 0; i < numberOfNodes; ++i) {
      stream >> id;

      node = ref.VertexID(id);

      if (!node) {
        node = ref.AtomID(id);
      }

      assert(node);

      nodes[i] = node;
    }
  }
}

template <typename Type, typename MAP>
void loadMap(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress, MAP &map,
             CGocadData::IGoCadTypes &parent) {
  int size = 0;

  stream >> size;

  for (int i = 0; i < size; ++i) {
    int key;

    stream >> key;

    Type *value = new Type;

    value->LoadStream(stream, version, progress, parent);
    map[key] = value;
  }
}

template <typename V> void saveVector(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress, V &vector) {
  int size = vector.size();

  stream << size;

  for (int i = 0; i < size; ++i) {
    vector[i]->SaveStream(stream, progress);
  }
}

template <typename V>
void saveVectorRef(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/, V &vector) {
  int size = vector.size();

  stream << size;

  for (int i = 0; i < size; ++i) {
    stream << vector[i]->ID();
  }
}

template <typename V>
void saveBasicVector(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/, V &vector) {
  int size = vector.size();

  stream << size;

  for (int i = 0; i < size; ++i) {
    stream << vector[i];
  }
}

template <typename N>
void saveNodesRef(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/, N &nodes) {
  int numberOfNodes = sizeof(nodes) / sizeof(CGocadData::CNode *);

  stream << numberOfNodes;

  for (int i = 0; i < numberOfNodes; ++i) {
    stream << nodes[i]->ID();
  }
}

template <typename M> void saveMap(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress, M &map) {
  int size = map.size();

  stream << size;

  for (typename M::const_iterator iterator = map.begin(); iterator != map.end(); ++iterator) {
    stream << iterator->first;
    iterator->second->SaveStream(stream, progress);
  }
}

void fillNodes(std::vector<const CGocadData::CNode *> &nodes, CGocadData::IGoCadTypes::TAtomMap &atomIDs,
               CGocadData::IGoCadTypes::TVertexMap &vertexIDs) {
  for (CGocadData::IGoCadTypes::TVertexMap::const_iterator vertex = vertexIDs.begin(); vertex != vertexIDs.end();
       ++vertex) {
    nodes.push_back(vertex->second);
  }

  for (CGocadData::IGoCadTypes::TAtomMap::const_iterator atom = atomIDs.begin(); atom != atomIDs.end(); ++atom) {
    nodes.push_back(atom->second);
  }
}

} // anonymous namespace

///// CNode

bool CGocadData::CNode::IsEqualTo(const CNode &node) const { return (Pointee() == &node || node.Pointee() == this); }

int CGocadData::CNode::ID() const { return m_nID; }

int CGocadData::CNode::PropertySize() const { return m_vcProperties.size(); }

const std::vector<double> &CGocadData::CNode::Property(int i) const {
  assert(i >= 0 && i < m_vcProperties.size());
  return m_vcProperties[i];
}

void CGocadData::CNode::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                   CStorageNode::TPROGRESS &progress) {
  stream >> m_nID;

  int numberOfOuterProperties = 0;

  stream >> numberOfOuterProperties;

  m_vcProperties.resize(numberOfOuterProperties);

  for (int i = 0; i < numberOfOuterProperties; ++i) {
    loadBasicVector(stream, version, progress, m_vcProperties[i]);
  }
}

void CGocadData::CNode::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const {
  stream << m_nID;

  int numberOfOuterProperties = m_vcProperties.size();

  stream << numberOfOuterProperties;

  for (int i = 0; i < numberOfOuterProperties; ++i) {
    saveBasicVector(stream, progress, m_vcProperties[i]);
  }
}

///// CVertex
double CGocadData::CVertex::X() const { return m_dCoord[0]; }

double CGocadData::CVertex::Y() const { return m_dCoord[1]; }

double CGocadData::CVertex::Z() const { return m_dCoord[2]; }

void CGocadData::CVertex::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                     CStorageNode::TPROGRESS &progress, IGoCadTypes & /* parent */) {
  CGocadData::CNode::LoadStream(stream, version, progress);

  int numberOfCoord = 0;

  stream >> numberOfCoord;

  for (int i = 0; i < numberOfCoord; ++i) {
    stream >> m_dCoord[i];
  }
}

void CGocadData::CVertex::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const {
  CGocadData::CNode::SaveStream(stream, progress);

  int numberOfCoord = sizeof(m_dCoord) / sizeof(double);

  stream << numberOfCoord;

  for (int i = 0; i < numberOfCoord; ++i) {
    stream << m_dCoord[i];
  }
}

///// CAtom
double CGocadData::CAtom::X() const {
  assert(m_pVertex);
  return m_pVertex->X();
}

double CGocadData::CAtom::Y() const {
  assert(m_pVertex);
  return m_pVertex->Y();
}

double CGocadData::CAtom::Z() const {
  assert(m_pVertex);
  return m_pVertex->Z();
}

const CGocadData::CNode *CGocadData::CAtom::Pointee() const { return m_pVertex; }

const CGocadData::CVertex &CGocadData::CAtom::Vertex() const {
  assert(m_pVertex);
  return *m_pVertex;
}

void CGocadData::CAtom::setVertex(const CVertex *vertex) { m_pVertex = vertex; }

void CGocadData::CAtom::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                   CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  CGocadData::CNode::LoadStream(stream, version, progress);

  int id;
  stream >> id;

  m_pVertex = parent.VertexID(id);
}

void CGocadData::CAtom::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const {
  CGocadData::CNode::SaveStream(stream, progress);

  stream << m_pVertex->m_nID;
}

///// CSharedVertex
double CGocadData::CSharedVertex::X() const {
  assert(m_pVertex);
  return m_pVertex->X();
}

double CGocadData::CSharedVertex::Y() const {
  assert(m_pVertex);
  return m_pVertex->Y();
}

double CGocadData::CSharedVertex::Z() const {
  assert(m_pVertex);
  return m_pVertex->Z();
}

const CGocadData::CNode *CGocadData::CSharedVertex::Pointee() const { return m_pVertex; }

const CGocadData::CVertex &CGocadData::CSharedVertex::Vertex() const {
  assert(m_pVertex);
  return *m_pVertex;
}

void CGocadData::CSharedVertex::setVertex(const CVertex *vertex) { m_pVertex = vertex; }

void CGocadData::CSharedVertex::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                           CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  CGocadData::CNode::LoadStream(stream, version, progress);

  int id;
  stream >> id;

  m_pVertex = parent.VertexID(id);
}

void CGocadData::CSharedVertex::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const {
  CGocadData::CNode::SaveStream(stream, progress);

  stream << m_pVertex->m_nID;
}

///// CTetra
const CGocadData::CNode &CGocadData::CTetra::Node(int i) const {
  assert(i >= 0 && i < 4);
  assert(m_Nodes[i]);
  return *m_Nodes[i];
}

int CGocadData::CTetra::PropertySize() const { return m_vcProperties.size(); }

const std::vector<double> &CGocadData::CTetra::Property(int i) const {
  assert(i >= 0 && i < m_vcProperties.size());
  return m_vcProperties[i];
}

void CGocadData::CTetra::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                    CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  loadNodesRef(stream, version, progress, m_Nodes, parent);

  int numberOfOuterProperties = 0;

  stream >> numberOfOuterProperties;

  m_vcProperties.resize(numberOfOuterProperties);

  for (int i = 0; i < numberOfOuterProperties; ++i) {
    loadBasicVector(stream, version, progress, m_vcProperties[i]);
  }
}

void CGocadData::CTetra::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  saveNodesRef(stream, progress, m_Nodes);

  int numberOfOuterProperties = m_vcProperties.size();

  stream << numberOfOuterProperties;

  for (int i = 0; i < numberOfOuterProperties; ++i) {
    saveBasicVector(stream, progress, m_vcProperties[i]);
  }
}

///// CTriangle

CGocadData::CTriangle::CTriangle(const CTriangle &rhs) {
  for (size_t s = 0; s < 3; ++s) {
    m_Nodes[s] = rhs.m_Nodes[s];
  }
}

const CGocadData::CNode &CGocadData::CTriangle::Node(int i) const {
  assert(i >= 0 && i < 3);
  assert(m_Nodes[i]);
  return *m_Nodes[i];
}

void CGocadData::CTriangle::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                       CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  loadNodesRef(stream, version, progress, m_Nodes, parent);
}

void CGocadData::CTriangle::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  saveNodesRef(stream, progress, m_Nodes);
}

///// CTFace
CGocadData::CTFace::~CTFace() {}

const QString &CGocadData::CTFace::Name() const { return m_strName; }

size_t CGocadData::CTFace::TriangleSize() const { return m_vcTriangles.size(); }

const CGocadData::CTriangle &CGocadData::CTFace::Triangle(size_t i) const {
  assert(i < m_vcTriangles.size());
  return *m_vcTriangles[i];
}

void CGocadData::CTFace::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                    CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  stream >> m_strName;

  loadVectorRef(stream, version, progress, m_vcTriangles, parent);
}

void CGocadData::CTFace::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << m_strName;

  saveVector(stream, progress, m_vcTriangles);
}

///// CSurface
CGocadData::CSurface::CSurface() : m_bDoubleSidedFaultSurface(false) {}

CGocadData::CSurface::~CSurface() {}

const QString &CGocadData::CSurface::Name() const { return m_strName; }

int CGocadData::CSurface::TFaceSize() const { return m_vcTFaces.size(); }

const CGocadData::CTFace &CGocadData::CSurface::TFace(int i) const {
  assert(i >= 0 && i < m_vcTFaces.size());
  return *m_vcTFaces[i];
}

bool CGocadData::CSurface::DoubleSidedFaultSurface() const { return m_bDoubleSidedFaultSurface; }

void CGocadData::CSurface::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                      CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  stream >> m_strName;

  loadVectorRef(stream, version, progress, m_vcTFaces, parent);

  // TODO: load m_bDoubleSidedFaultSurface;
}

void CGocadData::CSurface::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << m_strName;

  saveVector(stream, progress, m_vcTFaces);

  // TODO: save m_bDoubleSidedFaultSurface;
}

///// CTVolume
CGocadData::CTVolume::CTVolume() : m_bDoubleSidedFaultVolume(false) {}

CGocadData::CTVolume::~CTVolume() {}

const QString &CGocadData::CTVolume::Name() const { return m_strName; }

int CGocadData::CTVolume::VertexSize() const { return m_vcVertices.size(); }

const CGocadData::CVertex &CGocadData::CTVolume::Vertex(int i) const {
  assert(i >= 0 && i < m_vcVertices.size());
  return *m_vcVertices[i];
}

int CGocadData::CTVolume::AtomSize() const { return m_vcAtoms.size(); }

const CGocadData::CAtom &CGocadData::CTVolume::Atom(int i) const {
  assert(i >= 0 && i < m_vcAtoms.size());
  return *m_vcAtoms[i];
}

int CGocadData::CTVolume::TetraSize() const { return m_vcTetras.size(); }

const CGocadData::CTetra &CGocadData::CTVolume::Tetra(int i) const {
  assert(i >= 0 && i < m_vcTetras.size());
  return *m_vcTetras[i];
}

bool CGocadData::CTVolume::DoubleSidedFaultVolume() const { return m_bDoubleSidedFaultVolume; }

void CGocadData::CTVolume::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                      CStorageNode::TPROGRESS &progress, IGoCadTypes &parent) {
  stream >> m_strName;

  loadVectorVertexRef(stream, version, progress, m_vcVertices, parent);
  loadVectorAtomRef(stream, version, progress, m_vcAtoms, parent);
  loadVectorRef(stream, version, progress, m_vcTetras, parent);

  // TODO: load m_bDoubleSidedFaultVolume
}

void CGocadData::CTVolume::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << m_strName;

  saveVectorRef(stream, progress, m_vcVertices);
  saveVectorRef(stream, progress, m_vcAtoms);
  saveVector(stream, progress, m_vcTetras);

  // TODO: save m_bDoubleSidedFaultVolume
}

///// IGoCadTypes
const QString &CGocadData::IGoCadTypes::Name() const { return m_strName; }

CGocadData::IGoCadTypes::IGoCadTypes() : m_dZFactor(1.), m_strName(), m_vcPropertyNames() {
  m_dUnitFactor[0] = 1.;
  m_dUnitFactor[1] = 1.;
  m_dUnitFactor[2] = 1.;
}

CGocadData::IGoCadTypes::~IGoCadTypes() {
  for (std::vector<const CNode *>::iterator it = m_vcNodes.begin(); it != m_vcNodes.end(); ++it)
    delete (*it);
  for (TSharedVertexMap::iterator it = m_mpSharedVertexIDs.begin(); it != m_mpSharedVertexIDs.end(); ++it)
    delete it->second;
}

int CGocadData::IGoCadTypes::PropertiesSize() const { return m_vcPropertyNames.size(); }

const QString &CGocadData::IGoCadTypes::PropertyName(int i) const {
  assert(i >= 0 && i < m_vcPropertyNames.size());
  return m_vcPropertyNames[i];
}

double CGocadData::IGoCadTypes::ZFactor() const { return m_dZFactor; }

double CGocadData::IGoCadTypes::UnitFactor(int i) const {
  assert((0 <= i) && (i < 3));

  return m_dUnitFactor[i];
}

int CGocadData::IGoCadTypes::VertexIDsSize() const { return m_mpVertexIDs.size(); }

const CGocadData::CVertex *CGocadData::IGoCadTypes::VertexID(int i) const {
  TVertexMap::const_iterator iterator = m_mpVertexIDs.find(i);

  return (iterator != m_mpVertexIDs.end()) ? iterator->second : 0;
}

int CGocadData::IGoCadTypes::AtomIDsSize() const { return m_mpAtomIDs.size(); }

const CGocadData::CAtom *CGocadData::IGoCadTypes::AtomID(int i) const {
  TAtomMap::const_iterator iterator = m_mpAtomIDs.find(i);

  return (iterator != m_mpAtomIDs.end()) ? iterator->second : 0;
}

int CGocadData::IGoCadTypes::SharedVertexIDsSize() const { return m_mpSharedVertexIDs.size(); }

const CGocadData::CSharedVertex *CGocadData::IGoCadTypes::SharedVertexID(int i) const {
  TSharedVertexMap::const_iterator iterator = m_mpSharedVertexIDs.find(i);

  return (iterator != m_mpSharedVertexIDs.end()) ? iterator->second : 0;
}

const CGocadData::CNode *CGocadData::IGoCadTypes::NodeID(int i) const {
  const CGocadData::CNode *pNode = VertexID(i);

  if (!pNode)
    pNode = AtomID(i);

  if (!pNode)
    pNode = SharedVertexID(i);

  return pNode;
}

void CGocadData::IGoCadTypes::AddNode(const CVertex *pVertex) {
  m_mpVertexIDs.insert(TVertexMap::value_type(pVertex->m_nID, pVertex));
  m_vcNodes.push_back(pVertex);
}

void CGocadData::IGoCadTypes::AddNode(const CAtom *pAtom) {
  m_mpAtomIDs.insert(TAtomMap::value_type(pAtom->m_nID, pAtom));
  m_vcNodes.push_back(pAtom);
}

void CGocadData::IGoCadTypes::AddNode(const CSharedVertex *pSharedVertex) {
  m_mpSharedVertexIDs.insert(TSharedVertexMap::value_type(pSharedVertex->m_nID, pSharedVertex));
}

void CGocadData::IGoCadTypes::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                         CStorageNode::TPROGRESS &progress) {
  stream >> m_dZFactor;

  int numberOfUnitFactor = 0;

  stream >> numberOfUnitFactor;

  for (int i = 0; i < numberOfUnitFactor; ++i) {
    stream >> m_dUnitFactor[i];
  }

  stream >> m_strName;

  loadBasicVector(stream, version, progress, m_vcPropertyNames);
  loadBasicVector(stream, version, progress, m_vcNoDataValues);
  loadBasicVector(stream, version, progress, m_vcESizes);
  loadBasicVector(stream, version, progress, m_vcTetraPropertyNames);
  loadBasicVector(stream, version, progress, m_vcTetraNoDataValues);
  loadBasicVector(stream, version, progress, m_vcTetraESizes);
  loadMap<CVertex>(stream, version, progress, m_mpVertexIDs, *this);
  loadMap<CAtom>(stream, version, progress, m_mpAtomIDs, *this);

  if (version > CStreamVersion(4, 1, 55)) {
    loadMap<CSharedVertex>(stream, version, progress, m_mpSharedVertexIDs, *this);
  }

  // fillVertexReferences(m_mpAtomIDs, m_mpVertexIDs);
  fillNodes(m_vcNodes, m_mpAtomIDs, m_mpVertexIDs);
}

void CGocadData::IGoCadTypes::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << m_dZFactor;

  int numberOfUnitFactor = sizeof(m_dUnitFactor) / sizeof(double);

  stream << numberOfUnitFactor;

  for (int i = 0; i < numberOfUnitFactor; ++i) {
    stream << m_dUnitFactor[i];
  }

  stream << m_strName;

  saveBasicVector(stream, progress, m_vcPropertyNames);
  saveBasicVector(stream, progress, m_vcNoDataValues);
  saveBasicVector(stream, progress, m_vcESizes);
  saveBasicVector(stream, progress, m_vcTetraPropertyNames);
  saveBasicVector(stream, progress, m_vcTetraNoDataValues);
  saveBasicVector(stream, progress, m_vcTetraESizes);
  saveMap(stream, progress, m_mpVertexIDs);
  saveMap(stream, progress, m_mpAtomIDs);
  saveMap(stream, progress, m_mpSharedVertexIDs);
}

///// CTSolid
CGocadData::CTSolid::CTSolid(bool bLightTSolid) : m_type(bLightTSolid ? LightTSolid : TSolid) {}

CGocadData::CTSolid::~CTSolid() {}

CGocadData::CTSolid::TSolidType CGocadData::CTSolid::Type() const { return m_type; }

int CGocadData::CTSolid::NoDataValueSize() const { return m_vcNoDataValues.size(); }

double CGocadData::CTSolid::NoDataValue(int i) const {
  assert(i >= 0 && i < m_vcNoDataValues.size());
  return m_vcNoDataValues[i];
}

int CGocadData::CTSolid::ESizeSize() const { return m_vcESizes.size(); }

int CGocadData::CTSolid::ESize(int i) const {
  assert(i >= 0 && i < m_vcESizes.size());
  return m_vcESizes[i];
}

int CGocadData::CTSolid::TetraPropertiesSize() const { return m_vcTetraPropertyNames.size(); }

const QString &CGocadData::CTSolid::TetraPropertyName(int i) const {
  assert(i >= 0 && i < m_vcTetraPropertyNames.size());
  return m_vcTetraPropertyNames[i];
}

int CGocadData::CTSolid::TetraNoDataValueSize() const { return m_vcTetraNoDataValues.size(); }

double CGocadData::CTSolid::TetraNoDataValue(int i) const {
  assert(i >= 0 && i < m_vcTetraNoDataValues.size());
  return m_vcTetraNoDataValues[i];
}

int CGocadData::CTSolid::TetraESizeSize() const { return m_vcTetraESizes.size(); }

int CGocadData::CTSolid::TetraESize(int i) const {
  assert(i >= 0 && i < m_vcTetraESizes.size());
  return m_vcTetraESizes[i];
}

int CGocadData::CTSolid::VolumeSize() const { return m_vcVolumes.size(); }

const CGocadData::CTVolume &CGocadData::CTSolid::Volume(int i) const {
  assert(i >= 0 && i < m_vcVolumes.size());
  return *m_vcVolumes[i];
}

int CGocadData::CTSolid::SurfaceSize() const { return m_vcSurfaces.size(); }

const CGocadData::CSurface &CGocadData::CTSolid::Surface(int i) const {
  assert(i >= 0 && i < m_vcSurfaces.size());
  return *m_vcSurfaces[i];
}

void CGocadData::CTSolid::MinMaxNodeIDs(int &minID, int &maxID) const {
  minID = INT_MAX;
  maxID = INT_MIN;

  for (TVertexMap::const_iterator it = m_mpVertexIDs.begin(); it != m_mpVertexIDs.end(); ++it) {
    int id = it->second->ID();
    if (id < minID)
      minID = id;
    if (id > maxID)
      maxID = id;
  }
  for (TAtomMap::const_iterator it = m_mpAtomIDs.begin(); it != m_mpAtomIDs.end(); ++it) {
    int id = it->second->ID();
    if (id < minID)
      minID = id;
    if (id > maxID)
      maxID = id;
  }
  for (TSharedVertexMap::const_iterator it = m_mpSharedVertexIDs.begin(); it != m_mpSharedVertexIDs.end(); ++it) {
    int id = it->second->ID();
    if (id < minID)
      minID = id;
    if (id > maxID)
      maxID = id;
  }
}

void CGocadData::CTSolid::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                     CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::LoadStream(stream, version, progress);

  loadVectorRef(stream, version, progress, m_vcVolumes, *this);
  loadVectorRef(stream, version, progress, m_vcSurfaces, *this);
}

void CGocadData::CTSolid::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::SaveStream(stream, progress);

  saveVector(stream, progress, m_vcVolumes);
  saveVector(stream, progress, m_vcSurfaces);
}

void CGocadData::CVset::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                   CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::LoadStream(stream, version, progress);
}

void CGocadData::CVset::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::SaveStream(stream, progress);
}

void CGocadData::CTSurf::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                    CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::LoadStream(stream, version, progress);

  m_pSurface->LoadStream(stream, version, progress, *this);
}

void CGocadData::CTSurf::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  CGocadData::IGoCadTypes::SaveStream(stream, progress);

  m_pSurface->SaveStream(stream, progress);
}

#ifdef SKUA_NEW
namespace gm_skua {

// transform functions to go from the old CGocadData to the new SKUAParseData
// these can be long, but are straight-forward: the old format was object hierarchy/pointer based, the new one is
// flat/vector based

void transform(const CGocadData::CNode &old_data, struct SKUAParseData::Vertex &new_data) {
  new_data.id = old_data.ID();

  if (old_data.Type() == CGocadData::IS_VERTEX) {
    new_data.coordinate[0] = old_data.Y(); // SKUA has X/Y switched, and we keep it as in SKUA
    new_data.coordinate[1] = old_data.X();
    new_data.coordinate[2] = old_data.Z();
  } else {
    new_data.link = old_data.Vertex().ID();
  }
}

void transform(const CGocadData::CTVolume &old_data, struct SKUAParseData &new_data) {
  new_data.volumes.emplace_back(std::vector<int>());
  std::vector<int> &volumes = new_data.volumes.back();
  volumes.resize(old_data.TetraSize());

  new_data.volume_types.emplace_back(old_data.DoubleSidedFaultVolume() ? SKUAParseData::VolumeType::VolDSFVolume
                                                                       : SKUAParseData::VolumeType::VolNormal);

  int model_region = (int)new_data.model_region_names.size();

  for (int i = 0; i < old_data.TetraSize(); ++i) {
    volumes[i] = (int)new_data.tetras.size();

    new_data.tetras.emplace_back(SKUAParseData::Tetra());

    const CGocadData::CTetra &old_tetra = old_data.Tetra(i);
    SKUAParseData::Tetra &new_tetra = new_data.tetras.back();

    for (int j = 0; j < 4; ++j) {
      new_tetra.model_region = model_region;
      new_tetra.node[j] = new_data.find_node(old_tetra.Node(j).ID());
      new_tetra.opposite_signs[j] = 1;
      new_tetra.opposite_types[j] = 0;
      new_tetra.opposite_faces[j] = -1;
    }
  }

  new_data.model_region_names.push_back(SKUAParseData::String(model_region, old_data.Name().toStdString().c_str()));
}

void transform(const CGocadData::CSurface &old_data, struct SKUAParseData &new_data) {
  new_data.surface_names.emplace_back(
      SKUAParseData::String((int)new_data.surface_names.size(), old_data.Name().toStdString().c_str()));

  int type = old_data.DoubleSidedFaultSurface() ? SKUAParseData::SurfaceType::SurfDSFFault
                                                : SKUAParseData::SurfaceType::SurfUnknown;

  new_data.surfaces.emplace_back(std::vector<SKUAParseData::Triangle>());
  new_data.surface_faces.emplace_back(std::vector<SKUAParseData::Face>());
  new_data.surface_types.emplace_back(type);

  std::vector<SKUAParseData::Triangle> &surface = new_data.surfaces.back();
  std::vector<SKUAParseData::Face> &faces = new_data.surface_faces.back();
  faces.reserve(old_data.TFaceSize());

  std::vector<SKUAParseData::String> &face_names = new_data.face_names;

  size_t triangle_size = 0;

  for (int i = 0; i < old_data.TFaceSize(); ++i)
    triangle_size += old_data.TFace(i).TriangleSize();

  surface.reserve(triangle_size);

  for (int i = 0; i < old_data.TFaceSize(); ++i) {
    const CGocadData::CTFace &old_face = old_data.TFace(i);

    int old_triangle_size = old_face.TriangleSize();

    if (old_triangle_size > 0) {
      faces.emplace_back(SKUAParseData::Face());

      SKUAParseData::Face &new_face = faces.back();

      new_face.id = (int)face_names.size();
      face_names.emplace_back(SKUAParseData::String(new_face.id, old_face.Name().toStdString().c_str()));

      for (int j = 0; j < 3; ++j)
        new_face.key_vertices[j] = new_data.find_node(old_face.Triangle(0).Node(j).ID());

      new_face.triangle = (int)surface.size();
      new_face.type = type;

      for (int j = 0; j < old_triangle_size; ++j) {
        const CGocadData::CTriangle &old_triangle = old_face.Triangle(j);

        surface.emplace_back(SKUAParseData::Triangle());

        ++new_face.size;

        SKUAParseData::Triangle &new_triangle = surface.back();

        for (int k = 0; k < 3; ++k)
          new_triangle.node[k] = new_data.find_node(old_triangle.Node(k).ID());
      }
    }
  }
}

void transform_vertex_properties(const CGocadData::CTSolid &old_data, struct SKUAParseData &new_data, int minID,
                                 int maxID) {
  SKUAParseData::PropertyData &props = new_data.vertex_props;

  props.size = old_data.PropertiesSize();

  if (props.size == 0)
    return;

  props.names.reserve(props.size);
  props.esizes.reserve(props.size);
  props.no_data_values.reserve(props.size);
  props.units.reserve(props.size);

  for (int i = 0; i < old_data.PropertiesSize(); ++i)
    props.names.emplace_back(SKUAParseData::String(i, old_data.PropertyName(i).toStdString().c_str()));
  for (int i = 0; i < old_data.ESizeSize(); ++i)
    props.esizes.emplace_back(old_data.ESize(i));
  for (int i = 0; i < old_data.NoDataValueSize(); ++i)
    props.no_data_values.emplace_back(old_data.NoDataValue(i));
  for (int i = 0; i < old_data.PropertiesSize(); ++i)
    props.units.emplace_back(SKUAParseData::String(i, ""));

  size_t prop_size = 0;

  std::for_each(props.esizes.begin(), props.esizes.end(), [&](int esize) { prop_size += esize; });

  props.values.reserve(prop_size);

  for (int i = 0; i < prop_size; ++i) {
    props.values.emplace_back(std::vector<double>());
    props.values.back().reserve(new_data.vertices.size());
  }

  size_t value_index = 0;

  for (int i = minID; i <= maxID; ++i) {
    const CGocadData::CNode *old_node = old_data.NodeID(i);

    if (!old_node)
      continue;

    size_t prop_index = 0;
    for (int j = 0; j < old_node->PropertySize(); ++j) {
      const std::vector<double> &old_values = old_node->Property(j);

      for (std::vector<double>::const_iterator it = old_values.begin(); it != old_values.end(); ++it) {
        props.values[prop_index++].emplace_back(*it);
      }
    }
    ++value_index;
  }
}

void transform_tetra_properties(const CGocadData::CTSolid &old_data, struct SKUAParseData &new_data) {
  SKUAParseData::PropertyData &props = new_data.tetra_props;

  props.size = old_data.TetraPropertiesSize();

  if (props.size == 0)
    return;

  props.names.reserve(props.size);
  props.esizes.reserve(props.size);
  props.no_data_values.reserve(props.size);
  props.units.reserve(props.size);
  props.values.reserve(props.size);

  for (int i = 0; i < old_data.TetraPropertiesSize(); ++i)
    props.names.emplace_back(SKUAParseData::String(i, old_data.TetraPropertyName(i).toStdString().c_str()));
  for (int i = 0; i < old_data.TetraESizeSize(); ++i)
    props.esizes.emplace_back(old_data.TetraESize(i));
  for (int i = 0; i < old_data.TetraNoDataValueSize(); ++i)
    props.no_data_values.emplace_back(old_data.TetraNoDataValue(i));
  for (int i = 0; i < old_data.TetraPropertiesSize(); ++i)
    props.units.emplace_back(SKUAParseData::String(i, ""));

  size_t prop_size = 0;

  std::for_each(props.esizes.begin(), props.esizes.end(), [&](int esize) { prop_size += esize; });

  props.values.reserve(prop_size);

  for (int i = 0; i < prop_size; ++i) {
    props.values.emplace_back(std::vector<double>());
    props.values.back().reserve(new_data.tetras.size());
  }

  size_t value_index = 0;
  for (int i = 0; i < old_data.VolumeSize(); ++i) {
    const CGocadData::CTVolume &old_volume = old_data.Volume(i);

    for (int j = 0; j < old_volume.TetraSize(); ++j) {
      const CGocadData::CTetra &old_tetra = old_volume.Tetra(j);

      size_t prop_index = 0;
      for (int k = 0; k < old_tetra.PropertySize(); ++k) {
        const std::vector<double> &old_values = old_tetra.Property(k);

        for (std::vector<double>::const_iterator it = old_values.begin(); it != old_values.end(); ++it) {
          props.values[prop_index++].emplace_back(*it);
        }
      }
      ++value_index;
    }
  }
}

void transform(const CGocadData::CTSolid &old_data, struct SKUAParseData &new_data) {
  bool isTSolid = old_data.AtomIDsSize() > 0;

  // general data
  new_data.skua_type = isTSolid ? SKUAParseData::TSolid : SKUAParseData::LightTSolid;

  new_data.name = SKUAParseData::String(0, old_data.Name().toStdString().c_str());

  new_data.coord_unit[0] = old_data.UnitFactor(1);
  new_data.coord_unit[1] = old_data.UnitFactor(0);
  new_data.coord_unit[2] = old_data.UnitFactor(2) * old_data.ZFactor();

  new_data.volume = isTSolid ? old_data.VolumeSize() - 1 : -1;

  // vertices
  std::vector<SKUAParseData::Vertex> &vertices = new_data.vertices;
  vertices.resize(old_data.VertexIDsSize() + old_data.AtomIDsSize() + old_data.SharedVertexIDsSize());
  int vertex_idx = 0;

  int minID, maxID;
  old_data.MinMaxNodeIDs(minID, maxID);

  for (int i = minID; i <= maxID; ++i) {
    const CGocadData::CNode *node = old_data.NodeID(i);
    if (node)
      transform(*node, vertices[vertex_idx++]);
  }

  new_data.add_vertex_ranges();

  // tetras & volumes
  new_data.volumes.reserve(old_data.VolumeSize());
  new_data.volume_types.reserve(old_data.VolumeSize());
  new_data.model_region_names.reserve(old_data.VolumeSize());

  for (int i = 0; i < old_data.VolumeSize(); ++i)
    transform(old_data.Volume(i), new_data);

  // surfaces
  new_data.surfaces.reserve(old_data.SurfaceSize());
  new_data.surface_faces.reserve(old_data.SurfaceSize());
  new_data.surface_types.reserve(old_data.SurfaceSize());
  new_data.surface_names.reserve(old_data.SurfaceSize());

  size_t faces = 0;
  for (int i = 0; i < old_data.SurfaceSize(); ++i)
    faces += old_data.Surface(i).TFaceSize();
  new_data.face_names.reserve(faces);

  for (int i = 0; i < old_data.SurfaceSize(); ++i)
    transform(old_data.Surface(i), new_data);

  // properties
  transform_vertex_properties(old_data, new_data, minID, maxID);
  transform_tetra_properties(old_data, new_data);

  // TODO: fix and create additional info
}

void LoadStreamLegacy(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                      struct SKUAParseData &data) {
  CGocadData::CTSolid solid;
  solid.LoadStream(stream, version, progress);
  transform(solid, data);
}

} // namespace gm_skua
#endif
