 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_SELECTION_CONTEXT_H
#define __I_SELECTION_CONTEXT_H

#include <QObject>

#include "GuiFrameExports.h"

class GUIFRAME_EXPORT ISelectionContext : public QObject
{
  Q_OBJECT

signals:
  void selectionChanged();

public:
  ISelectionContext( QObject* owner );
  virtual ~ISelectionContext();

  virtual void deleteSelection();
  virtual bool canDeleteSelection() const;
#if 0
  virtual void cutSelection();
  virtual bool canCutSelection() const;
  virtual void copySelection();
  virtual bool canCopySelection() const;
  virtual void pasteAtCurrent();
  virtual void canPasteAtCurrent() const;
#endif

protected slots:
  void setDirty();
};

#endif // __I_SELECTION_CONTEXT_H
