#pragma once

#include <QObject>

class CDianaExecuterSignalHandler : public QObject
{
  Q_OBJECT

public:
  CDianaExecuterSignalHandler();
  virtual ~CDianaExecuterSignalHandler();
  bool GetCalculationResult();

private slots:
  void OnMessage(QString str);
  void OnAppSpcMessage(QString str);
  void OnError(QString str);
  void OnWarning(QString str);
  void OnFinished();

private:
  bool m_bFinished;
  bool m_bError;
};
