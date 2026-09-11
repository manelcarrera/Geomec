#ifndef _LasPointSet_Delegate_h_
#define _LasPointSet_Delegate_h_

class CLasPointSet;

#include "PointSet_Delegate.h"

class CLasPointSet_Delegate : public CPointSet_Delegate
{
public:
  CLasPointSet_Delegate(CLasPointSet* lasPointSet);

private:
  CLasPointSet_Delegate(const CLasPointSet_Delegate& rhs);
  CLasPointSet_Delegate& operator = (const CLasPointSet_Delegate& rhs);

  CLasPointSet* m_lasPointSet;

  REGISTER_DELEGATE(CLasPointSet, CLasPointSet_Delegate);
};

#endif  // _LasPointSet_Delegate_h_
