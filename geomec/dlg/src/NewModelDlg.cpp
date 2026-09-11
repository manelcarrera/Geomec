// NewModelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "NewModelDlg.h"
#include "GlobalMessage.h"
#include "FemAppModel.h"
#include "resourceIDS.h"
#include "VersionNumbers.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif


/////////////////////////////////////////////////////////////////////////////
// CNewModelDlg dialog


//##ModelId=3B6905BE008E
CNewModelDlg::CNewModelDlg(std::vector<ModelInfo> &vcModel, CWnd* pParent /*=NULL*/)
	: CDialog(CNewModelDlg::IDD, pParent)
{
	m_vcModel = vcModel;
	assert(m_vcModel.size() > 0);	// In case some caller tries neglect our pre-conditions

	m_nSelectedModel = -1;
	// Initialize the image list
	m_pImageList = new CImageList();
	m_pImageList->Create(32, 32, ILC_COLOR, 1, 7);

	//{{AFX_DATA_INIT(CNewModelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3B6905BE00AC
CNewModelDlg::~CNewModelDlg() 
{
	delete m_pImageList;
}


//##ModelId=3B6905BE00BC
void CNewModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewModelDlg)
	DDX_Control(pDX, IDC_LB_MODEL, m_lbModel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewModelDlg, CDialog)
	//{{AFX_MSG_MAP(CNewModelDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LB_MODEL, OnDblclkLbModel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewModelDlg message handlers

//##ModelId=3B6905BE00DB
BOOL CNewModelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the imagelist
	assert(m_pImageList);
	m_lbModel.SetImageList(m_pImageList, LVSIL_NORMAL);

	// We fill the dialog
	for(size_t i = 0; i < m_vcModel.size(); i++) {
		int nItem = m_lbModel.InsertItem(m_lbModel.GetItemCount(),
										 (const char*)m_vcModel[i].sType.toStdString().c_str(),
										 IconToImage(m_vcModel[i].nIcon));

		m_lbModel.SetItemData(nItem, (DWORD_PTR)(m_vcModel[i].nType));
	}

	// We must have at least one item in the control
	assert(m_lbModel.GetItemCount() != 0);

	// Select the first one
	m_lbModel.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

	//##ModelId=3A71504A02FF
int CNewModelDlg::IconToImage(const unsigned int uIconID)
{
	if(m_mpResToImage.find(uIconID) == m_mpResToImage.end()) {
		// Item not found in image list
		int nImage = m_pImageList->Add(AfxGetApp()->LoadIcon(uIconID));
		if(nImage != -1)
			m_mpResToImage.insert(std::pair<unsigned int, int>(uIconID, nImage));
	}

	return (*m_mpResToImage.find(uIconID)).second;
}




//##ModelId=3B6905BE00FA
void CNewModelDlg::OnCancel() 
{
	// Assure a unselected item
	m_nSelectedModel = -1;
	
	CDialog::OnCancel();
}

//##ModelId=3B6905BE010B
void CNewModelDlg::OnOK() 
{
	// Defaultly we have no selection
	m_nSelectedModel = -1;

	// Get the selection
	POSITION pos = m_lbModel.GetFirstSelectedItemPosition();

	if(!pos) {
		// No model selected -> say it to the user and return
		_m()->msg(IDS_NO_MODEL_SELECTED);
		return ;
	}

	// We have a selection
	int nItem = m_lbModel.GetNextSelectedItem(pos);
	m_nSelectedModel = (m_lbModel.GetItemData(nItem));

	// Based on our single selection criterium we expect no next selection
	assert(pos == 0);
	
	// Lets MFC finish the job
	CDialog::OnOK();
}

//##ModelId=3B6905BE007D
int CNewModelDlg::SelectedModel() const
{
	return m_nSelectedModel;
}

//##ModelId=3B6905BE011A
void CNewModelDlg::OnDblclkLbModel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	// Get the selection
	POSITION pos = m_lbModel.GetFirstSelectedItemPosition();
	if(pos) OnOK();
	*pResult = 0;
}
