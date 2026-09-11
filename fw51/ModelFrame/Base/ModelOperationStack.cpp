 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelOperationStack.h"
#include "ModelOperation.h"
#include "CompositeUndo.h"
#include "UndoOperation.h"
#include "DocumentBase.h"

#include <QStringList>
#include <QVector>

/*!
  \class CModelOperationStack
  \brief Container for document state changes

  All state changes during the life time of the document are captured via
  CModelOperations and passed to the document's stack.

  The collected state changes can be reverted.

  Any changes to the stack can be monitored via its signals.

  Whether or not the state of the document matches its storage is
  managed via setClean() and isClean(). Changes in this state are signalled too.
*/

/*!
  Constructs the CModelOperationStack for a specific document.
*/
CModelOperationStack::CModelOperationStack( CDocumentBase& document )
: QObject( 0 ),
  m_document( document ),
  m_hasCleanState( true ),
  m_cleanState( 0 ),
  m_pending( 0 ),
  m_enabled( true ),
  m_state( STACK_WAIT )
{
  setObjectName( "ModelOperationStack" );
}

/*!
  Destroys the object and frees any allocated resources.
*/
CModelOperationStack::~CModelOperationStack()
{
  CModelOperation* pending = m_pending;
  while ( pending ) {
    pending->m_stack = 0;
    pending = pending->m_pending;
  }
  while ( m_undoStack.size() ) delete m_undoStack.pop();
  while ( m_redoStack.size() ) delete m_redoStack.pop();
}

/*!
  Returns whether or not the latest state change can be reverted.
*/
bool CModelOperationStack::canUndo() const
{
  if ( !m_enabled ) return false;
  if ( m_pending )  return false;
  if ( m_undoStack.size() == 0 ) return false;
  return true;
}

/*!
  Returns whether or not the latest, reverted state change can be redone.
*/
bool CModelOperationStack::canRedo() const
{
  if ( !m_enabled ) return false;
  if ( m_pending )  return false;
  if ( m_redoStack.size() == 0 ) return false;
  return true;
}

/*!
  Returns whether or not the document state matches its state in storage.
*/
bool CModelOperationStack::isClean() const
{
  return m_hasCleanState && m_cleanState == 0;
}

/*!
  Returns whether or not the capturing of state changes is enabled or not.
*/
bool CModelOperationStack::isEnabled() const
{
  return m_enabled;
}

/*!
  Returns the description of the state change that can be reverted.
*/
QString CModelOperationStack::undoText() const
{
  if ( canUndo() ) return m_undoStack.top()->text();
  return QString::null;
}

/*!
  Returns the description of the state change that has been reverted but
  can be redone.
*/
QString CModelOperationStack::redoText() const
{
  if ( canRedo() ) return m_redoStack.top()->text();
  return QString::null;
}

/*!
  Reverts the latest state change.
  Any changes resulting from this are collected and added to the redo stack.
*/
void CModelOperationStack::undo()
{
  DIA_ASSERT( canUndo() );
  m_state = STACK_UNDO;
  {
    CUndoOperation* undoOperation = m_undoStack.pop();
    // Block the gui and collect redo operation(s).
    CModelOperation block( m_document, undoOperation->text() );
    undoOperation->undo( m_document );
    delete undoOperation;
  }
  m_state = STACK_WAIT;
  emit changed();
}

/*!
  Redoes the latest state change that has been reverted.
  Any changes resulting from this are collected and added to the undo stack
  again.
*/
void CModelOperationStack::redo()
{
  DIA_ASSERT( canRedo() );
  m_state = STACK_REDO;
  {
    CUndoOperation* redoOperation = m_redoStack.pop();
    // Block the gui and collect undo operation(s).
    CModelOperation block( m_document, redoOperation->text() );
  
    redoOperation->undo( m_document );
    delete redoOperation;
  }
  m_state = STACK_WAIT;
  emit changed();
}

/*!
  Sets the current document state to match against the document storage.
*/
void CModelOperationStack::setClean()
{
  if ( isClean() ) return;
  m_hasCleanState = true;
  m_cleanState = 0;
  emit cleanChanged( true );
}

/*!
  Enables or disables the capturing of state changes.
  It is advisable to disable the capturing during loading or creation of the
  document.
*/
void CModelOperationStack::setEnabled( bool on )
{
  DIA_ASSERT( m_state == STACK_WAIT );
  if ( m_enabled == on ) return;
  m_enabled = on;
  emit changed();
}

/*!
  Clears all captured state changes and marks the document to be clean.
*/
void CModelOperationStack::clear()
{
  while ( m_undoStack.size() ) delete m_undoStack.pop();
  while ( m_redoStack.size() ) delete m_redoStack.pop();
  emit changed();
  setClean();
}

/*!
  Called when a new operation comes into existence. While this new
  operation is pending, nothing can be done with the stack.
*/
void CModelOperationStack::beginOperation( CModelOperation& operation )
{
  if ( m_pending ) {
    m_pending->beginOperation( operation );
  } else {
    m_pending = &operation;
    emit operationStarted();
  }
}

/*!
  Called when an operation ends. If the last operation ends, the changes
  collected are taken over.
*/
void CModelOperationStack::endOperation( CModelOperation& operation )
{
  DIA_ASSERT( m_pending );
  if ( m_pending != &operation ) {
    m_pending->endOperation( operation );
  } else {
    m_pending = 0;
    push( operation );
    emit operationFinished();
  }
}

/*!
  Pushes the latest state change on the stack. The ownership of the object
  is transferred to this object. CUndoOperation needs to be created on the
  heap!

  If the stack is undoing any changes the state change is stored for redo.
  If the stack is redoing any changes the state change is stored for undo.
  If the stack is "idle" the state change is stored for undo, and the redo
  stack is cleared (invalid).

  Any changes in the stack are signalled.
*/
void CModelOperationStack::push( CUndoOperation* undo )
{
  bool wasClean = isClean();
  switch ( m_state ) {
    case STACK_WAIT:
      if ( m_hasCleanState && m_cleanState > 0 ) {
        // clean state in redo stack, not reachable anymore
        m_hasCleanState = false;
      }
      // New state, redoes invalid
      while ( m_redoStack.size() ) delete m_redoStack.pop();
    case STACK_REDO:
      if ( m_hasCleanState ) m_cleanState--;
      m_undoStack.push( undo );
      break;
    case STACK_UNDO:
      if ( m_hasCleanState ) m_cleanState++;
      m_redoStack.push( undo );
  }
  if ( wasClean != isClean() ) emit cleanChanged( isClean() );
}

/*!
  Takes over the changes collected by the operation.
*/
void CModelOperationStack::push( CModelOperation& operation )
{
  if ( !operation.isEmpty() ) {
    push( new CCompositeUndo( operation.m_localStack, operation.text() ) );
    emit changed();
  }
}

/*!
  Get names of all undo operations in stack (in reversed order).
  \retval QStringList list of undo names
*/
QStringList CModelOperationStack::undoNames()
{
  QStringList undoList;
  for (int i = m_undoStack.size(); i > 0; --i)
  {
    undoList << m_undoStack.at(i-1)->text();
  }
  return undoList;
}

/*!
  Get names of all redo operations in stack (in reversed order).
  \retval QStringList list of redo names
*/
QStringList CModelOperationStack::redoNames()
{
  QStringList redoList;
  for (int i = m_redoStack.size(); i > 0; --i)
  {
    redoList << m_redoStack.at(i-1)->text();
  }
  return redoList;
}

/*!
  Disables the operation stack for the lifetime of this object.
  On destruction, the previous state is restored.

  Advised usage is to create a CDisableOperationStack local variable in the
  scope where the operation stack must be disabled. When the variable goes
  out of scope the previous state of the operation stack is restored.
*/
CDisableOperationStack::CDisableOperationStack( CDocumentBase* doc )
  : m_document( doc )
{
  m_previousState = m_document->operationStack().isEnabled();
  m_document->operationStack().setEnabled( false );
}

CDisableOperationStack::~CDisableOperationStack()
{
  m_document->operationStack().setEnabled( m_previousState );
}

