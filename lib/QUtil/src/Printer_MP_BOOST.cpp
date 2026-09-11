#include "Printer_MP_BOOST.h"

//std
#include <stdio.h>
#include <cstdarg>
#include <iostream> // std::cout
//boost
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace boost::interprocess;

Printer_MP_BOOST::Printer_MP_BOOST() 
	: Printer()
{
	m_mutex = new named_mutex(open_or_create, std::to_string(Printer::pid()).c_str());
}

Printer_MP_BOOST::~Printer_MP_BOOST()
{
	named_mutex::remove(std::to_string(Printer::pid()).c_str());
	delete m_mutex;
};

void Printer_MP_BOOST::debug( const char* fmt, ... )
{
	if( m_cfg.debug && enabled() )
	{
		try{
			scoped_lock<named_mutex> lock(*m_mutex);

			char buf[ MAX_LEN_BUFFER ];

			va_list vl;
			va_start(vl, fmt);
			vsnprintf(buf, sizeof(buf), fmt, vl);
			va_end(vl);

			print(buf, Debug);
		}
		catch(interprocess_exception &ex)
		{
		  named_mutex::remove(std::to_string(Printer::pid()).c_str());
		  std::cout << ex.what() << std::endl;
		}
	}
}

void Printer_MP_BOOST::error( const char* fmt, ... )
{
	if( m_cfg.error && enabled())
	{
		try{
			scoped_lock<named_mutex> lock(*m_mutex);

			char buf[ MAX_LEN_BUFFER ];

			va_list vl;
			va_start(vl, fmt);
			vsnprintf(buf, sizeof(buf), fmt, vl);
			va_end(vl);

			print(buf, Error);
		}
		catch(interprocess_exception &ex)
		{
		  named_mutex::remove(std::to_string(Printer::pid()).c_str());
		  std::cout << ex.what() << std::endl;
		}
	}
}

void Printer_MP_BOOST::info( const char* fmt, ... )
{
	if( m_cfg.info && enabled())
	{
		try{
			scoped_lock<named_mutex> lock(*m_mutex);

			char buf[ MAX_LEN_BUFFER ];

			va_list vl;
			va_start(vl, fmt);
			vsnprintf(buf, sizeof(buf), fmt, vl);
			va_end(vl);

			print(buf, Info);
		}
		catch(interprocess_exception &ex)
		{
		  named_mutex::remove(std::to_string(Printer::pid()).c_str());
		  std::cout << ex.what() << std::endl;
		}
	}
}
