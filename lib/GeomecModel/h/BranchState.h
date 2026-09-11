#ifndef _BRANCHSTATE_H_
#define _BRANCHSTATE_H_

class CModelBase;
class CDepletionStage;


#include "StorageNode.h"

class CBranchState
{
public:
  // scope based override
  class COverride
  {
  public:
    COverride(CBranchState& state, const CDepletionStage& stage);
    ~COverride();

  private:
    CBranchState& m_state;
    const CDepletionStage* m_pOldStage;
  };

  friend class COverride;

  CBranchState(CModelBase& model);

  void SetBranchStage(const CDepletionStage& stage);
  void ClearBranch();

  bool IsBranch() const;
  const CDepletionStage& BranchStage() const;

  // the active stage is the stage after the branch stage
  //  or the initial stage if there is no branch
  const CDepletionStage& ActiveStage() const;

  bool IsFixedStage(const CDepletionStage& stage) const;

  const CModelBase& Model() const;

  bool Save(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg);
  bool Load(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prg);

private:
  CModelBase& m_model;
  const CDepletionStage* m_pBranchStage;
};

#endif // _BRANCHSTATE_H_
