#ifndef _MaterialFractureAperture_Delegate_h_
#define _MaterialFractureAperture_Delegate_h_

class CMaterialFractureAperture;

#include "MaterialFractureApertureBase_Delegate.h"

class CMaterialFractureAperture_Delegate : public CMaterialFractureApertureBase_Delegate
{
public:
  CMaterialFractureAperture_Delegate(
  CMaterialFractureAperture* materialFractureAperture);

private:
  CMaterialFractureAperture_Delegate(
  const CMaterialFractureAperture_Delegate& rhs);
  CMaterialFractureAperture_Delegate& operator = (
  const CMaterialFractureAperture_Delegate& rhs);

  CMaterialFractureAperture* m_materialFractureAperture;

  REGISTER_DELEGATE(CMaterialFractureAperture,
  CMaterialFractureAperture_Delegate);
};

#endif  // _MaterialFractureAperture_Delegate_h_
