#ifndef _cora_SummaryResultFile_h_
#define _cora_SummaryResultFile_h_

#include <fstream>
#include <sstream>

#include <QString>

namespace cora
{

class CSummaryResultFile
{
  public:
    CSummaryResultFile(const QString& fileName);
    ~CSummaryResultFile();

    enum TResultValue
    {
      RESULT_VALUE_OK,
      RESULT_VALUE_INCONSISTENT,
      RESULT_VALUE_TOO_MANY,
      RESULT_VALUE_DIVERGENCE
    };

    TResultValue getResultValue() const;
    void setResultValue(TResultValue resultValue);
    void addAdditionalInformation(const QString& additionalInformation);

  private:
    CSummaryResultFile(const CSummaryResultFile& rhs);
    CSummaryResultFile& operator = (CSummaryResultFile rhs);

    std::ofstream m_summaryResultFile;
    TResultValue m_resultValue;
    std::ostringstream m_additionalInformation;
};

} // namespace cora

#endif  // _cora_SummaryResultFile_h_
