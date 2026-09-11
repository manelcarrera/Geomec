#ifndef _ArchiveCArchive_h_
#define _ArchiveCArchive_h_

#include "ArchiveInterface.h"


// class for backwards compatibility with .gmp format, reading only

class CArchiveCArchive : public CArchiveInterface
{
public:
  CArchiveCArchive(const QString& filename);
  ~CArchiveCArchive();

// Attributes

  virtual bool IsLoading() const;
  virtual bool IsStoring() const;
  virtual bool Exists() const;

// Position

  virtual quint64 GetPosition();
  virtual void SetPosition(quint64 position);

// Operations

  virtual void Flush();
  virtual void Close();
  virtual void Abort();

// Insertion (not supported anymore)

  virtual CArchiveCArchive& operator << (char c);
  virtual CArchiveCArchive& operator << (int i);
  virtual CArchiveCArchive& operator << (unsigned int ui);
  virtual CArchiveCArchive& operator << (long l);
  virtual CArchiveCArchive& operator << (unsigned long ul);
  virtual CArchiveCArchive& operator << (double d);
  virtual CArchiveCArchive& operator << (const CString& s);
  virtual CArchiveCArchive& operator << (const COleDateTime& dt);
  virtual CArchiveCArchive& operator << (const QString& s);
  virtual CArchiveCArchive& operator << (const QDateTime& dt);

// Extraction

  virtual CArchiveCArchive& operator >> (char& c);
  virtual CArchiveCArchive& operator >> (int& i);
  virtual CArchiveCArchive& operator >> (unsigned int& ui);
  virtual CArchiveCArchive& operator >> (long& l);
  virtual CArchiveCArchive& operator >> (unsigned long& ul);
  virtual CArchiveCArchive& operator >> (double& d);
  virtual CArchiveCArchive& operator >> (CString& s);
  virtual CArchiveCArchive& operator >> (COleDateTime& dt);
  virtual CArchiveCArchive& operator >> (QString& s);
  virtual CArchiveCArchive& operator >> (QDateTime& dt);

private:
  CArchive *m_archive;
  CFile m_file;
};

#endif  // _ArchiveCArchive_h_
