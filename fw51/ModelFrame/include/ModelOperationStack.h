/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_OPERATION_STACK_H
#define __MODEL_OPERATION_STACK_H

#include "dimple.h"
#include <QObject>
#include <QStack>

#include "ModelFrameExports.h"

class CDocumentBase;
class CModelOperation;
class CUndoOperation;

class MODELFRAME_EXPORT CModelOperationStack : public QObject {
  Q_OBJECT

  friend class CModelOperation;

public:
  CModelOperationStack(CDocumentBase &document);
  virtual ~CModelOperationStack();

  bool canUndo() const;
  bool canRedo() const;
  bool isClean() const;
  bool isEnabled() const;

  QString undoText() const;
  QString redoText() const;

  QStringList undoNames();
  QStringList redoNames();

signals:
  void cleanChanged(bool clean);
  void changed();
  void operationStarted();
  void operationFinished();

public slots:
  void undo();
  void redo();
  void setClean();
  void setEnabled(bool on);
  void clear();

private:
  void beginOperation(CModelOperation &operation);
  void endOperation(CModelOperation &operation);

  void push(CModelOperation &operation);
  void push(CUndoOperation *undo);

  CDocumentBase &m_document;

  typedef QStack<CUndoOperation *> TOperationStack;
  TOperationStack m_undoStack;
  TOperationStack m_redoStack;

  bool m_hasCleanState;
  int m_cleanState;

  CModelOperation *m_pending;

  bool m_enabled;

  typedef enum { STACK_WAIT, STACK_UNDO, STACK_REDO } TStackState;
  TStackState m_state;
};

class MODELFRAME_EXPORT CDisableOperationStack {
public:
  CDisableOperationStack(CDocumentBase *document);
  ~CDisableOperationStack();

private: // not copyable
  CDisableOperationStack(const CDisableOperationStack &);
  CDisableOperationStack &operator=(const CDisableOperationStack);

private:
  bool m_previousState;
  CDocumentBase *m_document;
};

#endif // __MODEL_OPERATION_STACK_H
