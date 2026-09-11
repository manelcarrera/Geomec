/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// #include "dimple.h"
#include "LegendFrameBase.h"
#include "ColorScale.h"
#include "ColorScaleDrawDef.h"
#include "DrawDef.h"
#include "IDrawDef.h"
#include "Line.h"
#include "Rectangle.h"
#include <algorithm>

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// Base class of a legendframe. All objects in the legend frame are legend
// frames and are generated in the function void UpdateLegendeFrame()
/////////////////////////////////////////////////////////////////////////////
CLegendFrame::legend_square::legend_square(CLegendFrame &frame) : m_frame(frame) {
  m_frame.m_vcFrame.push_back(this);
  m_frame.m_nHeight = -1;
  m_frame.m_nWidth = -1;
}

CLegendFrame::legend_square::~legend_square() {}

CLegendFrame &CLegendFrame::legend_square::frame() { return m_frame; }

/////////////////////////////////////////////////////////////////////////////
// Implementation of legend frame separator
/////////////////////////////////////////////////////////////////////////////
CLegendFrame::legend_separator::legend_separator(CLegendFrame &frame, int space)
    : legend_square(frame), m_space(space) {}

int CLegendFrame::legend_separator::size_x() { return 0; }

int CLegendFrame::legend_separator::size_y() { return m_space; }

void CLegendFrame::legend_separator::render(int /*pos_x*/, int /*pos_y*/) {
  // Do nothing ....
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of legendframe text label
/////////////////////////////////////////////////////////////////////////////
CLegendFrame::legend_label::legend_label(CLegendFrame &frame, const QString &text)
    : legend_square(frame), m_label(0, 0, 0, text, QFont(), CLabelPoint::LA_TOP_LEFT) {}

int CLegendFrame::legend_label::size_x() { return QFontMetrics(m_label.font()).size(0, m_label.Label()).width(); }

int CLegendFrame::legend_label::size_y() { return QFontMetrics(m_label.font()).size(0, m_label.Label()).height(); }

void CLegendFrame::legend_label::render(int pos_x, int pos_y) {
  m_label.SetPoint(pos_x, pos_y);
  frame().DrawObject(m_label, CDrawDef(qRgb(255, 255, 255)));
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of the legend scale
/////////////////////////////////////////////////////////////////////////////

// draws the colorscale rectangle...
void CLegendFrame::legend_scale::render(int pos_x, int pos_y) {
  render_scale(pos_x, pos_y);
  render_labels(pos_x, pos_y);
}

int CLegendFrame::legend_scale::size_x() {
  int ret = m_size_x;
  for (int i = 0; i <= m_divisions; i++) {
    // Display labels
    QString label("%1");
    label.arg(((max_val() - min_val()) / m_divisions) * i + min_val());

    CLabelPoint scalelabel(m_size_x + 3, i * (m_size_y / m_divisions), 0, label, QFont(), CLabelPoint::LA_TOP_LEFT);
    int w = QFontMetrics(QFont()).size(0, label).width();
    ret = std::max(ret, m_size_x + w + 3);
  }

  return ret;
}

int CLegendFrame::legend_scale::size_y() { return m_size_y; }

void CLegendFrame::legend_scale::render_scale(int pos_x, int pos_y) {
  DIA_ASSERT(m_color.size() > 0);
  // Create size - 1 labels
  TColorMap::iterator it = m_color.begin();
  for (size_t i = 0; (i + 1) < m_color.size(); i++) {
    // Take top and bottom from iterator
    TColorMap::value_type &top_color = *it;
    it++;
    TColorMap::value_type &bottom_color = *it;

    // Calc top and bottom for rectangle
    double range = max_val() - min_val();
    double fraction = m_size_y / range;
    int top = int((top_color.first - min_val()) * fraction) + pos_y;
    int bottom = int((bottom_color.first - min_val()) * fraction) + pos_y;

    // Create a drawdef for the rectangle
    CColorScaleDrawDef color_scale(top, bottom, top_color.second, bottom_color.second);
    color_scale.ColorScaleDirection(CColorScaleDrawDef::CS_Y_DIRECTION);

    // Create the
    frame().DrawObject(geo::CRectangle(geo::CPoint(pos_x, top, 0), geo::CPoint(pos_x + m_size_x, bottom, 0)),
                       color_scale);
  }
}

CLegendFrame::legend_scale::legend_scale(CLegendFrame &frame, const TColorMap &color, int divisions, int size_x,
                                         int size_y)
    : legend_square(frame), m_color(color), m_size_x(size_x), m_size_y(size_y), m_divisions(divisions) {}

double CLegendFrame::legend_scale::max_val() const {
  DIA_ASSERT(m_color.size() > 1);
  TColorMap::const_iterator it = m_color.end();
  it--;
  return it->first;
}

double CLegendFrame::legend_scale::min_val() const {
  DIA_ASSERT(m_color.size() > 1);
  return m_color.begin()->first;
}

void CLegendFrame::legend_scale::render_labels(int pos_x, int pos_y) {
  // Draw label over the scene
  for (int i = 0; i <= m_divisions; i++) {
    // Draw the line
    geo::CPoint p1(pos_x, pos_y + (m_size_y / m_divisions) * i, 0);
    geo::CPoint p2(pos_x + m_size_x, pos_y + (m_size_y / m_divisions) * i, 0);
    frame().DrawObject(geo::CLine(p1, p2), CDrawDef(qRgb(0, 0, 0)));

    // Display labels
    QString label = QString("%1").arg(((max_val() - min_val()) / m_divisions) * i + min_val());

    CLabelPoint scalelabel(0, 0, 0, label, QFont(), CLabelPoint::LA_TOP_LEFT);
    int h = QFontMetrics(QFont()).size(0, label).height();
    scalelabel.SetPoint(pos_x + m_size_x + 3, pos_y + i * (m_size_y / m_divisions) - 0.7 * h);
    frame().DrawObject(scalelabel, CDrawDef(qRgb(255, 255, 255)));
  }
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of the legend index
/////////////////////////////////////////////////////////////////////////////
CLegendFrame::legend_index::legend_index(CLegendFrame &frame, int size_x, int size_y, TColor color, const QString &text)
    : legend_square(frame), m_label(0, 0, 0, text, QFont(), CLabelPoint::LA_TOP_LEFT), m_size_x(size_x),
      m_size_y(size_y), m_color(color) {}

int CLegendFrame::legend_index::size_x() {
  int w = QFontMetrics(m_label.font()).size(0, m_label.Label()).width();
  return w + m_size_x + 5;
}

int CLegendFrame::legend_index::size_y() {
  int h = QFontMetrics(m_label.font()).size(0, m_label.Label()).height();
  if (h < m_size_y)
    return m_size_y;

  return h;
}

void CLegendFrame::legend_index::render(int pos_x, int pos_y) {
  // Draw rectangle
  geo::CRectangle rect(geo::CPoint(pos_x, pos_y), geo::CPoint(pos_x + m_size_x, pos_y + m_size_y));
  frame().DrawObject(rect, CDrawDef(m_color));

  // Draw label
  int h = QFontMetrics(m_label.font()).size(0, m_label.Label()).height();

  m_label.SetPoint(pos_x + m_size_x + 5, pos_y + (size_y() - h) / 2);
  frame().DrawObject(m_label, CDrawDef(qRgb(255, 255, 255)));
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of legend marker
/////////////////////////////////////////////////////////////////////////////

CLegendFrame::legend_marker::legend_marker(const CMarker &marker, CLegendFrame &frame)
    : CLegendFrame::legend_square(frame), m_marker(marker),
      m_Label(0, 0, 0, marker.Label(), QFont(), CLabelPoint::LA_TOP_LEFT) {}

int CLegendFrame::legend_marker::size_x() { return QFontMetrics(m_Label.font()).size(0, m_Label.Label()).width(); }

int CLegendFrame::legend_marker::size_y() { return QFontMetrics(m_Label.font()).size(0, m_Label.Label()).height(); }

void CLegendFrame::legend_marker::render(int pos_x, int pos_y) {
  int MarkerSize = 12;
  m_Label.SetPoint(pos_x + MarkerSize + 13, pos_y);
  CDrawDef dd(m_marker.Color());
  CDrawDef dd2(qRgb(255, 255, 255));
  // dd.RequireCompile(FALSE);
  // dd2.RequireCompile(FALSE);
  dd.PolyFillBack(m_marker.Fill() ? TRUE : FALSE);
  dd.PolyFillFront(m_marker.Fill() ? TRUE : FALSE);
  frame().DrawObject(m_Label, dd2);
  m_marker.Position(geo::CPoint(pos_x + 5, pos_y + 5));
  frame().DrawObject(m_marker, dd);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of legend symbol
/////////////////////////////////////////////////////////////////////////////

CLegendFrame::legend_symbol::legend_symbol(CLegendFrame &frame, ISymbol *symbol, TColor color, const QString &label)
    : CLegendFrame::legend_square(frame), m_symbol(symbol), m_color(color),
      m_label(0, 0, 0, label, QFont(), CLabelPoint::LA_TOP_LEFT) {}

CLegendFrame::legend_symbol::~legend_symbol() { delete m_symbol; }

const int labelOffset = 25;

int CLegendFrame::legend_symbol::size_x() {
  return labelOffset + QFontMetrics(m_label.font()).size(0, m_label.Label()).width();
}

int CLegendFrame::legend_symbol::size_y() { return QFontMetrics(m_label.font()).size(0, m_label.Label()).height(); }

void CLegendFrame::legend_symbol::render(int pos_x, int pos_y) {
  m_label.SetPoint(pos_x + labelOffset, pos_y);
  CDrawDef symboldd(m_color);
  CDrawDef textdd(qRgb(255, 255, 255));
  frame().DrawObject(m_label, textdd);
  geo::CVector delta(m_symbol->Position(0), geo::CPoint(pos_x + 5, pos_y + 5));
  m_symbol->Move(delta);
  frame().DrawObject(*m_symbol, symboldd);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of legend line frame
/////////////////////////////////////////////////////////////////////////////

CLegendFrame::legend_lineframe::legend_lineframe(CLegendFrame &frame, const QString &label, bool bStipple, TColor color)
    : legend_square(frame), m_Label(0, 0, 0, label, QFont(), CLabelPoint::LA_TOP_LEFT), m_bStipple(bStipple),
      m_Color(color) {}

int CLegendFrame::legend_lineframe::size_x() { return QFontMetrics(m_Label.font()).size(0, m_Label.Label()).width(); }

int CLegendFrame::legend_lineframe::size_y() { return QFontMetrics(m_Label.font()).size(0, m_Label.Label()).height(); }

void CLegendFrame::legend_lineframe::render(int pos_x, int pos_y) {
  m_Label.SetPoint(pos_x + 25, pos_y);

  CDrawDef dd2(qRgb(255, 255, 255));
  frame().DrawObject(m_Label, dd2);

  geo::CPoint p1(pos_x, pos_y + size_y() / 2);
  geo::CPoint p2(pos_x + 20, pos_y + size_y() / 2);

  geo::CLine l(p1, p2);
  CDrawDef dd(m_Color);
  dd.LineStipple(m_bStipple ? TRUE : FALSE);
  frame().DrawObject(l, dd);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of legend_tree_item
/////////////////////////////////////////////////////////////////////////////

CLegendFrame::legend_tree_item::legend_tree_item(const QString &sName, int offset, CLegendFrame &frame)
    : legend_square(frame), m_Label(0, 0, 0, sName, QFont(), CLabelPoint::LA_TOP_LEFT), m_offset(offset) {}

int CLegendFrame::legend_tree_item::size_x() {
  int w = QFontMetrics(m_Label.font()).size(0, m_Label.Label()).width();
  return m_offset + w + size_y();
}

int CLegendFrame::legend_tree_item::size_y() { return QFontMetrics(m_Label.font()).size(0, m_Label.Label()).height(); }

int CLegendFrame::legend_tree_item::child_position() { return m_offset + size_y(); }

void CLegendFrame::legend_tree_item::render(int pos_x, int pos_y) {
  geo::CPoint upper(pos_x + (size_y() / 2) + m_offset, pos_y);
  geo::CPoint middle(pos_x + (size_y() / 2) + m_offset, pos_y + (size_y() / 2));
  geo::CPoint lower(pos_x + size_y() + m_offset - 2, pos_y + (size_y() / 2));
  geo::CLine l1(upper, middle);
  geo::CLine l2(middle, lower);
  CDrawDef dd(qRgb(255, 255, 255));
  frame().DrawObject(l1, dd);
  frame().DrawObject(l2, dd);
  m_Label.SetPoint(pos_x + size_y() + m_offset, pos_y);
  frame().DrawObject(m_Label, dd);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of the legend frame base
/////////////////////////////////////////////////////////////////////////////

// ##ModelId=3B6549E202B1
CLegendFrame::CLegendFrame() : m_nWidth(-1), m_nHeight(-1), m_nScrollX(0), m_nScrollY(0) {}

// ##ModelId=3B6549E202B2
CLegendFrame::~CLegendFrame() {}

// Frame work handles for adding and clearing
void CLegendFrame::ClearLegendFrames() {
  for (size_t i = 0; i < m_vcFrame.size(); i++)
    delete m_vcFrame[i];

  m_vcFrame.clear();
  m_nWidth = -1;
  m_nHeight = -1;
}

// overriden from OpenGl, draws the scene...
void CLegendFrame::DrawScene() {
  int pos_y = OffSet_Y();

  // Draw scene
  for (size_t i = 0; i < m_vcFrame.size(); i++) {
    m_vcFrame[i]->render(OffSet_X() - m_nScrollX, pos_y - m_nScrollY);
    pos_y += m_vcFrame[i]->size_y();
  }
}

int CLegendFrame::ViewPortX() const {
  if (Parent())
    return Parent()->ViewPortX();
  return 0;
}

/*!
  The position of the legend is currently default in the topleft corner
  of its parent. Override this function if a custom position is needed.
*/
int CLegendFrame::ViewPortY() const {
  if (Parent()) {
    return Parent()->ViewPortY() + Parent()->ViewPortHeight() - ViewPortHeight();
  }
  return 0;
}

/*!
  The size of the legend is calculated from its contents.
*/
int CLegendFrame::ViewPortWidth() const {
  CalcWidthHeight();

  if (Parent()) {
    return std::max(Parent()->ViewPortWidth(), m_nWidth);
  }
  return m_nWidth;
}

/*!
  The size of the legend is calculated from its contents.
*/
int CLegendFrame::ViewPortHeight() const {
  CalcWidthHeight();

  if (Parent()) {
    return std::max(Parent()->ViewPortHeight(), m_nHeight);
  }
  return m_nHeight;
}

int CLegendFrame::LegendHeight() const {
  CalcWidthHeight();
  return m_nHeight;
}

int CLegendFrame::LegendWidth() const {
  CalcWidthHeight();
  return m_nWidth;
}

/*!
  Calculates the required amount of space for all items inclusive a
  margin of size OffSet_X, OffSet_Y.
*/
void CLegendFrame::CalcWidthHeight() const {
  if ((m_nWidth == -1) || (m_nWidth == -1)) {
    if (m_vcFrame.size() == 0) {
      m_nWidth = 0;
      m_nHeight = 0;
    } else {
      m_nWidth = m_vcFrame[0]->size_x();
      m_nHeight = m_vcFrame[0]->size_y();
      for (size_t i = 1; i < m_vcFrame.size(); i++) {
        m_nWidth = std::max(m_nWidth, m_vcFrame[i]->size_x());
        m_nHeight += m_vcFrame[i]->size_y();
      }
      m_nWidth += 2 * OffSet_X();
      m_nHeight += 2 * OffSet_Y();
    }
  }
}

int CLegendFrame::ScrollX() const { return m_nScrollX; }

int CLegendFrame::ScrollY() const { return m_nScrollY; }

void CLegendFrame::setScrollX(int sx) { m_nScrollX = sx; }

void CLegendFrame::setScrollY(int sy) { m_nScrollY = sy; }

int CLegendFrame::OffSet_X() const { return 5; }

int CLegendFrame::OffSet_Y() const { return 5; }

void CLegendFrame::SetupProjectionMatrix() const {
  int w = ViewPortWidth();
  if (w <= 0)
    return;
  int h = ViewPortHeight();
  if (h <= 0)
    return;

  VERIFY(MakeCurrent());

  glOrtho(0, ViewPortWidth(), ViewPortHeight(), 0, 0, 1);
}
