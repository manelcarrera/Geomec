#ifndef _IColorScaleNode_Delegate_h_
#define _IColorScaleNode_Delegate_h_

class IColorScaleNode;

#include "StorageNode_Delegate.h"

class IColorScaleNode_Delegate : public CStorageNode_Delegate
{
public:
  IColorScaleNode_Delegate(IColorScaleNode* colorScaleNode);

private:
  IColorScaleNode_Delegate(const IColorScaleNode_Delegate& rhs);
  IColorScaleNode_Delegate& operator = (const IColorScaleNode_Delegate& rhs);

  IColorScaleNode* m_colorScaleNode;

  REGISTER_DELEGATE(IColorScaleNode, IColorScaleNode_Delegate);
};

#endif  // _IColorScaleNode_Delegate_h_
