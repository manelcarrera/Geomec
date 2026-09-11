
#include <QVector>

#include "NewWellPoint.h"
#include "NewWellPointList.h"

CNewWellPointList::CNewWellPointList()
    : QObject(), m_MaxPoint(), m_MinPoint(), m_pWellPath(0), m_Description("No Description"), m_lstPoints(),
      m_bAutoDelete(true), m_bAllowDuplicateTMD(false) {}

CNewWellPointList::CNewWellPointList(const CNewWellPathBase &WellPath, bool bAutoDelete, bool bAllowDuplicateTMD)
    : QObject(), m_MaxPoint(), m_MinPoint(), m_pWellPath(&WellPath), m_Description("No Description"), m_lstPoints(),
      m_bAutoDelete(bAutoDelete), m_bAllowDuplicateTMD(bAllowDuplicateTMD) {}

CNewWellPointList::~CNewWellPointList() { clear(); }

CNewWellPointList::CNewWellPointList(const CNewWellPointList &rhs)
    : QObject(), // QObject(const QObject&) is private,
                 // hence QObject(QObject* parent = 0)
      m_MaxPoint(rhs.m_MaxPoint), m_MinPoint(rhs.m_MinPoint), m_pWellPath(rhs.m_pWellPath),
      m_Description(rhs.m_Description), m_lstPoints(rhs.m_lstPoints), m_bAutoDelete(rhs.m_bAutoDelete),
      m_bAllowDuplicateTMD(rhs.m_bAllowDuplicateTMD) {}

CNewWellPointList &CNewWellPointList::operator=(CNewWellPointList rhs) {
  swap(*this, rhs);

  return *this;
}

bool CNewWellPointList::AutoDelete() const { return m_bAutoDelete; }

void CNewWellPointList::AutoDelete(bool autodelete) { m_bAutoDelete = autodelete; }

bool CNewWellPointList::AllowDuplicateTMD() const { return m_bAllowDuplicateTMD; }

void CNewWellPointList::AllowDuplicateTMD(bool allowduplicate) { m_bAllowDuplicateTMD = allowduplicate; }

int CNewWellPointList::NrOfPoints() const { return m_lstPoints.size(); }

bool CNewWellPointList::AllPointsDefined() const {
  ConstIterator it;

  for (it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it) {
    if (!(*it)->Defined())
      return false;
  }

  return true;
}

geo::CPoint CNewWellPointList::MaxPoint() const {
  if (m_MaxPoint.Empty())
    CalculateMaxMin();

  return m_MaxPoint;
}

geo::CPoint CNewWellPointList::MinPoint() const {
  if (m_MinPoint.Empty())
    CalculateMaxMin();

  return m_MinPoint;
}

QList<CNewWellPoint *> CNewWellPointList::GetUpperLower(const double &TMD) const {
  CNewWellPoint *MinPoint = NULL;
  CNewWellPoint *MaxPoint = NULL;
  QList<CNewWellPoint *> List;
  double StartTMD = -DBL_MAX;
  double EndTMD = DBL_MAX;
  ConstIterator it = m_lstPoints.begin();
  CNewWellPoint *CurrentPoint = 0;

  // calculate the min max
  if (it != m_lstPoints.end()) {
    StartTMD = (*it)->TMD();
    CurrentPoint = *it;
  }

  while (it != m_lstPoints.end()) {
    CurrentPoint = *it;

    if (fabs(CurrentPoint->TMD() - TMD) < EPS) {
      List.insert(0, CurrentPoint);
      List.insert(1, CurrentPoint);
      return List;
    }

    // calculate the maximum point under this TMD
    if (CurrentPoint->TMD() < TMD && CurrentPoint->TMD() >= StartTMD) {
      MinPoint = CurrentPoint;
      StartTMD = CurrentPoint->TMD();
    }

    if (CurrentPoint->TMD() > TMD && CurrentPoint->TMD() <= EndTMD) {
      MaxPoint = CurrentPoint;
      EndTMD = CurrentPoint->TMD();
    }

    if (MaxPoint && MinPoint)
      break;

    ++it;
  }

  List.insert(0, MinPoint);
  List.insert(1, MaxPoint);

  return List;
}

void CNewWellPointList::clear() {
  if (m_bAutoDelete) {
    for (Iterator it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it)
      delete *it;
  }

  m_lstPoints.clear();
}

void CNewWellPointList::RemoveUnDefinedPoints() {
  QVector<CNewWellPoint *> points_to_remove;
  Iterator it = m_lstPoints.begin();
  CNewWellPoint *point;

  while (it != m_lstPoints.end()) {
    point = *it;
    if (!point->Defined()) {
      points_to_remove.resize(points_to_remove.size() + 1);
      points_to_remove.insert(points_to_remove.count(), point);
    }
    ++it;
  }

  for (int i = 0; i < points_to_remove.count(); i++) {
    RemovePoint(points_to_remove[i]);
  }
}

int CNewWellPointList::GetIndex(const CNewWellPoint *point) const {
  return m_lstPoints.indexOf(const_cast<CNewWellPoint *>(point));
}

bool CNewWellPointList::EqualPointExist(const CNewWellPoint &point) const { return (GetIndex(&point) != -1); }

bool CNewWellPointList::Exist(const double &tmd) const { return EqualPointExist(CNewWellPoint(WellPath(), tmd)); }

bool CNewWellPointList::Exist(const CNewWellPoint *point) const { return !(GetIndex(point) == -1); }

CNewWellPointList::Iterator CNewWellPointList::GetIteratorAt(const CNewWellPoint *point) const {
  for (Iterator ret = This()->m_lstPoints.begin(); ret != This()->m_lstPoints.end(); ++ret) {
    if (*ret == point)
      return ret;
  }

  return This()->m_lstPoints.end();
}

CNewWellPointList::Iterator CNewWellPointList::begin() const { return This()->m_lstPoints.begin(); }

CNewWellPointList::Iterator CNewWellPointList::end() const { return This()->m_lstPoints.end(); }

CNewWellPoint *CNewWellPointList::First() const {
  if (m_lstPoints.empty())
    return 0;

  return m_lstPoints.first();
}

CNewWellPoint *CNewWellPointList::Last() const {
  if (m_lstPoints.empty())
    return 0;

  return m_lstPoints.last();
}

CNewWellPoint *CNewWellPointList::At(int index) {
  if (index >= 0 && index < m_lstPoints.size())
    return m_lstPoints.at(index);

  return 0;
}

const CNewWellPoint *CNewWellPointList::At(int index) const {
  if (index >= 0 && index < m_lstPoints.size())
    return This()->m_lstPoints.at(index);

  return 0;
}

int CNewWellPointList::AddPoint(const CNewWellPoint *point) {
  if (m_lstPoints.indexOf(const_cast<CNewWellPoint *>(point)) != -1 && !m_bAllowDuplicateTMD) {
    return -1;
  }

  m_MaxPoint = geo::CPoint();
  m_MinPoint = geo::CPoint();

  Iterator it = m_lstPoints.end();

  for (Iterator itt = m_lstPoints.begin(); itt != m_lstPoints.end(); ++itt) {
    int comp = compareItems(*itt, const_cast<CNewWellPoint *>(point));

    if (comp == 0)
      return -1;

    if (comp > 0)
      break;
  }

  m_lstPoints.insert(it, const_cast<CNewWellPoint *>(point));

  return m_lstPoints.indexOf(const_cast<CNewWellPoint *>(point));
}

void CNewWellPointList::RemoveAllPoints() {
  m_MaxPoint = geo::CPoint();
  m_MinPoint = geo::CPoint();

  clear();
}

void CNewWellPointList::RemovePoint(int index) {
  m_MaxPoint = geo::CPoint();
  m_MinPoint = geo::CPoint();
  m_lstPoints.removeAt(index);
}

void CNewWellPointList::RemovePoint(const CNewWellPoint *point) {
  int idx = m_lstPoints.indexOf(const_cast<CNewWellPoint *>(point));

  if (idx >= 0) {
    m_MaxPoint = geo::CPoint();
    m_MinPoint = geo::CPoint();

    m_lstPoints.removeAt(idx);
  }
}

const CNewWellPathBase &CNewWellPointList::WellPath() const { return *m_pWellPath; }

void CNewWellPointList::WellPath(CNewWellPathBase *pWellPath) { m_pWellPath = pWellPath; }

void CNewWellPointList::InsertNonDuplicatePoints(const CNewWellPointList &list) {
  Iterator it = list.begin();

  while (it != list.end()) {
    CNewWellPoint *point = *it;

    if (!EqualPointExist(*point)) {
      AddPoint(const_cast<CNewWellPoint *>(point));
    }
    ++it;
  }
}

void CNewWellPointList::RemoveNonDuplicatePoints(const CNewWellPointList &list) {
  QVector<CNewWellPoint *> points_to_remove;
  Iterator it = begin();

  while (it != end()) {
    CNewWellPoint *point = *it;

    if (!list.EqualPointExist(*point)) {
      points_to_remove.resize(points_to_remove.size() + 1);
      points_to_remove.insert(points_to_remove.count(), point);
    }

    ++it;
  }

  for (int i = 0; i < points_to_remove.count(); i++) {
    RemovePoint(points_to_remove[i]);
  }
}

void CNewWellPointList::RemoveDuplicatePoints(const CNewWellPointList &list) {
  QVector<CNewWellPoint *> points_to_remove;
  Iterator it = begin();

  while (it != end()) {
    CNewWellPoint *point = *it;

    if (list.EqualPointExist(*point)) {
      points_to_remove.resize(points_to_remove.size() + 1);
      points_to_remove.insert(points_to_remove.count(), point);
    }

    ++it;
  }

  for (int i = 0; i < points_to_remove.count(); i++) {
    RemovePoint(points_to_remove[i]);
  }
}

QString CNewWellPointList::GetDescription() const { return m_Description; }

void CNewWellPointList::SetDescription(QString description) { m_Description = description; }

// protected

void CNewWellPointList::CalculateMaxMin() const {
  for (ConstIterator it = m_lstPoints.begin(); it != m_lstPoints.end(); ++it) {
    m_MaxPoint = (*it)->Max(m_MaxPoint);
    m_MinPoint = (*it)->Min(m_MinPoint);
  }
}

// private

int CNewWellPointList::compareItems(CNewWellPoint *item1, CNewWellPoint *item2) {
  CNewWellPoint *p1 = (CNewWellPoint *)(item1);
  CNewWellPoint *p2 = (CNewWellPoint *)(item2);

  if (*p1 == *p2)
    return 0;

  if (*p1 > *p2)
    return 1;

  return -1;
}

CNewWellPointList *CNewWellPointList::This() const { return const_cast<CNewWellPointList *>(this); }

void CNewWellPointList::swap(CNewWellPointList &lhs, CNewWellPointList &rhs) {
  std::swap(lhs.m_MaxPoint, rhs.m_MaxPoint);
  std::swap(lhs.m_MinPoint, rhs.m_MinPoint);
  std::swap(lhs.m_pWellPath, rhs.m_pWellPath);
  std::swap(lhs.m_Description, rhs.m_Description);
  std::swap(lhs.m_lstPoints, rhs.m_lstPoints);
  std::swap(lhs.m_bAutoDelete, rhs.m_bAutoDelete);
  std::swap(lhs.m_bAllowDuplicateTMD, rhs.m_bAllowDuplicateTMD);
}
