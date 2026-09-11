// WellLevelDrawSpec.cpp: implementation of the CWellLevelDrawSpec class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <cmath>
#include "WaterLevelSymbol.h"
#include "ArrowLabelSymbol.h"
#include "WellSceneInterMed.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaterLevelSymbol::CWaterLevelSymbol(CWellSceneInterMed& WellSceneInterMed)
:CWellDrawSpecBase(WellSceneInterMed)
{
}

void CWaterLevelSymbol::CreateScene()
{

  DeleteAllDisplayList();

  if(!GetVisible())
    return;

  well::CWellPathBase* pWellPath = &InterMed().WellPath();

  if(pWellPath->MeanSeaLevel().Undefined())
    return;

  if(pWellPath->GetLocation() == well::CWellPathBase::ONSHORE)
    return;

  if(!pWellPath->Defined())
    return;

  CDisplayPair* pWaterLine = CreateNewPair();
  CDisplayPair* p = CreateNewPair();
  CDisplayPair* pSymbol = CreateNewPair();

  p->DrawDef().PolyFillFront(FALSE);
  p->DrawDef().PolyFillBack(FALSE);
  p->DrawDef().PointSize(GetPointSize());
  p->DrawDef().LineWidth(GetLineWidth());
  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());

  pSymbol->DrawDef() = p->DrawDef();

  pSymbol->DrawDef().LineWidth(0.5);
  pSymbol->DrawDef().TextColor(qRgb(255,255,255));
  pSymbol->DrawDef().RequireCompile(FALSE);

  pWaterLine->DrawDef() = pSymbol->DrawDef();

  double size = pWellPath->DefPointList().Last()->TVD().Value()/8.0;
  //double size=pWellPath->DeltaTMD()/8.0;

  geo::CPoint p1;
  geo::CPoint p2;

  p1 = geo::CPoint(size,size,0);
  p2 = geo::CPoint(-size,size,0);
  p->Array().PushBack(*new geo::CLine(p1,p2));
  
  p1 = p2;
  p2 = geo::CPoint(-size,-size,0);
  p->Array().PushBack(*new geo::CLine(p1,p2));

  p1 = p2;
  p2 = geo::CPoint(size,-size,0);
  p->Array().PushBack(*new geo::CLine(p1,p2));

  p1 = p2;
  p2 = geo::CPoint(size,size,0);
  p->Array().PushBack(*new geo::CLine(p1,p2));

  int NrOfLines = 10;
  
  double difZ=size/40;

  p1.Move(geo::CVector(0,+size*0.25,0));
  p2.Move(geo::CVector(0,-size*0.25,0));

  double difY=(p2.Y()-p1.Y())/double(NrOfLines+1)/2;///pow(1.2,NrOfLines);

  for(int i=0;i<NrOfLines;i++)
  {
    p1.Move(geo::CVector(0,+difY,+difZ));
    p2.Move(geo::CVector(0,-difY,+difZ));
    pWaterLine->Array().PushBack(*new geo::CLine(p1,p2));
    difZ=difZ*1.2;
  }


  CArrowLabelSymbol* pSym = new CArrowLabelSymbol(geo::CPoint::NullPoint,geo::CVector::Yaxis,GetDescription());
  pSymbol->Array().PushBack(*pSym);

  double dx = pWellPath->DefPointList().First()->X();
  double dy = pWellPath->DefPointList().First()->Y();
  double dz = pWellPath->MeanSeaLevel().Value();

  p->Array().Move(geo::CVector(dx,dy,dz));
  pSymbol->Array().Move(geo::CVector(dx,dy+size,dz));
  pWaterLine->Array().Move(geo::CVector(dx,dy,dz));

  UpdateScene();
}
