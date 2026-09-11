#ifndef _propertyValues_h_
#define _propertyValues_h_

#include <QString>

#include "RGUtils.h"
#include "Value.h"

namespace GeomecRGI
{

template <typename T>
  bool propertyValuesContainNull(const std::vector <T>& propertyValues)
{
  for (size_t s = 0; s < propertyValues.size(); ++s)
  {
    if (RGUtils::isNull(propertyValues[s]))
    {
      return true;
    }
  }

  return false;
}

namespace
{

QString NULL_VALUES = "properties of type %1 may not contain 'NULL' values";

} // anonymous namespace

template <typename T>
  void propertyValuesMayNotContainNull(const RGPropertyType::Type& propertyType,
    const std::vector <T>& propertyValues)
{
  if ((propertyValues.size() > 0) && propertyValuesContainNull(propertyValues))
  {
    QString message =
      NULL_VALUES.arg(RGProperty::toString(propertyType).c_str());

    throw message;
  }
}

template <typename T>
  void convert2geoCValue(std::vector <geo::CValue>& geoCValues,
    const std::vector <T>& values)
{
  geoCValues.resize(values.size());

  for (size_t s = 0; s < values.size(); ++s)
  {
    if (!RGUtils::isNull(values[s]))
    {
      geoCValues[s] = values[s];
    }
  }
}

} // namespace GeomecRGI

#endif  // _propertyValues_h_
