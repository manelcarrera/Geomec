#ifndef _ColorScaleEntry_Delegate_h_
#define _ColorScaleEntry_Delegate_h_

class CColorScaleEntry;

#include "IColorScaleEntry_Delegate.h"

class CColorScaleEntry_Delegate : public IColorScaleEntry_Delegate
{
public:
  CColorScaleEntry_Delegate(CColorScaleEntry* colorScaleEntry);

  virtual bool Attributes();

  // Creating gradients

  void OnNewColorGradient();
  void OnNewHotSpot();

private:
  CColorScaleEntry_Delegate(const CColorScaleEntry_Delegate& rhs);
  CColorScaleEntry_Delegate& operator = (const CColorScaleEntry_Delegate& rhs);

  CColorScaleEntry* m_colorScaleEntry;

  REGISTER_DELEGATE(CColorScaleEntry, CColorScaleEntry_Delegate);
};

#endif  // _ColorScaleEntry_Delegate_h_
