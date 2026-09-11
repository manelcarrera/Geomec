#include "FailureModePatternMismatch.h"

namespace cora {

namespace {

const QString PATTERN_NOT_FOUND = "the pattern '%1' is not found in failure mode '%2'";

} // anonymous namespace

CFailureModePatternMismatch::CFailureModePatternMismatch(const QString &failureMode, const QString &pattern)
    : std::runtime_error(QString(PATTERN_NOT_FOUND).arg(pattern).arg(failureMode).toStdString()) {}

} // namespace cora
