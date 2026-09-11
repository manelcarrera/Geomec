#include "IRunAnalysis.h"
// own
#include "Global.h"
#include "Printer.h"

IRunAnalysis::IRunAnalysis(const IListenerDefinition &d) : IRunAnalysis(d.eh_id, d.events_v) {}

IRunAnalysis::IRunAnalysis(CEvents::eEventsHandler eh_id, std::vector<eCmd> events_v_) : IListener(eh_id, events_v_) {
  _g->status(CGlobal::Running);
}

IRunAnalysis::~IRunAnalysis() {
  m_printer->debug("delete i_ra >>");
  _g->status(CGlobal::Idle);
  m_printer->debug("<< delete i_ra");
}