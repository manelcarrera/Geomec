#ifndef _ColorGradient_Delegate_h_
#define _ColorGradient_Delegate_h_

class CColorGradient;

#include "IColorScaleNode_Delegate.h"

class CColorGradient_Delegate : public IColorScaleNode_Delegate
{
public:
  CColorGradient_Delegate(CColorGradient* colorGradient);

  virtual bool Attributes();

  bool CanFindExtents() const;
  bool FindExtents();

private:
  CColorGradient_Delegate(const CColorGradient_Delegate& rhs);
  CColorGradient_Delegate& operator = (const CColorGradient_Delegate& rhs);

  CColorGradient* m_colorGradient;

  REGISTER_DELEGATE(CColorGradient, CColorGradient_Delegate);
};

#endif  // _ColorGradient_Delegate_h_
