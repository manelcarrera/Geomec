#include "stdafx.h"

#include "GeomecDoc.h"
#include "GeomecDocDianaSignals.h"

CGeomecDocDianaSignals::CGeomecDocDianaSignals(const QObject *sender, CGeomecDoc *geomecDoc)
    : CDianaSignals(sender), m_geomecDoc(geomecDoc) {}

CGeomecDocDianaSignals::~CGeomecDocDianaSignals() {}

void CGeomecDocDianaSignals::onMessage(QString message) {
  m_geomecDoc->OnDianaXDiaMessage(message.toStdString().c_str());
}

void CGeomecDocDianaSignals::onAppSpcMessage(QString appSpcMessage) {}

void CGeomecDocDianaSignals::onError(QString error) { m_geomecDoc->OnDianaXDiaError(error.toStdString().c_str()); }

void CGeomecDocDianaSignals::onWarning(QString warning) {
  m_geomecDoc->OnDianaXDiaWarning(warning.toStdString().c_str());
}

void CGeomecDocDianaSignals::onStopExecuting(bool &stop) {}

void CGeomecDocDianaSignals::onFinished() { m_geomecDoc->OnDianaXCloseDialog(); }
