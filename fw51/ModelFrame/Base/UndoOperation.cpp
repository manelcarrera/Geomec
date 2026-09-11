/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "UndoOperation.h"

/*!
  \class CUndoOperation
  \brief Base class for all undo classes

  Override undo() to implement specific state restoring actions.
*/

/*!
  Constructs the undo operation.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CUndoOperation::CUndoOperation(const QString &text) : m_text(text) {}

/*!
  Destroys the object and frees any allocated resources.
*/
CUndoOperation::~CUndoOperation() {}

/*!
  Returns the description of the action that can be reverted by this object.
*/
const QString &CUndoOperation::text() const { return m_text; }

/*!
  \fn virtual void CUndoOperation::undo( CDocumentBase& document ) = 0;
  Override to implement custom state restoring actions.
  \param document Pass the document for determining the locality of the state
  restoration.
*/
