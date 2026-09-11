#include "Printer_SP.h"

#include <stdio.h>
#include <cstdarg>

Printer_SP::Printer_SP() 
	: Printer()
{
}

void Printer_SP::debug( const char* fmt, ... )
{
	if( m_cfg.debug && enabled())
	{
		std::lock_guard<std::mutex> lock(m);

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start( vl, fmt );
		vsnprintf( buf, sizeof( buf ), fmt, vl );
		va_end( vl );

		print(buf,Debug);
	}
}

void Printer_SP::error( const char* fmt, ... )
{
	if( m_cfg.error && enabled())
	{
		std::lock_guard<std::mutex> lock(m);

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start( vl, fmt );
		vsnprintf( buf, sizeof( buf ), fmt, vl );
		va_end( vl );

		print(buf, Error);
	}
}

void Printer_SP::info( const char* fmt, ... )
{
	if( m_cfg.info && enabled())
	{
		std::lock_guard<std::mutex> lock(m);

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start( vl, fmt );
		vsnprintf( buf, sizeof( buf ), fmt, vl );
		va_end( vl );

		print(buf, Info); 
	}
}
