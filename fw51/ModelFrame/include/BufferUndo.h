 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __BUFFER_UNDO_H
#define __BUFFER_UNDO_H

#include "LocalUndo.h"

#include "ModelFrameExports.h"

class IModelStream;

class MODELFRAME_EXPORT  CBufferUndo : public CLocalUndo
{
public:
  CBufferUndo( IModelObject& location, const QString& text );
  virtual ~CBufferUndo();

  virtual void undo( CDocumentBase& document );

protected:
  IModelStream& buffer();
  virtual void undo( IModelStream& stream, CDocumentBase& document ) = 0;

private:
  QByteArray*   m_bytes;
protected:
  QDataStream*  m_writeStream;
  IModelStream* m_modelStream;
};

#endif // __BUFFER_UNDO_H
