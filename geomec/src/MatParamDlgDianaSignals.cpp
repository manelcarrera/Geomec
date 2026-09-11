#include "stdafx.h"

#include "MatParamDlg.h"
#include "MatParamDlgDianaSignals.h"

CMatParamDlgDianaSignals::CMatParamDlgDianaSignals(const QObject *sender, CMatParamDlg *matParamDlg)
    : CDianaSignals(sender), m_matParamDlg(matParamDlg) {}

CMatParamDlgDianaSignals::~CMatParamDlgDianaSignals() {}

void CMatParamDlgDianaSignals::onMessage(QString message) {
  m_matParamDlg->OnDianaXDiaMessage(message.toStdString().c_str());
}

void CMatParamDlgDianaSignals::onAppSpcMessage(QString appSpcMessage) {}

void CMatParamDlgDianaSignals::onError(QString error) { m_matParamDlg->OnDianaXDiaError(error.toStdString().c_str()); }

void CMatParamDlgDianaSignals::onWarning(QString warning) {
  m_matParamDlg->OnDianaXDiaWarning(warning.toStdString().c_str());
}

void CMatParamDlgDianaSignals::onStopExecuting(bool &stop) {}

void CMatParamDlgDianaSignals::onFinished() { m_matParamDlg->OnDianaXCloseDialog(); }
