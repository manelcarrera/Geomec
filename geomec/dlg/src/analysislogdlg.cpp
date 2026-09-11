// analysislogdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "analysislogdlg.h"
#include "GeomecStringTable.h"
#include "sharedResource.h"
#include "GlobalMessage.h"
#include "resourceIDS.h"
#include "resourceIDP.h"
#include "FemAppMainWindow.h"
#include "analysislogger.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalysisLogDlg dialog


CAnalysisLogDlg::CAnalysisLogDlg(CAnalysisLogger &logger, CWnd* pParent /*=NULL*/)
: m_logger(logger),
  CDialog(CAnalysisLogDlg::IDD, pParent),
  m_nVerbose(0)
{
	//{{AFX_DATA_INIT(CAnalysisLogDlg)
	//}}AFX_DATA_INIT
}


void CAnalysisLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalysisLogDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
  DDX_Check(pDX, IDC_VERBOSE, m_nVerbose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalysisLogDlg, CDialog)
	//{{AFX_MSG_MAP(CAnalysisLogDlg)
	ON_BN_CLICKED(IDC_BUT_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_BUT_EXPORT, OnExport)
  ON_BN_CLICKED(IDC_VERBOSE, OnVerbose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalysisLogDlg message handlers

void CAnalysisLogDlg::UpdateControls()
{
	m_List.ResetContent();
	FillControl((m_nVerbose == 0), (m_nVerbose != 0));

	GetDlgItem(IDC_BUT_EXPORT)->EnableWindow(!m_logger.IsEmpty());
	GetDlgItem(IDC_BUT_CLEAR)->EnableWindow(!m_logger.IsEmpty());
}

BOOL CAnalysisLogDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalysisLogDlg::OnExport()
{
	CString strFilter;
	strFilter = getStringTableEntry(IDS_ANALYSISLOGFILTER);
	CFileDialog dlg(FALSE, "log", "geomec", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, FemAppGetMainWnd());

	if(dlg.DoModal() == IDOK)
	{
		bool bRet = m_logger.ExportToFile(std::string(dlg.GetPathName()), m_nVerbose != 0);
		if(!bRet) _m()->msg(IDP_EXPORTANALYSISLOGFAIL);
	}
}

void CAnalysisLogDlg::OnClear()
{
	if(_m()->msg(IDP_CONFIRMCLEARANALYSISLOG, MB_YESNO) == IDYES)
	{
		m_logger.Empty();
		UpdateControls();
	}
}

void CAnalysisLogDlg::OnVerbose()
{
  UpdateData(TRUE);
  UpdateControls();
}

// private

void CAnalysisLogDlg::FillControl(bool bFilterBadShapeWarnings, bool bVerbose)
{
  int i;
  if(bFilterBadShapeWarnings)
  {
    std::vector<std::string> vcLines;
    for(i = 0; i < m_logger.LineSize(); ++i)
    {
      if(bVerbose || !m_logger.Verbose(i))
      {
        const std::string& line = m_logger.Line(i);
        if(line.find("Bad shaped element") != std::string::npos ||
           (line.find("less than") != std::string::npos && line.find("degrees or larger than") != std::string::npos))
        {
          size_t nLinesToRemove = std::min<size_t>(vcLines.size(), 4);
          for(size_t j = 0; j < nLinesToRemove; ++j)
            vcLines.pop_back();
        }
        else
          vcLines.push_back(line);
      }
    }

    for(size_t j = 0; j < vcLines.size(); ++j)
      m_List.AddString(vcLines[j].c_str());
  }
  else
  {
    for(i = 0; i < m_logger.LineSize(); ++i)
    {
      if(bVerbose || !m_logger.Verbose(i))
        m_List.AddString(CString(m_logger.Line(i).c_str()));
    }
  }

  if(m_logger.LineSize() > 0)
  {
    m_List.SetTopIndex(m_logger.LineSize() - 1);
  }
}
