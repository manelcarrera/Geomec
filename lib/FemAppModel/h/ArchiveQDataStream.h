#ifndef _ArchiveQDataStream_h_
#define _ArchiveQDataStream_h_

#include <QDataStream>

#include "ArchiveInterface.h"

class CArchiveQDataStream : public CArchiveInterface
{
public:
  CArchiveQDataStream(QIODevice* device, bool bIsHDF5File = false);
  ~CArchiveQDataStream();

// Attributes

  virtual bool IsLoading() const;
  virtual bool IsStoring() const;
  virtual bool Exists() const;

// DataSets

  virtual bool IsHDF5() const;
  virtual CHDF5File *GetHDF5File() const;
  virtual bool PushDataSet(const QString& dataSet, int hint = 0);
  virtual bool PopDataSet();

// Position

  virtual quint64 GetPosition();
  virtual void SetPosition(quint64 position);

// Operations

  virtual void Flush();
  virtual void Close();
  virtual void Abort();

// Insertion

  virtual CArchiveQDataStream& operator << (char c);
  virtual CArchiveQDataStream& operator << (int i);
  virtual CArchiveQDataStream& operator << (unsigned int ui);
  virtual CArchiveQDataStream& operator << (long l);
  virtual CArchiveQDataStream& operator << (unsigned long ul);
  virtual CArchiveQDataStream& operator << (double d);
  virtual CArchiveQDataStream& operator << (const QString& s);
  virtual CArchiveQDataStream& operator << (const QDateTime& dt);

// Extraction

  virtual CArchiveQDataStream& operator >> (char& c);
  virtual CArchiveQDataStream& operator >> (int& i);
  virtual CArchiveQDataStream& operator >> (unsigned int& ui);
  virtual CArchiveQDataStream& operator >> (long& l);
  virtual CArchiveQDataStream& operator >> (unsigned long& ul);
  virtual CArchiveQDataStream& operator >> (double& d);
  virtual CArchiveQDataStream& operator >> (QString& s);
  virtual CArchiveQDataStream& operator >> (QDateTime& dt);

// Compression
  virtual CArchiveQDataStream& CreateCompressedStream();
  virtual void DestroyCompressedStream();
  virtual bool IsCompressed() const;

private:
  QDataStream archive;
  CArchiveQDataStream* m_pCompressedStream;

  bool m_bIsHDF5;
};

#endif  // _ArchiveQDataStream_h_
