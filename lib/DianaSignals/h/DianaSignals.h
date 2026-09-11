#ifndef _DianaSignals_h_
#define _DianaSignals_h_

#include <qobject.h>

class CDianaSignals : public QObject
{
  Q_OBJECT

public:
  CDianaSignals(const QObject* sender);
  ~CDianaSignals();

  virtual void onMessage(QString message) = 0;
  virtual void onAppSpcMessage(QString appSpcMessage) = 0;
  virtual void onError(QString error) = 0;
  virtual void onWarning(QString warning) = 0;
  virtual void onStopExecuting(bool &stop) = 0;
  virtual void onFinished() = 0;

private slots:
  void slotMessage(QString message);
  void slotAppSpcMessage(QString appSpcMessage);
  void slotError(QString error);
  void slotWarning(QString warning);
  void slotStopExecuting(bool &stop);
  void slotFinished();

private:
  CDianaSignals(const CDianaSignals& rhs);
  CDianaSignals& operator = (const CDianaSignals& rhs);

  const QObject* m_sender;
};

#endif  // _DianaSignals_h_
