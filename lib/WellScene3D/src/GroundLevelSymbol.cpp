// GroundLevelSymbol.cpp: implementation of the CGroundLevelSymbol class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "GroundLevelSymbol.h"
#include "ArrowLabelSymbol.h"
#include "WellSceneInterMed.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CGroundLevelSymbol::CGroundLevelSymbol(CWellSceneInterMed& WellSceneInterMed)
:CWellDrawSpecBase(WellSceneInterMed)
{
}

CGroundLevelSymbol::~CGroundLevelSymbol()
{

}


void CGroundLevelSymbol::CreateScene()
{

	DeleteAllDisplayList();

	if(!GetVisible())
		return;

	well::CWellPathBase* pWellPath = &InterMed().WellPath();

	if(pWellPath->MeanGroundOrSeabedLevel().Undefined())
		return;

	if(!pWellPath->Defined())
		return;

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

	double size = pWellPath->DefPointList().Last()->TVD().Value()/8.0;
	//douuble size=pWellPath->DeltaTMD()/8.0;

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


	CArrowLabelSymbol* pSym = new CArrowLabelSymbol(geo::CPoint::NullPoint,geo::CVector::Yaxis,GetDescription());
	pSymbol->Array().PushBack(*pSym);

	double dx = pWellPath->DefPointList().First()->X();
	double dy = pWellPath->DefPointList().First()->Y();
	double dz = pWellPath->MeanGroundOrSeabedLevel().Value();

	p->Array().Move(geo::CVector(dx,dy,dz));
	pSymbol->Array().Move(geo::CVector(dx,dy+size,dz));

	UpdateScene();
}
