// wellinfodlg.cpp : implementation file
//

#include "stdafx.h"
#include "wellschemeutils.h"
#include "wellinfodlg.h"

#include "wellpathbase.h"
#include "dateeditdlg.h"
#include <qdatetimeedit.h>

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellInfoDlg dialog


CWellInfoDlg::CWellInfoDlg(well::CWellPathBase &well_path, CWnd* pParent /*=NULL*/)
	: CDialog(CWellInfoDlg::IDD, pParent), m_WellPath(well_path)
{
	m_spud_date.setDate(m_WellPath.GetSpudDate().year(), m_WellPath.GetSpudDate().month(), m_WellPath.GetSpudDate().day());
	//{{AFX_DATA_INIT(CWellInfoDlg)
	m_strCommonName = m_WellPath.GetWellName().toStdString().c_str();
	m_strCountry = m_WellPath.GetCountry().toStdString().c_str();
	m_strCustomerName = m_WellPath.GetCustomerName().toStdString().c_str();
	m_strWellDescr = m_WellPath.GetDescription().toStdString().c_str();
	m_strField = m_WellPath.GetFieldName().toStdString().c_str();
	m_strJobNumber = m_WellPath.GetJobNumber().toStdString().c_str();
	m_strLocName = m_WellPath.GetLocationName().toStdString().c_str();
	m_strWellType = m_WellPath.GetWellType().toStdString().c_str();
	m_strCustContact = m_WellPath.GetCustomerContact().toStdString().c_str();
	m_strDate = m_spud_date.toString("ddd d MMM yyyy").toStdString().c_str();
	//}}AFX_DATA_INIT

	
}


void CWellInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWellInfoDlg)
	DDX_Text(pDX, IDC_ED_COMM_NAME, m_strCommonName);
	DDX_Text(pDX, IDC_ED_COUNTRY, m_strCountry);
	DDX_Text(pDX, IDC_ED_CUST_NAME, m_strCustomerName);
	DDX_Text(pDX, IDC_ED_DESCRIPTION, m_strWellDescr);
	DDX_Text(pDX, IDC_ED_FIELD, m_strField);
	DDX_Text(pDX, IDC_ED_JOB_NR, m_strJobNumber);
	DDX_Text(pDX, IDC_ED_LOC_NAME, m_strLocName);
	DDX_Text(pDX, IDC_ED_WELL_TYPE, m_strWellType);
	DDX_Text(pDX, IDC_ED_CUST_CONTACT, m_strCustContact);
	DDX_Text(pDX, IDC_ED_SPUD_DATE, m_strDate);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		m_WellPath.SetWellName((LPCSTR) m_strCommonName);
		m_WellPath.SetCountry((LPCSTR) m_strCountry);
		m_WellPath.SetCustomerName((LPCSTR) m_strCustomerName);
		m_WellPath.SetCustomerContact((LPCSTR) m_strCustContact);
		m_WellPath.SetDescription((LPCSTR) m_strWellDescr);
		m_WellPath.SetFieldName((LPCSTR) m_strField);
		m_WellPath.SetJobNumber((LPCSTR) m_strJobNumber);
		m_WellPath.SetLocationName((LPCSTR) m_strLocName);
		m_WellPath.SetWellType((LPCSTR) m_strWellType);
		m_WellPath.SetSpudDate(m_spud_date);
	}
}


BEGIN_MESSAGE_MAP(CWellInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CWellInfoDlg)
	ON_BN_CLICKED(IDC_BTN_SPUD_CHANGE, OnBtnSpudChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellInfoDlg message handlers

void CWellInfoDlg::OnBtnSpudChange() 
{
	COleDateTime time(m_spud_date.year(), m_spud_date.month(), m_spud_date.day(), 0, 0, 0);

	CRect rect;
	
	CButton *btn = (CButton*)(GetDlgItem(IDC_BTN_SPUD_CHANGE));
	btn->GetWindowRect(&rect);
	CPoint ptScreen(rect.CenterPoint());
	CDateEditDlg dlg(time, ptScreen, this);

	
	if(dlg.DoModal() == IDOK)
	{
		m_spud_date.setDate(dlg.DateTime().GetYear(), dlg.DateTime().GetMonth(), dlg.DateTime().GetDay());
		CEdit *edit = (CEdit*)(GetDlgItem(IDC_ED_SPUD_DATE));
		edit->SetWindowText(m_spud_date.toString("ddd d MMM yyyy").toStdString().c_str());
	}

}
