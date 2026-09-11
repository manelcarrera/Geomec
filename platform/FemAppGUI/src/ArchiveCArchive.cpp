#include "stdafx.h"

#include "ArchiveCArchive.h"

CArchiveCArchive::CArchiveCArchive(const QString &filename) : m_archive(0) {
  if (m_file.Open(filename.toStdString().c_str(), CFile::modeRead)) {
    m_archive = new CArchive(&m_file, CArchive::load);
  }
}

CArchiveCArchive::~CArchiveCArchive() {
  if (m_archive) {
    m_archive->Close();
    delete m_archive;
    m_file.Close();
  }
}

bool CArchiveCArchive::IsLoading() const { return m_archive && m_archive->IsLoading(); }

bool CArchiveCArchive::IsStoring() const { return m_archive && m_archive->IsStoring(); }

bool CArchiveCArchive::Exists() const {
  CFileStatus status;
  return m_file.GetStatus(status);
}

quint64 CArchiveCArchive::GetPosition() { return m_archive->GetFile()->GetPosition(); }

void CArchiveCArchive::SetPosition(quint64 position) { m_archive->GetFile()->Seek(position, CFile::begin); }

void CArchiveCArchive::Flush() { m_archive->Flush(); }

void CArchiveCArchive::Close() { m_archive->Close(); }

void CArchiveCArchive::Abort() { m_archive->Abort(); }

// write -----------------------------------------------------------------------

CArchiveCArchive &CArchiveCArchive::operator<<(char c) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(int i) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(unsigned int ui) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(long l) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(unsigned long ul) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(double d) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(const CString &s) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(const COleDateTime &dt) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(const QString &s) {
  assert(false);

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator<<(const QDateTime &dt) {
  assert(false);

  return *this;
}

// read ------------------------------------------------------------------------

CArchiveCArchive &CArchiveCArchive::operator>>(char &c) {
  *m_archive >> c;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(int &i) {
  *m_archive >> i;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(unsigned int &ui) {
  *m_archive >> ui;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(long &l) {
  *m_archive >> l;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(unsigned long &ul) {
  *m_archive >> ul;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(double &d) {
  *m_archive >> d;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(CString &s) {
  *m_archive >> s;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(COleDateTime &dt) {
  *m_archive >> dt;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(QString &s) {
  CString cString;

  *m_archive >> cString;
  s = (LPCSTR)cString;

  return *this;
}

CArchiveCArchive &CArchiveCArchive::operator>>(QDateTime &dt) {
  COleDateTime oleDateTime;

  *m_archive >> oleDateTime;

  QDate date(oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay());
  QTime time(oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

  dt.setDate(date);
  dt.setTime(time);

  return *this;
}
