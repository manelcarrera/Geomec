 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ColorUndo.h"
#include "IModelObject.h"

/*!
  \class CColorUndo
  \brief Undo class for color changes.
*/

/*!
  Constructs the color undo operation.
  \param location is the document item whose color changed.
  \param previous is the color being replaced.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CColorUndo::CColorUndo( IModelObject& location, const QRgb& previous, const QString& text )
: CLocalUndo( location, text ),
  m_previous( previous )
{
}

/*!
  Destroys the object and frees any allocated resources.
*/
CColorUndo::~CColorUndo()
{
}

/*!
  Restores the color of the document item.
*/
void CColorUndo::undo( CDocumentBase& document )
{
  location( document ).color( m_previous );
}
