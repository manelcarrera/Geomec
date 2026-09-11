#include "HDF5StorageInterface.h"
#include "HDF5File.h"
#include "DataCell.h"
#include "MeshDataCacher.h"
#include "AnalysisType.h"


CHDF5StorageInterface::CHDF5StorageInterface(CHDF5File& h5file, int modelIndex, int maxStages, int maxColumns)
  : IStorageInterface()
  , m_h5file(&h5file)
  , m_modelIndex(modelIndex)
  , m_maxStages(maxStages)
  , m_maxColumns(maxColumns)
  , m_directory(0)
  , m_fallbackStorage(0)
  , m_useFallback(5, false)
  , m_invalid(5, false)
{
}

CHDF5StorageInterface::~CHDF5StorageInterface()
{
  delete m_fallbackStorage;
}

const char *CHDF5StorageInterface::Directory() const
{
  return m_directory;
}

void CHDF5StorageInterface::Directory(const char *directory)
{
  m_directory = directory;
  if (m_fallbackStorage)
    m_fallbackStorage->Directory(m_directory);
}

void CHDF5StorageInterface::SetupFallback()
{
  if (!m_fallbackStorage)
    m_fallbackStorage = new mdc::CStorageInterfaceDefault(m_directory);
}

void CHDF5StorageInterface::SplitColumn(int column, int& type, int& stage, int& result)
{
  int colsPerType = m_maxStages * m_maxColumns;

  type = column / colsPerType;
  column -= type * colsPerType;

  stage = column / m_maxColumns;
  result = column - stage * m_maxColumns;

  assert(type < 5);
  assert(stage >= 0);
  assert(result >= 0 && result < m_maxColumns);
}

bool CHDF5StorageInterface::ReadBlock(mdc::CDataCell& cell)
{
  bool retval = true;

  int type, stage, result;
  SplitColumn(cell.m_nColumn, type, stage, result);

  if (m_useFallback[type])
    return m_fallbackStorage->ReadBlock(cell);

  cell.InitData();

  if (m_invalid[type])
    return retval;

  if (!m_h5file->CanRead())
    return retval;

  int nBytesToRead = cell.m_cacher.Size() * sizeof(double);
  int hint = nBytesToRead < 48 * 1024 ? CHDF5File::SMALL : 0;

  if (m_h5file->PushDataSet(CHDF5File::ResultGroupStream(m_modelIndex, type, stage, result), hint))
  {
    int nBytesRead = 0;

    nBytesRead += m_h5file->read((char *)&cell.m_checkSum, sizeof(mdc::CChecksum));
    nBytesRead += m_h5file->read((char *)cell.m_pData, nBytesToRead);

    cell.m_bDirty = false; // if we failed to read, we should not write

    if (!(nBytesToRead + sizeof(mdc::CChecksum) == nBytesRead && cell.ChecksumOK()))
      retval = false;
  }

  m_h5file->PopDataSet();

  return retval;
}

bool CHDF5StorageInterface::WriteBlock(mdc::CDataCell& cell)
{
  bool retval = true;

  if (!cell.m_pData)
    return retval;

  int type, stage, result;
  SplitColumn(cell.m_nColumn, type, stage, result);

  if (!m_useFallback[type] && !m_h5file->CanWrite())
  {
    m_useFallback[type] = true;
    SetupFallback();
  }

  if (m_useFallback[type])
    return m_fallbackStorage->WriteBlock(cell);

  m_invalid[type] = false;

  int nBytesToWrite = cell.m_cacher.Size() * sizeof(double);
  int hint = nBytesToWrite < 48 * 1024 ? CHDF5File::SMALL : 0;

  if (m_h5file->PushDataSet(CHDF5File::ResultGroupStream(m_modelIndex, type, stage, result), hint))
  {
    int nBytesWritten = 0;

    cell.ChecksumOK();

    nBytesWritten += m_h5file->write((char *)&cell.m_checkSum, sizeof(mdc::CChecksum));
    nBytesWritten += m_h5file->write((char *)cell.m_pData, nBytesToWrite);

    if (nBytesToWrite + sizeof(mdc::CChecksum) == nBytesWritten) // if we failed to write, this cell remains dirty
      cell.m_bDirty = false;
    else
      retval = false;
  }
  else
    retval = false;

  m_h5file->PopDataSet();
  
  return retval;
}

bool CHDF5StorageInterface::OwnedByGuard() const
{
  return m_owned_by_guard;
}

void CHDF5StorageInterface::OwnedByGuard(bool bOwnedByGuard)
{
  m_owned_by_guard = bOwnedByGuard;
}

void CHDF5StorageInterface::Invalidate(int nAnalysisType)
{
  int type = -1;

  switch (nAnalysisType)
  {
  case CAnalysisType::AT_LINEAR:
    type = 1;
    break;
  case CAnalysisType::AT_NONLIN:
    type = 0;
    break;
  case CAnalysisType::AT_HEAT:
    type = 2;
    break;
  case CAnalysisType::AT_MIXTURE:
    type = 3;
    break;
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
    type = 4;
    break;
  }
  assert(type >= 0);
  
  m_invalid[type] = true;
}

void CHDF5StorageInterface::AttachHDF5File(CHDF5File *h5file)
{
  m_h5file = h5file;
}
