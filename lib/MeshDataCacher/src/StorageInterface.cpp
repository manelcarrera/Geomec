#include "StorageInterface.h"

#include <cstring>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "lbcx.h"

#include "DataCell.h"
#include "MeshDataCacher.h"

#include <algorithm>


namespace mdc
{

IStorageInterface::IStorageInterface()
{
}

IStorageInterface::~IStorageInterface()
{
}






#ifdef WIN32
#include <io.h>
#endif  // WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif  // _S_IFDIR

#ifndef _O_RDONLY
#define _O_RDONLY O_RDONLY
#endif  // _O_RDONLY

#ifndef _O_BINARY
#define _O_BINARY O_BINARY
#endif  // _O_BINARY

#ifndef _O_WRONLY
#define _O_WRONLY O_WRONLY
#endif  // _O_WRONLY

#ifndef _O_CREAT
#define _O_CREAT O_CREAT
#endif  // _O_CREAT

#ifndef _S_IWRITE
#define _S_IWRITE S_IWRITE
#endif  // _S_IWRITE

#ifndef _S_IREAD
#define _S_IREAD S_IREAD
#endif // _S_IREAD

#ifndef WIN32
#define _snprintf snprintf
#endif  // WIN32


CStorageInterfaceDefault::CStorageInterfaceDefault(const char *directory)
  : IStorageInterface()
  , m_dir(0)
  , m_dir_exists(false)
{
  if (directory)
#ifdef _WIN32
  m_dir = _strdup(directory);
#else
  m_dir = strdup(directory);
#endif

  m_dir_exists = DirExists();
}

CStorageInterfaceDefault::~CStorageInterfaceDefault()
{
  free(m_dir);
}

const char *CStorageInterfaceDefault::Directory() const
{
  return m_dir;
}

void CStorageInterfaceDefault::Directory(const char *directory)
{
  if (m_dir)
  {
  assert(!directory || strcmp(m_dir, directory) == 0);
  free(m_dir);
  m_dir = 0;
  }

  if (directory)
#ifdef _WIN32
  m_dir = _strdup(directory);
#else
  m_dir = strdup(directory);
#endif

  m_dir_exists = DirExists();
}


// Tested on the laptop, different sizes could perform better on different computers.
// Timings laptop were about 10ms for 50mb write and half that for read; unbuffered around 500ms write, 20ms read
#define READ_BUF_SIZE  0x80000
#define WRITE_BUF_SIZE 0x40000

bool CStorageInterfaceDefault::ReadBlock(CDataCell& cell)
{
  char *fname = FileNameExists(cell.m_nColumn);

  if (!fname)
  return true;

  int fh = open(fname, _O_RDONLY | _O_BINARY);
  DiFree(fname, "CStorageInterfaceDefault::ReadBlock");
  if (fh == -1)
  return true;

  cell.InitData();

  int nBytesToRead = cell.m_cacher.Size() * sizeof(double);

  int nBytesRead = 0;

  nBytesRead += read(fh, &cell.m_checkSum, sizeof(CChecksum));

#ifdef READ_BUF_SIZE
  for (int i = 0; i < nBytesToRead; i += READ_BUF_SIZE)
  {
  int nToRead = ::std::min(nBytesToRead - i, READ_BUF_SIZE);
  int nRead = read(fh, (uchar *)cell.m_pData + i, nToRead);

  nBytesRead += nRead;
  }
#else
  nBytesRead += read(fh, cell.m_pData, nBytesToRead);
#endif

  close(fh);

  cell.m_bDirty = false; // if we failed to read, we should not write

  return nBytesToRead + sizeof(CChecksum) == nBytesRead && cell.ChecksumOK();
}

bool CStorageInterfaceDefault::WriteBlock(CDataCell& cell)
{
  if (!cell.m_pData) return true;

  char *fname = FileName(cell.m_nColumn);

  if (!fname)
  return true;

  int fh = open(fname, _O_WRONLY | _O_CREAT | _O_BINARY, _S_IWRITE | _S_IREAD);
  DiFree(fname, "CDataCell::WriteData");
  if (fh == -1) return false;

  int nBytesToWrite = cell.m_cacher.Size() * sizeof(double);

  int nBytesWritten = 0;

  cell.ChecksumOK();

  nBytesWritten += write(fh, &cell.m_checkSum, sizeof(CChecksum));

#ifdef WRITE_BUF_SIZE

  for (int i = 0; i < nBytesToWrite; i += WRITE_BUF_SIZE)
  {
  int nToWrite = ::std::min(nBytesToWrite - i, WRITE_BUF_SIZE);
  int nWritten = write(fh, (uchar *)cell.m_pData + i, nToWrite);

  nBytesWritten += nWritten;
  }
#else
  nBytesWritten += write(fh, cell.m_pData, nBytesToWrite);
#endif

  close(fh);

  if (nBytesToWrite + sizeof(CChecksum) == nBytesWritten) // if we failed to write, this cell remains dirty
  {
  cell.m_bDirty = false;
  return true;
  }
  else
  {
  remove(fname);
  return false;
  }
}

char *CStorageInterfaceDefault::FileNameExists(int block)
{
  struct stat buf;

  char *fn = FileName(block);
  if (!fn)
  return 0;

  if (stat(fn, &buf) == 0)
  return fn;

  DiFree(fn, "CStorageInterfaceDefault::FileNameExists");
  return 0;
}

char *CStorageInterfaceDefault::FileName(int block)
{
  if (!m_dir_exists)
  return 0;

  char filename[127];
  char *ret;

  _snprintf(filename, 127, "mdc_%d", block);

  ret = vDiStrsave(m_dir, "/", filename, NULL);

  NativeStylePath(ret);

  return ret;
}

bool CStorageInterfaceDefault::DirExists()
{
  struct stat buf;

  if (!m_dir)
  return false;

  // check existence of directory
  int status = stat(m_dir, &buf);
  if (status != 0)
  return false; // does not exist
  if (!(buf.st_mode | _S_IFDIR))
  return false; // not a directory
  if (!(buf.st_mode & _S_IWRITE))
  return false; // read only

  return true;
}


bool CStorageInterfaceDefault::OwnedByGuard() const
{
  return false;
}

void CStorageInterfaceDefault::OwnedByGuard(bool /*bOwnedByGuard*/)
{
}


}