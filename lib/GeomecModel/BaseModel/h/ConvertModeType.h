#ifndef _ConvertModeType_h_
#define _ConvertModeType_h_

#include "FaultPressure.h"

// This class converts an CFaultPressure::TModeType object
// to (and from) the index for the radio button group consisting of
// IDC_RADIO_MAXIMUM, IDC_RADIO_MINIMUM, IDC_RADIO_GRADIENT,
// IDC_RADIO_MAXGRADIENT as in the dialogs IDD_ATTRI_FAULT_PRESSURE and
// IDD_GLOBAL_FAULT_BEHAVIOUR.

class CConvertModeType
{
  public:
  CConvertModeType();

  CFaultPressure::TModeType operator () (int index) const;
  int operator () (CFaultPressure::TModeType modeType) const;

  private:
  CConvertModeType(const CConvertModeType& rhs);
  CConvertModeType& operator = (const CConvertModeType& rhs);
};

#endif  // _ConvertModeType_h_
