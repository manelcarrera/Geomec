#ifndef _MaterialRigidity_Delegate_h_
#define _MaterialRigidity_Delegate_h_

class CMaterialRigidity;

#include "IMaterialRock_Delegate.h"

class CMaterialRigidity_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialRigidity_Delegate(CMaterialRigidity* materialRigidity);

private:
  CMaterialRigidity_Delegate(const CMaterialRigidity_Delegate& rhs);
  CMaterialRigidity_Delegate& operator = (
    const CMaterialRigidity_Delegate& rhs);

  CMaterialRigidity* m_materialRigidity;

  REGISTER_DELEGATE(CMaterialRigidity, CMaterialRigidity_Delegate);
};

#endif  // _MaterialRigidity_Delegate_h_
