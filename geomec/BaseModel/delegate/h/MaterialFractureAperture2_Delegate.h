#ifndef _MaterialFractureAperture2_Delegate_h_
#define _MaterialFractureAperture2_Delegate_h_

class CMaterialFractureAperture2;

#include "MaterialFractureApertureBase_Delegate.h"

class CMaterialFractureAperture2_Delegate : public CMaterialFractureApertureBase_Delegate
{
public:
  CMaterialFractureAperture2_Delegate(
    CMaterialFractureAperture2* materialFractureAperture);

private:
  CMaterialFractureAperture2_Delegate(
    const CMaterialFractureAperture2_Delegate& rhs);
  CMaterialFractureAperture2_Delegate& operator = (
    const CMaterialFractureAperture2_Delegate& rhs);

  CMaterialFractureAperture2* m_materialFractureAperture2;

  REGISTER_DELEGATE(CMaterialFractureAperture2,
    CMaterialFractureAperture2_Delegate);
};

#endif  // _MaterialFractureAperture2_Delegate_h_
