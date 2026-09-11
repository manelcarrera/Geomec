#include "stdafx.h"

#include "CamClayUpscaling.h"
#include "CamClayUpscaling_Delegate.h"
#include "Global.h"
#include "IProgressFactory.h"

CCamClayUpscalingNode_Delegate::CCamClayUpscalingNode_Delegate(CCamClayUpscalingNode *camClayUpscalingNode)
    : IUpscalingNodeBase_Delegate(camClayUpscalingNode), m_camClayUpscalingNode(camClayUpscalingNode) {}

bool CCamClayUpscalingNode_Delegate::CanExecute() const { return m_camClayUpscalingNode->CanExecute(); }

void CCamClayUpscalingNode_Delegate::Execute() {
  std::auto_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Computing upscaled parameters"));

  m_camClayUpscalingNode->Execute(*prog);
}
