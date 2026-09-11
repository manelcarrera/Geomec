// newxsecdlg.cpp : implementation file
//

#include "newxsecdlg.h"
#include "boundarybase.h"
#include "geomec.h"
#include "modelbase.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewXSecDlg dialog

CNewXSecDlg::CNewXSecDlg(CGraphModel &model, CWnd *pParent /*=NULL*/)
    : CDialog(CNewXSecDlg::IDD, pParent), m_model(model), m_dFirstNorth(0), m_dFirstEast(0), m_dSecondNorth(0),
      m_dSecondEast(0) {
  //{{AFX_DATA_INIT(CNewXSecDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

const CUnitNode &CNewXSecDlg::UnitNode() {
  CGeomecDoc &doc = ((CGeomecApp *)AfxGetApp())->GetDoc();
  return doc.UnitNode();
}

void CNewXSecDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  CString strUnit;

  CLengthQuantity qnLength;

  if (!pDX->m_bSaveAndValidate) {
    switch (UnitNode().Unit()) {
    case CQuantity::SI_UNIT:
      strUnit.Format("m");
      break;
    case CQuantity::FIELD_UNIT:
      strUnit.Format("ft");
      break;
    }
  }
  //{{AFX_DATA_MAP(CWellPathDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_FIRST_NORTH, m_dFirstNorth);
  DDX_Text(pDX, IDC_FIRST_EAST, m_dFirstEast);
  DDX_Text(pDX, IDC_SECOND_NORTH, m_dSecondNorth);
  DDX_Text(pDX, IDC_SECOND_EAST, m_dSecondEast);

  DDX_Text(pDX, IDC_ST_FIRSTPT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_ST_FIRSTPT_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_ST_SECONDPT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_ST_SECONDPT_EASTING_UNIT, strUnit);

  m_dFirstNorth = qnLength.Convert(m_dFirstNorth, CQuantity::SI_UNIT, UnitNode().Unit());
  m_dFirstEast = qnLength.Convert(m_dFirstEast, CQuantity::SI_UNIT, UnitNode().Unit());
  m_dSecondNorth = qnLength.Convert(m_dSecondNorth, CQuantity::SI_UNIT, UnitNode().Unit());
  m_dSecondEast = qnLength.Convert(m_dSecondEast, CQuantity::SI_UNIT, UnitNode().Unit());

  if (pDX->m_bSaveAndValidate) {
    // Validate your data
    CModelBase *pModel = dynamic_cast<CModelBase *>(&(m_model));
    ASSERT(pModel);

    if (!((m_dFirstNorth >= pModel->Boundary().Min().X() && m_dFirstNorth <= pModel->Boundary().Max().X()) &&
          (m_dFirstEast >= pModel->Boundary().Min().Y() && m_dFirstEast <= pModel->Boundary().Max().Y()))) {
      AfxMessageBox("First reference point is outside the model");
      pDX->Fail();
      return;
    }
    if (!((m_dSecondNorth >= pModel->Boundary().Min().X() && m_dSecondNorth <= pModel->Boundary().Max().X()) &&
          (m_dSecondEast >= pModel->Boundary().Min().Y() && m_dSecondEast <= pModel->Boundary().Max().Y()))) {
      AfxMessageBox("Second reference point is outside the model");
      pDX->Fail();
      return;
    }
  }
}

BEGIN_MESSAGE_MAP(CNewXSecDlg, CDialog)
//{{AFX_MSG_MAP(CNewXSecDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewXSecDlg message handlers
