// MeshDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "MeshDlgBar.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "GeomecDianaRunner.h"
#include "meshbase.h"
#include "resultregister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshDlgBar dialog


//CMeshDlgBar::CMeshDlgBar(CWnd* pParent /*=NULL*/)
CMeshDlgBar::CMeshDlgBar(): m_nCalcSelection(0), m_nDeplSelection(0)
  //: CDialogBar(CMeshDlgBar::IDD, pParent)
{
  
  //{{AFX_DATA_INIT(CMeshDlgBar)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CMeshDlgBar::DoDataExchange(CDataExchange* pDX)
{
  
  CDialogBar::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMeshDlgBar)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

// No real toggle, but an interface to the combo boxes to enable or disable them
void CMeshDlgBar::ToggleCombos(BOOL bValue1, BOOL bValue2, int Sel1, int Sel2)
{
  CComboBox *pDepStage = (CComboBox*)GetDlgItem(IDC_DEPSTAGE_DLGBAR);
  CComboBox *pCalcType = (CComboBox*)GetDlgItem(IDC_CALCTYPE_DLGBAR);
  pDepStage->EnableWindow(bValue1);
  pCalcType->EnableWindow(bValue2);
  pDepStage->SetCurSel(Sel1);
  pCalcType->SetCurSel(Sel2);

  if(bValue1 || bValue2)
    EnableToolTips(TRUE);
}

void CMeshDlgBar::ResetCombos()
{
  CComboBox *pDepStage = (CComboBox*)GetDlgItem(IDC_DEPSTAGE_DLGBAR);
  CComboBox *pCalcType = (CComboBox*)GetDlgItem(IDC_CALCTYPE_DLGBAR);
  pDepStage->ResetContent();
  pCalcType->ResetContent();
  pDepStage->InsertString(0, "(Depletion Stage)");
  pCalcType->InsertString(0, "(Calculation Type)");
}

void CMeshDlgBar::UpdateLists()
{
  CComboBox *pDepStage = (CComboBox*)GetDlgItem(IDC_DEPSTAGE_DLGBAR);
  CComboBox *pCalcType = (CComboBox*)GetDlgItem(IDC_CALCTYPE_DLGBAR);

// Clear the comboboxes befor they are filled again
  pDepStage->ResetContent();
  pCalcType->ResetContent();
  pDepStage->InsertString(0, "(Depletion Stage)");
  pCalcType->InsertString(0, "(Calculation Type)");


// We need the geomecdoc....
  CGeomecDoc* pDoc = Doc();

// Check if there are any results / is any model, else disable the combo boxes.
  CModelBase *pModel = (CModelBase*)pDoc->Model();
  
  if(!pModel)
  {
    ToggleCombos(FALSE, FALSE, 0, 0);
    return;
  }
  
  CResultRegister *pRegister = &(pModel->ResultRegister());
  if(!pRegister->Linear() && !pRegister->NonLinear())
  {
    ToggleCombos(FALSE, FALSE, 0, 0);
    return;
  }

// Results exist...
// Check whether there are nonlinear results
  BOOL bLinearOnly = FALSE;
  int nCalcSel = 0;
  
  if(pRegister->NonLinear())
  {
    pCalcType->InsertString(1, "Linear");
    pCalcType->InsertString(2, "NonLinear");
  }
  else
  {
    bLinearOnly = TRUE;
    pCalcType->InsertString(1, "Linear");
    nCalcSel = 1;
  }
    
  CDepletionStage *pStage = &(pModel->InitialDepletionStage());
  int i = 1;
  while(pStage)
  {
    pDepStage->InsertString(i, pStage->Name());
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &(pStage->Next());
    i++;
  }
  
  ToggleCombos(TRUE, !bLinearOnly, 0, nCalcSel);
}

CGeomecDoc *CMeshDlgBar::Doc()
{
  POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
  POSITION docpos = pDocTemplate->GetFirstDocPosition();
  return (CGeomecDoc*)(pDocTemplate->GetNextDoc(docpos));
}

BEGIN_MESSAGE_MAP(CMeshDlgBar, CDialogBar)
  //{{AFX_MSG_MAP(CMeshDlgBar)
  ON_CBN_SELCHANGE(IDC_CALCTYPE_DLGBAR, OnSelectionCalcType)
  ON_CBN_SELCHANGE(IDC_DEPSTAGE_DLGBAR, OnSelectionDepStage)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshDlgBar message handlers

void CMeshDlgBar::OnSelectionCalcType() 
{
  CComboBox *pCalcType = (CComboBox*)GetDlgItem(IDC_CALCTYPE_DLGBAR);
  int nType = pCalcType->GetCurSel();

  if(nType == m_nCalcSelection)
    return; // nothing has changed, return.
  else
    m_nCalcSelection = nType;

  CGeomecDoc *pDoc = Doc();
  CModelBase *pModel = (CModelBase*)(pDoc->Model());
  CMeshBase *pMesh = &(pModel->Mesh());

  if(nType == 1)
    pMesh->Linear(true);
  else
    pMesh->Linear(false);
}

void CMeshDlgBar::OnSelectionDepStage() 
{
  CComboBox *pDepStage = (CComboBox*)GetDlgItem(IDC_DEPSTAGE_DLGBAR);
  int nStage = pDepStage->GetCurSel();

  if(nStage == m_nDeplSelection)
    return; // nothing has changed, return.
  else
    m_nDeplSelection = nStage;

  CGeomecDoc *pDoc = Doc();
  CModelBase *pModel = (CModelBase*)(pDoc->Model());
  CMeshBase *pMesh = &(pModel->Mesh());
  
  pMesh->DepletionNr(nStage - 1);

}

BOOL CMeshDlgBar::OnToolTipNotify(UINT id, NMHDR *pTTTStruct, LRESULT *pResult)
{
  TOOLTIPTEXT *pText = (TOOLTIPTEXT*) pTTTStruct;
  UINT nID = pTTTStruct->idFrom;
  nID = ::GetDlgCtrlID((HWND)nID);

  if(nID)
  {
    if(nID == IDC_DEPSTAGE_DLGBAR)
      pText->lpszText = "Select the (deformed) mesh you want to view";
    else if(nID == IDC_CALCTYPE_DLGBAR)
      pText->lpszText = "Select the calculation type you want to view";
    else ASSERT(FALSE);

    return TRUE;
  }
  return FALSE;
}