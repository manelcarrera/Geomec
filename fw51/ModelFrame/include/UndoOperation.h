/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __UNDO_OPERATION_H
#define __UNDO_OPERATION_H

#include "dimple.h"
#include <qstring.h>

#include "ModelFrameExports.h"

class CDocumentBase;

class MODELFRAME_EXPORT CUndoOperation {
public:
  CUndoOperation(const QString &text);
  virtual ~CUndoOperation();

  const QString &text() const;

  virtual void undo(CDocumentBase &document) = 0;

private:
  QString m_text;
};

#endif // __UNDO_OPERATION_H
