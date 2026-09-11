
#include <cassert>

#include <qstring.h>

#include "GeomecStringTable.h"
#include "Global.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "TSSurfaceProgress.h"
#include "resourceIDS.h"

CTSSurfaceProgress::CTSSurfaceProgress() : m_pWaitCursor(0), m_progress(0) {}

CTSSurfaceProgress::~CTSSurfaceProgress() {
  // must call End()
  assert(m_progress.get() == 0);
  assert(m_pWaitCursor.get() == 0);
}

void CTSSurfaceProgress::Begin(int nSteps) {
  QString text;
  text = getStringTableEntry(IDS_CREATINGSURFACE);
  m_progress.reset(_g->prog()->create(eProgress::MainFrame));
  m_progress->StatusMessage(text);
  m_progress->AddSteps(nSteps);

  m_pWaitCursor.reset(_g->prog()->create(eProgress::Wait));
}

void CTSSurfaceProgress::End() {
  m_progress.reset();
  m_pWaitCursor.reset();
}

void CTSSurfaceProgress::Step() { m_progress->Step(); }
