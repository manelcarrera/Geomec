#ifndef _MaterialFractureApertureBase_Delegate_h_
#define _MaterialFractureApertureBase_Delegate_h_

class CMaterialFractureApertureBase;

#include "IMaterialRock_Delegate.h"

class CMaterialFractureApertureBase_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialFractureApertureBase_Delegate(
  CMaterialFractureApertureBase* materialFractureApertureBase);

private:
  CMaterialFractureApertureBase_Delegate(
  const CMaterialFractureApertureBase_Delegate& rhs);
  CMaterialFractureApertureBase_Delegate& operator = (
  const CMaterialFractureApertureBase_Delegate& rhs);

  CMaterialFractureApertureBase* m_materialFractureApertureBase;

  REGISTER_DELEGATE(CMaterialFractureApertureBase,
  CMaterialFractureApertureBase_Delegate);
};

#endif  // _MaterialFractureApertureBase_Delegate_h_
