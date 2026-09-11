// FistPointPickDlg.cpp: implementation of the CFistPointPickDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "FistPointPickDlg.h"
#include "NewWellPath.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFistPointPickDlg::CFistPointPickDlg(std::list<CNewWellPoint> *pList, CNewWellPath &wellpath,
                                     std::list<std::list<INewWellSection *>> Sections, CDoubleQuantity::UNIT unit,
                                     CWnd *pParent)
    : CWellPointPickDlg(pList, wellpath, Sections, unit, pParent), m_pNewWellPath(&wellpath) {}

CFistPointPickDlg::~CFistPointPickDlg() {}

/*virtual*/ CString CFistPointPickDlg::OnOutOfBoundaryMessage() {
  return "Some points are not added. Either the points are outside any defined reservoirs or no reservoir is specified "
         "as being a sand.";
}

CNewWellPoint CFistPointPickDlg::OnCreateWellPoint(CNewWellPathBase &wellpath, double tmd, bool &bDefined) {
  CNewGeoWellPoint Pt(dynamic_cast<CNewWellPath &>(wellpath), tmd);
  if (!Pt.Defined()) {
    bDefined = false;
    return Pt;
  }

  CFormationBase::TFormationType formtype = Pt.Formation()->FormationType();
  if (!Pt.Formation()->Depleting() ||
      (formtype != CFormationBase::FT_SAND && formtype != CFormationBase::FT_SANDSTONE &&
       formtype != CFormationBase::FT_NOTSPECIFIED)) {
    bDefined = false;
    Pt.Invalidate();
    return Pt;
  }

  bDefined = true;
  return Pt;
}

// wjrx mantis 3564
//
void CFistPointPickDlg::OnAddDefault(std::list<CNewWellPoint> &list) {
  m_pNewWellPath->AddDefaultFistPoints(list);
  m_WellPointListCtrl.UpdateList();
}
