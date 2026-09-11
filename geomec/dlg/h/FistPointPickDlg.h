// FistPointPickDlg.h: interface for the CFistPointPickDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FISTPOINTPICKDLG_H__6B1F5D24_10DE_4D91_97B1_F705E07573D1__INCLUDED_)
#define AFX_FISTPOINTPICKDLG_H__6B1F5D24_10DE_4D91_97B1_F705E07573D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NewWellPath.h"
#include "NewWellSection.h"
#include "WellDefinitionPointList.h"
#include "WellPoint.h"
#include "WellPointDrawSpec.h"
#include "WellPointList.h"
#include "WellPointPickDlg.h"
#include "WellSceneInterMed.h"
#include "WellSection.h"
#include "WellSectionDrawSpec.h"
#include "WellSectionList.h"
#include <qobject.h>

class CFistPointPickDlg : public CWellPointPickDlg {
public:
  // wjrx mantis 3564
  //
  CFistPointPickDlg(std::list<CNewWellPoint> *pList, CNewWellPath &wellpath,
                    std::list<std::list<INewWellSection *>> Sections, CDoubleQuantity::UNIT unit, CWnd *pParent = NULL);

  virtual ~CFistPointPickDlg();

  virtual CNewWellPoint OnCreateWellPoint(CNewWellPathBase &wellpath, double tmd, bool &bDefined);
  virtual void OnAddDefault(std::list<CNewWellPoint> &list);
  virtual CString OnOutOfBoundaryMessage();

private:
  CNewWellPath *m_pNewWellPath;
};

#endif // !defined(AFX_FISTPOINTPICKDLG_H__6B1F5D24_10DE_4D91_97B1_F705E07573D1__INCLUDED_)
