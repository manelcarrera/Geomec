/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __SIMPLE_UNDO_H
#define __SIMPLE_UNDO_H

#include "BufferUndo.h"

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT CSimpleUndo : public CBufferUndo {
public:
  CSimpleUndo(IModelObject &location, const QString &text);
  virtual ~CSimpleUndo();

protected:
  using CBufferUndo::undo;
  virtual void undo(IModelStream &stream, CDocumentBase &document);
};

#endif // __SIMPLE_UNDO_H
