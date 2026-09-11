// WellEditFrame.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "CutSurfaceFrame.h"
#include "OpenGLView.h"
#include "OpenGLScene.h"
#include "zoomwindowdragger.h"
#include "GeoObjectHandlerBase.h"
#include "EditForm.h"
#include "Point.h"
#include "Box.h"
#include "CutSurfaceDlg.h"
#include "LabelPoint.h"
#include "TriaIntersecBox.h"
#include "Vector.h"
#include "NamedDrawDef.h"
#include "CutsurfaceTreeView.h"
#include "GetSideName.h"
#include "GlobalMessage.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif


#define HDNAME_BOX			"Box"
#define HDNAME_ORG_SURFACE	"Orginal surfaces"
#define HDNAME_CUT_SURFACE	"Cut surfaces"
#define HDNAME_SIDE_SURFACE	"Side surfaces"
#define HDNAME_HELPER_GEO	"Helper geometry"


/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceFrame

IMPLEMENT_DYNCREATE(CCutSurfaceFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CCutSurfaceFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CCutSurfaceFrame)
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
	ON_COMMAND(ED_ID_3DV_VIEW_N_DIREC, On3dvViewNDirec)
	ON_COMMAND(ED_ID_3DV_VIEW_PERSPECTIVE, On3dvViewPerspective)
	ON_COMMAND(ED_ID_3DV_VIEW_Z_DIREC, On3dvViewZDirec)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_AXIS, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_BE_DIREC, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_BOUNDARY, OnUpdate3DViewButtons)
	ON_UPDATE_COMMAND_UI(ED_ID_3DV_VIEW_E_DIREC, OnUpdate3DViewButtons)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




CCutSurfaceFrame::CCutSurfaceFrame()
: m_bShowMinMaxPoints(false)
{
	CreateHandler(HDNAME_BOX,true);
	CreateHandler(HDNAME_ORG_SURFACE,false); //do not autodelete org. surfaces
	CreateHandler(HDNAME_CUT_SURFACE,false);
	CreateHandler(HDNAME_SIDE_SURFACE,false);
	CreateHandler(HDNAME_HELPER_GEO,true);

}

CCutSurfaceFrame::~CCutSurfaceFrame()
{
	std::map<CString,CGeoObjectHandlerBase*>::iterator it = m_handlermap.begin();
	while(it != m_handlermap.end())
	{
		CGeoObjectHandlerBase* gh=it->second;
		delete gh;
		it++;
	}
}




void CCutSurfaceFrame::Init(
							CCutsurfaceTreeView* treeview,	
							COpenGLView* view,
							CCutSurfaceDlg* dlg,
							CDoubleQuantity::UNIT unit)
{
	m_pTreeView = treeview;
	m_unit=unit;
	m_pView=view;
	m_pDlg=dlg;
	m_pTreeView->SetFrame(this);
	
	int i;
	for(i=0; i< m_pDlg->m_OrgSurfaces->size();i++ )
	{

		STNamedSurface& ns = m_pDlg->m_OrgSurfaces->at(i);
		const geo::ISurface* surf = ns.Surface;
		NamedDrawDef* dd = new NamedDrawDef(ns.Name.c_str(), QColor(100,100,100).rgb());
		dd->PolyFillFront(FALSE);
		dd->PolyFillBack(FALSE);
		dd->LineWidth(1);
		CGeoObjectHandlerBase* sh = GetHandler(HDNAME_ORG_SURFACE);

		CDisplayPair* pair  = sh->CreateNewPair( dd , NULL );
		pair->Array().PushBack((geo::IObject&)*surf);
		CurrentScene()->insert(pair->Array(),pair->DrawDef());
	}
	CurrentScene()->UpdateFrame();
}


void CCutSurfaceFrame::OnBBChanged(const geo::CBox* pBox)
{

	CGeoObjectHandlerBase* bh = GetHandler(HDNAME_BOX);
	RemoveHandlerFromScene(bh);
	bh->RemoveAllObjects();

	if(pBox)
	{
		BuildBox(pBox);
		ShowHandlerInScene(bh);
		
	}

	UpdateTree();
	CurrentScene()->UpdateFrame();
}

void CCutSurfaceFrame::OnNodesNrChanged(const geo::CBox* pBox, int nodesX,int nodesY,int nodesZ)
{
	CGeoObjectHandlerBase* bh = GetHandler(HDNAME_BOX);
	RemoveHandlerFromScene(bh);
	bh->RemoveAllObjects();

	CTreeCtrl& tc  = m_pTreeView->GetTreeCtrl();
	tc.DeleteAllItems();

	if(pBox)
		BuildBox(pBox);

	if(pBox)
		BuildNodes(pBox,nodesX,nodesY,nodesZ);
	
	if(pBox)
		ShowHandlerInScene(bh);

	UpdateTree();
	CurrentScene()->UpdateFrame();
}

static void InsertSubViewItem(HTREEITEM parent , CCutsurfaceTreeView* tree,const CString& name, CDisplayPair* pair)
{
	CTreeCtrl& tc  = tree->GetTreeCtrl();
	HTREEITEM subitem = tc.InsertItem(name,0,0,parent);
	tc.SetItemData(subitem,(DWORD_PTR)pair);
	tc.EnsureVisible(subitem);
	tc.SetCheck(subitem,TRUE);
}


void CCutSurfaceFrame::UpdateTree()
{

	int i;
	HTREEITEM item;
	CTreeCtrl& tc  = m_pTreeView->GetTreeCtrl();
	tc.DeleteAllItems();

	std::map<CString,CGeoObjectHandlerBase*>::iterator it = m_handlermap.begin();
	while(it != m_handlermap.end())
	{
		CGeoObjectHandlerBase* gh=it->second;
		int Size = gh->Size();
		if(Size)
		{
			item = tc.InsertItem( it->first );
			tc.SetCheck(item,TRUE);
			
			for(i=0;i<Size;i++)
			{
				CDisplayPair* pair = gh->GetPair(i);
				NamedDrawDef* pn = dynamic_cast<NamedDrawDef*>(&pair->DrawDef());
				if(pn)
				{
					InsertSubViewItem(item,m_pTreeView,pn->Name().toStdString().c_str(),pair);
				}
				else
				{
					InsertSubViewItem(item,m_pTreeView,"DrawDef",pair);
				}
			}
		}
		it++;
	}

}


void CCutSurfaceFrame::BuildNodes(const geo::CBox* pBox, int nodesX,int nodesY,int nodesZ)
{

	assert(pBox);

	int i;

	CGeoObjectHandlerBase* bh = GetHandler(HDNAME_BOX);
	TRib ribs[3];
	ribs[0] = GetRib(SIDE_ID_FRONT,SIDE_ID_BOTTOM);
	ribs[1] = GetRib(SIDE_ID_LEFT,SIDE_ID_BOTTOM);
	ribs[2] = GetRib(SIDE_ID_FRONT,SIDE_ID_LEFT);

	int nr[3];
	nr[0]=nodesX;nr[1]=nodesY;nr[2]=nodesZ;

	for(i=0;i<3;i++)
	{
		std::vector<const geo::IPoint*> vec;
		CTriaIntersecBox::GetPointsOnRib(*pBox, vec,  ribs[i] ,nr[i]);

		NamedDrawDef* dd = new NamedDrawDef("Nodes on rib", QColor(0,0,255).rgb());
		dd->PolyFillFront(FALSE);
		dd->PolyFillBack(FALSE);
		dd->PointSize(3);
		CDisplayPair* dp = bh->CreateNewPair( dd , NULL );
		for(size_t j=0;j<vec.size();j++)
		{
			dp->AddObject((geo::IObject&)*vec[j]);
		}
	}

}


void CCutSurfaceFrame::RemoveHandlerFromScene(CGeoObjectHandlerBase* handler)
{
	if(handler->Size())
	{
		for(int i=0;i<handler->Size();i++)
			CurrentScene()->erase(handler->GetPair(i)->Array(),false);
		
	}
	UpdateScene();
}


void CCutSurfaceFrame::UpdateScene()
{
	CurrentScene()->InvalidateAllDisplayLists(); 
	CurrentScene()->UpdateFrame();
}

void CCutSurfaceFrame::ShowHandlerInScene(CGeoObjectHandlerBase* handler)
{
	if(handler->Size())
	{
		for(int i=0;i<handler->Size();i++)
		{
			CDisplayPair* dp = handler->GetPair(i);
			CurrentScene()->insert(dp->Array(),dp->DrawDef());
		}
	}
	UpdateScene();
}



void CCutSurfaceFrame::BuildBox(const geo::CBox* pBox)
{


	assert(pBox);

	CGeoObjectHandlerBase* bh = GetHandler(HDNAME_BOX);

	NamedDrawDef* dd = new NamedDrawDef("Box", QColor(250,100,100).rgb());
	dd->PolyFillFront(FALSE);
	dd->PolyFillBack(FALSE);
	dd->LineWidth(2);
	CDisplayPair* box = bh->CreateNewPair( dd , NULL );
	box->AddObject(const_cast<geo::CBox&>(*pBox));

	if(m_bShowMinMaxPoints)
	{
		geo::CPoint* pMax = new geo::CPoint(pBox->Max());
		geo::CPoint* pMin = new geo::CPoint(pBox->Min());

		CString frmt;

		double x = CLengthQuantity(pMax->X()).Value(m_unit);
		double y = CLengthQuantity(pMax->Y()).Value(m_unit);
		double z = CLengthQuantity(pMax->Z()).Value(m_unit);
		frmt.Format("%.2f , %.2f , %.2f" ,x,y,z);
		CLabelPoint* lb1 = new CLabelPoint(*pMax,QString(frmt)); //,QFont(),CLabelPoint::LA_CENTER_RIGHT);

		x = CLengthQuantity(pMin->X()).Value(m_unit);
		y = CLengthQuantity(pMin->Y()).Value(m_unit);
		z = CLengthQuantity(pMin->Z()).Value(m_unit);
		frmt.Format("%.2f , %.2f , %.2f" ,x,y,z);
		CLabelPoint* lb2 = new CLabelPoint(*pMin,QString(frmt)); //,QFont(),CLabelPoint::LA_CENTER_RIGHT);
		
		CDisplayPair* labels = bh->CreateNewPair(new NamedDrawDef("box labels"),0);
		labels->DrawDef().SetFixedColor(QColor(0,0,255).rgb());
		labels->DrawDef().TextColor(QColor(0,0,255).rgb());
		labels->DrawDef().PointSize(6);
		labels->AddObject(*lb1);
		labels->AddObject(*lb2);
		labels->AddObject(*new geo::CPoint(pBox->Max()));
		labels->AddObject(*new geo::CPoint(pBox->Min()));
	}

}

void CCutSurfaceFrame::GetSurfaceMap(const char* name , TNamedSurfMap& surfmap)
{
	CGeoObjectHandlerBase* ch = GetHandler(name);
	for(int i=0 ;i<ch->Size();i++)
	{
		CDisplayPair* dp = ch->GetPair(i);
		NamedDrawDef* dd = (NamedDrawDef*)&dp->DrawDef();
		const geo::ISurface* surf = (const geo::ISurface*)(&(dp->Array().Object(0)));
		surfmap.insert(TNamedSurfMap::value_type(surf,dd->Name().toStdString()));
	}
}

void CCutSurfaceFrame::GetSideSurfaceMap( TNamedSurfMap& surfmap)
{
	GetSurfaceMap(HDNAME_SIDE_SURFACE,surfmap); 
}

void CCutSurfaceFrame::GetCutSurfaceMap( TNamedSurfMap& surfmap)
{
	GetSurfaceMap(HDNAME_CUT_SURFACE,surfmap); 
}

void CCutSurfaceFrame::OnCutSurface(const geo::CBox* pBox, int nodesX, int nodesY, int nodesZ, bool bShowMinMaxPoints, const CString& basename)
{
	m_bShowMinMaxPoints = bShowMinMaxPoints;

	CWaitCursor wait;

	CGeoObjectHandlerBase* bh = GetHandler(HDNAME_BOX);
	RemoveHandlerFromScene(bh);
	bh->RemoveAllObjects();


	CGeoObjectHandlerBase* gh = GetHandler(HDNAME_HELPER_GEO);
	RemoveHandlerFromScene(gh);
	gh->RemoveAllObjects();


	if(pBox)
	{
		BuildBox(pBox);
		BuildNodes(pBox,nodesX,nodesY,nodesZ);
	}

	CGeoObjectHandlerBase* ch = GetHandler(HDNAME_CUT_SURFACE);
	RemoveHandlerFromScene(ch);
	ch->RemoveAllObjects();

	CGeoObjectHandlerBase* sh = GetHandler(HDNAME_SIDE_SURFACE);
	RemoveHandlerFromScene(sh);
	sh->RemoveAllObjects();


	if(!pBox)
	{
		ShowHandlerInScene(GetHandler(HDNAME_ORG_SURFACE));
		UpdateTree();
		return;
	}

	RemoveHandlerFromScene(GetHandler(HDNAME_ORG_SURFACE));

	CTriaIntersecBox*	Intersec = m_pDlg->CreateNewIntersec(*pBox);

	Intersec->AllSides();


	const TRibs& ribs = GetRibs();
	assert(ribs.size() ==12);

	for(size_t i=0;i<ribs.size();i++)
	{
		int nr;
		TRib  rib = ribs[i];
		geo::CLine l = GetLine(*pBox, rib);
		if(l.Dir().IsParallel(geo::CVector(pBox->Point(0), pBox->Point(1))))
			nr=nodesX;
		else if(l.Dir().IsParallel(geo::CVector(pBox->Point(0), pBox->Point(3))))
			nr=nodesY;
		else if(l.Dir().IsParallel(geo::CVector(pBox->Point(0), pBox->Point(4))))
			nr=nodesZ;
		else
			assert(FALSE);

		Intersec->SetExtraPointsOnRib(rib,nr);
	}


	for(size_t i=0; i< m_pDlg->m_OrgSurfaces->size();i++ )
	{

		STNamedSurface& ns = m_pDlg->m_OrgSurfaces->at(i);
		const geo::ISurface* surf = ns.Surface;
		Intersec->AddSurfaceToCut(ns.Name, *surf);
	}


	Intersec->GenerateSideSurfaces();

	geo::CPtrArray<geo::IPoint>* points= new geo::CPtrArray<geo::IPoint>;
	CTriaIntersecBox::TEdgeVector edges;

	bool isolated = Intersec->LookForIsolatedEdges(*points,edges);
#if 0
	std::map<std::string,geo::CPtrArray<geo::IObject>* >& ta = Intersec->test_arrays;


	std::map<std::string,geo::CPtrArray<geo::IObject>* >::iterator it = ta.begin();


	while(it != ta.end())
	{

		CString name=CString(it->first.c_str());
		geo::CPtrArray<geo::IObject>* ptrArr = it->second;

		CDisplayPair* dp = gh->CreateNewPair(new NamedDrawDef(name),0);
		dp->DrawDef().SetFixedColor(QColor(255,0,0).rgb());
		dp->DrawDef().PointSize(10);
		dp->DrawDef().LineWidth(3);
		dp->AddObject(*ptrArr);
		it++;
	}
#endif


	if(isolated)
	{

		assert(points->Size()!=0);
		_m()->msg(IDP_ISOLATED_EDGES, MB_OK|MB_ICONINFORMATION);

		CDisplayPair* dp = gh->CreateNewPair(new NamedDrawDef("Isolated edges"),0);
		dp->DrawDef().SetFixedColor(QColor(255,0,0).rgb());
		dp->DrawDef().PointSize(10);
		dp->DrawDef().LineWidth(3);

		

		for(size_t i=0;i<edges.size();i++)
		{
			const geo::IPoint* p1 = (const geo::IPoint*)edges[i].first;
			const geo::IPoint* p2 = (const geo::IPoint*)edges[i].second;
			dp->AddObject(* new geo::CLine(*p1,*p2));
		}
		points->ClearAndDelete();
	}
	else
	{
		assert(points->Size()==0);
		delete points;
	}
	

	int count = Intersec->CutSurfacesCount();


	for(int i=0 ;i <count;i++)
	{
		

		CString Name;
		const STNamedSurface& ns =  Intersec->GetCutSurfaces(i);
		CDisplayPair* dp = ch->CreateNewPair(new NamedDrawDef(QString(basename+CString(ns.Name.c_str()))),0);
		dp->DrawDef().PolyFillFront(FALSE);
		dp->DrawDef().PolyFillBack(FALSE);
		dp->DrawDef().LineWidth(2);
		dp->AddObject((geo::IObject&)*ns.Surface);


	}

	const std::vector<SIDE_ID> sides = Intersec->GetSides();
	


	for(size_t i=0;i<sides.size();i++)
	{
		CString Name= basename + GetSideName(sides[i]).c_str();
		geo::CTriSurface* surf = Intersec->GetSideSurface(sides[i]);
		CDisplayPair* dp = sh->CreateNewPair(new NamedDrawDef(QString(Name)),0);
		dp->DrawDef().SetFixedColor(QColor(100,100,100).rgb());
		dp->DrawDef().PolyFillFront(FALSE);
		dp->DrawDef().PolyFillBack(FALSE);
		dp->DrawDef().LineWidth(1);
		dp->AddObject(*surf);

	}

	ShowHandlerInScene(ch);
	ShowHandlerInScene(sh);
	ShowHandlerInScene(bh);
	ShowHandlerInScene(gh);

	UpdateTree();
}




CGeoObjectHandlerBase* CCutSurfaceFrame::GetHandler(const char* name)
{
	std::map<CString,CGeoObjectHandlerBase*>::iterator it = m_handlermap.find(name);
	assert(it != m_handlermap.end());
	return it->second;
}


CGeoObjectHandlerBase* CCutSurfaceFrame::CreateHandler(const char* name,bool autodelete)
{
	std::map<CString,CGeoObjectHandlerBase*>::iterator it = m_handlermap.find(name);
	assert(it == m_handlermap.end());
	CGeoObjectHandlerBase* ret = new CGeoObjectHandlerBase(autodelete);
	m_handlermap[CString(name)]=ret;
	return ret;
}




/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceFrame message handlers
COpenGLScene* CCutSurfaceFrame::CurrentScene() 
{
	assert(m_pView->ChildSize() ==1);
	return (COpenGLScene*)&m_pView->Child(0);
}

void CCutSurfaceFrame::On3dvLockDepth() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Zaxis);
		CurrentScene()->UpdateFrame();
	}
}


void CCutSurfaceFrame::OnUpdate3DViewButtons(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CurrentScene()!=0);

	if(!CurrentScene())
		return;

	if(pCmdUI->m_nID==ED_ID_3DV_VIEW_AXIS)
		pCmdUI->SetCheck(CurrentScene()->DisplayAxis());
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




void CCutSurfaceFrame::On3dvLockEast() 
{

	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Yaxis);
		CurrentScene()->UpdateFrame();

	}
	
}

void CCutSurfaceFrame::On3dvLockNorth() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::Xaxis);
		CurrentScene()->UpdateFrame();
	}
	
}

void CCutSurfaceFrame::On3dvLockReset() 
{
	if(CurrentScene())
	{
		CurrentScene()->RotateLockVector(geo::CVector::NullVector);
		CurrentScene()->UpdateFrame();
	}
	
}


void CCutSurfaceFrame::On3dvZoomReset() 
{
	if(CurrentScene())
	{
		CurrentScene()->ResetPan();
		CurrentScene()->ResetScale();
		CurrentScene()->UpdateFrame();
	}

}

void CCutSurfaceFrame::On3dvZoomWindow() 
{
  // TODO TO GUI due to CZoomWindowDragger->CRectangleDragger->CDragger->
  // CMouseListener

	CZoomWindowDragger * ml =  dynamic_cast<CZoomWindowDragger*>(m_pView->MouseListener());
	if(!ml)
	{
		CZoomWindowDragger* p = new CZoomWindowDragger(*CurrentScene());
	}

}

void CCutSurfaceFrame::On3dvZoomin() 
{
	if(CurrentScene())
	{
		CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 1.2);
		CurrentScene()->UpdateFrame();
	}
	
}

void CCutSurfaceFrame::On3dvZoomout() 
{
	if(CurrentScene())
	{
		CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 0.8);
		CurrentScene()->UpdateFrame();
	}
}

void CCutSurfaceFrame::On3dvCopypicture() 
{
	if(CurrentScene())
	{
		CurrentScene()->SceneToClipboard();
		CurrentScene()->UpdateFrame();
	}
}

void CCutSurfaceFrame::On3dvViewAxis() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayAxis(!CurrentScene()->DisplayAxis());
		CurrentScene()->UpdateFrame();
	}
	
}

void CCutSurfaceFrame::On3dvViewBeDirec() 
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

void CCutSurfaceFrame::On3dvViewBoundary() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayBoundingBox(!CurrentScene()->DisplayBoundingBox());
		CurrentScene()->UpdateFrame();
	}
	
}

void CCutSurfaceFrame::On3dvViewEDirec() 
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

void CCutSurfaceFrame::On3dvViewLighting() 
{
	if(CurrentScene())
	{
		CurrentScene()->EnableLighting(!CurrentScene()->LightingEnabled());
		CurrentScene()->UpdateFrame();
	}
}

void CCutSurfaceFrame::On3dvViewNDirec() 
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

void CCutSurfaceFrame::On3dvViewPerspective() 
{
	if(CurrentScene())
	{
		CurrentScene()->DisplayPerspective(!CurrentScene()->DisplayPerspective());
		CurrentScene()->UpdateFrame();
	}
	
}

void CCutSurfaceFrame::On3dvViewZDirec() 
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

