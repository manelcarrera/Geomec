// DianaXPpg.cpp : Implementation of the CDianaXPropPage property page class.

#include "stdafx.h"
#include "DianaX.h"
#include "DianaXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


IMPLEMENT_DYNCREATE(CDianaXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDianaXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDianaXPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDianaXPropPage, "DIANAX.DianaXPropPage.1",
	0x3a01bc18, 0xba9f, 0x11d2, 0xb3, 0xb4, 0, 0x10, 0x4b, 0x25, 0x56, 0xd9)


/////////////////////////////////////////////////////////////////////////////
// CDianaXPropPage::CDianaXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDianaXPropPage

BOOL CDianaXPropPage::CDianaXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DIANAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXPropPage::CDianaXPropPage - Constructor

CDianaXPropPage::CDianaXPropPage() :
	COlePropertyPage(IDD, IDS_DIANAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDianaXPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXPropPage::DoDataExchange - Moves data between page and properties

void CDianaXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDianaXPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXPropPage message handlers
