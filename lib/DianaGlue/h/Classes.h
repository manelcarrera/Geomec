#pragma once

#include <QString>

class CAnalysisType {
public:
  typedef enum _AnalysisType {
    AT_FIRST,
    AT_LINEAR = AT_FIRST,
    AT_NONLIN,
    AT_HEAT,
    AT_MIXTURE,
    AT_MIXTURE_CONTAINMENT,
    AT_LAST = AT_MIXTURE_CONTAINMENT
  } TAnalysisType;

public:
  CAnalysisType() {};
};

class CDepletionStage {
public:
  enum eOutputType { NONE, RESULTS, BRANCH, PHASE };

public:
  CDepletionStage() {};
  CDepletionStage *Initial() { return new CDepletionStage(); }
  CDepletionStage *Previous() { return new CDepletionStage(); };
  void setOutputType(eOutputType type) {}
  bool IsPhaseStartStage() { return true; };
};

class CDepletionStageEntry {
public:
  CDepletionStageEntry() {}
  CDepletionStage *LastStage() { return new CDepletionStage(); };
};

class CBranchState {
public:
  CBranchState() {};
  void ClearBranch() {};
};

class CModelBase {
public:
  CModelBase() {};
  CDepletionStageEntry *DepletionStageEntry() { return new CDepletionStageEntry(); }
  CBranchState *BranchState() { return new CBranchState(); };
};

class CProgressCancel {
public:
  CProgressCancel() {};
};

class CGeomecDoc {
public:
  CGeomecDoc() {};
  CModelBase *Model() { return new CModelBase(); }
  QString GetPathName() { return ""; }
};
