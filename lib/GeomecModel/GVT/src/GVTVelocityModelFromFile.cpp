#include "GVTVelocityModelFromFile.h"
#include "GeomecUtils.h"

namespace GVT
{

CGVTVelocityModelFromFile::CGVTVelocityModelFromFile(
  const QString& velocityModelFile)
: CGVTVelocityModelSource()
, m_velocityModelFile(velocityModelFile)
{
}

CGVTVelocityModelFromFile::~CGVTVelocityModelFromFile()
{
}

const QString& CGVTVelocityModelFromFile::velocityModelFile() const
{
  return m_velocityModelFile;
}

void CGVTVelocityModelFromFile::velocityModelFile(
  const QString& velocityModelFile)
{
  m_velocityModelFile = velocityModelFile;
}

bool CGVTVelocityModelFromFile::isVelocityModelFileReady() const
{
  if (velocityModelFile().isEmpty() ||
  !FileExists(velocityModelFile().toStdString().c_str()))
  {
  return false;
  }

  return true;
}

int CGVTVelocityModelFromFile::velocityModelSourceNumerator() const
{
  return VELOCITY_FROM_FILE;
}

void CGVTVelocityModelFromFile::LoadStream(TSTREAM& stream,
  CStreamVersion& /*version*/, TPROGRESS& /*progress*/)
{
  stream >> m_velocityModelFile;
}

void CGVTVelocityModelFromFile::SaveStream(TSTREAM& stream,
  TPROGRESS& /*progress*/)
{
  stream << VELOCITY_FROM_FILE;
  stream << m_velocityModelFile;
}

} // namespace GVT
