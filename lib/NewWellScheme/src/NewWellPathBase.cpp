
#include <QVector>

#include "NewWellDefinitionPointList.h"
#include "NewWellPathBase.h"

CNewWellPathBase::CNewWellPathBase(const QString &text)
    : CCommonWellPath(text), m_pDefPointList(new CNewWellDefinitionPointList(*this)), m_GlobalNorthing(0),
      m_GlobalEasting(0), m_GlobalTVD(0) {
  ;
}

CNewWellPathBase::CNewWellPathBase()
    : CCommonWellPath("CNewWellPathBase"), m_pDefPointList(new CNewWellDefinitionPointList(*this)), m_GlobalNorthing(0),
      m_GlobalEasting(0), m_GlobalTVD(0) {
  ;
}

CNewWellPathBase::~CNewWellPathBase() { ; }

const CNewWellDefinitionPointList &CNewWellPathBase::DefPointList() const { return *m_pDefPointList; }

CNewWellDefinitionPointList &CNewWellPathBase::DefPointList() { return *m_pDefPointList; }

double CNewWellPathBase::TMD2TVD(double tmd) const {
  CNewWellPoint ret(*this, tmd);
  return ret.TVD();
}

void CNewWellPathBase::GetBetweenPosition(const CNewWellPoint &p1, const CNewWellPoint &p2, const double &tmd,
                                          double &eas, double &nor, double &tvd, double &azi, double &inc)

{
  CAngleQuantity azimuth1, inclination1, azimuth2, inclination2;
  double dum_dls;

  p1.AzimuthInclination(azimuth1, inclination1);
  p2.AzimuthInclination(azimuth2, inclination2);

  GetNewPosition(azimuth1.Value(), azimuth2.Value(), p1.TMD(), p2.TMD(), tmd, inclination1.Value(),
                 inclination2.Value(), p1.Easting(), p1.Northing(), p1.TVD(), eas, nor, tvd, azi, inc, dum_dls);
}

bool CNewWellPathBase::InitFromArray_tmd_azi_inc(double global_northing, double global_easting, double global_depth,
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
  CNewWellPoint *Point1 = new CNewWellPoint(*this, nor1, eas1, tvd1, tmd[0]);
  m_pDefPointList->AddPoint(Point1);

  Point1->AzimuthInclination(azi[0], inc[0]);

  for (int i = 0; i < tmd.count() - 1; i++) {
    double tvd2;
    double nor2;
    double eas2;

    double dum_azi, dum_inc, dum_dls;
    GetNewPosition(azi[i], azi[i + 1], tmd[i], tmd[i + 1], tmd[i + 1], inc[i], inc[i + 1], eas1, nor1, tvd1, eas2, nor2,
                   tvd2, dum_inc, dum_azi, dum_dls);

    CNewWellPoint *Point2 = new CNewWellPoint(*this, nor2, eas2, tvd2, tmd[i + 1]);
    m_pDefPointList->AddPoint(Point2);

    //		Point->m_Azimuth.SetValue(dum_azi);
    //		Point->m_Inclination.SetValue(dum_inc);

    Point2->AzimuthInclination(azi[i + 1], inc[i + 1]);

    tvd1 = tvd2;
    nor1 = nor2;
    eas1 = eas2;
  }

  blockSignals(false);

  geometryChanged();
  return true;
}
