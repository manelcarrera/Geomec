#ifndef _IPointSet_Delegate_h_
#define _IPointSet_Delegate_h_

class IPointSet;

#include "ColorNode_Delegate.h"

class IPointSet_Delegate : public CColorNode_Delegate
{
public:
  IPointSet_Delegate(IPointSet* pointSet);

  virtual void Export();

private:
  IPointSet_Delegate(const IPointSet_Delegate& rhs);
  IPointSet_Delegate& operator = (const IPointSet_Delegate& rhs);

  IPointSet* m_pointSet;

  REGISTER_DELEGATE(IPointSet, IPointSet_Delegate);
};

#endif  // _IPointSet_Delegate_h_
