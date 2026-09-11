
#include <limits>

#include "NewArchiveStdStringStream.h"

namespace
{

typedef unsigned char Byte;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

template <class S>
  void writeStringLength(S& archive, long length, bool unicode)
{
  assert(sizeof(Byte) == 1);
  assert(sizeof(UInt16) == 2);
  assert(sizeof(UInt32) == 4);
  assert(sizeof(UInt64) == 8);

  assert(length >=0);

  if (unicode)
  {
  Byte byte = 0xFF;
  UInt16 uint16 = 0xFFFE;

  archive.write(reinterpret_cast <char*> (&byte), sizeof(Byte));
  archive.write(reinterpret_cast <char*> (&uint16), sizeof(UInt16));
  }

  if (length < 255)
  {
  Byte byte = length;

  archive.write(reinterpret_cast <char*> (&byte), sizeof(Byte));
  }
  else if (length < 0xFFFE)
  {
  Byte byte = 0xFF;
  UInt16 uint16 = length;

  archive.write(reinterpret_cast <char*> (&byte), sizeof(Byte));
  archive.write(reinterpret_cast <char*> (&uint16), sizeof(UInt16));
  }
  else if (length < 0xFFFFFFFF)
  {
  Byte byte = 0xFF;
  UInt16 uint16 = 0xFFFF;
  UInt32 uint32 = length;

  archive.write(reinterpret_cast <char*> (&byte), sizeof(Byte));
  archive.write(reinterpret_cast <char*> (&uint16), sizeof(UInt16));
  archive.write(reinterpret_cast <char*> (&uint32), sizeof(UInt32));
  }
  else
  {
  Byte byte = 0xFF;
  UInt16 uint16 = 0xFFFF;
  UInt32 uint32 = 0xFFFFFFFF;
  UInt64 uint64 = length;

  archive.write(reinterpret_cast <char*> (&byte), sizeof(Byte));
  archive.write(reinterpret_cast <char*> (&uint16), sizeof(UInt16));
  archive.write(reinterpret_cast <char*> (&uint32), sizeof(UInt32));
  archive.write(reinterpret_cast <char*> (&uint64), sizeof(UInt64));
  }
}

template <class S>
  long readStringLength(S& archive, int& charSize)
{
  UInt64 uint64Length;
  UInt32 uint32Length;
  UInt16 uint16Length;
  Byte byteLength;

  charSize = 1;

  // First, try to read a one-byte length

  archive.read(reinterpret_cast <char*> (&byteLength), sizeof(Byte));

  if (byteLength < 0xFF)
  {
  return byteLength;
  }

  // Try a two-byte length

  archive.read(reinterpret_cast <char*> (&uint16Length), sizeof(UInt16));

  if (uint16Length == 0xFFFE)
  {
  // Unicode string. Start over at 1-byte length

  charSize = 2;

  archive.read(reinterpret_cast <char*> (&byteLength), sizeof(Byte));

  if (byteLength < 0xFF)
  {
      return byteLength;
  }

  // Two-byte length

  archive.read(reinterpret_cast <char*> (&uint16Length), sizeof(UInt16));

  // Fall through to continue on same branch as ANSI string
  }

  if (uint16Length < 0xFFFF)
  {
  return uint16Length;
  }

  // 4-byte length

  archive.read(reinterpret_cast <char*> (&uint32Length), sizeof(UInt32));

  if (uint32Length < 0xFFFFFFFF)
  {
  return uint32Length;
  }

  // 8-byte length

  archive.read(reinterpret_cast <char*> (&uint64Length), sizeof(UInt64));

#if (_MSC_VER != 1500)
  if (uint64Length > std::numeric_limits <UInt32> ::max())
#else // (_MSC_VER == 1500)
  if (uint64Length > UINT_MAX)
#endif  // (_MSC_VER != 1500)
  {
  throw "String too long";
  }

  return uint64Length;
}

} // anonymous namespace

//------------------------------------------------------------------------------

CNewArchiveStdStringStream::CNewArchiveStdStringStream(std::ios_base::openmode mode)
: archive(mode)
, mode(mode)
{
  assert(mode & std::ios_base::binary);
}

CNewArchiveStdStringStream::~CNewArchiveStdStringStream()
{
}

bool CNewArchiveStdStringStream::IsLoading() const
{
  return (mode & std::ios_base::in);
}

bool CNewArchiveStdStringStream::IsStoring() const
{
  return (mode & std::ios_base::out);
}

bool CNewArchiveStdStringStream::Exists() const
{
  return true;
}

quint64 CNewArchiveStdStringStream::GetPosition()
{
  return archive.tellg();
}

void CNewArchiveStdStringStream::SetPosition(quint64 position)
{
  archive.seekg(position, std::stringstream::beg);
}

void CNewArchiveStdStringStream::Flush()
{
  archive.flush();
}

void CNewArchiveStdStringStream::Close()
{
}

void CNewArchiveStdStringStream::Abort()
{
}

// write -----------------------------------------------------------------------

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (char c)
{
  archive.write(&c, sizeof(char));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (int i)
{
  archive.write(reinterpret_cast <char*> (&i), sizeof(int));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (unsigned int ui)
{
  archive.write(reinterpret_cast <char*> (&ui), sizeof(unsigned int));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (long l)
{
  archive.write(reinterpret_cast <char*> (&l), sizeof(long));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (unsigned long ul)
{
  archive.write(reinterpret_cast <char*> (&ul), sizeof(unsigned long));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (double d)
{
  archive.write(reinterpret_cast <char*> (&d), sizeof(double));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (const QString& s)
{
  QChar t;
  int length = s.length();

  writeStringLength(archive, length, false);

  for (int i = 0; i < length; ++i)
  {
  t = s[i];
  archive.write(reinterpret_cast <char*> (&t), sizeof(QChar));
  }

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  const QDateTime& dt)
{
  QString q = dt.toString();
  QChar t;
  int length = q.length();

  writeStringLength(archive, length, false);

  for (int i = 0; i < length; ++i)
  {
  t = q[i];
  archive.write(reinterpret_cast <char*> (&t), sizeof(QChar));
  }

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  bool /*b*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  quint8 /*b*/)
{
  assert(false);

  return *this;
}

#ifndef WIN32
CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  wchar_t /*wc*/)
{
  assert(false);

  return *this;
}
#endif  // WIN32

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  quint16 /*w*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  short /*w*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  qint64 /*ll*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  quint64 /*ull*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator << (
  float /*f*/)
{
  assert(false);

  return *this;
}

// read ------------------------------------------------------------------------

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (char& c)
{
  archive.read(&c, sizeof(char));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (int& i)
{
  archive.read(reinterpret_cast <char*> (&i), sizeof(int));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (unsigned int& ui)
{
  archive.read(reinterpret_cast <char*> (&ui), sizeof(unsigned int));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (long& l)
{
  archive.read(reinterpret_cast <char*> (&l), sizeof(long));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (unsigned long& ul)
{
  archive.read(reinterpret_cast <char*> (&ul), sizeof(unsigned long));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (double& d)
{
  archive.read(reinterpret_cast <char*> (&d), sizeof(double));

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (QString& s)
{
  QChar t;
  int charSize;
  long length = readStringLength(archive, charSize);

  for (int i = 0; i < length; ++i)
  {
  archive.read(reinterpret_cast <char*> (&t), sizeof(QChar));
  s += t;
  }

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (QDateTime& dt)
{
  QString q;
  QChar t;
  int charSize;
  long length = readStringLength(archive, charSize);

  for (int i = 0; i < length; ++i)
  {
  archive.read(reinterpret_cast <char*> (&t), sizeof(QChar));
  q += t;
  }

  dt = QDateTime::fromString(q);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  bool& /*b*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  quint8& /*b*/)
{
  assert(false);

  return *this;
}

#ifndef WIN32
CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  wchar_t& /*wc*/)
{
  assert(false);

  return *this;
}
#endif  // WIN32

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  quint16& /*w*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  short& /*w*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  qint64& /*ll*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  quint64& /*ull*/)
{
  assert(false);

  return *this;
}

CNewArchiveStdStringStream& CNewArchiveStdStringStream::operator >> (
  float& /*f*/)
{
  assert(false);

  return *this;
}
