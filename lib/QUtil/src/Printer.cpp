#include "Printer.h"

#include "Printer_SP.h" // Single process
#include "Printer_MP.h" // Multi process
#include "Printer_MP_BOOST.h" // Multi process BOOST

#include "util_.h"	//FIXME
#include "Xml.h"
#include "QUtil.h"

#include <fstream>

#include <stdio.h>
#include <time.h>

#include <string>
#include <QDir>

//
// static const hasn't the proper value
//
//static const std::string LOGS_CONFIG_FILE = "logs.xml";
//


//////////////////////////////////////// 
// 
//			Printer::Config 
//
//////////////////////////////////////// 

Printer::Config::Config()
{
	// FIXME: static const hasn't the proper value
	std::string LOGS_CONFIG_FILE = "logs.xml";

	std::map<eModule,std::string> key_m = { 
		// general
		{ Sel ,"SEL" },
		{ Gen ,"GEN" },
		{ App ,"APP" },
		{ Doc ,"DOC" },
		{ DocT ,"DOC-T" },
		{ Shell ,"GM-SHELL" },
		{ Tests ,"TESTS" },
		{ Evt ,"EVT" },
		// run analysis
		{ RunA ,"RUN-A" },
		{ RunWF ,"RUN-WF" },
		{ RunC ,"RUN-C" },
		{ RunR ,"RUN-R" },
		{ RunRes ,"RUN-RES" },
		{ RunU ,"RUN-U" },
		{ RunW ,"RUN-W" },
		{ RunP ,"RUN-P" },
		// dsa
		{ DiaI ,"DIA-I" },
		{ DiaH ,"DIA-H" },
		{ DiaE ,"DIA-E" },
		// others
		{ Mr,	"MR" }};


	std::string cfg_file = QUtil::url( QUtil::eUsrDir::Goemec, LOGS_CONFIG_FILE );

	CXml cfg( cfg_file, {}, {	
		// types
		"ERROR","INFO","DEBUG", 
		// others
		"MAXFILESIZE", 

		// modules
		// general
		key_m[Sel],key_m[Gen],key_m[App],key_m[Doc],key_m[DocT],key_m[Shell],key_m[Tests],
		key_m[Evt],

		// run analysis, controller, runner, DianaStartUp
		key_m[RunA],key_m[RunWF],key_m[RunC],key_m[RunR],key_m[RunRes],key_m[RunU], key_m[RunW], key_m[RunP],

		// dsa
		key_m[DiaI], key_m[DiaH], key_m[DiaE],

		// others modules
		key_m[Mr] } );		

	error	= cfg.exist( "ERROR" );
	info	= cfg.exist( "INFO" );
	debug	= cfg.exist( "DEBUG" );



	// modules
	for (auto const& item : key_m)
	{
		m_module_enabled[item.first] = cfg.exist(item.second);

		//
		// debug/info/error : enabled/disabled
		//
		std::string val = cfg.value(item.second);
		m_module_DIE[item.first] = 
			val.empty() ?
				DIE{	false,
						false,false,false } :
				DIE{
						true,
						val.find('D') != std::string::npos,
						val.find('I') != std::string::npos,
						val.find('E') != std::string::npos };
	}

	max_file_size = 0;
	if( cfg.exist( "MAXFILESIZE" ) )
		if( util::number( cfg.value("MAXFILESIZE") ) )
			max_file_size = atof( cfg.value("MAXFILESIZE").c_str() );
	max_file_size = max_file_size == 0 ? FILE_MAX_LEN : max_file_size;


	/*const std::map<
		Printer::eModule,
		std::pair<std::string, std::string>>*/

	//general
	modules_s_m[Sel]	= { "[S    |        |  | ]", "Geomec selector" };
	modules_s_m[Gen]	= { "[ G   |        |  | ]", "General : Logs not belonging to any specific module" };
	modules_s_m[App]	= { "[  A  |        |  | ]", "Initializations" };

	//workflows
	modules_s_m[Doc]	= { "[   D |        |  | ]", "Geomec doc : main workflow : gui" };
	modules_s_m[DocT]	= { "[    d|        |  | ]", "Geomec doc thread" };
	modules_s_m[Shell]	= { "[   H |        |  | ]", "gm_shell : main workflow : text" };
	modules_s_m[Tests]	= { "[   T |        |  | ]", "Run tests" };

	modules_s_m[Evt]	= { "[    e|        |  | ]", "Events" }; // gui-only
	//run
	modules_s_m[RunA]	= { "[     |A       |  | ]", "Run analysis gui workflow" };
	modules_s_m[RunWF]	= { "[     |W       |  | ]", "Run analysis non-gui workflow" };
	modules_s_m[RunC]	= { "[     | C      |  | ]", "Controller" };
	modules_s_m[RunR]	= { "[     |  R     |  | ]", "Runner" };
	modules_s_m[RunU]	= { "[     |   U    |  | ]", "Diana start up : the one that executes diana" };
	modules_s_m[RunRes] = { "[     |    r   |  | ]", "Results" };
	modules_s_m[RunW]	= { "[     |     W  |  | ]", "Progress bar wrapper" };
	modules_s_m[RunP]	= { "[     |      P |  | ]", "Progress bar" };
	//dsa
	modules_s_m[DiaI]	= { "[     |        |I | ]", "Diana Interface, DSA Geomec app side" };
	modules_s_m[DiaH]	= { "[     |       b|  | ]", "Diana app heartbeat" };
	modules_s_m[DiaE]	= { "[     |        | E| ]", "Diana Exe, DSA diana_app side" };
	//others
	modules_s_m[Mr]		= { "[     |        |  |M]", "Mesh refinement" };
}  

//////////////////////////////////////// 
// 
//				Printer 
//
//////////////////////////////////////// 

qint64 Printer::m_pid=0;

void Printer::url( std::string val )
{ 
	QFile f( QString::fromStdString( val ) );
	QFileInfo fileInfo( f.fileName() );

	QString filename(fileInfo.fileName());

	m_url = val; 

	m_file = fileInfo.fileName().toStdString(); 
	m_folder = fileInfo.absolutePath().toStdString(); 
};

void Printer::file( const std::string val )
{ 
	m_file = val; 
	m_url = QUtil::url( m_folder, m_file ); 
}

void Printer::folder( const std::string val )
{ 
	m_folder = val; 
	m_url = QUtil::url( m_folder, m_file ); 
}

Printer::~Printer()
{
}

Printer::Printer()
	: m_b_multi_process( false )
{
	url( QUtil::url( QUtil::eUsrDir::Goemec, "geomec.log" ) ); // sets: m_url, m_file, m_folder

	m_count = 0;

	if( !util::dir_exists( m_folder ) )
	{
		//util::create_dir( path );
		bool res = QDir().mkpath( QString::fromStdString( m_folder ) );
		//Printer::instance()->debug("Default log folder created: %s", path.c_str() );
	}
}

// factory
Printer* get_printer()
{
	std::string LOGS_CONFIG_FILE = "logs.xml";
	std::string KEY = "MULTIPROCESS";
	
	std::string cfg_file = QUtil::url( QUtil::eUsrDir::Goemec, LOGS_CONFIG_FILE );
	CXml cfg( cfg_file, {}, { KEY } );

	bool b_multi_process = cfg.exist( KEY );

	Printer* p = nullptr;

	if( b_multi_process )
		//p = new Printer_MP();
		p = new Printer_MP_BOOST();
	else
		p = new Printer_SP();

	p->m_b_multi_process = b_multi_process;
	return p;
}

Printer *Printer::instance( eModule module_ )
{
	static Printer* printers_a[ModulesNum]={nullptr,nullptr,nullptr};

	if (!printers_a[module_])
	{
		Printer* p = get_printer();
		printers_a[module_] = p;

		p->m_cfg.m_module = module_;
		p->m_module_s = p->m_cfg.modules_s_m.at(module_).first;
		p->m_multi_process_s = p->m_b_multi_process ? "[M]":"[S]";

		//
		// modules can override general debug/info/error settings
		//
		if(p->m_cfg.m_module_DIE[module_].override)
		{
			p->m_cfg.debug= p->m_cfg.m_module_DIE[module_].debug;
			p->m_cfg.info = p->m_cfg.m_module_DIE[module_].info;
			p->m_cfg.error = p->m_cfg.m_module_DIE[module_].error;
		}
	}
	return printers_a[module_];
}

bool Printer::swap_file()
{
	if( m_count % 10 == 0 )
	{
		long long size =  util::GetFileSize( m_url );
		if( size > m_cfg.max_file_size )
		{
			rename( m_url.c_str(), (m_url+".OLD").c_str() );
			m_count = 0;
			return true;
		}
	}
	return false;
}

//
// M/S: Multiporces / Single process
//
// -Type:
// D: Debug
// I: Info
// E: Error
//
// -Module:
// S: Selector
// G: General
// A: App (GeemcApp)
// D: Doc (gui workflow)
// H: gm_shell (non-gui workflow)
// T: Tests app (non-gui, uses non-gui workflow)
// B: Events broker
//
// | 
//
// A: run analysis gui workflow
// W: run analysis non-gui workflow
// C: Controller
// R: Runner
// U: DianaStartUp (the one that runs diana)
// r: Results
// W: Progress bar wrapper
// P: Progress bar Qt implementation
//
// |
//
// -Next ppecific for dsa-
// I: Diana Inetrface (events from Geomec to diana_app)
// b: Diana app heartbeat
// E: Diana Exe (diana_app sedparated process)
//
// |
//
// M: Mesh refinement
//
// 2020-04-24 18:18:26 - [M][DIE][SGAD|ACR|IE|M] on_event : Log
//


//
// https://stackoverflow.com/questions/27136854/c11-actual-system-time-with-milliseconds
//
// new implementation with milliseconds (C++11)
//
std::string datetime_()
{
    using namespace std;
    using namespace std::chrono;

    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();

    tp -= duration_cast<seconds>(tp);

    time_t tt = system_clock::to_time_t(now);

	//tm t = *gmtime(&tt); // universal
	tm t = *localtime(&tt); // local (better)

	char tmp[100];
	std::snprintf( tmp, 100, 
		"[%04u-%02u-%02u %02u:%02u:%02u.%03u]", 
		t.tm_year + 1900,
        t.tm_mon + 1, 
		t.tm_mday, 
		t.tm_hour, 
		t.tm_min, 
		t.tm_sec,
        static_cast<unsigned>(tp / milliseconds(1)) );
	return tmp;

}

//
// old implementation with milliseconds
//
std::string datetime__()
{
	time_t timer;
    char buffer[128];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
	//
	// why 26?
	//
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S - ", tm_info);
	return buffer;
}

#include <iostream> //cout
void Printer::print( const char* cp, eType type )
{
	m_count++;

	swap_file();

	std::ofstream myfile;
	myfile.open( m_url, std::ios_base::app );

	myfile	<< datetime_()
			<< m_multi_process_s 
			<< (type == Debug ? "[D  ]" : type == Info ? "[ I ]" : "[  E]")
			<< (m_module_s + " ")
			<< cp 
			<< std::endl;

	myfile.close();

	//
	// logs to std out
	//
	/*std::cout	<< datetime_()
				<< m_multi_process_s 
				<< (type == Debug ? "[D  ]" : type == Info ? "[ I ]" : "[  E]")
				<< (m_module_s + " ")
				<< cp 
				<< std::endl;*/
}