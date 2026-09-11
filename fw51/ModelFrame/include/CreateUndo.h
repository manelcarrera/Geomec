/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __CREATE_UNDO_H
#define __CREATE_UNDO_H

#include "LocalUndo.h"

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT CCreateUndo : public CLocalUndo {
public:
  CCreateUndo(IModelObject &location, const QString &text);
  virtual ~CCreateUndo();

  virtual void undo(CDocumentBase &document);
};

#endif // __CREATE_UNDO_H
