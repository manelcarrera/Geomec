// LicenseBorrowDlg.cpp : implementation file
//

#include "LicenseBorrowDlg.h"
#include "EPTFlexLm.h"
#include "Geomec.h"
#include "LicenseData.h"
#include "afxdialogex.h"
#include "stdafx.h"
// #include "ISettings.h"
// #include "stdafx.h"
#include "TnoWinApp.h"

#include "GeomecUtils.h"

#include <QString>

#if LICENSE_BORROWING

// CLicenseBorrowDlg dialog

IMPLEMENT_DYNAMIC(CLicenseBorrowDlg, CDialog)

CLicenseBorrowDlg::CLicenseBorrowDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CLicenseBorrowDlg::IDD, pParent), m_LicenseData(LicenseData::GetInstance()), m_BorrowEndDate() {}

CLicenseBorrowDlg::~CLicenseBorrowDlg() {}

BOOL CLicenseBorrowDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  if (m_font.CreatePointFont(80, "Courier New"))
    GetDlgItem(IDC_BORROWED_LICENSES)->SetFont(&m_font);

  fillBorrowList();

  return TRUE; // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLicenseBorrowDlg::OnBnClickedReturnLicense() {
  bool returnedBorrowedLicense = m_LicenseData->ReturnBorrowLicense();

  fillBorrowList();

  COivLicenseData::returnBorrow();
}

void CLicenseBorrowDlg::OnBnClickedRequestBorrow() {
  CTime borrowEndDate;
  DWORD dwFlag = m_BorrowEndDate.GetTime(borrowEndDate);

  if (dwFlag == GDT_VALID) {
    m_LicenseData->BorrowLicense(borrowEndDate.GetTime());
    fillBorrowList();
  }

  COivLicenseData::setupBorrow(borrowEndDate.GetDay(), borrowEndDate.GetMonth(), borrowEndDate.GetYear());
}

// protected

void CLicenseBorrowDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_BORROW_END_DATE, m_BorrowEndDate);
}

BEGIN_MESSAGE_MAP(CLicenseBorrowDlg, CDialog)
ON_BN_CLICKED(IDC_RETURN_LICENSE, &CLicenseBorrowDlg::OnBnClickedReturnLicense)
ON_BN_CLICKED(IDC_REQUEST_BORROW, &CLicenseBorrowDlg::OnBnClickedRequestBorrow)
ON_BN_CLICKED(IDC_BORROW_DIANA_BUTTON, &CLicenseBorrowDlg::OnBnClickedBorrowDianaButton)
END_MESSAGE_MAP()

// private

void CLicenseBorrowDlg::fillBorrowList() {
  char borrowListBuffer[EPTFLEXLM_MAX_MESS_LEN];
  char *borrowList = 0;

  borrowList = m_LicenseData->GetBorrowList(borrowListBuffer);

  SetDlgItemText(IDC_BORROWED_LICENSES, borrowList);
}

#endif

void CLicenseBorrowDlg::OnBnClickedBorrowDianaButton() {
  // http://stackoverflow.com/questions/4212002/how-to-find-out-from-the-windows-registry-where-ie-is-installed
  //
  HKEY hSoftKey = NULL;

  char buffer[MAX_PATH];

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE"), 0,
                   KEY_QUERY_VALUE, &hSoftKey) == ERROR_SUCCESS) {
    DWORD cbData = MAX_PATH;
    RegQueryValueEx(hSoftKey, NULL, NULL, NULL, (LPBYTE)buffer, &cbData);

    std::string cmdstr = std::string("\"");
    cmdstr += std::string(buffer);
    cmdstr += std::string("\" ");
    cmdstr += "http://localhost:1947/_int_/products.html";
    system(cmdstr.c_str());
  }
}
