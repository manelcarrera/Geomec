#include "ConvertModeType.h"

CConvertModeType::CConvertModeType()
{
}

CFaultPressure::TModeType CConvertModeType::operator () (int index) const
{
  CFaultPressure::TModeType modeType;

  switch (index)
  {
    case 0:
      modeType = CFaultPressure::MT_MAXIMUM;
      break;
    case 1:
      modeType = CFaultPressure::MT_MINIMUM;
      break;
    case 2:
      modeType = CFaultPressure::MT_GRADIENT;
      break;
    case 3:
      modeType = CFaultPressure::MT_MAXGRADIENT;
      break;
    default:
      assert(false);
      modeType = CFaultPressure::MT_MAXIMUM;
      break;
  }

  return modeType;
}

int CConvertModeType::operator () (CFaultPressure::TModeType modeType) const
{
  int index;

  switch (modeType)
  {
    case CFaultPressure::MT_MAXIMUM:
      index = 0;
      break;
    case CFaultPressure::MT_MINIMUM:
      index = 1;
      break;
    case CFaultPressure::MT_GRADIENT:
      index = 2;
      break;
    case CFaultPressure::MT_MAXGRADIENT:
      index = 3;
      break;
    default:
      assert(false);
      index = 0;
      break;
  }

  return index;
}
