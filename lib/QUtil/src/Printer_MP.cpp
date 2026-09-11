#include "Printer_MP.h"

#include <QSystemSemaphore>

#include <stdio.h>
#include <cstdarg>


Printer_MP::Printer_MP() 
	: Printer()
{
	m_sem = new QSystemSemaphore("Printer", 1);
}

Printer_MP::~Printer_MP()
{
	delete m_sem;
};

void Printer_MP::debug( const char* fmt, ... )
{
	if( m_cfg.debug && enabled() )
	{
		m_sem->acquire();

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start(vl, fmt);
		vsnprintf(buf, sizeof(buf), fmt, vl);
		va_end(vl);

		print(buf, Info);

		m_sem->release();
	}
}

void Printer_MP::error( const char* fmt, ... )
{
	if( m_cfg.error && enabled())
	{
		m_sem->acquire();

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start( vl, fmt );
		vsnprintf( buf, sizeof( buf ), fmt, vl );
		va_end( vl );

		print(buf, Error);
		m_sem->release();
	}
}

void Printer_MP::info( const char* fmt, ... )
{
	if( m_cfg.info && enabled())
	{
		m_sem->acquire();

		char buf[ MAX_LEN_BUFFER ];     // this should really be sized appropriately
						   // possibly in response to a call to vsnprintf()

		va_list vl;
		va_start( vl, fmt );
		vsnprintf( buf, sizeof( buf ), fmt, vl );
		va_end( vl );

		print( buf, Info );
		m_sem->release();
	}
}
