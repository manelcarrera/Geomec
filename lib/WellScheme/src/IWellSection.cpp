// IWellSection.cpp: implementation of the IWellSection class.
//
//////////////////////////////////////////////////////////////////////

#include "IWellSection.h"
#include "WellDefinitionPointList.h"
#include "WellPoint.h"
#include "WellPointList.h"
#include "WellSection.h"
#include "wellpathbase.h"

namespace well {

// ##ModelId=3F7AA1560345
IWellSection::IWellSection(const CWellPathBase &WellPath) : m_pWellPath(&WellPath) {}

// ##ModelId=3F7AA1560349
IWellSection::~IWellSection() { emit OnDestroy(*this); }

const CWellPathBase &IWellSection::WellPath() const { return *m_pWellPath; }

CWellPathBase &IWellSection::WellPath() { return *const_cast<CWellPathBase *>(m_pWellPath); }

// ##ModelId=3F7AA1560354
bool IWellSection::Defined() const { return (Top().Defined() && Bottom().Defined()); }

// ##ModelId=3F7AA1560356
bool IWellSection::operator==(const IWellSection &rhs) const {
  if (*this < rhs)
    return false;

  if (*this > rhs)
    return false;

  return true;
}

// ##ModelId=3F7AA1560359
bool IWellSection::operator!=(const IWellSection &rhs) const { return !(rhs == *this); }

// ##ModelId=3F7AA156035C
bool IWellSection::operator<(const IWellSection &rhs) const {
  if (Top() < rhs.Top()) {
    return true;
  } else if (Top() > rhs.Top()) {
    return false;
  } else // top are equal
  {

    if (Bottom() < rhs.Bottom()) {
      return true;
    } else if (Bottom() > rhs.Bottom()) {
      return false;
    } else {
      // Bottom are equal
      return false;
    }
  }
}

// ##ModelId=3F7AA156035F
bool IWellSection::operator>(const IWellSection &rhs) const {
  if (Top() > rhs.Top()) {
    return true;
  } else if (Top() < rhs.Top()) {
    return false;
  } else // top are equal
  {

    if (Bottom() > rhs.Bottom()) {
      return true;
    } else if (Bottom() < rhs.Bottom()) {
      return false;
    } else {
      // Bottom are equal
      return false;
    }
  }
}

void IWellSection::AddIntervalPoints(CWellPointList &lst, int NrOfIntervals, const double &distance,
                                     const IWellPointFactory *pFactory /*=NULL*/) const {
  well::CWellPoint *p1 = 0;
  well::CWellPoint *p2 = 0;

  if (!pFactory) {
    p1 = new well::CWellPoint(const_cast<CWellPathBase &>(WellPath()), Top().TMD().Value() + distance);
    p2 = new well::CWellPoint(const_cast<CWellPathBase &>(WellPath()), Bottom().TMD().Value() - distance);

    lst.AddPoint(p1);
    lst.AddPoint(p2);
  } else {
    p1 = pFactory->CreateWellPoint(const_cast<CWellPathBase &>(WellPath()), Top().TMD().Value() + distance);
    p2 = pFactory->CreateWellPoint(const_cast<CWellPathBase &>(WellPath()), Bottom().TMD().Value() - distance);
    if (p1)
      lst.AddPoint(p1);

    if (p2)
      lst.AddPoint(p2);
  }

  if (!p1 || !p2)
    return;

  double deltaTMD = (p2->TMD().Value() - p1->TMD().Value()) / (double)NrOfIntervals;

  int i;
  for (i = 0; i < NrOfIntervals - 1; i++) {
    double TMD = p1->TMD().Value() + double(i) * deltaTMD;
    if (!pFactory) {
      well::CWellPoint *p = new well::CWellPoint(const_cast<CWellPathBase &>(WellPath()), TMD);
      lst.AddPoint(p);
    } else {
      well::CWellPoint *p = pFactory->CreateWellPoint(const_cast<CWellPathBase &>(WellPath()), TMD);
      if (p)
        lst.AddPoint(p);
    }
  }
}

CWellPointList IWellSection::FilterContainingPoints(const CWellPointList &list, bool bIncludeEdge /*=true*/) const {
  CWellPointList ret(const_cast<CWellPathBase &>(WellPath()));
  ret.AutoDelete(false);

  CWellPointList::Iterator it = list.begin();
  while (it != list.end()) {
    CWellPoint *p = *it;
    ++it;
    if (Contains(*p, bIncludeEdge))
      ret.AddPoint(const_cast<CWellPoint *>(p));
  }

  return ret;
}

CWellPointList IWellSection::FilterNonContainingPoints(const CWellPointList &list, bool bIncludeEdge /*=true*/) const {

  CWellPointList ret(const_cast<CWellPathBase &>(WellPath()));
  ret.AutoDelete(false);

  CWellPointList::Iterator it = list.begin();
  while (it != list.end()) {
    CWellPoint *p = *it;
    ++it;
    if (!Contains(*p, bIncludeEdge))
      ret.AddPoint(const_cast<CWellPoint *>(p));
  }

  return ret;
}

bool IWellSection::Contains(const double &tmd, bool bIncludeEdge /*=true*/) const {
  return Contains(well::CWellPoint(*m_pWellPath, tmd), bIncludeEdge);
}

bool IWellSection::Contains(const CWellPoint &point, bool bIncludeEdge /*=true*/) const {
  if (point < Top())
    return false;

  if (point > Bottom())
    return false;

  if (point == Top())
    return bIncludeEdge;

  if (point == Bottom())
    return bIncludeEdge;

  return true;
}

double IWellSection::DeltaTVD() const { return (Bottom().TVD().Value() - Top().TVD().Value()); }

double IWellSection::DeltaTMD() const { return (Bottom().TMD().Value() - Top().TMD().Value()); }

void IWellSection::GetMaxMinTVD(double &maxTVD, double &minTVD) const {
  geo::CPoint p_max, p_min;
  GetBoundingBox(p_max, p_min);
  maxTVD = p_max.Z() - m_pWellPath->GlobalTVD().Value();
  minTVD = p_min.Z() - m_pWellPath->GlobalTVD().Value();
}

void IWellSection::GetBoundingBox(geo::IPoint &max, geo::IPoint &min) const {

  const CWellPoint *p1 = &Top();
  const CWellPoint *p2 = &Bottom();

  max = p2->Max(*p1);
  min = p2->Min(*p1);

  if ((p1->Inclination().Value() < 90.0 && p2->Inclination().Value() > 90.0) ||
      (p1->Inclination().Value() > 90.0 && p2->Inclination().Value() < 90.0)) {
    int nr_interval = 10;
    double delta = DeltaTMD() / double(nr_interval);
    for (int i = 0; i < nr_interval; i++) {
      double tmd = Top().TMD().Value() + delta * double(i);
      CWellPoint p(*m_pWellPath, tmd);
      max = max.Max(p);
      min = min.Min(p);
    }
  }
}

} // end namespace well
