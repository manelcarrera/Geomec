/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __COMPOSITE_UNDO_H
#define __COMPOSITE_UNDO_H

#include "UndoOperation.h"
#include <QStack>

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT CCompositeUndo : public CUndoOperation {
public:
  typedef QStack<CUndoOperation *> TOperationStack;

  CCompositeUndo(const TOperationStack &stack, const QString &text);
  virtual ~CCompositeUndo();

  virtual void undo(CDocumentBase &document);

private:
  TOperationStack m_stack;
};

#endif // __COMPOSITE_UNDO_H
