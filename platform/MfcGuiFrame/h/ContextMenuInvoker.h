#ifndef _CONTEXTMENUINVOKER_H_
#define _CONTEXTMENUINVOKER_H_

#include <string>
#include <map>
#include <vector>
#include "ICommandInvoker.h"
#include "GuiCommand.h"

class CContextMenuInvoker : public ICommandInvoker
{
  typedef std::pair<IGuiCommand*, bool> TCommand;
  typedef std::pair<std::string, TCommand> TCommandPair;
  typedef std::map<std::string, TCommand> TCommandMap;
  typedef std::vector<std::string> TCommandVec;
private:
  TCommandMap m_mpCommands;
  TCommandVec m_vcCommands;

  // These classes are used for identifying separators and invokers.
  class CSeparatorCommand : public IGuiCommand{};
  class CInvokerCommand : public IGuiCommand
  {
  private:
    CContextMenuInvoker &m_invoker;
    bool m_bOwner;
  public:
    CInvokerCommand(CContextMenuInvoker &invoker,bool bOwner=true) : m_invoker(invoker), m_bOwner(bOwner){}
    ~CInvokerCommand()
    {
      if (m_bOwner)
        delete &m_invoker;
    }
    CContextMenuInvoker &Invoker() { return m_invoker; }
  };
  CMenu m_menu;
protected:
  // Build the menu based on the current commands
  int BuildMenu(int iStartIdx);	
public:
  CContextMenuInvoker();
  virtual ~CContextMenuInvoker();
  // Add a separator to the contextmenu
  void AddSeparator();
  virtual void appendSeparator();	
  // Add an invoker (ie a submenu) to the contextmenu
  void AddInvoker(const CString &title, CContextMenuInvoker &subInvoker); 
  void AddInvoker(unsigned int uTitle, CContextMenuInvoker &subInvoker); 
  // Track the menu
  bool TrackMenu(int x, int y, CWnd &parent); 
  // Invoke the command based on its string representation
  virtual bool Invoke(std::string strCommand);
  bool AddCommand(std::string strCommand, IGuiCommand &command, bool bAutoDelete = true);
  bool AddCommand(unsigned int uCommand, IGuiCommand &command, bool bAutoDelete = true);
  virtual void append( IGuiCommand* cmd ); //!< ownership is transferred
};

#endif