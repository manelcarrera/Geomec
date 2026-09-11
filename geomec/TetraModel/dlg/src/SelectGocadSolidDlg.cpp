// SelectGocadSolidDlg.cpp: implementation of the CSelectGocadSolidDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "SelectGocadSolidDlg.h"

#include "sharedResource.h"
#include "ListCtrlBase.h"
#ifdef SKUA_NEW
#include "SKUAParseData.h"
#endif

//// CGocadSolidListCtrl

class CGocadSolidListCtrl : public CListCtrlBase
{
public:
  CGocadSolidListCtrl(CSelectGocadSolidDlg& dlg);
  void OnDoubleClick(int nIndex);

protected:
  afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

private:
  CSelectGocadSolidDlg& m_dlg;
};

BEGIN_MESSAGE_MAP(CGocadSolidListCtrl, CListCtrlBase)
  ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
END_MESSAGE_MAP()

CGocadSolidListCtrl::CGocadSolidListCtrl(CSelectGocadSolidDlg& dlg)
: m_dlg(dlg)
{
}

void CGocadSolidListCtrl::OnDoubleClick(int nIndex)
{
  m_dlg.OnDoubleClick(nIndex);
}

void CGocadSolidListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
  m_dlg.SelectionChanged();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef SKUA_NEW
CSelectGocadSolidDlg::CSelectGocadSolidDlg(
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  CWnd* pParent)
#else
CSelectGocadSolidDlg::CSelectGocadSolidDlg(
  const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
  CWnd* pParent)
#endif
: CDialog(IDD_SELECT_GOCAD_SOLID, pParent),
  m_solids(solids)
{
  m_plcList = new CGocadSolidListCtrl(*this);
}

CSelectGocadSolidDlg::~CSelectGocadSolidDlg()
{
  delete m_plcList;
}

int CSelectGocadSolidDlg::SelectedSize() const
{
  return int(m_vcSelected.size());
}

int CSelectGocadSolidDlg::SelectedEntry(int nIndex) const
{
  return m_vcSelected[nIndex];
}

void CSelectGocadSolidDlg::SelectionChanged()
{
  GetDlgItem(IDOK)->EnableWindow(m_plcList->GetSelectedCount() != 0);
}

void CSelectGocadSolidDlg::OnDoubleClick(int nIndex)
{
  m_vcSelected.clear();
  m_vcSelected.push_back(nIndex);
  EndDialog(IDOK);
}

void CSelectGocadSolidDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_LIST, *m_plcList);

  if(pDX->m_bSaveAndValidate)
  {
  m_vcSelected.clear();
  POSITION pos = m_plcList->GetFirstSelectedItemPosition();
  while(pos)
      m_vcSelected.push_back(m_plcList->GetNextSelectedItem(pos));
  }
}

BOOL CSelectGocadSolidDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  CRect rect;
  m_plcList->GetWindowRect(&rect);

  m_plcList->InsertColumn(0, _T("Name"), LVCFMT_LEFT, (rect.Width() - 3) * 0.70, 0);
  m_plcList->InsertColumn(1, _T("Volumes"), LVCFMT_LEFT, (rect.Width() - 3) * 0.15, 1);
  m_plcList->InsertColumn(2, _T("Tetras"), LVCFMT_LEFT, (rect.Width() - 3) * 0.15, 1);

  for(size_t i = 0; i < m_solids.size(); ++i)
  {
#ifdef SKUA_NEW
  const gm_skua::SKUAParseData& tsolid = *m_solids[i];
  new CGocadSolidListObject(*m_plcList, tsolid.name.data, tsolid.model_region_names.size(), tsolid.tetras.size());
#else
  const CGocadData::CTSolid& tsolid = *m_solids[i];
  int nTetras = 0;
  int j;
  for(j = 0; j < tsolid.VolumeSize(); ++j)
      nTetras += tsolid.Volume(j).TetraSize();

  new CGocadSolidListObject(*m_plcList, tsolid.Name().toStdString().c_str(), tsolid.VolumeSize(), nTetras);
#endif
  }

  GetDlgItem(IDOK)->EnableWindow(FALSE);

  return TRUE;
}


//// CSelectGocadSolidDlg::CGocadSolidListObject

CSelectGocadSolidDlg::CGocadSolidListObject::CGocadSolidListObject(CListCtrlBase& ctrl, const CString& sName, int nVolumes, int nTetras)
: IListObject(ctrl),
  m_sName(sName)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CIntSubListObject(*this, 1, nVolumes);
  new CIntSubListObject(*this, 2, nTetras);
}

QString CSelectGocadSolidDlg::CGocadSolidListObject::Text() const
{
  return (LPCSTR) m_sName;
}

BOOL CSelectGocadSolidDlg::CGocadSolidListObject::CanEditText() const
{
  return FALSE;
}

BOOL CSelectGocadSolidDlg::CGocadSolidListObject::EditText(const QString& strText)
{
  return FALSE;
}

unsigned int CSelectGocadSolidDlg::CGocadSolidListObject::Icon() const
{
  return 0;
}

void CSelectGocadSolidDlg::CGocadSolidListObject::OnDoubleClick()
{
  (static_cast<CGocadSolidListCtrl&>(Ctrl())).OnDoubleClick(Index());
}


//// CSelectGocadSolidDlg::CIntSubListObject

CSelectGocadSolidDlg::CIntSubListObject::CIntSubListObject(CGocadSolidListObject& parent, int nColumn, int nValue)
: ISubListObject(parent, nColumn),
  m_nValue(nValue)
{
}

QString CSelectGocadSolidDlg::CIntSubListObject::Text() const
{
  QString str;
  str = QString("%1").arg(m_nValue);
  return str;
}

BOOL CSelectGocadSolidDlg::CIntSubListObject::CanEditText() const
{
  return FALSE;
}

BOOL CSelectGocadSolidDlg::CIntSubListObject::EditText(const QString& strText)
{
  return FALSE;
}
