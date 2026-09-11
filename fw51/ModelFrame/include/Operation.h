/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __OPERATION_H
#define __OPERATION_H

#include "dimple.h"
#include <QObject>

#include "ModelFrameExports.h"

class IModelObject;
class CDocumentBase;
class CModelOperation;

class MODELFRAME_EXPORT COperation : public QObject {
  Q_OBJECT

public:
  typedef enum { OP_COMPOSITE, OP_CREATE, OP_DESTROY, OP_CUSTOM } TOperationType;

  COperation(IModelObject &location);
  COperation(IModelObject &location, TOperationType type, const QString &text = 0, int customID = 0);
  virtual ~COperation();

  void undo();

private slots:
  void onDocumentDestroyed();

private:
  CDocumentBase *m_document;
  CModelOperation *m_temp;
};

#endif //  __OPERATION_H
