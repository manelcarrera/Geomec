/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "SimpleUndo.h"
#include "DocumentBase.h"
#include "IProgressBase.h"
#include "ModelOperation.h"
#include "StreamVersion.h"

/*!
  \class CSimpleUndo
  \brief Basic undo class for restoring the entire (local) state.

  The buffer of the object is stored and restored with
  IModelObject::store() and IModelObject::restore().
*/

/*!
  Constructs the basic undo operation.
  \param location is the document item whose state is affected.
  \param text might be used to provide feedback to the user as to what state
  can be restored.
*/
CSimpleUndo::CSimpleUndo(IModelObject &location, const QString &text) : CBufferUndo(location, text) {
  IProgressBase progress;
  location.store(buffer(), progress, false);
}

/*!
  Destroys the object and frees any allocated resources.
*/
CSimpleUndo::~CSimpleUndo() {}

/*!
  Restores the state of the document item from the buffer() with
  IModelObject::restore().
*/
void CSimpleUndo::undo(IModelStream &stream, CDocumentBase &document) {
  IProgressBase progress;
  IModelObject &object = location(document);

  // store previous state for redo
  CModelOperation operation(object);
  CSimpleUndo *redo = new CSimpleUndo(object, "Change fields");

  object.restore(stream, document.documentVersion(), progress);

  if (operation.stackEnabled())
    operation.push(redo);
}
