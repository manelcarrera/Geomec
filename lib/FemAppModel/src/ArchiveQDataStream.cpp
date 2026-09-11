#include "qtiocompressor.h"
#include <QFile>

#include "ArchiveQDataStream.h"
#include "HDF5File.h"

CArchiveQDataStream::CArchiveQDataStream(QIODevice *device, bool bIsHDF5File)
    : archive(device), m_pCompressedStream(0), m_bIsHDF5(bIsHDF5File || dynamic_cast<CHDF5File *>(device)) {}

CArchiveQDataStream::~CArchiveQDataStream() {
  if (m_pCompressedStream)
    DestroyCompressedStream();
}

bool CArchiveQDataStream::IsLoading() const { return archive.device()->isReadable(); }

bool CArchiveQDataStream::IsStoring() const { return archive.device()->isWritable(); }

bool CArchiveQDataStream::Exists() const { return IsHDF5() || static_cast<QFile *>(archive.device())->exists(); }

bool CArchiveQDataStream::IsHDF5() const { return m_bIsHDF5; }

CHDF5File *CArchiveQDataStream::GetHDF5File() const {
  assert(IsHDF5());
  return static_cast<CHDF5File *>(archive.device());
}

bool CArchiveQDataStream::PushDataSet(const QString &dataSet, int hint) {
  if (IsHDF5())
    return static_cast<CHDF5File *>(archive.device())->PushDataSet(dataSet, hint);
  return true;
}

bool CArchiveQDataStream::PopDataSet() {
  if (IsHDF5())
    return static_cast<CHDF5File *>(archive.device())->PopDataSet();
  return true;
}

quint64 CArchiveQDataStream::GetPosition() { return archive.device()->pos(); }

void CArchiveQDataStream::SetPosition(quint64 position) { archive.device()->seek(position); }

void CArchiveQDataStream::Flush() {}

void CArchiveQDataStream::Close() { archive.device()->close(); }

void CArchiveQDataStream::Abort() { archive.device()->close(); }

// write -----------------------------------------------------------------------

CArchiveQDataStream &CArchiveQDataStream::operator<<(char c) {
  assert(sizeof(char) == sizeof(qint8));

  qint8 q = c;
  archive << q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(int i) {
  assert(sizeof(int) == sizeof(qint32));

  qint32 q = i;
  archive << q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(unsigned int ui) {
  assert(sizeof(unsigned int) == sizeof(quint32));

  quint32 q = ui;
  archive << q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(long l) {
  qint32 q = l;
  archive << q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(unsigned long ul) {
  quint32 q = ul;
  archive << q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(double d) {
  archive << d;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(const QString &s) {
  archive << s;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator<<(const QDateTime &dt) {
  archive << dt;

  return *this;
}

// read ------------------------------------------------------------------------

CArchiveQDataStream &CArchiveQDataStream::operator>>(char &c) {
  assert(sizeof(char) == sizeof(qint8));

  qint8 q;
  archive >> q;
  c = q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(int &i) {
  assert(sizeof(int) == sizeof(qint32));

  qint32 q;
  archive >> q;
  i = q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(unsigned int &ui) {
  assert(sizeof(unsigned int) == sizeof(quint32));

  quint32 q;
  archive >> q;
  ui = q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(long &l) {
  qint32 q;
  archive >> q;
  l = q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(unsigned long &ul) {
  quint32 q;
  archive >> q;
  ul = q;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(double &d) {
  archive >> d;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(QString &s) {
  archive >> s;

  return *this;
}

CArchiveQDataStream &CArchiveQDataStream::operator>>(QDateTime &dt) {
  archive >> dt;

  return *this;
}

// Compression
CArchiveQDataStream &CArchiveQDataStream::CreateCompressedStream() {
  if (m_bIsHDF5)
    return *this;

  assert(!m_pCompressedStream);
  assert(archive.device());
  if (dynamic_cast<QtIOCompressor *>(archive.device())) // already a compressed stream?
    return *this;

  m_pCompressedStream = new CArchiveQDataStream(new QtIOCompressor(archive.device()), m_bIsHDF5);
  m_pCompressedStream->archive.device()->open(archive.device()->openMode() | QIODevice::Append);
  return *m_pCompressedStream;
}

void CArchiveQDataStream::DestroyCompressedStream() {
  if (m_pCompressedStream) {
    delete m_pCompressedStream->archive.device();
    delete m_pCompressedStream;
    m_pCompressedStream = 0;
  }
}

bool CArchiveQDataStream::IsCompressed() const { return (dynamic_cast<QtIOCompressor *>(archive.device()) != 0); }
