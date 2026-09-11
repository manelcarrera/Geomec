#include "ArchiveInterface.h"

CArchiveInterface::~CArchiveInterface()
{
}

bool CArchiveInterface::IsHDF5() const
{
  return false;
}

CHDF5File *CArchiveInterface::GetHDF5File() const
{
  return 0;
}

bool CArchiveInterface::PushDataSet(const QString& /*dataSet*/, int /*hint*/)
{
  return true;
}
bool CArchiveInterface::PopDataSet()
{
  return true;
}

CArchiveInterface& CArchiveInterface::CreateCompressedStream()
{
  return *this;
}

void CArchiveInterface::DestroyCompressedStream()
{
}

bool CArchiveInterface::IsCompressed() const
{
  return false;
}
