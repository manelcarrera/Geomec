// attriglobalstressdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attriglobalstressdlg.h"
#include "ModelBase.h"
#include "BranchState.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriGlobalStressDlg dialog


CAttriGlobalStressDlg::CAttriGlobalStressDlg(CGlobalInitialStressNode& stress, CWnd* pParent /*=NULL*/)
: CAttributesTemplate<CGlobalInitialStressNode>(CAttriGlobalStressDlg::IDD, stress, pParent)
{
  //{{AFX_DATA_INIT(CAttriGlobalStressDlg)
  //}}AFX_DATA_INIT
}

void CAttriGlobalStressDlg::DoDataExchange(CDataExchange* pDX)
{

  double dWtrClmStrGrd, dWatSurfDepth;
  CString strWatSurfDepth, strWtrClmStrGrd;

  if(!pDX->m_bSaveAndValidate)
  {
    // Values
    dWatSurfDepth = Copy().WaterSurfaceDepth().Value(UnitNode().Unit());
    dWtrClmStrGrd = Copy().WaterColumnStressGradient().Value(UnitNode().Unit());

    // Units
    strWatSurfDepth = CString(Copy().WaterSurfaceDepth().UnitName(UnitNode().Unit()).c_str());
    strWtrClmStrGrd = CString(Copy().WaterColumnStressGradient().UnitName(UnitNode().Unit()).c_str());
  }

  CAttributesTemplate<CGlobalInitialStressNode>::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAttriGlobalStressDlg)
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_UNIT_WATER_DEPTH, strWatSurfDepth);
  DDX_Text(pDX, IDC_WATER_SURF_DEPTH, dWatSurfDepth);
  DDV_MinMaxDouble(pDX, 
           dWatSurfDepth, 
           Copy().WaterSurfaceDepth().MinValue( UnitNode().Unit()),
           Copy().WaterSurfaceDepth().MaxValue( UnitNode().Unit()));


  DDX_Text(pDX, IDC_ST_WATER_COL_STR_GRAD, dWtrClmStrGrd);
  DDX_Text(pDX, IDC_UNIT_WATER_COL_STR_GRAD, strWtrClmStrGrd);
  DDV_MinMaxDouble(pDX, 
           dWtrClmStrGrd, 
           Copy().WaterColumnStressGradient().MinValue( UnitNode().Unit()),
           Copy().WaterColumnStressGradient().MaxValue( UnitNode().Unit()));

  if(pDX->m_bSaveAndValidate)
  {
    Copy().WaterSurfaceDepth(dWatSurfDepth, UnitNode().Unit());
    Copy().WaterColumnStressGradient(dWtrClmStrGrd, UnitNode().Unit());
  }
}


BEGIN_MESSAGE_MAP(CAttriGlobalStressDlg, CAttributesTemplate<CGlobalInitialStressNode>)
  //{{AFX_MSG_MAP(CAttriGlobalStressDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriGlobalStressDlg message handlers
BOOL CAttriGlobalStressDlg::OnInitDialog()
{
  CAttributesTemplate<CGlobalInitialStressNode>::OnInitDialog();

  if((static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch())
  {
    GetDlgItem(IDC_WATER_SURF_DEPTH)->EnableWindow(FALSE);
    GetDlgItem(IDC_ST_WATER_COL_STR_GRAD)->EnableWindow(FALSE);
  }

  return FALSE;
}
