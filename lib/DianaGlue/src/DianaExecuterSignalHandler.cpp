#include "DianaExecuterSignalHandler.h"

#include "DianaStartUp.h"
#include "GlobalMessage.h"

CDianaExecuterSignalHandler::CDianaExecuterSignalHandler()
: m_bFinished(false),
  m_bError(false)
{
  CDianaStartUp* dsu = CDianaStartUp::instance();

  connect( dsu, SIGNAL(OnMessage(      QString)), this, SLOT(OnMessage(      QString)));
  connect( dsu, SIGNAL(OnAppSpcMessage(QString)), this, SLOT(OnAppSpcMessage(QString)));
  connect( dsu, SIGNAL(OnError(        QString)), this, SLOT(OnError(        QString)));
  connect( dsu, SIGNAL(OnWarning(      QString)), this, SLOT(OnWarning(      QString)));
  connect( dsu, SIGNAL(OnFinished(            )), this, SLOT(OnFinished(            )));
}

CDianaExecuterSignalHandler::~CDianaExecuterSignalHandler()
{
  CDianaStartUp* dsu = CDianaStartUp::instance();

  disconnect(dsu, SIGNAL(OnFinished()), this, SLOT(OnFinished()));
  disconnect(dsu, SIGNAL(OnWarning(QString)), this, SLOT(OnWarning(QString)));
  disconnect(dsu, SIGNAL(OnError(QString)), this, SLOT(OnError(QString)));
  disconnect(dsu, SIGNAL(OnAppSpcMessage(QString)), this, SLOT(OnAppSpcMessage(QString)));
  disconnect(dsu, SIGNAL(OnMessage(QString)), this, SLOT(OnMessage(QString)));
}

bool CDianaExecuterSignalHandler::GetCalculationResult()
{ 
  return (m_bFinished && !m_bError); 
}

void CDianaExecuterSignalHandler::OnMessage(QString str)
{
  _m()->outstream() << str.toStdString() << std::endl; 
}
void CDianaExecuterSignalHandler::OnAppSpcMessage(QString str)
{ 
  _m()->outstream() << str.toStdString() << std::endl; 
}

void CDianaExecuterSignalHandler::OnError(QString str)
{
  _m()->outstream() << str.toStdString() << std::endl;
  m_bError = true;
}

void CDianaExecuterSignalHandler::OnWarning(QString str)
{ 
  _m()->outstream() << str.toStdString() << std::endl; 
}

void CDianaExecuterSignalHandler::OnFinished()
{ 
  m_bFinished = true; 
}
