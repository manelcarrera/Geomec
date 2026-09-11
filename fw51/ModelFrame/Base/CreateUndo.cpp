 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "CreateUndo.h"
#include "IModelObject.h"

/*!
  \class CCreateUndo
  \brief Undo class for creation of document items.
*/

/*!
  Constructs the create undo operation.
  \param location is the document item being created.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CCreateUndo::CCreateUndo( IModelObject& location, const QString& text )
: CLocalUndo( location, text )
{
}

/*!
  Destroys the object and frees any allocated resources.
*/
CCreateUndo::~CCreateUndo()
{
}

/*!
  Removes the document item.
*/
void CCreateUndo::undo( CDocumentBase& document )
{
  location( document ).destroy();
}

