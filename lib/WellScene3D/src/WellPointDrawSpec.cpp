// WellPointDrawSpec.cpp: implementation of the CWellPointDrawSpec class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "WellDrawSpecBase.h"
#include "WellSectionDrawSpec.h"
#include "WellPointDrawSpec.h"
#include "WaterLevelSymbol.h"
#include "LevelSymbol.h"
#include "WaterLevelSymbol.h"
#include "DrillingTowerSymbol.h"
#include "WellSceneInterMed.h"
#include "ArrowLabelSymbol.h"
#include "VectorSymbol.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWellPointDrawSpec::CWellPointDrawSpec(CWellSceneInterMed& WellSceneInterMed, well::CWellPoint& WellPoint,CWellPointDrawSpec::TYPE type)
:CWellDrawSpecBase(WellSceneInterMed),m_pWellPoint(&WellPoint)
{

  
  assert(&m_pWellPoint->WellPath() == &InterMed().WellPath());

  connect(m_pWellPoint   ,SIGNAL(OnDestroy(const well::CWellPoint&)) ,this, SLOT(OnPointDestroyed(const well::CWellPoint&)));
  connect(m_pWellPoint   ,SIGNAL(OnTMDChanged()) ,this, SLOT(OnTMDChanged()));
  m_Color = qRgb(0,128,128);
  m_Type=type;
  m_ArrowDirection = geo::CVector::Yaxis;
  CreateScene();
}


CWellPointDrawSpec::~CWellPointDrawSpec()
{
  DeleteAllDisplayList();
}

void CWellPointDrawSpec::OnPointDestroyed(const well::CWellPoint& /*wellpoint*/)
{
  //this will remove and delete all CWellPointDrawSpec
  //attached to m_pWellPoint including "this"
  m_pWellSceneInterMed->RemovePoints(*m_pWellPoint);
}


void CWellPointDrawSpec::OnTMDChanged()
{
  CreateScene();
}

void CWellPointDrawSpec::SetArrowDirection(const geo::CVector& dir)
{
  m_ArrowDirection= dir;
}


void CWellPointDrawSpec::CreateScene()
{

  DeleteAllDisplayList();

  if(!GetVisible())
    return;

  if(m_Type == SINGLE_DOT)
  {
    CreateDot();
  }
  else if(m_Type == ARROW)
  {
    CreateArrow();
  }
  else
  {
    assert(false);
  }


}



void CWellPointDrawSpec::CreateArrow()
{

  

  CDisplayPair* p = CreateNewPair();
  CDisplayPair* p2 = CreateNewPair();
  CDisplayPair* p3 = CreateNewPair();


  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());
  p->DrawDef().PointSize(GetPointSize());
  p->DrawDef().RequireCompile(FALSE);
  p->DrawDef().LineWidth(1);
  p->DrawDef().DepthFunc(GL_ALWAYS);

  p2->DrawDef() = p->DrawDef();
  p3->DrawDef() = p->DrawDef();
  
  p3->DrawDef().LabelBorder(TRUE);
  p3->DrawDef().LabelBorderInteriorColor(GetColor());
  p3->DrawDef().LabelBorderOutlineColor(GetColor());
  p3->DrawDef().TextColor(GetTextColor());

  p->DrawDef().SetFixedColor(ArrowColor());
  p2->DrawDef().SetFixedColor(ArrowColor());
  p3->DrawDef().LineWidth(3);

  double size=(m_pWellPoint->WellPath().DeltaTMD())/150;
  double LineLength =15*size;

  CVectorSymbol* vs = new CVectorSymbol(*m_pWellPoint,-m_ArrowDirection,0.01,false);
  

  geo::CLine *line = new geo::CLine(*m_pWellPoint, m_ArrowDirection.UnitVector()*LineLength);
  p->Array().PushBack(*vs);

  CLabelPoint * lb = new CLabelPoint(line->Second(), GetDescription(), QFont(), GetAlign(), 0);
  p3->Array().PushBack(*lb);
  p2->Array().PushBack(*line);

  UpdateScene();

}



void CWellPointDrawSpec::CreateDot()
{

  CDisplayPair* p = CreateNewPair();

  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());
  p->DrawDef().PointSize(GetPointSize());
  p->DrawDef().RequireCompile(FALSE);
  p->DrawDef().DepthFunc(GL_ALWAYS);

  QString str,frmt;
  str = frmt.setNum(m_pWellPoint->Azimuth().Value());
  str = str + " / " + frmt.setNum(m_pWellPoint->Inclination().Value());

  //p->DrawDef().LabelBorder(TRUE);
  //p->DrawDef().LabelBorderInteriorColor(qRgb(190,190,190));
  //p->DrawDef().LabelBorderOutlineColor(qRgb(155,155,155));

  CLabelPoint* pLabelPoint = new CLabelPoint(geo::CPoint(*m_pWellPoint), GetDescription(), QFont(), GetAlign(), 0);	
  p->Array().PushBack(*pLabelPoint);
  p->Array().PushBack(*new geo::CPoint(*m_pWellPoint));

  UpdateScene();
}


