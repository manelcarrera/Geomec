#include "HeartbeatClient.h"




HeartbeatClient::HeartbeatClient(const QString& base, const QString& name, QObject *parent)
  : ModGMLocalBusClient(base, name, parent)
  , m_timer(nullptr)
  , m_dropped(0)
  , m_hb_id(0)
{
}

HeartbeatClient::~HeartbeatClient()
{
  delete m_timer;
}

void HeartbeatClient::heartbeat_setup()
{
  qDebug() << "Setup hb";
  m_hb_message.setFrom(m_name.toStdString().c_str());
  m_hb_message.setTo("Gui");

  m_timer = new QTimer();
  connect(m_timer, &QTimer::timeout, this, &HeartbeatClient::heartbeat);

  // go as fast as possible; or set interval otherwise
  m_timer->setSingleShot(true);// setInterval(1); // 10 + QRandomGenerator::global()->bounded(1000));
  m_timer->setInterval(1);
  m_timer->start();
}

// this one counts and checks if messages are dropped
// a previous version sent current time, and if there is throughput latency, the time in the Gui will be more and more behind
void HeartbeatClient::heartbeat()
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
}

