// DCasintSlotHandler.h: interface for the CDCasintSlotHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINTSLOTHANDLER_H__4F5F5C3A_421B_4ADE_A3DE_CA15FF125C86__INCLUDED_)
#define AFX_DCASINTSLOTHANDLER_H__4F5F5C3A_421B_4ADE_A3DE_CA15FF125C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>

class CDCasing;
class CDCasintSlotHandler;
class CSlotHandlerDianaSignals;

class CSlotHandler : public QObject
{
  Q_OBJECT;
  
  private :
  
    CDCasintSlotHandler & m_Parent;
    CSlotHandlerDianaSignals* m_slotHandlerDianaSignals;
  
  public :

    CSlotHandler(CDCasintSlotHandler & Handler);
    ~CSlotHandler();
  
    void Connect(const CDCasing & Casing);
    void Disconnect();

  public: 

    void slotOnMessage(QString str);
    void slotOnError(QString str);
    void slotOnWarning(QString str);
    void slotOnFinished();
};

class CDCasintSlotHandler
{

  //object handler
  
  friend  class CSlotHandler;

private :
    CSlotHandler m_SlotHandler;

protected :

  virtual void OnMessage(QString str);
  virtual void OnError(QString str);
  virtual void OnWarning(QString str);
  virtual void OnFinished();


public :

  CDCasintSlotHandler();
  ~CDCasintSlotHandler();

  //members
  void Connect(const CDCasing & Casing);
  void Disconnect();

};

#endif // !defined(AFX_DCASINTSLOTHANDLER_H__4F5F5C3A_421B_4ADE_A3DE_CA15FF125C86__INCLUDED_)
