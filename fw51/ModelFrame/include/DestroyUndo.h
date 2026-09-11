 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __DESTROY_UNDO_H
#define __DESTROY_UNDO_H

#include "BufferUndo.h"

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT  CDestroyUndo : public CBufferUndo
{
public:
  CDestroyUndo( IModelObject& parentLocation, IModelObject& object, const QString& text );
  virtual ~CDestroyUndo();

  using CBufferUndo::undo;

protected:
  virtual void undo( IModelStream& stream, CDocumentBase& document );

private:
  int m_index;
  QByteArray m_className;
};

#endif // __DESTROY_UNDO_H
