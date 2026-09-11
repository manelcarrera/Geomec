#include "stdafx.h"

#include "FemAppMainWindow.h"
#include "MainFrm.h"
#include "StatusMessageW.h"

void statusMessageW(const QString &message) {
  CMainFrame *mainFrame = static_cast<CMainFrame *>(FemAppGetMainWnd());

  mainFrame->StatusBar().SetWindowText(message.toStdString().c_str());
}
