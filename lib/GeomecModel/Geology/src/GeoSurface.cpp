// SurfaceBase.cpp: implementation of the CSurfaceBase class.
//
//////////////////////////////////////////////////////////////////////

#include "GeoSurface.h"
#include "BaseEntryTypes.h"
#include "FemAppModel.h"
#include "FvGocadFile.h"
#include "GetSideName.h"
#include "Global.h"
#include "GlobalMessage.h"
#include "HexaEntryTypes.h"
#include "HexaModel.h"
#include "HexaSurface.h"
#include "HorizonBase.h"
#include "IPlatform.h"
#include "IProgressFactory.h"
#include "OpenGLScene.h"
#include "PropertyExcel.h"
#include "PropertyFile.h"
#include "StreamVersion.h"
#include "TSSurface.h"
#include "TSSurfaceProgress.h"
#include "TetraEntryTypes.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSurface.h"
#include "TriaIntersecBox.h"
#include "ValueTypeFactory.h"
#include "unitnode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSurfaceBase::CSurfaceBase(CFemAppModel &model)
    : IElementSet(model), m_pSurface(0), m_pValueType(0), m_unit(CDoubleQuantity::SI_UNIT) {}

CSurfaceBase::CSurfaceBase(const CSurfaceBase &surface)
    : IElementSet(surface), m_pSurface(surface.m_pSurface), m_pValueType(surface.m_pValueType), m_unit(surface.m_unit) {
  AssertValid();
}

// ##ModelId=3BE7AEC2011F
CSurfaceBase::CSurfaceBase(CSurfaceValueType &value_type)
    : IElementSet(value_type.PointSet().Name(), value_type.Model(), CQuantity::SI_UNIT, CQuantity::SI_UNIT),
      m_pSurface(0), m_pValueType(&value_type), m_unit(CDoubleQuantity::SI_UNIT) {
  assert(value_type.ModeSize() == 1);
  assert(value_type.ComponentSize() == 1);

  // LinkTo surface
  LinkTo(value_type);

  // Check typing ....
  create(Model().GraphEntry(MD_BASE_SURFACE));
}

CSurfaceBase::CSurfaceBase(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel &model)
    : IElementSet(strName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_pValueType(0),
      m_unit(CDoubleQuantity::SI_UNIT) {
  CTSSurfaceProgress SurfaceProgress;
  CTSSurface temp(vcPoint, SurfaceProgress);
  m_pSurface = new geo::CTriSurface(temp);

  create(Model().GraphEntry(MD_BASE_SURFACE));
}

CSurfaceBase::CSurfaceBase(const QString &strName, const geo::CArray<geo::CPoint> &vcPoint, CFemAppModel &model)
    : IElementSet(strName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_pValueType(0),
      m_unit(CDoubleQuantity::SI_UNIT) {
  CTSSurfaceProgress SurfaceProgress;
  CTSSurface temp(vcPoint, SurfaceProgress);
  m_pSurface = new geo::CTriSurface(temp);

  create(Model().GraphEntry(MD_BASE_SURFACE));
}

CSurfaceBase::CSurfaceBase(const QString &strName, const geo::ISurface &surface, CFemAppModel &model)
    : IElementSet(strName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_pValueType(0),
      m_unit(CDoubleQuantity::SI_UNIT) {
  m_pSurface = new geo::CTriSurface(surface);

  create(Model().GraphEntry(MD_BASE_SURFACE));
}

CSurfaceBase::CSurfaceBase(const QString &strName, const std::vector<const geo::IFace *> &vcFace, CFemAppModel &model)
    : IElementSet(strName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_pValueType(0),
      m_unit(CDoubleQuantity::SI_UNIT) {
  CTSSurfaceProgress SurfaceProgress;
  CTSSurface temp(vcFace, SurfaceProgress);
  m_pSurface = new geo::CTriSurface(temp);

  create(Model().GraphEntry(MD_BASE_SURFACE));
}

// ##ModelId=3BC55D5F016D
CSurfaceBase::~CSurfaceBase() {
  if (!IsCopy())
    delete m_pSurface;

  if (!IsCopy()) {
    DeleteAllMeshregions();
  }
}

void CSurfaceBase::DeleteAllMeshregions() { MeshRegions().ClearAndDelete(); }

bool CSurfaceBase::Build() const {
  std::auto_ptr<IProgressBase> cursor(_g->prog()->create(eProgress::Wait));
  assert(!m_pSurface);
  assert(m_pValueType);

  // Try to build the surface
  CValueComponent *pComponent = dynamic_cast<CValueComponent *>(&m_pValueType->Component(0));
  assert(pComponent); // Should be a component

  if (pComponent->Defined() && (m_pValueType->PointSet().PointSize() > 0)) {
    // Initialize point vec
    IValueSet *pValueSet = &pComponent->ValueSet();
    IPointSet *pPointSet = &pValueSet->PointSet();
    geo::CPtrArray<geo::IPoint> vcPoint;

    const IValueDataInterfaceScalar &vC = m_pValueType->Component(0).ScalarData();
    for (int i = 0; i < pPointSet->PointSize(); i++) {
      geo::IPoint *pPoint = new geo::CPoint(pPointSet->PointAt(i).X(), pPointSet->PointAt(i).Y(),
                                            vC.ValuePoint(pPointSet->PointAt(i), CQuantity::SI_UNIT).Value());
      vcPoint.PushBack(*pPoint);
    }

    CTSSurfaceProgress SurfaceProgress;
    CTSSurface temp(vcPoint, SurfaceProgress);
    m_pSurface = new geo::CTriSurface(temp);

    // Destroy temporary nodes
    for (size_t i = 0; i < vcPoint.Size(); i++) {
      delete &vcPoint.Object(i);
    }

    return true;
  }

  return false;
}

// ##ModelId=3BC55D5F016E
unsigned int CSurfaceBase::IconId() const {
  if (Used())
    return IDI_SURFACE_USED;

  return IDI_SURFACE;
}

// ##ModelId=3BC55D5F0170
unsigned int CSurfaceBase::TypeId() const { return IDT_TREE_SURFACES; }

QString CSurfaceBase::TypeName() const { return getStringTableEntry(IDS_TREE_SURFACES); }

const geo::ISurface &CSurfaceBase::Surface() const {
  if (m_pSurface == 0)
    Build();

  return *m_pSurface;
}

bool CSurfaceBase::PointInConvexHull(const geo::IPoint & /*pt*/) const {
  assert(false);
  // TODO : Implement point in convexhull
  return true;
}

const geo::CPlane &CSurfaceBase::ProjectionPlane() const {
  assert(m_pSurface);
  if (m_ProjectionPlane.Empty()) {
    geo::CPoint mid = (m_pSurface->Min() + m_pSurface->Max()) * 0.5;
    m_ProjectionPlane = geo::CPlane(mid, m_pSurface->AverageNormal());
  }
  return m_ProjectionPlane;
}

CSurfaceBase &CSurfaceBase::operator=(const CSurfaceBase &rhs) {
  IElementSet::operator=(rhs);

  m_unit = rhs.m_unit;
  m_pSurface = rhs.m_pSurface;

  m_pValueType = rhs.m_pValueType;
  return *this;
}

bool CSurfaceBase::operator==(const CSurfaceBase &rhs) const {
  if (m_unit == rhs.m_unit)
    return IElementSet::operator==(rhs);

  return false;
}

int CSurfaceBase::DisplayListSize() const { return 2; }

// ##ModelId=3BE7AEC2010E
const geo::IObject &CSurfaceBase::DisplayList(int nIndex) const {
  if (m_pSurface == 0)
    Build();

  if (nIndex == 0)
    return *m_pSurface;

  return m_MeshRegions;
}

bool CSurfaceBase::Empty() const { return m_pSurface == 0; }

void CSurfaceBase::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  IElementSet::SaveStream(stream, progress);
  std::string pr("Saving Surface ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());
  int pointSetIndex = -1;
  int valueSetIndex = -1;
  if (m_pValueType) {
    IValueSet *pValueSet = &((CValueComponent &)m_pValueType->Component(0)).ValueSet();
    IPointSet *pPointSet = &pValueSet->PointSet();
    pointSetIndex = pPointSet->Index();
    valueSetIndex = pValueSet->Index();
  }
  int nUnit = (int)m_unit;
  stream << nUnit;
  stream << pointSetIndex;
  stream << valueSetIndex;
  stream << int(m_pSurface ? 1 : 0);
  if (m_pSurface) {
    // Only save the data when not linked to pointset
    int nPointSize = m_pSurface->PointSize();
    stream << nPointSize;
    for (int nPoint = 0; nPoint < nPointSize; nPoint++) {
      stream << m_pSurface->Point(nPoint).X();
      stream << m_pSurface->Point(nPoint).Y();
      stream << m_pSurface->Point(nPoint).Z();
      progress.Step();
    }

    // Save the topology
    int nFaceSize = m_pSurface->FaceSize();
    stream << nFaceSize;
    for (int nFace = 0; nFace < nFaceSize; nFace++) {
      assert(m_pSurface->Face(nFace).IndexingElementSet() == m_pSurface);
      assert(m_pSurface->Face(nFace).NrOfPoints() == 3);
      // Save point 1, 2 and 3
      stream << m_pSurface->Face(nFace).PointIndex(0);
      stream << m_pSurface->Face(nFace).PointIndex(1);
      stream << m_pSurface->Face(nFace).PointIndex(2);
      progress.Step();
    }
  }
}

class CTriFace : public geo::IFace {
public:
  typedef std::map<int, const geo::IPoint *> TPointMap;

private:
  const TPointMap &m_mpPoint;
  int m_node[3];

public:
  CTriFace(const TPointMap &mpPoint, int p1, int p2, int p3) : m_mpPoint(mpPoint) {
    m_node[0] = p1;
    m_node[1] = p2;
    m_node[2] = p3;
  }

  virtual const geo::IPoint &Point(int nIndex) const {
    TPointMap::const_iterator it = m_mpPoint.find(m_node[nIndex]);
    assert(it != m_mpPoint.end());
    return *it->second;
  }
  virtual void Point(int /*nIndex*/, const geo::IPoint & /*pt*/) { assert(false); }
  virtual int NrOfPoints() const { return 3; }
  virtual double Size() const {
    assert(false);
    return 0;
  }
  virtual geo::CValue InterpolateValue(const geo::IPoint & /*point*/,
                                       const std::vector<geo::CValue> & /*values*/) const {
    assert(false);
    return geo::CValue();
  }
};

void CSurfaceBase::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  IElementSet::LoadStream(stream, version, progress);
  std::string pr("Loading Surface ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());
  int ptSetIndex;
  int valSetIndex;
  if (version > CStreamVersion(3, 0, 74)) {
    int nUnit;
    stream >> nUnit;
    m_unit = (CQuantity::UNIT)nUnit;
  }
  stream >> ptSetIndex;
  stream >> valSetIndex;

  int nReadMeshed = 0;
  if (version >= CStreamVersion(4, 1, 55))
    stream >> nReadMeshed;

  if ((ptSetIndex != -1) && (valSetIndex != -1)) {
    // We are going to retrieve the surface created by the pointset during load which this surface claims to be it's
    // clone (ie this surface was created by the pointset during a previous session)
    TPointSetEntry &pointset_entry = (TPointSetEntry &)*Model().GraphEntry(MD_BASE_POINTSET);
    IPointSet *ptSet = pointset_entry.FindIndex(ptSetIndex);
    if (ptSet) {
      IValueSet *valSet = 0;
      for (int t = 0; t < ptSet->ValueSetSize(); t++) {
        if (ptSet->ValueSet(t).Index() == valSetIndex) {
          valSet = &ptSet->ValueSet(t);
          break;
        }
      }
      assert(valSet);
      CSurfaceValueType &pSVT = dynamic_cast<CSurfaceValueType &>(valSet->Component()->Parent());
      pSVT.Surface().Index(Index());

      if (nReadMeshed)
        pSVT.Surface().LoadMesh(stream, version, progress);
      delete this;
      return;
    }
  } else {
    LoadMesh(stream, version, progress);
  }
  create(Model().GraphEntry(MD_BASE_SURFACE));
}

void CSurfaceBase::LoadMesh(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Read in the points
  int nPointSize;
  stream >> nPointSize;
  std::vector<geo::CPoint> vcPoint(nPointSize);
  for (int nPoint = 0; nPoint < nPointSize; nPoint++) {
    // Just stream in the point and place it in the map
    double X, Y, Z;
    stream >> X;
    stream >> Y;
    stream >> Z;
    vcPoint[nPoint] = geo::CPoint(X, Y, Z);
    progress.Step();
  }

  // Read in topologie
  int nFaceSize;
  stream >> nFaceSize;
  std::vector<std::vector<int>> vcFace(nFaceSize, std::vector<int>(3));
  std::vector<int> vcEraseFaces;
  for (int nFace = 0; nFace < nFaceSize; nFace++) {
    // Read in face
    int i;
    for (i = 0; i < 3; i++)
      stream >> vcFace[nFace][i];

    // check for 'collapsed' faces (with coinciding nodes)
    for (i = 0; i < 3; ++i) {
      if (vcFace[nFace][i] == vcFace[nFace][(i + 1) % 3])
        vcEraseFaces.push_back(nFace);
    }

    progress.Step();
  }

  for (int i = vcEraseFaces.size() - 1; i >= 0; --i)
    vcFace.erase(vcFace.begin() + vcEraseFaces[i]);

  m_pSurface = new geo::CTriSurface(vcPoint, vcFace);
}

long CSurfaceBase::SavedItems() const {
  if (m_pSurface)
    return IElementSet::SavedItems() + m_pSurface->PointSize() + m_pSurface->FaceSize();
  return IElementSet::SavedItems();
}

bool CSurfaceBase::CanDestroy() const {
  // We can be destroyed under the condition that no of our clouds is used ...
  return Used() == 0;
}

const CGraphNode *CSurfaceBase::Used() const {
  if (IsCopy()) {
    const CSurfaceBase *pBase = (const CSurfaceBase *)(&Source());
    return pBase->Used();
  }

  for (size_t i = 0; i < referenceSize(); i++) {
    if (dynamic_cast<const CHorizonBase *>(&referenceAt(i)))
      return &referenceAt(i);
  }

  return 0;
}

void CSurfaceBase::Invalidate() {
  assert(m_pValueType);
  if (m_pSurface)
    delete m_pSurface;
  m_pSurface = 0;
  DeleteAllMeshregions();
}

void CSurfaceBase::OnNewNeighbour(const CGraphNode &node) {
  COpenGLNode::OnNewNeighbour(node);

  if (dynamic_cast<const CHorizonBase *>(&node))
    Modified(); // update icon
}

void CSurfaceBase::OnNeighbourDeleted(const CGraphNode &node) {
  COpenGLNode::OnNeighbourDeleted(node);

  if (dynamic_cast<const CHorizonBase *>(&node))
    Modified(); // update icon

  if (m_pValueType == &node) {
    // We're also about to delete
    delete this;
  }
}

void CSurfaceBase::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (m_pValueType == &node) {
    // Rename and rebuild
    // We keep the name of the point set
    //		if(Name() != m_pValueType->Name())
    //			Name(m_pValueType->Name());

    CValueComponent *pComponent = dynamic_cast<CValueComponent *>(&m_pValueType->Component());
    assert(pComponent);
    if (pComponent->Defined()) {
      Invalidate();
      Modified();
    }
  }

  COpenGLNode::OnNeighbourModified(node, uHint);
}
/*
bool CSurfaceBase::Original()
{
  // Do we need to build the surface
  if(m_pSurface == 0)
    VERIFY(Build());	// Build should succeed

  bool ret = m_pSurface->Original();
  if (ret) Modified();

  return ret;
}
*/
bool CSurfaceBase::Increase(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset,
                            double dOffset /* = 0 */) {
  // Do we need to build the surface
  if (m_pSurface == 0)
    VERIFY(Build()); // Build should succeed

  CTSSurfaceProgress SurfaceProgress;
  CTSSurface temp(*m_pSurface, SurfaceProgress);
  bool ret = temp.Increase(dXNOffset, dYNOffset, dXMOffset, dYMOffset, dOffset);

  delete m_pSurface;
  m_pSurface = new geo::CTriSurface(temp);
  if (ret)
    Modified();

  return ret;
}

bool CSurfaceBase::Increase(geo::CBox box, double dOffset /* = 0 */) {
  // Do we need to build the surface
  if (m_pSurface == 0)
    VERIFY(Build()); // Build should succeed

  geo::CPoint minpoint = m_pSurface->Min();
  geo::CPoint maxpoint = m_pSurface->Max();

  double dHalfEps = EPS / 2;

  // Check if the resizing box is larger then the bounding box and the precision offset is larger then 0.
  if ((dOffset == 0) || (minpoint.X() - box.Min().X() > dHalfEps) || (minpoint.Y() - box.Min().Y() > dHalfEps) ||
      (maxpoint.X() - box.Max().X() < -dHalfEps) || (maxpoint.Y() - box.Max().Y() < -dHalfEps)) {
    IPlatform *platform = IPlatform::instance();

    platform->trace(QString("CSurfaceBase::Increase; sending bounding box for "
                            "surface %1:\n")
                        .arg(Name()));
    platform->trace(QString("    x_min: %1\n").arg(box.Min().X()));
    platform->trace(QString("    y_min: %1\n").arg(box.Min().Y()));
    platform->trace(QString("    x_max: %1\n").arg(box.Max().X()));
    platform->trace(QString("    y_max: %1\n").arg(box.Max().Y()));
    CTSSurfaceProgress SurfaceProgress;
    CTSSurface temp(*m_pSurface, SurfaceProgress);
    bool ret = temp.Increase(box, dOffset);

    delete m_pSurface;
    m_pSurface = new geo::CTriSurface(temp);
    if (ret)
      Modified();

    return ret;
  }

  return false;
}
/*
double CSurfaceBase::CalcAverageEdgeSize()
{
  // Do we need to build the surface
  if(m_pSurface == 0)
    VERIFY(Build());	// Build should succeed

  return m_pSurface->CalcAverageEdgeSize();
}
*/
geo::CValue CSurfaceBase::InterpolateDepth(const geo::IPoint &point, bool bExtrapolate) const {
  // Do we need to build the surface
  if (m_pSurface == 0)
    VERIFY(Build()); // Build should succeed

  return m_pSurface->InterpolateZ(point, bExtrapolate);
}

CSurfaceBase *CSurfaceBase::OnCreateSurface(const QString & /*sName*/,
                                            const geo::CPtrArray<geo::IPoint> & /*vcPoint*/) {
  assert(false); // Should not be called...
  return 0;
}

CSurfaceBase *CSurfaceBase::OnCreateSurface(const QString & /*sName*/,
                                            const std::vector<const geo::IFace *> & /*vcFaces*/) {
  assert(false);
  return 0;
}

CSurfaceBase *CSurfaceBase::MergeSurfaces(std::vector<const CSurfaceBase *> surfaces, bool bReTriangulate) {
  CSurfaceBase *pCurrent = 0;
  CSurfaceBase *pOld = 0;
  std::vector<const CSurfaceBase *>::size_type st;
  for (st = 0; st < surfaces.size(); st++) {
    if (st == 0) {
      pCurrent = MergeSurface(*surfaces[st], bReTriangulate);
      continue;
    }
    pOld = pCurrent;
    pCurrent = pCurrent->MergeSurface(*surfaces[st], bReTriangulate);
    pOld->Destroy();
  }
  return pCurrent;
}

namespace {

const QString DEFAULT_MERGED_SURFACE_NAME = QObject::tr("Merged surface %1");
const size_t DEFAULT_INITIAL_NUMBER = 0;

} // anonymous namespace

CSurfaceBase *CSurfaceBase::MergeSurface(const CSurfaceBase &surface, bool bReTriangulate) {
  // Fix the hourglas, because this operation can take some time
  std::auto_ptr<IProgressBase> wait(_g->prog()->create(eProgress::Wait));

  if (!bReTriangulate) { // create the new surface using the old connectivity
    std::vector<const geo::IFace *> vcFaces;
    int i, j;
    for (i = 0; i < surface.Surface().FaceSize(); i++)
      vcFaces.push_back(&(surface.Surface().Face(i)));

    for (j = 0; j < Surface().FaceSize(); j++)
      vcFaces.push_back(&(Surface().Face(j)));

    // Create surface name
    QString sName =
        Model().GraphEntry(MD_BASE_SURFACE)->createUniqueName(DEFAULT_MERGED_SURFACE_NAME, DEFAULT_INITIAL_NUMBER);

    // Create surface
    return OnCreateSurface(sName, vcFaces);
  }

  // We first make a list of unique points for the merged surface
  typedef std::set<const geo::IPoint *, geo::ICoordinate::CCoordinateLess> TPointSet;
  TPointSet stPoint;

  int nPoint;
  for (nPoint = 0; nPoint < surface.Surface().PointSize(); nPoint++)
    stPoint.insert(&surface.Surface().Point(nPoint));

  for (nPoint = 0; nPoint < Surface().PointSize(); nPoint++)
    stPoint.insert(&Surface().Point(nPoint));

  geo::CPtrArray<geo::IPoint> vcPoint;
  for (TPointSet::const_iterator it = stPoint.begin(); it != stPoint.end(); it++) {
    vcPoint.PushBack((geo::IPoint &)**it);
  }

  // Create surface name
  QString sName =
      Model().GraphEntry(MD_BASE_SURFACE)->createUniqueName(DEFAULT_MERGED_SURFACE_NAME, DEFAULT_INITIAL_NUMBER);

  // Create surface
  return OnCreateSurface(sName, vcPoint);
}

bool CSurfaceBase::CanConnectItem(const CGraphNode &item) const {
  const CSurfaceBase *pBase = dynamic_cast<const CSurfaceBase *>(&item);
  if (pBase)
    return pBase != this;

  return IElementSet::CanConnectItem(item);
}

bool CSurfaceBase::ConnectItem(const CGraphNode &item) {
  assert(CanConnectItem(item));
  const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase *>(&item);
  if (pSurface) {
    MergeSurface(*pSurface, true);
  }

  return IElementSet::ConnectItem(item);
}

void CSurfaceBase::ExportSurface(const QString &sExt, const CUnitNode &unitNode, const QString &fileName) {
  if (sExt.right(3).compare(".ts", Qt::CaseInsensitive) == 0) {
    // Export to gocad

    geo::CMesh mesh;
    CGoCadFile file(mesh);

    // jbj
    // Mantis #2290
    // Using enum _UNIT constants instead of IQuantityDouble
    // because GoCadFile has no access to SingleQuantity.h or
    // FieldFActor.h
    // This seems the least intrusive solution.
    if (unitNode.Unit() == IQuantityDouble::SI_UNIT)
      file.setUnitType(SI_UNIT);
    else if (unitNode.Unit() == IQuantityDouble::FIELD_UNIT)
      file.setUnitType(FIELD_UNIT);
    else
      file.setUnitType(OTHER_UNIT);

    file.AppendSurface(Surface(), Name());
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, ""));
      file.Save(fileName, *prog);
    } catch (CProgressCancel *p) {
      delete p;
      file.Close();
    }
  } else if ((sExt.compare(".dat", Qt::CaseInsensitive) == 0) || (sExt.compare(".txt", Qt::CaseInsensitive) == 0)) {
    IExportFormat::CExportArg arg(unitNode.Unit());
    CElementSetExportFormat format(*this, ValueTypes(), arg);
    CElementSetFile file(format);
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, ""));
      file.Save(fileName, *prog);
    } catch (CProgressCancel *p) {
      delete p;
      file.Close();
    }
  } else if (sExt.compare(".xls", Qt::CaseInsensitive) == 0 ||
             fileName.right(5).left(4).compare(".xls", Qt::CaseInsensitive) == 0) {
    IExportFormat::CExportArg arg(unitNode.Unit());
    CElementSetExportFormat format(*this, ValueTypes(), arg);
    CElementSetExcelFile file(format);
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, fileName));
      file.Write(fileName, *prog);
    } catch (CProgressCancel *p) {
      delete p;
      //				file.Close();
    }
  } else {
    QString sMessage;
    sMessage = QString("Unknown extension %1").arg(sExt);
    _m()->msg(sMessage);
  }
}

bool CSurfaceBase::SurfaceIsVertical() {
  geo::CVector surfaceNormal = m_pSurface->AverageNormal();
  return surfaceNormal.IsPerpendicular(geo::CVector::Zaxis);
}

void CSurfaceBase::Unit(CDoubleQuantity::UNIT unit) {
  // "Interpret the coordinates of the points of the surface as being in <unit>"
  // internally we want everything to be in SI.

  if (m_unit == unit)
    return;

  const geo::CTriSurface &sf = dynamic_cast<const geo::CTriSurface &>(Surface());
  assert(&sf);
  geo::CTriSurface &surf = const_cast<geo::CTriSurface &>(sf);

  CLengthQuantity lq;
  int i;
  const geo::IPoint *pt = 0;
  for (i = 0; i < surf.PointSize(); i++) {
    pt = &(surf.Point(i));
    // out    in
    surf.Point(i, geo::CPoint(lq.Convert(pt->X(), m_unit, unit), lq.Convert(pt->Y(), m_unit, unit),
                              lq.Convert(pt->Z(), m_unit, unit)));
  }

  m_unit = unit;
}

int CSurfaceBase::RegisterPoint(const geo::IPoint &point) {
  assert(m_pSurface);
  return m_pSurface->AddPoint(point);
}

int CSurfaceBase::CreateElement(const TNodeVec &vcIndex) {
  if (vcIndex.size() != 3)
    return -1;
  return m_pSurface->AddTriangle(vcIndex[0], vcIndex[1], vcIndex[2]);
}

//////////////////////////////////////////////////////////////////////
// CSurfaceEntry
//////////////////////////////////////////////////////////////////////
CSurfaceEntry::CSurfaceEntry(CFemAppModel &model)
    : CStorageNodeEntry<CSurfaceBase>(MD_BASE_SURFACE, IDI_SURFACES, IDS_TREE_SURFACES, model) {}

void CSurfaceEntry::LoadStream(CFemAppModel &model, TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Load Surfaces
  int iNrOfSurfaces;
  stream >> iNrOfSurfaces;
  for (int c = 0; c < iNrOfSurfaces; c++) {
    CSurfaceBase *pSurface = 0;
    int iType;
    if (version >= CStreamVersion(4, 1, 0)) {
      stream >> iType;
    } else {
      // Depending on the model type we create surfaces
      if (dynamic_cast<CHexaModel *>(&model))
        iType = ST_HEXA;
      else
        iType = ST_TETRA;
    }

    if (iType == ST_HEXA)
      pSurface = new CHexaSurface(model);
    else
      pSurface = new CTetraSurface(model);

    pSurface->LoadStream(stream, version, progress);
  }
}

void CSurfaceEntry::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  TNodeSet stNodes = EntryNodes();
  stream << int(stNodes.size());
  for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    CSurfaceBase &surface = **it;
    assert(dynamic_cast<CHexaSurface *>(&surface) || dynamic_cast<CTetraSurface *>(&surface));
    if (dynamic_cast<CHexaSurface *>(&surface))
      stream << ST_HEXA;
    else
      stream << ST_TETRA;

    surface.SaveStream(stream, progress);
  }
}

int CSurfaceEntry::NonUsedSurfaces() const {
  int ret = 0;
  TNodeSet stSurfaces = EntryNodes();
  for (TNodeSet::iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    if ((*it)->CanDestroy())
      ++ret;
  }
  return ret;
}

// if there is a surface that is not used in the model, this will return true
bool CSurfaceEntry::NonUsedSurface() const {
  TNodeSet stSurfaces = EntryNodes();
  for (TNodeSet::iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    if ((*it)->CanDestroy())
      return true;
  }
  return false;
}

bool CSurfaceEntry::GetSurfaceCount() const {
  TNodeSet stSurfaces = EntryNodes();
  return stSurfaces.size() != 0;
}

void CSurfaceEntry::DoSelectAndCreateHorizons(CNamedSurfaces &slected_surfaces, CModelBase &model) {
  // only valid for horizon models
  CGraphEntry *pHexaHorEntry = model.GraphEntry(MD_HEXA_HORIZON);

  for (size_t i = 0; i < slected_surfaces.size(); i++) {

    STNamedSurface &ns = slected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    if (base) {
      if (pHexaHorEntry)
        pHexaHorEntry->ConnectItem(*base); // this will create the horizon and insert it
      else
        new CTetraSubHorizon(*base, model, false);
    }
  }
}

bool CSurfaceEntry::CanCreateSubBoundaryTopAndBottom(const CModelBase &model) const {
  return dynamic_cast<const CTetraModel *>(&model) != 0 && NonUsedSurfaces() > 1;
}

void CSurfaceEntry::DoCreateSubBoundaryTopAndBottom(CModelBase &model) {
  std::vector<CSurfaceBase *> candidates;

  TNodeSet stSurfaces = EntryNodes();

  for (TNodeSet::const_iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    if (!(*it)->SurfaceIsVertical())
      candidates.push_back(*it);
  }

  double currentTopMin = 0, currentBottomMin = 0;
  double currentTopMax = 0, currentBottomMax = 0;

  double maxTemp, minTemp;
  size_t topIndex = 0, bottomIndex = 0;

  for (size_t i = 0; i < candidates.size(); ++i) {
    maxTemp = candidates[i]->Max().Z();
    minTemp = candidates[i]->Min().Z();

    // first check if it's a possible bottom surface
    if (i == 0 || (maxTemp > currentBottomMax && minTemp > currentBottomMin)) {
      currentBottomMin = minTemp;
      currentBottomMax = maxTemp;
      bottomIndex = i;
    }

    // a possible top surface then?
    if (i == 0 || (minTemp < currentTopMin && maxTemp < currentTopMax)) {
      currentTopMin = minTemp;
      currentTopMax = maxTemp;
      topIndex = i;
    }
  }
  assert(topIndex != bottomIndex);
  CTetraSubHorizon *pTopHorizon = new CTetraSubHorizon(*candidates[topIndex], model, false);
  CTetraSubHorizon *pBottomHorizon = new CTetraSubHorizon(*candidates[bottomIndex], model, false);

  assert(dynamic_cast<CTetraModel *>(&model));
  CTetraModel *pTetModel = static_cast<CTetraModel *>(&model);
  pTetModel->SubBoundary().TopHorizon().ConnectItem(*pTopHorizon);
  pTetModel->SubBoundary().BottomHorizon().ConnectItem(*pBottomHorizon);
}

bool CSurfaceEntry::CanSelectAndCreateSubBoundarySides(const CModelBase &model) const {
  if (!NonUsedSurface())
    return false;

  const CTetraModel *pTetModel = dynamic_cast<const CTetraModel *>(&model);

  if (!pTetModel)
    return false;

  return true;
}

void CSurfaceEntry::DoSelectAndCreateSubBoundarySides(CNamedSurfaces &slected_surfaces, CModelBase &model) {
  for (size_t i = 0; i < slected_surfaces.size(); i++) {

    STNamedSurface &ns = slected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    if (base) {
      CTetraModel *pModel = dynamic_cast<CTetraModel *>(&model);
      assert(pModel);
      pModel->SubBoundary().LinkTo(*base);
    }
  }
}

// wjrx mantis 3000
bool CSurfaceEntry::CanSelectAndCreateFaults(const CModelBase &model) const {
  if (!NonUsedSurface())
    return false;

  const CTetraModel *pTetModel = dynamic_cast<const CTetraModel *>(&model);

  if (!pTetModel)
    return false;

  return true;
}

void CSurfaceEntry::DoSelectAndCreateFaults(CNamedSurfaces &slected_surfaces, CModelBase &model) {
  for (size_t i = 0; i < slected_surfaces.size(); i++) {

    STNamedSurface &ns = slected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    if (base) {
      new CTetraSubHorizon(*base, model, true);
    }
  }
}

bool CSurfaceEntry::CanSelectAndUnassignSurfaces() const {
  TNodeSet stSurfaces = EntryNodes();
  for (TNodeSet::iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    if (!(*it)->CanDestroy())
      return true;
  }
  return false;
}

void CSurfaceEntry::DoSelectAndUnassignSurfaces(CNamedSurfaces &selected_surfaces) {
  for (size_t i = 0; i < selected_surfaces.size(); ++i) {
    STNamedSurface &ns = selected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    if (base) {
      for (size_t j = 0; j < base->referenceSize(); ++j) {
        base->UnLink(base->referenceAt(j));
      }
    }
  }
}

bool CSurfaceEntry::CanSelectAndDeleteSurfaces() const { return NonUsedSurface(); }

void CSurfaceEntry::DoSelectAndDeleteSurfaces(CNamedSurfaces &selected_surfaces) {
  for (size_t i = 0; i < selected_surfaces.size(); ++i) {
    STNamedSurface &ns = selected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    if (base && base->CanDestroy()) {
      base->Destroy();
    }
  }
}

bool CSurfaceEntry::CanMergeFaultSurfaces(const CModelBase &model) const {
  if (model.GraphEntry(MD_TETRA_SUB_FAULT)) {
    TEntryNodeSet stFaultEntries = model.GraphEntry(MD_TETRA_SUB_FAULT)->GraphEntryNodes();

    for (TEntryNodeSet::iterator it = stFaultEntries.begin(); it != stFaultEntries.end(); ++it) {
      CTetraSubHorizon *base = dynamic_cast<CTetraSubHorizon *>(*it);
      if (base && base->Slip() && base->SurfaceSize() > 1)
        return true;
    }
  }

  return false;
}

void CSurfaceEntry::DoMergeFaultSurfaces(CModelBase &model) {
  assert(model.GraphEntry(MD_TETRA_SUB_FAULT));
  TEntryNodeSet stFaultEntries = model.GraphEntry(MD_TETRA_SUB_FAULT)->GraphEntryNodes();

  for (TEntryNodeSet::iterator it = stFaultEntries.begin(); it != stFaultEntries.end(); ++it) {
    CTetraSubHorizon *base = dynamic_cast<CTetraSubHorizon *>(*it);
    if (base && base->Slip() && base->SurfaceSize() > 1) {
      std::vector<const CSurfaceBase *> surfaces;
      for (int i = 0; i < base->SurfaceSize(); ++i) {
        surfaces.push_back(&base->Surface(i));
      }

      CSurfaceBase *firstSurface = const_cast<CSurfaceBase *>(surfaces[0]);
      CSurfaceBase *newSurface = firstSurface->MergeSurfaces(surfaces, false);

      for (size_t i = 0; i < surfaces.size(); ++i) {
        base->UnLink(*const_cast<CSurfaceBase *>(surfaces[i]));
      }
      base->LinkTo(*newSurface);
    }
  }
}

CSurfaceBase *CSurfaceEntry::InsertSurface(const QString &name, const geo::ISurface &surface) {
  CSurfaceBase *ret;
  if (dynamic_cast<CHexaModel *>(&Model()))
    ret = new CHexaSurface(name.toStdString().c_str(), surface, Model());
  else
    ret = new CTetraSurface(name.toStdString().c_str(), surface, Model());

  return ret;
}

void CSurfaceEntry::DoCutSurfaces(CModelBase &model, int cutSurfacesCount,
                                  const std::vector<STNamedSurface> &cutSurfaces,
                                  const std::map<SIDE_ID, geo::CTriSurface *> &sideSurfaces, const QString &baseName,
                                  const std::vector<SIDE_ID> &sides, const geo::CBox * /*pbox*/) {
  int iCount = 1;
  std::vector<SIDE_ID>::const_iterator its = sides.begin();
  while (its != sides.end()) {
    QString name;
    std::string side_name = GetSideName(*its);
    name = QString("%1%2").arg(baseName).arg(side_name.c_str());
    const geo::ISurface *surf = sideSurfaces.find(*its)->second;

    // wjrx improved zoom-in mantis 2421
    // Automatically assign sides surfaces to GoCad subboundary.
    // surfaces: front back left right  + top bottom
    //
    CSurfaceBase *pSurfaceBase = InsertSurface(name, *surf);

    assert(pSurfaceBase != NULL);
    CTetraModel *pTetra = dynamic_cast<CTetraModel *>(&model);
    assert(pTetra != NULL);

    if (side_name == "Top") {
      pTetra->SubBoundary().TopHorizon().ConnectItem(*new CTetraSubHorizon(*pSurfaceBase, model, false));

    } else if (side_name == "Bottom") {
      pTetra->SubBoundary().BottomHorizon().ConnectItem(*new CTetraSubHorizon(*pSurfaceBase, model, false));
    } else
      pTetra->SubBoundary().ConnectItem(*pSurfaceBase);

    iCount++;
    its++;
  }

  for (int i = 0; i < cutSurfacesCount; i++) {
    const STNamedSurface &ns = cutSurfaces[i];
    QString name;
    name = QString("%1").arg(baseName + ns.Name.c_str());
    const geo::ISurface *surf = ns.Surface;

    // wjrx improved zoom-in mantis 2421
    // Automatically assign surfaces to GoCad subboundary.
    // Here are the 'non-standard' surfaces they must be assigned to horizons
    //
    CSurfaceBase *pSurfaceBase = InsertSurface(name, *surf);

    assert(pSurfaceBase != NULL);
    new CTetraSubHorizon(*pSurfaceBase, model, false);
  }
}

CSurfaceBase *CSurfaceEntry::LookUpSurface(const geo::ISurface *surf) {
  assert(surf);

  TNodeSet stSurfaces = EntryNodes();

  for (TNodeSet::const_iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    const geo::ISurface *s = dynamic_cast<const geo::ISurface *>(&((*it)->Surface()));
    if (s && s == surf) {
      return *it;
    }
  }
  return 0;
}

void CSurfaceEntry::MergeSurfaces(CNamedSurfaces &selected_surfaces) {
  CSurfaceBase *pFirst = 0;
  std::vector<const CSurfaceBase *> surfaces;
  for (size_t i = 0; i < selected_surfaces.size(); i++) {
    STNamedSurface &ns = selected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    assert(base);

    if (i == 0) {
      pFirst = base;
      continue;
    }
    surfaces.push_back(base);
  }

  pFirst->MergeSurfaces(surfaces, false);
  /*


  std::vector<const geo::IFace*> vcFaces;
  for(int i = 0; i < selected_surfaces.GetSize(); i++)
  {
    STNamedSurface& ns = selected_surfaces.ElementAt(i);
    CSurfaceBase* base = LookUpSurface(ns.Surface);
    assert(base);
    for(int j = 0; j < base->Surface().FaceSize(); j++)
      vcFaces.push_back(&(base->Surface().Face(i)));
  }

  // Create surface name
  QString sName = Model().GraphEntry(MD_BASE_SURFACE)->createUniqueName(
  DEFAULT_MERGED_SURFACE_NAME, DEFAULT_INITIAL_NUMBER);

  // Create surface
  new CTetraSurface(sName, vcFaces, Model());
  */
}

void CSurfaceEntry::RemoveUnused() {
  TNodeSet stSurfaces = EntryNodes();

  for (TNodeSet::const_iterator it = stSurfaces.begin(); it != stSurfaces.end(); it++) {
    if ((*it)->CanDestroy())
      (*it)->Destroy();
  }
}

void CSurfaceEntry::ExportSurfaces(CNamedSurfaces &slected_surfaces, const QString &fileName,
                                   const CUnitNode &unitNode) {
  // Create file object
  geo::CMesh mesh;
  CGoCadFile file(mesh);

  if (unitNode.Unit() == IQuantityDouble::SI_UNIT)
    file.setUnitType(SI_UNIT);
  else if (unitNode.Unit() == IQuantityDouble::FIELD_UNIT)
    file.setUnitType(FIELD_UNIT);
  else
    file.setUnitType(OTHER_UNIT);

  // Insert surfaces in file object
  TNodeSet stSurface = EntryNodes();

  for (size_t i = 0; i < slected_surfaces.size(); i++) {
    STNamedSurface &ns = slected_surfaces.at(i);
    CSurfaceBase *base = LookUpSurface(ns.Surface);
    assert(base);
    file.AppendSurface(base->Surface(), base->Name());
  }

  // Set up progress dialog and save surfaces
  std::auto_ptr<IProgressBase> prog;
  try {
    prog.reset(_g->prog()->create(eProgress::Geo, ""));
    file.Save(fileName, *prog);
  } catch (CProgressCancel *p) {
    delete p;
    file.Close();
  }
}
