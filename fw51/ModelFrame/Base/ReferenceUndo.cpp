 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ReferenceUndo.h"
#include "IModelObject.h"

/*!
  \class CReferenceUndo
  \brief Undo class for restoring links between document items.

  The class restores both links being created as well as removed.
*/

/*!
  Constructs the reference removal undo operation.
  \param location is the document item whose state is affected.
  \param reference is the document item to which the link is affected.
  \param index is the index of the link being removed.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CReferenceUndo::CReferenceUndo( IModelObject& location, const IModelObject& reference, int index, const QString& text )
: CLocalUndo( location, text ),
  m_index( index ),
  m_remove( true ),
  m_referenceLocation( reference )
{
}

/*!
  Constructs the reference creation undo operation.
  \param location is the document item whose state is affected.
  \param reference is the document item to which the link is affected.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CReferenceUndo::CReferenceUndo( IModelObject& location, const IModelObject& reference, const QString& text )
: CLocalUndo( location, text ),
  m_index( -1 ),
  m_remove( false ),
  m_referenceLocation( reference )
{
}

/*!
  Destroys the object and frees any allocated resources.
*/
CReferenceUndo::~CReferenceUndo()
{
}

/*!
  Removes or recreates a link between the two document items.
*/
void CReferenceUndo::undo( CDocumentBase& document )
{
  IModelObject& object = location( document );
  IModelObject& reference = *m_referenceLocation.getObject( document );

  if ( m_remove ) {
  // undo remove
  const IModelObject* before = 0;
  if ( m_index < object.referenceSize() )
      before = &object.referenceAt( m_index );
  object.insertReference( reference, before );
  } else {
  // undo insert
  object.removeReference( reference );
  }
}
