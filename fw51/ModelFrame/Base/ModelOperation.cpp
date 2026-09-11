/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelOperation.h"
#include "CompositeUndo.h"
#include "DocumentBase.h"
#include "ModelOperationStack.h"
#include <cassert>

/*!
  \class CModelOperation
  \brief Receiver of state changes.

  All state changes during the life time are captured via either nested
  CModelOperations or CUndoOperation objects, explicitly being inserted.

  Collected state changes are either passed up to the document's
  CModelOperationStack or reverted via cancel().
*/

/*!
  Constructs a CModelOperation.
  This operation is on top of already existing CModelOperation instances.
  While this is the case, CUndoOperations can be inserted and all changes
  so far can be cancelled.
  As soon as a newer CModelOperation is there, this object waits for newer
  operations to end and takes over all recorded state changes.
*/
CModelOperation::CModelOperation(IModelObject &location, const QString &text)
    : QObject(0), m_text(text), m_document(location.document()), m_stack(0), m_pending(0), m_state(OPERATION_WAIT) {
  setObjectName("ModelOperation");

  if (m_document) {
    m_stack = &m_document->operationStack();
    m_stack->beginOperation(*this);
  }
}

/*!
  Destroys the object, ends the operation and passes all state changes
  recorded to the object next on top. This can be either an older
  CModelOperation instance or the document's CModelOperationStack.
*/
CModelOperation::~CModelOperation() {
  if (m_stack) {
    m_stack->endOperation(*this);
  } else {
    assert(m_localStack.size() == 0);
    while (m_localStack.size())
      delete m_localStack.pop();
  }
}

/*!
  Returns true if state changes need to be recorded.
  Check this member before inserting any CUndoOperation objects.
*/
bool CModelOperation::stackEnabled() const { return m_stack != 0 && m_stack->isEnabled(); }

/*!
  Returns a description about the state change(s) being gathered.
*/
const QString &CModelOperation::text() const { return m_text; }

/*!
  Pushes the latest state change on the stack. The ownership of the object
  is transferred to this object. CUndoOperation needs to be created on the
  heap!

  If this object is cancelling its state changes, the state change is ignored
  (and destroyed).
*/
void CModelOperation::push(CUndoOperation *undo) {
  assert(stackEnabled());
  switch (m_state) {
  case OPERATION_WAIT:
    m_localStack.push(undo);
    if (m_text.isEmpty())
      m_text = undo->text();
    break;
  case OPERATION_CANCEL:
    delete undo;
  }
}

/*!
  Reverts all state changes collected by this operation so far.
  Any state changes resulting from this restoration are ignored.
*/
void CModelOperation::cancel() {
  assert(m_document);
  m_state = OPERATION_CANCEL;
  while (m_localStack.size()) {
    CUndoOperation *undoOperation = m_localStack.pop();
    undoOperation->undo(*m_document);
    delete undoOperation;
  }
  m_state = OPERATION_WAIT;
}

/*!
  Returns true if any state change has been collected so far.
*/
bool CModelOperation::isEmpty() const { return m_localStack.size() == 0; }

/*!
  Called when a new operation comes into existence. While this new
  operation is pending, nothing can be done with this object.
*/
void CModelOperation::beginOperation(CModelOperation &operation) {
  if (m_pending) {
    m_pending->beginOperation(operation);
  } else {
    m_pending = &operation;
  }
}

/*!
  Called when a newer operation ends. The changes collected by this
  newer operation are taken over.
*/
void CModelOperation::endOperation(CModelOperation &operation) {
  assert(m_pending);
  if (m_pending != &operation) {
    m_pending->endOperation(operation);
  } else {
    m_pending = 0;
    push(operation);
  }
}

/*!
  Takes over the changes collected by the operation.
*/
void CModelOperation::push(CModelOperation &operation) {
  if (!operation.isEmpty()) {
    if (operation.m_localStack.size() == 1) {
      push(operation.m_localStack.pop()); // remove 1 level
    } else {
      push(new CCompositeUndo(operation.m_localStack, operation.text()));
    }
  }
}
