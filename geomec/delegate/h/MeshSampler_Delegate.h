#ifndef _MeshSampler_Delegate_h_
#define _MeshSampler_Delegate_h_

class CPillarMap;

#include "ColorNode_Delegate.h"

class CPillarMap_Delegate : public CColorNode_Delegate
{
public:
  CPillarMap_Delegate(CPillarMap* pillarMap);

  virtual bool Attributes();

private:
  CPillarMap_Delegate(const CPillarMap_Delegate& rhs);
  CPillarMap_Delegate& operator = (const CPillarMap_Delegate& rhs);

  CPillarMap* m_pillarMap;

  REGISTER_DELEGATE(CPillarMap, CPillarMap_Delegate);
};

#endif  // _MeshSampler_Delegate_h_
