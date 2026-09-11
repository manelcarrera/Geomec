/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "2DGraphBase.h"
#include "2DGraphLineBase.h"
#include "2DGraphParentBase.h"
#include "LabelPoint.h"
#include "Line.h"
#include "Polygon.h"
#include "dimple.h"
#include <QList>
#include <cmath>
#include <map>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C2DGraphBase::CDisplayPair::CDisplayPair(C2DGraphBase &parent) : m_parent(parent) {}

C2DGraphBase::CDisplayPair::~CDisplayPair() {
  m_parent.RemoveObject(this);
  m_array.ClearAndDelete();
}

void C2DGraphBase::CDisplayPair::Update() {
  // transform the points
  m_parent.Update(this);
}

geo::CPtrArray<geo::IObject> &C2DGraphBase::CDisplayPair::Array() { return m_array; }

CDrawDef &C2DGraphBase::CDisplayPair::DrawDef() { return m_drawdef; }

void C2DGraphBase::CDisplayPair::AddObject(geo::IObject &Object) { m_array.PushBack(Object); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C2DGraphBase::C2DGraphBase(int nDivisionX, int nDivisionY)
    : m_dMinX(DBL_MAX), m_dMaxX(-DBL_MAX), m_dGridX(0), m_dGridY(0), m_nDivisionX(nDivisionX), m_nDivisionY(nDivisionY),
      m_bAutoX(true), m_bAutoY(true), m_DragLineColor(qRgb(255, 255, 255)), m_bMarkerEnable(true),
      m_InvalidSectionColor(qRgb(90, 90, 90)), m_bInvalidSectionEnable(true),
      m_Label((CDisplayPair *)0, std::pair<int, int>(-1, -1)) {
  m_nDivisionX = nDivisionX;
  m_nDivisionY = nDivisionY;
}

C2DGraphBase::~C2DGraphBase() {
  RemoveAllObjects();
  while (!m_vcGraphLine.isEmpty())
    delete m_vcGraphLine.takeLast();
}

C2DGraphBase::CDisplayPair *C2DGraphBase::CreateNewPair() {
  CDisplayPair *p = new CDisplayPair(*this);
  m_mpDisplay.insert(TDisplayMap::value_type(p, -1));
  return p;
}

//////////////////////////////////////////////////////////////////////
void C2DGraphBase::DrawScene() {
  // Just making the display list
  for (TDisplayMap::iterator it = m_mpDisplay.begin(); it != m_mpDisplay.end(); it++) {
    if ((it->second == -1) && (it->first->DrawDef().RequireCompile()))
      it->second = CompileList(it->first->Array(), it->first->DrawDef());

    if (it->second == -1)
      DrawObject(it->first->Array(), it->first->DrawDef());
    else
      DrawList(it->second, it->first->DrawDef());
  }

  // Are we dragging?
  if (!m_ptStartDrag.Empty() && MaxY().Valid() && MinY().Valid()) {
    // Drawdef for draglines
    CDrawDef draw_def(m_DragLineColor);
    draw_def.Visible(TRUE);
    draw_def.PointSize(2);
    draw_def.LineStipple(TRUE);
    draw_def.SetDepthLayer2D(GRAPH_LAYER_FOREGROUND);

    geo::CPoint StartPoint1(m_ptStartDrag.X(), MinY().Value());
    geo::CPoint EndPoint1(m_ptStartDrag.X(), MaxY().Value());
    DrawObject(geo::CLine(StartPoint1, EndPoint1), draw_def);

    if (!m_ptPendingDrag.Empty()) {
      geo::CPoint StartPoint2(m_ptPendingDrag.X(), MinY().Value());
      geo::CPoint EndPoint2(m_ptPendingDrag.X(), MaxY().Value());
      DrawObject(geo::CLine(StartPoint2, EndPoint2), draw_def);
    }
  }
}

void C2DGraphBase::Update(CDisplayPair *pObject) {
  TDisplayMap::iterator it = m_mpDisplay.find(pObject);
  assert(it != m_mpDisplay.end());
  if (it != m_mpDisplay.end() && it->second != -1) {
    DeleteList(it->second);
    it->second = -1;
  }
}

//////////////////////////////////////////////////////////////////////

void C2DGraphBase::RemoveObject(CDisplayPair *pObject) {
  Update(pObject); // Removes any existing lists
  m_mpDisplay.erase(pObject);
}

void C2DGraphBase::RemoveAllObjects() {
  while (m_mpDisplay.size() > 0)
    delete m_mpDisplay.begin()->first;
}

void C2DGraphBase::FlushDisplayList() {
  for (TDisplayMap::iterator it = m_mpDisplay.begin(); it != m_mpDisplay.end(); it++) {
    if (it->second != -1) {
      DeleteList(it->second);
      it->second = -1;
    }
  }
}

void C2DGraphBase::InsertPolygon(int nMinIndex, int nMaxIndex, TColor color, const double &dEps, bool bCutOnEps) {
  assert(nMinIndex != nMaxIndex);
  assert(nMinIndex >= 0 && nMinIndex < LineSize());
  assert(nMaxIndex >= 0 && nMaxIndex < LineSize());
  TLinePair line_pair(nMinIndex, nMaxIndex);
  TColorCut color_cut(color, TEpsCut(dEps, bCutOnEps));
  m_vcPolygonLine.push_back(std::pair<TLinePair, TColorCut>(line_pair, color_cut));
}

int C2DGraphBase::LineSize() const { return m_vcGraphLine.count(); }

const C2DGraphLineBase &C2DGraphBase::Line(int nIndex) const { return *m_vcGraphLine.at(nIndex); }

C2DGraphLineBase &C2DGraphBase::Line(int nIndex) { return *m_vcGraphLine.at(nIndex); }

void C2DGraphBase::UpdateGraph() {
  if (MinX() >= MaxX()) {
    // assert(m_MinimumView < m_MaximumView);
    return;
  }

  RemoveAllObjects();

  UpdateGraphLayout();

  for (int i = 0; i < LineSize(); i++) {
    Line(i).Update();
  }

  for (size_t i = 0; i < m_vcPolygonLine.size(); i++) {
    FillPolygon(Line(m_vcPolygonLine[i].first.first), Line(m_vcPolygonLine[i].first.second),
                m_vcPolygonLine[i].second.first, m_vcPolygonLine[i].second.second.first,
                m_vcPolygonLine[i].second.second.second);
  }
}

C2DGraphBase::TColor C2DGraphBase::DragLineColor() const { return m_DragLineColor; }

void C2DGraphBase::DragLineColor(const TColor &Color) { m_DragLineColor = Color; }

void C2DGraphBase::EnableMarkers(bool bEnable) {
  if (m_bMarkerEnable != bEnable) {
    m_bMarkerEnable = bEnable;
    UpdateGraph();
  }
}

bool C2DGraphBase::MarkersEnabled() const { return m_bMarkerEnable; }

void C2DGraphBase::Clear() {
  while (!m_vcGraphLine.isEmpty())
    delete m_vcGraphLine.takeLast();
  m_vcPolygonLine.clear();
  RemoveAllObjects();
}

double C2DGraphBase::MaxX() const { return m_dMaxX; }

double C2DGraphBase::MinX() const { return m_dMinX; }

double C2DGraphBase::GridX() const { return m_dGridX; }

double C2DGraphBase::GridY() const { return m_dGridY; }

void C2DGraphBase::SetupProjectionMatrix() const {
  if (Parent() && MaxY().Valid() && MinY().Valid()) {
    const C2DGraphParentBase *pParent = dynamic_cast<const C2DGraphParentBase *>(Parent());
    assert(pParent);
    double left = -1, right = 1, bottom = -1, top = 1;
    // Switch on the XDirection
    switch (pParent->XDirection()) {
    case C2DGraphParentBase::LeftRight:
      left = MinX();
      right = MaxX();
      break;
    case C2DGraphParentBase::RightLeft:
      right = MinX();
      left = MaxX();
      break;
    case C2DGraphParentBase::TopBottom:
      top = MinX();
      bottom = MaxX();
      break;
    case C2DGraphParentBase::BottomTop:
      bottom = MinX();
      top = MaxX();
      break;
    default:
      assert(false);
    }

    // Switch on the YDirection
    switch (pParent->YDirection()) {
    case C2DGraphParentBase::LeftRight:
      left = MinY().Value();
      right = MaxY().Value();
      break;
    case C2DGraphParentBase::RightLeft:
      right = MinY().Value();
      left = MaxY().Value();
      break;
    case C2DGraphParentBase::TopBottom:
      top = MinY().Value();
      bottom = MaxY().Value();
      break;
    case C2DGraphParentBase::BottomTop:
      bottom = MinY().Value();
      top = MaxY().Value();
      break;
    default:
      assert(false);
    }
    //		const C2DGraphParentBase* pParent = dynamic_cast<const C2DGraphParentBase*>(Parent());
    //		assert(pParent);

    // X and Y are switched
    if ((pParent->XDirection() == C2DGraphParentBase::TopBottom) ||
        (pParent->XDirection() == C2DGraphParentBase::BottomTop)) {
      assert((pParent->YDirection() == C2DGraphParentBase::LeftRight) ||
             (pParent->YDirection() == C2DGraphParentBase::RightLeft));

      // Switch X and Y
      glRotated(180, 1, 1, 0);

      if (pParent->XDirection() == C2DGraphParentBase::TopBottom) {
        if (pParent->YDirection() == C2DGraphParentBase::LeftRight)
          glOrtho(MaxX(), MinX(), MinY().Value(), MaxY().Value(), -1, 1); // wjrx mantis 2621
        else
          glOrtho(MaxX(), MinX(), MaxY().Value(), MinY().Value(), -1, 1); // wjrx mantis 2621
      }

      if (pParent->XDirection() == C2DGraphParentBase::BottomTop) {
        if (pParent->YDirection() == C2DGraphParentBase::LeftRight)
          glOrtho(MinX(), MaxX(), MinY().Value(), MaxY().Value(), -1, 1); // wjrx mantis 2621
        else
          glOrtho(MinX(), MaxX(), MaxY().Value(), MinY().Value(), -1, 1); // wjrx mantis 2621
      }

    } else
      glOrtho(left, right, bottom, top, -1, 1); // wjrx mantis 2621
  } else
    IOpenGLFrame::SetupProjectionMatrix();
}

void C2DGraphBase::SetupModelMatrix() const { IOpenGLFrame::SetupModelMatrix(); }

// wjrx mantis 2559
bool C2DGraphBase::AutoX() const { return m_bAutoX; }

// wjrx mantis 2559
bool C2DGraphBase::AutoY() const { return m_bAutoY; }

// wjrx mantis 2559
void C2DGraphBase::AutoX(bool bAutoX) { m_bAutoX = bAutoX; }

// wjrx mantis 2559
void C2DGraphBase::AutoY(bool bAutoY) { m_bAutoY = bAutoY; }

// wjrx mantis 2559
void C2DGraphBase::SetMinMaxY(const geo::CValue &dMinY, const geo::CValue &dMaxY) {
  m_dMinY = dMinY;
  m_dMaxY = dMaxY;
}

void C2DGraphBase::SetMinMaxX(const double &dMinX, const double &dMaxX) {
  m_dMinX = dMinX;
  m_dMaxX = dMaxX;
}

void C2DGraphBase::ResetMinMaxX() {
  double dMin = DBL_MAX;
  double dMax = -DBL_MAX;
  for (int i = 0; i < LineSize(); i++) {
    for (int j = 0; j < Line(i).PointSize(); j++) {
      if (dMin > Line(i).X(j))
        dMin = Line(i).X(j);
      if (dMax < Line(i).X(j))
        dMax = Line(i).X(j);
    }
  }

  SetMinMaxX(dMin, dMax);
}

geo::CValue C2DGraphBase::MaxY() const { return m_dMaxY; }

geo::CValue C2DGraphBase::MinY() const { return m_dMinY; }

class CEpsLess {
  double m_eps;

public:
  CEpsLess() : m_eps(EPS) {}
  CEpsLess(double dEps) : m_eps(dEps) {}
  bool operator()(const double &d1, const double &d2) const { return (d1 - d2) < -m_eps; }
};

void C2DGraphBase::FillPolygon(const C2DGraphLineBase::TSerie &first_serie,
                               const C2DGraphLineBase::TSerie &second_serie, TColor color, const double &dEps,
                               bool bCutOnEps) {
  enum eLineMode { INVALID, GREATER, EQUAL, LESS };
  typedef std::pair<C2DGraphLineBase::TSerie, C2DGraphLineBase::TSerie> TPoint;
  typedef std::map<double, TPoint, CEpsLess> TPointMap;
  CEpsLess el(dEps);
  TPointMap mpPoint(el);

  // Add first and second line to set
  for (size_t i = 0; i < first_serie.size(); i++) {
    TPointMap::value_type vt(first_serie[i].first.X(), TPoint());
    TPointMap::iterator it = mpPoint.insert(vt).first;
    it->second.first.push_back(first_serie[i]);
  }
  for (size_t i = 0; i < second_serie.size(); i++) {
    TPointMap::iterator it = mpPoint.insert(TPointMap::value_type(second_serie[i].first.X(), TPoint())).first;
    it->second.second.push_back(second_serie[i]);
  }

  // Interpolate values
  TPointMap::iterator first_prev_it = mpPoint.end();
  TPointMap::iterator second_prev_it = mpPoint.end();
  TPointMap::iterator cur_it;
  for (cur_it = mpPoint.begin(); cur_it != mpPoint.end(); cur_it++) {
    // Look for first serie
    if (cur_it->second.first.size() == 0) {
      // Search for next value
      TPointMap::iterator next_it = cur_it;
      while ((next_it != mpPoint.end()) && (next_it->second.first.size() == 0))
        next_it++;
      if ((first_prev_it != mpPoint.end()) && (next_it != mpPoint.end())) {
        double Y_next = next_it->second.first[0].first.Y();
        double Y_prev = first_prev_it->second.first[first_prev_it->second.first.size() - 1].first.Y();
        double DeltaY = Y_next - Y_prev;
        double DeltaX = next_it->first - first_prev_it->first;
        assert(fabs(DeltaX) >= EPS);
        geo::CPoint point(cur_it->first, (DeltaY / DeltaX) * (cur_it->first - first_prev_it->first) + Y_prev);
        cur_it->second.first.push_back(C2DGraphLineBase::TGraphPoint(point, -1));
      }
    } else
      first_prev_it = cur_it;

    // Look for second serie
    if (cur_it->second.second.size() == 0) {
      // Search for next value
      TPointMap::iterator next_it = cur_it;
      while ((next_it != mpPoint.end()) && (next_it->second.second.size() == 0))
        next_it++;
      if ((second_prev_it != mpPoint.end()) && (next_it != mpPoint.end())) {
        double Y_next = next_it->second.second[0].first.Y();
        double Y_prev = second_prev_it->second.second[second_prev_it->second.second.size() - 1].first.Y();
        double DeltaY = Y_next - Y_prev;
        double DeltaX = next_it->first - second_prev_it->first;
        assert(fabs(DeltaX) >= EPS);
        geo::CPoint point(cur_it->first, (DeltaY / DeltaX) * (cur_it->first - second_prev_it->first) + Y_prev);
        cur_it->second.second.push_back(C2DGraphLineBase::TGraphPoint(point, -1));
      }
    } else
      second_prev_it = cur_it;
  }

  // Search for polygon's
  CDisplayPair *pNewPair = 0;

  // Add cross points
  eLineMode line_mode = INVALID;
  geo::CArray<geo::CPoint> vcFirst;
  geo::CArray<geo::CPoint> vcSecond;
  double prev_x;
  double prev_first_value, prev_second_value;
  for (TPointMap::iterator it = mpPoint.begin(); it != mpPoint.end(); it++) {
    bool bTerminatePolygon = true;
    bool bCalculateIntersection = false;
    if ((it->second.first.size() > 0) && (it->second.second.size() > 0)) {
      // Take the first and last and look for eqality
      double y_first_begin = it->second.first[0].first.Y();
      double y_first_end = it->second.first[it->second.first.size() - 1].first.Y();
      double y_second_begin = it->second.second[0].first.Y();
      double y_second_end = it->second.second[it->second.second.size() - 1].first.Y();

      if (y_first_begin < y_second_begin) {
        // LESS // BEGIN SECTION
        if (line_mode == GREATER)
          bCalculateIntersection = true;
        line_mode = LESS;
      } else if (y_first_begin > y_second_begin) {
        // GREATER // BEGIN SECTION
        vcFirst.PushBack(it->second.first[0].first);
        vcSecond.PushBack(it->second.second[0].first);
        // Is there a 'cut'?
        if ((y_first_end > y_second_begin) || !bCutOnEps) //&& (line_mode == GREATER))
        {
          bTerminatePolygon = false; // Altijd behalve cut
        }
        if (line_mode == LESS)
          bCalculateIntersection = true;
        if (line_mode == EQUAL) {
          geo::CPoint prv(prev_x, prev_first_value);
          vcSecond.PushBack(prv);
        }
        line_mode = GREATER;

      } else {
        // EQUAL // BEGIN SECTION
        if (line_mode == GREATER)
          vcSecond.PushBack(it->second.second[0].first);
        line_mode = EQUAL;
      }

      if (bCalculateIntersection) {
        geo::CLine A(geo::CPoint(prev_x, prev_first_value), it->second.first[0].first);
        geo::CLine B(geo::CPoint(prev_x, prev_second_value), it->second.second[0].first);
        geo::CPoint ptIntersec = A.Intersection(B);
        assert(!ptIntersec.Empty());
        vcSecond.PushBack(ptIntersec);
      }

      if (bTerminatePolygon) {
        if ((vcFirst.Size() + vcSecond.Size()) > 2) {
          for (int i = vcFirst.Size() - 1; i != -1; i--)
            vcSecond.PushBack(vcFirst.Object(i));
          if (!pNewPair)
            pNewPair = CreateNewPair();
          pNewPair->AddObject(*(new geo::CPolygon(vcSecond)));
          if (bCutOnEps)
            goto EINDE;
        }
        vcSecond.Clear();
        vcFirst.Clear();
      }

      bTerminatePolygon = true;
      if (y_first_end < y_second_end) {
        // LESS	// END SECTION
        if (line_mode == GREATER)
          bTerminatePolygon = true;
        line_mode = LESS;

      } else if (y_first_end > y_second_end) {
        // GREATER // END SECTION
        for (size_t i = 1; i < it->second.first.size(); i++)
          vcFirst.PushBack(it->second.first[i].first);
        for (size_t i = 1; i < it->second.second.size(); i++)
          vcSecond.PushBack(it->second.second[i].first);
        bTerminatePolygon = false;
        line_mode = GREATER;

      } else {
        // EQUAL // END SECTION
        if (line_mode == LESS)
          bTerminatePolygon = false;
        vcSecond.PushBack(it->second.second[it->second.second.size() - 1].first);
        line_mode = EQUAL;
      }
      prev_first_value = y_first_end;
      prev_second_value = y_second_end;
    } else
      line_mode = INVALID;

    prev_x = it->first;

    TPointMap::const_iterator it_end = it;
    it_end++;

    if ((bTerminatePolygon || it_end == mpPoint.end()) && ((vcFirst.Size() + vcSecond.Size()) > 2)) {
      for (int i = vcFirst.Size() - 1; i != -1; i--)
        vcSecond.PushBack(vcFirst.Object(i));
      if (!pNewPair)
        pNewPair = CreateNewPair();
      pNewPair->AddObject(*(new geo::CPolygon(vcSecond)));
      if (bCutOnEps)
        goto EINDE;
    }

    if (bTerminatePolygon) {
      vcSecond.Clear();
      vcFirst.Clear();
    }
  }
EINDE:
  if (pNewPair) {
    pNewPair->DrawDef().Visible(TRUE);
    pNewPair->DrawDef().PolyFillBack(TRUE);
    pNewPair->DrawDef().PolyFillFront(TRUE);
    pNewPair->DrawDef().SetFixedColor(color);
    pNewPair->DrawDef().SetDepthLayer2D(GRAPH_LAYER_GRID_TOP);
    pNewPair->Update();
  }
}
void C2DGraphBase::FillPolygon(const C2DGraphLineBase &min_line, const C2DGraphLineBase &max_line, TColor color,
                               const double &dEps, bool bCutOnEps) {
  C2DGraphLineBase::TSerieVec first_serie = min_line.GenerateValues();
  C2DGraphLineBase::TSerieVec second_serie = max_line.GenerateValues();
  for (size_t nFirst = 0; nFirst < first_serie.size(); nFirst++) {
    for (size_t nSecond = 0; nSecond < second_serie.size(); nSecond++) {
      FillPolygon(first_serie[nFirst], second_serie[nSecond], color, dEps, bCutOnEps);
    }
  }
}

void C2DGraphBase::EnableInvalidSections(bool bEnable) { m_bInvalidSectionEnable = bEnable; }

bool C2DGraphBase::InvalidateSections() const { return m_bInvalidSectionEnable; }

C2DGraphBase::TColor C2DGraphBase::InvalidSectionColor() const { return m_InvalidSectionColor; }

void C2DGraphBase::InvalidSectionColor(TColor color) { m_InvalidSectionColor = color; }

void C2DGraphBase::UpdateGraphLayout(bool /*bEditLimits*/) {
  // wjrx mantis 3383
  if (m_nDivisionX < 2)
    m_nDivisionX = 2;
  m_dGridX = (MaxX() - MinX()) / (m_nDivisionX - 1);

  /* wjrx mantis 3383 taken out
  if (bEditLimits)
  {
    m_dGridX = (MaxX() - MinX()) / m_nDivisionX ;
    DefineAxis(&m_dMinX, &m_dMaxX, &m_dGridX, m_nDivisionX);
  }
  */

  // We first calculate the min and max for the Y.
  if (m_bAutoY) // wjrx mantis 2559
  {
    m_dMaxY = geo::CValue();
    m_dMinY = geo::CValue();

    for (int i = 0; i < LineSize(); i++) {
      C2DGraphLineBase::TSerieVec vcSerie = Line(i).GenerateValues(false);
      for (size_t nSerie = 0; nSerie < vcSerie.size(); nSerie++) {
        for (size_t nPoint = 0; nPoint < vcSerie[nSerie].size(); nPoint++) {
          m_dMaxY = m_dMaxY.Max(geo::CValue(vcSerie[nSerie][nPoint].first.Y()));
          m_dMinY = m_dMinY.Min(geo::CValue(vcSerie[nSerie][nPoint].first.Y()));
        }
      }
    }

    if (!m_dMinY.Valid())
      m_dMinY.Value(0);
    if (!m_dMaxY.Valid())
      m_dMaxY.Value(0);

    // wjrx mantis 3441
    //
    if (fabs(m_dMinY.Value() - m_dMaxY.Value()) < 1e-4) {
      if (fabs(m_dMinY.Value()) < 1e-4) {
        m_dMinY = -1;
        m_dMaxY = 1;
      } else if (m_dMinY.Value() < 0) {
        m_dMinY = m_dMinY * 1.01;
        m_dMaxY = m_dMaxY * 0.99;
      } else {
        m_dMinY = m_dMinY * 0.99;
        m_dMaxY = m_dMaxY * 1.01;
      }
    }
  }

  // Create min and max of Y-axis
  // wjrx mantis 3383
  if (m_nDivisionY < 2)
    m_nDivisionY = 2;
  m_dGridY = (MaxY().Value() - MinY().Value()) / (m_nDivisionY - 1);

  /* wjrx mantis 3383 taken out
  double dMinY(m_dMinY.Value()), dMaxY(m_dMaxY.Value());
  DefineAxis(&dMinY, &dMaxY, &m_dGridY, m_nDivisionY);
  m_dMinY = geo::CValue(dMinY);
  m_dMaxY = geo::CValue(dMaxY);
  */

  // update the grid
  UpdateGrid();

  // Update polygons
  for (size_t i = 0; i < m_vcPolygonLine.size(); i++)
    FillPolygon(Line(m_vcPolygonLine[i].first.first), Line(m_vcPolygonLine[i].first.second),
                m_vcPolygonLine[i].second.first, m_vcPolygonLine[i].second.second.first,
                m_vcPolygonLine[i].second.second.second);
}

void C2DGraphBase::UpdateGrid() {

  if (MaxY().Valid() && MinY().Valid()) {
    CDisplayPair *NewPair = CreateNewPair();
    geo::CLine *pLine = NULL;
    double dValue;
    QString strValue;

    // Gridlines Y values
    // wjrx mantis 2664
    int Count = int(0.5 + (MaxY().Value() - MinY().Value()) / m_dGridY);
    int i;
    for (i = 0; i < Count + 1; i++) {
      dValue = MinY().Value() + (m_dGridY * i);

      geo::CPoint ptStart(MinX(), dValue);
      geo::CPoint ptEnd(MaxX(), dValue);

      pLine = new geo::CLine(ptStart, ptEnd);

      NewPair->AddObject(*pLine);
    }

    // gridlines X values
    Count = int((MaxX() - MinX()) / m_dGridX);

    for (i = 0; i < Count + 1; i++) {
      dValue = MinX() + (m_dGridX * i);

      geo::CPoint Start(dValue, MinY().Value());
      geo::CPoint End(dValue, MaxY().Value());

      pLine = new geo::CLine(Start, End);
      NewPair->AddObject(*pLine);
    }

    NewPair->DrawDef().Visible(TRUE);
    NewPair->DrawDef().SetFixedColor(GetGraphGridColor());
    NewPair->DrawDef().LineStipple(TRUE);
    NewPair->DrawDef().LineWidth(1);
    NewPair->DrawDef().PointSize(2);
    //		NewPair->DrawDef().DepthFunc(GL_ALWAYS);
    NewPair->DrawDef().SetDepthLayer2D(GRAPH_LAYER_GRID_BOTTOM);
    NewPair->Update();
  }
}

bool C2DGraphBase::MousePress(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  if (button == Qt::LeftButton) {
    m_ptStartDrag = UnProject(point);
    m_ptPendingDrag = UnProject(point);
  }

  return true;
}

//////////////////////////////////////////////////////////////////////

bool C2DGraphBase::MouseMove(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  if ((button == Qt::LeftButton) && !m_ptStartDrag.Empty()) {
    m_ptPendingDrag = UnProject(point);
    UpdateFrame();
    return true;
  }

  m_ptStartDrag = geo::CPoint();
  m_ptPendingDrag = geo::CPoint();

  geo::CPoint t_point = UnProject(point);

  std::pair<int, int> prHit = MarkerHitTest(t_point.X(), t_point.Y());

  if (prHit == m_Label.second)
    return false;

  if (m_Label.first) {
    RemoveObject(m_Label.first);
    m_Label.first = 0;
  }

  if (prHit.first > -1) {
    CMarker *pMarker = Line(prHit.first).CreateMarker(prHit.second);

    // wjrx mantis 2355
    // if (the start point of the string + the length of the string)
    // extends beyond the viewport, move the start point back to fit
    // the string in the view port.
    //
    int deltaX = point.x() + QFontMetrics(QFont()).size(0, pMarker->Label()).width() - ViewPortWidth() + 5;   // pixels
    int deltaY = point.y() + QFontMetrics(QFont()).size(0, pMarker->Label()).height() - ViewPortHeight() + 5; // pixels

    if (deltaX > 0 || deltaY > 0) {
      int pX = deltaX > 0 ? point.x() - deltaX : point.x();
      int pY = deltaY > 0 ? point.y() - deltaY : point.y();

      // wjrx mantis 2372
      // If the moved label starts at < 0, move it to 0 to avoid clipping
      if (pX < 0)
        pX = 0;
      if (pY < 0)
        pY = 0;

      // transform the screen coordinates back to world coordinates
      //
      TScreenPoint myPoint(pX, pY); // pixels
      t_point = UnProject(myPoint); // world
    }

    m_Label.second = prHit;
    m_Label.first = CreateNewPair();
    m_Label.first->AddObject(*new CLabelPoint(t_point, pMarker->Label()));
    m_Label.first->DrawDef().Visible();
    m_Label.first->DrawDef().TextColor(GetGraphTextColor());
    m_Label.first->DrawDef().LabelBorder(TRUE);
    m_Label.first->DrawDef().RequireCompile(FALSE);
    m_Label.first->DrawDef().SetDepthLayer2D(GRAPH_LAYER_FOREGROUND);
    m_Label.first->Update();
    delete pMarker;
  } else
    m_Label.second = std::pair<int, int>(-1, -1);
  return true;
}

//////////////////////////////////////////////////////////////////////
bool C2DGraphBase::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint & /*point*/) {
  if (m_ptStartDrag.Empty() || m_ptPendingDrag.Empty())
    return true;

  // Remove painting from screen, but the data (GraphLines) are still there
  RemoveAllObjects();

  // Transform incoming screen points
  geo::CPoint ptStart(m_ptStartDrag);
  geo::CPoint ptEnd(m_ptPendingDrag);

  m_ptStartDrag = geo::CPoint();
  m_ptPendingDrag = geo::CPoint();

  // Set MinX and MaxX
  if (fabs(ptStart.X() - ptEnd.X()) < EPS)
    return true;

  if (ptStart.X() < ptEnd.X())
    SetMinMaxX(ptStart.X(), ptEnd.X());
  else
    SetMinMaxX(ptEnd.X(), ptStart.X());

  C2DGraphParentBase *pParent = dynamic_cast<C2DGraphParentBase *>(Parent());
  assert(pParent);
  pParent->UpdateGraph();

  return true;
}

std::pair<int, int> C2DGraphBase::MarkerHitTest(const double &dX, const double &dY) const {
  if (MarkersEnabled() && MaxY().Valid() && MinY().Valid()) {
    double dEPSX = (MaxX() - MinX()) / 100;
    double dEPSY = (MaxY().Value() - MinY().Value()) / 100;
    for (int iLine = 0; iLine < LineSize(); iLine++) {
      for (int iPoint = 0; iPoint < Line(iLine).PointSize(); iPoint++) {
        geo::CValue value = Line(iLine).Y(iPoint);
        if (value.Valid()) {
          if ((fabs(dY - value.Value()) < dEPSY) && (fabs(dX - Line(iLine).X(iPoint)) < dEPSX)) {
            return std::pair<int, int>(iLine, iPoint);
          }
        }
      }
    }
  }

  return std::pair<int, int>(-1, -1);
}
//////////////////////////////////////////////////////////////////////

bool C2DGraphBase::MouseWheel(TKeyboardModifiers /*state*/, int nDelta, const TScreenPoint & /*point*/) {
  int Increment = nDelta / 120;

  double dMaxX = MinX();
  double dMinX = MaxX();

  // retriev the limits of the X serie
  double dMinimum = DBL_MAX;
  double dMaximum = -DBL_MAX;

  for (int i = 0; i < LineSize(); i++) {
    for (int nPoint = 0; nPoint < Line(i).PointSize(); nPoint++) {
      dMaximum = std::max(Line(i).X(nPoint), dMaximum);
      dMinimum = std::min(Line(i).X(nPoint), dMinimum);
    }
  }

  while (dMaxX < dMaximum) {
    dMaxX += m_dGridX;
  }

  while (dMinX > dMinimum) {
    dMinX -= m_dGridX;
  }

  m_dMinX -= (double)Increment * m_dGridX;
  m_dMaxX -= (double)Increment * m_dGridX;

  if (!(MaxX() <= dMaxX && MinX() >= dMinX)) {
    m_dMinX += (double)Increment * m_dGridX;
    m_dMaxX += (double)Increment * m_dGridX;
    return true;
  }

  RemoveAllObjects();
  //	m_parent.UpdateScroll();
  return true;
}

//////////////////////////////////////////////////////////////////////

void C2DGraphBase::DefineAxis(double *Min, double *Max, double *Inc, int MinTicks) {
  // check params
  assert(Min != 0);
  assert(Max != 0);
  assert(Inc != 0);
  if (MinTicks <= 1)
    MinTicks = 2;
  unsigned int intervals = MinTicks - 1;

  // our local versions
  double myMin, myMax, myInc;

  if (*Max > *Min) {
    myMax = *Max;
    myMin = *Min;
  } else if (*Min > *Max) {
    // We only handle Max > Min
    // Reverse Min and Max
    myMax = *Min;
    myMin = *Max;
  } else // *Min == *Max
  {
    myMax = *Max * 1.01;
    myMin = *Min * 0.99;
  }

  // How to get from Min to Max
  myInc = (myMax - myMin) / intervals;

  // normInc: myInc normalized to range 1-10
  double normInc;

  // myInc= 2.3   normInc= ( 2.3   / pow(10,1-1) )
  // myInc= 0.23  normInc= ( 0.23  / pow(10,0-1) )
  // myInc= 0.023 normInc= ( 0.023 / pow(10,-1-1) )
  //
  double incPower = ceil(log10(myInc)) - 1;
  normInc = myInc / pow(10.0, incPower);

  // "Round" myInc
  // We want nice short numbers on our axis
  // myInc= 3.714 -> 4
  //
  myInc = 1;
  if (normInc > myInc)
    myInc = 2;
  if (normInc > myInc)
    myInc = 3;
  if (normInc > myInc)
    myInc = 4;
  if (normInc > myInc)
    myInc = 5;
  if (normInc > myInc)
    myInc = 6;
  if (normInc > myInc)
    myInc = 7;
  if (normInc > myInc)
    myInc = 8;
  if (normInc > myInc)
    myInc = 9;
  if (normInc > myInc)
    myInc = 10;

  //"denormalize"
  myInc = myInc * pow(10.0, incPower);

  // Do the same rounding to myMin and myMax to create the nice round numbers
  //
  myMin = myInc * floor(myMin / myInc);
  double tmpMax1 = myInc * ceil(myMax / myInc);

  // tmpMax1 is not always correct, we must make sure we have
  // 'intervals' intervals.
  // Example: Min= 16.6 Max= 17.2 MinTicks=5:
  //           => myInc=0.2 myMin= 16.6 myMax= 17.2 intervals=4
  // Going from myMin to myMax only requires 3 intervals, not 4!
  //
  double tmpMax2 = myMin + intervals * myInc;
  myMax = tmpMax1 > tmpMax2 ? tmpMax1 : tmpMax2;

  if (*Max < *Min) {
    // reverse Min and Max back
    *Max = myMin;
    *Min = myMax;
    *Inc = -myInc;
  } else {
    *Max = myMax;
    *Min = myMin;
    *Inc = myInc;
  }
}

C2DGraphBase::TColor C2DGraphBase::GetGraphBorderColor() const { return qRgb(0, 250, 0); }

C2DGraphBase::TColor C2DGraphBase::GetGraphGridColor() const { return qRgb(150, 150, 150); }

C2DGraphBase::TColor C2DGraphBase::GetGraphTextColor() const { return qRgb(255, 255, 255); }
