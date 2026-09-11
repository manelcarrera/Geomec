#ifndef _cora_ParameterLimit_h_
#define _cora_ParameterLimit_h_

#include "ParameterBase.h"

namespace cora {

class CParameterLimit : public CParameterBase {
public:
  CParameterLimit();
  virtual ~CParameterLimit();

  virtual const QString lowerLimit() const;
  virtual const QString upperLimit() const;

protected:
  static const double NAN_DOUBLE;
  static const QString NAN_STRING;

private:
  CParameterLimit(const CParameterLimit &rhs);
  CParameterLimit &operator=(const CParameterLimit &rhs);

  static std::pair<geo::CValue, geo::CValue> getRange(CGraphNode *object, unsigned int valueTypeID);
};

} // namespace cora

#endif // _cora_ParameterLimit_h_
