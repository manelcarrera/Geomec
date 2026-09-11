#ifndef _BaseSupportNode_Delegate_h_
#define _BaseSupportNode_Delegate_h_

class CBaseSupportNode;
class CSupportDepletion;

#include "OpenGLNode_Delegate.h"

class CSupportDepletion_Delegate : public CStorageNode_Delegate
{
public:
  CSupportDepletion_Delegate(CSupportDepletion* supportDepletion);

private:
  CSupportDepletion_Delegate(const CSupportDepletion_Delegate& rhs);
  CSupportDepletion_Delegate& operator = (
    const CSupportDepletion_Delegate& rhs);

  CSupportDepletion* m_supportDepletion;

  REGISTER_DELEGATE(CSupportDepletion, CSupportDepletion_Delegate);
};

class CBaseSupportNode_Delegate : public COpenGLNode_Delegate
{
public:
  CBaseSupportNode_Delegate(CBaseSupportNode* baseSupportNode);

private:
  CBaseSupportNode_Delegate(const CBaseSupportNode_Delegate& rhs);
  CBaseSupportNode_Delegate& operator = (const CBaseSupportNode_Delegate& rhs);

  CBaseSupportNode* m_baseSupportNode;

  REGISTER_DELEGATE(CBaseSupportNode, CBaseSupportNode_Delegate);
};

#endif  // _BaseSupportNode_Delegate_h_
