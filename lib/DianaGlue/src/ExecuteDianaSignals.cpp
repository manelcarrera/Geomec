#include "ExecuteDianaSignals.h"

//#include "ExecuteDianaDialog.h"
#include "IExecuteDianaDialog.h"

CExecuteDianaSignals::CExecuteDianaSignals( const QObject* sender,
											IExecuteDianaDialog* exe_diana_dlg)
: CDianaSignals(sender), m_exe_diana_dlg( exe_diana_dlg )
{
}

CExecuteDianaSignals::~CExecuteDianaSignals(){}

void CExecuteDianaSignals::onMessage(QString message){				m_exe_diana_dlg->OnMessage(message); }
void CExecuteDianaSignals::onAppSpcMessage(QString appSpcMessage){	m_exe_diana_dlg->OnAppSpcMessage(appSpcMessage); }
void CExecuteDianaSignals::onError(QString error){					m_exe_diana_dlg->OnError(error); }
void CExecuteDianaSignals::onWarning(QString warning){				m_exe_diana_dlg->OnWarning(warning); }
void CExecuteDianaSignals::onStopExecuting(bool &stop){				m_exe_diana_dlg->OnStopExecuting(stop); }
void CExecuteDianaSignals::onFinished(){							m_exe_diana_dlg->OnFinished(); }
