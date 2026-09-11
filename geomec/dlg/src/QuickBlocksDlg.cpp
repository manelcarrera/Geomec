// QuickBlocksDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "DepletionStage.h"
#include "QuickBlocksDlg.h"
#include "QBSettings.h"
#include "LengthQuantity.h"
#include "GeomecUtils.h"
#include "SelectFormationsDlg.h"
#include "TnoFileDialog.h"
#include "GlobalMessage.h"



// CQuickBlocksDlg dialog

IMPLEMENT_DYNAMIC(CQuickBlocksDlg, CDialog)

CQuickBlocksDlg::CQuickBlocksDlg(CQBSettings &settings, CWnd* pParent /*=NULL*/)
  : CDialog(CQuickBlocksDlg::IDD, pParent)
  , m_dNorth(0)
  , m_dEast(0)
  , m_dLength(0)
  , m_dWidth(0)
  , m_dTrack(0)
  , m_dBin(0)
  , m_Source(settings)
  , m_lbPressures(settings.Model())
  , m_lbTemperatures(settings.Model())
{

}

BOOL CQuickBlocksDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  CRect rect;
  m_lbPressures.GetWindowRect(&rect);
  m_lbPressures.InsertColumn(0, _T("Pointset Name"), LVCFMT_LEFT,(rect.Width() - 3) * 0.30, 0);
  m_lbPressures.InsertColumn(1, _T("Initial Pressure"), LVCFMT_LEFT,(rect.Width() - 3) * 0.35, 1);
  m_lbPressures.InsertColumn(2, _T("Final Pressure"), LVCFMT_LEFT,(rect.Width() - 3) * 0.35, 1);
  m_lbPressures.SetComboColumns(1);
  m_lbPressures.SetComboColumns(2);

  for(size_t i = 0; i < m_Source.DeltaPressures().size(); ++i)
  {
  new CTimeLapseListObject<CQBSettings>(m_Source.Model(), m_lbPressures, 
      m_Source.DeltaPressures()[i].first, m_Source.DeltaPressures()[i].second,
      &m_Source);
  }
  if(m_lbPressures.GetItemCount() == 0)
  {
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE)->EnableWindow(FALSE);
  }

  m_lbTemperatures.GetWindowRect(&rect);
  m_lbTemperatures.InsertColumn(0, _T("Pointset Name"), LVCFMT_LEFT,(rect.Width() - 3) * 0.30, 0);
  m_lbTemperatures.InsertColumn(1, _T("Initial Temperature"), LVCFMT_LEFT,(rect.Width() - 3) * 0.35, 1);
  m_lbTemperatures.InsertColumn(2, _T("Final Temperature"), LVCFMT_LEFT,(rect.Width() - 3) * 0.35, 1);
  m_lbTemperatures.SetComboColumns(1);
  m_lbTemperatures.SetComboColumns(2);

  for(size_t i = 0; i < m_Source.DeltaTemperatures().size(); ++i)
  {
  new CTimeLapseListObject<CQBSettings, Temperature>(m_Source.Model(), m_lbTemperatures, 
      m_Source.DeltaTemperatures()[i].first, m_Source.DeltaTemperatures()[i].second,
      &m_Source);
  }

  if(m_lbTemperatures.GetItemCount() == 0)
  {
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE2)->EnableWindow(FALSE);
  }

  CLengthQuantity lq;
  CUnitNode::TUnitType unit = GetGeomecDoc()->UnitNode().Unit();
  m_dNorth = lq.Convert(m_Source.Grid().CornerNorthing(), unit, CLengthQuantity::SI_UNIT);
  m_dEast = lq.Convert(m_Source.Grid().CornerEasting(), unit, CLengthQuantity::SI_UNIT);
  m_dLength = lq.Convert(m_Source.Grid().GetLength(), unit, CLengthQuantity::SI_UNIT);
  m_dWidth = lq.Convert(m_Source.Grid().GetWidth(), unit, CLengthQuantity::SI_UNIT);
  m_dTrack = lq.Convert(m_Source.Grid().TrackSpacing(), unit, CLengthQuantity::SI_UNIT);
  m_dBin = lq.Convert(m_Source.Grid().BinSpacing(), unit, CLengthQuantity::SI_UNIT);

  GetDlgItem(IDC_STATIC_QBNORTHUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());
  GetDlgItem(IDC_STATIC_QBEASTUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());
  GetDlgItem(IDC_STATIC_QBLENGTHUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());
  GetDlgItem(IDC_STATIC_QBWIDTHUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());
  GetDlgItem(IDC_STATIC_QBTRACKUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());
  GetDlgItem(IDC_STATIC_QBBINUNIT)->SetWindowTextA(lq.UnitName(unit).c_str());

  // mantis #3207: functionality not yet implemented
  GetDlgItem(IDC_BUTTON_QB_START)->ShowWindow(SW_HIDE);

  UpdateData(FALSE);

  return TRUE;
}

CQuickBlocksDlg::~CQuickBlocksDlg()
{
}

void CQuickBlocksDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_QBNORTHING, m_dNorth);
  DDX_Text(pDX, IDC_EDIT_QBEASTING, m_dEast);
  DDX_Text(pDX, IDC_EDIT_QBLENGTH, m_dLength);
  DDX_Text(pDX, IDC_EDIT_QBWIDTH, m_dWidth);
  DDX_Text(pDX, IDC_EDIT_QBTRACK, m_dTrack);
  DDX_Text(pDX, IDC_EDIT_QBBIN, m_dBin);

  if(pDX->m_bSaveAndValidate)
  {
  m_Source.Grid().CornerNorthing(m_dNorth);
  m_Source.Grid().CornerEasting(m_dEast);
  m_Source.Grid().SetLength(m_dLength);
  m_Source.Grid().SetWidth(m_dWidth);
  m_Source.Grid().TrackSpacing(m_dTrack);
  m_Source.Grid().BinSpacing(m_dBin);
  m_Source.Grid().NumBin(m_dLength / m_dBin);
  m_Source.Grid().NumTrack(m_dWidth / m_dTrack);
  }
  DDX_Control(pDX, IDC_QB_DEPLSTAGE_LIST, m_lbPressures);
  DDX_Control(pDX, IDC_QB_DEPLSTAGE_LIST2, m_lbTemperatures);
}


BEGIN_MESSAGE_MAP(CQuickBlocksDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_QB_START, &CQuickBlocksDlg::OnBnClickedButtonQbStart)
  ON_BN_CLICKED(IDC_QB_ADD_DEPLSTAGE, &CQuickBlocksDlg::OnBnClickedQbAddDeplstage)
  ON_BN_CLICKED(IDC_QB_REMOVE_DEPLSTAGE, &CQuickBlocksDlg::OnBnClickedQbRemoveDeplstage)
  ON_BN_CLICKED(IDC_QB_ADD_DEPLSTAGE2, &CQuickBlocksDlg::OnBnClickedQbAddDeplstage2)
  ON_BN_CLICKED(IDC_QB_REMOVE_DEPLSTAGE2, &CQuickBlocksDlg::OnBnClickedQbRemoveDeplstage2)
  ON_BN_CLICKED(IDC_BUTTON_QB_WRITE_DAT, &CQuickBlocksDlg::OnBnClickedButtonQbWriteDat)
  ON_BN_CLICKED(IDC_QB_SELECT_FORMS, &CQuickBlocksDlg::OnBnClickedQbSelectForms)
END_MESSAGE_MAP()


// CQuickBlocksDlg message handlers

void CQuickBlocksDlg::OnBnClickedButtonQbStart()
{
  m_Source.StartQB(); 
}

void CQuickBlocksDlg::OnBnClickedQbAddDeplstage()
{
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(m_Source.Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  TDepletionStageEntry::TNodeSet::const_iterator it;
  it = stDepletionStage.begin();
  CDepletionStage *depl1 = (*it);
  CDepletionStage *depl2 = (*++it);
  m_Source.AddDeltaPressure(depl1, depl2);
  new CTimeLapseListObject<CQBSettings>(m_Source.Model(), m_lbPressures, depl1, depl2, &m_Source);
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE)->EnableWindow();
}

void CQuickBlocksDlg::OnBnClickedQbRemoveDeplstage()
{
  m_Source.RemoveDeltaPressure(m_Source.DeltaPressures().size() - 1);
  m_lbPressures.DeleteItem(m_lbPressures.GetItemCount()-1);
  if(m_lbPressures.GetItemCount() == 0)
  {
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE)->EnableWindow(FALSE);
  }
}

void CQuickBlocksDlg::OnBnClickedQbAddDeplstage2()
{
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(m_Source.Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  TDepletionStageEntry::TNodeSet::const_iterator it;
  it = stDepletionStage.begin();
  CDepletionStage *depl1 = (*it);
  CDepletionStage *depl2 = (*++it);
  m_Source.AddDeltaTemperature(depl1, depl2);
  new CTimeLapseListObject<CQBSettings, Temperature>(m_Source.Model(), m_lbTemperatures, depl1, depl2, &m_Source);
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE2)->EnableWindow();
}

void CQuickBlocksDlg::OnBnClickedQbRemoveDeplstage2()
{
  m_Source.RemoveDeltaTemperature(m_Source.DeltaTemperatures().size() - 1);
  m_lbTemperatures.DeleteItem(m_lbTemperatures.GetItemCount()-1);
  if(m_lbTemperatures.GetItemCount() == 0)
  {
  GetDlgItem(IDC_QB_REMOVE_DEPLSTAGE2)->EnableWindow(FALSE);
  }
}

void CQuickBlocksDlg::OnBnClickedButtonQbWriteDat()
{
  UpdateData(TRUE);
  
  if(m_Source.SelectedFormations().size() == 0)
  {
  _m()->msg("Please select at least one formation", MB_OK|MB_ICONWARNING);
  return;
  }

  CTnoFileDialog dlg(FALSE, "dat", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "QB dat file (*dat)|*.dat|All Files (*.*)|*.*||");

  if(dlg.DoModal() == IDCANCEL)
  return;

  m_Source.WriteDat((LPCSTR) dlg.GetPathName());
}



void CQuickBlocksDlg::OnBnClickedQbSelectForms()
{
  CSelectFormationsSetNode sfSetNode(m_Source.SelectedFormations(), m_Source.Model());
  CSelectFormationsDlg dlg(sfSetNode);
  dlg.DoModal();
}
