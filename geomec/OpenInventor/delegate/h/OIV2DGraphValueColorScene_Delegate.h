#ifndef _OIV2DGraphValueColorScene_Delegate_h
#define _OIV2DGraphValueColorScene_Delegate_h

class OIV2DGraphValueColorScene;

#include "StorageNode_Delegate.h"

class OIV2DGraphValueColorScene_Delegate : public CStorageNode_Delegate
{
  public:
  OIV2DGraphValueColorScene_Delegate(OIV2DGraphValueColorScene* node);

  private:
  OIV2DGraphValueColorScene_Delegate(
      const OIV2DGraphValueColorScene_Delegate& rhs);
  OIV2DGraphValueColorScene_Delegate(
      OIV2DGraphValueColorScene_Delegate&& rhs);
  OIV2DGraphValueColorScene_Delegate& operator = (
      const OIV2DGraphValueColorScene_Delegate& rhs);
  OIV2DGraphValueColorScene_Delegate& operator = (
      const OIV2DGraphValueColorScene_Delegate&& rhs);

  OIV2DGraphValueColorScene* m_Node;

  REGISTER_DELEGATE(OIV2DGraphValueColorScene,
      OIV2DGraphValueColorScene_Delegate);
};

#endif  // _OIV2DGraphValueColorScene_Delegate_h
