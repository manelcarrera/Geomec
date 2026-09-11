#include "stdafx.h"

#include "RunAnalysis_NonDsa.h"

CRunAnalysis_NonDsa::CRunAnalysis_NonDsa() : CRunAnalysis()
{
}

void CRunAnalysis_NonDsa::handle(Cmd cmd)
{
  _e->notify(cmd,CEvents::GuiApp_EH);
}
