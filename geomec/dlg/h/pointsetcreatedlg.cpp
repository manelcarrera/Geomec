// pointsetcreatedlg.cpp : implementation file
//

#include "stdafx.h"
#include <cmath>
#include "geomec.h"
#include "BaseEntryTypes.h"
#include "pointsetcreatedlg.h"
#include "BoundaryBase.h"
#include "PointSet.h"
#include "GlobalMessage.h"
#include "CreatePointSet.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointSetCreateDlg dialog

namespace
{

const QString DEFAULT_POINT_SET_NAME = QObject::tr("New PointSet%1");
const size_t DEFAULT_INITIAL_NUMBER = 0;

} // anonymous namespace

CPointSetCreateDlg::CPointSetCreateDlg(CModelBase& model, CWnd* pParent /*=NULL*/)
: CDialog(CPointSetCreateDlg::IDD, pParent),
  m_model(model),
  m_pointSetCoordinates(m_model, UnitNode(), false),
  m_bValidateDensityOnly(FALSE)
{
  QString name = model.GraphEntry(MD_BASE_POINTSET)->createUniqueName(
  DEFAULT_POINT_SET_NAME, DEFAULT_INITIAL_NUMBER);

  m_sName = name.toStdString().c_str();

  //{{AFX_DATA_INIT(CPointSetCreateDlg)
  //}}AFX_DATA_INIT
}

void CPointSetCreateDlg::DoDataExchange(CDataExchange* pDX)
{
  assert(m_pointSetCoordinates().size() > 0);
  CDialog::DoDataExchange(pDX);
  int nDimension = 0;

  if(!pDX->m_bSaveAndValidate)
  {
    GetDlgItem(IDC_ED_MIN_VAL_II)->EnableWindow(m_pointSetCoordinates().size() > 1);
    GetDlgItem(IDC_ED_MAX_VAL_II)->EnableWindow(m_pointSetCoordinates().size() > 1);
    GetDlgItem(IDC_ED_DENSITY_II)->EnableWindow(m_pointSetCoordinates().size() > 1);
    //GetDlgItem(IDC_ED_MIN_VAL_III)->EnableWindow(FALSE);		
    //GetDlgItem(IDC_ED_MAX_VAL_III)->EnableWindow(FALSE);
    GetDlgItem(IDC_ED_MIN_VAL_III)->EnableWindow(m_pointSetCoordinates().size() == 3);		
    GetDlgItem(IDC_ED_MAX_VAL_III)->EnableWindow(m_pointSetCoordinates().size() == 3);
    GetDlgItem(IDC_ED_DENSITY_III)->EnableWindow(m_pointSetCoordinates().size() > 2);

    if(m_pointSetCoordinates().size() == 3)
      nDimension = 1;
  }

  CLengthQuantity q;
  CString name0(m_pointSetCoordinates()[0].m_name.toStdString().c_str());
  DDX_Text(pDX, IDC_ST_MIN_I, name0);
  DDX_Text(pDX, IDC_ST_MAX_I, name0);
  DDX_Text(pDX, IDC_ED_MIN_VAL_I, m_pointSetCoordinates()[0].m_min);
  DDX_Text(pDX, IDC_ED_MAX_VAL_I, m_pointSetCoordinates()[0].m_max);
  GetDlgItem(IDC_ST_UNIT1)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
  GetDlgItem(IDC_ST_UNIT4)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
  DDX_Text(pDX, IDC_ST_DENSITY_I, name0);
  DDX_Text(pDX, IDC_ED_DENSITY_I, m_pointSetCoordinates()[0].m_density);

  if( m_pointSetCoordinates().size() > 1 )
  {
  CString name1(m_pointSetCoordinates()[1].m_name.toStdString().c_str());
    DDX_Text(pDX, IDC_ST_MIN_II, name1);
    DDX_Text(pDX, IDC_ST_MAX_II, name1);
    DDX_Text(pDX, IDC_ED_MIN_VAL_II, m_pointSetCoordinates()[1].m_min);
    DDX_Text(pDX, IDC_ED_MAX_VAL_II, m_pointSetCoordinates()[1].m_max);
    GetDlgItem(IDC_ST_UNIT2)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
    GetDlgItem(IDC_ST_UNIT5)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
    DDX_Text(pDX, IDC_ST_DENSITY_II, name1);
    DDX_Text(pDX, IDC_ED_DENSITY_II, m_pointSetCoordinates()[1].m_density);
  }
  else
  {
    GetDlgItem(IDC_ST_MIN_II)->SetWindowText("");
    GetDlgItem(IDC_ST_MAX_II)->SetWindowText("");
    GetDlgItem(IDC_ED_MIN_VAL_II)->SetWindowText("");
    GetDlgItem(IDC_ED_MAX_VAL_II)->SetWindowText("");
    GetDlgItem(IDC_ST_UNIT2)->SetWindowText("");
    GetDlgItem(IDC_ST_UNIT5)->SetWindowText("");
    GetDlgItem(IDC_ST_DENSITY_II)->SetWindowText("");
    GetDlgItem(IDC_ED_DENSITY_II)->SetWindowText("");
  }

  if( m_pointSetCoordinates().size() > 2 )
  {
  CString name2(m_pointSetCoordinates()[2].m_name.toStdString().c_str());
    DDX_Text(pDX, IDC_ST_MIN_III, name2);
    DDX_Text(pDX, IDC_ST_MAX_III, name2);
    DDX_Text(pDX, IDC_ED_MIN_VAL_III, m_pointSetCoordinates()[2].m_min);
    DDX_Text(pDX, IDC_ED_MAX_VAL_III, m_pointSetCoordinates()[2].m_max);
    GetDlgItem(IDC_ST_UNIT3)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
    GetDlgItem(IDC_ST_UNIT6)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
    DDX_Text(pDX, IDC_ST_DENSITY_III, name2);
    DDX_Text(pDX, IDC_ED_DENSITY_III, m_pointSetCoordinates()[2].m_density);
  }
  else
  {
    GetDlgItem(IDC_ST_MIN_III)->SetWindowText("");
    GetDlgItem(IDC_ST_MAX_III)->SetWindowText("");
    GetDlgItem(IDC_ED_MIN_VAL_III)->SetWindowText("");
    GetDlgItem(IDC_ED_MAX_VAL_III)->SetWindowText("");
    GetDlgItem(IDC_ST_UNIT3)->SetWindowText("");
    GetDlgItem(IDC_ST_UNIT6)->SetWindowText("");
    GetDlgItem(IDC_ST_DENSITY_III)->SetWindowText("");
    GetDlgItem(IDC_ED_DENSITY_III)->SetWindowText("");
  }

  //{{AFX_DATA_MAP(CPointSetCreateDlg)
  DDX_Text(pDX, IDC_ED_NAME, m_sName);
  DDX_Radio(pDX, IDC_RB_2D, nDimension);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    if(!m_bValidateDensityOnly)
    {
      for(size_t i = 0; i < m_pointSetCoordinates().size(); i++)
      {
        if(m_pointSetCoordinates()[i].m_min > m_pointSetCoordinates()[i].m_max)
        {
          CString sMessage;
          sMessage.Format(_T("Maximum of %s is smaller than minimum"), m_pointSetCoordinates()[i].m_name);
          _m()->msg(sMessage);
          pDX->Fail();
        }

        if(m_pointSetCoordinates()[i].m_density < 1)
        {
          _m()->msg("Density must be greater than 0");
          pDX->Fail();
        }
      }
    }

    // Create the pointset
    CreatePointSet();
  }

}


BEGIN_MESSAGE_MAP(CPointSetCreateDlg, CDialog)
  //{{AFX_MSG_MAP(CPointSetCreateDlg)
  ON_BN_CLICKED(IDC_RB_2D, OnDimensionChanged)
  ON_BN_CLICKED(IDC_RB_3D, OnDimensionChanged)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CPointSetCreateDlg message handlers

const CUnitNode& CPointSetCreateDlg::UnitNode()
{
  CGeomecDoc& doc = ((CGeomecApp*)AfxGetApp())->GetDoc();
  return doc.UnitNode();
}

void CPointSetCreateDlg::CreatePointSet()
{
  CWaitCursor c;
  CCreatePointSet createPointSet(m_pointSetCoordinates);
  CPointSet* pPointSet =
    createPointSet.create(m_model, UnitNode(), (LPCTSTR) m_sName);
}
        
void CPointSetCreateDlg::OnDimensionChanged() 
{
  CButton& button = (CButton&)*GetDlgItem(IDC_RB_2D);
  m_pointSetCoordinates.initialize(button.GetCheck() == 1);
  UpdateData(FALSE);
}
