#ifndef _OIV2DHistoryScene_Delegate_h_
#define _OIV2DHistoryScene_Delegate_h_

class OIV2DHistoryScene;

#include "OIV2DGraphValueColorScene_Delegate.h"

class OIV2DHistoryScene_Delegate : public OIV2DGraphValueColorScene_Delegate {
  OIV2DHistoryScene_Delegate(const OIV2DHistoryScene_Delegate &rhs);
  OIV2DHistoryScene_Delegate &operator=(const OIV2DHistoryScene_Delegate &rhs);

  OIV2DHistoryScene *m_Node;

  REGISTER_DELEGATE(OIV2DHistoryScene, OIV2DHistoryScene_Delegate);

public:
  OIV2DHistoryScene_Delegate(OIV2DHistoryScene *node);

  virtual bool Attributes();
};

#endif // _OIV2DHistoryScene_Delegate_h_
