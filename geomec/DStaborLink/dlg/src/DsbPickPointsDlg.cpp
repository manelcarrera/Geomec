// DsbPickPointsDlg.cpp: implementation of the CDsbPickPointsDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DsbPickPointsDlg.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDsbPickPointsDlg::CDsbPickPointsDlg(std::list<CNewWellPoint> *pList, CNewWellPathBase &wellpath,
                                     std::list<std::list<INewWellSection *>> Sections, CDoubleQuantity::UNIT unit,
                                     CWnd *pParent)
    : CWellPointPickDlg(pList, wellpath, Sections, unit, pParent) {}

CDsbPickPointsDlg::~CDsbPickPointsDlg() {}

void CDsbPickPointsDlg::OnAddDefault(std::list<CNewWellPoint> &list) {
  const CNewWellPath *pNewWellPath = dynamic_cast<const CNewWellPath *>(m_pNewWellPath);
  assert(pNewWellPath);
  if (m_pNewWellPath) {
    pNewWellPath->AddDefaultDstaborPoints(list);
    m_WellPointListCtrl.UpdateList();
  }
}

CNewWellPoint CDsbPickPointsDlg::OnCreateWellPoint(CNewWellPathBase &wellpath, double tmd, bool &bDefined) {
  CNewGeoWellPoint Pt(dynamic_cast<CNewWellPath &>(wellpath), tmd);
  if (!Pt.Formation()) {
    bDefined = false;
    return Pt;
  }

  CFormationBase::TFormationType formtype = Pt.Formation()->FormationType();
  if (formtype != CFormationBase::FT_LIMESTONE && formtype != CFormationBase::FT_SHALE &&
      formtype != CFormationBase::FT_NOTSPECIFIED) {
    bDefined = false;
    Pt.Invalidate();
    return Pt;
  }

  bDefined = true;
  return Pt;
}

CString CDsbPickPointsDlg::OnOutOfBoundaryMessage() {
  return "Some points are not added. Either the points are outside any formations\nor some formations are not "
         "specified as limestone or shale.";
}
