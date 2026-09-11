// TestViewExeDoc.cpp : implementation of the CTestViewExeDoc class
//

#include "stdafx.h"
#include "TestViewExe.h"
#include "TestViewExeDoc.h"
#include "WellPointPickDlg.h"
#include "DerivedWellPickDlg.h"
#include "WellPointListDlg.h"
#include "wellpathbase.h"
#include "welllog.h"
#include "welllogfile.h"
#include "rtfile.h"
#include "welllogdlg.h"
#include "xmlbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


COLORREF color_table[]=
{
  RGB(255,0,0),
  RGB(0,255,0),
  RGB(0,0,255),
  RGB(255,255,0),
  RGB(0,255,255)
};
/////////////////////////////////////////////////////////////////////////////
// CTestViewExeDoc


IMPLEMENT_DYNCREATE(CTestViewExeDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestViewExeDoc, CDocument)
  //{{AFX_MSG_MAP(CTestViewExeDoc)
  ON_COMMAND(ID_TESTMENU_DELETESECTION, OnTestmenuDeletesection)
  ON_COMMAND(ID_TESTMENU_ATACHSCENE, OnTestmenuAtachscene)
  ON_COMMAND(ID_TESTMENU_DETACHSCENE, OnTestmenuDetachscene)
  ON_COMMAND(ID_TESTMENU_SHOWDIALOG, OnTestmenuShowdialog)
  ON_COMMAND(ID_TESTMENU_SHOW_LISTDLG, OnTestmenuShowListdlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeDoc construction/destruction

CTestViewExeDoc::CTestViewExeDoc()
{
  InitWellPath();

}

CTestViewExeDoc::~CTestViewExeDoc()
{
  m_pSceneInterMed->RemoveAllPoints();

  delete m_pSceneInterMed;
  delete m_pWellPath;
  delete m_pScene;
  

}

BOOL CTestViewExeDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}



COpenGLScene* CTestViewExeDoc::Scene()
{
  return m_pScene;
}

void CTestViewExeDoc::InitWellPath2()
{

  m_pWellPath = new well::CWellPathBase();
  m_pWellPath->InitFromMWRFile("D:\\users\\tnma\\DCasint\\A9Simple.mwr");
  m_pWellPath->SetHoleName("test wellpath");

  well::CWellSection *section = new well::CWellSection(*m_pWellPath, 4917.31502, 5008.750558);
  well::CWellLog welllog(m_pWellPath, section);
  well::CWellLogFile logfile(&welllog);
  CProgressCallBack prog;

  //logfile.Open("D:\\users\\tnma\\DCasint\\compressibilityVert_edit.txt", prog);
  logfile.Open("D:\\users\\tnma\\DCasint\\compressibilityA9_edit.txt", prog);
  //logfile.Open("D:\\users\\tnma\\DCasint\\test_log_small.txt", prog);
  well::CRTFile ratfile(&welllog);
  //ratfile.Open("D:\\users\\tnma\\DCasint\\radioactiveVert_edit.txt", prog);
  ratfile.Open("D:\\users\\tnma\\DCasint\\radioactiveA9_edit.txt", prog);
  //ratfile.Open("D:\\users\\tnma\\DCasint\\rat_test.txt", prog);


  CWellLogDlg logdlg(&welllog);
  logdlg.DoModal();

/*	m_pWellPath->InitVertical(500,600,50,100,500);
//								double start_northing,
//								 double start_easting,
//								 double start_depth,
//								 double start_tmd,
//								 double end_depth);


  m_pWellPath->DumpToFile("D:\\users\\htg\\GEOMEC3\\test\\Dstabor\\test1\\well_dump2.mwr");
  m_pScene = new COpenGLScene;
  m_pScene->ViewVector(geo::CVector::Xaxis);
  m_pScene->UpVector(-geo::CVector::Zaxis);
  m_pScene->EnableLighting(FALSE);
  m_pScene->DisplayBoundingBox(false);
  m_pSceneInterMed = new CWellSceneInterMed(*m_pWellPath);

  well::CWellPointList::Iterator it = m_pWellPath->DefPointList().GetIterator();
  well::CWellPoint* pPoint = it.current();

  well::CWellPointList* pDStaborList=new well::CWellPointList(*m_pWellPath);
  pDStaborList->SetDescription("DStabor Points");
  m_pWellPath->InsertWellPointList(pDStaborList,LST_DSTABORPOINTS);

  int i=0;
  while(pPoint)
  {
    CWellPointDrawSpec& ds = m_pSceneInterMed->AddPoint( *pPoint );
    QString str,frmt;
    str = "     wp " + frmt.setNum(pPoint->Azimuth().Value());
    str = str + " / "  + frmt.setNum(pPoint->Inclination().Value());

    str = frmt.setNum(pPoint->TMD().Value());

    ds.SetDescription(str);
    ds.SetTextColor(RGB(255,255,255));
    ds.SetColor(RGB(0,255,0));
    ds.SetVisible(true);
      
    pPoint = ++it;
    i++;
  }

  double tmd1 = m_pWellPath->DefPointList().First()->TMD().Value();
  double tmd2 = m_pWellPath->DefPointList().Last()->TMD().Value();

  well::CWellSectionList* pList0 = new well::CWellSectionList(*m_pWellPath);
  pList0->SetDescription("Casing 1");
  m_pWellPath->InsertWellSectionList(pList0,0);
  well::CWellSection* pSection = new well::CWellSection(*m_pWellPath,tmd1,tmd2);
  pList0->AddSection(pSection);


  well::CWellSectionList* pList = new well::CWellSectionList(*m_pWellPath);
  pList->SetDescription("Formations");
  m_pWellPath->InsertWellSectionList(pList,1);
  pSection = new well::CWellSection(*m_pWellPath,tmd1,tmd2);
  pList->AddSection(pSection);

  CWellSectionDrawSpec* sds = &m_pSceneInterMed->AddSection(*pSection,CWellSectionDrawSpec::ARROW);
  QString frm;
  sds->SetLineWidth(1);
  sds->SetPointSize(6);
  sds->SetArrowDistanceLevel(2);
  sds->SetDescription(QString("Total length: ") + frm.setNum(tmd2-tmd1));
  sds->SetVisible(true);

  double delta_tmd = (tmd2-tmd1)/5.0;
  for(int k=0;k<5;k++)
  {
    double tmd_1 = tmd1 + delta_tmd*double(k);
    double tmd_2 = tmd_1 + delta_tmd;

    well::CWellSection* pSection2 = new well::CWellSection(*m_pWellPath,tmd_1,tmd_2);
    pList->AddSection(pSection2);
    CWellSectionDrawSpec* sds2;

    sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::ARROW);
    sds2->SetLineWidth(1);
    sds2->SetColor(RGB(255,255,255));
    QString frm;
    sds2->SetPointSize(6);
    sds2->SetDescription(QString("Section ") + frm.setNum(k));
    sds2->SetVisible(true);

//		sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::SQUARE);
//		sds2->SetLineWidth(1);
//		sds2->SetColor(color_table[k]);
//		sds2->SetVisible(true);

    sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::LINE);
    sds2->SetLineWidth(8);
    sds2->SetColor(color_table[k]);
    sds2->SetVisible(true);
  }

  m_pSceneInterMed->WellOverburdenDrawSpec().SetVisible(true);	

//	pSection2 = new well::CWellSection(m_pWellPath,8000,12000);
//	pList->AddSection(pSection2);
//	sds2 = &m_pSceneInterMed->AddSection(*pSection2);
//	sds2->SetLineWidth(4);
//	sds2->SetColor(RGB(255,255,255));
  
  UpdateAllViews(0);
  */
}


void CTestViewExeDoc::InitWellPath()
{
/*
  CXMLBase xml_base("CASE_TREE");
  xml_base.AddElement("CASE");
  xml_base.CurrentNode(xml_base.AddElement("LAYER"));
  xml_base.AddAttribute("NAME", "Rotliegendes");
  xml_base.AddAttribute("DEPLETION_CONSTANT", 0.688615);
  xml_base.AddAttribute("FRICTION_ANGLE", 12.5);
  xml_base.AddAttribute("COHESIVE_STRENGTH", 1200000);
  xml_base.AddAttribute("POISSONS_RATIO", 0.19);
  xml_base.AddAttribute("YOUNG_MODULUS", 7000000000.0);
  xml_base.AddAttribute("ENABLED", true);

  xml_base.CurrentNode(xml_base.AddElement("DEPLETION"));
  xml_base.AddAttribute("YEAR", 1991);
  xml_base.AddAttribute("RESERVOIR_PRESSURE", 39000000);
  xml_base.AddAttribute("ENABLED", true);

  xml_base.CurrentNode(xml_base.FindElement("LAYER"));
  xml_base.CurrentNode(xml_base.AddElement("DEPLETION"));
  xml_base.AddAttribute("YEAR", 2040);
  xml_base.AddAttribute("RESERVOIR_PRESSURE", 950000);
  xml_base.AddAttribute("ENABLED", true);

  xml_base.CurrentNode(xml_base.Root());
  xml_base.AddElement("WELL");
*/
  QFile file0("d:\\users\\tnma\\original.fxi");
  file0.open(IO_ReadOnly);
  CXMLBase xml_base(file0);
  file0.close();

  QFile file("d:\\users\\tnma\\test.fxi");
  if(file.open(IO_WriteOnly))
  {
    QTextStream stream(&file);
    xml_base.Save(stream);
    file.close();
  }
  //InitWellPath2();
/*
  m_pWellPath = new well::CWellPath();
  m_pWellPath->SetHoleName("test wellpath");
  m_pWellPath->InitFromMWRFile("D:\\users\\htg\\GEOMEC3\\test\\Dstabor\\test1\\A9Simple.mwr"); 
  m_pWellPath->DumpToFile("D:\\users\\htg\\GEOMEC3\\test\\Dstabor\\test1\\well_dump.mwr");
  long nr = m_pWellPath->DefPointList().RemoveRedundantPoints();
  m_pWellPath->DumpToFile("D:\\users\\htg\\GEOMEC3\\test\\Dstabor\\test1\\well_dump2.mwr");
  //RTD16B.mwr

  m_pScene = new COpenGLScene;
  //m_pScene->EnableLighting(FALSE);
  m_pSceneInterMed = new CWellSceneInterMed(*m_pWellPath);

  well::CWellPointList::Iterator it = m_pWellPath->DefPointList().GetIterator();
  well::CWellPoint* pPoint = it.current();

  well::CWellPointList* pDStaborList=new well::CWellPointList(*m_pWellPath);
  pDStaborList->SetDescription("DStabor Points");
  m_pWellPath->InsertWellPointList(pDStaborList,LST_DSTABORPOINTS);

  int i=0;
  while(pPoint)
  {

    if((i % 100) == 0)
    {
      pDStaborList->AddPoint(new well::CWellPoint(*pPoint));		
      CWellPointDrawSpec& ds = m_pSceneInterMed->AddPoint( *pPoint );
      QString str,frmt;
      str = "     wp " + frmt.setNum(pPoint->Azimuth().Value());
      str = str + " / "  + frmt.setNum(pPoint->Inclination().Value());

      str = frmt.setNum(pPoint->TMD().Value());

      ds.SetDescription(str);
      ds.SetTextColor(RGB(255,255,255));
      ds.SetColor(RGB(0,255,0));
      ds.SetVisible(true);
      
      
    }
    pPoint = ++it;
    i++;
  }

  double tmd1 = m_pWellPath->DefPointList().First()->TMD().Value();
  double tmd2 = m_pWellPath->DefPointList().Last()->TMD().Value();

  well::CWellSectionList* pGlobalList = new well::CWellSectionList(*m_pWellPath);
  pGlobalList->SetDescription("Casings");
  m_pWellPath->InsertWellSectionList(pGlobalList,0);

  well::CWellSection* pSection = new well::CWellSection(*m_pWellPath,tmd1,tmd2);
  pSection->SetDescription("Casing380");
  pGlobalList->AddSection(pSection);

  well::CWellSectionList* pList = new well::CWellSectionList(*m_pWellPath);
  pList->SetDescription("Formations");
  m_pWellPath->InsertWellSectionList(pList,1);

  

  //CWellSectionDrawSpec* sds = &m_pSceneInterMed->AddSection(*pSection);
  //sds->SetLineWidth(4);

  for(int k=0;k<5;k++)
  {
    double tmd_1 = tmd1+(tmd2-tmd1)/(5)*k;
    double tmd_2 = tmd_1+(tmd2-tmd1)/(5);

    QString frmt;

    well::CWellSection* pSection2 = new well::CWellSection(*m_pWellPath,tmd_1,tmd_2);
    pSection2->SetDescription(QString("Formation ") + frmt.setNum(k));
    pList->AddSection(pSection2);

    CWellSectionDrawSpec* sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::ARROW);
    sds2->SetLineWidth(1);
    sds2->SetColor(RGB(255,255,255));
    QString frm;
    sds2->SetPointSize(6);
    sds2->SetDescription(QString("Section ") + frm.setNum(k));
    sds2->SetVisible(true);

    sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::SQUARE);
    sds2->SetLineWidth(1);
    sds2->SetColor(color_table[k]);
    sds2->SetVisible(true);

    sds2 = &m_pSceneInterMed->AddSection(*pSection2,CWellSectionDrawSpec::LINE);
    sds2->SetLineWidth(8);
    sds2->SetColor(color_table[k]);
    sds2->SetVisible(true);
  }

  //m_pSceneInterMed->WellOverburdenDrawSpec().SetVisible(true);	

//	pSection2 = new well::CWellSection(m_pWellPath,8000,12000);
//	pList->AddSection(pSection2);
//	sds2 = &m_pSceneInterMed->AddSection(*pSection2);
//	sds2->SetLineWidth(4);
//	sds2->SetColor(RGB(255,255,255));
  
  UpdateAllViews(0);
  */
}

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeDoc serialization

void CTestViewExeDoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    // TODO: add loading code here
  }
}

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeDoc diagnostics

#ifdef _DEBUG
void CTestViewExeDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CTestViewExeDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeDoc commands

void CTestViewExeDoc::OnTestmenuDeletesection() 
{
  if(m_pWellPath->GetWellSectionList(1).NrOfSections())
  {
    m_pWellPath->GetWellSectionList(1).RemoveSection(0);
  }
  else
  {
    AfxMessageBox("No more sections");
  }
  
}

void CTestViewExeDoc::OnTestmenuAtachscene() 
{
  m_pSceneInterMed->AttachScene(m_pScene);
  
}

void CTestViewExeDoc::OnTestmenuDetachscene() 
{
  m_pSceneInterMed->DetachScene();
  
}

void CTestViewExeDoc::OnTestmenuShowdialog() 
{
  QPtrVector<well::CWellSectionList> Sections;
  Sections.resize(2);
  Sections.insert(0,&m_pWellPath->GetWellSectionList(0));
  Sections.insert(1,&m_pWellPath->GetWellSectionList(1));


  //CDerivedWellPickDlg	 dlg(&m_pWellPath->GetWellPointList(LST_DSTABORPOINTS), m_pSceneInterMed, &Sections ,CDoubleQuantity::SI_UNIT);
//	CDerivedWellPickDlg	 dlg(&m_pWellPath->DefPointList(), m_pSceneInterMed, Sections ,CDoubleQuantity::SI_UNIT);
//	dlg.DoModal();
  
}

void CTestViewExeDoc::OnTestmenuShowListdlg() 
{
  CWellPointListDlg dlg(*m_pWellPath);
  dlg.DoModal();
  // TODO: Add your command handler code here
  
}
