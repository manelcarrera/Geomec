// attricolorscaleentrydlg.cpp : implementation file
//

#include "attricolorscaleentrydlg.h"
#include "ColorGradient_Delegate.h"
#include "ColorScaleEntry_Delegate.h"
#include "HotSpot.h"
#include "HotSpot_Delegate.h"
#include "ISceneWrapper.h"
#include "colorgradient.h"
#include "colorscaleentry.h"
#include "geomec.h"
#include "modelbase.h"
#include "resourceIDS.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriColorScaleEntryDlg dialog

CAttriColorScaleEntryDlg::CAttriColorScaleEntryDlg(CColorScaleEntry &entry, CWnd *pParent /*=NULL*/)
    : CDialog(CAttriColorScaleEntryDlg::IDD, pParent), m_entry(entry) {
  //{{AFX_DATA_INIT(CAttriColorScaleEntryDlg)
  //}}AFX_DATA_INIT
}

void CAttriColorScaleEntryDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  int nScale = -1;
  if (!pDX->m_bSaveAndValidate) {
    GetDlgItem(IDC_RB_GLOBAL)->EnableWindow(m_entry.GlobalColorGradient() != 0);
    GetDlgItem(IDC_RD_HOTSPOT)->EnableWindow(m_entry.HotSpot() != 0);
    GetDlgItem(IDC_CB_GLOBAL)->EnableWindow(m_entry.ColorScaleType() == CColorScaleEntry::GLOBAL);
    GetDlgItem(IDC_CB_HOTSPOT)->EnableWindow(m_entry.ColorScaleType() == CColorScaleEntry::HOTSPOT);
    GetDlgItem(IDC_BT_GLOBAL_DELETE)->EnableWindow(m_entry.GlobalColorGradient() != 0);
    GetDlgItem(IDC_BT_GLOBAL_ATTRIBUTES)->EnableWindow(m_entry.GlobalColorGradient() != 0);
    GetDlgItem(IDC_BT_HOTSPOT_DELETE)->EnableWindow(m_entry.HotSpot() != 0);
    GetDlgItem(IDC_BT_HOTSPOT_ATTRIBUTES)->EnableWindow(m_entry.HotSpot() != 0);

    switch (m_entry.ColorScaleType()) {
    case CColorScaleEntry::LOCAL:
      nScale = 0;
      break;
    case CColorScaleEntry::GLOBAL:
      nScale = 1;
      break;
    case CColorScaleEntry::HOTSPOT:
      nScale = 2;
      break;
    default:
      break;
    }
  }

  //{{AFX_DATA_MAP(CAttriColorScaleEntryDlg)
  DDX_Control(pDX, IDC_CB_HOTSPOT, m_cbHotSpot);
  DDX_Control(pDX, IDC_CB_GLOBAL, m_cbGlobal);
  DDX_Radio(pDX, IDC_RD_LOCAL, nScale);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate) {
    switch (nScale) {
    case -1:
      break;
    case 0:
      m_entry.ColorScaleType(CColorScaleEntry::LOCAL);
      break;
    case 1:
      m_entry.ColorScaleType(CColorScaleEntry::GLOBAL);
      break;
    case 2:
      m_entry.ColorScaleType(CColorScaleEntry::HOTSPOT);
      break;
    default:
      break;
    }
  }
}

BEGIN_MESSAGE_MAP(CAttriColorScaleEntryDlg, CDialog)
//{{AFX_MSG_MAP(CAttriColorScaleEntryDlg)
ON_BN_CLICKED(IDC_BT_GLOBAL_DELETE, OnDeleteGlobalScale)
ON_BN_CLICKED(IDC_BT_GLOBAL_NEW, OnNewGlobalScale)
ON_BN_CLICKED(IDC_BT_GLOBAL_ATTRIBUTES, OnAttributesGlobalScale)
ON_BN_CLICKED(IDC_BT_GLOBAL_COPY, OnCopyGlobalScale)
ON_BN_CLICKED(IDC_BT_HOTSPOT_ATTRIBUTES, OnAttributesHotSpot)
ON_BN_CLICKED(IDC_BT_HOTSPOT_DELETE, OnDeleteHotSpot)
ON_BN_CLICKED(IDC_BT_HOTSPOT_NEW, OnNewHotSpot)
ON_CBN_SELCHANGE(IDC_CB_GLOBAL, OnGlobalSelectionChanged)
ON_CBN_SELCHANGE(IDC_CB_HOTSPOT, OnHotSpotSelectionChanged)
ON_BN_CLICKED(IDC_RB_GLOBAL, OnSelectionChanged)
ON_BN_CLICKED(IDC_RD_HOTSPOT, OnSelectionChanged)
ON_BN_CLICKED(IDC_RD_LOCAL, OnSelectionChanged)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriColorScaleEntryDlg message handlers

void CAttriColorScaleEntryDlg::OnDeleteGlobalScale() {
  // Get the current selection
  if (m_cbGlobal.GetCurSel() >= 0) {
    CColorGradient *pGlobal = (CColorGradient *)(m_cbGlobal.GetItemData(m_cbGlobal.GetCurSel()));
    delete pGlobal;
    OnUpdateGlobalScaleBox();
  }

  UpdateData(FALSE);
}

void CAttriColorScaleEntryDlg::OnNewGlobalScale() {
  CColorScaleEntry_Delegate *m_entry_Delegate = dynamic_cast<CColorScaleEntry_Delegate *>(m_entry.getDelegate());
  m_entry_Delegate->OnNewColorGradient();
  OnUpdateGlobalScaleBox();
  UpdateData(FALSE);
}

void CAttriColorScaleEntryDlg::OnAttributesGlobalScale() {
  // wjrx mantis 2575
  if (m_entry.ColorScaleType() == CColorScaleEntry::LOCAL && m_entry.LocalColorGradient() != NULL) {
    CColorGradient_Delegate *colorGradient_Delegate = new CColorGradient_Delegate(m_entry.LocalColorGradient());

    colorGradient_Delegate->Attributes();

    return;
  }

  // Get the current selection
  if (m_cbGlobal.GetCurSel() >= 0) {
    CColorGradient *pGlobal = (CColorGradient *)(m_cbGlobal.GetItemData(m_cbGlobal.GetCurSel()));
    CColorGradient_Delegate *pGlobal_Delegate = new CColorGradient_Delegate(pGlobal);
    pGlobal_Delegate->Attributes();
    OnUpdateGlobalScaleBox();
  }
}

namespace {

typedef QSharedPointer<CColorGradient_Delegate> TColorGradient_Delegate;

} // anonymous namespace

void CAttriColorScaleEntryDlg::OnCopyGlobalScale() {
  if (m_cbGlobal.GetCurSel() >= 0) {
    CColorGradient *globalColorScale =
        reinterpret_cast<CColorGradient *>(m_cbGlobal.GetItemData(m_cbGlobal.GetCurSel()));
    CColorGradient *globalColorScaleCopy = new CColorGradient(
        IDS_NEW_COLOR_GRADIENT, // need a stringtable entry
        globalColorScale->Model(), globalColorScale->MinMaxValue().first, globalColorScale->MinMaxValue().second,
        globalColorScale->MinColor(), globalColorScale->MaxColor(), true);

    // the constructor does not properly interpret the supplied values

    globalColorScaleCopy->colorMap(globalColorScale->colorMap());

    // overrule the name assigned at construction

    globalColorScaleCopy->Name(globalColorScale->Name() + " - Copy");

    TColorGradient_Delegate globalColorScaleCopy_Delegate =
        TColorGradient_Delegate(new CColorGradient_Delegate(globalColorScaleCopy));

    if (!globalColorScaleCopy_Delegate->Attributes()) {
      delete globalColorScaleCopy;
    } else {
      m_entry.SelectGlobalColorGradient(*globalColorScaleCopy);
      OnUpdateGlobalScaleBox();
      UpdateData(FALSE);
    }
  }
}

void CAttriColorScaleEntryDlg::OnAttributesHotSpot() {
  // Get the current selection
  if (m_cbHotSpot.GetCurSel() >= 0) {
    CHotSpot *pHotSpot = (CHotSpot *)(m_cbHotSpot.GetItemData(m_cbHotSpot.GetCurSel()));
    CHotSpot_Delegate *pHotSpot_Delegate = new CHotSpot_Delegate(pHotSpot);
    pHotSpot_Delegate->Attributes();
    OnUpdateHotSpotBox();
  }
}

void CAttriColorScaleEntryDlg::OnDeleteHotSpot() {
  // Get the current selection
  if (m_cbHotSpot.GetCurSel() >= 0) {
    CHotSpot *pHotSpot = (CHotSpot *)(m_cbHotSpot.GetItemData(m_cbHotSpot.GetCurSel()));
    delete pHotSpot;
    OnUpdateHotSpotBox();
  }

  UpdateData(FALSE);
}

void CAttriColorScaleEntryDlg::OnNewHotSpot() {
  CColorScaleEntry_Delegate *m_entry_Delegate = dynamic_cast<CColorScaleEntry_Delegate *>(m_entry.getDelegate());
  m_entry_Delegate->OnNewHotSpot();
  OnUpdateHotSpotBox();
  UpdateData(FALSE);
}

BOOL CAttriColorScaleEntryDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  OnUpdateHotSpotBox();
  OnUpdateGlobalScaleBox();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriColorScaleEntryDlg::OnUpdateHotSpotBox() {
  m_cbHotSpot.ResetContent();

  // Just walk over the hotspot
  CColorScaleEntry::TNodeSet stNode = m_entry.EntryNodes();
  for (CColorScaleEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    CHotSpot *pHotSpot = dynamic_cast<CHotSpot *>(*it);
    if (pHotSpot) {
      int nIndex = m_cbHotSpot.AddString(pHotSpot->Name().toStdString().c_str());
      m_cbHotSpot.SetItemData(nIndex, (DWORD_PTR)(*it));
      if (m_entry.HotSpot() == pHotSpot)
        m_cbHotSpot.SetCurSel(nIndex);
    }
  }
}

void CAttriColorScaleEntryDlg::OnUpdateGlobalScaleBox() {
  m_cbGlobal.ResetContent();

  // Just walk over the hotspot
  CColorScaleEntry::TNodeSet stNode = m_entry.EntryNodes();
  for (CColorScaleEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    CColorGradient *pGlobal = dynamic_cast<CColorGradient *>(*it);
    if (pGlobal) {
      int nIndex = m_cbGlobal.AddString(pGlobal->Name().toStdString().c_str());
      m_cbGlobal.SetItemData(nIndex, (DWORD_PTR)(*it));
      if (m_entry.GlobalColorGradient() == pGlobal)
        m_cbGlobal.SetCurSel(nIndex);
    }
  }
}

void CAttriColorScaleEntryDlg::OnGlobalSelectionChanged() {
  if (m_cbGlobal.GetCurSel() >= 0) {
    CColorGradient *pGlobal = (CColorGradient *)(m_cbGlobal.GetItemData(m_cbGlobal.GetCurSel()));
    m_entry.SelectGlobalColorGradient(*pGlobal);
  }
  UpdateData(FALSE);
}

void CAttriColorScaleEntryDlg::OnHotSpotSelectionChanged() {
  if (m_cbHotSpot.GetCurSel() >= 0) {
    CHotSpot *pHotSpot = (CHotSpot *)(m_cbHotSpot.GetItemData(m_cbHotSpot.GetCurSel()));
    m_entry.SelectHotSpot(*pHotSpot);
  }
  UpdateData(FALSE);
}

void CAttriColorScaleEntryDlg::OnSelectionChanged() {
  UpdateData(TRUE);

  // Enable/Disable
  m_cbGlobal.EnableWindow(m_entry.ColorScaleType() == CColorScaleEntry::GLOBAL);
  m_cbHotSpot.EnableWindow(m_entry.ColorScaleType() == CColorScaleEntry::HOTSPOT);
}
