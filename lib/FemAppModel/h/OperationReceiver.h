// OperationReceiver.h: interface for the COperationReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPERATIONRECEIVER_H__13C76075_9EEA_480B_91E1_B72C9D6103E3__INCLUDED_)
#define AFX_OPERATIONRECEIVER_H__13C76075_9EEA_480B_91E1_B72C9D6103E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>

class CFemAppModel;
class COperationReceiver : public QObject  
{
  Q_OBJECT
  bool m_bOperation;
protected slots:
  void slotBeginOperation();
  void slotEndOperation();
protected:
  virtual void OnBeginOperation();
  virtual void OnEndOperation();
public:
  COperationReceiver(CFemAppModel& model);
  virtual ~COperationReceiver();
  bool IsOperation() const;
};

#endif // !defined(AFX_OPERATIONRECEIVER_H__13C76075_9EEA_480B_91E1_B72C9D6103E3__INCLUDED_)
