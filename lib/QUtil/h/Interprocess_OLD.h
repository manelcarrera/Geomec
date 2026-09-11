#pragma once

#include <QString> // (2)

////////////////////////////////////////////////
//				
//						(2)
//				itp communication
//			OLD implementation with Qt
//
////////////////////////////////////////////////


enum BusClientCmd // Both request and ack
{
  LaunchDiana_Req,
  RunDiana_Req,
  StopDiana_Req,
  DianaHeartbeat,

  LaunchDiana_Ack,
  RunDiana_Done,
  StopDiana_Ack,
};
static const std::string g_bus_client_cmd_s[]
{
  "LaunchDiana_Req",
  "RunDiana_Req",
  "StopDiana_Req",
  "DianaHeartbeat",
  "LaunchDiana_Ack",
  "RunDiana_Done",
  "StopDiana_Ack",
};

static const QString DIANA_ID		= "Diana";
static const QString LAUNCHER_ID	= "Launch";
static const QString GEOMEC_ID		= "Geomec";

static const QString DIANA_BIN		= "diana_app.exe";
static const QString LAUNCHER_BIN	= "geomec_launcher.exe";
static const QString GEOMEC_BIN		= "Geomec.exe";

static const QString SHARED_MEMORY_RUN_DIANA_KEY = "KEY_1";

//Icons
enum DSA_Icon : int { _1P, _2P, Running, Crash, Empty };
static const std::string DSA_Icon_s_v[] = { "_1P", "_2P", "Running", "Crash", "Empty" };
