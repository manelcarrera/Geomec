#ifndef _ICOMMAND_H_
#define _ICOMMAND_H_

#include "GuiCommand.h"

// This template can be used as a command executer for single functions. It accepts pointers to functions with the following
// prototype:  void f(void);
template<class T>
class CSingleCommandTemplate : public CGuiCommand<T>
{
public:
  CSingleCommandTemplate(T& object,
    typename CGuiCommand <T> ::TExecMember exec,
    typename CGuiCommand <T> ::TCanExecMember canExec = 0,
    typename CGuiCommand <T> ::TIsChkMember isChecked = 0) 
  : CGuiCommand<T>("", object, exec, canExec, isChecked)
  {
  }

  CSingleCommandTemplate(T& object,
    typename CGuiCommand <T> ::TBoolExecMember exec,
    typename CGuiCommand <T> ::TCanExecMember canExec = 0,
    typename CGuiCommand <T> ::TIsChkMember isChecked = 0) 
  : CGuiCommand<T>("", object, exec, canExec, isChecked)
  {
  }
};

template<class T>
class CConstSingleCommandTemplate : public IGuiCommand
{
private:
  typedef bool (T::*pfIsChecked)(void) const;
  typedef bool (T::*pfCanExecute)(void) const;
  typedef void (T::*pfExecute)(void) const;

  const T*			m_pObject;
  pfCanExecute		m_pCanExecute;
  pfExecute			m_pExecute;
  pfIsChecked			m_pIsChecked;
public:

  CConstSingleCommandTemplate(const T& object, pfExecute execute, pfCanExecute canExecute=0, pfIsChecked isChecked=0) 
  : m_pObject(&object),
    m_pCanExecute(canExecute) ,
    m_pExecute(execute),
    m_pIsChecked(isChecked)
  {
  }

  virtual void execute()
  {
    (m_pObject->*m_pExecute)();
  }

  virtual bool canExecute()
  {
    if (m_pCanExecute)
      return (m_pObject->*m_pCanExecute)();
    return true;
  }

  virtual bool isChecked()
  {
    if (m_pIsChecked)
      return (m_pObject->*m_pIsChecked)();
    return false;
  }
};

#endif
