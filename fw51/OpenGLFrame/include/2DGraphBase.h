/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/*                                         Copyright (c) 2007 TNO DIANA BV */
#if !defined(OPENGL_2DGRAPHBASE_H__INCLUDED_)
#define OPENGL_2DGRAPHBASE_H__INCLUDED_

#include "2DGraphLineBase.h"
#include "DrawDef.h"
#include "IOpenGLFrame.h"
#include <QList>

#include "OpenGLFrameExports.h"

class C2DGraphLineBase;
class C2DGraphParentBase;

class OPENGLFRAME_EXPORT C2DGraphBase : public IOpenGLFrame {
public:
  class OPENGLFRAME_EXPORT CDisplayPair {
    C2DGraphBase &m_parent;
    geo::CPtrArray<geo::IObject> m_array;
    CDrawDef m_drawdef;

  public:
    // construction / destruction
    CDisplayPair(C2DGraphBase &parent);
    virtual ~CDisplayPair();

    // interface
    geo::CPtrArray<geo::IObject> &Array();
    CDrawDef &DrawDef();
    void AddObject(geo::IObject &Object);
    void Update();
  };
  friend class C2DGraphLineBase;
  QList<C2DGraphLineBase *> m_vcGraphLine;
  double m_dMinX, m_dMaxX;
  geo::CValue m_dMinY, m_dMaxY;
  double m_dGridX, m_dGridY;
  int m_nDivisionX, m_nDivisionY;
  bool m_bAutoX; // wjrx mantis 2559
  bool m_bAutoY; // wjrx mantis 2559
  geo::CPoint m_ptStartDrag;
  geo::CPoint m_ptPendingDrag;
  typedef std::pair<int, int> TLinePair;
  typedef std::pair<double, bool> TEpsCut;
  typedef std::pair<TColor, TEpsCut> TColorCut;
  std::vector<std::pair<TLinePair, TColorCut>> m_vcPolygonLine;
  TColor m_DragLineColor;
  bool m_bMarkerEnable;
  TColor m_InvalidSectionColor;
  bool m_bInvalidSectionEnable;
  std::pair<CDisplayPair *, std::pair<int, int>> m_Label;
  void DefineAxis(double *Min, double *Max, double *Inc, int MinTicks);
  std::pair<int, int> MarkerHitTest(const double &dX, const double &dY) const;
  void FillPolygon(const C2DGraphLineBase &min_line, const C2DGraphLineBase &max_line, TColor color, const double &dEps,
                   bool bCutOnEps);
  void FillPolygon(const C2DGraphLineBase::TSerie &first, const C2DGraphLineBase::TSerie &second, TColor color,
                   const double &dEps, bool bCutOnEps);
  typedef std::map<CMarker *, std::pair<int, int>> TMarkerMap;
  TMarkerMap m_mpMarker;
  typedef std::map<CDisplayPair *, int> TDisplayMap;
  TDisplayMap m_mpDisplay;
  // Paint handlers
  // construction destruction
  void Update(CDisplayPair *pObject);
  CDisplayPair *CreateNewPair();

  // construction with drawdef and array,see constructors of CDisplayPair
  //	CDisplayPair* CreateNewPair(CDrawDef * dd,geo::CPtrArray<geo::IObject> * ptr_array);

  void RemoveObject(CDisplayPair *pObject);
  void RemoveAllObjects();

public:
  C2DGraphBase(int nDivisionX = 5, int nDivisionY = 5);
  virtual ~C2DGraphBase();

  void UpdateGraphLayout(bool bEditLimits = true);

  virtual void Clear();

  virtual int LineSize() const;
  virtual const C2DGraphLineBase &Line(int nIndex) const;
  virtual C2DGraphLineBase &Line(int nIndex);
  virtual TColor GetGraphBorderColor() const;
  virtual TColor GetGraphGridColor() const;
  virtual TColor GetGraphTextColor() const;

  virtual void DrawScene();

  double MaxX() const;
  double MinX() const;
  geo::CValue MaxY() const;
  geo::CValue MinY() const;

  double GridX() const;
  double GridY() const;

  int TickX() const { return m_nDivisionX; }
  int TickY() const { return m_nDivisionY; }
  void TickX(const int t) { m_nDivisionX = t; }
  void TickY(const int t) { m_nDivisionY = t; }

  void SetMinMaxX(const double &dMinX, const double &dMax);
  // wjrx mantis 2559
  void AutoX(bool bAutoX);
  void AutoY(bool bAutoY);
  bool AutoX() const;
  bool AutoY() const;
  void SetMinMaxY(const geo::CValue &vMinY, const geo::CValue &vMaxY);

  void ResetMinMaxX();

  TColor DragLineColor() const;
  void DragLineColor(const TColor &Color);

  void InsertPolygon(int nMinIndex, int nMaxIndex, TColor color, const double &dEps = EPS, bool bCutOnEps = false);

  void EnableMarkers(bool bEnable);
  bool MarkersEnabled() const;

  void EnableInvalidSections(bool bEnable);
  bool InvalidateSections() const;
  TColor InvalidSectionColor() const;
  void InvalidSectionColor(TColor color);
  virtual DIMENSION Dimension() const { return DIM_2D; }

  virtual void FlushDisplayList();

  // Call this function when graphobjects should be re-generated
  virtual void UpdateGraph();

protected:
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  void SetupProjectionMatrix() const;
  void SetupModelMatrix() const;

private:
  void UpdateGrid();
};

#endif // !defined(OPENGL_2DGRAPHBASE_H__INCLUDED_)
