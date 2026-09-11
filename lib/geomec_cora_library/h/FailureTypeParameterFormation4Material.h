#ifndef _cora_FailureTypeParameterFormation4Material_h_
#define _cora_FailureTypeParameterFormation4Material_h_

#include "FailureTypeParameterFormation.h"

namespace cora {

class CFailureTypeParameterFormation4Material : public CFailureTypeParameterFormation {
public:
  CFailureTypeParameterFormation4Material(CSummaryResultFile &summaryResultFile, const std::string &object,
                                          const std::string &option, const std::string &parameter,
                                          std::vector<double> &value, const CGetModelInfo &modelInfo);
  virtual ~CFailureTypeParameterFormation4Material();

  virtual void modify(CModelBase *modelBase);

private:
  CFailureTypeParameterFormation4Material(const CFailureTypeParameterFormation4Material &rhs);
  CFailureTypeParameterFormation4Material &operator=(CFailureTypeParameterFormation4Material rhs);
};

} // namespace cora

#endif // _cora_FailureTypeParameterFormation4Material_h_
