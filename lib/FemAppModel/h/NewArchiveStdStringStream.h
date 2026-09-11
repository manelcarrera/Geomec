#ifndef _NewArchiveStdStringStream_h_
#define _NewArchiveStdStringStream_h_

#include <fstream>
#include <sstream>

#include "ArchiveInterface.h"

class CNewArchiveStdStringStream : public CArchiveInterface {
public:
  CNewArchiveStdStringStream(std::ios_base::openmode mode);
  ~CNewArchiveStdStringStream();

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

  // Insertion

  virtual CNewArchiveStdStringStream &operator<<(char c);
  virtual CNewArchiveStdStringStream &operator<<(int i);
  virtual CNewArchiveStdStringStream &operator<<(unsigned int ui);
  virtual CNewArchiveStdStringStream &operator<<(long l);
  virtual CNewArchiveStdStringStream &operator<<(unsigned long ul);
  virtual CNewArchiveStdStringStream &operator<<(double d);
  virtual CNewArchiveStdStringStream &operator<<(const QString &s);
  virtual CNewArchiveStdStringStream &operator<<(const QDateTime &dt);

  virtual CNewArchiveStdStringStream &operator<<(bool b);
  virtual CNewArchiveStdStringStream &operator<<(quint8 b);
#ifndef WIN32
  virtual CNewArchiveStdStringStream &operator<<(wchar_t wc);
#endif // WIN32
  virtual CNewArchiveStdStringStream &operator<<(quint16 w);
  virtual CNewArchiveStdStringStream &operator<<(short w);
  virtual CNewArchiveStdStringStream &operator<<(qint64 ll);
  virtual CNewArchiveStdStringStream &operator<<(quint64 ull);
  virtual CNewArchiveStdStringStream &operator<<(float f);

  // Extraction

  virtual CNewArchiveStdStringStream &operator>>(char &c);
  virtual CNewArchiveStdStringStream &operator>>(int &i);
  virtual CNewArchiveStdStringStream &operator>>(unsigned int &ui);
  virtual CNewArchiveStdStringStream &operator>>(long &l);
  virtual CNewArchiveStdStringStream &operator>>(unsigned long &ul);
  virtual CNewArchiveStdStringStream &operator>>(double &d);
  virtual CNewArchiveStdStringStream &operator>>(QString &s);
  virtual CNewArchiveStdStringStream &operator>>(QDateTime &dt);

  virtual CNewArchiveStdStringStream &operator>>(bool &b);
  virtual CNewArchiveStdStringStream &operator>>(quint8 &b);
#ifndef WIN32
  virtual CNewArchiveStdStringStream &operator>>(wchar_t &wc);
#endif // WIN32
  virtual CNewArchiveStdStringStream &operator>>(quint16 &w);
  virtual CNewArchiveStdStringStream &operator>>(short &w);
  virtual CNewArchiveStdStringStream &operator>>(qint64 &ll);
  virtual CNewArchiveStdStringStream &operator>>(quint64 &ull);
  virtual CNewArchiveStdStringStream &operator>>(float &f);

private:
  std::stringstream archive;
  std::ios_base::openmode mode;
};

#endif // _NewArchiveStdStringStream_h_
