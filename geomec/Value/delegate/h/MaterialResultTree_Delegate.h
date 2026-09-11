#ifndef _MaterialResultTree_Delegate_h_
#define _MaterialResultTree_Delegate_h_

class CMaterialResult;
class CMaterialResultTree;

#include "Result_Delegate.h"

class CMaterialResult_Delegate : public IResult_Delegate
{
public:
  CMaterialResult_Delegate(CMaterialResult* materialResult);

private:
  CMaterialResult_Delegate(const CMaterialResult_Delegate& rhs);
  CMaterialResult_Delegate& operator = (const CMaterialResult_Delegate& rhs);

  CMaterialResult* m_materialResult;

  REGISTER_DELEGATE(CMaterialResult, CMaterialResult_Delegate);
};

class CMaterialResultTree_Delegate : public CResultGroup_Delegate
{
public:
  CMaterialResultTree_Delegate(CMaterialResultTree* materialResultTree);

private:
  CMaterialResultTree_Delegate(const CMaterialResultTree_Delegate& rhs);
  CMaterialResultTree_Delegate& operator = (
  const CMaterialResultTree_Delegate& rhs);

  CMaterialResultTree* m_materialResultTree;

  REGISTER_DELEGATE(CMaterialResultTree, CMaterialResultTree_Delegate);
};

#endif  // _MaterialResultTree_Delegate_h_
