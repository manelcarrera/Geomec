#include "NewWellPathInput.h"
#include "BaseEntryTypes.h"
#include "BodyGroup.h"
#include "BoundingBox.h"
#include "FemAppModel.h"
#include "NewWellPath.h"
#include "WellPathFile.h"

// Shortcut; we define the input always with respect to the root model
static CModelBase &rootModel(CFemAppModel &femAppModel) {
  assert(dynamic_cast<CModelBase *>(&femAppModel));
  return static_cast<CModelBase &>(femAppModel).RootModel();
}

CNewWellPathInput::CNewWellPathInput(CFemAppModel &model)
    : CPointSet(rootModel(model)), m_bIsVertical(false), m_dApproximateRadius(1), m_dGlobalNorthing(0),
      m_dGlobalEasting(0), m_dGlobalTVD(0) {
  Init();
}

CNewWellPathInput::CNewWellPathInput(const QString &sName, CFemAppModel &model)
    : CPointSet(sName, rootModel(model), 0, DIM_3D), m_bIsVertical(false), m_dApproximateRadius(1),
      m_dGlobalNorthing(0), m_dGlobalEasting(0), m_dGlobalTVD(0) {
  Init();
}

CNewWellPathInput::CNewWellPathInput(const QString &sName, CFemAppModel &model, const geo::CPoint &BasePoint)
    : CPointSet(sName, rootModel(model), 0, DIM_3D), m_bIsVertical(true), m_dApproximateRadius(1),
      m_dGlobalNorthing(BasePoint.X()), m_dGlobalEasting(BasePoint.Y()), m_dGlobalTVD(0) {
  Init();
  InitVerticalWell(true);
}

CNewWellPathInput::CNewWellPathInput(const QString &file, const QString &name, CFemAppModel &model,
                                     CQuantity::UNIT unit, IProgressBase &dlg)
    : CPointSet(name, rootModel(model), 2, DIM_3D), m_bIsVertical(false), m_dApproximateRadius(1), m_dGlobalNorthing(0),
      m_dGlobalEasting(0), m_dGlobalTVD(0) {
  Init();
  CWellPathFile wp_file(this, unit);
  if (!wp_file.Open(file, dlg)) {
    // should we do something?
  }
}

CNewWellPathInput::CNewWellPathInput(const CNewWellPath &wellpath)
    : CPointSet(wellpath.Name(), rootModel(const_cast<CFemAppModel &>(wellpath.Model())), 0, DIM_3D),
      m_bIsVertical(wellpath.IsVertical()), m_dApproximateRadius(1), m_dGlobalNorthing(wellpath.GlobalNorthing()),
      m_dGlobalEasting(wellpath.GlobalEasting()), m_dGlobalTVD(wellpath.GlobalTVD()) {
  Init();

  BirthDate(wellpath.GetBirthDate());

  if (IsVertical())
    InitVerticalWell(false);
  else {
    const size_t size = wellpath.WellPointSize();
    if (size > 0) {
      QVector<geo::CPoint> points(size);
      for (size_t i = 0; i < size; ++i) {
        CNewWellPoint point = wellpath.WellPoint(i);
        points[i].Set(point.X(), point.Y(), point.Z());
      }
      InitFromPointArray(points, false);
    }
  }
}

CNewWellPathInput::~CNewWellPathInput() {}

CNewWellPathInput *CNewWellPathInput::EnsureUniqueness(CNewWellPathInput *input) {
  const TPointSetEntry *pointSetEntry =
      dynamic_cast<const TPointSetEntry *>(input->Model().GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TNodeSet nodeSet = pointSetEntry->EntryNodes();

  for (TPointSetEntry::TNodeSet::iterator it = nodeSet.begin(); it != nodeSet.end(); ++it) {
    if ((*it)->pointSetType() == IPointSet::INPUT) {
      CNewWellPathInput *existing = static_cast<CNewWellPathInput *>(*it);

      if (existing != input && *existing == *input) {
        input->Destroy();
        return existing;
      }
    }
  }

  return input;
}

const CNewWellPathInput *CNewWellPathInput::FindByIndex(const CModelBase &model, int nIndex) {
  const TPointSetEntry *pointSetEntry =
      dynamic_cast<const TPointSetEntry *>(model.RootModel().GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TNodeSet nodeSet = pointSetEntry->EntryNodes();

  for (TPointSetEntry::TNodeSet::iterator it = nodeSet.begin(); it != nodeSet.end(); ++it) {
    if ((*it)->pointSetType() == IPointSet::INPUT) {
      CNewWellPathInput *input = static_cast<CNewWellPathInput *>(*it);
      if (input->Index() == nIndex)
        return input;
    }
  }
  return 0;
}

const CNewWellPath *CNewWellPathInput::WellPathInModel(const CModelBase &model) const {
  std::set<CNewWellPath *> wellpaths = Links<CNewWellPath>();

  for (std::set<CNewWellPath *>::iterator it = wellpaths.begin(); it != wellpaths.end(); ++it) {
    if (&(*it)->Model() == &model)
      return *it;
  }
  return 0;
}

const CNewWellPath *CNewWellPathInput::CreateWellPath(CNewWellPathInput *input, const CModelBase &model,
                                                      std::vector<std::string> *messages) {
  QString inputName = input->Name();

  CNewWellPathInput *unique = CNewWellPathInput::EnsureUniqueness(input);

  const CNewWellPath *existing = unique->WellPathInModel(model);

  if (unique != input) {
    if (messages)
      messages->push_back(QString("Input '%1' is already present in the data storage as '%2'")
                              .arg(inputName)
                              .arg(unique->Name())
                              .toStdString());
  }

  if (existing) {
    if (messages)
      messages->push_back(
          QString("Wellpath '%1' is already attached to this model").arg(existing->Name()).toStdString());
  } else if (model.IsMesh()) {
    geo::CPoint min = model.Mesh().Min();
    geo::CPoint max = model.Mesh().Max();

    if (geo::BBox::Intersects(min, max, unique->m_min, unique->m_max, true))
      return new CNewWellPath(*unique, const_cast<CModelBase &>(model));
  }

  return 0;
}

bool CNewWellPathInput::operator==(const CNewWellPathInput &rhs) const {
  if (m_BirthDate != rhs.m_BirthDate)
    return false;

  bool vertical = CheckVerticality() || rhs.CheckVerticality();

  double deltaX = fabs(m_dGlobalNorthing - rhs.m_dGlobalNorthing);
  double deltaY = fabs(m_dGlobalEasting - rhs.m_dGlobalEasting);
  double deltaZ = fabs(m_dGlobalTVD - rhs.m_dGlobalTVD);

  if (deltaX > m_dApproximateRadius || deltaY > m_dApproximateRadius || (deltaZ > m_dApproximateRadius && !vertical))
    return false;

  if (PointSize() != rhs.PointSize())
    return false;

  if (vertical) {
    for (size_t i = 0; i < PointSize(); ++i) {
      const geo::IPoint &p0 = PointAt(i);
      const geo::IPoint &p1 = rhs.PointAt(i);

      deltaX = fabs(p0.X() - p1.X());
      deltaY = fabs(p0.Y() - p1.Y());

      if (deltaX > m_dApproximateRadius || deltaY > m_dApproximateRadius)
        return false;
    }
  } else {
    for (size_t i = 0; i < PointSize(); ++i) {
      const geo::IPoint &p0 = PointAt(i);
      const geo::IPoint &p1 = rhs.PointAt(i);

      deltaX = fabs(p0.X() - p1.X());
      deltaY = fabs(p0.Y() - p1.Y());
      deltaZ = fabs(p0.Z() - p1.Z());

      if (deltaX > m_dApproximateRadius || deltaY > m_dApproximateRadius || deltaZ > m_dApproximateRadius)
        return false;
    }
  }

  return true;
}

bool CNewWellPathInput::CanDestroy() const {
  std::set<CNewWellPath *> wps = Links<CNewWellPath>();
  for (std::set<CNewWellPath *>::const_iterator it = wps.begin(); it != wps.end(); ++it)
    if (!(*it)->CanDestroy())
      return false;

  return true;
}

bool CNewWellPathInput::Destroy() {
  if (CanDestroy()) {
    std::set<CNewWellPath *> wps = Links<CNewWellPath>();

    if (!wps.empty()) {
      QString msg =
          wps.size() == 1
              ? "Deleting this input wellpath here will also delete the corresponding wellpath in the model. Continue?"
              : "Deleting this input wellpath here will also delete all corresponding wellpaths in the models. "
                "Continue?";

      if (_m()->msg(msg, MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
        for (std::set<CNewWellPath *>::iterator it = wps.begin(); it != wps.end(); ++it)
          if (!(*it)->Destroy()) {
            _m()->msg(QString("There was a problem deleting wellpath '%1'; aborting...").arg((*it)->Name()),
                      MB_ICONERROR);
            return false;
          }

        return CGraphNode::Destroy();
      }
    } else
      return CGraphNode::Destroy();
  }

  return false;
}

void CNewWellPathInput::Init() {
  pointSetType(IPointSet::INPUT);

  CDepletionStage &initial_stage = static_cast<CModelBase &>(Model()).InitialDepletionStage();

  BirthDate(QDate(initial_stage.Time().GetYear(), initial_stage.Time().GetMonth(), 1));
}

void CNewWellPathInput::GetInterSectionPointsWithFormationSkin(const geo::ILine &line,
                                                               std::set<geo::CPoint> &PointSet) const {
  const CModelBase *pModel = &static_cast<const CModelBase &>(Model());

  if (!pModel->IsMesh())
    return;

  // loop over the volumes in a formation
  CNewWellPathInput *pThis = const_cast<CNewWellPathInput *>(this);

  TFormationBaseEntry::TNodeSet formations =
      ((TFormationBaseEntry *)pThis->Model().GraphEntry(MD_BASE_FORMATION))->EntryNodes();
  // loop over the formations
  for (TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); ++iter) {
    // loop over the volumes in a formation
    for (int i = 0; i < (*iter)->ElementSetSize(); ++i) {
      geo::CBodyGroup *pGeoVolume = dynamic_cast<geo::CBodyGroup *>(&(*iter)->ElementSet(i).ElementSet());
      assert(pGeoVolume);

      // get the intersection points with the volume and the vertical line
      pGeoVolume->IntersectionWithEdgeFaces(line, PointSet);
    }
  }
}

void CNewWellPathInput::InitVerticalWell(bool bResetGlobalTVD) {
  CModelBase *pModel = &static_cast<CModelBase &>(Model());

  if (!pModel->IsMesh())
    return;

  double Zmax = pModel->Mesh().Mesh().Max().Z();
  double Zmin = pModel->Mesh().Mesh().Min().Z();

  double maxZ = Zmax + (Zmax - Zmin) / 2;
  double minZ = Zmin - (Zmax - Zmin) / 2;

  geo::CPoint p1(m_dGlobalNorthing, m_dGlobalEasting, minZ);
  geo::CPoint p2(m_dGlobalNorthing, m_dGlobalEasting, maxZ);
  geo::CLine line(p1, p2);

  std::set<geo::CPoint> PointSet;
  GetInterSectionPointsWithFormationSkin(line, PointSet);

  if (PointSet.size() == 0)
    return;

  const geo::CPoint *first = &*PointSet.begin();
  const geo::CPoint *last = &*PointSet.rbegin();

  // initialization of a vertical wellpath (SI_UNIT)
  InitVertical(first->X(), first->Y(), bResetGlobalTVD ? first->Z() : GlobalTVD(), 0, last->Z());
}

void CNewWellPathInput::InitVertical(double start_northing, double start_easting, double start_depth, double start_tmd,
                                     double end_depth) {
  m_dGlobalNorthing = start_northing;
  m_dGlobalEasting = start_easting;
  m_dGlobalTVD = start_depth;

  if (PointSize() == 0) {
    std::vector<double> point(3);
    point[0] = start_northing;
    point[1] = start_easting;
    point[2] = start_tmd + start_depth;

    PushBack(point);

    point[2] = (start_tmd + start_depth + end_depth) / 2;

    PushBack(point);

    point[2] = end_depth;

    PushBack(point);
  } else {
    assert(PointSize() == 3);

    SetPoint(0, geo::CPoint(start_northing, start_easting, start_tmd + start_depth));
    SetPoint(1, geo::CPoint(start_northing, start_easting, (start_tmd + start_depth + end_depth) / 2));
    SetPoint(2, geo::CPoint(start_northing, start_easting, end_depth));
  }

  CalcBB();

  assert(IsVertical());
}

void CNewWellPathInput::InitFromPointArray(QVector<geo::CPoint> &vcPoint, bool setGlobals, QVector<double> *azimuth,
                                           QVector<double> *inclination) {
  assert(PointSize() == 0);

  if (vcPoint.size() == 0)
    return;

  assert(!vcPoint[0].Empty());

  if (setGlobals) {
    m_dGlobalNorthing = vcPoint[0].X();
    m_dGlobalEasting = vcPoint[0].Y();
    m_dGlobalTVD = vcPoint[0].Z();
  }

  bool haveAziInc =
      azimuth && inclination && azimuth->size() == vcPoint.size() && inclination->size() == vcPoint.size();

  std::vector<double> point(3 + (haveAziInc ? 2 : 0), 0);

  point[0] = vcPoint[0].X();
  point[1] = vcPoint[0].Y();
  point[2] = vcPoint[0].Z();

  if (haveAziInc) {
    point[2] = (*azimuth)[0];
    point[3] = (*inclination)[0];
  }

  PushBack(point);

  if (vcPoint.size() == 2) // bug in OIV viewing of pointsets with 2 points; we add one half-way
  {
    point[0] = (vcPoint[1].X() + vcPoint[0].X()) / 2;
    point[1] = (vcPoint[1].Y() + vcPoint[0].Y()) / 2;
    point[2] = (vcPoint[1].Z() + vcPoint[0].Z()) / 2;

    if (haveAziInc) {
      point[2] = ((*azimuth)[1] + (*azimuth)[0]) / 2;
      point[3] = ((*inclination)[1] + (*inclination)[0]) / 2;
    }
    PushBack(point);
  }

  for (size_t i = 1; i < (size_t)vcPoint.size(); ++i) {
    point[0] = vcPoint[i].X();
    point[1] = vcPoint[i].Y();
    point[2] = vcPoint[i].Z();

    if (haveAziInc) {
      point[2] = (*azimuth)[i];
      point[3] = (*inclination)[i];
    }

    PushBack(point);
  }

  CalcBB();
}

// wjrx mantis 3322 Attributes
// offset comes from a change in dialog Wellpath_attr/Derrick Depth Coord
void CNewWellPathInput::Recalculate(double offset) {
  for (size_t i = 0; i < PointSize(); ++i) {
    const geo::IPoint &curPt = PointAt(i);
    SetPoint(i, geo::CPoint(curPt.X(), curPt.Y(), curPt.Z() + offset));
  }

  CalcBB();

  Modified();
}

void CNewWellPathInput::Recalculate(const geo::CPoint &point) { Recalculate(point.X(), point.Y(), point.Z()); }

void CNewWellPathInput::Recalculate(double Global_Northing, double Global_Easting, double Global_TVD) {
  GlobalNorthing(Global_Northing);
  GlobalEasting(Global_Easting);
  GlobalTVD(Global_TVD);

  InitVerticalWell(false);

  Modified();
}

bool CNewWellPathInput::CanConnectItem(const CGraphNode & /*item*/) const { return false; }

bool CNewWellPathInput::ConnectItem(const CGraphNode & /*item*/) { return false; }

void CNewWellPathInput::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CPointSet::LoadStream(stream, version, progress);

  if (version <= CStreamVersion(4, 1, 42)) {
    int dummy;
    stream >> dummy;
  } else // >= 4.1.43
  {
    int vertical;
    stream >> vertical;
    m_bIsVertical = vertical != 0;

    int year, month, day;
    stream >> year;
    stream >> month;
    stream >> day;
    BirthDate(QDate(year, month, day));

    stream >> m_dGlobalNorthing;
    stream >> m_dGlobalEasting;
    stream >> m_dGlobalTVD;
  }

  CalcBB();
}

void CNewWellPathInput::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  CPointSet::SaveStream(stream, progress);

  stream << int(IsVertical() ? 1 : 0);

  stream << BirthDate().year();
  stream << BirthDate().month();
  stream << BirthDate().day();

  stream << GlobalNorthing();
  stream << GlobalEasting();
  stream << GlobalTVD();
}

bool CNewWellPathInput::CheckVerticality() const {
  if (IsVertical())
    return true;

  bool vertical = true;

  for (size_t i = 0; i < PointSize(); ++i) {
    const geo::IPoint &p = PointAt(i);
    double deltaX = fabs(m_dGlobalNorthing - p.X());
    double deltaY = fabs(m_dGlobalEasting - p.Y());

    // using a more rigid test here than 'm_dApproximateRadius
    if (deltaX > 0.1 || deltaY > 0.1) {
      vertical = false;
      break;
    }
  }

  return vertical;
}

void CNewWellPathInput::CalcBB() {
  m_min.Set(DBL_MAX, DBL_MAX, DBL_MAX);
  m_max.Set(-DBL_MAX, -DBL_MAX, -DBL_MAX);

  if (PointSize() == 0)
    return;

  for (size_t i = 0; i < PointSize(); ++i) {
    const geo::CPoint &p = PointAt(i);

    if (p.X() < m_min.X())
      m_min.X(p.X());
    if (p.Y() < m_min.Y())
      m_min.Y(p.Y());
    if (p.Z() < m_min.Z())
      m_min.Z(p.Z());

    if (p.X() > m_max.X())
      m_max.X(p.X());
    if (p.Y() > m_max.Y())
      m_max.Y(p.Y());
    if (p.Z() > m_max.Z())
      m_max.Z(p.Z());
  }

  m_min.Set(m_min.X() - m_dApproximateRadius, m_min.Y() - m_dApproximateRadius, m_min.Z());
  m_max.Set(m_max.X() + m_dApproximateRadius, m_max.Y() + m_dApproximateRadius, m_max.Z());
}

void CNewWellPathInput::FlipDepth() {
  size_t size = PointSize();

  std::vector<geo::CPoint> points;
  points.reserve(size);

  for (size_t i = 0; i < size; ++i) {
    points.push_back(PointAt(i));
    points.back().Z(-points.back().Z());
  }

  for (size_t i = 0; i < size; ++i) {
    SetPoint(i, points[size - i - 1]);
  }

  const geo::IPoint &first = PointAt(0);

  GlobalNorthing(first.X());
  GlobalEasting(first.Y());
  GlobalTVD(first.Z());

  Modified(WELLPATH_FLIPPED);
}