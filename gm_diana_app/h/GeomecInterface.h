#pragma once

//qt
#include <QString>
//own
#include "IListener.h"
#include "IListenerIpc.h"
#include "Wait.h"

class DR;

struct trace_queue;

class GI : public IListener, public IListenerIpc
{
	DR* m_dr;

	CWait m_wait;

protected:
	void handle(Cmd cmd);
	void received(Cmd cmd);

public:

	GI( const std::string& token = std::string() );
	~GI();

	void wait();
};