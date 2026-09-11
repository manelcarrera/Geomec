#ifndef NEWWELLPATHBASE_H
#define NEWWELLPATHBASE_H

class CNewWellDefinitionPointList;

#include "SingleQuantity.h"
#include "IModelObject.h"
#include "IPolyLine.h"
#include "NewWellPoint.h"
#include "CommonWellPath.h"
#include "LengthQuantity.h"

class CNewWellPathBase : public CCommonWellPath
{
public:
  CNewWellPathBase(const QString &text);
  CNewWellPathBase();
  virtual ~CNewWellPathBase();
  const CNewWellDefinitionPointList& DefPointList() const;
  CNewWellDefinitionPointList& DefPointList();
  double TMD2TVD(double tmd) const;
  virtual double TMD
  ( unsigned int index
  , CDoubleQuantity::UNIT unit= CDoubleQuantity::SI_UNIT
  ) const= 0;
  virtual double GlobalNorthing() const =0;
  virtual double GlobalEasting() const =0;
  virtual double GlobalTVD() const =0;
  virtual unsigned int WellPointSize() const= 0;
  virtual CNewWellPoint WellPoint(unsigned int index) const= 0;
  virtual bool InitWellPoint(CNewWellPoint &point) const= 0;
  virtual CAngleQuantity Azimuth(size_t index) const= 0;
  virtual CAngleQuantity Inclination(size_t index) const= 0;
  virtual void AzimuthInclination(double azimuth, double inclination) = 0;
  virtual size_t AngleIndex(const CNewWellPoint &pt) const= 0;
  virtual double MudGradRefDepth() const= 0;
  
  static void GetBetweenPosition( const CNewWellPoint& p1,
                  const CNewWellPoint& p2,
                  const double& tmd,
                  double& eas,
                  double& nor,
                  double& tvd,
                  double& azi,
                  double& inc);
  
  //initialization of the definition points with arrays (SI_UNIT)
  bool InitFromArray_tmd_azi_inc(
                      double global_northing,
                      double global_easting,
                      double global_depth,
                      double start_northing,
                      double start_easting,
                      double start_depth,
                      QVector<double>& tmd,
                      QVector<double>& azi,
                      QVector<double>& inc);
protected:
  CNewWellDefinitionPointList* m_pDefPointList;

  //global position
  CLengthQuantity m_GlobalNorthing;
  CLengthQuantity m_GlobalEasting;
  CLengthQuantity m_GlobalTVD;

private:
  CNewWellPathBase(const CNewWellPathBase& rhs);
  CNewWellPathBase& operator = (const CNewWellPathBase& rhs);
};
#endif // NEWWELLPATHBASE_H
