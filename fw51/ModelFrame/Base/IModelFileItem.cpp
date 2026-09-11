 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IModelFileItem.h"
#include "IModelFile.h"

/*!
  Constructor.
  Note that all objects with parent == 0 are put in a global list,
  and are removed from that list on destruction time, which will cause a
  performance degradation.
 */
IModelFileItem::IModelFileItem( QObject* parent )
 : QObject( parent )
{
}

/*!
  Gives a message to the user. This message is not related to any error or
  warning.
  The message will be extended with position information and line contents if
  needed.
*/
void IModelFileItem::putStatusMessage( const QString&       message,
                                       const TFilePosition& info )
{
  if ( info.isNull() )
    modelFile().putStatusMessage( TFileMessage( message, *this ) );
  else
    modelFile().putStatusMessage( TFileMessage( message, info  ) );
}

/*!
  Gives a warning to the user. The code execution continues.
  The message will be extended with position information and line contents if
  needed.
*/
void IModelFileItem::putWarningMessage( int                  errorNo,
                                        const QString&       message,
                                        const TFilePosition& info )
{
  TModelError modelError( message, errorNo );
  if ( info.isNull() )
    modelFile().putWarningMessage( TFileError( modelError, *this ) );
  else
    modelFile().putWarningMessage( TFileError( modelError, info  ) );
}

/*!
  Gives a fatal error to the user. A TFatalException is thrown. It must be
  handled somewhere in the implementation of read(). Execution can continue on
  the next record.
  The message will be extended with position information and line contents if
  needed.
*/
void IModelFileItem::putFatalMessage( int                  errorNo,
                                      const QString&       message,
                                      const TFilePosition& info )
{
  TModelError modelError( message, errorNo );
  if ( info.isNull() )
    modelFile().putFatalMessage( TFileError( modelError, *this ) );
  else
    modelFile().putFatalMessage( TFileError( modelError, info  ) );
}

/*!
  Gives an abort error to the user. A TAbortException is thrown. It must be
  handled somewhere in the implementation of read(). Execution must be aborted.
  The message will be extended with position information and line contents if
  needed.
*/
void IModelFileItem::putAbortMessage( int                  errorNo,
                                      const QString&       message,
                                      const TFilePosition& info )
{
  TModelError modelError( message, errorNo );
  if ( info.isNull() )
    modelFile().putAbortMessage( TFileError( modelError, *this ) );
  else
    modelFile().putAbortMessage( TFileError( modelError, info  ) );
}

/*!
  \fn IModelFile& IModelFileItem::modelFile() = 0

  Override to provide access to the toplevel IModelFile.
*/

/*!
  \fn IModelFileItem::operator TFilePosition() = 0

  Override to provide file position information for this item.
*/
