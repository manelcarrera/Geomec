#ifndef _FaultFracture_h_
#define _FaultFracture_h_

class CHorizonBase;
class CFaultParametersNode;

#include "SingleQuantity.h"


#include <QString>

class CFaultFracture
{
  public:
  CFaultFracture(CHorizonBase* horizonBase, CDoubleQuantity::UNIT unit);
  CFaultFracture(const CFaultFracture& rhs);

  CFaultFracture& operator=(CFaultFracture& rhs);

  const QString& name() const;

  bool HasHorizon(const CHorizonBase *horizonBase) const;

  bool Slip() const;
  bool IsBranch() const;

  bool hasFaultParametersForInitialDepletionStage() const;

  QString CohesionUnitName() const;
  double Cohesion() const;
  void Cohesion(double value);

  QString FrictionAngleUnitName() const;
  double FrictionAngle() const;
  void FrictionAngle(double value);

  int SlipType() const;
  void SlipType(int slip);

  void FinalizeBehaviour();
  void FinalizeInitD0();

  private:

  CHorizonBase* m_horizonBase;
  int    m_slipType;

  CFaultParametersNode *m_faultParametersNode;

  QString m_cohesionUnitName;
  QString m_frictionAngleUnitName;

  double m_cohesion;
  double m_frictionAngle;

  CDoubleQuantity::UNIT m_unit;
};

#endif  // _FaultFracture_h_
