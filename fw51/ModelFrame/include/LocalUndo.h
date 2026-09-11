 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __LOCAL_UNDO_H
#define __LOCAL_UNDO_H

#include "UndoOperation.h"
#include "ModelLocation.h"

#include "ModelFrameExports.h"

class IModelObject;
class CDocumentBase;

class MODELFRAME_EXPORT  CLocalUndo : public CUndoOperation
{
public:
  CLocalUndo( IModelObject& location, const QString& text );
  virtual ~CLocalUndo();

protected:
  IModelObject& location( CDocumentBase& document );

private:
  CModelLocation m_documentLocation;
};

#endif // __LOCAL_UNDO_H
