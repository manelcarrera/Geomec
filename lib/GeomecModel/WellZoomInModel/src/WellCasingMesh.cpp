
#include "WellCasingMesh.h"

#include "BodyGroup.h"
#include "BodyQuadrilateral.h"
#include "BodyTriangle.h"
#include "Hexahedron.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "ISupport.h"
#include "ModifiedHint.h"
#include "StreamVersion.h"
#include "WellCasingCement.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"
#include "WellCasingSteel.h"
#include "WellFormation.h"
#include "WellFormationVolume.h"

namespace {

const size_t DEFAULT_NUMBER_OF_STEEL_ELEMENTS = 1;
const size_t DEFAULT_NUMBER_OF_CEMENT_ELEMENTS = 1;

} // anonymous namespace

CWellCasingMesh::CWellCasingMesh(const QString &sName, CWellCasingModel &casingmodel)
    : IWellMesh(sName, casingmodel), m_dCasingDiameter(0.1778), // 7"
      m_dCasingThickness(0.01), m_dHoleDiameter(0.25), m_nRadialElements(5), m_nTangentialElements(8),
      m_dTargetElementSize(2), m_pCementGroup(0), m_pSteelGroup(0), m_pCementInterfaceGroup(0),
      m_numberOfSteelElements(DEFAULT_NUMBER_OF_STEEL_ELEMENTS),
      m_numberOfCementElements(DEFAULT_NUMBER_OF_CEMENT_ELEMENTS), m_outerDiameterRock(2 * casingmodel.Radius()) {}

CWellCasingMesh::CWellCasingMesh(const CWellCasingMesh &rhs)
    : IWellMesh(rhs), m_dCasingDiameter(rhs.m_dCasingDiameter), m_dCasingThickness(rhs.m_dCasingThickness),
      m_dHoleDiameter(rhs.m_dHoleDiameter), m_nRadialElements(rhs.m_nRadialElements),
      m_nTangentialElements(rhs.m_nTangentialElements), m_dTargetElementSize(rhs.m_dTargetElementSize),
      m_pCementGroup(0), m_pSteelGroup(0), m_pCementInterfaceGroup(0),
      m_numberOfSteelElements(rhs.m_numberOfSteelElements), m_numberOfCementElements(rhs.m_numberOfCementElements),
      m_outerDiameterRock(rhs.m_outerDiameterRock) {}

CWellCasingMesh::~CWellCasingMesh() {}

CWellCasingMesh &CWellCasingMesh::operator=(const CWellCasingMesh &rhs) {
  if (!(*this == rhs)) {
    IWellMesh::operator=(rhs);

    m_dCasingDiameter = rhs.m_dCasingDiameter;
    m_dCasingThickness = rhs.m_dCasingThickness;
    m_dHoleDiameter = rhs.m_dHoleDiameter;
    m_nRadialElements = rhs.m_nRadialElements;
    m_nTangentialElements = rhs.m_nTangentialElements;
    m_dTargetElementSize = rhs.m_dTargetElementSize;
    m_numberOfSteelElements = rhs.m_numberOfSteelElements;
    m_numberOfCementElements = rhs.m_numberOfCementElements;
    m_outerDiameterRock = rhs.m_outerDiameterRock;

    m_bInvalidateAfterDlg = true;
  }

  return *this;
}

bool CWellCasingMesh::operator==(const CWellCasingMesh &rhs) const {
  if (!IWellMesh::operator==(rhs))
    return false;

  return (m_dCasingDiameter == rhs.m_dCasingDiameter && m_dCasingThickness == rhs.m_dCasingThickness &&
          m_dHoleDiameter == rhs.m_dHoleDiameter && m_nRadialElements == rhs.m_nRadialElements &&
          m_nTangentialElements == rhs.m_nTangentialElements && m_dTargetElementSize == rhs.m_dTargetElementSize &&
          m_numberOfSteelElements == rhs.m_numberOfSteelElements &&
          m_numberOfCementElements == rhs.m_numberOfCementElements && m_outerDiameterRock == rhs.m_outerDiameterRock);
}

bool CWellCasingMesh::CanCreateMesh() const { return !IsMesh(); }

bool CWellCasingMesh::IsMesh() const { return !m_bMeshing && Mesh().ElementSize() > 0; }

namespace {

const QString CREATE_WELL_CASING_MESH = QObject::tr("Creating casing model mesh");

} // anonymous namespace

void CWellCasingMesh::CreateMesh() {
  CreateDefinitionPointList(m_dTargetElementSize);

  wellCreateMesh(CREATE_WELL_CASING_MESH);
}

void CWellCasingMesh::InvalidateMesh() {
  if (IsMesh()) {
    Lock();

    COperation oper(Model());

    if (m_pCementGroup) {
      delete m_pCementGroup;
      m_pCementGroup = 0;
    }

    if (m_pSteelGroup) {
      delete m_pSteelGroup;
      m_pSteelGroup = 0;
    }

    if (m_pCementInterfaceGroup) {
      delete m_pCementInterfaceGroup;
      m_pCementInterfaceGroup = 0;
    }

    IWellMesh::InvalidateMesh();

    m_stCementElements.clear();
    m_stSteelElements.clear();
    m_stCementInterfaceElements.clear();
    m_stOuterInterfaceElements.clear();

    Destroy2DGrid();
    ClearDefinitionPointList();

    m_mpNodeElements.clear();
    m_mpSteel2Cement.clear();

    Modified(MeshCleared);

    CWellCasingModel &model = static_cast<CWellCasingModel &>(Model());
    model.InvalidateDeformation();

    Unlock();
  }
}

double CWellCasingMesh::CasingDiameter() const { return m_dCasingDiameter; }

void CWellCasingMesh::CasingDiameter(double d) {
  m_dCasingDiameter = d;
  InvalidateMesh();
}

double CWellCasingMesh::CasingThickness() const { return m_dCasingThickness; }

void CWellCasingMesh::CasingThickness(double d) {
  m_dCasingThickness = d;
  InvalidateMesh();
}

double CWellCasingMesh::HoleDiameter() const { return m_dHoleDiameter; }

void CWellCasingMesh::HoleDiameter(double d) {
  m_dHoleDiameter = d;
  InvalidateMesh();
}

int CWellCasingMesh::RadialElements() const { return m_nRadialElements; }

void CWellCasingMesh::RadialElements(int n) {
  m_nRadialElements = n;
  InvalidateMesh();
}

int CWellCasingMesh::TangentialElements() const { return m_nTangentialElements; }

void CWellCasingMesh::TangentialElements(int n) {
  m_nTangentialElements = n;
  InvalidateMesh();
}

double CWellCasingMesh::TargetElementSize() const { return m_dTargetElementSize; }

void CWellCasingMesh::TargetElementSize(double d) {
  m_dTargetElementSize = d;
  InvalidateMesh();
}

int CWellCasingMesh::NodesPerSpoke() const { return 5 + m_nRadialElements; }

int CWellCasingMesh::NodesPerGrid() const { return NodesPerSpoke() * m_nTangentialElements; }

int CWellCasingMesh::NumberOfGrids() const {
  assert(!DefinitionPointSize() || !NewDefinitionPointSize());

  if (!NewDefinitionPointSize())
    return DefinitionPointSize();

  return NewDefinitionPointSize();
}

int CWellCasingMesh::ElementsPerSegment() const { return NodesPerSpoke() - 1; }

int CWellCasingMesh::ElementsPerGrid() const { return ElementsPerSegment() * m_nTangentialElements; }

int CWellCasingMesh::NumberOfElementGrids() const { return NumberOfGrids() - 1; }

size_t CWellCasingMesh::numberOfSteelElements() const { return m_numberOfSteelElements; }

void CWellCasingMesh::numberOfSteelElements(size_t numberOfSteelElements) {
  m_numberOfSteelElements = numberOfSteelElements;
  InvalidateMesh();
}

size_t CWellCasingMesh::numberOfCementElements() const { return m_numberOfCementElements; }

void CWellCasingMesh::numberOfCementElements(size_t numberOfCementElements) {
  m_numberOfCementElements = numberOfCementElements;
  InvalidateMesh();
}

double CWellCasingMesh::outerDiameterRock() const { return m_outerDiameterRock; }

void CWellCasingMesh::outerDiameterRock(double outerDiameterRock) {
  m_outerDiameterRock = outerDiameterRock;
  InvalidateMesh();
}

const geo::IElement *CWellCasingMesh::GetElement(int iGrid, int iSegment, int iIndexOnSegment) const {
  const geo::IElement *pRet = 0;

  int iCapGridElmsPerSegment = 2 * ElementsPerSegment() - 2;
  int iCapGridSize = iCapGridElmsPerSegment * TangentialElements();

  bool bCap = (iGrid == 0 || iGrid == NumberOfElementGrids() - 1);
  if (bCap) {
    // skip the cap interface elements
    int iIndex =
        iSegment * iCapGridElmsPerSegment + ((iIndexOnSegment <= 2) ? iIndexOnSegment * 2 : iIndexOnSegment * 2 - 1);
    if (iGrid > 0)
      iIndex += iCapGridSize + (NumberOfElementGrids() - 2) * ElementsPerGrid();
    assert(iIndex < Mesh().ElementSize());
    pRet = &Mesh().Element(iIndex);
  } else {
    int iIndex = iCapGridSize + (iGrid - 1) * ElementsPerGrid() + iSegment * ElementsPerSegment() + iIndexOnSegment;
    assert(iIndex < Mesh().ElementSize());
    pRet = &Mesh().Element(iIndex);
  }

  return pRet;
}

const geo::CBodyGroup &CWellCasingMesh::CementElements() const {
  assert(m_pCementGroup);
  return *m_pCementGroup;
}

const geo::CBodyGroup &CWellCasingMesh::SteelElements() const {
  assert(m_pSteelGroup);
  return *m_pSteelGroup;
}

const geo::CPtrArray<geo::IElement> &CWellCasingMesh::CementInterfaceElements() const {
  assert(m_pCementInterfaceGroup);
  return *m_pCementInterfaceGroup;
}

bool CWellCasingMesh::IsSteelElement(const geo::IElement &element) const {
  return (m_stSteelElements.find(&element) != m_stSteelElements.end());
}

bool CWellCasingMesh::IsCementElement(const geo::IElement &element) const {
  return (m_stCementElements.find(&element) != m_stCementElements.end());
}

bool CWellCasingMesh::IsCementInterfaceElement(const geo::IElement &element) const {
  return (m_stCementInterfaceElements.find(&element) != m_stCementInterfaceElements.end());
}

bool CWellCasingMesh::IsOuterInterfaceElement(const geo::IElement &element) const {
  return (m_stOuterInterfaceElements.find(&element) != m_stOuterInterfaceElements.end());
}

long CWellCasingMesh::SavedItems() const {
  long lRet = IWellMesh::SavedItems();

  if (IsMesh()) {
    lRet += Mesh().NodeSize();
    lRet += Mesh().ElementSize();
    lRet += Mesh().SupportSize();
    lRet += m_pCementGroup->BodySize();
    int i;
    for (i = 0; i < m_pCementGroup->SideSurfaceSize(); ++i)
      lRet += m_pCementGroup->SideSurface(i).FaceSize();
    for (i = 0; i < m_pSteelGroup->SideSurfaceSize(); ++i)
      lRet += m_pSteelGroup->SideSurface(i).FaceSize();
    lRet += m_stCementElements.size();
    lRet += m_stSteelElements.size();
    lRet += m_stCementInterfaceElements.size();
    lRet += m_stOuterInterfaceElements.size();
    lRet += m_pCementInterfaceGroup->Size();
    lRet += m_vc2DGrid.size();
    lRet += m_vcTopGrid.size();
    lRet += m_vcBottomGrid.size();
    lRet += m_mpNodeElements.size();
    lRet += m_mpSteel2Cement.size();
  }

  lRet += 1;

  return lRet;
}

void CWellCasingMesh::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  IWellMesh::SaveStream(stream, progress);
  progress.StatusMessage("Saving well casing mesh");

  stream << int(IsMesh() ? 1 : 0);

  if (IsMesh()) {
    CWellCasingModel &model = static_cast<CWellCasingModel &>(Model());
    CWellCasingModel::CBoundary &boundary = static_cast<CWellCasingModel::CBoundary &>(model.Boundary());
    int nNodes1 = Mesh().NodeSize();
    stream << nNodes1;

    for (int i = 0; i < nNodes1; ++i) {
      const geo::INode &node = Mesh().Node(i);
      stream << node.X() << node.Y() << node.Z();
      progress.Step();
    }

    int nElem = Mesh().ElementSize();
    stream << nElem;
    for (int i = 0; i < nElem; ++i) {
      const geo::IElement &elm = Mesh().Element(i);
      const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&elm);
      int nIsIface = (pIface != 0 ? 1 : 0);
      stream << nIsIface;

      int nNodes2 = elm.NrOfNodes();
      stream << nNodes2;

      for (int j = 0; j < nNodes2; ++j)
        stream << elm.Node(j).Index();

      if (pIface) {
        const geo::CBodyQuadrilateral *pFront = dynamic_cast<const geo::CBodyQuadrilateral *>(pIface->FrontFace());
        const geo::CBodyQuadrilateral *pBack = dynamic_cast<const geo::CBodyQuadrilateral *>(pIface->BackFace());

        assert(pFront);
        stream << int(pBack != pFront ? 2 : 1);
        stream << pFront->Parent()->Index();
        stream << pFront->Index();
        if (pBack != pFront) {
          stream << pBack->Parent()->Index();
          stream << pBack->Index();
        } else {
          // side interface
          CBoundaryInterfaceDef::TBoundarySurface surfdef = boundary.BoundarySurfaceDef(*pIface);
          stream << int(surfdef);
        }
      }

      progress.Step();
    }

    stream << int(Mesh().SupportSize());
    for (int i = 0; i < Mesh().SupportSize(); ++i) {
      const geo::ISupport &support = Mesh().Support(i);
      assert(dynamic_cast<const geo::CTranslationSupport *>(&support));
      stream << support.NodeIndex();
      stream << int(support.DirectionSize());
      for (size_t j = 0; j < support.DirectionSize(); ++j) {
        const geo::IVector &vecDir = support.Direction(j);
        stream << vecDir.X() << vecDir.Y() << vecDir.Z();
      }

      progress.Step();
    }

    SaveFormationVolumes(stream, progress);

    SaveBodyGroup(*m_pCementGroup, stream, progress);
    SaveBodyGroup(*m_pSteelGroup, stream, progress);

    stream << int(m_stCementElements.size());

    for (std::set<const geo::IElement *>::const_iterator it = m_stCementElements.begin();
         it != m_stCementElements.end(); ++it) {
      stream << (*it)->Index();
      progress.Step();
    }

    stream << int(m_stSteelElements.size());
    for (std::set<const geo::IElement *>::const_iterator it = m_stSteelElements.begin(); it != m_stSteelElements.end();
         ++it) {
      stream << (*it)->Index();
      progress.Step();
    }

    stream << int(m_stCementInterfaceElements.size());
    for (std::set<const geo::IElement *>::const_iterator it = m_stCementInterfaceElements.begin();
         it != m_stCementInterfaceElements.end(); ++it) {
      stream << (*it)->Index();
      progress.Step();
    }

    stream << int(m_stOuterInterfaceElements.size());
    for (std::set<const geo::IElement *>::const_iterator it = m_stOuterInterfaceElements.begin();
         it != m_stOuterInterfaceElements.end(); ++it) {
      stream << (*it)->Index();
      progress.Step();
    }

    stream << int(m_pCementInterfaceGroup->Size());
    for (size_t i = 0; i < m_pCementInterfaceGroup->Size(); ++i) {
      stream << m_pCementInterfaceGroup->Object(i).Index();
      progress.Step();
    }

    int nSize = m_vc2DGrid.size();
    stream << nSize;

    for (int i = 0; i < nSize; ++i) {
      geo::CPoint *p = m_vc2DGrid[i];
      stream << p->X() << p->Y() << p->Z();
      progress.Step();
    }

    stream << int(m_vcTopGrid.size());
    for (size_t i = 0; i < m_vcTopGrid.size(); ++i) {
      stream << m_vcTopGrid[i];
      progress.Step();
    }

    stream << int(m_vcBottomGrid.size());
    for (size_t i = 0; i < m_vcBottomGrid.size(); ++i) {
      stream << m_vcBottomGrid[i];
      progress.Step();
    }

    stream << int(m_mpNodeElements.size());
    for (TNodeElementsMap::const_iterator itne = m_mpNodeElements.begin(); itne != m_mpNodeElements.end(); ++itne) {
      stream << itne->first;
      stream << int(itne->second.size());
      for (std::set<int>::const_iterator it = itne->second.begin(); it != itne->second.end(); ++it)
        stream << *it;
      progress.Step();
    }

    stream << int(m_mpSteel2Cement.size());

    for (TSteel2CementMap::const_iterator itsc = m_mpSteel2Cement.begin(); itsc != m_mpSteel2Cement.end(); ++itsc) {
      stream << itsc->first->Index() << itsc->second->Index();
      progress.Step();
    }
  }

  stream << m_dCasingDiameter;
  stream << m_dCasingThickness;
  stream << m_dHoleDiameter;
  stream << m_nRadialElements;
  stream << m_nTangentialElements;
  stream << m_dTargetElementSize;

  int numberOfSteelElements = m_numberOfSteelElements;
  int numberOfCementElements = m_numberOfCementElements;

  stream << numberOfSteelElements;
  stream << numberOfCementElements;

  stream << m_outerDiameterRock;

  progress.Step();
}

void CWellCasingMesh::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  IWellMesh::LoadStream(stream, version, progress);
  progress.StatusMessage("Loading well casing mesh");

  int nIsMesh;
  stream >> nIsMesh;

  if (nIsMesh) {
    CWellCasingModel &model = static_cast<CWellCasingModel &>(Model());
    CWellCasingModel::CBoundary &boundary = static_cast<CWellCasingModel::CBoundary &>(model.Boundary());
    int nNodes1;
    stream >> nNodes1;
    int i;
    for (i = 0; i < nNodes1; ++i) {
      double x, y, z;
      stream >> x >> y >> z;
      Mesh().RegisterNode(geo::CPoint(x, y, z), false);
      progress.Step();
    }

    int nElem1;
    stream >> nElem1;
    for (i = 0; i < nElem1; ++i) {
      int nIsIface;
      stream >> nIsIface;
      int nNodes2;
      stream >> nNodes2;
      std::vector<int> vcNodeIndex(nNodes2);
      int j;
      for (j = 0; j < nNodes2; ++j) {
        int nNodeIndex;
        stream >> nNodeIndex;
        vcNodeIndex[j] = nNodeIndex;
      }

      if (nIsIface) {
        // interface element
        int nSides;
        stream >> nSides;
        int nFrontParentIndex;
        stream >> nFrontParentIndex;
        int nFrontIndex;
        stream >> nFrontIndex;

        assert(dynamic_cast<const geo::IBody *>(&Mesh().Element(nFrontParentIndex)));
        const geo::IFace *pFront =
            &(static_cast<const geo::IBody &>(Mesh().Element(nFrontParentIndex))).Face(nFrontIndex);
        const geo::IFace *pBack = pFront;

        int nSurfDef = 0;
        if (nSides == 2) {
          int nBackParentIndex;
          stream >> nBackParentIndex;
          int nBackIndex;
          stream >> nBackIndex;
          pBack = &(static_cast<const geo::IBody &>(Mesh().Element(nBackParentIndex))).Face(nBackIndex);
        } else {
          stream >> nSurfDef;
        }

        geo::CInterfaceElement *pIface = new geo::CInterfaceElement(Mesh(), pFront, pBack, vcNodeIndex);
        if (nSides == 1) {
          assert(nSurfDef >= 0 && nSurfDef < 3);
          boundary.AddBoundaryElement(*pIface, (CBoundaryInterfaceDef::TBoundarySurface)nSurfDef);
        }
      } else {
        // hexahedron
        new geo::CHexahedron(Mesh(), vcNodeIndex);
      }

      progress.Step();
    }

    // load supports
    int nSupports;
    stream >> nSupports;
    for (i = 0; i < nSupports; ++i) {
      int idx;
      int nDir;
      stream >> idx;
      stream >> nDir;
      int j;
      for (j = 0; j < nDir; ++j) {
        double x, y, z;
        stream >> x >> y >> z;
        Mesh().CreateTranslationSupport(idx, geo::CVector(x, y, z));
      }

      progress.Step();
    }

    LoadFormationVolumes(stream, version, progress);

    m_pCementGroup = new geo::CBodyGroup(Mesh());
    LoadBodyGroup(*m_pCementGroup, stream, version, progress);
    m_pSteelGroup = new geo::CBodyGroup(Mesh());
    LoadBodyGroup(*m_pSteelGroup, stream, version, progress);

    int n;
    int idx1;
    stream >> n;
    for (i = 0; i < n; ++i) {
      stream >> idx1;
      m_stCementElements.insert(&Mesh().Element(idx1));
      progress.Step();
    }

    stream >> n;
    for (i = 0; i < n; ++i) {
      stream >> idx1;
      m_stSteelElements.insert(&Mesh().Element(idx1));
      progress.Step();
    }

    stream >> n;
    for (i = 0; i < n; ++i) {
      stream >> idx1;
      m_stCementInterfaceElements.insert(&Mesh().Element(idx1));
      progress.Step();
    }

    stream >> n;
    for (i = 0; i < n; ++i) {
      stream >> idx1;
      m_stOuterInterfaceElements.insert(&Mesh().Element(idx1));
      progress.Step();
    }

    m_pCementInterfaceGroup = new geo::CPtrArray<geo::IElement>;
    stream >> n;
    for (i = 0; i < n; ++i) {
      stream >> idx1;
      m_pCementInterfaceGroup->PushBack(const_cast<geo::IElement &>(Mesh().Element(idx1)));
      progress.Step();
    }

    stream >> n;
    m_vc2DGrid.resize(n);
    for (i = 0; i < n; ++i) {
      double x, y, z;
      stream >> x >> y >> z;
      m_vc2DGrid[i] = new geo::CPoint(x, y, z);
      progress.Step();
    }

    stream >> n;
    m_vcTopGrid.resize(n);
    for (i = 0; i < n; ++i) {
      int idx2;
      stream >> idx2;
      m_vcTopGrid[i] = idx2;
      progress.Step();
    }

    stream >> n;
    m_vcBottomGrid.resize(n);
    for (i = 0; i < n; ++i) {
      int idx2;
      stream >> idx2;
      m_vcBottomGrid[i] = idx2;
      progress.Step();
    }

    if (version >= CStreamVersion(3, 7, 14)) {
      stream >> n;
      for (i = 0; i < n; ++i) {
        int nNode;
        stream >> nNode;
        TNodeElementsMap::iterator it =
            m_mpNodeElements.insert(TNodeElementsMap::value_type(nNode, std::set<int>())).first;
        int nsize;
        stream >> nsize;
        int j;
        for (j = 0; j < nsize; ++j) {
          int nElem2;
          stream >> nElem2;
          it->second.insert(nElem2);
        }
        progress.Step();
      }

      stream >> n;
      for (i = 0; i < n; ++i) {
        int nIdxSteel;
        int nIdxCement;
        stream >> nIdxSteel >> nIdxCement;
        m_mpSteel2Cement.insert(TSteel2CementMap::value_type(&Mesh().Element(nIdxSteel), &Mesh().Element(nIdxCement)));
        progress.Step();
      }
    }
  }

  stream >> m_dCasingDiameter;
  stream >> m_dCasingThickness;
  stream >> m_dHoleDiameter;
  stream >> m_nRadialElements;
  stream >> m_nTangentialElements;
  stream >> m_dTargetElementSize;

  if (version >= CStreamVersion(4, 1, 48)) {
    int numberOfSteelElements = 0;
    int numberOfCementElements = 0;

    stream >> numberOfSteelElements;
    stream >> numberOfCementElements;

    m_numberOfSteelElements = numberOfSteelElements;
    m_numberOfCementElements = numberOfCementElements;

    stream >> m_outerDiameterRock;
  }

  if (version < CStreamVersion(3, 7, 19))
    InvalidateAtUnLock(); // previous versions have created an incorrect mesh

  progress.Step();
}

void CWellCasingMesh::CollectBoundaryNodes(CModelBase::TNodeElementsMap &mpNodeElements) const {
  // we don't want to add the boundary interface elements
  // (base class has no knowledge about them, assumes faults for interfaces),
  // so skip them

  // make sure nodes are mapped to elements
  if (mpNodeElements.empty())
    BuildNodeToElementsMap();

  int nNodesPerSpoke = NodesPerSpoke();
  int nNodesPerGrid = NodesPerGrid();
  assert(nNodesPerGrid / nNodesPerSpoke == m_nTangentialElements);

  assert(m_vcTopGrid.size() == nNodesPerGrid);

  int nGrids = NumberOfGrids();

  int n;

  // the top and bottom grids contain the outer node indices
  // we want the inner nodes, those indices are one less

  // the top grid
  for (n = 0; n < m_nTangentialElements; ++n) {
    int i;
    for (i = 0; i < nNodesPerSpoke; ++i) {
      // skip the second node in each spoke, they are not attached to rock formations or cement (containing pressure
      // values), nor do they have internal pressures values
      if (i == 0 || i > 1) {
        int idx = n * nNodesPerSpoke + i;
        assert(idx < m_vcTopGrid.size());
        int nNode = m_vcTopGrid[idx] - 1;
        TNodeElementsMap::const_iterator it = m_mpNodeElements.find(nNode);
        if (it != m_mpNodeElements.end())
          mpNodeElements.insert(CModelBase::TNodeElementsMap::value_type(nNode, it->second));
      }
    }
  }

  // the current node index
  // int nNode = 2 * m_vcTopGrid.size() + nNodesPerSpoke - 2; // move to the one-but-last node in the spoke (not the
  // outer one connected to the interface)
  int nNode1 = 2 * m_vcTopGrid.size();

  for (n = 0; n < nGrids - 2; ++n) {
    // loop over the spokes
    int i;
    for (i = 0; i < m_nTangentialElements; ++i) {
      int j;
      for (j = 0; j < 2; ++j) {
        // the elements attached to this node
        TNodeElementsMap::const_iterator it = m_mpNodeElements.find(nNode1);
        assert(it != m_mpNodeElements.end());

        mpNodeElements.insert(CModelBase::TNodeElementsMap::value_type(nNode1, it->second));

        if (!j) {
          // move to the end of the spoke
          nNode1 += nNodesPerSpoke - 2;
        }
      }

      // move to the next spoke
      nNode1 += 2;
    }
  }

  // the bottom grid
  for (n = 0; n < m_nTangentialElements; ++n) {
    int i;
    for (i = 0; i < nNodesPerSpoke; ++i) {
      // skip the second node in each spoke, they are not attached to rock formations or cement (containing pressure
      // values), nor do they have internal pressures values
      if (i == 0 || i > 1) {
        int idx = n * nNodesPerSpoke + i;
        assert(idx < m_vcBottomGrid.size());
        int nNode2 = m_vcBottomGrid[idx] - 1;
        TNodeElementsMap::const_iterator it = m_mpNodeElements.find(nNode2);
        if (it != m_mpNodeElements.end())
          mpNodeElements.insert(CModelBase::TNodeElementsMap::value_type(nNode2, it->second));
      }
    }
  }
}

const geo::IElement &CWellCasingMesh::GetAdjacentCementElement(const geo::IElement &steel_element) const {
  TSteel2CementMap::const_iterator it = m_mpSteel2Cement.find(&steel_element);
  assert(it != m_mpSteel2Cement.end());
  return *it->second;
}

bool CWellCasingMesh::getInvalidateAfterDlg() const { return m_bInvalidateAfterDlg; }

void CWellCasingMesh::setInvalidateAfterDlg(bool invalidateAfterDlg) { m_bInvalidateAfterDlg = invalidateAfterDlg; }

const CFFMaterial *CWellCasingMesh::getCFFMaterial(const geo::IElement &element,
                                                   const CDepletionStage &depletionStage) const {
  if (IsCementElement(element)) {
    const CWellCasingModel &wellCasingModel = dynamic_cast<const CWellCasingModel &>(Model());
    const CWellCasingCement &wellCasingCement = wellCasingModel.CasingCement();

    return &(wellCasingCement.Material(depletionStage).Material(element));
  } else if (IsSteelElement(element)) {
    const CWellCasingModel &wellCasingModel = dynamic_cast<const CWellCasingModel &>(Model());
    const CWellCasingNode &wellCasingNode = wellCasingModel.CasingNode();
    const CWellCasingSteel &wellCasingSteel = wellCasingNode.Steel();

    return &(wellCasingSteel.Material(depletionStage).Material(element));
  } else {
    return IWellMesh::getCFFMaterial(element, depletionStage);
  }
}

namespace {

bool addPoint(std::vector<geo::CPoint *> &vc2DGrid, IProgressBase &progress, double radius, double cosineAngle,
              double sineAngle) {
  vc2DGrid.push_back(new geo::CPoint(cosineAngle * radius, sineAngle * radius));

  try {
    progress.Step();
  }

  catch (CProgressCancel *c) {
    delete c;

    return true;
  }

  return false;
}

} // anonymous namespace

bool CWellCasingMesh::Create2DGrid(IProgressBase &prog) {
  assert(m_vc2DGrid.empty());

  int nTotalPoints =
      m_nTangentialElements * (3 + m_nRadialElements + numberOfSteelElements() + numberOfCementElements());
  prog.NextJob("Creating 2D grid"); // TEST HIT
  int nProgSteps = nTotalPoints;
  prog.AddSteps(nProgSteps);

  // create the nodes for each 'spoke', going from inner to outer

  bool cancelled = false;
  for (int i = 0; (i < m_nTangentialElements) && !cancelled; ++i) {
    // create nodes along spoke i

    // angle from x-axis (radians)
    double dAngle = i * 2 * PI / m_nTangentialElements;
    double cosa = cos(dAngle);
    double sina = sin(dAngle);

    // inner casing ring
    double rad = m_dCasingDiameter / 2 - m_dCasingThickness;

    for (size_t s = 0; s < numberOfSteelElements(); ++s) {
      cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);
      rad += (m_dCasingThickness / numberOfSteelElements());
    }

    // outer casing ring, inner interface nodes
    cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);

    // outercasing ring, outer interface nodes
    cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);

    for (size_t c = 0; c < numberOfCementElements(); ++c) {
      // hole ring (around cement)
      rad += (m_dHoleDiameter - m_dCasingDiameter) / (2 * numberOfCementElements());
      cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);
    }

    // rock element rings
    int n = m_nRadialElements;
    double dSum = 0;

    for (int i2 = 1; i2 <= n; ++i2)
      dSum += i2;

    double dr = ((outerDiameterRock() / 2) - rad) / n;
    double x = ((outerDiameterRock() / 2) - rad) / (dr * dSum);

    std::vector<double> vcSizes(n);
    for (int i2 = 0; i2 < n; ++i2)
      vcSizes[i2] = x * dr * (i2 + 1);

#ifdef _DEBUG
    double dDbg = 0;
    for (i = 0; i < n; ++i)
      dDbg += vcSizes[i];
    assert(fabs(dDbg - ((outerDiameterRock() / 2) - rad)) < EPS);
#endif

    for (int j = 0; j < m_nRadialElements; ++j) {
      rad += vcSizes[j];
      cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);
    }

    assert(fabs((outerDiameterRock() / 2) - rad) < 1e-4);

    // outer boundary interface nodes
    cancelled = cancelled || addPoint(m_vc2DGrid, prog, rad, cosa, sina);
  }

  assert(m_vc2DGrid.size() == nTotalPoints);

  return true;
}

void CWellCasingMesh::Destroy2DGrid() {
  for (size_t i = 0; i < m_vc2DGrid.size(); ++i)
    delete m_vc2DGrid[i];

  m_vc2DGrid.clear();
}

void CWellCasingMesh::RegisterGridNodes(const T3DGrid &vc3DGrid, std::vector<int> &vcGridNodeIndices, bool bModelStart,
                                        bool bModelEnd) {
  // vcGridNodeIndices should aready have the correct size
  assert(vcGridNodeIndices.size() == vc3DGrid.second.size());

  if (bModelStart)
    m_vcTopGrid.resize(vc3DGrid.second.size());

  if (bModelEnd)
    m_vcBottomGrid.resize(vc3DGrid.second.size());

  for (size_t i = 0; i < vc3DGrid.second.size(); ++i) {
    vcGridNodeIndices[i] = Mesh().RegisterNode(vc3DGrid.second[i], false);
    if (bModelStart)
      m_vcTopGrid[i] = Mesh().RegisterNode(vc3DGrid.second[i], false);
    if (bModelEnd)
      m_vcBottomGrid[i] = Mesh().RegisterNode(vc3DGrid.second[i], false);
  }
}

void CWellCasingMesh::CreateSupports(const std::vector<int> &vcGrid, bool bModelStart, bool bModelEnd) {
  if (bModelStart || bModelEnd) {
    // support all nodes of the top or bottom grid
    for (size_t i = 0; i < vcGrid.size(); ++i) {
      int iNode;
      if (bModelStart)
        iNode = m_vcTopGrid[i];
      else
        iNode = m_vcBottomGrid[i];

      Mesh().CreateTranslationSupport(iNode, geo::CVector::Xaxis);
      Mesh().CreateTranslationSupport(iNode, geo::CVector::Yaxis);
      Mesh().CreateTranslationSupport(iNode, geo::CVector::Zaxis);
    }
  } else {
    int nSpokeSize = vcGrid.size() / m_nTangentialElements; // the number of nodes from center to edge in one spoke
    int i;
    for (i = 0; i < m_nTangentialElements; ++i) {
      int iOuterNode = vcGrid[(i + 1) * nSpokeSize - 1];
      Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Xaxis);
      Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Yaxis);
      Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Zaxis);
    }
  }
}

geo::CHexahedron *CWellCasingMesh::createHexaElement(IWellModel::CBoundary &boundary, geo::CBodyGroup *bodyGroup,
                                                     std::set<const geo::IElement *> *elements,
                                                     const std::vector<int> &grid1, const std::vector<int> &grid2,
                                                     const SegmentInfo &segmentinfo,
                                                     const CFormationBase *formationBase, int firstSpoke,
                                                     int secondSpoke, bool first, bool last, bool top, bool bottom,
                                                     bool useActualBodyGroup) {
  THexahedronBodyGroupPair hexahedronBodyGroupPair =
      CreateHexahedron(grid1, grid2, firstSpoke, secondSpoke, elements, bodyGroup, formationBase);
  geo::CHexahedron *hexahedron = hexahedronBodyGroupPair.first;

  if (!useActualBodyGroup) {
    assert(bodyGroup == hexahedronBodyGroupPair.second);
  } else if (/* useActualBodyGroup && */
             (bodyGroup != hexahedronBodyGroupPair.second)) {
    bodyGroup = hexahedronBodyGroupPair.second;
  }

  if (top) {
    bodyGroup->SideSurface(SS_TOP).AddFace(hexahedron->Face(HS_TOP));
  }

  if (segmentinfo.bModelStart) {
    geo::CInterfaceElement &iface =
        CreateCapInterface(m_vcTopGrid, grid1, firstSpoke, secondSpoke, &hexahedron->Face(HS_TOP),
                           &hexahedron->Face(HS_TOP), m_stOuterInterfaceElements);

    boundary.AddBoundaryElement(iface, CBoundaryInterfaceDef::BSURF_TOP);
  }

  if (bottom) {
    bodyGroup->SideSurface(SS_BOTTOM).AddFace(hexahedron->Face(HS_BOTTOM));
  }

  if (segmentinfo.bModelEnd) {
    geo::CInterfaceElement &iface =
        CreateCapInterface(grid2, m_vcBottomGrid, firstSpoke, secondSpoke, &hexahedron->Face(HS_BOTTOM),
                           &hexahedron->Face(HS_BOTTOM), m_stOuterInterfaceElements);

    boundary.AddBoundaryElement(iface, CBoundaryInterfaceDef::BSURF_BOTTOM);
  }

  if (first) {
    bodyGroup->SideSurface(SS_INNER).AddFace(hexahedron->Face(HS_INNER));
  } else if (last) {
    bodyGroup->SideSurface(SS_OUTER).AddFace(hexahedron->Face(HS_OUTER));
  }

  return hexahedron;
}

void CWellCasingMesh::CreateHexaElements(const std::vector<int> &vcGrid1, const std::vector<int> &vcGrid2,
                                         double uppertmd, double lowertmd, TBoundaryElementVec & /*vcBoundaryElements*/,
                                         const SegmentInfo &segmentinfo) {
  IWellModel &wellmodel = static_cast<IWellModel &>(Model());
  IWellModel::CBoundary &boundary = static_cast<CWellZoomInModel::CBoundary &>(wellmodel.Boundary());

  assert(vcGrid1.size() == vcGrid2.size());

  if (!m_pCementGroup) {
    m_pCementGroup = new geo::CBodyGroup(Mesh());
    new geo::CBodyGroup::CSideSurface(*m_pCementGroup); // top     0
    new geo::CBodyGroup::CSideSurface(*m_pCementGroup); // bottom  1
    new geo::CBodyGroup::CSideSurface(*m_pCementGroup); // inner   2
    new geo::CBodyGroup::CSideSurface(*m_pCementGroup); // outer   3

    assert(!m_pSteelGroup);
    m_pSteelGroup = new geo::CBodyGroup(Mesh());
    new geo::CBodyGroup::CSideSurface(*m_pSteelGroup); // top     0
    new geo::CBodyGroup::CSideSurface(*m_pSteelGroup); // bottom  1
    new geo::CBodyGroup::CSideSurface(*m_pSteelGroup); // inner   2
    new geo::CBodyGroup::CSideSurface(*m_pSteelGroup); // outer   3

    assert(!m_pCementInterfaceGroup);
    m_pCementInterfaceGroup = new geo::CPtrArray<geo::IElement>;
  }

  const CFormationBase *pFormation = wellmodel.FormationContaining((uppertmd + lowertmd) / 2.);
  geo::CBodyGroup *pVol = getBodyGroup4Formation(wellmodel, pFormation);

  if (pVol) {
    assert(vcGrid1.size() % m_nTangentialElements == 0);
    int nNodesPerSpoke = vcGrid1.size() / m_nTangentialElements;
    int i;
    for (i = 0; i < m_nTangentialElements; ++i) {
      int idxStartFirstSpoke = i * nNodesPerSpoke;
      int idxStartSecondSpoke = (i < m_nTangentialElements - 1 ? (i + 1) * nNodesPerSpoke : 0);

      // steel

      geo::CHexahedron *hexasteel = 0;

      for (size_t s = 0; s < numberOfSteelElements(); ++s) {
        hexasteel = createHexaElement(boundary, m_pSteelGroup, &m_stSteelElements, vcGrid1, vcGrid2, segmentinfo, 0,
                                      idxStartFirstSpoke + s, idxStartSecondSpoke + s, (s == 0),
                                      (s == (numberOfSteelElements() - 1)), segmentinfo.bModelStart,
                                      segmentinfo.bModelEnd, false);
      }

      // cement

      geo::CHexahedron *hexacement = 0;
      geo::CHexahedron *firstHexacement = 0;

      for (size_t c = 0; c < numberOfCementElements(); ++c) {
        hexacement = createHexaElement(
            boundary, m_pCementGroup, &m_stCementElements, vcGrid1, vcGrid2, segmentinfo, 0,
            idxStartFirstSpoke + 1 + numberOfSteelElements() + c, idxStartSecondSpoke + 1 + numberOfSteelElements() + c,
            (c == 0), (c == (numberOfCementElements() - 1)), segmentinfo.bModelStart, segmentinfo.bModelEnd, false);

        if (firstHexacement == 0) {
          firstHexacement = hexacement;
        }
      }

      VERIFY(m_mpSteel2Cement.insert(TSteel2CementMap::value_type(hexasteel, firstHexacement)).second);

      // steel-cement interface
      m_pCementInterfaceGroup->PushBack(CreateSideInterface(
          vcGrid1, vcGrid2, idxStartFirstSpoke + numberOfSteelElements(), idxStartSecondSpoke + numberOfSteelElements(),
          &hexasteel->Face(HS_OUTER), &firstHexacement->Face(HS_INNER), m_stCementInterfaceElements));

      // rock elements
      geo::CHexahedron *pLastHexa = 0;
      int j;
      for (j = 0; j < m_nRadialElements; ++j) {
        geo::CHexahedron *hexa = createHexaElement(
            boundary, pVol, 0, vcGrid1, vcGrid2, segmentinfo, pFormation,
            idxStartFirstSpoke + 1 + j + numberOfSteelElements() + numberOfCementElements(),
            idxStartSecondSpoke + 1 + j + numberOfSteelElements() + numberOfCementElements(), (j == 0),
            (j == (m_nRadialElements - 1)), segmentinfo.bFormationTop, segmentinfo.bFormationBottom, true);

        pLastHexa = hexa;
      }

      // outer interface
      geo::CInterfaceElement &iface = CreateSideInterface(
          vcGrid1, vcGrid2,
          idxStartFirstSpoke + 1 + m_nRadialElements + numberOfSteelElements() + numberOfCementElements(),
          idxStartSecondSpoke + 1 + m_nRadialElements + numberOfSteelElements() + numberOfCementElements(),
          &pLastHexa->Face(HS_OUTER), &pLastHexa->Face(HS_OUTER), m_stOuterInterfaceElements);

      boundary.AddBoundaryElement(iface, CBoundaryInterfaceDef::BSURF_SIDE);
    }
  }
}

void CWellCasingMesh::Create3DGridAt(const geo::IPoint &point, const geo::IVector &vecNormal, T3DGrid &vcGrid3D) const {
  // transformation matrix for the 2D grid to this position and orientation
  geo::CMatrix matTransform = Create3DGridTransformationMatrix(point, vecNormal);

  // create the 3D grid
  vcGrid3D.second.resize(m_vc2DGrid.size());

  for (size_t i = 0; i < m_vc2DGrid.size(); ++i) {
    geo::CPoint pt(*m_vc2DGrid[i]);
    pt.Transform(matTransform);
    vcGrid3D.second[i] = pt;
  }
}

void CWellCasingMesh::SaveBodyGroup(const geo::CBodyGroup &bodygroup, TSTREAM &stream, TPROGRESS &progress) {
  stream << int(bodygroup.BodySize());

  int i;
  for (i = 0; i < bodygroup.BodySize(); ++i) {
    stream << bodygroup.Body(i).Index();
    progress.Step();
  }

  stream << int(bodygroup.SideSurfaceSize());
  for (i = 0; i < bodygroup.SideSurfaceSize(); ++i) {
    const geo::CBodyGroup::CSideSurface &ss = bodygroup.SideSurface(i);
    stream << int(ss.FaceSize());
    int j;
    for (j = 0; j < ss.FaceSize(); ++j) {
      assert(dynamic_cast<const geo::CBodyQuadrilateral *>(&ss.Face(j)));
      const geo::CBodyQuadrilateral &quad = static_cast<const geo::CBodyQuadrilateral &>(ss.Face(j));
      stream << quad.Parent()->Index();
      stream << quad.Index();
      progress.Step();
    }
  }
}

void CWellCasingMesh::LoadBodyGroup(geo::CBodyGroup &bodygroup, TSTREAM &stream, CStreamVersion & /*version*/,
                                    TPROGRESS &progress) {
  int nBodySize;
  stream >> nBodySize;
  int i;
  for (i = 0; i < nBodySize; ++i) {
    int idx;
    stream >> idx;
    const geo::IElement &elm = Mesh().Element(idx);
    assert(dynamic_cast<const geo::IBody *>(&elm) != 0);
    bodygroup.AddBody(static_cast<const geo::IBody &>(elm));
    progress.Step();
  }

  int nSurfSize;
  stream >> nSurfSize;
  for (i = 0; i < nSurfSize; ++i) {
    geo::CBodyGroup::CSideSurface *pSS = new geo::CBodyGroup::CSideSurface(bodygroup);
    int nFaces;
    stream >> nFaces;
    int j;
    for (j = 0; j < nFaces; ++j) {
      int nParentIndex;
      int nFaceIndex;
      stream >> nParentIndex;
      stream >> nFaceIndex;
      assert(dynamic_cast<const geo::IBody *>(&Mesh().Element(nParentIndex)) != 0);
      pSS->AddFace((static_cast<const geo::IBody &>(Mesh().Element(nParentIndex))).Face(nFaceIndex));
      progress.Step();
    }
  }
}

CWellCasingMesh::THexahedronBodyGroupPair CWellCasingMesh::CreateHexahedron(
    const std::vector<int> &vcGrid1, const std::vector<int> &vcGrid2, int idxStartFirstSpoke, int idxStartSecondSpoke,
    std::set<const geo::IElement *> *pstElements, geo::CBodyGroup *BodyGroup, const CFormationBase *formationBase) {
  std::vector<int> vcNode(8);
  vcNode[0] = vcGrid2[idxStartSecondSpoke];
  vcNode[1] = vcGrid2[idxStartSecondSpoke + 1];
  vcNode[2] = vcGrid2[idxStartFirstSpoke + 1];
  vcNode[3] = vcGrid2[idxStartFirstSpoke];
  vcNode[4] = vcGrid1[idxStartSecondSpoke];
  vcNode[5] = vcGrid1[idxStartSecondSpoke + 1];
  vcNode[6] = vcGrid1[idxStartFirstSpoke + 1];
  vcNode[7] = vcGrid1[idxStartFirstSpoke];

  geo::CHexahedron *pHexa = new geo::CHexahedron(Mesh(), vcNode);
  geo::CBodyGroup *actualBodyGroup = BodyGroup;

  if (formationBase != 0) {
    IWellModel &wellModel = static_cast<IWellModel &>(Model());

    actualBodyGroup = findBodyGroupContainingMidPointOfHexahedron(wellModel, BodyGroup, formationBase, pHexa);
  }

  if (pstElements)
    pstElements->insert(pHexa);

  actualBodyGroup->AddBody(*pHexa);

  MapNodesToElement(*pHexa);

  return THexahedronBodyGroupPair(pHexa, actualBodyGroup);
}

geo::CInterfaceElement &CWellCasingMesh::CreateCapInterface(const std::vector<int> &vcGrid1,
                                                            const std::vector<int> &vcGrid2, int idxStartFirstSpoke,
                                                            int idxStartSecondSpoke, const geo::IFace *pFront,
                                                            const geo::IFace *pBack,
                                                            std::set<const geo::IElement *> &stElements) {
  std::vector<int> vcNode(8);
  vcNode[0] = vcGrid2[idxStartSecondSpoke];
  vcNode[1] = vcGrid2[idxStartSecondSpoke + 1];
  vcNode[2] = vcGrid2[idxStartFirstSpoke + 1];
  vcNode[3] = vcGrid2[idxStartFirstSpoke];
  vcNode[4] = vcGrid1[idxStartSecondSpoke];
  vcNode[5] = vcGrid1[idxStartSecondSpoke + 1];
  vcNode[6] = vcGrid1[idxStartFirstSpoke + 1];
  vcNode[7] = vcGrid1[idxStartFirstSpoke];

  geo::CInterfaceElement *pIface = new geo::CInterfaceElement(Mesh(), pFront, pBack, vcNode);
  stElements.insert(pIface);

  // we're not mapping this element to the nodes, we don't want to use it

  return *pIface;
}

geo::CInterfaceElement &CWellCasingMesh::CreateSideInterface(const std::vector<int> &vcGrid1,
                                                             const std::vector<int> &vcGrid2, int idxStartFirstSpoke,
                                                             int idxStartSecondSpoke, const geo::IFace *pFront,
                                                             const geo::IFace *pBack,
                                                             std::set<const geo::IElement *> &stElements) {
  std::vector<int> vcNode(8);
  vcNode[0] = vcGrid1[idxStartSecondSpoke];
  vcNode[1] = vcGrid1[idxStartFirstSpoke];
  vcNode[2] = vcGrid2[idxStartFirstSpoke];
  vcNode[3] = vcGrid2[idxStartSecondSpoke];
  vcNode[4] = vcGrid1[idxStartSecondSpoke + 1];
  vcNode[5] = vcGrid1[idxStartFirstSpoke + 1];
  vcNode[6] = vcGrid2[idxStartFirstSpoke + 1];
  vcNode[7] = vcGrid2[idxStartSecondSpoke + 1];

  geo::CInterfaceElement *pIface = new geo::CInterfaceElement(Mesh(), pFront, pBack, vcNode);
  stElements.insert(pIface);

  // only map this side interface if it's not an outer interface
  if (pFront != pBack)
    MapNodesToElement(*pIface);

  return *pIface;
}

bool CWellCasingMesh::ElementsAttachedToNode(const std::set<int> &stElements, int nNode) const {
  std::set<int>::const_iterator it;
  for (it = stElements.begin(); it != stElements.end(); ++it) {
    const geo::IElement &elm = Mesh().Element(*it);
    int n;
    bool bAttached = false;
    for (n = 0; n < elm.NrOfNodes(); ++n) {
      if (elm.Node(n).Index() == nNode) {
        bAttached = true;
        break;
      }
    }

    if (!bAttached)
      return false;
  }

  return true;
}

void CWellCasingMesh::MapNodesToElement(const geo::IElement &element) {
  int i;
  for (i = 0; i < element.NrOfNodes(); ++i) {
    TNodeElementsMap::iterator it =
        m_mpNodeElements.insert(TNodeElementsMap::value_type(element.Node(i).Index(), std::set<int>())).first;
    it->second.insert(element.Index());
  }
}

void CWellCasingMesh::BuildNodeToElementsMap() const {
  int i;
  for (i = 0; i < Mesh().ElementSize(); ++i) {
    const geo::IElement &elm = Mesh().Element(i);
    const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&elm);
    if (pIface && pIface->FrontFace() == pIface->BackFace())
      continue;

    int n;
    for (n = 0; n < elm.NrOfNodes(); ++n) {
      TNodeElementsMap::iterator it =
          m_mpNodeElements.insert(TNodeElementsMap::value_type(elm.Node(n).Index(), std::set<int>())).first;
      it->second.insert(elm.Index());
    }
  }
}

geo::CBodyGroup *CWellCasingMesh::findBodyGroupContainingMidPointOfHexahedron(IWellModel &wellModel,
                                                                              geo::CBodyGroup *bodyGroup,
                                                                              const CFormationBase *formation,
                                                                              const geo::CHexahedron *hexahedron) {
  geo::CElementPoint midPoint = hexahedron->MidPoint();
  CModelBase &parentModel = wellModel.ParentModel();

  TFormationBaseEntry *graphEntry = dynamic_cast<TFormationBaseEntry *>(parentModel.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet entryNodes = graphEntry->EntryNodes();
  bool formationContainsMidPoint = formation->ParentFormation()->Contains(midPoint, true);

  for (TFormationBaseEntry::TNodeSet::const_iterator entryNode = entryNodes.begin();
       !formationContainsMidPoint && (entryNode != entryNodes.end()); entryNode++) {
    CFormationBase *anotherFormation = (*entryNode);

    if ((anotherFormation != formation) && anotherFormation->ParentFormation()->Contains(midPoint, true)) {
      geo::CBodyGroup *anotherBodyGroup = getBodyGroup4Formation(wellModel, anotherFormation);

      if (anotherBodyGroup != 0) {
        return anotherBodyGroup;
      }
    }
  }

  return bodyGroup;
}

///// CElementCenterPointCache

CElementCenterPointCache::CElementCenterPointCache() {}

void CElementCenterPointCache::Create(const geo::CBodyGroup &bodygroup) {
  Clear();

  int i;
  for (i = 0; i < bodygroup.BodySize(); ++i) {
    geo::CElementPoint point = bodygroup.Body(i).MidPoint();
    m_array.PushBack(point);
  }
}

void CElementCenterPointCache::Create(const geo::CPtrArray<geo::IElement> &elements) {
  Clear();

  for (size_t i = 0; i < elements.Size(); ++i) {
    geo::CElementPoint point = elements.Object(i).MidPoint();
    m_array.PushBack(point);
  }
}

void CElementCenterPointCache::Clear() { m_array.Clear(); }

bool CElementCenterPointCache::Empty() const { return m_array.Empty(); }

const geo::CArray<geo::CElementPoint> &CElementCenterPointCache::Get() const { return m_array; }
