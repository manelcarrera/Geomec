/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_OPERATION_H
#define __MODEL_OPERATION_H

#include "dimple.h"
#include <QObject>
#include <QStack>

#include "ModelFrameExports.h"

class IModelObject;
class CDocumentBase;
class CModelOperationStack;
class CUndoOperation;

class MODELFRAME_EXPORT CModelOperation : public QObject {
  Q_OBJECT

  friend class CModelOperationStack;

public:
  CModelOperation(IModelObject &location, const QString &text = 0);
  virtual ~CModelOperation();

  bool stackEnabled() const;

  const QString &text() const;

  void push(CUndoOperation *undo);
  bool isEmpty() const;

  void cancel();

private:
  void beginOperation(CModelOperation &operation);
  void endOperation(CModelOperation &operation);

  void push(CModelOperation &operation);

  QString m_text;

  CDocumentBase *m_document;
  CModelOperationStack *m_stack;
  CModelOperation *m_pending;

  typedef QStack<CUndoOperation *> TOperationStack;
  TOperationStack m_localStack;

  typedef enum { OPERATION_WAIT, OPERATION_CANCEL } TOperationState;
  TOperationState m_state;
};

#endif //  __MODEL_OPERATION_H
