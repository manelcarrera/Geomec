 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "CompositeUndo.h"

/*!
  \class CCompositeUndo
  \brief Undo class for a composition of undo objects.
*/

/*!
  Constructs the composed undo operation.
  \param stack The stack of undo objects (gathered by CModelOperation).
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CCompositeUndo::CCompositeUndo( const TOperationStack& stack, const QString& text )
: CUndoOperation( text ),
  m_stack( stack )
{
}

/*!
  Destroys the object and frees any allocated resources.
*/
CCompositeUndo::~CCompositeUndo()
{
  while ( m_stack.size() ) delete m_stack.pop();
}

/*!
  Undoes the state changes of all composed undo objects.
*/
void CCompositeUndo::undo( CDocumentBase& document )
{
  while ( m_stack.size() ) {
  CUndoOperation* undoOperation = m_stack.pop();
  undoOperation->undo( document );
  delete undoOperation;
  }
}
