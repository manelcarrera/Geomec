 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __FILE_MESSAGE_H
#define __FILE_MESSAGE_H

#include "ModelMessage.h"

#include "ModelFrameExports.h"

struct MODELFRAME_EXPORT TFilePosition {
  int lineNr;
  int pos;
  int field;
  QString line;
  
  TFilePosition() : lineNr( -1 ), pos( -1 ), field( -1 ), line() {}
  TFilePosition( int _pos ) { lineNr = -1; pos = _pos; field = -1; line = (const char*)0; }
  TFilePosition( int _lineNr, int _pos, int _field, const QString& _line ) : lineNr( _lineNr ), pos( _pos ), field( _field ), line( _line ) {}

  bool isNull() const { return ( lineNr == -1 && pos == -1 && field == -1 && line.isNull() ); }
};

struct MODELFRAME_EXPORT TFileMessage : public TModelMessage, public TFilePosition
{
  TFileMessage( const TModelMessage& msg, const TFilePosition& pos ) : TModelMessage( msg ), TFilePosition( pos ) {}
  TFileMessage( const QString& msg, int lineNr, int pos, int field, const QString& line ) : TModelMessage( msg ), TFilePosition( lineNr, pos, field, line ) {}
};

struct MODELFRAME_EXPORT TFileError : public TModelError, public TFilePosition
{
  TFileError( const TModelError& msg, const TFilePosition& pos ) : TModelError( msg ), TFilePosition( pos ) {}
  TFileError( const QString& msg, int errorNo ) : TModelError( msg, errorNo ) {}
  TFileError( const QString& msg, int errorNo, int lineNr, int pos, int field, const QString& line ) : TModelError( msg, errorNo ), TFilePosition( lineNr, pos, field, line ) {}
};

#endif // __FILE_MESSAGE_H
