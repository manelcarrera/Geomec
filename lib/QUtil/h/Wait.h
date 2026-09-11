#pragma once

#include <mutex>
#include <condition_variable>

#include "Events.h"

class Printer;

class CWait
{
	std::mutex m_mutex;
	std::condition_variable m_cond_var;

	CEvents::eEventsHandler m_eh_id;

	Printer* m_printer;

	bool m_waiting;
	
public:
	CWait(CEvents::eEventsHandler eh_id);
	~CWait();

	bool wait(int timeout=0);
	void signal();
};
