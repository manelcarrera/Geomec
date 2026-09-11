// attriresulttree.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attriresulttree.h"
#include "resulttree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriResultTree dialog


CAttriResultTree::CAttriResultTree(CResultTree& result_tree, CWnd* pParent /*=NULL*/)
	: CDialog(CAttriResultTree::IDD, pParent), m_result_tree(result_tree)
{
	//{{AFX_DATA_INIT(CAttriResultTree)
	//}}AFX_DATA_INIT
}


void CAttriResultTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	int nType;
	if(!pDX->m_bSaveAndValidate)
	{
		switch(m_result_tree.WeightingType())
		{
		case geo::CValueMapper::WT_NONE:
			nType = 0;
			break;
		case geo::CValueMapper::WT_VOIGHT:
			nType = 1;
			break;
		case geo::CValueMapper::WT_REUSS:
			nType = 2;
			break;
		default:
			assert(false);
			break;
		};

	}

	//{{AFX_DATA_MAP(CAttriResultTree)
	DDX_Radio(pDX, IDC_RB_UNWEIGTHED, nType);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		switch(nType)
		{
		case 0:
			m_result_tree.WeightingType( geo::CValueMapper::WT_NONE );
			break;
		case 1:
			m_result_tree.WeightingType( geo::CValueMapper::WT_VOIGHT );
			break;
		case 2:
			m_result_tree.WeightingType( geo::CValueMapper::WT_REUSS );
			nType = 2;
			break;
		default:
			assert(false);
			break;
		};

	}
}


BEGIN_MESSAGE_MAP(CAttriResultTree, CDialog)
	//{{AFX_MSG_MAP(CAttriResultTree)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriResultTree message handlers
