// WellEditFrame.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WellEditDlg.h"
#include "WellEditFrame.h"
#include "OpenGLView.h"
#include "OpenGLScene.h"
#include "zoomwindowdragger.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellEditFrame

IMPLEMENT_DYNCREATE(CWellEditFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CWellEditFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CWellEditFrame)
	ON_BN_CLICKED(IDC_BUTTON_SPREAD_OK, OnButtonSpreadOK)
	ON_BN_CLICKED(IDC_BUTTON_SPREAD_CANCEL, OnButtonSpreadCancel)
	ON_BN_CLICKED(ID_CHECK_SPREAD_AZIMUTH, OnCheckSpreadAzimuth)
	ON_CBN_CLOSEUP(ID_COMBO_SPREAD_TYPE, OnCloseupComboSpreadType)
	ON_COMMAND(ED_ID_3DV_LOCK_DEPTH, On3dvLockDepth)
	ON_COMMAND(ED_ID_3DV_LOCK_EAST, On3dvLockEast)
	ON_COMMAND(ED_ID_3DV_LOCK_NORTH, On3dvLockNorth)
	ON_COMMAND(ED_ID_3DV_LOCK_RESET, On3dvLockReset)
	ON_COMMAND(ED_ID_3DV_ZOOM_RESET, On3dvZoomReset)
	ON_COMMAND(ED_ID_3DV_ZOOM_WINDOW, On3dvZoomWindow)
	ON_COMMAND(ED_ID_3DV_ZOOMIN, On3dvZoomin)
	ON_COMMAND(ED_ID_3DV_ZOOMOUT, On3dvZoomout)
	ON_COMMAND(ED_ID_3DV_VIEW_AXIS, On3dvViewAxis)
	ON_COMMAND(ED_ID_3DV_VIEW_BE_DIREC, On3dvViewBeDirec)
	ON_COMMAND(ED_ID_3DV_VIEW_BOUNDARY, On3dvViewBoundary)
	ON_COMMAND(ED_ID_3DV_VIEW_E_DIREC, On3dvViewEDirec)
	ON_COMMAND(ED_ID_3DV_VIEW_LIGHTING, On3dvViewLighting)
	ON_COMMAND(ED_ID_3DV_VIEW_N_DIREC, On3dvViewNDirec)
	ON_COMMAND(ED_ID_3DV_VIEW_PERSPECTIVE, On3dvViewPerspective)
	ON_COMMAND(ED_ID_3DV_VIEW_Z_DIREC, On3dvViewZDirec)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_AXIS, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_BE_DIREC, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_BOUNDARY, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_E_DIREC, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_LIGHTING, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_N_DIREC, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_PERSPECTIVE, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_Z_DIREC, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_LOCK_DEPTH, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_LOCK_EAST, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_LOCK_NORTH, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_LOCK_RESET, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_ZOOM_RESET, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_ZOOM_WINDOW, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_ZOOMIN, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_ZOOMOUT, OnUpdate3DViewButtons)
	ON_COMMAND(ED_ID_3DV_COPYPICTURE, On3dvCopypicture)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_COPYPICTURE, OnUpdate3DViewButtons)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CWellEditFrame::CWellEditFrame()
{

}

CWellEditFrame::~CWellEditFrame()
{
}

bool CWellEditFrame::IsDirty()
{
	return m_Dirty;
}

void CWellEditFrame::Init(COpenGLView* pView,CWellEditDlg* pDlg)
{
	m_pDlg=pDlg;
	m_pView=pView;
	m_pDlg->SetAzimuth(true);
	m_pDlg->SetType(TMD_INC);
	m_Dirty = false;
	
	CComboBox* pCombo = (CComboBox*)m_pDlg->m_SpreadDlgbar.GetDlgItem(ID_COMBO_SPREAD_TYPE);
	pCombo->SetCurSel(1);

	CButton* pAzi = (CButton*)m_pDlg->m_SpreadDlgbar.GetDlgItem(ID_CHECK_SPREAD_AZIMUTH);
	pAzi->SetCheck(TRUE);


}

void CWellEditFrame::OnButtonSpreadOK() 
{

	m_pDlg->OnOK();
}

void CWellEditFrame::OnButtonSpreadCancel() 
{
	m_pDlg->OnCancel();
}



/////////////////////////////////////////////////////////////////////////////
// CWellEditFrame message handlers
COpenGLScene* CWellEditFrame::CurrentScene() 
{
	assert(m_pView->ChildSize() < 2);
	if(m_pView->ChildSize() == 0)
		return 0;
	return (COpenGLScene*)&m_pView->Child(0);
}

void CWellEditFrame::On3dvLockDepth() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Zaxis);
		CurrentScene()->UpdateFrame();
	}
}


void CWellEditFrame::OnUpdate3DViewButtons(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CurrentScene()!=0);

	if(!CurrentScene())
		return;

	if(pCmdUI->m_nID==ED_ID_3DV_VIEW_AXIS)
		pCmdUI->SetCheck(CurrentScene()->DisplayAxis());
	else if(pCmdUI->m_nID==ED_ID_3DV_VIEW_LIGHTING)
		pCmdUI->SetCheck(CurrentScene()->LightingEnabled());
	else if(pCmdUI->m_nID==ED_ID_3DV_VIEW_PERSPECTIVE)
		pCmdUI->SetCheck(CurrentScene()->DisplayPerspective());
	else if(pCmdUI->m_nID==ED_ID_3DV_VIEW_BOUNDARY)
		pCmdUI->SetCheck(CurrentScene()->DisplayBoundingBox());
	else if(pCmdUI->m_nID==ED_ID_3DV_ZOOM_WINDOW)
	{
		CZoomWindowDragger * ml =  dynamic_cast<CZoomWindowDragger*>(m_pView->MouseListener());
		pCmdUI->SetCheck(ml!=0);
	}

}




void CWellEditFrame::On3dvLockEast() 
{

	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Yaxis);
		CurrentScene()->UpdateFrame();

	}
	
}

void CWellEditFrame::On3dvLockNorth() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Xaxis);
		CurrentScene()->UpdateFrame();
	}
	
}

void CWellEditFrame::On3dvLockReset() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::NullVector);
		CurrentScene()->UpdateFrame();
	}
	
}


void CWellEditFrame::On3dvZoomReset() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetScale();
		CurrentScene()->UpdateFrame();
	}

}

void CWellEditFrame::On3dvZoomWindow() 
{
  // TODO TO GUI due to CZoomWindowDragger->CRectangleDragger->CDragger->
  // CMouseListener

	CZoomWindowDragger * ml =  dynamic_cast<CZoomWindowDragger*>(m_pView->MouseListener());
	if(!ml)
	{
		CZoomWindowDragger* p = new CZoomWindowDragger(*CurrentScene());
	}

}

void CWellEditFrame::On3dvZoomin() 
{
	if(CurrentScene())
	{
		CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 1.2);
		CurrentScene()->UpdateFrame();
	}
	
}

void CWellEditFrame::On3dvZoomout() 
{
	if(CurrentScene())
	{
		CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 0.8);
		CurrentScene()->UpdateFrame();
	}
}

void CWellEditFrame::On3dvCopypicture() 
{
	if(CurrentScene())
	{
		CurrentScene()->SceneToClipboard();
		CurrentScene()->UpdateFrame();
	}
}

void CWellEditFrame::On3dvViewAxis() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayAxis(!CurrentScene()->DisplayAxis());
		CurrentScene()->UpdateFrame();
	}
	
}

void CWellEditFrame::On3dvViewBeDirec() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetRotate();
		CurrentScene()->ResetScale();
		CurrentScene()->ViewVector(geo::CVector(-1.0, -1.0, -0.5));
		CurrentScene()->UpVector(geo::CVector(0.2, 0.2, -1.0));
		CurrentScene()->UpdateFrame();
	}
}

void CWellEditFrame::On3dvViewBoundary() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayBoundingBox(!CurrentScene()->DisplayBoundingBox());
		CurrentScene()->UpdateFrame();
	}
	
}

void CWellEditFrame::On3dvViewEDirec() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetRotate();
		CurrentScene()->ResetScale();
		CurrentScene()->ViewVector(geo::CVector::Yaxis*-1.0);
		CurrentScene()->UpVector(geo::CVector(0, 0, -1.0));
		CurrentScene()->UpdateFrame();
	}

	
}

void CWellEditFrame::On3dvViewLighting() 
{
	if(CurrentScene())
	{
		CurrentScene()->EnableLighting(!CurrentScene()->LightingEnabled());
		CurrentScene()->UpdateFrame();
	}
}

void CWellEditFrame::On3dvViewNDirec() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetRotate();
		CurrentScene()->ResetScale();
		CurrentScene()->ViewVector(geo::CVector::Xaxis*-1.0);
		CurrentScene()->UpVector(geo::CVector(0, 0, -1.0));
		CurrentScene()->UpdateFrame();
	}

	
}

void CWellEditFrame::On3dvViewPerspective() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayPerspective(!CurrentScene()->DisplayPerspective());
		CurrentScene()->UpdateFrame();
	}
	
}

void CWellEditFrame::On3dvViewZDirec() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetRotate();
		CurrentScene()->ResetScale();
		CurrentScene()->ViewVector(geo::CVector(0,0,-1));
		CurrentScene()->UpVector(geo::CVector(1.0, 0.0, 0));
		CurrentScene()->UpdateFrame();
	}

	
}

void CWellEditFrame::OnCheckSpreadAzimuth() 
{
	m_Dirty = true;

	CButton* pAzi = (CButton*)m_pDlg->m_SpreadDlgbar.GetDlgItem(ID_CHECK_SPREAD_AZIMUTH);

	if(pAzi->GetCheck())
		m_pDlg->SetAzimuth(true);
	else
		m_pDlg->SetAzimuth(false);
}

void CWellEditFrame::OnCloseupComboSpreadType() 
{
	m_Dirty = true;

	CComboBox* pCombo = (CComboBox*)m_pDlg->m_SpreadDlgbar.GetDlgItem(ID_COMBO_SPREAD_TYPE);

	if(pCombo->GetCurSel()==0)
		m_pDlg->SetType(TMD_TVD);
	else if(pCombo->GetCurSel()==1)
		m_pDlg->SetType(TMD_INC);
	else
		assert(false);
}

