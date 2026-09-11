 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(OPENGL_2DGRAPHPARENTBASE_H__INCLUDED_)
#define OPENGL_2DGRAPHPARENTBASE_H__INCLUDED_

#include "IOpenGLFrame.h"

#include "OpenGLFrameExports.h"

class C2DGraphBase;

// Defines for the layering of the graph
#define GRAPH_LAYER_BACKGROUND	1
#define GRAPH_LAYER_GRID_BOTTOM	2
#define GRAPH_LAYER_GRID_TOP	3
#define GRAPH_LAYER_BORDER		4
#define GRAPH_LAYER_LINE_BOTTOM	5
#define GRAPH_LAYER_LINE_TOP	6
#define GRAPH_LAYER_FOREGROUND	7

class OPENGLFRAME_EXPORT C2DGraphParentBase : public IOpenGLFrame
{
public:
  enum TAxis { AxisLeft, AxisRight, AxisTop, AxisBottom };
  enum TDirection { LeftRight, RightLeft, TopBottom, BottomTop };
private:
  TAxis m_x_axis;
  TAxis m_y_axis;
  TDirection m_x_direction;
  TDirection m_y_direction;
  int ScaleFactor(const double & Maximum, const double & Minimum);
  void AdaptOffset(TAxis axis,
           double& dLeftOffset, 
           double& dRightOffset, 
           double& dTopOffset,
           double& dBottomOffset);
  void DrawAxis(TAxis axis);
public:
  C2DGraphParentBase(TDirection x_direction, TAxis x_axis, TDirection y_direction, TAxis y_axis);
  virtual ~C2DGraphParentBase();

  void SetAxis(TDirection x_direction, TAxis x_axis, TDirection y_direction, TAxis y_axis);
  
  TDirection XDirection() const { return m_x_direction; }
  TDirection YDirection() const { return m_y_direction; }
  TAxis XAxis() const { return m_x_axis; }
  TAxis YAxis() const { return m_y_axis; }

  virtual C2DGraphBase &Graph() = 0;
  virtual const C2DGraphBase &Graph() const = 0;
  void TickX(unsigned int t);
  void TickY(unsigned int t);
  void SetMinMaxX(const double& dMinX, const double& dMax);
  virtual void UpdateScroll();
  virtual void Clear();

  // if UpdateLabels() is not overridden, the base functionality calls XLabel(), XUnit(), YLabel() and YUnit(),
  // so they must be properly implemented
  virtual QString XLabel() const;
  virtual QString XUnit() const;
  virtual QString YLabel() const;
  virtual QString YUnit() const;

  virtual QFont  GetTitleFontX() const;
  virtual QFont  GetTitleFontY() const;

  virtual void DrawScene();

  virtual void OnResizeParent();

  virtual DIMENSION Dimension() const { return DIM_2D; }
  // Call this function when graph object should be regenerated
  virtual void UpdateGraph();

  virtual double TextOffset() const;
  virtual double Offset() const;

protected:
  // The update labels function
  virtual void DrawYLabels();
  virtual void DrawXLabels();

  void DrawAxisLabel(const QString sLabel,
             TAxis axis,
             QFont pFont);
  void DrawLabel(const QString& sLabel,
     		   const double& dFraction, 
           TDirection direction,
           TAxis axis);
  virtual void SetupProjectionMatrix() const;
};

#endif // !defined(OPENGL_2DGRAPHPARENTBASE_H__INCLUDED_)
