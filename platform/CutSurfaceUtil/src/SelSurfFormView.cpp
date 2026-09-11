// SelSurfFormView.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "SelSurfFormView.h"
#include "DisplayPair.h"
#include "NamedDrawDef.h"
#include "GeoObjectHandlerBase.h"
#include "ISurface.h"
#include "SelSurfDlg.h"
#include "SelSurfGLView.h"
#include "OpenGLScene.h"





/////////////////////////////////////////////////////////////////////////////
// SelSurfFormView

IMPLEMENT_DYNCREATE(SelSurfFormView, CFormView)

SelSurfFormView::SelSurfFormView()
  : CFormView(IDD_FORMVIEW_SELECT_SURF)
{
  
  m_geo_handler = new CGeoObjectHandlerBase(false);

  //{{AFX_DATA_INIT(SelSurfFormView)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

SelSurfFormView::~SelSurfFormView()
{
  delete m_geo_handler; 
}


void SelSurfFormView::Init(SelSurfDlg* dlg,SelSurfGLView* From,SelSurfGLView* To)
{
  m_dlg=dlg;
  m_ViewFrom=From;
  m_ViewTo=To;

  int i;
  CNamedSurfaces* mp = dlg->m_surfaces;
  for(i=0; i< mp->size();i++ )
  {

    STNamedSurface& ns = mp->at(i);

    CString name;
    name=ns.Name.c_str();
    const geo::ISurface* surf=ns.Surface;

    NamedDrawDef* dd = new NamedDrawDef(QString(name), QColor(100,100,100).rgb());
    dd->PolyFillFront(FALSE);
    dd->PolyFillBack(FALSE);
    dd->LineWidth(1);
    CDisplayPair* pair  = m_geo_handler->CreateNewPair( dd , NULL );
    pair->Array().PushBack((geo::IObject&)*surf);
    
    int index = m_ListFrom.AddString(ns.Name.c_str());
    m_ListFrom.SetItemDataPtr(index,(void*)pair);

    m_ViewFrom->CurrentScene()->insert(pair->Array(),pair->DrawDef());		
  }

  m_ViewFrom->CurrentScene()->InvalidateAllDisplayLists(); 
  m_ViewFrom->CurrentScene()->UpdateFrame();


}





void SelSurfFormView::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(SelSurfFormView)
  DDX_Control(pDX, IDC_LIST_TO, m_ListTo);
  DDX_Control(pDX, IDC_LIST_FROM, m_ListFrom);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelSurfFormView, CFormView)
  //{{AFX_MSG_MAP(SelSurfFormView)
  ON_BN_CLICKED(IDC_BUTTON_FROM, OnButtonFrom)
  ON_BN_CLICKED(IDC_BUTTON_FROM_ALL, OnButtonFromAll)
  ON_BN_CLICKED(IDC_BUTTON_TO, OnButtonTo)
  ON_BN_CLICKED(IDC_BUTTON_TO_ALL, OnButtonToAll)
  ON_LBN_SELCHANGE(IDC_LIST_FROM, OnSelchangeListFrom)
  ON_LBN_SELCHANGE(IDC_LIST_TO, OnSelchangeListTo)
  ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
  ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelSurfFormView diagnostics

#ifdef _DEBUG
void SelSurfFormView::AssertValid() const
{
  CFormView::AssertValid();
}

void SelSurfFormView::Dump(CDumpContext& dc) const
{
  CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SelSurfFormView message handlers

void SelSurfFormView::OnButtonFrom() 
{
  DoFromTo(m_ViewTo, &m_ListTo,m_ViewFrom,&m_ListFrom,false);
  UpdateSelectionInViews();
}

void SelSurfFormView::OnButtonFromAll() 
{
  DoFromTo(m_ViewTo,&m_ListTo,m_ViewFrom,&m_ListFrom,true);
  UpdateSelectionInViews();
}

void SelSurfFormView::OnButtonTo() 
{
  DoFromTo(m_ViewFrom , &m_ListFrom, m_ViewTo, &m_ListTo,false);
  UpdateSelectionInViews();
  
}

void SelSurfFormView::OnButtonToAll() 
{
  DoFromTo(m_ViewFrom,&m_ListFrom,m_ViewTo,&m_ListTo,true);
  UpdateSelectionInViews();
}


void SelSurfFormView::OnSelchangeListTo() 
{
  HandleSelect(m_ViewTo,&m_ListTo);
  m_ViewTo->CurrentScene()->InvalidateAllDisplayLists(); 
  m_ViewTo->CurrentScene()->UpdateFrame();
}

void SelSurfFormView::OnSelchangeListFrom() 
{
  HandleSelect(m_ViewFrom,&m_ListFrom);
  m_ViewFrom->CurrentScene()->InvalidateAllDisplayLists(); 
  m_ViewFrom->CurrentScene()->UpdateFrame();
}

void SelSurfFormView::HandleSelect(SelSurfGLView* View,CListBox* List)
{
  int count = List->GetCount();
  
  for(int i=0;i<count;i++)
  {
    BOOL sel= List->GetSel(i);
    CDisplayPair* pair = (CDisplayPair*)List->GetItemDataPtr(i);
    if(sel)
    {
      pair->DrawDef().SetFixedColor(QColor(255,0,0).rgb());
    }
    else
    {
      pair->DrawDef().SetFixedColor(QColor(100,100,100).rgb());
    }
  }

}

void SelSurfFormView::GetMap(CListBox* List,CNamedSurfaces& surfaces) 
{

  int count = List->GetCount();
  
  for(int i=0;i<count;i++)
  {
    CDisplayPair* pair = (CDisplayPair*)List->GetItemDataPtr(i);
    NamedDrawDef* dd = (NamedDrawDef*)&pair->DrawDef();
    assert(pair->Array().Size() == 1);
    const geo::ISurface* surf = (const geo::ISurface*)(&pair->Array().Object(0));
    STNamedSurface ns(dd->Name().toStdString(),surf);
    surfaces.push_back(ns);
  }

}


void SelSurfFormView::UpdateSelectionInViews()
{
  HandleSelect(m_ViewTo,&m_ListTo);
  HandleSelect(m_ViewFrom,&m_ListFrom);

  m_ViewFrom->CurrentScene()->InvalidateAllDisplayLists(); 
  m_ViewFrom->CurrentScene()->UpdateFrame();

  m_ViewTo->CurrentScene()->InvalidateAllDisplayLists(); 
  m_ViewTo->CurrentScene()->UpdateFrame();

}


void SelSurfFormView::SwitchPairInView(CDisplayPair* pair,	SelSurfGLView* ViewFrom , SelSurfGLView* ViewTo)
{
  ViewFrom->CurrentScene()->erase(pair->Array(),false);
  ViewTo->CurrentScene()->insert(pair->Array(),pair->DrawDef());		
}


void SelSurfFormView::DoFromTo(SelSurfGLView* ViewFrom,CListBox* FromList,SelSurfGLView* ViewTo,CListBox* ToList,bool all)
{

  int items[1024]; // = new int(m_ListFrom.GetSelCount());
  
  CDisplayPair* info1;
  CDisplayPair* info2;
  bool exist;
  int count;
  if(all)
    count = FromList->GetCount();
  else
    count = FromList->GetSelItems(1024,items);
    
  int i;
  for(i=0;i<count;i++)
  {
    exist=false;
    if(all)
      info1 = (CDisplayPair*)FromList->GetItemDataPtr(i);
    else
      info1 = (CDisplayPair*)FromList->GetItemDataPtr(items[i]);

    assert(info1);
    for(int j=0;j<ToList->GetCount();j++)
    {
      info2 = (CDisplayPair*)ToList->GetItemDataPtr(j);
      if(info1==info2)
      {
        exist=true;
        break;
      }
    }
    if(!exist)
    {
      SwitchPairInView(info1,ViewTo,ViewFrom);
      NamedDrawDef* dd = (NamedDrawDef*)&info1->DrawDef();
      int index = ToList->AddString(dd->Name().toStdString().c_str());
      ToList->SetItemDataPtr(index,(CDisplayPair*)info1);
    }
  }
  
  
  count = ToList->GetCount();
  for(i=0;i<count;i++)
  {
    info1 = (CDisplayPair*)ToList->GetItemDataPtr(i);
    assert(info1);
    for(int j=0;j<FromList->GetCount();j++)
    {
      info2 = (CDisplayPair*)FromList->GetItemDataPtr(j);
      assert(info2);
      if(info1==info2)
      {
        SwitchPairInView(info1,ViewFrom,ViewTo);
        FromList->DeleteString(j);
        break;
      }
    }
  }


}



void SelSurfFormView::OnButtonOk() 
{
  m_dlg->OnOK();
  
}


void SelSurfFormView::GetSelecetedSurfaces(CNamedSurfaces& surfaces)
{
  GetMap(&m_ListTo,surfaces);  
}

void SelSurfFormView::GetUnSelecetedSurfaces(CNamedSurfaces& surfaces)
{
   GetMap(&m_ListFrom,surfaces);
}




void SelSurfFormView::OnButtonCancel() 
{
  m_dlg->OnCancel();
  
}

void SelSurfFormView::OnDestroy() 
{
  CFormView::OnDestroy();
}
