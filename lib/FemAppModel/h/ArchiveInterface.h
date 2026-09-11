#ifndef ArchiveInterface_h
#define ArchiveInterface_h

#include <cassert>
#include <QDateTime>

class CHDF5File;

class CArchiveInterface
{
public:
  enum Mode {store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4};

  virtual ~CArchiveInterface() = 0;

// Attributes

  virtual bool IsLoading() const = 0;
  virtual bool IsStoring() const = 0;
  virtual bool Exists() const = 0;

// DataSets

  virtual bool IsHDF5() const;
  virtual CHDF5File *GetHDF5File() const;
  virtual bool PushDataSet(const QString& dataSet, int hint = 0);
  virtual bool PopDataSet();

// Position

  virtual quint64 GetPosition() = 0;
  virtual void SetPosition(quint64 position) = 0;

// Operations

  virtual void Flush() = 0;
  virtual void Close() = 0;
  virtual void Abort() = 0;

// Insertion

  virtual CArchiveInterface& operator << (char c) = 0;
  virtual CArchiveInterface& operator << (int i) = 0;
  virtual CArchiveInterface& operator << (unsigned int ui) = 0;
  virtual CArchiveInterface& operator << (long l) = 0;
  virtual CArchiveInterface& operator << (unsigned long ul) = 0;
#ifdef _WIN32
  // accommodating int -> size_t conversions; TODO: make true 64-bit:
  virtual CArchiveInterface& operator << (std::size_t s) { assert(s >> 32 == 0); return operator<<(static_cast<unsigned long>(s)); }
#endif
  virtual CArchiveInterface& operator << (double d) = 0;
  virtual CArchiveInterface& operator << (const QString& s) = 0;
  virtual CArchiveInterface& operator << (const QDateTime& dt) = 0;

// Extraction

  virtual CArchiveInterface& operator >> (char& c) = 0;
  virtual CArchiveInterface& operator >> (int& i) = 0;
  virtual CArchiveInterface& operator >> (unsigned int& ui) = 0;
  virtual CArchiveInterface& operator >> (long& l) = 0;
  virtual CArchiveInterface& operator >> (unsigned long& ul) = 0;
  virtual CArchiveInterface& operator >> (double& d) = 0;
  virtual CArchiveInterface& operator >> (QString& s) = 0;
  virtual CArchiveInterface& operator >> (QDateTime& dt) = 0;

// Compression
  virtual CArchiveInterface& CreateCompressedStream();
  virtual void DestroyCompressedStream();
  virtual bool IsCompressed() const;
};

#endif  // ArchiveInterface_h
