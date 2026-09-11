/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "QtDataStream.h"
#include "dimple.h"
#include <QVariant>
#include <qdatastream.h>

#include "NumRange.h"

CQtDataStream::CQtDataStream(QDataStream &stream) : m_stream(stream) {}

IModelStream &CQtDataStream::operator>>(qint8 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(quint8 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(qint16 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(quint16 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(qint32 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(quint32 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(qint64 &i) {
  m_stream >> i;
  return *this;
}

IModelStream &CQtDataStream::operator>>(quint64 &i) {
  m_stream >> i;
  return *this;
}

#if !defined(Q_OS_WIN64)

// QT4_REVIEW streaming longs - I think this should duplicate the old
// behaviour.  It seems like a bit of a platform-specific thing to be doing
// anyway, so it might be better to get rid of it if possible.
//
// If it stays, it would arguably be better to do a compile-time check for
// the size of longs - although in theory the optimizer should be able to
// completely optimize out the conditional anyway.

IModelStream &CQtDataStream::operator>>(long &i) {
  if (sizeof(long) == 8) {
    qint64 t;
    m_stream >> t;
    i = long(t);
  } else if (sizeof(long) == 4) {
    qint32 t;
    m_stream >> t;
    i = long(t);
  } else {
    qFatal("can't cope with this word size");
  }
  return *this;
}

IModelStream &CQtDataStream::operator>>(ulong &i) {
  if (sizeof(ulong) == 8) {
    quint64 t;
    m_stream >> t;
    i = ulong(t);
  } else if (sizeof(long) == 4) {
    quint32 t;
    m_stream >> t;
    i = ulong(t);
  } else {
    qFatal("can't cope with this word size");
  }
  return *this;
}

#endif

IModelStream &CQtDataStream::operator>>(float &f) {
  m_stream >> f;
  return *this;
}

IModelStream &CQtDataStream::operator>>(double &f) {
  m_stream >> f;
  return *this;
}

IModelStream &CQtDataStream::operator>>(char *&s) {
  m_stream >> s;
  return *this;
}

IModelStream &CQtDataStream::operator>>(QString &string) {
  m_stream >> string;
  return *this;
}

IModelStream &CQtDataStream::operator>>(QByteArray &string) {
  QString s;
  m_stream >> s;
  string = s.toLatin1();
  return *this;
}

IModelStream &CQtDataStream::operator>>(QVariant &v) {
  m_stream >> v;
  return *this;
}

//! Deserializes a TFtnDoubleVec from an IModelStream
//! @param [out] vec TFtnDoubleVec to read from the stream
//! @returns the IModelStream read from
IModelStream &CQtDataStream::operator>>(TFtnDoubleVec &vec) {
  int size;
  m_stream >> size;
  vec.resize(size);
  for (int i = 0; i < size; ++i) {
    m_stream >> vec[i];
  }
  return *this;
}

//! Deserializes a TFtnIntVec from an IModelStream
//! @param [out] vec TFtnIntVec to read from the stream
//! @returns the IModelStream read from
IModelStream &CQtDataStream::operator>>(TFtnIntVec &vec) {
  int size;
  m_stream >> size;
  vec.resize(size);
  for (int i = 0; i < size; ++i) {
    this->operator>>(vec[i]);
  }
  return *this;
}

//! Deserializes an NumRange from an IModelStream
//! @param [out] range Range to read from the stream
//! @returns the IModelStream read from
IModelStream &CQtDataStream::operator>>(NumRange &range) {
  QString expr;
  m_stream >> expr;
  range = NumRange(expr);
  return *this;
}

IModelStream &CQtDataStream::operator<<(float f) {
  m_stream << f;
  return *this;
}

IModelStream &CQtDataStream::operator<<(double f) {
  m_stream << f;
  return *this;
}

IModelStream &CQtDataStream::operator<<(const char *s) {
  m_stream << s;
  return *this;
}

IModelStream &CQtDataStream::operator<<(const QString &string) {
  m_stream << string;
  return *this;
}

IModelStream &CQtDataStream::operator<<(const QVariant &v) {
  m_stream << v;
  return *this;
}

IModelStream &CQtDataStream::operator<<(const QByteArray &string) {
  QString s(string);
  m_stream << s;
  return *this;
}

IModelStream &CQtDataStream::operator<<(qint8 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(quint8 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(qint16 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(quint16 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(qint32 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(quint32 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(qint64 i) {
  m_stream << i;
  return *this;
}

IModelStream &CQtDataStream::operator<<(quint64 i) {
  m_stream << i;
  return *this;
}

#if !defined(Q_OS_WIN64)

// QT4_REVIEW see above

IModelStream &CQtDataStream::operator<<(long i) {
  if (sizeof(long) == 8)
    m_stream << qint64(i);
  else if (sizeof(long) == 4)
    m_stream << qint32(i);
  else
    qFatal("can't cope with this sizeof(long)");
  return *this;
}

IModelStream &CQtDataStream::operator<<(ulong i) {
  if (sizeof(long) == 8)
    m_stream << quint64(i);
  else if (sizeof(long) == 4)
    m_stream << quint32(i);
  else
    qFatal("can't cope with this sizeof(ulong)");
  return *this;
}

#endif

//! Serializes a TFtnDoubleVec to an IModelStream
//! @param [in] vec TFtnDoubleVec to write to the stream
//! @returns the IModelStream written to
IModelStream &CQtDataStream::operator<<(TFtnDoubleVec const &vec) {
  m_stream << (int)vec.size();
  for (int i = 0; i < vec.size(); ++i) {
    m_stream << vec[i];
  }
  return *this;
}

//! Serializes a TFtnIntVec to an IModelStream
//! @param [in] vec TFtnIntVec to write to the stream
//! @returns the IModelStream written to
IModelStream &CQtDataStream::operator<<(const TFtnIntVec &vec) {
  m_stream << (int)vec.size();
  for (int i = 0; i < vec.size(); ++i) {
    this->operator<<(vec[i]);
  }
  return *this;
}

//! Serializes a NumRange to an IModelStream
//! @param [in] range Range to write to the stream
//! @returns the IModelStream written to
IModelStream &CQtDataStream::operator<<(const NumRange &range) {
  m_stream << range.expression();
  return *this;
}

IModelStream &CQtDataStream::readRawBytes(char *s, uint len) {
  m_stream.readRawData(s, len);
  return *this;
}

IModelStream &CQtDataStream::writeRawBytes(const char *s, uint len) {
  m_stream.writeRawData(s, len);
  return *this;
}
