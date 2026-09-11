/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "TextUndo.h"
#include "IModelObject.h"

/*!
  \class CTextUndo
  \brief Undo class for IModelObject::setText changes.
*/

/*!
  Constructs the text undo operation.
  \param location is the document item whose text is changed.
  \param previous is the replaced text.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CTextUndo::CTextUndo(IModelObject &location, const QString &previous, const QString &text)
    : CLocalUndo(location, text), m_previous(previous) {}

/*!
  Destroys the object and frees any allocated resources.
*/
CTextUndo::~CTextUndo() {}

/*!
  Resets the text of the document item.
*/
void CTextUndo::undo(CDocumentBase &document) { location(document).setText(m_previous); }
