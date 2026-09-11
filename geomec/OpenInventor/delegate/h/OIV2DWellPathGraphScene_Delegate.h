#ifndef _OIV2DWellPathGraphScene_Delegate_h
#define _OIV2DWellPathGraphScene_Delegate_h_

class OIV2DWellPathGraphScene;

#include "OIV2DGraphValueColorScene_Delegate.h"

class OIV2DWellPathGraphScene_Delegate: public OIV2DGraphValueColorScene_Delegate
{
  OIV2DWellPathGraphScene_Delegate(const OIV2DWellPathGraphScene_Delegate& rhs);
  OIV2DWellPathGraphScene_Delegate& operator=(const OIV2DWellPathGraphScene_Delegate& rhs);

  OIV2DWellPathGraphScene* m_Node;

  REGISTER_DELEGATE(OIV2DWellPathGraphScene, OIV2DWellPathGraphScene_Delegate);

public:

  OIV2DWellPathGraphScene_Delegate(OIV2DWellPathGraphScene* node);

  virtual bool Attributes();
};

#endif  // _OIV2DWellPathGraphScene_Delegate_h_
