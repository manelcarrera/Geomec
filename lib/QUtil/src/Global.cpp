#include "Global.h"

#include "IProgressFactory.h"

CGlobal::CGlobal() : m_dsa(false), m_dsa_available(false), m_status(Idle), m_qt_dlg_bg_color("grey") {
  m_prog = new EmptyProgressFactory;

  m_mfc_v[RunDiana] = false;
  m_mfc_v[Progress] = false;
}

CGlobal::~CGlobal() {
  if (m_prog)
    delete m_prog;
}

void CGlobal::prog(IProgressFactory *val) {
  if (m_prog)
    delete m_prog;
  m_prog = val;
}

CGlobal *CGlobal::instance() {
  static CGlobal *_instance = new CGlobal();
  return _instance;
}