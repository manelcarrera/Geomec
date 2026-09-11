 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_MESSAGE_H
#define __MODEL_MESSAGE_H

#include "dimple.h"
#include <qstring.h>

#include "ModelFrameExports.h"

struct MODELFRAME_EXPORT TModelMessage
{
  QString text;
  QString url;

  TModelMessage( const QString& _text, const QString& _url = 0 );
};

struct MODELFRAME_EXPORT TModelError : public TModelMessage
{
  int error;

  TModelError( const QString& text, const QString& url, int _error );
  TModelError( const QString& text, int _error = -1 );
};

#endif // __MODEL_MESSAGE_H
