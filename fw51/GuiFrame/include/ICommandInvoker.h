 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_COMMAND_INVOKER_H
#define __I_COMMAND_INVOKER_H

#include "GuiFrameExports.h"

class IGuiCommand;

class GUIFRAME_EXPORT ICommandInvoker
{
public:
  virtual void append( IGuiCommand* cmd ) = 0; //!< ownership is transferred
  virtual void appendSeparator() = 0;
};

#endif // __I_COMMAND_INVOKER_H
