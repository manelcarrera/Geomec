#ifndef _DepletionStage_Delegate_h_
#define _DepletionStage_Delegate_h_

class CDepletionStage;
class CDepletionStageEntry;

#include "GraphEntry_Delegate.h"
#include "StorageNode_Delegate.h"

class CDepletionStage_Delegate : public CStorageNode_Delegate
{
public:
  CDepletionStage_Delegate(CDepletionStage* depletionStage);

private:
  CDepletionStage_Delegate(const CDepletionStage_Delegate& rhs);
  CDepletionStage_Delegate& operator = (const CDepletionStage_Delegate& rhs);

  CDepletionStage* m_depletionStage;

  REGISTER_DELEGATE(CDepletionStage, CDepletionStage_Delegate);
};

class CDepletionStageEntry_Delegate :
  public CGraphEntryTemp_Delegate <CDepletionStage>
{
public:
  CDepletionStageEntry_Delegate(CDepletionStageEntry* depletionStageEntry);

private:
  CDepletionStageEntry_Delegate(const CDepletionStageEntry_Delegate& rhs);
  CDepletionStageEntry_Delegate& operator = (
  const CDepletionStageEntry_Delegate& rhs);

  CDepletionStageEntry* m_depletionStageEntry;

  REGISTER_DELEGATE(CDepletionStageEntry, CDepletionStageEntry_Delegate);
};

#endif  // _DepletionStage_Delegate_h_
