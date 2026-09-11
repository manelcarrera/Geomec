#include "BranchState.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "StreamVersion.h"

//// CBranchState::COverride

CBranchState::COverride::COverride(CBranchState &state, const CDepletionStage &stage) : m_state(state), m_pOldStage(0) {
  if (state.IsBranch())
    m_pOldStage = &state.BranchStage();

  state.m_pBranchStage = &stage;
}

CBranchState::COverride::~COverride() { m_state.m_pBranchStage = m_pOldStage; }

//// CBranchState

CBranchState::CBranchState(CModelBase &model) : m_model(model), m_pBranchStage(0) {}

void CBranchState::SetBranchStage(const CDepletionStage &stage) { m_pBranchStage = &stage; }

void CBranchState::ClearBranch() {
  m_pBranchStage = 0;

  // and clear the results
  m_model.ResultRegister().ClearNonLinear();
}

bool CBranchState::IsBranch() const { return (m_pBranchStage != 0); }

const CDepletionStage &CBranchState::BranchStage() const {
  assert(IsBranch());
  return *m_pBranchStage;
}

const CDepletionStage &CBranchState::ActiveStage() const {
  if (IsBranch()) {
    assert(!BranchStage().Last());
    return BranchStage().Next();
  }

  return m_model.InitialDepletionStage();
}

bool CBranchState::IsFixedStage(const CDepletionStage &stage) const {
  if (!IsBranch())
    return false;

  // the branch stage itself is fixed
  return !stage.isAfter(*m_pBranchStage);
}

const CModelBase &CBranchState::Model() const { return m_model; }

bool CBranchState::Save(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*prg*/) {
  // store whether it's a branch
  int iBranch = (int)IsBranch();
  stream << iBranch;

  if (IsBranch()) {
    // store branch stage index

    // get the index
    const CDepletionStage *pStage = &m_model.DepletionStageEntry().InitialStage();
    int idx = 0;

    while (pStage) {
      if (pStage == m_pBranchStage || pStage->Last()) // Last() should not happen, but prevent infinite loop anyway
        pStage = 0;
      else {
        pStage = &pStage->Next();
        ++idx;
      }
    }

    // store the index
    stream << idx;
  }

  return true;
}

bool CBranchState::Load(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS & /*prg*/) {
  // branch state saved from version 3.0.83
  if (CStreamVersion(3, 0, 82) < version) {
    // is it a branch?
    int iBranch;
    stream >> iBranch;

    if (iBranch) {
      // get the branch stage index
      int branch_idx;
      stream >> branch_idx;

      // should be less than total number of stages
      assert(branch_idx >= 0 && branch_idx < m_model.DepletionStageEntry().EntryNodes().size());

      const CDepletionStage *pStage = &m_model.DepletionStageEntry().InitialStage();
      int idx = 0;
      while (pStage) {
        if (idx == branch_idx) {
          SetBranchStage(*pStage);
          break;
        }

        if (pStage->Last())
          pStage = 0;
        else {
          pStage = &pStage->Next();
          ++idx;
        }
      }
    }
  }

  return true;
}
