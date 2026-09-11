#ifndef _FemAppModel_Delegate_h_
#define _FemAppModel_Delegate_h_

class CFemAppModel;

#include "GraphNode_Delegate.h"

class CFemAppModel_Delegate : public CGraphNode_Delegate {
public:
  CFemAppModel_Delegate(CFemAppModel *femAppModel);

private:
  CFemAppModel_Delegate(const CFemAppModel_Delegate &rhs);
  CFemAppModel_Delegate &operator=(const CFemAppModel_Delegate &rhs);

  CFemAppModel *m_femAppModel;

  REGISTER_DELEGATE(CFemAppModel, CFemAppModel_Delegate);
};

#endif // _FemAppModel_Delegate_h_
