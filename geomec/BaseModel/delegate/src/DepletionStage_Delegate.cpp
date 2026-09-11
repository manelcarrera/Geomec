#include "stdafx.h"

#include "DepletionStage_Delegate.h"
#include "DepletionStage.h"

CDepletionStage_Delegate::CDepletionStage_Delegate(
  CDepletionStage* depletionStage)
: CStorageNode_Delegate(depletionStage)
, m_depletionStage(depletionStage)
{
}

CDepletionStageEntry_Delegate::CDepletionStageEntry_Delegate(
  CDepletionStageEntry* depletionStageEntry)
: CGraphEntryTemp_Delegate <CDepletionStage> (depletionStageEntry)
, m_depletionStageEntry(depletionStageEntry)
{
}
