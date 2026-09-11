
#include "DCasingDianaSignals.h"
#include "DCasing.h"

CDCasingDianaSignals::CDCasingDianaSignals(const QObject *sender, CDCasing *dCasing)
    : CDianaSignals(sender), m_dCasing(dCasing) {}

CDCasingDianaSignals::~CDCasingDianaSignals() {}

void CDCasingDianaSignals::onMessage(QString message) { m_dCasing->HandleMessage(message); }

void CDCasingDianaSignals::onAppSpcMessage(QString /*appSpcMessage*/) {}

void CDCasingDianaSignals::onError(QString error) { m_dCasing->HandleError(error); }

void CDCasingDianaSignals::onWarning(QString warning) { m_dCasing->HandleWarning(warning); }

void CDCasingDianaSignals::onStopExecuting(bool & /*stop*/) {}

void CDCasingDianaSignals::onFinished() { m_dCasing->HandleFinished(); }
