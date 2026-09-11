// DrillingTowerSymbol.cpp: implementation of the CDrillingTowerSymbol class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#define NOMINMAX
#include "ArrowLabelSymbol.h"
#include "DrillingTowerSymbol.h"
#include "WellSceneInterMed.h"
#include <cmath>

CDrillingTowerSymbol::CDrillingTowerSymbol(CWellSceneInterMed &WellSceneInterMed)
    : CWellDrawSpecBase(WellSceneInterMed) {}

void CDrillingTowerSymbol::CreateScene() {

  DeleteAllDisplayList();

  if (!GetVisible())
    return;

  well::CWellPathBase *pWellPath = &InterMed().WellPath();

  if (!pWellPath->Defined())
    return;

  double SCALE = pWellPath->DefPointList().Last()->TVD().Value() / 40.0;
  double HEIGH = 5.0 * SCALE;

  double WIDTH1 = 1.5 * SCALE;
  double WIDTH2 = 4.0 * SCALE;

  CDisplayPair *p = CreateNewPair();
  CDisplayPair *pSymbol = CreateNewPair();

  p->DrawDef().PolyFillFront(FALSE);
  p->DrawDef().PolyFillBack(FALSE);
  p->DrawDef().PointSize(GetPointSize());
  p->DrawDef().LineWidth(GetLineWidth());
  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());

  pSymbol->DrawDef() = p->DrawDef();

  pSymbol->DrawDef().LineWidth(0.5);
  pSymbol->DrawDef().TextColor(qRgb(255, 255, 255));
  pSymbol->DrawDef().RequireCompile(FALSE);

  geo::CPoint p1 = geo::CPoint(-WIDTH1 / 2.0, 0, -HEIGH / 2.0);
  geo::CPoint p2 = geo::CPoint(WIDTH1 / 2.0, 0, -HEIGH / 2.0);

  geo::CPoint p5 = geo::CPoint(-(WIDTH1 + WIDTH2) / 4.0, 0, 0);
  geo::CPoint p6 = geo::CPoint((WIDTH1 + WIDTH2) / 4.0, 0, 0);

  geo::CPoint p4 = geo::CPoint(-WIDTH2 / 2.0, 0, HEIGH / 2.0);
  geo::CPoint p3 = geo::CPoint(WIDTH2 / 2.0, 0, HEIGH / 2.0);

  p->Array().PushBack(*new geo::CLine(p1, p2));
  p->Array().PushBack(*new geo::CLine(p2, p3));
  // p->Array().PushBack(*new geo::CLine( p3, p4));
  p->Array().PushBack(*new geo::CLine(p4, p1));

  // p->Array().PushBack(*new geo::CLine( p1, p6));
  // p->Array().PushBack(*new geo::CLine( p2, p5));
  // p->Array().PushBack(*new geo::CLine( p5, p3));
  // p->Array().PushBack(*new geo::CLine( p6, p4));
  p->Array().PushBack(*new geo::CLine(p5, p6));

  double angle = RAD2DEG(atan(((WIDTH2 - WIDTH1) / 2.0) / HEIGH));
  geo::CLine rotline = geo::CLine(geo::CPoint::NullPoint, geo::CVector::Zaxis);

  p->Array().RotateLine(geo::CLine(p5, p6), angle);
  p->Array().Move(geo::CVector(0, -(WIDTH1 + WIDTH2) / 4.0, 0));

  size_t size = p->Array().Size();

  for (int i = 0; i < 3; i++) {
    double angle2 = 90.0 * double(i + 1);
    for (size_t j = 0; j < size; j++) {
      geo::CLine l = dynamic_cast<geo::CLine &>(p->Array().Object(j));

      l.RotateLine(rotline, angle2);
      p->Array().PushBack(*new geo::CLine(l));
    }
  }

  double dx = pWellPath->DefPointList().First()->X();
  double dy = pWellPath->DefPointList().First()->Y();
  double dz = pWellPath->GlobalTVD().Value();

  CArrowLabelSymbol *pSym = new CArrowLabelSymbol(geo::CPoint::NullPoint, geo::CVector::Yaxis, GetDescription());
  pSymbol->Array().PushBack(*pSym);

  p->Array().Move(geo::CVector(dx, dy, dz));
  pSymbol->Array().Move(geo::CVector(dx, dy, dz));

  double minZ = pWellPath->GlobalTVD().Value();
  double maxZ = pWellPath->DefPointList().First()->Z();

  if (!pWellPath->MeanGroundOrSeabedLevel().Undefined()) {
    minZ = std::min(minZ, pWellPath->MeanGroundOrSeabedLevel().Value());
    maxZ = std::max(maxZ, pWellPath->MeanGroundOrSeabedLevel().Value());
  }

  if (!pWellPath->MeanSeaLevel().Undefined()) {
    minZ = std::min(minZ, pWellPath->MeanSeaLevel().Value());
    maxZ = std::max(maxZ, pWellPath->MeanSeaLevel().Value());
  }

  p1 = *pWellPath->DefPointList().First();
  p2 = p1;

  p2.Z(minZ);
  p1.Z(maxZ);

  if (!(p1 == p2)) {
    CDisplayPair *pCenter = CreateNewPair();
    pCenter->DrawDef() = p->DrawDef();
    pCenter->DrawDef().SetFixedColor(qRgb(190, 190, 190));
    pCenter->DrawDef().LineStipple(TRUE);
    pCenter->DrawDef().LineWidth(float(0.3));
    pCenter->Array().PushBack(*new geo::CLine(p1, p2));
  }

  UpdateScene();
}
