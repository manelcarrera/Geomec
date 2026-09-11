#include "ModGMGui.h"
#include <QCloseEvent>

ModGMGui::ModGMGui(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);

  connect(ui.pushButton, &QPushButton::clicked, this, &ModGMGui::onClicked);
  connect(this, &ModGMGui::receivedMessage, this, &ModGMGui::onReceivedMessage);
}

void ModGMGui::closeEvent(QCloseEvent *event)
{
  emit finished();
  event->accept();
}


void ModGMGui::onReceivedMessage(const ModGMBusMessage& message)
{
  static int x = 0;
  QString from = message.from();
  int id = message.payloadAsInt();
  int old_id = -1;

  if (m_ids.contains(from))
    old_id = m_ids.value(from);

  m_ids.insert(from, id);

  ++x;

  if (id != old_id + 1)
    ui.label->setText(QString("lost ") + QString::number(id - old_id + 1));
  else
    ui.label->setText(QString::number(x));
}

void ModGMGui::onReceivedError(const QString& error)
{
  ui.label->setText(error);
}

void ModGMGui::onClicked()
{
  ModGMBusMessage msg(ModGMBusMessage::SIGNAL, "Gui", "*");
  msg.setPayload(ui.lineEdit->text().toStdString().c_str());
  emit handleMessage(msg);
}