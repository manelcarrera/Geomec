#pragma once

#include <map>
#include <string>

class IValueComponentBase;

namespace GeomecRGI {

struct PropertyMetaData {
  std::string propertyName;
  std::string typeName;
  enum Type { SCALAR = 0, DUAL_SCALAR = 1, VECTOR = 2, TENSOR_VECTOR = 3, TENSOR = 4 } typeType;
  int componentIndex;

  PropertyMetaData() : typeType(SCALAR), componentIndex(0) {}

  PropertyMetaData(const PropertyMetaData &rhs)
      : propertyName(rhs.propertyName), typeName(rhs.typeName), typeType(rhs.typeType),
        componentIndex(rhs.componentIndex) {}

  static void Setup(const std::map<std::string, const IValueComponentBase *> &properties,
                    std::map<std::string, PropertyMetaData> &metaData);
};

} // namespace GeomecRGI
