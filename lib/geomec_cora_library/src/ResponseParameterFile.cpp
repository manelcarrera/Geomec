#include "ResponseParameterFile.h"

namespace cora
{

CResponseParameterFile::CResponseParameterFile(const QString& fileName)
: m_responseParameterFile(fileName.toStdString().c_str())
{
}

void CResponseParameterFile::addResponseParameter(
  const QString& responseParameter)
{
  m_responseParameterFile << responseParameter.toStdString() << std::endl;
}

} // namespace cora
