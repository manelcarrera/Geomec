 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "BufferUndo.h"
#include "QtDataStream.h"
#include <DocumentBase.h>
#include <QDataStream>
#include <cassert>

/*!
  \class CBufferUndo
  \brief Base class for all undo classes using a buffer to store previous state.

  Override undo(IModelStream&,CDocumentBase&) to implement specific state
  restoring actions using the bufer
*/

/*!
  Constructs the buffered undo operation.
  \param location is the document item whose state is affected.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CBufferUndo::CBufferUndo( IModelObject& location, const QString& text )
: CLocalUndo( location, text ),
  m_writeStream( 0 ),
  m_modelStream( 0 )
{
  m_bytes = new QByteArray;
}

/*!
  Destroys the object and frees any allocated resources.
*/
CBufferUndo::~CBufferUndo()
{
  if ( m_modelStream ) delete m_modelStream;
  if ( m_writeStream ) delete m_writeStream;
  if ( m_bytes ) delete m_bytes;
}

/*!
  Defers state restoration to specialized classes with
  undo(IModelStream&,CDocumentBase&).
*/
void CBufferUndo::undo( CDocumentBase& document )
{
  QDataStream readStream( m_bytes, QIODevice::ReadWrite );
  CQtDataStream modelStream( readStream );
  modelStream.setModel(&document);
  undo( modelStream, document );
}

/*!
  Returns a buffer for specialized classes to write to.
*/
IModelStream& CBufferUndo::buffer()
{
  if ( !m_modelStream ) {
  assert( !m_writeStream );
  m_writeStream = new QDataStream( m_bytes, QIODevice::ReadWrite );
  m_modelStream = new CQtDataStream( *m_writeStream );
  }
  return *m_modelStream;
}

/*!
  \fn virtual void CBufferUndo::undo( IModelStream&, CDocumentBase& ) = 0;
  Override to implement specific state restoring actions using the buffer.
*/
