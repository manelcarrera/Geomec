#ifndef _IScalingBox_Delegate_h_
#define _IScalingBox_Delegate_h_

class IScalingBox;

#include "ColorNode_Delegate.h"

class IScalingBox_Delegate : public CColorNode_Delegate {
public:
  IScalingBox_Delegate(IScalingBox *scalingBox);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  IScalingBox_Delegate(const IScalingBox_Delegate &rhs);
  IScalingBox_Delegate &operator=(const IScalingBox_Delegate &rhs);

  IScalingBox *m_scalingBox;

  REGISTER_DELEGATE(IScalingBox, IScalingBox_Delegate);
};

#endif // _IScalingBox_Delegate_h_
