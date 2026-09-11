#ifndef _AnalysisPoint_Delegate_h_
#define _AnalysisPoint_Delegate_h_

class CAnalysisPoint;
class CAnalysisPointEntry;

#include "ColorNode_Delegate.h"

class CAnalysisPoint_Delegate : public CColorNode_Delegate {
public:
  CAnalysisPoint_Delegate(CAnalysisPoint *analysisPoint);

  virtual bool Attributes();

private:
  CAnalysisPoint_Delegate(const CAnalysisPoint_Delegate &rhs);
  CAnalysisPoint_Delegate &operator=(const CAnalysisPoint_Delegate &rhs);

  CAnalysisPoint *m_analysisPoint;

  REGISTER_DELEGATE(CAnalysisPoint, CAnalysisPoint_Delegate);
};

class CAnalysisPointEntry_Delegate : public CStorageNodeEntry_Delegate<CAnalysisPoint> {
public:
  CAnalysisPointEntry_Delegate(CAnalysisPointEntry *analysisPointEntry);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);
  void CreateAnalysisPoint();

private:
  CAnalysisPointEntry_Delegate(const CAnalysisPointEntry_Delegate &rhs);
  CAnalysisPointEntry_Delegate &operator=(const CAnalysisPointEntry_Delegate &rhs);

  CAnalysisPointEntry *m_analysisPointEntry;

  REGISTER_DELEGATE(CAnalysisPointEntry, CAnalysisPointEntry_Delegate);
};

#endif // _AnalysisPoint_Delegate_h_
