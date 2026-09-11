 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef QT_DATA_STREAM_H
#define QT_DATA_STREAM_H

#include "IModelStream.h"
//Added by qt3to4:
#include <QByteArray>

#include "ModelFrameExports.h"

class QDataStream;
class MODELFRAME_EXPORT  CQtDataStream : public IModelStream
{
  QDataStream& m_stream;
public:
  CQtDataStream(QDataStream& stream);

  virtual IModelStream& operator>> ( qint8 & i );
  virtual IModelStream& operator>> ( quint8 & i );
  virtual IModelStream& operator>> ( qint16 & i );
  virtual IModelStream& operator>> ( quint16 & i );
  virtual IModelStream& operator>> ( qint32 & i );
  virtual IModelStream& operator>> ( quint32 & i );
  virtual IModelStream& operator>> ( qint64 & i );
  virtual IModelStream& operator>> ( quint64 & i );
#if !defined(Q_OS_WIN64)
  virtual IModelStream& operator>> ( long & i );
  virtual IModelStream& operator>> ( ulong & i );
#endif
  virtual IModelStream& operator>> ( float & f );
  virtual IModelStream& operator>> ( double & f );
  virtual IModelStream& operator>> ( char *& s );
  virtual IModelStream& operator>> ( QString & string );
  virtual IModelStream& operator>> ( QByteArray & string );
  virtual IModelStream& operator>> ( QVariant& v );
  virtual IModelStream& operator>> ( TFtnDoubleVec& vec );
  virtual IModelStream& operator>> ( TFtnIntVec& vec );
  virtual IModelStream& operator>> ( NumRange& range );

  virtual IModelStream& operator<< ( qint8 i);
  virtual IModelStream& operator<< ( quint8 i );
  virtual IModelStream& operator<< ( qint16 i );
  virtual IModelStream& operator<< ( quint16 i );
  virtual IModelStream& operator<< ( qint32 i );
  virtual IModelStream& operator<< ( quint32 i );
  virtual IModelStream& operator<< ( qint64 i );
  virtual IModelStream& operator<< ( quint64 i );
#if !defined(Q_OS_WIN64)
  virtual IModelStream& operator<< ( long i );
  virtual IModelStream& operator<< ( ulong i );
#endif
  virtual IModelStream& operator<< ( float f );
  virtual IModelStream& operator<< ( double f );
  virtual IModelStream& operator<< ( const char * s );
  virtual IModelStream& operator<< ( const QString& string );
  virtual IModelStream& operator<< ( const QByteArray& string );
  virtual IModelStream& operator<< ( const QVariant& v );
  virtual IModelStream& operator<< ( const TFtnDoubleVec& vec );
  virtual IModelStream& operator<< ( const TFtnIntVec& vec );
  virtual IModelStream& operator<< ( const NumRange& range );

  virtual IModelStream& readRawBytes(char* s, uint len);
  virtual IModelStream& writeRawBytes(const char* s, uint len);
};

#endif
