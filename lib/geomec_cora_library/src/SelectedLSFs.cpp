#include "SelectedLSFs.h"
#include "ParseCSV.h"
#include "SummaryResultFile.h"

namespace cora {

CSelectedLSFs::CSelectedLSFs(CSummaryResultFile &summaryResultFile, CGetModelInfo &modelInfo,
                             const QString &versionNumber, const QString &fileName)
    : m_fileName(fileName), m_selectedLSFsFile(m_fileName.toStdString().c_str()),
      m_summaryResultFile(summaryResultFile),
      m_limitStateFunctions(
          readSelectedLSFs(m_summaryResultFile, m_selectedLSFsFile, modelInfo, versionNumber, m_fileName)) {}

std::vector<TLimitStateFunction> CSelectedLSFs::getLimitStateFunctions() const { return m_limitStateFunctions; }

// private

// static

namespace {

const QString SELECTED_LFS_FILE_DOES_NOT_EXIST = "selected LSF file '%1' does not exist";

} // anonymous namespace

std::vector<TLimitStateFunction> CSelectedLSFs::readSelectedLSFs(CSummaryResultFile &summaryResultFile,
                                                                 std::ifstream &selectedLSFsFile,
                                                                 CGetModelInfo &modelInfo, const QString &versionNumber,
                                                                 const QString &fileName) {
  if (!selectedLSFsFile.good()) {
    summaryResultFile.setResultValue(CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    summaryResultFile.addAdditionalInformation(QString(SELECTED_LFS_FILE_DOES_NOT_EXIST).arg(fileName));
  }

  std::vector<TLimitStateFunction> limitStateFunctions;
  CParseCSV parseCSV(selectedLSFsFile);
  std::vector<QString> oneFunction = parseCSV.readLine();

  while (oneFunction.size() > 0) {
    TLimitStateFunction limitStateFunction =
        TLimitStateFunction(new CLimitStateFunction(summaryResultFile, oneFunction, modelInfo, versionNumber));

    limitStateFunctions.push_back(limitStateFunction);
    oneFunction = parseCSV.readLine();
  }

  return limitStateFunctions;
}

} // namespace cora
