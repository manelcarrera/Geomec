#include "stdafx.h"

#include "StatusMessageW.h"
#include "MainFrm.h"
#include "FemAppMainWindow.h"

void statusMessageW(const QString& message)
{
  CMainFrame* mainFrame = static_cast <CMainFrame*> (FemAppGetMainWnd());

  mainFrame->StatusBar().SetWindowText(message.toStdString().c_str());
}
