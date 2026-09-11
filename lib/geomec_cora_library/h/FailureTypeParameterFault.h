#ifndef _cora_FailureTypeParameterFault_h_
#define _cora_FailureTypeParameterFault_h_

namespace cora {

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "CreateElementValueSet.h"
#include "FailureTypeParameterBase.h"

namespace cora {

class CFailureTypeParameterFault : public CFailureTypeParameterBase {
public:
  CFailureTypeParameterFault(CSummaryResultFile &summaryResultFile, const std::string &object,
                             const std::string &option, const std::string &parameter, std::vector<double> &value,
                             const CGetModelInfo &modelInfo);
  virtual ~CFailureTypeParameterFault();

  virtual void modify(CModelBase *modelBase);

private:
  CFailureTypeParameterFault(const CFailureTypeParameterFault &rhs);
  CFailureTypeParameterFault &operator=(CFailureTypeParameterFault rhs);

  static TCreateElementValueSet selectElementValueSetCreator(TParameter actualParameter);

  TCreateElementValueSet m_createElementValueSet;
};

} // namespace cora

#endif // _cora_FailureTypeParameterFault_h_
