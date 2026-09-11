#include "RunDianaWorker.h"

#include "DianaStartUp.h" //FIXME
#include "GeomecBusClient.h"
#include "Printer.h"

#include <QCoreApplication>

#include <QFile>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Functions
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
void heartbeat_(GeomecBusClient *client) {
  QDateTime now = QDateTime::currentDateTime();
  int secs = client->m_heartbeat_timestamp.secsTo(now);
  int msecs = client->m_heartbeat_timestamp.msecsTo(now);

  /*Printer::instance()->info( "[Geomec] QDianaStartUp > hearbeat_ > timestamp:'%s' now:'%s' > msec:%d",
    client->m_heartbeat_timestamp.toString("hh:mm:ss.zzz").toStdString().c_str(),
    now.toString("hh:mm:ss.zzz").toStdString().c_str(),
    msecs );*/

  if (secs > 4) // FIXME: Too low, just to make it fail
  {
    client->m_heartbeat_error = true;
    client->status(GeomecBusClient::eStatus::Error);
    Printer::instance()->info("[Geomec] CDianaStartUp > hearbeat_ : ERROR : secs:%d ]", secs);
  } else {
    // Printer::instance()->info( "QDianaStartUp > hearbeat_ : Ok : secs:%d ]", secs );
  }
}

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// RunDianaWorker
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This could maybe go in three theads one per stage
// 1. Launch diana_app
// 2. Launch heartbeat
// 3. Run Diana
// 4. Stop diana

// This can be also managed by cond_vars

RunDianaWorker::RunDianaWorker(const QString &path, int pid) : m_path(path), m_pid(pid), m_res(false) {}

RunDianaWorker::~RunDianaWorker() {
  // free resources
};

// this works just by polling but steps should be asyn implemented with var conditions
void RunDianaWorker::process() {
  GeomecBusClient *client = nullptr;

  QString name(GEOMEC_ID);
  QString base = ModGMLocalBusHelper::getBase(m_pid);

  client = new GeomecBusClient(
      base, name); // this_->m_qapp -> no difference passing the parent in terms of events handling -> so don't pass it

  QTimer::singleShot(0, client, SLOT(setup())); // needed to rergister the client into the bus
  QCoreApplication::instance()->processEvents();
  // ------------------------------------------------------------------------ (1) req: LAUNCH
  client->path(m_path);
  QTimer::singleShot(3 * 1000, [=]() { client->send(LAUNCHER_ID, BusClientCmd::LaunchDiana_Req); });
  QCoreApplication::instance()->processEvents();

  // waiting diana launched
  while (client->status() == GeomecBusClient::Stopped) {
    Printer::instance()->info("[Geomec] D-Worker > Stopped");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    QCoreApplication::instance()->processEvents();
  }
  // ------------------------------------------------------------------------ (2) req: RUN

  /*client->data( m_path );
  client->status( GeomecBusClient::Running );
  client->result_available( false );
  //client->send( DIANA_ID, BusClientCmd::RunDiana_Req );
  QTimer::singleShot( 0, [=]() { client->send( LAUNCHER_ID, BusClientCmd::RunDiana_Req ); } );*/

  QTimer::singleShot(3 * 1000, client, SLOT(send_run_diana_request()));
  QCoreApplication::instance()->processEvents();

  // heartbeat
  QTimer *timer = new QTimer();
  QObject::connect(timer, &QTimer::timeout, [=]() { heartbeat_(client); });
  // comment next line to disable heartbeat
  timer->start(2 * 1000);

  client->m_heartbeat_error = false;
  client->m_heartbeat_timestamp = QDateTime::currentDateTime();
  //

  // waiting run results
  int count = 0;
  while (client->status() == GeomecBusClient::Running && count < 15) // FIXME
  {
    // Printer::instance()->info( "run_diana_task : waiting... result available:%d", client->m_result_available );
    // ///something was going wrong with 'client->m_result_available' update
    Printer::instance()->info("[Geomec] D-Worker > Running");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count++;
    QCoreApplication::instance()->processEvents();
  }
  Printer::instance()->info("[Geomec] D-Worker > [ result_available:%d, count:%d ]", client->m_result_available, count);

  if (client->m_result_available)
    Printer::instance()->info("[Geomec] D-Worker > Succeded");

  delete timer; // heratbeat
  // ------------------------------------------------------------------------ // req: STOP
  QTimer::singleShot(3 * 1000, [=]() { client->send(LAUNCHER_ID, BusClientCmd::StopDiana_Req); });
  QCoreApplication::instance()->processEvents();

  // waiting diana to be stopped
  while (client->status() != GeomecBusClient::Stopped) {
    Printer::instance()->info("[Geomec] D-Worker > Not Stopped");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    QCoreApplication::instance()->processEvents();
  }

  m_res = !client->m_heartbeat_error && client->m_result_available;

  emit finished();
  Printer::instance()->debug("emitted");
}