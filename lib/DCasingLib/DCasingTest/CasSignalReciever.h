// CasSignalReciever.h: interface for the CCasSignalReciever class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASSIGNALRECIEVER_H__187E07F6_E7E5_48E9_AA6A_CE66858BB914__INCLUDED_)
#define AFX_CASSIGNALRECIEVER_H__187E07F6_E7E5_48E9_AA6A_CE66858BB914__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>
class CDCasing;

class CCasingSignalReciever :public QObject 
{

  Q_OBJECT;
public:
  CCasingSignalReciever(CDCasing * obj = NULL);
  ~CCasingSignalReciever();
public slots:
  void OnMessage(QString str);
  void OnError(QString str);
  void OnWarning(QString str);
  void OnFinished();

private : 
  CDCasing * m_obj;
};


#endif // !defined(AFX_CASSIGNALRECIEVER_H__187E07F6_E7E5_48E9_AA6A_CE66858BB914__INCLUDED_)
