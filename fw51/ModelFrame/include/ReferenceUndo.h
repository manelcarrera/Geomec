 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __REFERENCE_UNDO_H
#define __REFERENCE_UNDO_H

#include "LocalUndo.h"

#include "ModelFrameExports.h"

class MODELFRAME_EXPORT  CReferenceUndo : public CLocalUndo
{
public:
  CReferenceUndo( IModelObject& location, const IModelObject& reference, int index, const QString& text );
  CReferenceUndo( IModelObject& location, const IModelObject& reference, const QString& text );
  virtual ~CReferenceUndo();

  virtual void undo( CDocumentBase& document );

private:
  int m_index;
  bool m_remove;
  CModelLocation m_referenceLocation;
};

#endif // __REFERENCE_UNDO_H
