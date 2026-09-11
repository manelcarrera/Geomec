#include "DianaRunner.h"

//std
#include <iostream>
#include <cstdio>
#include <thread>
//own
#include "Printer.h"
#include "DianaStartUp.h"
#include "ISettings.h"
#include "Environment.h"
#include "QUtil.h" // util::concurrency::threads::quit()
using namespace util::concurrency;
//qt
#include <sstream> // to_string


//
// Notes:
//
// ptime t1 = second_clock::universal_time(); // needs to be 'universal_time', not local_time or others
// std::string t1_s = to_simple_string( t1 );
//

/////////////////////////////////////////////////////////////////
//
//
//					util functions
//
//
/////////////////////////////////////////////////////////////////
#ifdef KK
namespace gi // diana executer
{
	//
	// Builds FF from FFDIR
	//
	// NOT USED
	//
	QString FF( const QString& path )
	{
		int pos = path.lastIndexOf(QChar('/'));
		QString TEMP	= path.left( pos ); // Ej.: C:\Users\MANEL~1.CAR\AppData\Local\Temp\dra07288
		QString id = path.right( 5 );
		QString FF_ = QString::asprintf( "ffa%s.ff", id.toStdString().c_str() );
		return FF_;
	}
}
#endif
/////////////////////////////////////////////////////////////////
//
//
//			DianaRunner (DR)
//
//
/////////////////////////////////////////////////////////////////


DR::DR( IListenerIpc* gi )
	:	IListener( CEvents::DR_EH, {} ),
		m_th( nullptr ),
		m_gi( gi ),
		m_done(false)
{
	m_printer->info( "new" );
}

DR::~DR()
{
	m_printer->debug( "delete DR >>" );
	
	//
	// this can be done as now we now how to quit diana gracefully 
	//
	threads::quit(m_th,m_printer);

	//
	// just a clue on how to kill the thread
	//
	//void* hdl= m_th->native_handle();
	//pthread_cancel(hdl);

	m_printer->debug("<< delete DR");
}

//
// runs diana
//
// res: 1: success, 0: error
//
// params must have been verified in gm side: CDianaInterface
//
void DR::run_( const QStringList& params )
{
	CDianaStartUp* dsu = CDianaStartUp::instance();

	auto prepare_run=[=]()
	{
		m_printer->debug("size:%d --FFDIR-- %s --FF-- %s --STP0-- %s --BASE-- %s --DISPLAY-- %s --USER_LEADER-- %s --USER_TRAILER-- %s", 
			params.join("*").toStdString().size(),
			params[0].toStdString().c_str(),
			params[1].toStdString().c_str(),
			params[2].toStdString().c_str(),
			params[3].toStdString().c_str(),
			params[4].toStdString().c_str(),
			params[5].toStdString().c_str(),
			params[6].toStdString().c_str() );
		

		//
		//env
		//
		int i=0;
		for(const auto& x : {
			CDianaStartUp::FFDIR,
			CDianaStartUp::FF })
		{
			dsu->SetDianaEnv( x, params.at(i++) );
		}
		//
		// env STP0: construct fullpath
		//
		QString STP0 = dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + params.at(i++);
		dsu->SetDianaEnv( CEnvironment::STP0, STP0 );
		m_printer->debug("STP0 fullpath: %s", STP0.toStdString().c_str() ); 
		//
		// vars
		//
		for(const auto& x : {
			CDianaStartUp::BASE,
			CDianaStartUp::DISPLAY,
			CDianaStartUp::USER_LEADER,
			CDianaStartUp::USER_TRAILER	})
		{
			dsu->var( x, params[i++] );
		}

		//
		// enabled by default
		//
		// FIXME: there are functions to do this in Geomec.cpp, set them in a common place and re-use 
		//
		QString val = ISettings::instance()->getProfileString("Dsa", "ENABLE_MSG_EVENTS").toUpper();
		bool bEnable = val != "N" && val != "NO";
		m_printer->debug("settings : Dsa > ENABLE_MSG_EVENTS: %s", bEnable?"yes":"no"); 
		if (bEnable)
			dsu->gi( m_gi );
	};
	prepare_run();

	//
	// TODO: verify if gm42 kill or die can be caught here
	//
	try
	{
		//
		// even when run is aborted result is 0 (succes) -> res is wrong
		//
		int res = dsu->RunDiana_ONE_PROCESS();
		if(dsu->quit())
		{
			_e->notify( eCmd::QuitDiana_Done, CEvents::GI_EH );
		}
		else
		{
			done(true);
			_e->notify( Cmd( 
				eCmd::RunDiana_Done_, 
				new std::string( std::to_string( res == 0 ? 1:0 ) ) ), 
				//
				CEvents::GI_EH ); // 1: success
		}
	}
	catch(...)
	{
		m_printer->error("run diana"); 
	}
}

//
// run diana in a separated thread to let exe thread handle other coming commnads
//
void DR::on_run_diana(Cmd& cmd)
{
	void* data_ = cmd.second;
	std::string data = *reinterpret_cast< std::string* >(data_);
	QStringList params = QString::fromStdString(data).split("*");
	delete data_;

	threads::quit(m_th,m_printer); // just in case 

	m_th = new std::thread( &DR::run_, this, params );
}

void DR::handle(Cmd cmd)
{
	eCmd cmd_id = (eCmd)cmd.first;

	m_printer->debug("DR : handle: %s", _cmd_s(cmd_id));

	switch( cmd_id )
	{
		case eCmd::RunDiana:
		{
			on_run_diana(cmd);
			break;
		}
		case eCmd::QuitDiana:
		{
			CDianaStartUp* dsu = CDianaStartUp::instance();
			dsu->quit(true);
			break;
		}
		default: 
		{
			m_printer->error("cmd not handled : %s", g_cmd_s[ cmd.first ].c_str()); 
			break;
		}
	}
}