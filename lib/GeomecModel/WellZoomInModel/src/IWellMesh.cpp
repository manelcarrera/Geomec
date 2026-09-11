#include <stdexcept>

#include <memory>

#include "IWellMesh.h"

#include "BodyGroup.h"
#include "FormationBase.h"
#include "GeoSurface.h"
#include "Global.h"
#include "Hexahedron.h"
#include "HorizonBase.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "IWellModel.h"
#include "NewWellPath.h"
#include "SurfaceDesc.h"
#include "TetMeshBase.h"
#include "TetSurface.h"
#include "Tetrahedron.h"
#include "WellFormation.h"
#include "WellFormationVolume.h"
#include "WellModelEntryTypes.h"
#include "WellPath.h"
#include "WellPoint.h"
#include "WellTetraMesher.h"
#include "WellZoomInModel.h"
#include "resourceIDI.h"

IWellMesh::IWellMesh(const QString &sName, IWellModel &model)
    : CMeshBase(sName, model.DepletionStageEntry()), m_bMeshing(false), m_mesh(new geo::CMesh3D) {}

IWellMesh::IWellMesh(const IWellMesh &rhs) : CMeshBase(rhs), m_bMeshing(false), m_mesh(new geo::CMesh3D) {}

IWellMesh::~IWellMesh() {
  if (!IsCopy()) {
    m_mesh->Clear();
    delete m_mesh;
  }
}

IWellMesh &IWellMesh::operator=(const IWellMesh &rhs) {
  if (m_mesh) {
    m_mesh->Clear();
    delete m_mesh;
  }
  m_mesh = rhs.m_mesh;
  return *this;
}

bool IWellMesh::operator==(const IWellMesh & /*rhs*/) const { return true; }

unsigned int IWellMesh::IconId() const { return IDI_MESH; }

unsigned int IWellMesh::TypeId() const { return 0; }

void IWellMesh::OnNewNeighbour(const CGraphNode &node) {
  const IFormationElementSet *pVol = dynamic_cast<const IFormationElementSet *>(&node);
  if (pVol) {
    const CWellFormation &form = static_cast<const CWellFormation &>(pVol->Formation());
    m_mpFormationVolumes
        .insert(TFormationVolumeMap::value_type(const_cast<CWellFormation *>(&form), (CWellFormationVolume *)pVol))
        .first;
  }

  CMeshBase::OnNewNeighbour(node);
}

void IWellMesh::OnNeighbourDeleted(const CGraphNode &node) {
  TFormationVolumeMap::iterator it;
  for (it = m_mpFormationVolumes.begin(); it != m_mpFormationVolumes.end(); ++it) {
    if (it->second == &node) {
      m_mpFormationVolumes.erase(it);
      break;
    }
  }

  CMeshBase::OnNeighbourDeleted(node);
}

void IWellMesh::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (&node == &Model() && uHint == GEOMETRY_CHANGED)
    InvalidateMesh();
}

IWellMesh::DIMENSION IWellMesh::Dimension() const { return DIM_3D; }

const CHorizonBase *IWellMesh::SlipHorizon(const geo::IElement & /*element*/) const { return 0; }

const geo::IMesh &IWellMesh::Mesh() const { return *m_mesh; }

geo::IMesh &IWellMesh::Mesh() { return *m_mesh; }

void IWellMesh::InvalidateMesh() {
  CMeshBase::InvalidateMesh();

  Mesh().Clear();

  while (!m_mpFormationVolumes.empty())
    delete m_mpFormationVolumes.begin()->second;
}

void IWellMesh::SwitchToTetraMesh() {
  if (!dynamic_cast<geo::CTetMeshBase *>(m_mesh)) {
    if (m_mesh)
      m_mesh->Clear();
    delete m_mesh;
    m_mesh = geo::CTetMeshBase::GetTetMesher("CM2");
  }
}

void IWellMesh::SwitchToHexaMesh() {
  if (dynamic_cast<geo::CTetMeshBase *>(m_mesh)) {
    if (m_mesh)
      m_mesh->Clear();
    delete m_mesh;
    m_mesh = new geo::CMesh3D;
  }
}

const CWellFormationVolume &IWellMesh::FormationVolume(const CWellFormation &formation) const {
  TFormationVolumeMap::const_iterator it = m_mpFormationVolumes.find(const_cast<CWellFormation *>(&formation));
  assert(it != m_mpFormationVolumes.end());

  return *it->second;
}

CWellFormationVolume &IWellMesh::FormationVolume(CWellFormation &formation) {
  TFormationVolumeMap::const_iterator it = m_mpFormationVolumes.find(&formation);
  assert(it != m_mpFormationVolumes.end());

  return *it->second;
}

bool IWellMesh::HasFormationVolume(const CWellFormation &formation) const {
  TFormationVolumeMap::const_iterator it = m_mpFormationVolumes.find(const_cast<CWellFormation *>(&formation));
  return (it != m_mpFormationVolumes.end());
}

geo::CVector IWellMesh::WellDirectionAt(const geo::IElement &element) const {
  const geo::CBodyQuadrilateral *pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral *>(&element);
  if (pBodyQuad)
    return WellDirectionAt(*pBodyQuad->Parent());

  const geo::CBodyTriangle *pBodyTriangle = dynamic_cast<const geo::CBodyTriangle *>(&element);
  if (pBodyTriangle)
    return WellDirectionAt(*pBodyTriangle->Parent());

  const geo::CHexahedron *pHexa = dynamic_cast<const geo::CHexahedron *>(&element);
  if (pHexa)
    return geo::CVector(pHexa->Point(4), pHexa->Point(0)).UnitVector();

  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  if (pIface)
    return geo::CVector(pIface->Point(0), pIface->Point(3)).UnitVector();

  return geo::CVector();
}

const geo::IFace &IWellMesh::BodyFace(const geo::CInterfaceElement &iface) const {
  const geo::IFace *pFront = iface.FrontFace();
  const geo::IFace *pBack = iface.BackFace();

  assert(pFront || pBack);
  return pFront != 0 ? *pFront : *pBack;
}

long IWellMesh::SavedItems() const {
  long lRet = CMeshBase::SavedItems();

  if (IsMesh()) {
    TFormationVolumeMap::const_iterator it;
    for (it = m_mpFormationVolumes.begin(); it != m_mpFormationVolumes.end(); ++it) {
      assert(dynamic_cast<const CWellFormationVolume *>(it->second));
      const CWellFormationVolume &vol = static_cast<const CWellFormationVolume &>(*it->second);
      lRet += vol.Volume().ElementSize();
      int i;
      for (i = 0; i < vol.Volume().SideSurfaceSize(); ++i)
        lRet += vol.Volume().SideSurface(i).FaceSize();
    }
  }

  return lRet;
}

void IWellMesh::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Unlink from the node entries
  UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
  UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));

  IWellModel &model = static_cast<IWellModel &>(Model());
  if (model.WellPath()) {
    if (version >= CStreamVersion(3, 7, 11)) {
      int n;
      stream >> n;
      ClearDefinitionPointList();
      m_vcDefPoints.resize(n);
      int i;
      for (i = 0; i < n; ++i) {
        double tmd;
        stream >> tmd;
        m_vcDefPoints[i] = new well::CWellPoint(*(model.WellPath()), tmd);
      }
    }
  } else // wjrx mantis 3401
  {
    if (version >= CStreamVersion(3, 7, 27)) {
      int n;
      stream >> n;
      ClearDefinitionPointList();
      m_vcNewDefPoints.reserve(n);
      int i;
      for (i = 0; i < n; ++i) {
        double tmd;
        stream >> tmd;
        m_vcNewDefPoints.push_back(CNewWellPoint(*(model.NewWellPath()), tmd));
      }
    }
  }

  CMeshBase::LoadStream(stream, version, progress);
}

void IWellMesh::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  IWellModel &model = static_cast<IWellModel &>(Model());

  if (model.WellPath()) {
    stream << int(m_vcDefPoints.size());
    for (size_t i = 0; i < m_vcDefPoints.size(); ++i) {
      const well::CWellPoint *p = m_vcDefPoints[i];
      stream << p->TMD();
    }
  } else // wjrx mantis 3401
  {
    stream << int(m_vcNewDefPoints.size());
    for (size_t i = 0; i < m_vcNewDefPoints.size(); ++i) {
      stream << m_vcNewDefPoints[i].TMD();
    }
  }

  CMeshBase::SaveStream(stream, progress);
}

void IWellMesh::CreateDefinitionPointList(double dTargetElementSize) {
  const IWellModel &wellmodel = static_cast<const IWellModel &>(Model());

  double dStartPos = wellmodel.StartPosition().AHD();
  double dEndPos = wellmodel.EndPosition().AHD();
  double dLength = dEndPos - dStartPos;
  double dNumSegments = dLength / dTargetElementSize;
  int nSegments = int(dNumSegments + 0.5);
  double dSegmentLength = dLength / double(nSegments);

  if (wellmodel.WellPath()) {
    m_vcDefPoints.push_back(new well::CWellPoint(*(wellmodel.WellPath()), dStartPos));

    int i;
    for (i = 0; i < nSegments; ++i)
      m_vcDefPoints.push_back(new well::CWellPoint(*(wellmodel.WellPath()), dStartPos + (i + 1) * dSegmentLength));
  } else {
    // wjrx mantis 3401
    m_vcNewDefPoints.push_back(CNewWellPoint(*(wellmodel.NewWellPath()), dStartPos));

    int i;
    for (i = 0; i < nSegments; ++i)
      m_vcNewDefPoints.push_back(CNewWellPoint(*(wellmodel.NewWellPath()), dStartPos + (i + 1) * dSegmentLength));
  }
}

void IWellMesh::ClearDefinitionPointList() {
  for (size_t i = 0; i < m_vcDefPoints.size(); ++i)
    delete m_vcDefPoints[i];

  m_vcDefPoints.clear();

  // wjrx mantis 3401
  m_vcNewDefPoints.clear();
}

int IWellMesh::DefinitionPointSize() const { return m_vcDefPoints.size(); }

int IWellMesh::NewDefinitionPointSize() const { return m_vcNewDefPoints.size(); }

well::CWellPoint &IWellMesh::DefinitionPoint(int i) const {
  assert(i >= 0 && i < m_vcDefPoints.size());
  return *m_vcDefPoints[i];
}

const CNewWellPoint &IWellMesh::NewDefinitionPoint(int i) const {
  assert(i >= 0 && i < m_vcNewDefPoints.size());
  return m_vcNewDefPoints[i];
}

namespace {

typedef std::pair<double, double> TMinMax;

class CNoHorizonFound : public std::runtime_error {
public:
  CNoHorizonFound();
};

const QString FORMATION_WITHOUT_HORIZONS = QObject::tr("encountered a formation without horizons");

CNoHorizonFound::CNoHorizonFound() : std::runtime_error(FORMATION_WITHOUT_HORIZONS.toStdString()) {}

const CFormationBase *findNonWellFormation(const CFormationBase *formationBase) {
  while (dynamic_cast<const CWellFormation *>(formationBase))
    formationBase = formationBase->ParentFormation();

  return formationBase;
}

TMinMax retrieveBoundary(const CFormationBase *formationBase) {
  std::vector<double> depthValues;

  formationBase = findNonWellFormation(formationBase);

  for (int s = 0; s < formationBase->ElementSetSize(); s++) {
    TGraphNodeSet graphNodeSet = formationBase->ElementSet(s).Identifier();

    for (TGraphNodeSet::iterator graphNode = graphNodeSet.begin(); graphNode != graphNodeSet.end(); ++graphNode) {
      const CSurfaceBase *surfaceBase = dynamic_cast<const CSurfaceBase *>(*graphNode);
      const CHorizonBase *horizonBase = 0;

      if (surfaceBase != 0) {
        horizonBase = dynamic_cast<const CHorizonBase *>(surfaceBase->Used());
      } else {
        horizonBase = dynamic_cast<const CHorizonBase *>(*graphNode);
      }

      if (horizonBase != 0) {
        geo::CPoint minimumPoint = horizonBase->Min();
        geo::CPoint maximumPoint = horizonBase->Max();

        if (!minimumPoint.Empty() && !maximumPoint.Empty()) {
          depthValues.push_back(std::min(minimumPoint.Z(), maximumPoint.Z()));
          depthValues.push_back(std::max(minimumPoint.Z(), maximumPoint.Z()));
        }
      }
    }
  }

  if (depthValues.empty()) {
    CNoHorizonFound noHorizonFound;

    throw noHorizonFound;
  }

  return TMinMax(*(std::min_element(depthValues.begin(), depthValues.end())),
                 *(std::max_element(depthValues.begin(), depthValues.end())));
}

IWellMesh::TFormations collectAllFormations(const CFemAppModel &femAppModel, std::vector<IWellMesh::T3DGrid> &grid) {
  IWellMesh::TFormations formations;
  const TFormationBaseEntry *formationBaseEntry =
      dynamic_cast<const TFormationBaseEntry *>(femAppModel.GraphEntry(MD_BASE_FORMATION));

  double modelTop = grid.front().first;
  double modelBottom = grid.back().first;

  assert(formationBaseEntry != 0);

  TFormationBaseEntry::TNodeSet nodeSet = formationBaseEntry->EntryNodes();

  for (TFormationBaseEntry::TNodeSet::iterator node = nodeSet.begin(); node != nodeSet.end(); ++node) {
    try {
      const CWellFormation *wellFormation = dynamic_cast<const CWellFormation *>(*node);

      if (wellFormation != 0) {
        const CFormationBase *parentFormation = wellFormation->ParentFormation();
        TMinMax boundary = retrieveBoundary(parentFormation);

        if (((modelTop <= boundary.first) && (boundary.first <= modelBottom)) ||
            ((modelTop <= boundary.second) && (boundary.second <= modelBottom))) {
          std::pair<IWellMesh::TFormations::iterator, bool> inserted = formations.insert(parentFormation);

          assert(inserted.second);
        }
      }
    }

    catch (const CNoHorizonFound &) {
    }
  }

  return formations;
}

const QString MERGE = QObject::tr("%1%2");
const QString QUOTED_NAME = QObject::tr("\"%1\"");
const QString COMMA_SPACE = QObject::tr(", ");
const QString MISSING_FORMATIONS = QObject::tr("the mesh does not include all "
                                               "intersected formations, the formations %1 %2 missing");
const QString ONE_FORMATION = QObject::tr("is");
const QString MORE_FORMATIONS = QObject::tr("are");
const size_t ONE = 1;

} // anonymous namespace

void IWellMesh::CreateTetraMesh(CModelBase &model, const std::vector<geo::CPoint> &points, double radius,
                                double eltSize) {
  m_bMeshing = true;

  CWellTetraMesher mesher(model, points, radius, eltSize);

  std::auto_ptr<IProgressBase> prog(
      _g->prog()->create(eProgress::Dual, "Create well zoom-in tetra mesh", true, 3 + mesher.NrOfJobs()));

  geo::CTetMeshBase *mesh = mesher.CreateMesh(*prog);

  if (mesh) {
    delete m_mesh;
    m_mesh = mesh;

    std::vector<int> boundaryNodes;

    bool retval = AssignTetraVolumes(*prog) && AssignTetraBoundaries(boundaryNodes, *prog) &&
                  AssignTetraSupports(boundaryNodes, *prog);

    m_bMeshing = false;

    if (retval)
      Modified(MeshCreated);
    else
      InvalidateMesh();
  }

  m_bMeshing = false;
}

void IWellMesh::wellCreateMesh(const QString &primaryTitle) {
  TFormations formations;

  {
    std::auto_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Dual, primaryTitle, true, 4));

    m_bMeshing = true;

    if (!Create2DGrid(*prog)) {
      InvalidateMesh();
      m_bMeshing = false;
      return;
    }

    if (!Create3DMesh(*prog, formations)) {
      InvalidateMesh();
      m_bMeshing = false;
      return;
    }

    m_bMeshing = false;
  }

  if (!formations.empty()) {
    TFormations::const_iterator formation = formations.begin();
    QString missingFormations = QString(QUOTED_NAME).arg((*formation)->Name());

    for (++formation; formation != formations.end(); ++formation) {
      missingFormations = QString(MERGE).arg(missingFormations).arg(COMMA_SPACE);

      QString missingFormation = QString(QUOTED_NAME).arg((*formation)->Name());

      missingFormations = QString(MERGE).arg(missingFormations).arg(missingFormation);
    }

    QString warning = QString(MISSING_FORMATIONS)
                          .arg(missingFormations)
                          .arg(formations.size() > ONE ? MORE_FORMATIONS : ONE_FORMATION);

    _m()->msg(warning);
  }

  Modified(MeshCreated);
}

bool IWellMesh::Create3DMesh(IProgressBase &prog, TFormations &formations) {
  std::vector<T3DGrid> vc3DGrids;
  if (!Create3DGrids(vc3DGrids, prog))
    return false;

  TBoundaryElementVec vcBoundaryElements;
  const IWellModel &wellmodel = static_cast<const IWellModel &>(Model());

  if (!vc3DGrids.empty()) {
    formations = collectAllFormations(Model(), vc3DGrids);

    prog.NextJob("Creating elements"); // TEST HIT

    typedef std::pair<TIndexVec, TIndexVec> TIndexVecPair;

    TIndexVecPair prNodeIndexVectors(TIndexVec(vc3DGrids[0].second.size()), TIndexVec(vc3DGrids[0].second.size()));
    TIndexVec *pvcFirst = &prNodeIndexVectors.first;
    TIndexVec *pvcSecond = &prNodeIndexVectors.second;

    RegisterGridNodes(vc3DGrids[0], *pvcFirst, true, false);

    CreateSupports(*pvcFirst, true, false);

    prog.AddSteps(vc3DGrids.size());

    double uppertmd = vc3DGrids[0].first;
    const CFormationBase *pPrevFormation = 0;

    // register the nodes and create the elements
    for (size_t i = 1; i < vc3DGrids.size(); ++i) {
      double lowertmd = vc3DGrids[i].first;

      const CFormationBase *pFormation = wellmodel.FormationContaining((uppertmd + lowertmd) / 2.);

      TFormations::iterator formation = std::find(formations.begin(), formations.end(), pFormation);

      if (formation != formations.end()) {
        formations.erase(formation);
      }

      const CFormationBase *pNextFormation = 0;
      if (i < vc3DGrids.size() - 1)
        pNextFormation = wellmodel.FormationContaining((lowertmd + vc3DGrids[i + 1].first) / 2.);

      SegmentInfo segmentinfo;
      segmentinfo.bFormationTop = (pFormation != pPrevFormation);
      segmentinfo.bFormationBottom = (pFormation != pNextFormation);
      segmentinfo.bModelStart = (i == 1);
      segmentinfo.bModelEnd = (i == vc3DGrids.size() - 1);

      RegisterGridNodes(vc3DGrids[i], *pvcSecond, false, segmentinfo.bModelEnd);
      CreateSupports(*pvcSecond, false, segmentinfo.bModelEnd);

      CreateHexaElements(*pvcFirst, *pvcSecond, uppertmd, lowertmd, vcBoundaryElements, segmentinfo);

      uppertmd = lowertmd;

      SwapGrids(&pvcFirst, &pvcSecond);
      pPrevFormation = pFormation;

      try {
        prog.Step();
      }

      catch (CProgressCancel *c) {
        delete c;
        return false;
      }
    }
  }

  if (!CreateBoundaryElements(vcBoundaryElements, prog))
    return false;

  return true;
}

bool IWellMesh::Create3DGrids(std::vector<T3DGrid> &vc3DGrids, IProgressBase &prog) const {
  prog.NextJob("Creating 3D grids"); // TEST HIT

  int nWellPoints = 0;
  nWellPoints = DefinitionPointSize();

  if (nWellPoints > 0) {
    prog.AddSteps(nWellPoints);
    const well::CWellPoint *pPrev = 0;
    geo::CVector vecLastNormal; // empty vector
    int i;
    for (i = 0; i < nWellPoints; ++i) {
      try {
        prog.Step();
      }

      catch (CProgressCancel *c) {
        delete c;
        return false;
      }

      const well::CWellPoint &ptWell = DefinitionPoint(i);

      // is there a next ?
      const well::CWellPoint *pNext = 0;
      if (i < nWellPoints - 1)
        pNext = &DefinitionPoint(i + 1);

      assert(pPrev || pNext);

      // get the average normal for the current definition point
      geo::CVector vecNormal = geo::CVector::NullVector;
      if (pPrev)
        vecNormal += geo::CVector(*pPrev, ptWell).UnitVector();
      if (pNext)
        vecNormal += geo::CVector(ptWell, *pNext).UnitVector();
      vecNormal = vecNormal.UnitVector();

      // create the grid for this well definition point
      T3DGrid vcGrid3D;
      vcGrid3D.first = ptWell.TMD();
      Create3DGridAt(ptWell, vecNormal, vcGrid3D);
      vc3DGrids.push_back(vcGrid3D);

      // update normal for last point
      vecLastNormal = vecNormal;

      pPrev = &ptWell;
    }
  } else // wjrx mantis 3401
  {
    nWellPoints = NewDefinitionPointSize();
    prog.AddSteps(nWellPoints);

    const CNewWellPoint *pPrev = 0;
    geo::CVector vecLastNormal; // empty vector
    int i;
    for (i = 0; i < nWellPoints; ++i) {
      try {
        prog.Step();
      }

      catch (CProgressCancel *c) {
        delete c;
        return false;
      }

      const CNewWellPoint &ptWell = NewDefinitionPoint(i);

      // is there a next ?
      const CNewWellPoint *pNext = 0;
      if (i < nWellPoints - 1)
        pNext = &NewDefinitionPoint(i + 1);

      assert(pPrev || pNext);

      // get the average normal for the current definition point
      geo::CVector vecNormal = geo::CVector::NullVector;
      if (pPrev)
        vecNormal += geo::CVector(*pPrev, ptWell).UnitVector();
      if (pNext)
        vecNormal += geo::CVector(ptWell, *pNext).UnitVector();
      vecNormal = vecNormal.UnitVector();

      // create the grid for this well definition point
      T3DGrid vcGrid3D;
      vcGrid3D.first = ptWell.TMD();
      Create3DGridAt(ptWell, vecNormal, vcGrid3D);
      vc3DGrids.push_back(vcGrid3D);

      // update normal for last point
      vecLastNormal = vecNormal;

      pPrev = &ptWell;
    }
  }
  return true;
}

geo::CMatrix IWellMesh::Create3DGridTransformationMatrix(const geo::IPoint &point,
                                                         const geo::IVector &vecNormal) const {
  geo::CMatrix matTransform(3, 4);

  assert(vecNormal == vecNormal.UnitVector());
  geo::CVector vecRotate(vecNormal.CrossProduct(geo::CVector::Zaxis));
  if (!vecRotate.isNullVector()) // do we need to rotate?
  {
    double dAngleRad = acos(vecNormal.DotProduct(geo::CVector::Zaxis));
    geo::CMatrix matRotate;
    matRotate.CreateRotationMatrixRad(vecRotate, -dAngleRad);
    assert(matRotate.RowSize() == 3 && matRotate.ColumnSize() == 3);

    int iRow, iCol;
    for (iRow = 0; iRow < 3; ++iRow) {
      for (iCol = 0; iCol < 3; ++iCol) {
        matTransform.Value(iRow, iCol, matRotate.Value(iRow, iCol));
      }
    }
  } else {
    // unity rotation
    matTransform.Value(0, 0, 1.);
    matTransform.Value(1, 1, 1.);
    matTransform.Value(2, 2, 1.);
  }

  // translation components
  matTransform.Value(0, 3, point.X());
  matTransform.Value(1, 3, point.Y());
  matTransform.Value(2, 3, point.Z());

  return matTransform;
}

void IWellMesh::SaveFormationVolumes(TSTREAM &stream, TPROGRESS &progress) {
  stream << int(m_mpFormationVolumes.size());
  TFormationVolumeMap::iterator itfv;
  for (itfv = m_mpFormationVolumes.begin(); itfv != m_mpFormationVolumes.end(); ++itfv) {
    // save formation index
    stream << itfv->first->Index();

    // save formation volume element indices
    assert(dynamic_cast<const CWellFormationVolume *>(itfv->second));
    const CWellFormationVolume &vol = static_cast<const CWellFormationVolume &>(*itfv->second);
    int nElements = vol.Volume().ElementSize();
    stream << nElements;
    int i;
    for (i = 0; i < nElements; ++i) {
      stream << vol.Volume().Element(i).Index();
      progress.Step();
    }

    // save side surface bodyface indices
    int nSideSurfaces = vol.Volume().SideSurfaceSize();
    stream << nSideSurfaces;
    for (i = 0; i < nSideSurfaces; ++i) {
      const geo::CBodyGroup::CSideSurface &surf = vol.Volume().SideSurface(i);

      // stream the surface's faces
      int nFaces = surf.FaceSize();
      stream << nFaces;
      int j;
      for (j = 0; j < nFaces; ++j) {
        assert(dynamic_cast<const geo::IFace *>(&surf.Face(j)));
        const geo::IFace &quad = static_cast<const geo::IFace &>(surf.Face(j));

        // a face is determined by its parent index and its own index in the parent
        stream << quad.Parent()->Index() << quad.Index();

        progress.Step();
      }
    }
  }
}

void IWellMesh::LoadFormationVolumes(TSTREAM &stream, CStreamVersion & /*version*/, TPROGRESS &progress) {
  int i;

  CWellFormationEntry &form_entry = (CWellFormationEntry &)(*Model().GraphEntry(MD_WELLMODEL_FORMATION));
  int nSize;
  stream >> nSize; // number of formations
  for (i = 0; i < nSize; ++i) {
    // get the formation index
    int idx;
    stream >> idx;

    CWellFormation *pForm = form_entry.FindIndex(idx);
    assert(pForm);

    // create the volume and side surfaces
    CFormationVolume *pFormVol = new CWellFormationVolume(*pForm, *new geo::CBodyGroup(Mesh()));
    pFormVol->reParent(pForm);

    // the number of elements in this formation(volume)
    int nElements;
    stream >> nElements;
    int j;
    for (j = 0; j < nElements; ++j) {
      int nIndex;
      stream >> nIndex;
      assert((!dynamic_cast<geo::CTetMeshBase *>(m_mesh) &&
              dynamic_cast<const geo::CHexahedron *>(&Mesh().Element(nIndex))) ||
             (dynamic_cast<geo::CTetMeshBase *>(m_mesh) &&
              dynamic_cast<const geo::CTetrahedron *>(&Mesh().Element(nIndex))));
      const geo::IBody &body = static_cast<const geo::IBody &>(Mesh().Element(nIndex));
      pFormVol->Volume().AddBody(body);
      progress.Step();
    }

    // the number of side surfaces in this formationvolume
    int nSideSurfaces;
    stream >> nSideSurfaces;
    for (j = 0; j < nSideSurfaces; ++j) {
      geo::CBodyGroup::CSideSurface *pSideSurface = new geo::CBodyGroup::CSideSurface(pFormVol->Volume());

      // get the faces
      int nFaces;
      stream >> nFaces;
      int k;
      for (k = 0; k < nFaces; ++k) {
        int nBodyIndex;
        int nFaceIndex;
        stream >> nBodyIndex >> nFaceIndex;
        assert((!dynamic_cast<geo::CTetMeshBase *>(m_mesh) &&
                dynamic_cast<const geo::CHexahedron *>(&Mesh().Element(nBodyIndex))) ||
               (dynamic_cast<geo::CTetMeshBase *>(m_mesh) &&
                dynamic_cast<const geo::CTetrahedron *>(&Mesh().Element(nBodyIndex))));

        const geo::IBody &body = static_cast<const geo::IBody &>(Mesh().Element(nBodyIndex));
        pSideSurface->AddFace(body.Face(nFaceIndex));
        progress.Step();
      }
    }
  }
}

geo::CBodyGroup *IWellMesh::getBodyGroup4Formation(IWellModel &wellModel, const CFormationBase *formationBase) {
  geo::CBodyGroup *bodyGroup = 0;

  if (formationBase != 0) {
    CWellFormation *associatedFormation = wellModel.AssociatedFormation(*formationBase);

    if (associatedFormation == 0) {
      // a formation containing a mid-point of a well path hexahedron has been
      // identified but apparently has not been detected by
      // 'IWellModel::DetectFormations()' hence the addition of the formation

      wellModel.addFormation(formationBase);
      associatedFormation = wellModel.AssociatedFormation(*formationBase);
      assert(associatedFormation != 0);
    }

    if (associatedFormation != 0) {
      if (!HasFormationVolume(*associatedFormation)) {
        CFormationVolume *wellFormationVolume =
            new CWellFormationVolume(*associatedFormation, *new geo::CBodyGroup(Mesh()));

        wellFormationVolume->reParent(associatedFormation);

        assert(HasFormationVolume(*associatedFormation));

        for (int i = 0; i < 8; ++i) {
          // sides
          new geo::CBodyGroup::CSideSurface(wellFormationVolume->Volume());
        }

        // top
        new geo::CBodyGroup::CSideSurface(wellFormationVolume->Volume());
        // bottom
        new geo::CBodyGroup::CSideSurface(wellFormationVolume->Volume());
      }

      CFormationVolume &formationVolume = FormationVolume(*associatedFormation);

      bodyGroup = &formationVolume.Volume();
    }
  }

  return bodyGroup;
}

geo::CBodyGroup *IWellMesh::findBodyGroupContainingMidPointOfHexahedron(IWellModel &wellModel,
                                                                        geo::CBodyGroup *bodyGroup,
                                                                        const CFormationBase *formation,
                                                                        const geo::CHexahedron *hexahedron) {
  geo::CElementPoint midPoint = hexahedron->MidPoint();
  CModelBase &parentModel = wellModel.ParentModel();

  TFormationBaseEntry *graphEntry = static_cast<TFormationBaseEntry *>(parentModel.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet entryNodes = graphEntry->EntryNodes();

  bool formationContainsMidPoint = formation->Contains(midPoint, true);

  for (TFormationBaseEntry::TNodeSet::const_iterator entryNode = entryNodes.begin();
       !formationContainsMidPoint && entryNode != entryNodes.end(); ++entryNode) {
    CFormationBase *anotherFormation = (*entryNode);

    if (anotherFormation != formation && anotherFormation->Contains(midPoint, true)) {
      geo::CBodyGroup *anotherBodyGroup = getBodyGroup4Formation(wellModel, anotherFormation);

      if (anotherBodyGroup != 0)
        return anotherBodyGroup;
    }
  }

  return bodyGroup;
}

bool IWellMesh::meshing() const { return m_bMeshing; }

void IWellMesh::SwapGrids(TIndexVec **vcFirst, TIndexVec **vcSecond) {
  std::vector<int> *vcTmp = *vcFirst;
  *vcFirst = *vcSecond;
  *vcSecond = vcTmp;
}

bool IWellMesh::CreateBoundaryElements(const TBoundaryElementVec &vcBoundaryElements, IProgressBase &prog) {
  prog.NextJob("Creating boundary elements"); // TEST HIT
  prog.AddSteps(vcBoundaryElements.size());

  IWellModel &wellmodel = static_cast<IWellModel &>(Model());
  IWellModel::CBoundary &boundary = static_cast<CWellZoomInModel::CBoundary &>(wellmodel.Boundary());

  size_t i;
  for (i = 0; i < vcBoundaryElements.size(); ++i) {
    boundary.AddBoundaryElement(*new geo::CInterfaceElement(Mesh(), &vcBoundaryElements[i].Face(),
                                                            &vcBoundaryElements[i].Face(),
                                                            vcBoundaryElements[i].Nodes()),
                                vcBoundaryElements[i].SurfaceDef());
    try {
      prog.Step();
    }

    catch (CProgressCancel *c) {
      delete c;
      return false;
    }
  }

  if (i == 0) {
    prog.AddSteps(1);

    try {
      prog.Step();
    }

    catch (CProgressCancel *c) {
      delete c;
      return false;
    }
  }

  return true;
}

bool IWellMesh::AssignTetraVolumes(IProgressBase &prog) {
  geo::CTetMeshBase *mesh = static_cast<geo::CTetMeshBase *>(m_mesh);

  if (mesh->NrOfVolumes() == 0)
    return false;

  prog.NextJob("Assign volumes");
  prog.AddSteps(mesh->NrOfVolumes());

  CWellFormationEntry &form_entry = (CWellFormationEntry &)(*Model().GraphEntry(MD_WELLMODEL_FORMATION));
  CWellFormationEntry::TNodeSet forms = form_entry.EntryNodes();

  for (int i = 0; i < mesh->NrOfVolumes(); ++i) {
    geo::CBodyGroup &volume = mesh->Volume(i);
    geo::CPoint midPoint(0, 0, 0);

    for (int j = 0; j < volume.PointSize(); ++j)
      midPoint = midPoint + volume.Point(j);
    midPoint = midPoint / volume.PointSize();

    CWellFormation *formation = nullptr;
    for (CWellFormationEntry::TNodeSet::iterator it = forms.begin(); it != forms.end(); ++it)
      if ((*it)->ParentFormation()->Contains(midPoint, false)) {
        formation = *it;
        break;
      }
    if (formation) {
      CWellFormationVolume *wellVolume = new CWellFormationVolume(*formation, volume);
      wellVolume->reParent(formation);
    }

    try {
      prog.Step();
    } catch (CProgressCancel *c) {
      delete c;
      return false;
    }
  }

  return true;
}

bool IWellMesh::AssignTetraBoundaries(std::vector<int> &boundaryNodes, IProgressBase &prog) {
  prog.NextJob("Assign boundaries");
  prog.AddSteps(5);

  geo::CTetMeshBase *mesh = static_cast<geo::CTetMeshBase *>(m_mesh);

  IWellModel &wellmodel = static_cast<IWellModel &>(Model());
  IWellModel::CBoundary &boundary = static_cast<CWellZoomInModel::CBoundary &>(wellmodel.Boundary());

  std::map<int, int> newNodes;
  std::vector<int> nodes(6);

  std::vector<CBoundaryInterfaceDef::TBoundarySurface> types{
      CBoundaryInterfaceDef::BSURF_SIDE, CBoundaryInterfaceDef::BSURF_TOP, CBoundaryInterfaceDef::BSURF_BOTTOM};

  int surfSize = mesh->NrOfSurfaces();

  if (surfSize < 3)
    return false;

  for (int i = 0; i < 3; ++i) {
    geo::CSurfaceDesc &surf = mesh->SurfaceDesc(i);

    int tetSurfSize = surf.TetSurfaceSize();
    for (int j = 0; j < tetSurfSize; ++j) {
      const geo::CTetSurface &tetSurf = surf.TetSurface(j);

      const geo::CBodyGroup::CSideSurface *sideSurf = dynamic_cast<const geo::CBodyGroup::CSideSurface *>(&tetSurf);

      if (sideSurf) {
        int faceSize = tetSurf.FaceSize();
        for (int k = 0; k < faceSize; ++k) {
          const geo::CBodyTriangle &face = static_cast<const geo::CBodyTriangle &>(tetSurf.Face(k));

          for (int l = 0; l < 3; ++l) {
            int pointIndex = face.PointIndex(l);
            nodes[l] = pointIndex;

            int duplicate = 0;
            std::map<int, int>::iterator it = newNodes.find(pointIndex);
            if (it == newNodes.end()) {
              duplicate = mesh->RegisterNode(face.Point(l), false);
              newNodes.insert(std::make_pair(pointIndex, duplicate));
            } else {
              duplicate = it->second;
            }
            nodes[l + 3] = duplicate;
          }
          boundary.AddBoundaryElement(*new geo::CInterfaceElement(*mesh, &face, &face, nodes), types[i]);
        }
      }
    }
    try {
      prog.Step();
    } catch (CProgressCancel *c) {
      delete c;
      return false;
    }
  }

  RegisterTetraBoundaryNodes(newNodes);

  try {
    prog.Step();
  } catch (CProgressCancel *c) {
    delete c;
    return false;
  }

  boundaryNodes.reserve(newNodes.size());
  for (std::map<int, int>::const_iterator it = newNodes.begin(); it != newNodes.end(); ++it) {
    boundaryNodes.push_back(it->second);
  }

  try {
    prog.Step();
  } catch (CProgressCancel *c) {
    delete c;
    return false;
  }

  return true;
}

bool IWellMesh::AssignTetraSupports(const std::vector<int> &boundaryNodes, IProgressBase &prog) {
  prog.NextJob("Create supports");
  prog.AddSteps(1);

  for (std::vector<int>::const_iterator it = boundaryNodes.begin(); it != boundaryNodes.end(); ++it) {
    Mesh().CreateTranslationSupport(*it, geo::CVector::Xaxis);
    Mesh().CreateTranslationSupport(*it, geo::CVector::Yaxis);
    Mesh().CreateTranslationSupport(*it, geo::CVector::Zaxis);
  }

  return true;
}

void IWellMesh::RegisterTetraBoundaryNodes(const std::map<int, int> &boundaryNodes) {}