// modeltimesteppg.cpp : implementation file
//

#include "modeltimesteppg.h"
#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "MonthCalDlg.h"
#include "ResultRegister.h"
#include "geomec.h"
#include "modelbase.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

#define MONTH_ID 1000

static BOOL IsInteger(const CString &sString) {
  // Integer
  for (int i = 0; i < sString.GetLength(); i++) {
    if (i == (sString.GetLength() - 1))
      if (!((sString[i] == '-') || isdigit(sString[i])))
        return FALSE;
      else if (!isdigit(sString[i]))
        return FALSE;
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTimeStepListObject sub class
/////////////////////////////////////////////////////////////////////////////
CModelTimestepPg::CTimeStepListObject::CTimeStepListObject(CModelTimestepPg &dlg, CListCtrl &ctrl,
                                                           CDepletionStage &stage)
    : IListObject(ctrl), m_dlg(dlg), m_pStage(&stage), m_sName(stage.Name().toStdString().c_str()),
      m_Time(stage.Time()), m_OutputType(stage.OutputType()), m_bInserted(false) {
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  new CYearSubObject(*this);
  new CMonthSubObject(*this);
  new COutputSubObject(*this);
}

// DELETE: This constructor is never called...
CModelTimestepPg::CTimeStepListObject::CTimeStepListObject(CModelTimestepPg &dlg, CListCtrl &ctrl, const CString &sName,
                                                           const CGeomecTime &time,
                                                           CDepletionStage::eOutputType output_type)
    : IListObject(ctrl), m_dlg(dlg), m_sName(sName), m_Time(time), m_OutputType(output_type), m_pStage(0),
      m_bInserted(false) {
  assert(ctrl.GetItemCount() > 1);
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  new CYearSubObject(*this);
  new CMonthSubObject(*this);
  new COutputSubObject(*this);
}

CModelTimestepPg::CTimeStepListObject::CTimeStepListObject(CModelTimestepPg &dlg, CListCtrl &ctrl, int nIndex)
    : IListObject(ctrl, nIndex), m_dlg(dlg), m_pStage(0), m_bInserted(false) {
  assert(nIndex >= 0);

  if (nIndex == 0) {
    CTimeStepListObject *next = (CTimeStepListObject *)(Ctrl().GetItemData(nIndex + 1));
    m_Time = next->Time();
    m_Time.SetOneMonthEarlier();
    m_sName.Format(_T("Initial"));
  } else {
    CTimeStepListObject *prev = (CTimeStepListObject *)(Ctrl().GetItemData(nIndex - 1));
    m_Time = prev->Time();
    m_Time.SetOneMonthLater();
    ShiftTimes(m_Time, nIndex + 1);
    m_sName.Format(_T("New Depletion Stage"));
  }

  m_OutputType = CDepletionStage::DefaultOutputType();

  Ctrl().SetItemData(nIndex, (DWORD_PTR)this);
  new CYearSubObject(*this);
  new CMonthSubObject(*this);
  new COutputSubObject(*this);
}

CModelTimestepPg::CTimeStepListObject::CTimeStepListObject(CTimeStepListObject &list_object)
    : IListObject(list_object.Ctrl()), m_dlg(list_object.m_dlg), m_pStage(0), m_bInserted(false) {
  assert(list_object.Index() == (Ctrl().GetItemCount() - 2));
  assert(Ctrl() == list_object.Ctrl());
  // DELETE: Is this 'if' really necessary?
  if (list_object.m_pStage && !list_object.m_pStage->Last()) {
    m_pStage = &list_object.m_pStage->Next();
    m_sName = m_pStage->Name().toStdString().c_str();
    m_Time = m_pStage->Time();
    m_OutputType = m_pStage->OutputType();
  } else {
    // Initialise new stage
    m_sName.Format(_T("Depletion Stage %d"), list_object.Index() + 1);
    m_Time = CGeomecTime(list_object.Time().GetYear() + 1, list_object.Time().GetMonth());
    m_OutputType = CDepletionStage::DefaultOutputType();
  }

  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  new CYearSubObject(*this);
  new CMonthSubObject(*this);
  new COutputSubObject(*this);
}

void CModelTimestepPg::CTimeStepListObject::Destroy() {
  if (m_pStage) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(&m_pStage->Model());
    pModel->ResultRegister().ClearAll();
  }

  delete m_pStage;
}

QString CModelTimestepPg::CTimeStepListObject::Text() const { return (LPCSTR)m_sName; }

BOOL CModelTimestepPg::CTimeStepListObject::CanEditText() const {
  const CModelBase &model = static_cast<const CModelBase &>(m_pStage->Model());
  return !model.BranchState().IsFixedStage(*m_pStage);
}

BOOL CModelTimestepPg::CTimeStepListObject::EditText(const QString &strText) {
  if (!CanEditText())
    return FALSE;

  m_sName = strText.toStdString().c_str();
  return TRUE;
}

unsigned int CModelTimestepPg::CTimeStepListObject::Icon() const {
  int direction;
  if (Index() == 1 && !m_pStage) // this object was inserted after the initial stage
    direction = 1;
  else
    direction = -1;
  const CTimeStepListObject *pObject = this;
  while (pObject->m_pStage == 0) {
    assert(pObject->Index() != 0);
    pObject = (const CTimeStepListObject *)(Ctrl().GetItemData(pObject->Index() + direction));
  }

  return pObject->m_pStage->IconId();
}

const CGeomecTime &CModelTimestepPg::CTimeStepListObject::Time() const { return m_Time; }

CGeomecTime &CModelTimestepPg::CTimeStepListObject::Time() { return m_Time; }

void CModelTimestepPg::CTimeStepListObject::ShiftTimes(CGeomecTime time, int fromIndex) {
  for (int i = fromIndex; i < Ctrl().GetItemCount(); ++i) {
    CTimeStepListObject *obj = (CTimeStepListObject *)(Ctrl().GetItemData(i));
    if (obj->Time() <= time) {
      obj->Time().SetOneMonthLater();
      obj->m_pStage->Time(obj->Time());
      time = obj->Time();
    }
  }
}

void CModelTimestepPg::CTimeStepListObject::Time(const CGeomecTime &time) { m_Time = time; }

CDepletionStage::eOutputType CModelTimestepPg::CTimeStepListObject::OutputType() const { return m_OutputType; }

void CModelTimestepPg::CTimeStepListObject::setOutputType(CDepletionStage::eOutputType output_type) {
  if (output_type != m_OutputType) {
    m_OutputType = output_type;
    m_dlg.UpdateCleanupCheckbox();
  }
}

void CModelTimestepPg::CTimeStepListObject::Apply() {
  if (m_pStage) {
    if (m_Time != m_pStage->Time()) {
      m_pStage->Time(m_Time);
      CModelBase *pModel = dynamic_cast<CModelBase *>(&m_pStage->Model());
      pModel->ResultRegister().ClearNonLinear();
    }
    if (m_sName != m_pStage->Name())
      m_pStage->Name((LPCSTR)m_sName);
    if (m_OutputType != m_pStage->OutputType()) {
      m_pStage->setOutputType(m_OutputType);
      CModelBase *pModel = dynamic_cast<CModelBase *>(&m_pStage->Model());
      pModel->ResultRegister().ClearNonLinear();
    }
    if (Index() == (Ctrl().GetItemCount() - 1) && !Inserted()) {
      // Move to the last dp
      CDepletionStage *pStage = m_pStage;
      while (!pStage->Last())
        pStage = &pStage->Next();
      // Delete all till this one
      while (pStage != m_pStage) {
        CDepletionStage *pDelete = pStage;
        pStage = &pStage->Previous();
        CModelBase *pModel = dynamic_cast<CModelBase *>(&pDelete->Model());
        pModel->ResultRegister().ClearAll();
        delete pDelete;
      }
    }
  } else {
    if (Inserted()) {
      CDepletionStage *prev = 0;
      CDepletionStage *next = 0;
      if (Index() == 0) {
        next = ((CTimeStepListObject *)(Ctrl().GetItemData(Index() + 1)))->m_pStage;
      } else {
        prev = ((CTimeStepListObject *)(Ctrl().GetItemData(Index() - 1)))->m_pStage;
        next = ((CTimeStepListObject *)(Ctrl().GetItemData(Index() + 1)))->m_pStage;
      }
      m_pStage = new CDepletionStage((LPCSTR)m_sName, prev, next, Time());
    } else {
      assert(Index() > 0);
      CTimeStepListObject *pListObject = (CTimeStepListObject *)(Ctrl().GetItemData(Index() - 1));
      assert(pListObject->m_pStage);
      m_pStage = new CDepletionStage((LPCSTR)m_sName, *pListObject->m_pStage);
      m_pStage->Time(m_Time);
    }
    m_pStage->setOutputType(m_OutputType);
    CModelBase *pModel = dynamic_cast<CModelBase *>(&m_pStage->Model());
    pModel->ResultRegister().ClearAll();
  }

  Update();
}
/////////////////////////////////////////////////////////////////////////////
// CYearSubObject
/////////////////////////////////////////////////////////////////////////////
CModelTimestepPg::CYearSubObject::CYearSubObject(CTimeStepListObject &parent)
    : ISubListObject(parent, 1), m_parent(parent) {}

QString CModelTimestepPg::CYearSubObject::Text() const {
  QString sRet;
  sRet = QString("%1").arg(m_parent.Time().GetYear());
  return sRet;
}

BOOL CModelTimestepPg::CYearSubObject::CanEditText() const { return m_parent.CanEditText(); }

BOOL CModelTimestepPg::CYearSubObject::EditText(const QString &strText) {
  if (!IsInteger(strText.toStdString().c_str())) {
    _m()->msg("Year is not an integer");
    return FALSE;
  }

  m_parent.Time(CGeomecTime(atoi(strText.toStdString().c_str()), m_parent.Time().GetMonth()));
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMonthSubObject
/////////////////////////////////////////////////////////////////////////////
CModelTimestepPg::CMonthSubObject::CMonthSubObject(CTimeStepListObject &parent)
    : ISubListObject(parent, 2), m_parent(parent) {}

QString CModelTimestepPg::CMonthSubObject::Text() const {
  QString sRet;
  sRet = QString("%1").arg(m_parent.Time().GetMonth());
  return sRet;
}

BOOL CModelTimestepPg::CMonthSubObject::CanEditText() const { return m_parent.CanEditText(); }

BOOL CModelTimestepPg::CMonthSubObject::EditText(const QString &strText) {
  if (!IsInteger(strText.toStdString().c_str())) {
    _m()->msg("Month is not an integer");
    return FALSE;
  }

  if ((atoi(strText.toStdString().c_str()) < 1) || (atoi(strText.toStdString().c_str()) > 12)) {
    _m()->msg("Month is not in range 1-12");
    return FALSE;
  }

  m_parent.Time(CGeomecTime(m_parent.Time().GetYear(), atoi(strText.toStdString().c_str())));
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COutputSubObject
/////////////////////////////////////////////////////////////////////////////

CModelTimestepPg::COutputSubObject::COutputSubObject(CTimeStepListObject &parent)
    : ISubListObject(parent, 3), m_parent(parent) {}

QString CModelTimestepPg::COutputSubObject::Text() const {
  return CDepletionStage::OutputTypeTag(OutputType()).c_str();
}

void CModelTimestepPg::COutputSubObject::setOutputType(CDepletionStage::eOutputType output_type) {
  m_parent.setOutputType(output_type);
}

CDepletionStage::eOutputType CModelTimestepPg::COutputSubObject::OutputType() const { return m_parent.OutputType(); }

BOOL CModelTimestepPg::COutputSubObject::CanEditText() const { return m_parent.CanEditText(); }

BOOL CModelTimestepPg::COutputSubObject::EditText(const QString &strText) {
  if (strText == CDepletionStage::OutputTypeTag(CDepletionStage::NONE).c_str())
    setOutputType(CDepletionStage::NONE);
  else if (strText == CDepletionStage::OutputTypeTag(CDepletionStage::RESULTS).c_str())
    setOutputType(CDepletionStage::RESULTS);
  else if (strText == CDepletionStage::OutputTypeTag(CDepletionStage::BRANCH).c_str())
    setOutputType(CDepletionStage::BRANCH);
  else if (strText == CDepletionStage::OutputTypeTag(CDepletionStage::PHASE).c_str())
    setOutputType(CDepletionStage::PHASE);
  else {
    assert(FALSE);
    return FALSE;
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CModelTimestepPg property page

// ##ModelId=3BC55D6101DC
CModelTimestepPg::CModelTimestepPg(CModelBase &model)
    : CPropertyPage(CModelTimestepPg::IDD), m_model(model), m_nTimeSteps(0), m_bCancelling(false) {
  //{{AFX_DATA_INIT(CModelTimestepPg)
  //}}AFX_DATA_INIT
}

// ##ModelId=3BC55D6101EA
CModelTimestepPg::~CModelTimestepPg() {}

// ##ModelId=3BC55D6101EB
void CModelTimestepPg::DoDataExchange(CDataExchange *pDX) {
  CPropertyPage::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_LB_TIMESTEP, m_lbTimestep);

  int nCleanupOldBranchResults;

  if (!pDX->m_bSaveAndValidate) {
    UpdateCleanupCheckbox();
    nCleanupOldBranchResults = m_model.CleanupOldBranchResults() ? 1 : 0;
  }

  DDX_Text(pDX, IDC_ED_TIMESTEPS, m_nTimeSteps);
  DDX_Check(pDX, IDC_CHK_CLNRES, nCleanupOldBranchResults);

  if (!m_bCancelling) {
    int nMinStage = 2;
    if (m_model.BranchState().IsBranch()) {
      int iBranchIndex = m_model.BranchState().BranchStage().Index();
      if (iBranchIndex + 2 > nMinStage)
        nMinStage = iBranchIndex + 2;
    }

    DDV_MinMaxInt(pDX, m_nTimeSteps, nMinStage, 100);
  }

  if (pDX->m_bSaveAndValidate) {
    m_model.CleanupOldBranchResults(nCleanupOldBranchResults != 0);

    if (m_nTimeSteps != m_lbTimestep.GetItemCount()) {
      if (m_nTimeSteps > m_lbTimestep.GetItemCount())
        Increase(m_nTimeSteps - m_lbTimestep.GetItemCount());
      else
        Decrease(m_lbTimestep.GetItemCount() - m_nTimeSteps);
    }
  }
}

void CModelTimestepPg::UpdateCleanupCheckbox() {
  bool bHasRestarts = HasRestarts();
  GetDlgItem(IDC_CHK_CLNRES)->EnableWindow(bHasRestarts != false);
  GetDlgItem(IDC_TXT_CLNRES)->EnableWindow(bHasRestarts != false);
}

BEGIN_MESSAGE_MAP(CModelTimestepPg, CPropertyPage)
//{{AFX_MSG_MAP(CModelTimestepPg)
ON_EN_CHANGE(IDC_ED_TIMESTEPS, OnChangeTimesteps)
ON_EN_KILLFOCUS(IDC_ED_TIMESTEPS, OnKillfocuTimesteps)
//}}AFX_MSG_MAP
ON_COMMAND(ID_POPUP_INSERTBEFORE, &CModelTimestepPg::OnPopupInsertbefore)
ON_NOTIFY(NM_RCLICK, IDC_LB_TIMESTEP, &CModelTimestepPg::OnNMRclickLbTimestep)
ON_COMMAND(ID_POPUP_INSERTAFTER, &CModelTimestepPg::OnPopupInsertafter)
ON_COMMAND(ID_POPUP_DELETE, &CModelTimestepPg::OnPopupDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelTimestepPg message handlers

void CModelTimestepPg::OnChangeTimesteps() { SetModified(true); }

BOOL CModelTimestepPg::OnInitDialog() {
  TDepletionStageEntry *pEntry = (TDepletionStageEntry *)(m_model.GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  m_nTimeSteps = stDepletionStage.size();

  CPropertyPage::OnInitDialog();

  CRect rect;
  m_lbTimestep.GetWindowRect(&rect);

  m_lbTimestep.InsertColumn(0, _T("Name"), LVCFMT_LEFT, (rect.Width() - 3) * 0.50, 0);
  m_lbTimestep.InsertColumn(1, _T("Year"), LVCFMT_LEFT, (rect.Width() - 3) * 0.15, 1);
  m_lbTimestep.InsertColumn(2, _T("Month"), LVCFMT_LEFT, (rect.Width() - 3) * 0.15, 1);
  m_lbTimestep.InsertColumn(3, _T("Output"), LVCFMT_LEFT, (rect.Width() - 3) * 0.20, 1);
  m_lbTimestep.SetComboColumns(3);

  // First initialisation of list box ...
  CDepletionStage *pStage = &m_model.InitialDepletionStage();
  assert(pStage->Initial());
  while (pStage) {
    new CTimeStepListObject(*this, m_lbTimestep, *pStage);

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  UpdateData(FALSE);

  return TRUE;
}

void CModelTimestepPg::Decrease(const int n) {
  assert(n < m_lbTimestep.GetItemCount());
  for (int i = 0; i < n; i++)
    m_lbTimestep.DeleteItem(m_lbTimestep.GetItemCount() - 1);

  CheckOutputTypeLastStage();
}

void CModelTimestepPg::CheckOutputTypeLastStage() {
  // output type of last stage can not be NONE
  IListObject *pLast = m_lbTimestep.GetObject(m_lbTimestep.GetItemCount() - 1);
  assert(pLast);
  COutputSubObject *pOutputSubObject = (COutputSubObject *)pLast->SubObject(pLast->SubObjectSize() - 1);
  assert(pOutputSubObject);
  if (pOutputSubObject->OutputType() == CDepletionStage::NONE ||
      pOutputSubObject->OutputType() == CDepletionStage::PHASE)
    pOutputSubObject->setOutputType(CDepletionStage::RESULTS);
}

void CModelTimestepPg::Increase(const int n) {
  assert(m_lbTimestep.GetItemCount() > 0);

  // Get latest stage
  CTimeStepListObject *pStageObject =
      (CTimeStepListObject *)(m_lbTimestep.GetItemData(m_lbTimestep.GetItemCount() - 1));

  for (int i = 0; i < n; i++)
    pStageObject = new CTimeStepListObject(*pStageObject);
}

void CModelTimestepPg::Insert(const int prev, const int next) {
  assert(m_lbTimestep.GetItemCount() > 2);

  CTimeStepListObject *pPrevious = (CTimeStepListObject *)(m_lbTimestep.GetItemData(prev));
  CTimeStepListObject *pNext = (CTimeStepListObject *)(m_lbTimestep.GetItemData(next));
}

int CModelTimestepPg::Year(const int nIndex) const {
  assert(nIndex > -1);
  assert(nIndex < m_lbTimestep.GetItemCount());

  return atoi(m_lbTimestep.GetItemText(nIndex, 1));
}

bool CModelTimestepPg::IsNumerique(const CString &s) { return true; }

void CModelTimestepPg::OnKillfocuTimesteps() {
  //	UpdateData();
  //	Apply();
}

void CModelTimestepPg::Apply() {
  for (int nTimeStep = 0; nTimeStep < m_lbTimestep.GetItemCount(); nTimeStep++) {
    CTimeStepListObject *pStageObject = (CTimeStepListObject *)(m_lbTimestep.GetItemData(nTimeStep));
    assert(pStageObject);
    pStageObject->Apply();
  }
}

BOOL CModelTimestepPg::OnApply() {
  if (!CheckTimes()) {
    _m()->msg("Times are not in range, please correct your input");
    return FALSE;
  }
  return CPropertyPage::OnApply();
}

void CModelTimestepPg::OnOK() {

  Apply();

  CPropertyPage::OnOK();
}

bool CModelTimestepPg::CheckTimes() const {
  CTimeStepListObject *obj = (CTimeStepListObject *)(m_lbTimestep.GetItemData(0));
  CGeomecTime current = obj->Time();

  for (int i = 1; i < m_lbTimestep.GetItemCount(); ++i) {
    obj = (CTimeStepListObject *)(m_lbTimestep.GetItemData(i));
    if (current >= obj->Time()) {
      return false;
    }
    current = obj->Time();
  }
  return true;
}

bool CModelTimestepPg::HasRestarts() {
  bool bRet = false;

  for (int i = 0; i < m_lbTimestep.GetItemCount(); ++i) {
    IListObject *pListObject = m_lbTimestep.GetObject(i);
    COutputSubObject *pOutputSubObject = (COutputSubObject *)pListObject->SubObject(pListObject->SubObjectSize() - 1);
    if (pOutputSubObject->OutputType() == CDepletionStage::BRANCH ||
        pOutputSubObject->OutputType() == CDepletionStage::PHASE) {
      bRet = true;
      break;
    }
  }

  return bRet;
}

BOOL CModelTimestepPg::OnQueryCancel() {
  m_bCancelling = true;

  BOOL bRet = CPropertyPage::OnQueryCancel();

  if (!bRet)
    m_bCancelling = false;

  return bRet;
}

BOOL CModelTimestepPg::OnKillActive() {
  Apply();
  return CPropertyPage::OnKillActive();
}

void CModelTimestepPg::OnPopupInsertbefore() {
  int iSelection = m_lbTimestep.GetNextItem(-1, LVNI_SELECTED);
  if (iSelection != -1) {
    CTimeStepListObject *object = new CTimeStepListObject(*this, m_lbTimestep, iSelection);
    object->Inserted(true);
    ++m_nTimeSteps;
    UpdateData(FALSE);
  }
  Apply();
  //  CancelToClose();
}

void CModelTimestepPg::OnPopupInsertafter() {
  int iSelection = m_lbTimestep.GetNextItem(-1, LVNI_SELECTED);
  if (iSelection != -1) {
    ++m_nTimeSteps;
    UpdateData(FALSE);
    if (iSelection == m_lbTimestep.GetItemCount() - 1) {
      Increase(1);
    } else {
      CTimeStepListObject *object = new CTimeStepListObject(*this, m_lbTimestep, iSelection + 1);
      object->Inserted(true);
    }
  }
  Apply();
  //  CancelToClose();
}

void CModelTimestepPg::OnPopupDelete() {
  int iSelection = m_lbTimestep.GetNextItem(-1, LVNI_SELECTED);
  if (iSelection != -1) {
    CTimeStepListObject *pObj = (CTimeStepListObject *)(m_lbTimestep.GetItemData(iSelection));
    const CString &stage_name = pObj->Name();
    CString warning;
    warning.Format("Are you sure you want to delete depletion stage \"%s\"?", stage_name);
    if (_m()->msg(warning, MB_OKCANCEL) == IDOK) {
      --m_nTimeSteps;
      UpdateData(FALSE);
      pObj->Destroy();
      m_lbTimestep.DeleteItem(iSelection);
      CheckOutputTypeLastStage();
    }
  }
}

void CModelTimestepPg::OnNMRclickLbTimestep(NMHDR *pNMHDR, LRESULT *pResult) {
  int iSelection = m_lbTimestep.GetNextItem(-1, LVNI_SELECTED);
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  CMenu mnuPopupDepl;
  mnuPopupDepl.LoadMenu(IDR_POPUP_DEPLETION);
  CMenu *mnuPopupMenu = mnuPopupDepl.GetSubMenu(0);
  assert(mnuPopupMenu);
  if (iSelection == 0 || m_lbTimestep.GetItemCount() == 2) {
    mnuPopupMenu->EnableMenuItem(ID_POPUP_DELETE, MF_DISABLED | MF_GRAYED);
  }

  CURSORINFO cursInf;
  cursInf.cbSize = sizeof(CURSORINFO);
  GetCursorInfo(&cursInf);
  mnuPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursInf.ptScreenPos.x, cursInf.ptScreenPos.y, this);

  *pResult = 0;
}

////

CModelTimestepPg::CTSComboListCtrl::CTSComboListCtrl() {}

void CModelTimestepPg::CTSComboListCtrl::OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList &lstItems) {
  if (iRowIndex > 0 && iRowIndex < GetItemCount() - 1) // not first or last
    lstItems.AddTail(_T("None"));

  lstItems.AddTail(_T("Results"));
  lstItems.AddTail(_T("Branch"));

  if (iRowIndex < GetItemCount() - 1) // not last
    lstItems.AddTail(_T("Phase"));
}
