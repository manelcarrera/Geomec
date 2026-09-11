// WellSectionDrawSpec.cpp: implementation of the CWellSectionDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "ColorScale.h"
#include "VectorSymbol.h"
#include "WellDrawSpecBase.h"
#include "WellSceneInterMed.h"
#include "WellSectionDrawSpec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWellSectionDrawSpec::CWellSectionDrawSpec(CWellSceneInterMed &WellSceneInterMed, well::IWellSection &WellSection,
                                           CWellSectionDrawSpec::TYPE Type)
    : CWellDrawSpecBase(WellSceneInterMed), m_pWellSection(&WellSection) {
  // point to the same wellpath
  assert(&m_pWellSceneInterMed->WellPath() == &m_pWellSection->WellPath());

  m_ArrowLineAngle = 0;
  m_ArrowDistanceLevel = 1;
  m_Visible = false;
  m_LineWidth = 2;
  m_Color = qRgb(255, 255, 255);
  m_ColorScale = 0;

  connect(m_pWellSection, SIGNAL(OnDestroy(const well::IWellSection &)), this,
          SLOT(OnSectionDestroyed(const well::IWellSection &)));
  m_Type = Type;
  m_GradientType = TMD;
  CreateScene();
}

void CWellSectionDrawSpec::CreateScene() {
  DeleteAllDisplayList();

  if (!GetVisible())
    return;

  if (m_Type == LINE) {
    CreateLines();
  } else if (m_Type == SQUARE) {
    CreateSquares();
  } else if (m_Type == ARROW) {
    CreateArrow();
  } else if (m_Type == GRADIENT) {
    CreateGradient();
  } else {
    assert(false);
  }
}

void CWellSectionDrawSpec::SetColorScaleForGradient(const CColorScale &cs) {
  if (!m_ColorScale)
    m_ColorScale = new CColorScale;

  *m_ColorScale = cs;

  CreateScene();
}

void CWellSectionDrawSpec::OnSectionDestroyed(const well::IWellSection & /*section*/) {
  //"this" is also deleted (see autoDelete in WellSceneInterMed)
  m_pWellSceneInterMed->RemoveSections(*m_pWellSection);
}

CWellSectionDrawSpec::~CWellSectionDrawSpec() {
  if (m_ColorScale)
    delete m_ColorScale;

  DeleteAllDisplayList();
}

void CWellSectionDrawSpec::CreateArrow() {
  DeleteAllDisplayList();
  CDisplayPair *p1 = CreateNewPair();

  p1->DrawDef().PolyFillFront(FALSE);
  p1->DrawDef().PolyFillBack(FALSE);
  p1->DrawDef().PointSize(GetPointSize());
  p1->DrawDef().LineWidth(GetLineWidth());
  p1->DrawDef().TextColor(GetTextColor());
  p1->DrawDef().SetFixedColor(GetColor());

  CDisplayPair *p2 = CreateNewPair();

  p2->DrawDef().PolyFillFront(TRUE);
  p2->DrawDef().PolyFillBack(TRUE);
  p2->DrawDef().PointSize(GetPointSize());
  p2->DrawDef().LineWidth(GetLineWidth());
  p2->DrawDef().TextColor(GetTextColor());
  p2->DrawDef().SetFixedColor(GetColor());

  CDisplayPair *pSymbol = CreateNewPair();

  pSymbol->DrawDef().PolyFillFront(FALSE);
  pSymbol->DrawDef().PolyFillBack(FALSE);
  pSymbol->DrawDef().PointSize(GetPointSize());
  pSymbol->DrawDef().LineWidth(GetLineWidth());
  pSymbol->DrawDef().TextColor(GetTextColor());
  pSymbol->DrawDef().SetFixedColor(GetColor());
  pSymbol->DrawDef().RequireCompile(FALSE);

  if (m_pWellSection->WellPath().DefPointList().NrOfPoints() < 2)
    return;

  well::CWellPoint &wp = *m_pWellSection->WellPath().DefPointList().First();
  well::CWellPoint &wp2 = *m_pWellSection->WellPath().DefPointList().Last();

  geo::CPoint p_start = wp;
  geo::CPoint p_end = wp2;

  double size = (wp2.TMD().Value() - wp.TMD().Value()) / 20;

  int count = 40; // number of lines in section

  if (m_pWellSection->WellPath().IsStraight())
    count = 3;

  double top = m_pWellSection->Top().TMD().Value();
  double bottom = m_pWellSection->Bottom().TMD().Value();

  double dl = (bottom - top) / (count - 1);

  // f_dir  is direction of first point
  geo::CVector f_dir = wp.GetDirection();
  geo::CVector nor;

  well::CWellPoint *pwell_1 = m_pWellSection->WellPath().DefPointList().At(0);
  well::CWellPoint *pwell_2 = m_pWellSection->WellPath().DefPointList().At(1);

  nor = pwell_1->GetDirection().CrossProduct(pwell_2->GetDirection());

  if (nor == geo::CVector::NullVector) {
    // get the normal vector to the start point
    if (!geo::CPlane::PlaneXY.Normal().IsParallel(f_dir))
      nor = f_dir.CrossProduct(geo::CPlane::PlaneXY.Normal());
    else
      nor = f_dir.CrossProduct(geo::CPlane::PlaneYZ.Normal());
  }

  // a normal to the first point
  nor = nor.UnitVector();

  // create the first point
  geo::CPoint first_point = p_start + nor * size * m_ArrowDistanceLevel;

  if (m_ArrowLineAngle != 0) {
    first_point.Move(geo::CVector(p_start * -1.0));
    first_point.Rotate(f_dir, -m_ArrowLineAngle);
    first_point.Move(geo::CVector(p_start));
  }

  nor = geo::CVector(first_point - wp).UnitVector();

  geo::CVector plane_normal = f_dir.CrossProduct(geo::CVector(first_point - wp));

  geo::CPoint *pPrevPoint = 0;
  geo::CPoint *point = 0;

  geo::IObject *conus1 = 0;
  geo::IObject *conus2 = 0;

  for (int i = 0; i < count; i++) {
    double tmd = top + dl * i;

    well::CWellPoint p_mid(m_pWellSection->WellPath(), tmd);
    assert(p_mid.Defined());
    geo::CVector p_dir = p_mid.GetDirection();

    // move the mid point along nor
    point = new geo::CPoint(p_mid + nor * size * m_ArrowDistanceLevel);

    assert(!point->Empty());

    if (i == int(count / 2)) {
      // we are in the middle so create a label
      CLabelPoint *label = new CLabelPoint(*point, GetDescription());
      CDisplayPair *pTmp = CreateNewPair();
      pTmp->Array().PushBack(*label);
      pTmp->DrawDef().TextColor(qRgb(255, 255, 255));
      pTmp->DrawDef().RequireCompile(FALSE);
    }

    // create the line from this point to previous point
    if (pPrevPoint) {
      if (conus1 == 0) {
        geo::CVector dir(*point, *pPrevPoint);
        conus1 = new CVectorSymbol(*pPrevPoint, dir, 20, false);

        pSymbol->Array().PushBack(*conus1);
      }
      // not first
      geo::CLine *pl = new geo::CLine(*point, *pPrevPoint);
      p1->Array().PushBack(*pl);
    } else {

      // first point
      p1->Array().PushBack(*new geo::CLine(p_mid, *point));
    }

    if (i == count - 1) {
      // last point
      geo::CVector dir(*point, *pPrevPoint);
      if (!(dir == geo::CVector::NullVector)) {
        conus2 = new CVectorSymbol(*point, -dir, 20, false);
        pSymbol->Array().PushBack(*conus2);
        p1->Array().PushBack(*new geo::CLine(p_mid, *point));
      }
    }

    delete pPrevPoint;
    pPrevPoint = point;
  }

  delete pPrevPoint;

  UpdateScene();
}

void CWellSectionDrawSpec::CreateSquares() {

  DeleteAllDisplayList();

  CDisplayPair *dp = CreateNewPair();

  dp->DrawDef().TextColor(GetTextColor());
  dp->DrawDef().SetFixedColor(GetColor());
  dp->DrawDef().LineWidth(GetLineWidth());
  dp->DrawDef().PolyFillFront(FALSE);
  dp->DrawDef().PolyFillBack(FALSE);
  dp->DrawDef().PointSize(GetPointSize());

  well::CWellPoint &wp = *m_pWellSection->WellPath().DefPointList().First();
  well::CWellPoint &wp2 = *m_pWellSection->WellPath().DefPointList().Last();

  geo::CPoint p_start = wp;
  geo::CPoint p_end = wp2;

  double size = (wp2.TMD().Value() - wp.TMD().Value()) / 80;

  int count = 40; // number sections to draw a circle

  if (m_pWellSection->WellPath().IsStraight())
    count = 3;

  long NrOfPointsOnPoly = 4; // number of points on polygon (the more it becomes a circle)

  double top = m_pWellSection->Top().TMD().Value();
  double bottom = m_pWellSection->Bottom().TMD().Value();

  double dl = (bottom - top) / double((count - 1));

  geo::CVector dir = wp.GetDirection();
  geo::CVector nor;

  // get the normal vector to the start point
  if (!geo::CPlane::PlaneXY.Normal().IsParallel(dir))
    nor = dir.CrossProduct(geo::CPlane::PlaneXY.Normal());
  else
    nor = dir.CrossProduct(geo::CPlane::PlaneYZ.Normal());

  nor = nor.UnitVector();

  // create the first polygon
  geo::CPoint corner_point = p_start + nor * size;
  corner_point.Move(geo::CVector(p_start * -1.0));
  corner_point.Rotate(dir, 45);
  corner_point.Move(geo::CVector(p_start));
  geo::CPolygon *top_poly = new geo::CPolygon;
  for (int k = 0; k < NrOfPointsOnPoly; k++) {
    corner_point.Move(geo::CVector(p_start * -1.0));
    corner_point.Rotate(dir, 360.0 / NrOfPointsOnPoly);
    corner_point.Move(geo::CVector(p_start));
    top_poly->PushBack(corner_point);
  }

  geo::CPtrArray<geo::CPolygon> PolyToDelete;

  // copy the first polydgon along the section
  geo::CPolygon *pPrevPoly = 0;
  for (int i = 0; i < count; i++) {
    double tmd = top + dl * double(i);

    well::CWellPoint p(m_pWellSection->WellPath(), tmd);

    geo::CVector p_dir = p.GetDirection();
    geo::CPoint p_mid = p;

    // make a copy of the start polygon
    geo::CPolygon *poly = new geo::CPolygon(*top_poly);

    // translate and rotate polygon at right poition
    geo::CVector rot_vec = poly->Normal().CrossProduct(p_dir);
    if (!(rot_vec == geo::CVector::NullVector)) {
      poly->Move(geo::CVector(p_start * -1.0));
      double rot_angle = poly->Normal().AngleDeg(p_dir);
      poly->Rotate(rot_vec, rot_angle);
      poly->Move(geo::CVector(p_start));
    }
    poly->Move(geo::CVector(p_start, p_mid));

    // TESTING GEOMETRY !!!!
    //		geo::CLine* pLine = new geo::CLine(p_mid,p_dir.UnitVector()*200);
    //		QString str,frmt1,frmt2;
    //		str = "    " + frmt1.setNum(p.Azimuth().Value()) + " / " + frmt2.setNum(p.Inclination().Value());
    //		dp->Array().PushBack(*new CLabelPoint(p_mid,CString(str)));
    //		dp->Array().PushBack(*new geo::CPoint(p_mid));
    //		dp->Array().PushBack(*pLine);
    //		dp->Array().PushBack(* new geo::CLine(p_mid,poly->Normal().UnitVector()*200));

    if (i == count - 1)
      dp->Array().PushBack(*new geo::CPolygon(*poly));

    PolyToDelete.PushBack(*poly);

    // create the lines from this and previous polygon
    if (pPrevPoly) {
      for (int j = 0; j < poly->NrOfPoints(); j++) {
        geo::CLine *pl = new geo::CLine(poly->Point(j), pPrevPoly->Point(j));
        dp->Array().PushBack(*pl);
      }

      // put polygon in PtrArray
      // dp->Array().PushBack(*poly);

    } else {
      dp->Array().PushBack(*new geo::CPolygon(*poly));
    }

    pPrevPoly = poly;
  }

  // we dond need the start poygon
  delete top_poly;

  PolyToDelete.ClearAndDelete();

  // update scene
  UpdateScene();
}

/// 2 helper classes /////////////////////////////////////////////////
class CWellLine : public geo::ILine {
public:
  CWellLine(well::CWellPoint *p1, well::CWellPoint *p2, CWellLine *prev = 0) : m_p1(p1), m_p2(p2), m_prev(prev) {}
  ~CWellLine() { delete m_p1, delete m_p2; }

  double GetTotalLength() const {
    if (m_prev)
      return m_prev->GetTotalLength() + Length();

    return m_p1->TMD().Value() + Length();
  }

  double GetLength(const geo::IPoint &point) const {
    geo::CPoint pr = Project(point);
    double l = First().Distance(pr);
    if (m_prev)
      return m_prev->GetTotalLength() + l;

    return m_p1->TMD().Value() + l;
  }
  virtual const geo::IPoint &Point(int nIndex) const {
    if (nIndex == 0)
      return *m_p1;
    else
      return *m_p2;
  }
  virtual void Point(int /*nIndex*/, const geo::IPoint & /*pt*/) { assert(false); }

  //	virtual const geo::IPoint &First() const {return *m_p1;}
  //	virtual void First(const geo::IPoint& point) {assert(false);}
  //	virtual void Second(const geo::IPoint& point) {assert(false);}
  //	virtual const geo::IPoint &Second() const {return *m_p2;}
  virtual size_t Order() const { return 1; }

private:
  well::CWellPoint *m_p1;
  well::CWellPoint *m_p2;
  CWellLine *m_prev;
};

class CWellLineDrawDef : public CDrawDef {
public:
  virtual bool IsSelectable(const geo::IObject &object) const {
    const CWellLine *l = dynamic_cast<const CWellLine *>(&object);
    if (l)
      return true;

    return false;
  }
};

class GradiendDrawDef : public CWellLineDrawDef {
public:
  GradiendDrawDef(CColorScale *ColorScale, CWellSectionDrawSpec::GRADIENT_TYPE GradientType)
      : m_ColorScale(ColorScale), m_GradientType(GradientType) {}

  virtual std::vector<TColor> Color(const geo::IObject &object) const {
    std::vector<TColor> ret;
    const CWellLine &line = dynamic_cast<const CWellLine &>(object);
    assert(&line);
    double val1;
    double val2;

    if (m_GradientType == CWellSectionDrawSpec::TVD) {
      val1 = ((well::CWellPoint *)(&line.First()))->TVD().Value();
      val2 = ((well::CWellPoint *)(&line.Second()))->TVD().Value();
    } else {
      val1 = ((well::CWellPoint *)(&line.First()))->TMD().Value();
      val2 = ((well::CWellPoint *)(&line.Second()))->TMD().Value();
    }

    ret.push_back(m_ColorScale->GetColor(val1));
    ret.push_back(m_ColorScale->GetColor(val2));

    return ret;
  }

private:
  CColorScale *m_ColorScale;
  CWellSectionDrawSpec::GRADIENT_TYPE m_GradientType;
};
////////////////////////////////////////////////////

void CWellSectionDrawSpec::CreateLines() {
  DeleteAllDisplayList();

  CDisplayPair *p = CreateNewPair(new CWellLineDrawDef, 0);

  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());
  p->DrawDef().LineWidth(GetLineWidth());
  p->DrawDef().PolyFillFront(FALSE);
  p->DrawDef().PolyFillBack(FALSE);
  p->DrawDef().PointSize(GetPointSize());

  double top = m_pWellSection->Top().TMD().Value();
  double bottom = m_pWellSection->Bottom().TMD().Value();

  int count = 100;

  if (m_pWellSection->WellPath().IsStraight())
    count = 3;

  double dl = (bottom - top) / (count - 1);

  CWellLine *pPrevLine = 0;
  for (int i = 0; i < count - 1; i++) {
    double tmd1 = top + dl * i;
    double tmd2 = top + dl * (i + 1);
    well::CWellPoint *p1 = new well::CWellPoint(m_pWellSection->WellPath(), tmd1);
    well::CWellPoint *p2 = new well::CWellPoint(m_pWellSection->WellPath(), tmd2);
    CWellLine *pLine = new CWellLine(p1, p2, pPrevLine);
    pPrevLine = pLine;
    p->Array().PushBack(*pLine);
  }
  UpdateScene();
}

void CWellSectionDrawSpec::CreateGradient() {
  DeleteAllDisplayList();

  if (!m_ColorScale)
    return;

  GradiendDrawDef *dd = new GradiendDrawDef(m_ColorScale, m_GradientType);
  CDisplayPair *p = CreateNewPair(dd, 0);

  p->DrawDef().TextColor(GetTextColor());
  p->DrawDef().SetFixedColor(GetColor());
  p->DrawDef().LineWidth(GetLineWidth());
  p->DrawDef().PolyFillFront(FALSE);
  p->DrawDef().PolyFillBack(FALSE);
  p->DrawDef().PointSize(GetPointSize());

  double top = m_pWellSection->Top().TMD().Value();
  double bottom = m_pWellSection->Bottom().TMD().Value();

  int count = 100;

  if (m_pWellSection->WellPath().IsStraight())
    count = 3;

  double dl = (bottom - top) / (count - 1);

  CWellLine *pPrevLine = 0;
  for (int i = 0; i < count - 1; i++) {
    double tmd1 = top + dl * i;
    double tmd2 = top + dl * (i + 1);
    well::CWellPoint *p1 = new well::CWellPoint(m_pWellSection->WellPath(), tmd1);
    well::CWellPoint *p2 = new well::CWellPoint(m_pWellSection->WellPath(), tmd2);
    CWellLine *pLine = new CWellLine(p1, p2, pPrevLine);
    pPrevLine = pLine;
    p->Array().PushBack(*pLine);
  }
  UpdateScene();
}

CWellSectionDrawSpec::TYPE CWellSectionDrawSpec::GetType() const { return m_Type; }

void CWellSectionDrawSpec::SetType(CWellSectionDrawSpec::TYPE type) {
  m_Type = type;
  CreateScene();
}

void CWellSectionDrawSpec::SetArrowDistanceLevel(int level) { m_ArrowDistanceLevel = level; }

int CWellSectionDrawSpec::GetArrowDistanceLevel() { return m_ArrowDistanceLevel; }

void CWellSectionDrawSpec::SetArrowLineAngle(double lineAngle) { m_ArrowLineAngle = lineAngle; }

void CWellSectionDrawSpec::SetGradientType(GRADIENT_TYPE type) {
  m_GradientType = type;
  CreateScene();
}

// returns a new created wellpoint depending on selected objects in vec
// if no selection then return null
// caller is responsible for deleting return point
well::CWellPoint *CWellSectionDrawSpec::ProcessNewPointSelection(std::vector<const geo::IObject *> &vec,
                                                                 const geo::CLine &selection_line) {
  for (size_t j = 0; j < vec.size(); j++) {
    const CWellLine *line = dynamic_cast<const CWellLine *>(vec[j]);
    if (line) {
      geo::CPoint PointAtLine, PointAtThis;
      line->GetClosestPoints(selection_line, PointAtLine, PointAtThis);
      double tmd = line->GetLength(PointAtThis);
      return new well::CWellPoint(m_pWellSection->WellPath(), tmd);
    }
  }

  return 0;
}