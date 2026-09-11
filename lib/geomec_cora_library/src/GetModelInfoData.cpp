#include "GetModelInfoData.h"

namespace cora {

namespace {

const QString DEFAULT_VERSION_NUMBER = "1.9"; // any number less than "2.0"

} // anonymous namespace

CGetModelInfoData::CGetModelInfoData(int argc, char *argv[])
    : m_modelData(argv[1]), m_versionNumberIsPresent(isVersionNumberPresent(argc, argv)),
      m_versionNumber(m_versionNumberIsPresent ? argv[3] : DEFAULT_VERSION_NUMBER),
      m_modelInfoFileName(m_versionNumberIsPresent ? argv[4] : argv[3]),
      m_summaryResultFile(m_versionNumberIsPresent ? argv[5] : argv[4]) {}

CGetModelInfoData::~CGetModelInfoData() {}

CModelData &CGetModelInfoData::modelData() { return m_modelData; }

const CModelData &CGetModelInfoData::modelData() const { return m_modelData; }

const QString &CGetModelInfoData::versionNumber() const { return m_versionNumber; }

const char *CGetModelInfoData::modelInfoFileName() const { return m_modelInfoFileName; }

CSummaryResultFile &CGetModelInfoData::summaryResultFile() { return m_summaryResultFile; }

const CSummaryResultFile &CGetModelInfoData::summaryResultFile() const { return m_summaryResultFile; }

// private

// static

bool CGetModelInfoData::isVersionNumberPresent(int /*argc*/, char *argv[]) {
  std::stringstream stream(argv[3]);
  double versionNumber;

  stream >> versionNumber;

  return !stream.fail();
}

} // namespace cora
