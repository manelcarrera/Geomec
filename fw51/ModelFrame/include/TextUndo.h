 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __TEXT_UNDO_H
#define __TEXT_UNDO_H

#include "LocalUndo.h"

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT  CTextUndo : public CLocalUndo
{
public:
  CTextUndo( IModelObject& location, const QString& previous, const QString& text );
  virtual ~CTextUndo();

  virtual void undo( CDocumentBase& document );

private:
  QString m_previous;
};

#endif // __TEXT_UNDO_H
