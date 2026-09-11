#pragma once

//std
#include <utility> // std::pair
#include <string>
#include <map>
#include <vector>
//qt
#include <QString>

// FIXME
#define WM_USER 0x0400
#define ID_ON_EVENT WM_USER+5000


////////////////////////////////////////////////
//
//						(1)
//				itp communication
//			new implementation with boost (Ini)
//
////////////////////////////////////////////////

// 
// for gm, this is more generic
//
typedef std::pair< int, void* > Cmd;
static Cmd make_cmd(int x, void* y = nullptr) { return Cmd(x, y); } // _f -> function

static const int EventsPerQueue = 100;


enum eDsaError
{
  UnableToLaunchDiana,
  HeartBeatError,
  EndDepletionCheckError
};

enum eQueues
{
  DianaInterface,
  GUI,
  RA
};

enum eCmd
{
  //////////////////////////////////
  //
  // 1) diana_app
  //
  //////////////////////////////////
  // a) requests: 
  // 1) internal 
  LaunchDiana,
  Start_HB,
  Quit_HB,
  Quit_Listen,
  DoNothing,
  // 2)to diana app
  RunDiana,
  HeartBeat,
  QuitDiana,

  // b) responses
  LaunchDiana_Ack_,
  RunDiana_Ack_,
  RunDiana_Done_,
  HeartBeat_Ack,
  HeartBeat_Error,
  QuitDiana_Ack,
  QuitDiana_Done,
  Quit_HB_Done,
  DianaMsg,

  //////////////////////////////////
  //
  // 2) run_analysis
  //
  //////////////////////////////////

  // GUI messages
  Log,				  // logs foprwarded to gui thread (doc)
  DisableViews,
  DisableViews_Done, // then run can be performed
  UpdateViews,
  SetParams,			  // run type: linear, etc
  Start,
  Run,
  Save,
  Save_Done,
  ChecksError,
  //
  Progress,
  //DeleteContext,

  //
  //only dsa
  //
  RunError,
  RunSuccess,			// end iteration
  RunEnd,				// end analysis
  CancelRun,

  //
  // Controller
  //
  Run_All,
  Run_Stage,
  Run_First,
  Run_Next,
  RunController,
  ClearController,	// batch tests only, need to be sent at the analysis end

  //
  //
  //
  Quit_Thread,	// IListener, used by:
          //	- CThreadWrapper in CMatParamDlg 
          //	- CDianaRunController thread
          //	- GeomecDoc
          //	- CRunAnalysis_CLI
          //	- CRunAnalysis


  //
  //
  //
  Question,
  Response,
  Status,
  //
  // events from here down forwarded from doc to RA: run analysus
  //
  Quit_RA,	// = EventsPerQueue*eQueues::RA,
        // TODO: modify this later
  Quit_RA_Done,
  //
  Test,		// Gui functional test
  Test_Ack,	// Geomec GUI acks the unittest order
  Test_Step,	// Feedback on command to be executed next
  Test_End,	// Geomec GUI sends it to the unittest when all test have run Ok
  Test_Command	// CGeomecDoc msg map
};

//
// FIXME:
//
static const std::string g_cmd_s[]
{
  //////////////////////////////////
  //
  // 1) diana_app
  //
  //////////////////////////////////
  // requests
  "LaunchDiana",		//0
  "Start_HB",			//1
  "Quit_HB",			//2
  "Quit_Listen",
  "DoNothing",
  "RunDiana",
  "HeartBeat",
  "QuitDiana",

  //responses
  "LaunchDiana_Ack",
  "RunDiana_Ack",
  "RunDiana_Done",
  "HeartBeat_Ack",
  "HeartBeat_Error",
  "QuitDiana_Ack",
  "QuitDiana_Done",
  "Quit_HB_Done",
  "DianaMsg",

  //////////////////////////////////
  //
  // 2) run_analysis
  //
  //////////////////////////////////

  //GUI messages
  "Log",					
  "DisableViews",		
  "DisableViews_Done",		
  "UpdateViews",			
  "SetParams",				
  "Start",				
  "Run",				
  "Save",				// memory -> .gm5 file
  "Save_Done",
  "ChecksError",				
  //
  "Progress",
  //"DeleteContext",		

  //
  //only dsa
  //
  "RunError",
  "RunSuccess",
  "RunEnd",
  "CancelRun",
  //


  //
  // Controller
  //
  "Run_All",
  "Run_Stage",
  "Run_First",
  "Run_Next",
  "RunController",
  "ClearController",
  //
  //
  //
  "Quit_Thread",
  //
  //
  //
  "Question",
  "Response",
  "Status",
  //
  // events from here forwarded from doc to RA: run analysus
  //
  "Quit_RA",
  "Quit_RA_Done",
  //
  "Test",
  "Test_Ack",
  "Test_Step",
  "Test_End",
  "Test_Command"
};
static const char* _cmd_s(eCmd cmd) {	return g_cmd_s[cmd].c_str(); }
// for conevnience
static const char* _cmd_s(int cmd) {	return _cmd_s((eCmd)cmd); }
static const char* _cmd_s(Cmd cmd) {	return _cmd_s(cmd.first); }


