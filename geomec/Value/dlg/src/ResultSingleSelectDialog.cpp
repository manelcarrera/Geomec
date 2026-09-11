// ResultSingleSelectDialog.cpp : implementation file
//

#include "ResultSingleSelectDialog.h"
#include "BaseEntryTypes.h"
#include "DepletionStage.h"
#include "GlobalMessage.h"
#include "IValueComposite_Delegate.h"
#include "LocalResultObserver.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "Result.h"
#include "ResultTree.h"
#include "SelectionObserver.h"
#include "SelectionObserver_Delegate.h"
#include "ValueType.h"
#include "geomec.h"
#include "pointset.h"
#include "resourceIDI.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// Implementatie van CResultSingleSelectDialog::CResultGroupNode
/////////////////////////////////////////////////////////////////////////////
CResultSingleSelectDialog::CResultGroupObserver::CResultGroupObserver(CResultGroup &result_group,
                                                                      CResultSingleSelectDialog &dlg)
    : IResultGroupObserver(result_group, dlg.m_lbValue, TVI_ROOT, TVI_LAST), m_dlg(dlg) {
  Update();
}

unsigned int
CResultSingleSelectDialog::CResultGroupObserver::ResultComponentStateIconId(const IValueComposite &composite,
                                                                            int nComponent) const {
  const IResult &result = static_cast<const IResult &>(composite);

  if (result.Mode(m_dlg.AnalysisType(), m_dlg.TimeStep().Index()) > -1) {
    if (&result.Component(nComponent, result.Mode(m_dlg.AnalysisType(), m_dlg.TimeStep().Index())) == m_dlg.Selection())
      return IDI_RADIO_CHECKED;
  }

  if (!m_dlg.FixedLinStage()) // always allow selection
  {
    if (&result.Component(nComponent, 0) == m_dlg.Selection())
      return IDI_RADIO_CHECKED;

    return IDI_RADIO_UNCHECKED;
  }

  // Some result does not exist for initial stage
  if (&m_dlg.TimeStep() == &m_dlg.Model().InitialDepletionStage()) {
    assert(!m_dlg.Model().InitialDepletionStage().Last());
    if (result.Mode(m_dlg.AnalysisType(), m_dlg.Model().InitialDepletionStage().Next().Index()) > -1)
      return IDI_RADIO_UNCHECKED;
  }

  if (result.Mode(m_dlg.AnalysisType(), m_dlg.TimeStep().Index()) > -1)
    return IDI_RADIO_UNCHECKED;

  return IDI_RADIO_DISABLED;
}

void CResultSingleSelectDialog::CResultGroupObserver::ResultComponentToggleState(IValueComposite &composite,
                                                                                 int nComponent) {
  const IResult &result = static_cast<const IResult &>(composite);

  if (!m_dlg.FixedLinStage()) {
    m_dlg.Select(&result.Component(nComponent, 0));
    return;
  }

  // Some result does not exist for initial stage
  if (&m_dlg.TimeStep() == &m_dlg.Model().InitialDepletionStage()) {
    assert(!m_dlg.Model().InitialDepletionStage().Last());
    if (result.Mode(m_dlg.AnalysisType(), m_dlg.Model().InitialDepletionStage().Next().Index()) > -1)
      m_dlg.Select(&result.Component(
          nComponent, result.Mode(m_dlg.AnalysisType(), m_dlg.Model().InitialDepletionStage().Next().Index())));
  }

  if (result.Mode(m_dlg.AnalysisType(), m_dlg.TimeStep().Index()) > -1)
    m_dlg.Select(&result.Component(nComponent, result.Mode(m_dlg.AnalysisType(), m_dlg.TimeStep().Index())));
}

void CResultSingleSelectDialog::CResultGroupObserver::AppendResultComponentContextMenu(IResult_Delegate &result,
                                                                                       int nComponent,
                                                                                       CContextMenuInvoker &invoker) {
  // No ncontext menu needed
}

BOOL CResultSingleSelectDialog::CResultGroupObserver::CanInsertResult(const IResult &result) const {
  const CDerivedResult::CDerivedResultComponent *pResultBeingCreated = m_dlg.ResultBeingCreated();
  if (pResultBeingCreated && &pResultBeingCreated->Parent() == &result)
    return FALSE;

  return IResultGroupObserver::CanInsertResult(result);
}

BOOL CResultSingleSelectDialog::CResultGroupObserver::CanInsertResultGroup(const CResultGroup &group) const {
  const CDerivedResultGroup *pDerivedResultGroup = dynamic_cast<const CDerivedResultGroup *>(&group);
  if (pDerivedResultGroup && pDerivedResultGroup->StoreOnFile() == (m_dlg.Global() == TRUE))
    return FALSE;

  return IResultGroupObserver::CanInsertResultGroup(group);
}

BOOL CResultSingleSelectDialog::CResultGroupObserver::Valid(const CResultGroup &group) const {
  for (int i = 0; i < group.ChildSize(); i++) {
    const IResult *pResult = dynamic_cast<const IResult *>(&group.Child(i));
    if (pResult) {
      if (m_dlg.Valid(*pResult))
        return TRUE;
    }
    const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup *>(&group.Child(i));
    if (pResultGroup) {
      if (Valid(*pResultGroup))
        return TRUE;
    }
    const CValueType *pValueType = dynamic_cast<const CValueType *>(&group.Child(i));
    if (pValueType) {
      if (m_dlg.Valid(*pValueType))
        return TRUE;
    }
  }
  return FALSE;
}

CTreeNode *CResultSingleSelectDialog::CResultGroupObserver::InsertChild(CGraphNode &node) {
  CGVTResultGroup *pGVTGroup = dynamic_cast<CGVTResultGroup *>(&node);
  if (pGVTGroup)
    return new CGVTResultGroupObserverSingleSelectTempl<CResultSingleSelectDialog>(
        *pGVTGroup, *this, const_cast<CResultSingleSelectDialog &>(m_dlg));
  return IResultGroupObserver::InsertChild(node);
}

BOOL CResultSingleSelectDialog::CResultGroupObserver::OnFilter(const CGraphNode &node) const {
  if (dynamic_cast<const CGVTResultGroup *>(&node))
    return Valid(static_cast<const CGVTResultGroup &>(node));
  return IResultGroupObserver::OnFilter(node);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of CResultSingleSelectDialog::CPointSetObserver
/////////////////////////////////////////////////////////////////////////////
CResultSingleSelectDialog::CPointSetObserver::CPointSetObserver(IPointSet &point_set, CResultSingleSelectDialog &dlg)
    : CChildEnumerator<CValueType>(dlg.m_lbValue, TVI_ROOT, TVI_LAST), m_point_set(point_set), m_dlg(dlg) {
  Observer().LinkTo(point_set);
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  Update();
}

const CGraphNode &CResultSingleSelectDialog::CPointSetObserver::ObservedItem() const { return m_point_set; }

CGraphNode &CResultSingleSelectDialog::CPointSetObserver::ObservedItem() { return m_point_set; }

const CGraphNode_Delegate &CResultSingleSelectDialog::CPointSetObserver::Delegate() const {
  assert(FALSE);
  return *(new CGraphNode_Delegate(0));
}

CGraphNode_Delegate &CResultSingleSelectDialog::CPointSetObserver::Delegate() {
  assert(FALSE);
  return *(new CGraphNode_Delegate(0));
}

CTreeNode *CResultSingleSelectDialog::CPointSetObserver::InsertChild(CValueType &value_type) {
  return new CValueTypeObserverTempl<CPointSetObserver, IDI_RADIO_CHECKED, IDI_RADIO_UNCHECKED>(value_type, *this);
}

BOOL CResultSingleSelectDialog::CPointSetObserver::OnFilter(const CValueType &t) const {
  const CPointSet *pPointSet = dynamic_cast<const CPointSet *>(&t.PointSet());
  if (pPointSet)
    return &pPointSet->Coordinates() != &t;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CResultSingleSelectDialog dialog
/////////////////////////////////////////////////////////////////////////////

CResultSingleSelectDialog::CResultSingleSelectDialog(CModelBase &model, BOOL bGlobal,
                                                     const CDerivedResult::CDerivedResultComponent *pResultBeingCreated,
                                                     CWnd *pParent /*=NULL*/)
    : CDialog(CResultSingleSelectDialog::IDD, pParent), m_antype(CAnalysisType::AT_LINEAR), m_bInput(FALSE),
      m_model(model), m_bGlobal(bGlobal), m_pSelection(0), m_bGenerateName(TRUE),
      m_pTimeStep(&model.InitialDepletionStage()), m_pResultBeingCreated(pResultBeingCreated) {
  //{{AFX_DATA_INIT(CResultSingleSelectDialog)
  m_sName = _T("");
  //}}AFX_DATA_INIT
}

CResultSingleSelectDialog::CResultSingleSelectDialog(
    CDerivedResult::CDerivedResultComponent::CValueComponentProxy &proxy, BOOL bGlobal, CWnd *pParent)
    : CDialog(CResultSingleSelectDialog::IDD, pParent), m_bGlobal(bGlobal),
      m_model(dynamic_cast<CModelBase &>(proxy.ValueComponent().Model())), m_antype(CAnalysisType::AT_LINEAR),
      m_pTimeStep(&dynamic_cast<CModelBase &>(proxy.ValueComponent().Model()).InitialDepletionStage()),
      m_sName(proxy.TextTag().toStdString().c_str()), m_pSelection(&proxy.ValueComponent()), m_pResultBeingCreated(0) {
  m_bInput = !(dynamic_cast<CDerivedResult *>(&proxy.ValueComponent().Parent()) ||
               dynamic_cast<IResult *>(&proxy.ValueComponent().Parent()) ||
               dynamic_cast<const TDeltaT *>(&proxy.ValueComponent().Parent()) ||
               dynamic_cast<const TDeltaV *>(&proxy.ValueComponent().Parent()));
  m_bGenerateName = m_sName.CompareNoCase(m_pSelection->ExportLabel().toStdString().c_str()) == 0;
  IResultComponent *pComponent = dynamic_cast<IResultComponent *>(&proxy.ValueComponent());
  if (pComponent) {
    m_antype = pComponent->AnalysisType();
    m_pTimeStep = &pComponent->Stage();
  }
}

void CResultSingleSelectDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  int nInput, nLinear;

  if (!pDX->m_bSaveAndValidate) {
    if (m_bInput)
      nInput = 0;
    else
      nInput = 1;

    if (m_antype == CAnalysisType::AT_LINEAR)
      nLinear = 0;
    else if (m_antype == CAnalysisType::AT_NONLIN)
      nLinear = 1;
    else if (m_antype == CAnalysisType::AT_HEAT)
      nLinear = 2;
    else if (m_antype == CAnalysisType::AT_MIXTURE)
      nLinear = 3;
    else {
      assert(m_antype == CAnalysisType::AT_MIXTURE_CONTAINMENT);
      nLinear = 4;
    }
    DDX_Check(pDX, IDC_CK_NAME, m_bGenerateName);
    GetDlgItem(IDC_RB_INPUT)->EnableWindow(!m_bGlobal);
    OnSetLinStage();
  }

  DDX_Radio(pDX, IDC_RB_INPUT, nInput);
  DDX_Radio(pDX, IDC_RB_LINEAR, nLinear);

  //{{AFX_DATA_MAP(CResultSingleSelectDialog)
  DDX_Control(pDX, IDC_CB_TIMESTEP, m_cbTimeStep);
  DDX_Control(pDX, IDC_LB_VALUE_TREE, m_lbValue);
  DDX_Text(pDX, IDC_ED_NAME, m_sName);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate) {
    if (m_sName.GetLength() < 1) {
      _m()->msg("Name field empty");
      pDX->Fail();
    }

    if (Selection() == 0) {
      _m()->msg("Nothing selected");
      pDX->Fail();
    }
  }
}

BEGIN_MESSAGE_MAP(CResultSingleSelectDialog, CDialog)
//{{AFX_MSG_MAP(CResultSingleSelectDialog)
ON_BN_CLICKED(IDC_RB_INPUT, OnInput)
ON_BN_CLICKED(IDC_RB_LINEAR, OnLinear)
ON_BN_CLICKED(IDC_RB_OUTPUT, OnOutput)
ON_BN_CLICKED(IDC_RB_NON_LINEAR, OnNonLinear)
ON_BN_CLICKED(IDC_RB_HEAT, OnHeat)
ON_BN_CLICKED(IDC_RB_MIXTURE, OnMixture)
ON_BN_CLICKED(IDC_RB_MIXTURE_CONTAINMENT, OnMixtureContainment)
ON_CBN_SELCHANGE(IDC_CB_TIMESTEP, OnChangeTimeStep)
ON_BN_CLICKED(IDC_CK_NAME, OnCkName)
ON_BN_CLICKED(IDC_CHK_SETLINSTAGE, OnSetLinStage)
ON_EN_CHANGE(IDC_ED_NAME, OnNameChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultSingleSelectDialog message handlers

bool CResultSingleSelectDialog::Composite(const IValueComposite &composite) const {
  if (m_pSelection)
    return &composite == &m_pSelection->Parent();
  return false;
}

void CResultSingleSelectDialog::Composite(const IValueComposite &composite, bool bEnable) {
  if (bEnable)
    Select(&composite.Component());
}

const CDerivedResult::CDerivedResultComponent *CResultSingleSelectDialog::ResultBeingCreated() const {
  return m_pResultBeingCreated;
}

void CResultSingleSelectDialog::BuildTree() {
  // Clear Tree
  m_lbValue.DeleteAllItems();

  if (m_bInput) {
    // If we have input, walk over points set
    TPointSetEntry &entry = (TPointSetEntry &)(*Model().GraphEntry(MD_BASE_POINTSET));
    TPointSetEntry::TNodeSet stNode = entry.EntryNodes();
    for (TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
      new CPointSetObserver(**it, *this);
  } else {
    typedef CSelectionLeafObserver_Delegate<IValueComposite, IValueComposite_Delegate, CResultSingleSelectDialog, TRUE>
        TCompositeObs;
    typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs,
                                              IValueComposite, CResultSingleSelectDialog, FALSE, FIXED_ITEM>
        TCompositeEntryObs;
    // Mesh result observer
    new TCompositeEntryObs(*Model().GraphEntry(MD_BASE_MESH_RESULT), m_lbValue, *this,
                           &CResultSingleSelectDialog::Composite, &CResultSingleSelectDialog::Composite);

    new CResultGroupObserver(Model().GVTResultTree(), *this);

    // If we have output walk over the results
    new CResultGroupObserver(Model().ResultTree(), *this);
  }
}

void CResultSingleSelectDialog::UpdateSelection() {
  // The combo box is filled with depletion stages and the first one is default selected.
  m_cbTimeStep.ResetContent();
  // Do we have an result with a depletion stage?
  const IResultComponent *pScalComp = dynamic_cast<const IResultComponent *>(m_pSelection);
  m_cbTimeStep.EnableWindow(m_bSetLinStage && pScalComp != 0);
  CButton *pChkSetLinStage = (CButton *)(GetDlgItem(IDC_CHK_SETLINSTAGE));
  pChkSetLinStage->EnableWindow(pScalComp != 0);
  pChkSetLinStage->SetCheck(pScalComp != 0 && m_bSetLinStage);
  if (!m_bSetLinStage) {
    GetDlgItem(IDC_RB_LINEAR)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_NON_LINEAR)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_HEAT)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_MIXTURE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_MIXTURE_CONTAINMENT)->EnableWindow(FALSE);
  }

  const IResultComponent *pComponent = dynamic_cast<const IResultComponent *>(m_pSelection);
  if (pComponent) {
    // Only add depletion stages to the combo which are supported by this result
    const IResult &result = dynamic_cast<const IResult &>(pComponent->Parent());

    const CDepletionStage *pStage = &Model().InitialDepletionStage();
    while (pStage) {
      if ((result.Mode(CAnalysisType::AT_LINEAR, *pStage) > -1) ||
          (result.Mode(CAnalysisType::AT_NONLIN, *pStage) > -1) ||
          (result.Mode(CAnalysisType::AT_HEAT, *pStage) > -1) ||
          (result.Mode(CAnalysisType::AT_MIXTURE, *pStage) > -1) ||
          (result.Mode(CAnalysisType::AT_MIXTURE_CONTAINMENT, *pStage) > -1)) {
        int nIndex = m_cbTimeStep.AddString(pStage->Name().toStdString().c_str());
        m_cbTimeStep.SetItemData(nIndex, (DWORD_PTR)pStage);
        if (m_pTimeStep == pStage)
          m_cbTimeStep.SetCurSel(nIndex);
      } else {
        // Stage is not supported by result. Reset selection
        if (m_pTimeStep == pStage)
          m_pTimeStep = 0;
      }

      // Go to next stage if any
      if (pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
    }

    // Is there a selection?
    assert(m_cbTimeStep.GetCount() > 0);
    if (m_pTimeStep == 0) {
      m_pTimeStep = (const CDepletionStage *)m_cbTimeStep.GetItemData(0);
      m_cbTimeStep.SetCurSel(0);
    }

    // Select an analysis type
    // Change mode ...
    CButton &btLin = (CButton &)*GetDlgItem(IDC_RB_LINEAR);
    CButton &btNonLin = (CButton &)*GetDlgItem(IDC_RB_NON_LINEAR);
    CButton &btHeat = (CButton &)*GetDlgItem(IDC_RB_HEAT);
    CButton &btMixture = (CButton &)*GetDlgItem(IDC_RB_MIXTURE);
    CButton &btMixtureContainment = (CButton &)*GetDlgItem(IDC_RB_MIXTURE_CONTAINMENT);
    btLin.SetCheck(m_antype == CAnalysisType::AT_LINEAR ? 1 : 0);
    btNonLin.SetCheck(m_antype == CAnalysisType::AT_NONLIN ? 1 : 0);
    btHeat.SetCheck(m_antype == CAnalysisType::AT_HEAT ? 1 : 0);
    btMixture.SetCheck(m_antype == CAnalysisType::AT_MIXTURE ? 1 : 0);
    btMixtureContainment.SetCheck(m_antype == CAnalysisType::AT_MIXTURE_CONTAINMENT ? 1 : 0);
    btLin.EnableWindow(!m_bInput && m_bSetLinStage && result.Mode(CAnalysisType::AT_LINEAR, *m_pTimeStep) > -1);
    btNonLin.EnableWindow(!m_bInput && m_bSetLinStage && result.Mode(CAnalysisType::AT_NONLIN, *m_pTimeStep) > -1);
    btHeat.EnableWindow(!m_bInput && m_bSetLinStage && result.Mode(CAnalysisType::AT_HEAT, *m_pTimeStep) > -1);
    btMixture.EnableWindow(!m_bInput && m_bSetLinStage && result.Mode(CAnalysisType::AT_MIXTURE, *m_pTimeStep) > -1);
    btMixtureContainment.EnableWindow(!m_bInput && m_bSetLinStage &&
                                      result.Mode(CAnalysisType::AT_MIXTURE_CONTAINMENT, *m_pTimeStep) > -1);

    if (m_bSetLinStage) {
      // Select the component
      int nNewMode = result.Mode(AnalysisType(), TimeStep());
      if (nNewMode == -1) {
        CAnalysisType otherType;
        if (m_antype == CAnalysisType::AT_LINEAR || m_antype == CAnalysisType::AT_NONLIN)
          otherType = (m_antype == CAnalysisType::AT_LINEAR ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR);

        if (otherType.Valid() && result.Mode(otherType, TimeStep()) != -1)
          m_antype = otherType;
        else
          m_pSelection = 0;

        UpdateSelection();
        return;
      } else {
        m_pSelection = &result.Component(m_pSelection->ComponentIndex(), nNewMode);
      }
    }
  }

  UpdateName();
}

void CResultSingleSelectDialog::OnSetLinStage() {
  CButton &button = (CButton &)*GetDlgItem(IDC_CHK_SETLINSTAGE);
  m_bSetLinStage = button.GetCheck() == 1;

  if (::IsWindow(m_lbValue.m_hWnd)) {
    UpdateSelection();
    m_lbValue.UpdateTree();
  }
}

void CResultSingleSelectDialog::OnCkName() {
  CButton &button = (CButton &)*GetDlgItem(IDC_CK_NAME);
  m_bGenerateName = button.GetCheck() == 1;
  UpdateName();
}

void CResultSingleSelectDialog::OnNameChange() { GetDlgItem(IDC_ED_NAME)->GetWindowText(m_sName); }

void CResultSingleSelectDialog::OnInput() {
  if (!m_bInput) {
    m_bInput = TRUE;
    m_pSelection = 0;
    BuildTree();
    UpdateSelection();
    GetDlgItem(IDC_CHK_SETLINSTAGE)->EnableWindow(FALSE);
  }
}

void CResultSingleSelectDialog::OnLinear() {
  m_antype = CAnalysisType::AT_LINEAR;
  UpdateSelection();
  m_lbValue.UpdateTree();
}

void CResultSingleSelectDialog::OnOutput() {
  if (m_bInput) {
    m_bInput = FALSE;
    m_pSelection = 0;
    BuildTree();
    UpdateSelection();
    GetDlgItem(IDC_CHK_SETLINSTAGE)->EnableWindow(TRUE);
  }
}

void CResultSingleSelectDialog::UpdateName() {
  if (m_bGenerateName) {
    if (m_pSelection) {
      const IResult *pDResult = NULL;
      if (m_bSetLinStage || !(pDResult = dynamic_cast<const IResult *>(&m_pSelection->Parent())))
        m_sName = m_pSelection->ExportLabel().toStdString().c_str();
      else {
        if (pDResult)
          m_sName = pDResult->ExportLabel(m_pSelection->ComponentIndex()).toStdString().c_str();
        else
          m_sName = m_pSelection->Parent().ExportLabel().toStdString().c_str();
      }
    } else {
      m_sName = "";
    }

    GetDlgItem(IDC_ED_NAME)->SetWindowText(m_sName);
  }
}

void CResultSingleSelectDialog::OnNonLinear() {
  m_antype = CAnalysisType::AT_NONLIN;
  UpdateSelection();
  m_lbValue.UpdateTree();
}

void CResultSingleSelectDialog::OnHeat() {
  m_antype = CAnalysisType::AT_HEAT;
  UpdateSelection();
  m_lbValue.UpdateTree();
}

void CResultSingleSelectDialog::OnMixture() {
  m_antype = CAnalysisType::AT_MIXTURE;
  UpdateSelection();
  m_lbValue.UpdateTree();
}

void CResultSingleSelectDialog::OnMixtureContainment() {
  m_antype = CAnalysisType::AT_MIXTURE_CONTAINMENT;
  UpdateSelection();
  m_lbValue.UpdateTree();
}

BOOL CResultSingleSelectDialog::Global() const { return m_bGlobal; }

BOOL CResultSingleSelectDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  UpdateSelection();
  BuildTree();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CResultSingleSelectDialog::Select(const IValueComponentBase *pSelection) {
  m_pSelection = pSelection;
  UpdateSelection();
  UpdateName();

  m_lbValue.UpdateTree();
}

void CResultSingleSelectDialog::OnChangeTimeStep() {
  assert(m_cbTimeStep.GetCurSel() > -1);

  int nIndex = m_cbTimeStep.GetCurSel();
  m_pTimeStep = (const CDepletionStage *)m_cbTimeStep.GetItemData(nIndex);

  UpdateSelection();
  m_lbValue.UpdateTree();
}

bool CResultSingleSelectDialog::Valid(const IValueComposite &composite) const {
  assert(composite.ModeSize() > 0);

  for (int i = 0; i < composite.ComponentSize(); ++i) {
    if (Valid(composite.Component(i))) {
      return true;
    }
  }

  return false;
}

bool CResultSingleSelectDialog::Valid(const IValueComponentBase &component) const { return true; }
