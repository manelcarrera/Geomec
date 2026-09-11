#ifndef _ThinLayerUpscaling_Delegate_h_
#define _ThinLayerUpscaling_Delegate_h_

class CThinLayerUpscalingNode;

#include "Upscaling_Delegate.h"

class CThinLayerUpscalingNode_Delegate : public IUpscalingNodeBase_Delegate
{
public:
  CThinLayerUpscalingNode_Delegate(
  CThinLayerUpscalingNode* thinLayerUpscalingNode);

  virtual bool CanExecute() const;
  virtual void Execute();

private:
  CThinLayerUpscalingNode_Delegate(const CThinLayerUpscalingNode_Delegate& rhs);
  CThinLayerUpscalingNode_Delegate& operator = (
  const CThinLayerUpscalingNode_Delegate& rhs);

  CThinLayerUpscalingNode* m_thinLayerUpscalingNode;

  REGISTER_DELEGATE(CThinLayerUpscalingNode, CThinLayerUpscalingNode_Delegate);
};

#endif  // _ThinLayerUpscaling_Delegate_h_
