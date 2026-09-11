#pragma once

#include "IListener.h"
#include "IListenerIpc.h"

class TI : public IListener, public IListenerIpc
{
public:
	TI(const std::string& token);

protected:
	virtual void handle(Cmd cmd);
	virtual void received(Cmd cmd); //ipc
};
