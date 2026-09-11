#include "stdafx.h"

#include "DepletionStage.h"
#include "DepletionStage_Delegate.h"

CDepletionStage_Delegate::CDepletionStage_Delegate(CDepletionStage *depletionStage)
    : CStorageNode_Delegate(depletionStage), m_depletionStage(depletionStage) {}

CDepletionStageEntry_Delegate::CDepletionStageEntry_Delegate(CDepletionStageEntry *depletionStageEntry)
    : CGraphEntryTemp_Delegate<CDepletionStage>(depletionStageEntry), m_depletionStageEntry(depletionStageEntry) {}
