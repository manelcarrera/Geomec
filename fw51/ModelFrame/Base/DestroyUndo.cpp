 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "DestroyUndo.h"
#include "IProgressBase.h"
#include "DocumentBase.h"
#include "StreamVersion.h"
#include "IModelObjectFactory.h"
#include <cassert>

/*!
  \class CDestroyUndo
  \brief Undo class for destruction of document items.
*/

/*!
  Constructs the destroy undo operation.
  \param parentLocation is the document item whose child is removed.
  \param object is the object being destroyed.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CDestroyUndo::CDestroyUndo( IModelObject& parentLocation, IModelObject& object, const QString& text )
: CBufferUndo( parentLocation, text ),
  m_index( -1 )
{
  for ( int i = 0; i < parentLocation.childSize(); ++i )
  if ( &object == &parentLocation.childAt( i ) ) {
      m_index = i;
      break;
  }
  assert( m_index != -1 );
  m_className = object.metaObject()->className();
  IProgressBase progress;
  object.store( buffer(), progress );
}

/*!
  Destroys the object and frees any allocated resources.
*/
CDestroyUndo::~CDestroyUndo()
{
}

/*!
  Recreates a document item at the same spot it used to be, restoring its
  state using buffer() with IModelObject::restore().
*/
void CDestroyUndo::undo( IModelStream& stream, CDocumentBase& document )
{
  IModelObject& parentLocation = location( document );

  IModelObject* object = IModelObjectFactory::create( m_className );
  assert( object );

  IModelObject* insertBefore = 0;
  if ( m_index < parentLocation.childSize() )
  insertBefore = &parentLocation.childAt( m_index );

  //object->create( &parentLocation, insertBefore );

  IProgressBase progress;
  object->restore( stream, document.documentVersion(), progress );

  object->create( &parentLocation, insertBefore );
}

