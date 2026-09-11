#ifndef _cora_ValueVectorContainsInvalidValues_h_
#define _cora_ValueVectorContainsInvalidValues_h_

#include <stdexcept>

#include <QString>

namespace cora {

class CValueVectorContainsInvalidValues : public std::runtime_error {
public:
  CValueVectorContainsInvalidValues(const QString &valueType);
};

} // namespace cora

#endif // _cora_ValueVectorContainsInvalidValues_h_
