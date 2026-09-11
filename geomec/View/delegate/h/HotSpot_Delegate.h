#ifndef _HotSpot_Delegate_h_
#define _HotSpot_Delegate_h_

class CHotSpot;

#include "IColorScaleNode_Delegate.h"

class CHotSpot_Delegate : public IColorScaleNode_Delegate
{
public:
  CHotSpot_Delegate(CHotSpot* hotSpot);

  virtual bool Attributes();

private:
  CHotSpot_Delegate(const CHotSpot_Delegate& rhs);
  CHotSpot_Delegate& operator = (const CHotSpot_Delegate& rhs);

  CHotSpot* m_hotSpot;

  REGISTER_DELEGATE(CHotSpot, CHotSpot_Delegate);
};

#endif  // _HotSpot_Delegate_h_
