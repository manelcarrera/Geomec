#ifndef _MeasuredTopDisplacementsNode_Delegate_h_
#define _MeasuredTopDisplacementsNode_Delegate_h_

class CMeasuredTopDisplacementsNode;

#include "StorageNode_Delegate.h"

class CMeasuredTopDisplacementsNode_Delegate : public CStorageNode_Delegate {
public:
  CMeasuredTopDisplacementsNode_Delegate(CMeasuredTopDisplacementsNode *measuredTopDisplacementsNode);

private:
  CMeasuredTopDisplacementsNode_Delegate(const CMeasuredTopDisplacementsNode_Delegate &rhs);
  CMeasuredTopDisplacementsNode_Delegate &operator=(const CMeasuredTopDisplacementsNode_Delegate &rhs);

  CMeasuredTopDisplacementsNode *m_measuredTopDisplacementsNode;

  REGISTER_DELEGATE(CMeasuredTopDisplacementsNode, CMeasuredTopDisplacementsNode_Delegate);
};

#endif // _MeasuredTopDisplacementsNode_Delegate_h_
