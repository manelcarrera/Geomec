#include "Pre.2.0.ResponseTypeBaseWithValue.h"
#include "SummaryResultFile.h"
#include "IncompleteLimitStateFunction.h"

namespace cora
{

namespace
{

const size_t LSF_LENGTH_WITH_VALUE = 5;
const size_t VALUE_POSITION = 4;

} // anonymous namespace

CPre20ResponseTypeBaseWithValue::CPre20ResponseTypeBaseWithValue(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CResponseTypeBaseValidate(summaryResultFile, function)
, m_value(extractValue(summaryResultFile, function, LSF_LENGTH_WITH_VALUE,
  VALUE_POSITION))
{
}

} // namespace cora
