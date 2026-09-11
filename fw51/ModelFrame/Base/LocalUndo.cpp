 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "LocalUndo.h"
#include <cassert>

/*!
  \class CLocalUndo
  \brief Base class for all undo classes acting upon a specific document item.

  Override undo() to implement specific state restoring actions.
*/

/*!
  Constructs the local undo operation.
  \param location is the document item whose state is affected.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CLocalUndo::CLocalUndo( IModelObject& location, const QString& text )
: CUndoOperation( text ), m_documentLocation( location )
{
  assert( location.document() );
}

/*!
  Destroys the object and frees any allocated resources.
*/
CLocalUndo::~CLocalUndo()
{
}

/*!
  Returns the object whose state must be restored.
*/
IModelObject& CLocalUndo::location( CDocumentBase& document )
{
  return *m_documentLocation.getObject( document );
}
