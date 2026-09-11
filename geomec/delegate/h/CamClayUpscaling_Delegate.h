#ifndef _CamClayUpscaling_Delegate_h_
#define _CamClayUpscaling_Delegate_h_

class CCamClayUpscalingNode;

#include "Upscaling_Delegate.h"

class CCamClayUpscalingNode_Delegate : public IUpscalingNodeBase_Delegate {
public:
  CCamClayUpscalingNode_Delegate(CCamClayUpscalingNode *camClayUpscalingNode);

  virtual bool CanExecute() const;
  virtual void Execute();

private:
  CCamClayUpscalingNode_Delegate(const CCamClayUpscalingNode_Delegate &rhs);
  CCamClayUpscalingNode_Delegate &operator=(const CCamClayUpscalingNode_Delegate &rhs);

  CCamClayUpscalingNode *m_camClayUpscalingNode;

  REGISTER_DELEGATE(CCamClayUpscalingNode, CCamClayUpscalingNode_Delegate);
};

#endif // _CamClayUpscaling_Delegate_h_
