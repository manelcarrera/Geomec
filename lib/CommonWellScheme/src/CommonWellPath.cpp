#include "CommonWellPath.h"
#include "Point.h"
#include "Vector.h"
#include <QVector>

CCommonWellPath::CCommonWellPath(const QString& text)
: IModelObject(text)
{
}

QString CCommonWellPath::GetHoleName() const
{
  return m_HoleName;
}

void CCommonWellPath::SetHoleName(const QString& holeName)
{
  m_HoleName = holeName;
  onDisplayChanged(*this);
}

// static

geo::CVector CCommonWellPath::GetDirection(const double& azimuth,
  const double& inclination)
{
  double sin_i = sin(DEG2RAD(inclination));
  double cos_i = cos(DEG2RAD(inclination));
  double cos_a = cos(DEG2RAD(azimuth));
  double sin_a = sin(DEG2RAD(azimuth));

  return geo::CVector(cos_a * sin_i, sin_a * sin_i, cos_i);
}

// interpolation on survey interval
void CCommonWellPath::GetNewPosition(const double& a1, const double& a2,
  const double& tmd1, const double& tmd2, const double& tmd, const double& i1,
  const double& i2, const double& eas1, const double& nor1, const double& tvd1,
  double& eas, double& nor, double& tvd, double& azi, double& inc, double& dls)
{
  double cos_i1 = cos(DEG2RAD(i1));
  double cos_i2 = cos(DEG2RAD(i2));
  double sin_i1 = sin(DEG2RAD(i1));
  double sin_i2 = sin(DEG2RAD(i2));

  double cos_a1 = cos(DEG2RAD(a1));
  double cos_a2 = cos(DEG2RAD(a2));
  double sin_a1 = sin(DEG2RAD(a1));
  double sin_a2 = sin(DEG2RAD(a2));

  // page 18 {B3}
  double cos_y = sin_i1*sin_i2*(cos_a1*cos_a2 + sin_a1*sin_a2) + cos_i1*cos_i2;

  if(cos_y > 1)
  cos_y = 1;

  if(cos_y < -1)
  cos_y = -1;

  double y = acos(cos_y);

  //straight line
  if(fabs(y) < EPS)
  {
  geo::CVector u1 = GetDirection(a1,i1);
  geo::CVector u2 = GetDirection(a2,i2);
  geo::CVector utot=u1+u2;

  utot = utot.UnitVector();
  eas = eas1 + utot.Y()*(tmd-tmd1);
  nor = nor1 + utot.X()*(tmd-tmd1);
  tvd = tvd1 + utot.Z()*(tmd-tmd1);
  azi=a1;
  inc=i1;
  dls = 0.0;
  }
  else
  {
  dls = y / (tmd2 - tmd1);

  double beta = y*((tmd-tmd1)/(tmd2-tmd1) -0.5); //page 20 {B18}
  double sin_y2 = sin(0.5*y);
  double cos_y2 = cos(0.5*y);	
  double sin_b =  sin(beta);	
  double cos_b =  cos(beta);	

  geo::CVector u1 = GetDirection(a1,i1);
  geo::CVector u2 = GetDirection(a2,i2);

  geo::CVector u1_plus_u2  =  u1 + u2;
  geo::CVector u2_minus_u1 =  u2 - u1;

  double c1 = 0.5*(sin_y2+sin_b)/cos_y2;
  double c2 = 0.5*(cos_y2-cos_b)/sin_y2;
  double c3 = (tmd2-tmd1)/y;

  eas   = eas1 + c3*(u1_plus_u2.Y()*c1 + u2_minus_u1.Y()*c2);
  nor   = nor1 + c3*(u1_plus_u2.X()*c1 + u2_minus_u1.X()*c2);
  tvd   = tvd1 + c3*(u1_plus_u2.Z()*c1 + u2_minus_u1.Z()*c2);

  double const1 = 0.5*cos_b/cos_y2;
  double const2 = 0.5*sin_b/sin_y2;

  //page 20 {B21}
  double dir_east  = const1*u1_plus_u2.Y() + const2*u2_minus_u1.Y();
  double dir_north = const1*u1_plus_u2.X() + const2*u2_minus_u1.X();
  double dir_tvd   = const1*u1_plus_u2.Z() + const2*u2_minus_u1.Z();

  if(dir_tvd > 1)
      dir_tvd = 1;

  if(dir_tvd < -1)
      dir_tvd = -1;

  inc = acos(dir_tvd) * RAD_TO_DEG;
  azi=0;

  // 0 to 360 degrees
  if(fabs(inc) > EPS)
  {
      azi = (atan2(-dir_east , -dir_north )+PI) * RAD_TO_DEG;
  }
  }
}


void CCommonWellPath::PartialInitFromArray_tmd_azi_inc(double start_northing, double start_easting, double start_depth, QVector<double>& tmd, QVector<double>& azi, QVector<double>& inc, QVector<geo::CPoint>& points, QVector<double>& output_azi, QVector<double>& output_inc)
{
  double nor1   = start_northing;
  double eas1   = start_easting;
  double tvd1   = start_depth;

  points.reserve(tmd.size());
  output_azi.reserve(tmd.size());
  output_inc.reserve(tmd.size());

  points.push_back(geo::CPoint(nor1, eas1, tvd1));
  output_azi.push_back(azi[0]);
  output_inc.push_back(inc[0]);

  for (int i = 0; i < tmd.count() - 1; i++)
  {
  double tvd2;
  double nor2;
  double eas2;

  double dum_azi, dum_inc, dum_dls;
  GetNewPosition(azi[i],
      azi[i + 1],
      tmd[i],
      tmd[i + 1],
      tmd[i + 1],
      inc[i],
      inc[i + 1],
      eas1,
      nor1,
      tvd1,
      eas2,
      nor2,
      tvd2,
      dum_inc,
      dum_azi,
      dum_dls);

  bool add = i == 0; // always add the 2nd point

  if (!add)
  {
      double deltaX = (nor2 + points[i - 1].X()) / 2;
      double deltaY = (eas2 + points[i - 1].Y()) / 2;
      double deltaZ = (tvd2 + points[i - 1].Z()) / 2;

      const double eps = 0.1;

      add = fabs(deltaX - nor1) > eps || fabs(deltaY - eas1) > eps || fabs(deltaZ - tvd1) > eps;
  }

  if (add)
  {
      points.push_back(geo::CPoint(nor2, eas2, tvd2));
      output_azi.push_back(dum_azi);
      output_inc.push_back(dum_inc);
  }
  else
  {
      points.back().Set(nor2, eas2, tvd2);
  }

    tvd1=tvd2;
    nor1=nor2;
    eas1=eas2;
  }

}