#ifndef _TetraSubBoundaryObserver_h_
#define _TetraSubBoundaryObserver_h_

class CTetraSubModelBranch;

#include "NodeObserver_Delegate.h"
#include "TetraSubBoundary_Delegate.h"

typedef CNodeObserver_Delegate<CTetraSubBoundary, CTetraSubBoundary_Delegate, CDummyNode, CDummyObserver, FALSE,
                               FIXED_ITEM>
    TTetraSubBoundaryObserver;
class CTetraSubBoundaryObserver : public TTetraSubBoundaryObserver {

public:
  CTetraSubBoundaryObserver(CTetraSubModelBranch &model_branch, CTetraSubBoundary &boundary);

  virtual BOOL OnSelect();
};

#endif // _TetraSubBoundaryObserver_h_
