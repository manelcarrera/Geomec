#include "DianaClient.h"

#include "DianaStartUp.h"
#include "lbcp.h"

DianaClient::DianaClient(const QString &base, const QString &name, QObject *parent)
    : ModGMLocalBusClient(base, name, parent)
      //, m_timer(nullptr)
      ,
      m_dropped(0), m_hb_id(0) {}

DianaClient::~DianaClient() { delete m_timer; }

void DianaClient::diana_setup() {
  qDebug() << "Setup hb";
  m_hb_message.setFrom(m_name.toStdString().c_str());
  m_hb_message.setTo("Gui");

  m_timer = new QTimer();
  connect(m_timer, &QTimer::timeout, this, &DianaClient::run_diana);

  // go as fast as possible; or set interval otherwise
  m_timer->setSingleShot(true); // setInterval(1); // 10 + QRandomGenerator::global()->bounded(1000));
  m_timer->setInterval(1);
  m_timer->start();
}

// this one counts and checks if messages are dropped
// a previous version sent current time, and if there is throughput latency, the time in the Gui will be more and more
// behind
/*void DianaClient::heartbeat()
{
  qDebug() << "Client heartbeat";
  while (true)
  {
  m_hb_message.setPayload(m_hb_id++);

  if (!m_shm_socket.write(m_hb_message))
  {
      ++m_dropped;

      if (m_dropped >= 1)
      {
    qDebug() << m_name << " dropped " << m_dropped;
    m_dropped = 0;
      }
  }
  }
}*/

void DianaClient::run_diana() {
  qDebug() << "Client diana";

  QString Exe = "C:/Program Files/GEOMEC 5.7 - SVS/Diana/binseg/ap/gm42.exe";
  QString Path = "C:/Users/Manel.CarreraRuibal/AppData/Local/Temp/dra11072";
  // Path = "E:/Temp/c/dra12836";
  QString Dat;
  QString Com;
  QString Filos = "ff" + Path.right(6) + ".ff";
  QString Base = "DIANA";
  bool DisplayDefMessages = false;
  QString UserLeader;
  QString UserTrailer;

  // quick copy from GeomecModel/src/DianaEnv.cpp
  // DIASLIB is responsible for errors about non-existing element types
  // DIALIB was not set in call from command line, so maybe not needed
  // DIAERRPATH was not needed when DIASLIB was set
  QString strEnvDiaPath = "C:/Program Files/GEOMEC 5.7 - SVS/Diana";
  QString strEnvDiaShare = strEnvDiaPath + "/share";
  QString strEnvDiaLib = strEnvDiaPath + "/lib";
  QString strEnvDiaSLib = strEnvDiaShare + "/lib";

  vDiSetenv("%s=%s", "DIAPATH", strEnvDiaPath.toStdString().c_str());
  vDiSetenv("%s=%s", "DIASHARE", strEnvDiaShare.toStdString().c_str());
  vDiSetenv("%s=%s", "DIALIB", strEnvDiaLib.toStdString().c_str());
  vDiSetenv("%s=%s", "DIASLIB", strEnvDiaSLib.toStdString().c_str());

  // Note: DianaEnv.cpp should be in Glue, but it depends on registry; we may want to refactor that altogether (later)

  int res = CDianaStartUp::GetInstance()->RunDiana(Exe, Path, Dat, Com, Filos, Base, DisplayDefMessages, UserLeader,
                                                   UserTrailer);

  res = CDianaStartUp::GetInstance()->RunDiana(Exe, Path, Dat, Com, Filos, Base, DisplayDefMessages, "UserLeader",
                                               "UserTrailer");

  /*while (true)
  {
  m_hb_message.setPayload(m_hb_id++);

  if (!m_shm_socket.write(m_hb_message))
  {
      ++m_dropped;

      if (m_dropped >= 1)
      {
    qDebug() << m_name << " dropped " << m_dropped;
    m_dropped = 0;
      }
  }
  }*/
}
