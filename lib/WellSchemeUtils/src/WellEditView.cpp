// WellEditView.cpp : implementation file
//

#include "stdafx.h"
#include "wellpathbase.h"
#include "WellEditView.h"
#include "WaterLevelSymbol.h"
#include "GroundLevelSymbol.h"
#include "DrillingTowerSymbol.h"
#include "WellDrawSpecBase.h"
#include "WellPointDrawSpec.h"
#include "WellSectionDrawSpec.h"
#include "WellSceneInterMed.h"
#include "OpenGLScene.h"
#include "openglaxis.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellEditView

IMPLEMENT_DYNCREATE(CWellEditView, COpenGLView)

CWellEditView::CWellEditView()
{
}

CWellEditView::~CWellEditView()
{
}


BEGIN_MESSAGE_MAP(CWellEditView, COpenGLView)
	//{{AFX_MSG_MAP(CWellEditView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_WM_SIZE()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONUP()
		ON_WM_LBUTTONDBLCLK()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_KEYDOWN()
		ON_WM_KEYUP()
		ON_WM_SETCURSOR()
		ON_WM_CREATE()
		ON_WM_DESTROY()
		ON_WM_ERASEBKGND()
		ON_WM_PALETTECHANGED()
		ON_WM_QUERYNEWPALETTE()
		ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellEditView drawing

/////////////////////////////////////////////////////////////////////////////
// CWellEditView diagnostics

#ifdef _DEBUG
void CWellEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWellEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


void CWellEditView::OnSize(unsigned int nType, int cx, int cy) 
{
	COpenGLView::OnSize( nType,  cx,  cy) ;
}


int CWellEditView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_pWellSceneInterMed=0;
	int ret = COpenGLView::OnCreate(lpCreateStruct);
	m_pScene = new COpenGLScene();
	m_pScene->Axis().AxisText(geo::CVector::Xaxis, "Northing");
	m_pScene->Axis().AxisText(geo::CVector::Yaxis, "Easting");
	m_pScene->Axis().AxisText(geo::CVector::Zaxis, "Depth");
	m_pScene->ViewVector(geo::CVector::Xaxis*-1.0);
	m_pScene->UpVector(geo::CVector(0, 0, -1.0));
	m_pScene->DisplayPerspective(FALSE);
	m_pScene->EnableLighting(FALSE);

	m_pScene->ReParent(this);

	return ret;
}


void CWellEditView::UpdateWellView()
{
	assert(m_pWellpath);

	QU::UNIT us = m_Us;

	if(m_pWellSceneInterMed)
	{
		m_pWellSceneInterMed->DetachScene();
		delete m_pWellSceneInterMed;
		m_pWellSceneInterMed=0;
	}
	
	if(m_pWellpath->Defined())
	{
		m_pWellSceneInterMed = new CWellSceneInterMed(*m_pWellpath);
		m_pWellSceneInterMed->AttachScene(m_pScene);
		well::IWellSection* section = new well::CWellSection(*m_pWellpath,m_pWellpath->DefPointList().First()->TMD().Value(),m_pWellpath->DefPointList().Last()->TMD().Value());

		CWellSectionDrawSpec* wd = &m_pWellSceneInterMed->AddSection(*section);
		wd->SetLineWidth(5);
		wd->SetColor(qRgb(0,0,255));
		wd->SetVisible(true);

		wd = &m_pWellSceneInterMed->AddSection(*section,CWellSectionDrawSpec::ARROW);
		wd->SetLineWidth(1);
		wd->SetColor(qRgb(255,255,255));
		QString frmt;
		frmt.setNum(section->Bottom().TMD().Value(us)-section->Top().TMD().Value(us));
		QString txt;
		txt = QString("Wellpath L= ") + frmt + " [" + QString(section->Bottom().TMD().UnitName(us).c_str()) + "]";
		wd->SetDescription(txt);
		wd->SetVisible(true);

//		m_pWellSceneInterMed->DrillingTowerSymbol().SetDescription("Reference point level");
//		m_pWellSceneInterMed->DrillingTowerSymbol().SetVisible(TRUE);
//
//		if(m_pWellpath->GetLocation()== well::CWellPathBase::ONSHORE)
//		{
//			m_pWellSceneInterMed->GroundLevelSymbol().SetDescription("Mean ground level");
//			m_pWellSceneInterMed->GroundLevelSymbol().SetVisible(TRUE);
//		}
//		else
//		{
//			m_pWellSceneInterMed->WaterLevelSymbol().SetDescription("Mean sea level");
//			m_pWellSceneInterMed->WaterLevelSymbol().SetVisible(TRUE);
//
//
//			m_pWellSceneInterMed->GroundLevelSymbol().SetDescription("Seabed level");
//			m_pWellSceneInterMed->GroundLevelSymbol().SetVisible(TRUE);
//
//		}
		m_pScene->InvalidateAllDisplayLists();
	}
}

void CWellEditView::Init(well::CWellPathBase& wellpath, QU::UNIT us)
{
	m_Us=us;
	m_pWellpath=&wellpath;
	bool ret = connect(m_pWellpath,SIGNAL(onGeometryChanged()),this,SLOT(geometryChanged()));
	UpdateWellView();
}

void CWellEditView::geometryChanged()
{
	UpdateWellView(); 
}

/////////////////////////////////////////////////////////////////////////////
// CWellEditView message handlers
