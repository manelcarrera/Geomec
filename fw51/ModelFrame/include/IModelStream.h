 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef MODEL_STREAM_H
#define MODEL_STREAM_H

#include <QVectorIterator>
#include <qstring.h>
#include "IModelObject.h"

#include "ModelFrameExports.h"
class NumRange;

class MODELFRAME_EXPORT  IModelStream
{
public:
	IModelStream() {}
	virtual ~IModelStream() {}

	virtual IModelStream& operator>> ( qint8 & i ) = 0;
	virtual IModelStream& operator>> ( quint8 & i ) = 0;
	virtual IModelStream& operator>> ( qint16 & i ) = 0;
	virtual IModelStream& operator>> ( quint16 & i ) = 0;
	virtual IModelStream& operator>> ( qint32 & i ) = 0;
	virtual IModelStream& operator>> ( quint32 & i ) = 0;
	virtual IModelStream& operator>> ( qint64 & i ) = 0;
	virtual IModelStream& operator>> ( quint64 & i ) = 0;
#if !defined(Q_OS_WIN64)
  virtual IModelStream& operator>> ( long & i ) = 0;
  virtual IModelStream& operator>> ( ulong & i ) = 0;
#endif
	virtual IModelStream& operator>> ( float & f ) = 0;
	virtual IModelStream& operator>> ( double & f ) = 0;
	virtual IModelStream& operator>> ( char *& s ) = 0;
	virtual IModelStream& operator>> ( QString & string ) = 0;
	virtual IModelStream& operator>> ( QByteArray & string ) = 0;
  virtual IModelStream& operator>> ( QVariant& v ) = 0;
  virtual IModelStream& operator>> ( TFtnDoubleVec& vec ) = 0;
  virtual IModelStream& operator>> ( TFtnIntVec& vec ) = 0;
  virtual IModelStream& operator>> ( NumRange& range ) = 0;

	virtual IModelStream& operator<< ( qint8 i ) = 0;
	virtual IModelStream& operator<< ( quint8 i ) = 0;
	virtual IModelStream& operator<< ( qint16 i ) = 0;
	virtual IModelStream& operator<< ( quint16 i ) = 0;
  virtual IModelStream& operator<< ( qint32 i ) = 0;
	virtual IModelStream& operator<< ( quint32 i ) = 0;
	virtual IModelStream& operator<< ( qint64 i ) = 0;
	virtual IModelStream& operator<< ( quint64 i ) = 0;
#if !defined(Q_OS_WIN64)
	virtual IModelStream& operator<< ( long i ) = 0;
	virtual IModelStream& operator<< ( ulong i ) = 0;
#endif
	virtual IModelStream& operator<< ( float f ) = 0;
	virtual IModelStream& operator<< ( double f ) = 0;
	virtual IModelStream& operator<< ( const char * s ) = 0;
	virtual IModelStream& operator<< ( const QString& string ) = 0;
	virtual IModelStream& operator<< ( const QByteArray& string ) = 0;
  virtual IModelStream& operator<< ( const QVariant& v ) = 0;
  virtual IModelStream& operator<< ( const TFtnDoubleVec& vec ) = 0;
  virtual IModelStream& operator<< ( const TFtnIntVec& vec ) = 0;
  virtual IModelStream& operator<< ( const NumRange& range ) = 0;

	virtual IModelStream& readRawBytes(char* s, uint len) = 0;
	virtual IModelStream& writeRawBytes(const char* s, uint len) = 0;

  IModelObject* getModel(){ assert( m_currentModel); return m_currentModel; }
  void setModel(IModelObject* model) { m_currentModel = model; }

private:
  IModelObject* m_currentModel;
};

class MODELFRAME_EXPORT  CDummyStream : public IModelStream
{
public:
	CDummyStream() {}

	virtual IModelStream& operator>> ( qint8 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( quint8 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( qint16 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( quint16 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( qint32 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( quint32 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( qint64 & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( quint64 & /*i*/ ) { return *this; }
#if !defined(Q_OS_WIN64)
	virtual IModelStream& operator>> ( long & /*i*/ ) { return *this; }
	virtual IModelStream& operator>> ( ulong & /*i*/ ) { return *this; }
#endif
	virtual IModelStream& operator>> ( float & /*f*/ ) { return *this; }
	virtual IModelStream& operator>> ( double & /*f*/ ) { return *this; }
	virtual IModelStream& operator>> ( char *& /*s*/ ) { return *this; }
	virtual IModelStream& operator>> ( QString & /*string*/ ) { return *this; }
	virtual IModelStream& operator>> ( QByteArray & /*string*/ ) { return *this; }
  virtual IModelStream& operator>> ( QVariant& /*v*/ ) { return *this; }
  virtual IModelStream& operator>> ( TFtnDoubleVec& ) { return *this; }
  virtual IModelStream& operator>> ( TFtnIntVec& /*vec*/ ) { return *this; }
  virtual IModelStream& operator>> ( NumRange& /*range*/ ) { return *this; }

	virtual IModelStream& operator<< ( qint8 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( quint8 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( qint16 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( quint16 /*i*/ ) { return *this; }
  virtual IModelStream& operator<< ( qint32 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( quint32 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( qint64 /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( quint64 /*i*/ ) { return *this; }
#if !defined(Q_OS_WIN64)
	virtual IModelStream& operator<< ( long /*i*/ ) { return *this; }
	virtual IModelStream& operator<< ( ulong /*i*/ ) { return *this; }
#endif
	virtual IModelStream& operator<< ( float /*f*/ ) { return *this; }
	virtual IModelStream& operator<< ( double /*f*/ ) { return *this; }
	virtual IModelStream& operator<< ( const char * /*s*/ ) { return *this; }
	virtual IModelStream& operator<< ( const QString& /*string*/ ) { return *this; }
	virtual IModelStream& operator<< ( const QByteArray& /*string*/ ) { return *this; }
  virtual IModelStream& operator<< ( const QVariant& /*v*/ ) { return *this; }
  virtual IModelStream& operator<< ( const TFtnDoubleVec& ) { return *this; }
  virtual IModelStream& operator<<( const TFtnIntVec& /*vec*/ ) { return *this; }
  virtual IModelStream& operator<< ( const NumRange& /*range*/ ) { return *this; }

	virtual IModelStream& readRawBytes(char* /*s*/, uint /*len*/) { return *this; }
	virtual IModelStream& writeRawBytes(const char* /*s*/, uint /*len*/) { return *this; }
};

//! general template to store QList<> objects
template<class T>
IModelStream& operator<< (IModelStream& stream, const QList<T>& list)
{
  stream << list.size();
  QListIterator<T> iter (list);
  while (iter.hasNext())
  {
    stream << iter.next();
  }
  return stream;
}

//! general template to restore QList<> objects
template<class T>
IModelStream& operator>> (IModelStream& stream, QList<T>& list)
{
  int size = 0;
  stream >> size;

  list.clear();
  while (--size >= 0)
  {
    T item;
    stream >> item;
    list << item;
  }
  return stream;
}

//! general template to store QVector<> objects
template<class T>
IModelStream& operator<< (IModelStream& stream, const QVector<T>& vec)
{
  stream << vec.size();
  QVectorIterator<T> iter (vec);
  while (iter.hasNext())
  {
    stream << iter.next();
  }
  return stream;
}

//! general template to restore QVector<> objects
template<class T>
IModelStream& operator>> (IModelStream& stream, QVector<T>& vec)
{
  int size = 0;
  stream >> size;

  vec.clear();
  if (size > 0)
  {
    vec.reserve (size);
    while (--size >= 0)
    {
      T item;
      stream >> item;
      vec << item;
    }
  }
  return stream;
}

#endif
