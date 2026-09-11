#ifndef _cora_Pre20ResponseTypeBaseWithValue_h_
#define _cora_Pre20ResponseTypeBaseWithValue_h_

#include "ResponseTypeBaseValidate.h"

namespace cora
{

class CPre20ResponseTypeBaseWithValue : public CResponseTypeBaseValidate
{
  public:
  CPre20ResponseTypeBaseWithValue(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);

  protected:
  double m_value;

  private:
  CPre20ResponseTypeBaseWithValue(const CPre20ResponseTypeBaseWithValue& rhs);
  CPre20ResponseTypeBaseWithValue& operator = (
      const CPre20ResponseTypeBaseWithValue& rhs);
};

} // namespace cora

#endif  // _cora_Pre20ResponseTypeBaseWithValue_h_
