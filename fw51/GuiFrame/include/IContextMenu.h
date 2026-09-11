 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_CONTEXT_MENU_H
#define __I_CONTEXT_MENU_H

#include "GuiFrameExports.h"

class IGuiCommand;

class GUIFRAME_EXPORT IContextMenu
{
public:
  virtual void append( IGuiCommand* cmd ) = 0; //!< ownership is transferred
  virtual void appendSeparator() = 0;
};

#endif // __I_CONTEXT_MENU_H
