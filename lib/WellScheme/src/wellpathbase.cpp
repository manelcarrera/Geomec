// wellpath.cpp: implementation of the WellPath class.
//
//////////////////////////////////////////////////////////////////////

#include "wellpathbase.h"
#include <QList>
#include <QVector>
#include <cmath>
#include <float.h>
#include <qfile.h>
// #include <qmemarray.h>
#include "BoundingBox.h"
#include <QTextStream>
#include <qglobal.h>

#if !defined(WIN32) && !defined(WIN64)
#define _copysign copysign
#endif // !defined(WIN32) && !defined(WIN64)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static bool InterpolateInclination_newton(const double &tmd1, const double &tmd2, const double &tvd1,
                                          const double &tvd2, const double &azi1, const double &azi2,
                                          const double &inc1, double &inc2);

/*
static bool InterpolateInclination_secant(const double& tmd1,
                const double& tmd2,
                const double& tvd1,
                const double& tvd2,
                const double& azi1,
                const double& azi2,
                const double& inc1,
                double& inc2);
*/

static bool InterpolateTmd_newton(const double &tmd1, const double &tmd2, const double &tvd1, const double &tvd2,
                                  const double &azi1, const double &azi2, const double &inc1, const double &inc2,
                                  const double &tvd, double &tmd);

static double GetDeltaTMD(const double &a1, const double &a2, const double &i1, const double &i2, const double &tmd1,
                          const double &tmd2, const double &tvd1, const double &tvd2, const double &tvd);

namespace well {

/* Implementation of CWellDateList ********************************************************/

CWellDateList::CWellDateList() { AutoDelete(true); }

CWellDateList::~CWellDateList() { Clear(); }

CWellDateList &CWellDateList::operator=(const CWellDateList &rhs) {
  // QPtrList<QDate>::operator =(rhs);
  m_lstDates = rhs.m_lstDates;
  return *this;
}

CWellDateList::CWellDateList(const CWellDateList &rhs)
    : QObject(), // QObject(const QObject&) is private,
                 // hence QObject(QObject* parent = 0)
      m_lstDates(rhs.m_lstDates), m_bAutoDelete(false) {}

CWellDateList::Iterator CWellDateList::begin() const { return This()->m_lstDates.begin(); }

CWellDateList::Iterator CWellDateList::end() const { return This()->m_lstDates.end(); }

CWellDateList::Iterator CWellDateList::GetIteratorAt(QDate *pDate) const {
  Iterator ret;
  for (ret = This()->m_lstDates.begin(); ret != This()->m_lstDates.end(); ++ret) {
    if (*ret == pDate)
      return ret;
  }

  return This()->m_lstDates.end();
}

int CWellDateList::compareItems(QDate *item1, QDate *item2) {
  QDate *p1 = (QDate *)(item1);
  QDate *p2 = (QDate *)(item2);

  if (*p1 == *p2)
    return 0;

  if (*p1 > *p2)
    return 1;

  return -1;
}

int CWellDateList::AddDate(QDate *pDate) {
  if (m_lstDates.indexOf(pDate) != -1) {
    return -1;
  }

  QList<QDate *>::iterator it;
  for (it = m_lstDates.begin(); it != m_lstDates.end(); ++it) {
    int comp = compareItems(*it, pDate);
    if (comp == 0)
      return -1; // already there
    if (comp > 0)
      break; // insert before this one
  }
  m_lstDates.insert(it, pDate);
  return m_lstDates.indexOf(pDate);
}

bool CWellDateList::RemoveDate(int nIndex) {
  bool bRet = (nIndex < m_lstDates.size());

  if (bRet)
    m_lstDates.removeAt(nIndex);

  return bRet;
}

QDate *CWellDateList::At(int index) { return m_lstDates.at(index); }

const QDate *CWellDateList::At(int index) const { return This()->m_lstDates.at(index); }

CWellDateList *CWellDateList::This() const { return const_cast<CWellDateList *>(this); }

int CWellDateList::Size() const { return m_lstDates.count(); }

QDate *CWellDateList::First() const {
  This()->m_iterator = This()->m_lstDates.begin();
  if (This()->m_iterator == This()->end())
    return 0;

  return *This()->m_iterator;
}

QDate *CWellDateList::Next() const {
  if (This()->m_iterator == This()->end())
    return 0;

  ++(This()->m_iterator);
  if (This()->m_iterator == This()->end())
    return 0;

  return *m_iterator;
}

void CWellDateList::AutoDelete(bool bAutoDelete) { m_bAutoDelete = bAutoDelete; }

void CWellDateList::Clear() {
  if (m_bAutoDelete) {
    Iterator it;
    for (it = m_lstDates.begin(); it != m_lstDates.end(); ++it)
      delete *it;
  }
  m_lstDates.clear();
}
/******************************************************************************************/

void CWellPathBase::Init() {
  m_GlobalNorthing.SetValue(0);
  m_GlobalEasting.SetValue(0);
  m_GlobalTVD.SetValue(0);

  m_pDefPointList = new CWellDefinitionPointList(*this);

  m_AutoDeletePointLists = true;
  m_AutoDeleteSectionLists = true;

  m_Location = ONSHORE;
  m_BirthDate.setDate(8000, 1, 1); // as a default, the date is set to januari 1st, 8000

  connect(&m_MeanGroundOrSeabedLevel, SIGNAL(OnChanged(const CDoubleQuantity &)), this,
          SLOT(OnQuantityChanged(const CDoubleQuantity &)));
  connect(&m_MeanSeaLevel, SIGNAL(OnChanged(const CDoubleQuantity &)), this,
          SLOT(OnQuantityChanged(const CDoubleQuantity &)));
}

/*!
  Initialize with points
  X = NORTHING
  Y = EASTING
  Z = DEPTH
*/
bool CWellPathBase::InitFromPointArray(const QVector<geo::CPoint> &vcPoint) {
  if (vcPoint.count() < 2)
    return false;

  QVector<double> azi(vcPoint.size());
  QVector<double> inc(vcPoint.size());
  QVector<double> tmd(vcPoint.size());

  tmd[0] = 0.; // First tmd value
  for (int i = 0; i < tmd.count() - 1; i++) {
    geo::CVector v(vcPoint[i], vcPoint[i + 1]);

    double dLength2D = sqrt(v.X() * v.X() + v.Y() * v.Y());
    double dLength3D = v.Length();

    double dInc = RAD2DEG(atan2(dLength2D, v.Z()));

    double azi_rad = atan2(v.Y(), v.X());
    if (azi_rad < 0)
      azi_rad = 2.0 * PI + azi_rad;
    double dAzimuth = RAD2DEG(azi_rad);

    tmd[i + 1] = tmd[i] + dLength3D;
    azi[i] = dAzimuth;
    inc[i] = dInc;
  }

  InitFromArray_tmd_azi_inc(vcPoint[0].X(), vcPoint[0].Y(), vcPoint[0].Z(), 0, 0, 0, tmd, azi, inc);

  return true;
}

// ##ModelId=3F7AA1570123
CWellPathBase::~CWellPathBase() {
  RemoveAllWellSectionLists();
  RemoveAllWellPointLists();
  delete m_pDefPointList;
}

CWellPathBase::CWellPathBase(const CWellPathBase &right) : CCommonWellPath(right.text()), m_pDefPointList(0) {
  create();

  // This is NOT a complete copy of the object. the sectionlistmap and the pointlistmap are not copied.
  m_GlobalEasting = right.m_GlobalEasting;
  m_GlobalNorthing = right.m_GlobalNorthing;
  m_GlobalTVD = right.m_GlobalTVD;
  // m_AutoDeletePointLists = right.m_AutoDeletePointLists;
  // m_AutoDeleteSectionLists = right.m_AutoDeleteSectionLists;
  m_BirthDate = right.m_BirthDate;
  m_Country = right.m_Country;
  m_Description = right.m_Description;
  m_HoleName = right.m_HoleName;
  m_Location = right.m_Location;
  m_LocationName = right.m_LocationName;
  m_MeanGroundOrSeabedLevel = right.m_MeanGroundOrSeabedLevel;
  m_MeanSeaLevel = right.m_MeanSeaLevel;
  m_SpudDate = right.m_SpudDate;
  m_WellName = right.m_WellName;
  m_WellType = right.m_WellType;

  m_pDefPointList = new CWellDefinitionPointList(*this);

  m_AutoDeletePointLists = true;
  m_AutoDeleteSectionLists = true;

  HardCopyDefPointList(right);

  connect(&m_MeanGroundOrSeabedLevel, SIGNAL(OnChanged(const CDoubleQuantity &)), this,
          SLOT(OnQuantityChanged(const CDoubleQuantity &)));
  connect(&m_MeanSeaLevel, SIGNAL(OnChanged(const CDoubleQuantity &)), this,
          SLOT(OnQuantityChanged(const CDoubleQuantity &)));
}

CWellPathBase::CWellPathBase(const QString &text) : CCommonWellPath(text), m_pDefPointList(0) {}

CWellPathBase::CWellPathBase() : CCommonWellPath("CWellPathBase"), m_pDefPointList(0) {}

/*virtual*/ void CWellPathBase::onCreate() {
  assert(!m_pDefPointList);
  Init();
}

///*virtual*/ void CWellPathBase::EmitDefPointsChanged()
//{
//	emit DefPointsChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitWellNameChanged()
//{
//	emit WellNameChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitHoleNameChanged()
//{
//	emit HoleNameChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitLocationChanged()
//{
//	emit LocationChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitMeanGroundOrSeabedLevelChanged()
//{
//	emit MeanGroundOrSeabedLevelChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitMeanSeaLevelChanged()
//{
//	emit MeanSeaLevelChanged();
//}
//
///*virtual*/ void CWellPathBase::EmitStartLongJob(const QString& description,long NrOfSteps)
//{
//	emit StartLongJob(description,NrOfSteps);
//}
//
///*virtual*/ void CWellPathBase::EmitDuringLongJob(long Step)
//{
//	emit DuringLongJob(Step);
//}
//
///*virtual*/ void CWellPathBase::EmitFinishedLongJob()
//{
//	emit FinishedLongJob();
//}
//

void CWellPathBase::OnQuantityChanged(const CDoubleQuantity &quantity) {
  if (&quantity == &m_MeanGroundOrSeabedLevel) {
    geometryChanged();
  } else if (&quantity == &m_MeanSeaLevel) {
    geometryChanged();
  }
}

bool CWellPathBase::AutoDeletePointLists() const { return m_AutoDeletePointLists; }

void CWellPathBase::AutoDeletePointLists(bool autodel) { m_AutoDeletePointLists = autodel; }

bool CWellPathBase::AutoDeleteSectionLists() const { return m_AutoDeleteSectionLists; }

void CWellPathBase::AutoDeleteSectionLists(bool autodel) { m_AutoDeleteSectionLists = autodel; }

// ##ModelId=3F7AA157015A
const CWellDefinitionPointList &CWellPathBase::DefPointList() const { return *m_pDefPointList; }

// ##ModelId=3F7AA1570160
CWellDefinitionPointList &CWellPathBase::DefPointList() { return *m_pDefPointList; }

void CWellPathBase::InvalidateDefPointList() {
  m_pDefPointList->RemoveAllPoints();
  geometryChanged();
}

void CWellPathBase::HardCopyDefPointList(const CWellPathBase &org_wellpath) {
  m_pDefPointList->RemoveAllPoints();
  CWellPointList::Iterator it = org_wellpath.m_pDefPointList->begin();
  while (it != org_wellpath.m_pDefPointList->end()) {
    CWellPoint *p = new CWellPoint();
    p->m_pWellPath = this;
    p->m_Azimuth.SetValue((*it)->m_Azimuth.Value());
    p->m_Easting.SetValue((*it)->m_Easting.Value());
    p->m_Inclination.SetValue((*it)->m_Inclination.Value());
    p->m_Northing.SetValue((*it)->m_Northing.Value());
    p->m_TMD.SetValue((*it)->m_TMD.Value());
    p->m_TVD.SetValue((*it)->m_TVD.Value());
    p->Connect();
    m_pDefPointList->AddPoint(p);
    ++it;
  }
  geometryChanged();
}

void CWellPathBase::GetBoundingBox(geo::IPoint &max, geo::IPoint &min) const {
  CWellPointList::Iterator it = m_pDefPointList->begin();
  CWellPoint *p1 = 0;
  CWellPoint *p2 = 0;

  if (it != m_pDefPointList->end())
    p1 = *it++;
  if (it != m_pDefPointList->end())
    p2 = *it;

  while (p1 && p2) {
    if (p1 == m_pDefPointList->First()) {
      max = p2->Max(*p1);
      min = p2->Min(*p1);
    } else {
      max = max.Max(*p2);
      min = min.Min(*p2);
    }

    if ((p1->Inclination().Value() < 90.0 && p2->Inclination().Value() > 90.0) ||
        (p1->Inclination().Value() > 90.0 && p2->Inclination().Value() < 90.0)) {
      geo::CVector n = p1->GetDirection().CrossProduct(p2->GetDirection()).UnitVector();
      double DLS = GetDogLegSeverity(p2->TMD().Value());
      if (fabs(DLS) > EPS) {
        // TODO:
        // find more speedup/analystic exact algorithm for finding the boundingbox
        // of circle segment
        // double Rad = 1.0 / DLS;
        // geo::CPoint mid = *p1 + n*Rad;

        int nr_sections = 10;
        double delta_tmd = (p2->TMD().Value() - p1->TMD().Value()) / nr_sections;

        for (int i = 0; i < nr_sections - 1; i++) {
          double tmd = p1->TMD().Value() + i * delta_tmd;
          CWellPoint p(*this, tmd);
          max = max.Max(p);
          min = min.Min(p);
        }
      }
    }

    p1 = p2;
    ++it;

    if (it != m_pDefPointList->end())
      p2 = *it;
    else
      p2 = 0;
  }
}

static CWellPoint *CalculateInterSectionWithPlane(const geo::IPlane &plane, CWellPoint *p1, CWellPoint *p2) {

  const int JMAX = 20;
  int j;
  const double xacc = EPS;

  double x1 = p1->TMD().Value();
  double x2 = p2->TMD().Value();

  double df, dx, f, rtn;

  rtn = 0.5 * (x1 + x2);
  CWellPoint *ipoint = new CWellPoint(p1->WellPath(), rtn);

  for (j = 0; j < JMAX; j++) {
    // calculate f,df for given rtn
    ipoint->TMD().SetValue(rtn);
    CWellPoint temp(p1->WellPath(), rtn + 0.1);

    f = plane.SignedDistance(*ipoint, true);
    df = plane.SignedDistance(temp, true) - f;

    dx = f / df;
    rtn = rtn - dx;
    if ((x1 - rtn) * (rtn - x2) < 0.0) {
      delete ipoint;
      return 0;
    }

    if (fabs(dx) < xacc) {
      if (plane.Distance(*ipoint) <= EPS)
        return ipoint;
      else {
        delete ipoint;
        return 0;
      }
    }
  }

  return 0;
}

//*******************************************************************
//
//  COMMENTS:   uses Neton-Raphson for calculating intersection with a face
//
//*******************************************************************
static CWellPoint *CalculateInterSectionWithFace(const geo::IFace &face, CWellPoint *p1, CWellPoint *p2) {

  const int JMAX = 20;           // max itterations
  const double xacc = 0.5 * EPS; // convergence criteria

  double x1 = p1->TMD().Value();
  double x2 = p2->TMD().Value();

  double df, dx, f, tmd;

  tmd = 0.5 * (x1 + x2); // start Value

  CWellPoint *point = new CWellPoint(p1->WellPath(), tmd); // start point

  // see if face is a Quadrilateral
  const geo::IQuadrilateral *quad = dynamic_cast<const geo::IQuadrilateral *>(&face);

  const double delta_x = 0.001; // used for calcualtion of derived
  for (int j = 0; j < JMAX; j++) {
    if (quad && !quad->IsPlanar()) {
      // if(!quad->ContainsInXYPlane(*point, true))
      //{
      //	delete point;
      //	return 0;
      // }

      // calculate vertical distance
      double d1 = quad->GetDepthAt(point->X(), point->Y());
      double d2 = point->Z();
      f = d2 - d1;

      if (fabs(f) < xacc) {
        //				assert(face.Contains(*point,true));
        return point;
      }

      // calculate numerical derived (use delata_x)
      CWellPoint temp(p1->WellPath(), tmd + delta_x);
      d1 = quad->GetDepthAt(temp.X(), temp.Y());
      d2 = temp.Z();
      double f2 = d2 - d1;
      df = (f2 - f) / delta_x;
    } else {
      // calculate distance
      f = face.SignedDistance(*point, true);
      if (fabs(f) < xacc) {
        if (face.Contains(*point, true))
          return point;
        else {
          // delete point;
          // return 0;
        }
      }

      // calculate numerical derived (use delata_x)
      CWellPoint temp(p1->WellPath(), tmd + delta_x);
      double f2 = face.SignedDistance(temp, true);
      df = (f2 - f) / delta_x;
    }

    // calculate new tmd Value
    dx = f / df;
    tmd = tmd - dx;

    // numerical unequalties....
    if (fabs(tmd - x1) < EPS)
      tmd = x1;

    // numerical unequalties....
    if (fabs(tmd - x2) < EPS)
      tmd = x2;

    double tmp = (x1 - tmd) * (tmd - x2);
    if (tmp < -EPS) {
      // tmd has jumped out of the initial boundarys (x,x2)
      delete point;
      return 0;
    }

    // get point at tmd;
    point->TMD().SetValue(tmd);
  }

  return 0;
}

namespace {

class ZLess {
public:
  bool operator()(const CWellPoint *p1, const CWellPoint *p2) { return (p1->TMD().Value() < p2->TMD().Value()); }
};

} // anonymous namespace

void CWellPathBase::CalculateIntersections(const geo::IMesh &mesh, CWellPointList &list) const {
  CWellPointList::Iterator it = DefPointList().begin();

  CWellPoint *pWellPoint = 0;
  CWellPoint *pNext = 0;

  if (it != DefPointList().end())
    pWellPoint = *it++;
  if (it != DefPointList().end())
    pNext = *it;

  typedef std::set<CWellPoint *, ZLess> TWellPointSet;
  TWellPointSet stPoints;

  // This()->EmitStartLongJob("Calculate intersection points", DefPointList().NrOfPoints());

  while (pNext) {
    // This()->EmitDuringLongJob(++count);

    // Take an initial guess
    std::set<int> stElementIndices = mesh.Candidates(pWellPoint->Min(*pNext), pWellPoint->Max(*pNext));
    for (std::set<int>::iterator itIndex = stElementIndices.begin(); itIndex != stElementIndices.end(); itIndex++) {
      const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&mesh.Element(*itIndex));
      if (pBody) {
        if (geo::BBox::Intersects(*pWellPoint, *pNext, pBody->Max(), pBody->Min(), true)) {
          int nNrOfFaces = pBody->NrOfFaces();
          for (int i = 0; i < nNrOfFaces; i++) {
            const geo::IFace &face = pBody->Face(i);
            if (geo::BBox::Intersects(*pWellPoint, *pNext, face.Max(), face.Min(), true)) {
              CWellPoint *p = CalculateInterSectionWithFace(face, pWellPoint, pNext);
              if (p) {
                std::pair<TWellPointSet::iterator, bool> prInsert = stPoints.insert(p);
                (*prInsert.first)->addElement(pBody);
                if (!prInsert.second)
                  delete p;
                // list.AddPoint(p);
              }
            }
          }
        }
      }
    }

    pWellPoint = pNext;
    ++it;
    if (it != DefPointList().end())
      pNext = *it;
    else
      pNext = 0;
  }

  for (TWellPointSet::iterator itt = stPoints.begin(); itt != stPoints.end(); ++itt)
    list.AddPoint(*itt);
  // This()->EmitFinishedLongJob();
}

void CWellPathBase::GetIntersectionPointsWithPlane(const geo::CPlane &plane, CWellPointList &list) const {
  CWellPointList::Iterator it = DefPointList().begin();

  CWellPoint *pWellPoint = 0;
  CWellPoint *pNext = 0;

  if (it != DefPointList().end())
    pWellPoint = *it++;
  if (it != DefPointList().end())
    pNext = *it;

  while (pNext) {
    if (_copysign(1, plane.SignedDistance(*pWellPoint)) != _copysign(1, plane.SignedDistance(*pNext))) {
      CWellPoint *p = CalculateInterSectionWithPlane(plane, pWellPoint, pNext);
      if (p)
        list.AddPoint(p);
    }
    pWellPoint = pNext;
    ++it;
    if (it != DefPointList().end())
      pNext = *it;
    else
      pNext = 0;
  }
}

void CWellPathBase::InsertWellSectionList(CWellSectionList *pList, int listID) {
  m_SectionListMap.insert(listID, pList);
}

// ##ModelId=3F7AA1570146
CWellSectionList &CWellPathBase::GetWellSectionList(int listID) {
  QMap<int, CWellSectionList *>::iterator it = m_SectionListMap.find(listID);
  assert(it != m_SectionListMap.end());
  return *m_SectionListMap[listID];
}

// ##ModelId=3F7AA1570148
const CWellSectionList &CWellPathBase::GetWellSectionList(int listID) const {
  QMap<int, CWellSectionList *>::const_iterator it = m_SectionListMap.find(listID);
  assert(it != m_SectionListMap.end());
  return *m_SectionListMap[listID];
}

// ##ModelId=3F7AA157014F
void CWellPathBase::RemoveWellSectionList(int listID) {
  QMap<int, CWellSectionList *>::iterator it = m_SectionListMap.find(listID);
  if (m_AutoDeleteSectionLists) {
    assert(it != m_SectionListMap.end());
    CWellSectionList *list = *it;
    delete list;
  }

  m_SectionListMap.erase(it);
}

// ##ModelId=3F7AA1570151
void CWellPathBase::RemoveAllWellSectionLists() {
  if (m_AutoDeleteSectionLists) {
    while (m_SectionListMap.count()) {
      QMap<int, CWellSectionList *>::iterator it = m_SectionListMap.begin();
      CWellSectionList *list = *it;
      delete list;
      m_SectionListMap.erase(it);
    }
  } else {
    m_SectionListMap.clear();
  }
}

// ##ModelId=3F7AA1570130
void CWellPathBase::InsertWellPointList(CWellPointList *pList, int listID) {
  assert(!IsWellPointList(listID));
  m_PointListMap.insert(listID, pList);
}

QList<int> CWellPathBase::GetAllPointListID() const { return m_PointListMap.keys(); }

bool CWellPathBase::IsWellPointList(int listID) const {
  QList<int> list = m_PointListMap.keys();
  QList<int>::iterator it;
  for (it = list.begin(); it != list.end(); ++it) {
    if (*it == listID)
      return true;
  }
  return false;
}

int CWellPathBase::GetUniqueWellPointListID() const {
  for (int i = 0; i < 1000000; i++) {
    if (!IsWellPointList(i))
      return i;
  }
  assert(false);
  return -1;
}

// ##ModelId=3F7AA1570132
CWellPointList &CWellPathBase::GetWellPointList(int listID) {
  QMap<int, CWellPointList *>::iterator it = m_PointListMap.find(listID);
  assert(it != m_PointListMap.end());
  return *m_PointListMap[listID];
}

// ##ModelId=3F7AA1570134
const CWellPointList &CWellPathBase::GetWellPointList(int listID) const {
  QMap<int, CWellPointList *>::const_iterator it = m_PointListMap.find(listID);
  assert(it != m_PointListMap.end());
  return *This()->m_PointListMap[listID];
}

int CWellPathBase::AddDate(QDate *pDate) { return m_DateList.AddDate(pDate); }

QDate *CWellPathBase::GetDate(int nIndex) { return m_DateList.At(nIndex); }

// ##ModelId=3F7AA1570137
void CWellPathBase::RemoveWellPointList(int listID) {
  QMap<int, CWellPointList *>::iterator it = m_PointListMap.find(listID);
  if (m_AutoDeletePointLists) {
    assert(it != m_PointListMap.end());
    CWellPointList *list = *it;
    delete list;
  }

  m_PointListMap.erase(it);
}

// ##ModelId=3F7AA1570139
void CWellPathBase::RemoveAllWellPointLists() {
  if (m_AutoDeletePointLists) {
    while (m_PointListMap.count()) {
      QMap<int, CWellPointList *>::iterator it = m_PointListMap.begin();
      CWellPointList *list = *it;
      delete list;
      m_PointListMap.erase(it);
    }
  } else {
    m_PointListMap.clear();
  }
}

// ##ModelId=3F7AA15701B3
CWellPathBase *CWellPathBase::This() const { return const_cast<CWellPathBase *>(this); }

bool CWellPathBase::IsSnakeWell() const {

  CWellPointList::Iterator it = DefPointList().begin();
  while (it != DefPointList().end()) {
    CWellPoint *p = *it;
    ++it;
    if (p->Inclination().Value() > 90 || p->Inclination().Value() < -90)
      return true;
  }
  return false;
}

bool CWellPathBase::IsDeviated() const {
  CWellPointList::Iterator it = DefPointList().begin();
  while (it != DefPointList().end()) {
    CWellPoint *p = *it;
    ++it;
    if (p->Inclination().Value() > 0.0)
      return true;
  }
  return false;
}

bool CWellPathBase::IsStraight() const {
  assert(DefPointList().NrOfPoints() > 0);

  double inc = 0;
  double azi = 0;
  CWellPointList::Iterator it = DefPointList().begin();
  if (it != DefPointList().end()) {
    inc = (*it)->Inclination().Value();
    azi = (*it)->Azimuth().Value();
    ++it;
  }

  while (it != DefPointList().end()) {
    CWellPoint *p = *it;

    if (fabs(p->Inclination().Value() - inc) > EPS)
      return false;

    if (fabs(p->Azimuth().Value() - azi) > EPS)
      return false;

    ++it;
  }
  return true;
}

// ##ModelId=3F7AA1570173
bool CWellPathBase::IsVertical() const { return !IsDeviated(); }

void CWellPathBase::InitVertical(double start_northing, double start_easting, double start_depth, double start_tmd,
                                 double end_depth) {
  assert(end_depth > start_depth);
  assert(start_tmd >= 0);

  blockSignals(true);

  m_pDefPointList->RemoveAllPoints();

  m_GlobalNorthing.SetValue(start_northing);
  m_GlobalEasting.SetValue(start_easting);
  m_GlobalTVD.SetValue(start_depth);

  CWellPoint *p1 = new CWellPoint();
  p1->m_pWellPath = this;
  p1->m_TMD.SetValue(start_tmd); // before putting in list first set TMD (sort in list)
  m_pDefPointList->AddPoint(p1);

  p1->m_Azimuth.SetValue(0);
  p1->m_Inclination.SetValue(0);
  p1->m_Northing.SetValue(0);
  p1->m_Easting.SetValue(0);
  p1->m_TVD.SetValue(0);
  p1->Connect();

  CWellPoint *p2 = new CWellPoint();
  p2->m_pWellPath = this;
  p2->m_TMD.SetValue(start_tmd + (end_depth - start_depth));
  m_pDefPointList->AddPoint(p2);
  p2->m_Azimuth.SetValue(0);
  p2->m_Inclination.SetValue(0);
  p2->m_Northing.SetValue(0);
  p2->m_Easting.SetValue(0);
  p2->m_TVD.SetValue((end_depth - start_depth));
  p2->Connect();
  blockSignals(false);

  geometryChanged();

  assert(IsVertical());
}

bool CWellPathBase::Defined() const {
  if (m_pDefPointList->NrOfPoints() < 2)
    return false;

  if (!m_pDefPointList->AllPointsDefined())
    return false;

  return true;
}

// ##ModelId=3F7AA15701BD
bool CWellPathBase::InitWellPoint(CWellPoint &Point) const {
  // can not init definition point
  if (!(DefPointList().GetIndex(&Point) == -1)) // i am a definition point
    return false;

  // see if definition points exists and defined
  if (!DefPointList().AllPointsDefined() || DefPointList().NrOfPoints() < 2) {
    Point.Invalidate();
    return false;
  }

  assert(!Point.TMD().Undefined());

  double TMD = Point.TMD().Value();

  CWellPoint *Upper = 0;
  CWellPoint *Lower = 0;
  GetUpLowDefPoints(TMD, &Upper, &Lower);

  assert(Upper || Lower);

  double eas, nor, tvd, azi, inc, dls;

  if (Upper && !Lower) {
    // above fisrt point
    GetNewPosition(Upper->Azimuth().Value(), Upper->Azimuth().Value(), Upper->TMD().Value(), TMD, TMD,
                   Upper->Inclination().Value(), Upper->Inclination().Value(), Upper->Easting().Value(),
                   Upper->Northing().Value(), Upper->TVD().Value(), eas, nor, tvd, azi, inc, dls);

  } else if (!Upper && Lower) {
    // below last point
    GetNewPosition(Lower->Azimuth().Value(), Lower->Azimuth().Value(), Lower->TMD().Value(), TMD, TMD,
                   Lower->Inclination().Value(), Lower->Inclination().Value(), Lower->Easting().Value(),
                   Lower->Northing().Value(), Lower->TVD().Value(), eas, nor, tvd, azi, inc, dls);

  } else {
    GetBetweenPosition(*Upper, *Lower, TMD, eas, nor, tvd, azi, inc);
  }

  Point.m_Azimuth.SetValue(azi);
  Point.m_Inclination.SetValue(inc);

  Point.m_Easting.SetValue(eas);
  Point.m_Northing.SetValue(nor);
  Point.m_TVD.SetValue(tvd);

  return true;
}

void CWellPathBase::GetUpLowDefPoints(double tmd, CWellPoint **pUpper, CWellPoint **pLower) const {
  double UpperVal = 0;
  double LowerVal = 0;

  const CWellDefinitionPointList &def_list = DefPointList();
  CWellPointList::Iterator it = def_list.begin();

  const CWellPoint *pFirst = def_list.First();
  const CWellPoint *pLast = def_list.Last();

  if (tmd - pFirst->TMD().Value() < 0.0) { // tmd above all definition points
    *pUpper = 0;
    *pLower = const_cast<CWellPoint *>(pFirst);
    return;
  }

  if (tmd - pLast->TMD().Value() > 0.0) { // tmd below all definition points
    *pUpper = const_cast<CWellPoint *>(pLast);
    *pLower = 0;
    return;
  }

  if (it != def_list.end())
    *pUpper = *it;
  else
    *pUpper = 0;

  ++it;
  if (it != def_list.end())
    *pLower = *it;
  else
    *pLower = 0;

  while (*pUpper && *pLower) { // Loop over sections....

    UpperVal = (*pUpper)->TMD().Value();
    LowerVal = (*pLower)->TMD().Value();

    if (fabs(UpperVal - tmd) < EPS)
      return;
    else if (fabs(LowerVal - tmd) < EPS)
      return;
    else if ((LowerVal - tmd > 0.0) && (UpperVal - tmd < 0.0))
      return;

    *pUpper = *pLower;

    ++it;
    if (it != def_list.end())
      *pLower = *it;
    else
      *pLower = 0;
  }

  // assert(false);
}

//*******************************************************************
//
//  FUNCTION:   InitFromArray_tmd_tvd_azi
//  RETURNS:    succes if is initialized
//  PARAMETERS: array containing Values (SI_UNIT!!)
//  COMMENTS:   uses min. curvature interpolation
//
//*******************************************************************
bool CWellPathBase::InitFromArray_tmd_tvd_azi(double global_northing, double global_easting, double global_depth,
                                              double start_northing, double start_easting, double start_inc,
                                              QVector<double> &tmd, QVector<double> &tvd, QVector<double> &azi)

{

  if (!(tmd.count() == azi.count() && azi.count() == tvd.count()))
    return false;

  if (tmd.count() < 2)
    return false;

  blockSignals(true);

  m_pDefPointList->RemoveAllPoints();

  m_GlobalNorthing.SetValue(global_northing);
  m_GlobalEasting.SetValue(global_easting);
  m_GlobalTVD.SetValue(global_depth);

  double nor1 = start_northing;
  double eas1 = start_easting;
  double inc1 = start_inc;

  double nor2 = 0;
  double eas2 = 0;
  double inc2 = start_inc;

  // Craete the first Point
  CWellPoint *Point1 = new CWellPoint();
  Point1->m_pWellPath = this;
  Point1->m_TMD.SetValue(tmd[0]);
  m_pDefPointList->AddPoint(Point1);

  Point1->m_Azimuth.SetValue(azi[0]);
  Point1->m_Inclination.SetValue(start_inc);
  Point1->m_Northing.SetValue(start_northing);
  Point1->m_Easting.SetValue(start_easting);
  Point1->m_TVD.SetValue(tvd[0]);

  Point1->Connect();

  double delta_inc = 0;

  for (int i = 0; i < tmd.count() - 1; i++) {

    if (i > 1) {
      inc2 = inc2 + delta_inc;
    }

    // get incl2
    InterpolateInclination_newton(tmd[i], tmd[i + 1], tvd[i], tvd[i + 1], azi[i], azi[i + 1], inc1, inc2);

    if (i >= 1) {
      delta_inc = inc2 - inc1;
    }

    double dum_azi, dum_inc, dum_dls;
    GetNewPosition(azi[i], azi[i + 1], tmd[i], tmd[i + 1], tmd[i + 1], inc1, inc2, eas1, nor1, tvd[i], eas2, nor2,
                   tvd[i + 1], dum_azi, dum_inc, dum_dls);

    CWellPoint *Point2 = new CWellPoint();
    Point2->m_pWellPath = this;
    Point2->m_TMD.SetValue(tmd[i + 1]);
    m_pDefPointList->AddPoint(Point2);

    Point2->m_Azimuth.SetValue(azi[i + 1]);
    Point2->m_Inclination.SetValue(inc2);

    Point2->m_Northing.SetValue(nor2);
    Point2->m_Easting.SetValue(eas2);
    Point2->m_TVD.SetValue(tvd[i + 1]);

    Point2->Connect();

    nor1 = nor2;
    eas1 = eas2;
    inc1 = inc2;
  }

  blockSignals(false);

  geometryChanged();
  return true;
}

//*******************************************************************
//
//  FUNCTION:   InitFromArray_tmd_azi_inc
//  RETURNS:    succes if is initialized
//  PARAMETERS: array containing Values (SI_UNIT!!)
//  COMMENTS:   uses min. curvature interpolation
//
//*******************************************************************
// ##ModelId=3F7AA1570161
bool CWellPathBase::InitFromArray_tmd_azi_inc(double global_northing, double global_easting, double global_depth,
                                              double start_northing, double start_easting, double start_depth,
                                              QVector<double> &tmd, QVector<double> &azi, QVector<double> &inc) {

  COperation op(*this);

  if (!(tmd.count() == azi.count() && azi.count() == inc.count()))
    return false;

  if (tmd.count() < 2)
    return false;

  blockSignals(true);

  m_pDefPointList->RemoveAllPoints();

  m_GlobalNorthing.SetValue(global_northing);
  m_GlobalEasting.SetValue(global_easting);
  m_GlobalTVD.SetValue(global_depth);

  double nor1 = start_northing;
  double eas1 = start_easting;
  double tvd1 = start_depth;

  // Create the first Point
  CWellPoint *Point1 = new CWellPoint();
  Point1->m_pWellPath = this;
  Point1->m_TMD.SetValue(tmd[0]);
  m_pDefPointList->AddPoint(Point1);

  Point1->m_Azimuth.SetValue(azi[0]);
  Point1->m_Inclination.SetValue(inc[0]);

  Point1->m_Northing.SetValue(nor1);
  Point1->m_Easting.SetValue(eas1);
  Point1->m_TVD.SetValue(tvd1);

  Point1->Connect();

  for (int i = 0; i < tmd.count() - 1; i++) {
    double tvd2;
    double nor2;
    double eas2;

    double dum_azi, dum_inc, dum_dls;
    GetNewPosition(azi[i], azi[i + 1], tmd[i], tmd[i + 1], tmd[i + 1], inc[i], inc[i + 1], eas1, nor1, tvd1, eas2, nor2,
                   tvd2, dum_inc, dum_azi, dum_dls);

    CWellPoint *Point2 = new CWellPoint();
    Point2->m_pWellPath = this;
    Point2->m_TMD.SetValue(tmd[i + 1]);
    m_pDefPointList->AddPoint(Point2);

    //		Point->m_Azimuth.SetValue(dum_azi);
    //		Point->m_Inclination.SetValue(dum_inc);

    Point2->m_Azimuth.SetValue(azi[i + 1]);
    Point2->m_Inclination.SetValue(inc[i + 1]);

    Point2->m_Northing.SetValue(nor2);
    Point2->m_Easting.SetValue(eas2);
    Point2->m_TVD.SetValue(tvd2);

    Point2->Connect();

    tvd1 = tvd2;
    nor1 = nor2;
    eas1 = eas2;
  }

  blockSignals(false);

  geometryChanged();
  return true;
}

// ##ModelId=3F7AA1570170
void CWellPathBase::DumpToFile(QString sFileName) const {
  QFile file(sFileName);
  file.open(QIODevice::WriteOnly);

  QTextStream stream(&file);
  stream.setRealNumberNotation(QTextStream::FixedNotation);

  CWellPointList::Iterator it = DefPointList().begin();

  while (it != DefPointList().end()) {
    stream << (*it)->TMD().Value() << "\t";
    stream << (*it)->Inclination().Value() << "\t";
    stream << (*it)->Azimuth().Value() << "\t";
    stream << (*it)->Northing().Value() << "\t";
    stream << (*it)->Easting().Value() << "\t";
    stream << (*it)->TVD().Value() << "\r\n";
    ++it;
  }

  file.close();
}

// ##ModelId=3F7AA1570168
bool CWellPathBase::InitFromMWRFile(QString sPathAndFileName) {
  QFile file(sPathAndFileName);
  file.open(QIODevice::ReadOnly);

  QTextStream ts(&file);
  QString line = ts.readLine();

  if (line.isNull())
    return false;

  double start_northing = 0;
  double start_easting = 0;
  double start_depth = 0;

  double global_northing = 0;
  double global_easting = 0;
  double global_depth = 0;

  QVector<double> a_tmd;
  QVector<double> a_azi;
  QVector<double> a_inc;

  int count = 0;

  while (!line.isNull()) {
    line = ts.readLine();
    if (line.isNull())
      break;

    line = line.simplified();
    if (line.indexOf("ahd inclination azimuth northing easting tvd") != -1) {

      line = ts.readLine();
      line = ts.readLine();
      line = ts.readLine();

      while (!line.isNull() && !line.isEmpty()) {
        QTextStream t2(&line, QIODevice::ReadOnly);
        double ahd, inclination, azimuth, northing, easting, tvd;
        t2 >> ahd >> inclination >> azimuth >> northing >> easting >> tvd;

        if (count == 0) {
          start_northing = northing;
          start_easting = easting;
          start_depth = tvd;
        }

        a_tmd.resize(count + 1);
        a_azi.resize(count + 1);
        a_inc.resize(count + 1);

        a_tmd[count] = ahd;
        a_azi[count] = azimuth;
        a_inc[count] = inclination;

        count++;

        line = ts.readLine();

        if (!(line.indexOf("!") == -1))
          break;
      }
    }
  }
  file.close();

  InitFromArray_tmd_azi_inc(global_northing, global_easting, global_depth, start_northing, start_easting, start_depth,
                            a_tmd, a_azi, a_inc);

  return true;
}

// ##ModelId=3F7AA1570152
QString CWellPathBase::GetWellName() const { return m_WellName; }

// ##ModelId=3F7AA1570154
void CWellPathBase::SetWellName(QString wellname) {
  m_WellName = wellname;
  onDisplayChanged(*this);
}

double CWellPathBase::GetDogLegSeverity(double tmd) const {
  CWellPointList &def_list = const_cast<CWellDefinitionPointList &>(DefPointList());
  if (tmd - def_list.First()->TMD().Value() < 0.0)
    return 0.0;

  if (tmd - def_list.Last()->TMD().Value() > 0.0)
    return 0.0;

  CWellPoint *Upper = 0;
  CWellPoint *Lower = 0;
  GetUpLowDefPoints(tmd, &Upper, &Lower);

  double dls, dum_eas, dum_nor, dum_tvd, dum_azi, dum_inc;

  if (Upper && Lower) {
    GetNewPosition(Upper->Azimuth().Value(), Lower->Azimuth().Value(), Upper->TMD().Value(), Lower->TMD().Value(), tmd,
                   Upper->Inclination().Value(), Lower->Inclination().Value(), Upper->Easting().Value(),
                   Upper->Northing().Value(), Upper->TVD().Value(), dum_eas, dum_nor, dum_tvd, dum_azi, dum_inc, dls);
    return dls;
  }

  CWellPoint *p1 = 0;
  CWellPoint *p2 = 0;

  if ((def_list.First() == Upper) ||
      (def_list.First() == Lower)) { // tmd is exactly on the first definition point, set to zero
    return 0.0;
  }

  // tmd is exactly on one of the points (except first), take the dls of the previous section
  if (Upper) {
    p1 = def_list.At(def_list.GetIndex(Upper) - 1);
    p2 = Upper;
  } else {
    p1 = def_list.At(def_list.GetIndex(Lower) - 1);
    p2 = Lower;
  }

  GetNewPosition(p1->Azimuth().Value(), p2->Azimuth().Value(), p1->TMD().Value(), p2->TMD().Value(),
                 (p1->TMD().Value() + p2->TMD().Value()) / 2.0, p1->Inclination().Value(), p2->Inclination().Value(),
                 p1->Easting().Value(), p1->Northing().Value(), p1->TVD().Value(), dum_eas, dum_nor, dum_tvd, dum_azi,
                 dum_inc, dls);
  return dls;
}

QString CWellPathBase::GetCustomerName() const { return m_CustName; }

QString CWellPathBase::GetJobNumber() const { return m_JobNumber; }

QString CWellPathBase::GetCustomerContact() const { return m_CustContact; }

QString CWellPathBase::GetDescription() const { return m_Description; }

QString CWellPathBase::GetWellType() const { return m_WellType; }

QString CWellPathBase::GetLocationName() const { return m_LocationName; }

QString CWellPathBase::GetCountry() const { return m_Country; }

const QDate &CWellPathBase::GetSpudDate() const { return m_SpudDate; }

const QDate &CWellPathBase::GetBirthDate() const { return m_BirthDate; }

void CWellPathBase::SetSpudDate(const QDate &date) { m_SpudDate = date; }

void CWellPathBase::SetBirthDate(const QDate &date) { m_BirthDate = date; }

void CWellPathBase::SetLocationName(QString str) { m_LocationName = str; }

void CWellPathBase::SetCountry(QString str) { m_Country = str; }

void CWellPathBase::SetWellType(QString str) { m_WellType = str; }

void CWellPathBase::SetDescription(QString str) { m_Description = str; }

QString CWellPathBase::GetFieldName() const { return m_FieldName; }

void CWellPathBase::SetFieldName(QString str) { m_FieldName = str; }

void CWellPathBase::SetCustomerName(QString str) { m_CustName = str; }

void CWellPathBase::SetCustomerContact(QString str) { m_CustContact = str; }

void CWellPathBase::SetJobNumber(QString str) { m_JobNumber = str; }

CLengthQuantity &CWellPathBase::MeanGroundOrSeabedLevel() { return m_MeanGroundOrSeabedLevel; }

CLengthQuantity &CWellPathBase::MeanSeaLevel() { return m_MeanSeaLevel; }

CWellPathBase::LOCATION CWellPathBase::GetLocation() const { return m_Location; }

void CWellPathBase::SetLocation(LOCATION location) {
  m_Location = location;
  onDisplayChanged(*this);
}

void CWellPathBase::ChangeGlobalPos(const double &northing, const double &easting, const double &tvd) {
  m_GlobalNorthing.SetValue(northing);
  m_GlobalEasting.SetValue(easting);
  m_GlobalTVD.SetValue(tvd);
}

double CWellPathBase::TMD2TVD(double tmd) const {
  assert(DefPointList().NrOfPoints() >= 2);
  assert(DefPointList().AllPointsDefined());

  CWellPoint ret(*This(), tmd);
  return ret.TVD().Value();
}

double CWellPathBase::TVD2TMD(double tvd) const {

  const CWellPoint *pFirst = DefPointList().First();
  const CWellPoint *pLast = DefPointList().Last();

  if (tvd - pFirst->TVD().Value() < 0.0) { // tvd above all definition points

    double delta_tmd = GetDeltaTMD(pFirst->Azimuth().Value(), pFirst->Azimuth().Value(), pFirst->Inclination().Value(),
                                   pFirst->Inclination().Value(), pFirst->TMD().Value(), pFirst->TMD().Value(),
                                   pFirst->TVD().Value(), pFirst->TVD().Value(), tvd);

    return (pFirst->TMD().Value() + delta_tmd);

  } else if (tvd - pLast->TVD().Value() > 0.0) { // tvd below all definition points

    double delta_tmd = GetDeltaTMD(pLast->Azimuth().Value(), pLast->Azimuth().Value(), pLast->Inclination().Value(),
                                   pLast->Inclination().Value(), pLast->TMD().Value(), pLast->TMD().Value(),
                                   pLast->TVD().Value(), pLast->TVD().Value(), tvd);

    return (pLast->TMD().Value() + delta_tmd);
  } else {
    bool bFound = false;
    CWellPointList::Iterator it = DefPointList().begin();
    CWellPoint *p1 = 0;
    CWellPoint *p2 = 0;

    if (it != DefPointList().end())
      p1 = *it;
    ++it;
    if (it != DefPointList().end())
      p2 = *it;

    while (p1 && p2) {
      if (p1->TVD().Value() <= tvd && p2->TVD().Value() >= tvd) {
        bFound = true;
        break;
      }
      p1 = p2;
      ++it;
      if (it != DefPointList().end())
        p2 = *it;
      else
        p2 = 0;
    }
    assert(bFound);

    double delta_tmd =
        GetDeltaTMD(p1->Azimuth().Value(), p2->Azimuth().Value(), p1->Inclination().Value(), p2->Inclination().Value(),
                    p1->TMD().Value(), p2->TMD().Value(), p1->TVD().Value(), p2->TVD().Value(), tvd);

    return (p1->TMD().Value() + delta_tmd);
  }
}

double CWellPathBase::DeltaTVD() const {
  assert(Defined());
  return DefPointList().Last()->TVD().Value() - DefPointList().First()->TVD().Value();
}

double CWellPathBase::DeltaTMD() const {
  assert(Defined());
  return DefPointList().Last()->TMD().Value() - DefPointList().First()->TMD().Value();
}

const CLengthQuantity &CWellPathBase::GlobalNorthing() const { return m_GlobalNorthing; }

const CLengthQuantity &CWellPathBase::GlobalEasting() const { return m_GlobalEasting; }

const CLengthQuantity &CWellPathBase::GlobalTVD() const { return m_GlobalTVD; }
CLengthQuantity &MeanGroundOrSeabedLevel(); // always
CLengthQuantity &MeanSeaLevel();            // PLATFORM and SUBSEA only

double CWellPathBase::HeightOfWaterColumn() const {
  assert(m_Location != ONSHORE);
  assert(!m_MeanGroundOrSeabedLevel.Undefined());
  assert(!m_MeanSeaLevel.Undefined());
  return m_MeanGroundOrSeabedLevel.Value() - m_MeanSeaLevel.Value();
}

void CWellPathBase::GetBetweenPosition(const CWellPoint &p1, const CWellPoint &p2, const double &tmd, double &eas,
                                       double &nor, double &tvd, double &azi, double &inc)

{
  double dum_dls;
  GetNewPosition(p1.Azimuth().Value(), p2.Azimuth().Value(), p1.TMD().Value(), p2.TMD().Value(), tmd,
                 p1.Inclination().Value(), p2.Inclination().Value(), p1.Easting().Value(), p1.Northing().Value(),
                 p1.TVD().Value(), eas, nor, tvd, azi, inc, dum_dls);
}

} // end namespace well

static double GetDeltaTMD(const double &a1, const double &a2, const double &i1, const double &i2, const double &tmd1,
                          const double &tmd2, const double &tvd1, const double &tvd2, const double &tvd) {

  double cos_i1 = cos(DEG2RAD(i1));
  double cos_i2 = cos(DEG2RAD(i2));
  double sin_i1 = sin(DEG2RAD(i1));
  double sin_i2 = sin(DEG2RAD(i2));

  double cos_a1 = cos(DEG2RAD(a1));
  double cos_a2 = cos(DEG2RAD(a2));
  double sin_a1 = sin(DEG2RAD(a1));
  double sin_a2 = sin(DEG2RAD(a2));

  double cos_y = sin_i1 * sin_i2 * (cos_a1 * cos_a2 + sin_a1 * sin_a2) + cos_i1 * cos_i2; // page 18 {B3}
  double y = acos(cos_y);

  // straight line
  if (fabs(y) < EPS) {
    geo::CVector u1 = well::CWellPathBase::GetDirection(a1, i1);
    geo::CVector u2 = well::CWellPathBase::GetDirection(a2, i2);
    geo::CVector utot = u1 + u2;
    utot = utot.UnitVector();

    if (utot.Z() == 0) {
      assert(false);
      return (tvd - tvd1);
    }

    return (tvd - tvd1) / utot.Z();
  } else {

    double tmd;
    InterpolateTmd_newton(tmd1, tmd2, tvd1, tvd2, a1, a2, i1, i2, tvd, tmd);
    return tmd - tmd1;

    //		double R = (tmd2 - tmd1)/y;
    //		double sin_y2 = sin(0.5*y);
    //		double cos_y2 = cos(0.5*y);
    //
    //
    //		geo::CVector u1 =well::CWellPathBase::GetDirection(a1,i1);
    //		geo::CVector u2 =well::CWellPathBase::GetDirection(a2,i2);
    //
    //		geo::CVector u1_plus_u2  =  u1 + u2;
    //		geo::CVector u2_minus_u1 =  u2 - u1;
    //
    //		u1_plus_u2=u1_plus_u2.UnitVector();
    //		u2_minus_u1=u2_minus_u1.UnitVector();
    //
    //		//double cr = 0.5/cos_y2;
    //		geo::CVector r = u1_plus_u2 / (2.0*cos_y2);
    //		r = r.UnitVector();
    //
    //		//double cs = 0.5/sin_y2;
    //		geo::CVector s = u2_minus_u1/(2.0*sin_y2);
    //		s = s.UnitVector();
    //
    //		double C = tvd - tvd1 - r.Z()*R*sin_y2 - s.Z()*R*cos_y2;
    //		double A = -s.Z()*R;
    //		double B = r.Z()*R;
    //
    //		double sq = sqrt(A*A + B*B);
    //
    //		double f = atan(B/A);
    //		double f2 = acos(A/sq);
    //
    //		if(f2 < f)
    //			f=f2;
    //
    //
    //		double beta = acos(C/sq)+f;
    //		double tmd = (beta/y+0.5)*(tmd2-tmd1);
    //		return tmd;
    //
  }
}

static bool InterpolateTmd_newton(const double &tmd1, const double &tmd2, const double &tvd1, const double & /*tvd2*/,
                                  const double &azi1, const double &azi2, const double &inc1, const double &inc2,
                                  const double &tvd, double &tmd) {
  const int MAXIT = 30;
  const double xacc = 0.000001;

  double df, f, dx;
  double x = tmd1;

  double nor1 = 0, nor2 = 0;
  double eas1 = 0, eas2 = 0;
  double dum_azi, dum_inc, dum_dls;

  double delta = 0.001;

  for (int i = 0; i < MAXIT; i++) {

    CCommonWellPath::GetNewPosition(azi1, azi2, tmd1, tmd2, x, inc1, inc2, eas1, nor1, tvd1, eas2, nor2, f, dum_azi,
                                    dum_inc, dum_dls);
    CCommonWellPath::GetNewPosition(azi1, azi2, tmd1, tmd2, x + delta, inc1, inc2, eas1, nor1, tvd1, eas2, nor2, df,
                                    dum_azi, dum_inc, dum_dls);

    f = f - tvd;
    df = df - tvd;

    if (fabs(f) < xacc) {
      tmd = x;
      return true;
    }

    df = df - f;
    dx = f * delta / df;
    x = x - dx;
  }

  assert(false);
  // maximum nr of iteration exceeded
  return false;
}

static bool InterpolateInclination_newton(const double &tmd1, const double &tmd2, const double &tvd1,
                                          const double &tvd2, const double &azi1, const double &azi2,
                                          const double &inc1, double &inc2) {
  const int MAXIT = 30;
  const double xacc = 0.000001;

  double df, rtn, f, dx;
  double x1 = inc1;
  double x2 = inc2;

  // start Value
  rtn = 0.5 * (x1 + x2);

  double delta = 0.001;

  for (int i = 0; i < MAXIT; i++) {

    double nor1 = 0, nor2 = 0;
    double eas1 = 0, eas2 = 0;
    double dum_azi, dum_inc, dum_dls;

    CCommonWellPath::GetNewPosition(azi1, azi2, tmd1, tmd2, tmd2, inc1, rtn, eas1, nor1, tvd1, eas2, nor2, f, dum_azi,
                                    dum_inc, dum_dls);
    CCommonWellPath::GetNewPosition(azi1, azi2, tmd1, tmd2, tmd2, inc1, rtn + delta, eas1, nor1, tvd1, eas2, nor2, df,
                                    dum_azi, dum_inc, dum_dls);

    f = f - tvd2;
    df = df - tvd2;

    if (fabs(f) < xacc) {
      inc2 = rtn;
      return true;
    }

    df = df - f;
    dx = f * delta / df;
    rtn = rtn - dx;

    rtn = rtn - int(rtn / 180) * 180.0;
  }

  assert(false);
  // maximum nr of iteration exceeded
  return false;
}

/*
static bool InterpolateInclination_secant(const double& tmd1,
                const double& tmd2,
                const double& tvd1,
                const double& tvd2,
                const double& azi1,
                const double& azi2,
                const double& inc1,
                double& inc2)
{


  const int MAXIT = 30;
  const double xacc = 0.0001;

  double xl,xh;

  double x1=inc1;
  double x2=inc2;

  double fl=tvd1;
  double fh=tvd2;

  if(fl < tvd2)
  {
    xl = x1;
    xh = x2;
  }
  else
  {
    xl = x2;
    xh = x1;
    std::swap(fl,fh);
  }

  double dx = xh - xl;
  double rtf,f,del;

  for(int i=0 ; i<MAXIT ; i++)
  {
    rtf = xl + dx*fl/(fl-fh); //increment with respect to last Value;

    double nor1=0,nor2=0;
    double eas1=0,eas2=0;
    double dum_azi,dum_inc,dum_dls;
    GetNewPosition(azi1,azi2,tmd1,tmd2,tmd2,inc1,rtf,eas1,nor1,tvd1,eas2,nor2,f,dum_azi,dum_inc,dum_dls);

    if(f < tvd2)
    {
      del = xl - rtf;
      xl=rtf;
      fl=f;
    }
    else
    {
      del = xh - rtf;
      xh=rtf;
      fh=f;
    }
    dx = xh - xl;
    if(fabs(del) < xacc || fabs(f-tvd2) < xacc)
    {
      inc2=rtf;
      return true;
    }
  }

  return false;


}
*/
