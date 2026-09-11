#ifndef _CommonWellPath_h_
#define _CommonWellPath_h_

namespace geo
{

class IPoint;

} // namespace geo

#include "IModelObject.h"
#include "Vector.h"

class CCommonWellPath : public IModelObject
{
  public:
    CCommonWellPath(const QString& text);

    QString GetHoleName() const;
    void SetHoleName(const QString& holeName);

    //create a vector from angles
    static geo::CVector	GetDirection(const double& azimuth,
      const double& inclination);

    static void GetNewPosition(const double& azi1, const double& azi2,
      const double& tmd1, const double& tmd2, const double& tmd,
      const double& inc1, const double& inc2, const double& eas1,
      const double& nor1, const double& tvd1, double& eas, double& nor,
      double& tvd, double& azi, double& inc, double& dls);

    static void PartialInitFromArray_tmd_azi_inc(double start_northing, double start_easting, double start_depth,	QVector<double>& tmd, QVector<double>& azi, QVector<double>& inc, QVector<geo::CPoint>& points, QVector<double>& output_azi, QVector<double>& output_inc);

  protected:
    QString m_HoleName;

  private:
    CCommonWellPath(const CCommonWellPath& rhs);
    CCommonWellPath& operator = (const CCommonWellPath& rhs);
};

#endif  // _CommonWellPath_h_
