/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __GUI_COMMAND_H
#define __GUI_COMMAND_H

#include <qstring.h>

#include "GuiFrameExports.h"

class GUIFRAME_EXPORT IGuiCommand {
public:
  virtual ~IGuiCommand() {}
  virtual QString caption() { return 0; }
  virtual void execute() {}
  virtual bool canExecute() { return true; }
  virtual bool isChecked() { return false; }
};

template <class TARGET_TYPE> class CGuiCommand : public IGuiCommand {
public:
  typedef void (TARGET_TYPE::*TExecMember)(void);
  typedef bool (TARGET_TYPE::*TBoolExecMember)(void);
  typedef bool (TARGET_TYPE::*TCanExecMember)(void) const;
  typedef bool (TARGET_TYPE::*TIsChkMember)(void) const;

  CGuiCommand(const QString &caption, TARGET_TYPE &target, TExecMember exec, TCanExecMember canExec = 0,
              TIsChkMember isChecked = 0);
  CGuiCommand(const QString &caption, TARGET_TYPE &target, TBoolExecMember exec, TCanExecMember canExec = 0,
              TIsChkMember isChecked = 0);

  virtual QString caption();
  virtual void execute();
  virtual bool canExecute();
  virtual bool isChecked();

private:
  QString m_caption;
  TARGET_TYPE *m_target;
  TExecMember m_execMember;
  TBoolExecMember m_boolExecMember;
  TCanExecMember m_canExecMember;
  TIsChkMember m_isCheckedMember;
};

template <class TARGET_TYPE>
CGuiCommand<TARGET_TYPE>::CGuiCommand(const QString &caption, TARGET_TYPE &target, TExecMember exec,
                                      TCanExecMember canExec, TIsChkMember isChecked)
    : m_caption(caption), m_target(&target), m_execMember(exec), m_boolExecMember(0), m_canExecMember(canExec),
      m_isCheckedMember(isChecked) {}

template <class TARGET_TYPE>
CGuiCommand<TARGET_TYPE>::CGuiCommand(const QString &caption, TARGET_TYPE &target, TBoolExecMember exec,
                                      TCanExecMember canExec, TIsChkMember isChecked)
    : m_caption(caption), m_target(&target), m_execMember(0), m_boolExecMember(exec), m_canExecMember(canExec),
      m_isCheckedMember(isChecked) {}

template <class TARGET_TYPE> QString CGuiCommand<TARGET_TYPE>::caption() { return m_caption; }

template <class TARGET_TYPE> void CGuiCommand<TARGET_TYPE>::execute() {
  if (m_execMember)
    (m_target->*m_execMember)();
  else if (m_boolExecMember)
    (m_target->*m_boolExecMember)();
}

template <class TARGET_TYPE> bool CGuiCommand<TARGET_TYPE>::canExecute() {
  if (m_canExecMember)
    return (m_target->*m_canExecMember)();
  return true;
}

template <class TARGET_TYPE> bool CGuiCommand<TARGET_TYPE>::isChecked() {
  if (m_isCheckedMember)
    return (m_target->*m_isCheckedMember)();
  return false;
}

#endif // __GUI_COMMAND_H
