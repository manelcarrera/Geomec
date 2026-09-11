#ifndef _WMCommandCommand_h_
#define _WMCommandCommand_h_

#include "GuiCommand.h"

// This class can be used for sending WM_COMMAND messages to a particular window
class CWMCommandCommand : public IGuiCommand
{
private:
  HWND m_hWnd;
  long m_id;
public:
  CWMCommandCommand(HWND hwnd,long id) : m_hWnd(hwnd), m_id(id){}
  virtual void execute()
  {
    ::SendMessage(m_hWnd,WM_COMMAND,m_id,0);
  }
};

#endif  // _WMCommandCommand_h_
