#pragma once

#include "Printer.h"

namespace boost{
	namespace interprocess{
		class named_mutex;
	}
}

// boost multi-process
class Printer_MP_BOOST : public Printer
{
	boost::interprocess::named_mutex* m_mutex;

public:
	
	Printer_MP_BOOST();
	~Printer_MP_BOOST();

	void debug( const char* fmt, ... );
	void error( const char* fmt, ... );
	void info( const char* fmt, ... );
};

