/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __COLOR_UNDO_H
#define __COLOR_UNDO_H

#include "LocalUndo.h"
#include <qcolor.h>

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT CColorUndo : public CLocalUndo {
public:
  CColorUndo(IModelObject &location, const QRgb &previous, const QString &text);
  virtual ~CColorUndo();

  virtual void undo(CDocumentBase &document);

private:
  QRgb m_previous;
};

#endif // __COLOR_UNDO_H
