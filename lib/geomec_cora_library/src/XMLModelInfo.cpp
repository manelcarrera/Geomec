#include <fstream>

#include "XMLModelInfo.h"
#include "ModelData.h"
#include "VersionNumber.h"

namespace cora
{

CXMLModelInfo::CXMLModelInfo(CModelData& modelData,
  const char* modelInfoFileName)
: m_modelData(modelData)
, m_modelInfoFileName(modelInfoFileName)
, m_components(modelData)
{
}

bool CXMLModelInfo::operator () () const
{
  return (((m_modelData() != 0) && createModelInfo()) ? true : false);
}

namespace
{

const QString XML_DECLARATION = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
const QString MODELINFO_OPEN = "<ModelInfo type=\"Deltares.Probabilistic."
  "Calculation.ModelInfo\" version=\"" + VERSION_NUMBER + "\" Name=\"Geomec\" "
  "Description=\"modelinfo as provided by geomec_cora.exe\" "
  "MaxProcesses=\"8\">";
const QString MODELINFO_CLOSE = "</ModelInfo>";

} // anonymous namespace

bool CXMLModelInfo::createModelInfo() const
{
  std::ofstream modelInfoFile((QString(m_modelInfoFileName) + ".xml").
    toStdString().c_str());

  modelInfoFile << XML_DECLARATION.toStdString() << std::endl;
  modelInfoFile << MODELINFO_OPEN.toStdString() << std::endl;

  modelInfoFile << m_components;

  modelInfoFile << MODELINFO_CLOSE.toStdString() << std::endl;

  return true;
}

} // namespace cora
