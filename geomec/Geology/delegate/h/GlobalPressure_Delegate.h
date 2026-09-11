#ifndef _GlobalPressure_Delegate_h_
#define _GlobalPressure_Delegate_h_

class TPressure;

#include "GlobalFormationLoad_Delegate.h"
#include "ComponentConstant.h"

class CGlobalPressure_Delegate :
  public CGlobalFormationLoad_Delegate <TPressure,
    CComponentConstant  <TPressure> >
{
public:
  CGlobalPressure_Delegate(CGlobalPressure* globalPressure);

  virtual bool Attributes();

private:
  CGlobalPressure_Delegate(const CGlobalPressure_Delegate& rhs);
  CGlobalPressure_Delegate& operator = (const CGlobalPressure_Delegate& rhs);

  CGlobalPressure* m_globalPressure;

  REGISTER_DELEGATE(CGlobalPressure, CGlobalPressure_Delegate);
};

#endif  // _GlobalPressure_Delegate_h_
