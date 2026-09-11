
#include <memory>

#include <QFile>

#include "FileCopier.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "IProgressBase.h"

CFileCopier::CFileCopier(const QString& strSourcePath, const QString& strTargetPath, const QString& strTitle)
: m_strSourcePath(strSourcePath),
  m_strTargetPath(strTargetPath),
  m_strTitle(strTitle)
{
}

bool CFileCopier::Copy()
{
  std::auto_ptr <IProgressBase> progress(_g->prog()->create(eProgress::Geo, m_strTitle));

  progress->AddSteps(100);
  progress->StatusMessage(m_strTitle);

  QFile from(m_strSourcePath);
  QFile to(m_strTargetPath);
  qint64 totalBytes = from.size();
  qint64 chunkSize = totalBytes / 100;
  char* buffer = new char [chunkSize];
  qint64 bytesRead = 0, bytesWritten = 0;
  bool opened = false;

  opened = from.open(QIODevice::ReadOnly);
  opened = opened && to.open(QIODevice::WriteOnly);

  if (!opened)
  {
  return false;
  }

  while ((totalBytes > 0) && ((bytesRead = from.read(buffer, chunkSize)) != -1))
  {
  assert(bytesRead = chunkSize);
  bytesWritten = to.write(buffer, chunkSize);
  assert(bytesWritten = chunkSize);
  totalBytes -= bytesRead;

  try
  {
      progress->Step();
  }

  catch (CProgressCancel* e)
  {
      delete e;
      bytesRead = -1;
      break;
  }

  if (chunkSize > totalBytes)
  {
      chunkSize = totalBytes;
  }
  }

  if (bytesRead == -1)
  {
  return false;
  }

  return true;
}
