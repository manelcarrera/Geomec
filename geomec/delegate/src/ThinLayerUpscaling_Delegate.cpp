#include "stdafx.h"

#include "Global.h"
#include "IProgressFactory.h"
#include "ThinLayerUpscaling.h"
#include "ThinLayerUpscaling_Delegate.h"

CThinLayerUpscalingNode_Delegate::CThinLayerUpscalingNode_Delegate(CThinLayerUpscalingNode *thinLayerUpscalingNode)
    : IUpscalingNodeBase_Delegate(thinLayerUpscalingNode), m_thinLayerUpscalingNode(thinLayerUpscalingNode) {}

bool CThinLayerUpscalingNode_Delegate::CanExecute() const { return m_thinLayerUpscalingNode->CanExecute(); }

void CThinLayerUpscalingNode_Delegate::Execute() {
  std::auto_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Computing upscaled parameters"));

  m_thinLayerUpscalingNode->Execute(*prog);
}
