#include "headers.h"

//boost
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <cstdio>
//#include "doc_anonymous_condition_shared_data.hpp"
#include <libs/interprocess/example/doc_anonymous_condition_shared_data.hpp>

using namespace boost::interprocess;

//own
#include "Printer.h"
#include "SafeQueue.h"
//qt
#include <QString> 
#include <QProcess> 
#include <QStringList>
//std
#include <thread>
#include <chrono>

/////////////////////////////////////////////////
//
//			work-around OIV license
//
/////////////////////////////////////////////////

//own
#include "DianaStartUp.h"
#include "Global.h"
#include "Events.h"
#include "DianaInterface.h"
#include "SettingsIni.h"
#include "QUtil.h"
//qt
#include <QFile>

//defines
static const QString BASE = "C:\\aWork\\aTmp";
static const QString TRIGGER = BASE + "\\trigger";
static const QString FFDIR = BASE + "\\drb26312";

//
// GUI emulator, replaces Geomec app
//
class CGUI
{
	std::thread* m_thread;
	SafeQueue< Cmd > m_queue;

private:

public:
	CGUI()
	{
		m_thread =  new std::thread( &CGUI::exe, this );
	}

	~CGUI()
	{
		if( m_thread->joinable() )
			m_thread->join();
		delete m_thread;
	}

	void push( Cmd cmd )
	{ 
		Printer::instance()->info("[GUI] push : cmd: %s", g_cmd_s[ cmd.first ].c_str() );
		m_queue.enqueue( cmd ); 
	}

	Cmd pop()
	{
		Cmd cmd = m_queue.dequeue();
		Printer::instance()->debug("[GUI] pop :'%s'", g_cmd_s[ cmd.first ].c_str() );
		return cmd;
	}


private:

	void exe()
	{
		CDianaStartUp* dsu = CDianaStartUp::instance();

		for(;;)
		{
			Cmd cmd = pop();

			static int count = 1;

			Printer::instance()->info("[GUI] exe : pop: %s", g_cmd_s[ cmd.first ].c_str() );

			//
			//
			//
			/*DI* di = dsu->di();
			std::thread* thread = di->m_thread_exe;
			if( thread->joinable() )
			{
				//
				// crash joining
				//
				thread->join();
			}
			delete thread;
			thread = nullptr;*/

			//Printer::instance()->info("[GUI] exe thread : deleted" );

			// FIXME
			/*if( true )
				return;
			else*/

			//Printer::instance()->info("[GUI] exe : count: %d", count );
			if( count > 1 )
				break;

			switch( cmd.first )
			{
				case QuitDiana_Ack:
				case HeartBeat_Error:
				{
					Printer::instance()->info( "GUI : execution : res : %s", cmd.first == QuitDiana_Ack ? "Ok" : "ERROR" );

					std::this_thread::sleep_for(std::chrono::milliseconds( 1*1000 ));

					QFile file( TRIGGER );
					file.open(QIODevice::WriteOnly); // Or QIODevice::ReadWrite

					count++;
					break;
				}
				default:
				{
					break;
				}
			}
		} // loop
	} // exe
}; // GUI

//
// to manually trigger an execvution:
//
// type nul > trigger
//
// to kill diana executer process:
//
// taskkill /IM "diana_app_new.exe" /F
//
// 2020-03-19 - conclusions:
// 
// so far the implementation that works best is:
//
// - launch and end diana for each execution
// - create a new DI for each execution
// - wait few seconds to let DI threads start properly
// - don't set hearbeat pace too low (at least 150ms)
//
// DI: Diana Interface (Geomec side)
// DE: Diana Executer (diana_app side)
//
// many other ways tested (without good results):
//
// - keep both alive: DI and DE
// - keep only DI alive
// 
// second executions end up in dead locks, in one or other side
//
// timed_wait doesn't solve issue with short times
//

//
// guard: 0 (no need) / hb: 10 -> works fine, 2020-03-23
//
static const int TIME_GUARD = 0;//1*1000;


//
// ERROR: doing this safe queue std mutex crashes!!!!!
//
/*namespace
{
	void remove( DI* di )
	{
		di->m_thread_exe->join();
		Printer::instance()->info( "minimal_app_qt : di : exe thread : join : after" );

		delete di;
		di = nullptr;

		Printer::instance()->info( "minimal_app_qt : di : delete : after" );
	}

	/*DI* di create()
	{
		Printer::instance()->info( "minimal_app_qt : di : after new" );
		return new DI();
	}
};*/

//
// Google: interprocess mutex locked when process die
// https://www.google.com/search?q=interprocess+mutex+locked+when+process+die&rlz=1C1CHBF_enNL832NL832&oq=interprocess+mutex+locked+when+process+die&aqs=chrome..69i57.35431j0j7&sourceid=chrome&ie=UTF-8
// https://stackoverflow.com/questions/15772768/boost-interprocess-mutexes-and-checking-for-abandonment
//
// -> support for 'robust mutex' (only POSIX threads)
//

//
//
//
void Class_01::start()
{
	CSettingsIni::instance()->init();

	//Printer::multiprocess( true );
	std::string filename = "gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log";
	Printer::instance()->url( QUtil::url( QUtil::eUsrDir::Goemec, filename ) );
	Printer::instance()->info( "" );
	Printer::instance()->info( " main_app_qt >>>>>>" );
	Printer::instance()->info( "" );

	/*
	QString result;
	//
	// FIXME: only for windows
	//
	//const QString cmd = "cmd /c taskkill /IM diana_app_new.exe /F";
	const QString cmd = "cmd /c \"taskkill /IM /F \"diana_app_new.exe\"\"";

	gm::di::launch( "diana_app_new.exe" );

	std::this_thread::sleep_for(std::chrono::milliseconds( 5*1000 ));
	*/

	//
	//
	//
	CGUI gui; // fake
	//
	// dsu
	//
	CDianaStartUp* dsu = CDianaStartUp::instance();
	CEvents::TFunction_push push_cb = std::bind( &CGUI::push, &gui, std::placeholders::_1 );
	//
	// FIXME: nneds to be adapted to new events handler
	//
	//dsu->register_push_cb( push_cb );	

	dsu->SetDianaEnv(); 
	dsu->Print_DianaEnv();
	_g->dsa(true);
	//dsu->dsa( true );
	dsu->SetDianaEnv( CDianaStartUp::FFDIR, FFDIR );
	//
	dsu->di( nullptr );
	//DI* di = nullptr;
	//
	QFile file( TRIGGER );
	//
	//
	//

	for(;;)
	{
		if( file.exists() )
		{
			Printer::instance()->info( "" );
			Printer::instance()->info( "" );
			Printer::instance()->info( "" );
			Printer::instance()->info( "execution : request >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

			bool res = file.remove();
			Printer::instance()->debug( "minimal_app_qt : trigger : removed" );

			if( dsu->di() )
			{
				//remove( di );

				//
				// no need, doien in 1)the GUI or in 2)DI destructor
				//
				/*dsu->di()->m_thread_exe->join();
				Printer::instance()->info( "minimal_app_qt : exe : after join" );*/

				//delete di;
				//di = nullptr;


				//
				// FIXME: A
				//
				delete dsu->di();
				dsu->di( nullptr );

				Printer::instance()->info( "minimal_app_qt : di : after delete" );
			}

			std::this_thread::sleep_for(std::chrono::milliseconds( TIME_GUARD ));

			if( !dsu->di() )
			{
				dsu->di( new DI() );
				//di = new DI();
				//Printer::instance()->info( "minimal_app_qt : di : after new" );*/

				//di = create( di );
			}

			// needs to wait for the threads to be started
			std::this_thread::sleep_for(std::chrono::milliseconds( TIME_GUARD ));

			//
			// FIXME: needs to be adxapted to new events handler
			//
			//dsu->di()->push( cmd_f( eCmd::LaunchDiana ) );
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds( TIME_GUARD ));
		}
	}
}