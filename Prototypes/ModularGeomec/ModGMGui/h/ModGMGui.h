#pragma once

#include "ui_ModGMGui.h"
#include <QtCore/QHash>
#include <QtWidgets/QMainWindow>

#include "ModGMBus.h"

class ModGMGui : public QMainWindow {
  Q_OBJECT

public:
  ModGMGui(QWidget *parent = Q_NULLPTR);

signals:
  void finished();

  void receivedMessage(const ModGMBusMessage &);
  void handleMessage(const ModGMBusMessage &);

public slots:
  void onReceivedError(const QString &error);

private slots:
  void onReceivedMessage(const ModGMBusMessage &message);
  void onClicked();

protected:
  virtual void closeEvent(QCloseEvent *event);

private:
  Ui::ModGMGuiClass ui;
  QHash<QString, int> m_ids;
};
