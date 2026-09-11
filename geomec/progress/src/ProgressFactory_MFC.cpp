#include "stdafx.h"

#include "ProgressFactory_MFC.h"

#include "WaitProgress.h" // for CProgressFactory_MFC
#include "GeoProgress.h"
#include "GraphMainFrameProgress.h" //defined in 'FemAppGUI' lib
#include "DualProgress.h"
#include "DualSilentProgress.h"
#include "SilentProgress.h"

#include "FemAppMainWindow.h" //FemAppGetMainWnd()->GetActiveWindow()

  IProgressBase* CProgressFactory_MFC::create(
    eProgress type_,
    const QString& title, 
    bool cancel,
    int jobs)
  { 
    return create_imp(type_, title, cancel, jobs);
  }

  IProgressBase* CProgressFactory_MFC::create_imp(
    eProgress type_,
    const QString& title,
    bool cancel,
    int jobs)
  {
    IProgressBase* prg = nullptr;
    switch (type_)
    {
      case Geo:			prg = new CGeoProgress(FemAppGetMainWnd()->GetActiveWindow(), title, cancel); break;

      case Wait:			prg = new CWaitProgress; break;
      case MainFrame:		prg = new CGraphMainFrameProgress(); break;
      //
      // also title as int impl
      //
      case Dual:			prg = new CDualProgress(jobs, title, cancel); break;
      //
      // not present in the original code
      //
      case DualSilent:	prg = new CDualSilentProgress; break;
      case Silent:		prg = new CSilentProgress; break;
      //
      // FIXME
      //
      default:			prg = CProgressFactory::create_imp(type_, title, cancel, jobs);
    }
    return prg;
  }