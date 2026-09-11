/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_LEGENDVIEWBASE_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_)
#define AFX_LEGENDVIEWBASE_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CLegendFrame use legend frames which are vertically alligned to build up the legend
// view
#include "IOpenGLFrame.h"
#include "LabelPoint.h"
#include "Marker.h"
#include <map>

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CLegendFrame : public IOpenGLFrame {
public:
  class OPENGLFRAME_EXPORT legend_square {
    CLegendFrame &m_frame;

  public:
    legend_square(CLegendFrame &frame);
    virtual ~legend_square();
    CLegendFrame &frame();
    virtual int size_x() = 0;
    virtual int size_y() = 0;
    virtual void render(int pos_x, int pos_y) = 0;
  };

  friend class legend_square;

  class OPENGLFRAME_EXPORT legend_separator : public legend_square {
    int m_space;

  public:
    legend_separator(CLegendFrame &view, int space);
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_label : public legend_square {
    CLabelPoint m_label;

  public:
    legend_label(CLegendFrame &frame, const QString &text);
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_index : public legend_square {
  public:
    typedef QRgb TColor;

  private:
    CLabelPoint m_label;
    int m_size_x, m_size_y;
    TColor m_color;

  public:
    legend_index(CLegendFrame &view, int size_x, int size_y, TColor color, const QString &text);
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_scale : public legend_square {
  public:
    typedef QRgb TColor;
    typedef std::map<double, TColor> TColorMap;

  private:
    TColorMap m_color;
    int m_size_x, m_size_y;
    int m_divisions;
    void render_labels(int pos_x, int pos_y);
    void render_scale(int pos_x, int pos_y);
    double max_val() const;
    double min_val() const;

  public:
    legend_scale(CLegendFrame &view, const TColorMap &color, int divisions, int size_x, int size_y);
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_marker : public legend_square {
    CMarker m_marker;
    CLabelPoint m_Label;

  public:
    legend_marker(const CMarker &marker, CLegendFrame &view);
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_symbol : public legend_square {
    ISymbol *m_symbol;
    TColor m_color;
    CLabelPoint m_label;

  public:
    legend_symbol(CLegendFrame &view, ISymbol *symbol, TColor color, const QString &label);
    virtual ~legend_symbol();
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_tree_item : public legend_square {
    CLabelPoint m_Label;
    int m_offset;

  public:
    legend_tree_item(const QString &sName, int offset, CLegendFrame &frame);
    legend_tree_item(const QString &sName, legend_tree_item &parent);
    virtual int size_x();
    virtual int size_y();
    int child_position();
    virtual void render(int pos_x, int pos_y);
  };

  class OPENGLFRAME_EXPORT legend_lineframe : public legend_square {
  public:
    typedef QRgb TColor;

  private:
    CLabelPoint m_Label;
    bool m_bStipple;
    TColor m_Color;

  public:
    legend_lineframe(CLegendFrame &frame, const QString &label, bool bStipple = false, TColor color = qRgb(255, 0, 0));
    virtual int size_x();
    virtual int size_y();
    virtual void render(int pos_x, int pos_y);
  };

private:
  mutable int m_nWidth;
  mutable int m_nHeight;
  void CalcWidthHeight() const;
  typedef std::vector<legend_square *> TFrameVec;
  TFrameVec m_vcFrame;
  int m_nScrollX;
  int m_nScrollY;

public:
  CLegendFrame();
  virtual ~CLegendFrame();

  int ScrollX() const;
  int ScrollY() const;

  void setScrollX(int sx);
  void setScrollY(int sy);

  // Overides to initialise the legend view
  virtual int OffSet_X() const;
  virtual int OffSet_Y() const;

  virtual int ViewPortX() const;
  virtual int ViewPortY() const;
  virtual int ViewPortWidth() const;
  virtual int ViewPortHeight() const;

  int LegendHeight() const;
  int LegendWidth() const;

  // Frame work handles for adding and clearing
  virtual void ClearLegendFrames();
  virtual void DrawScene();
  virtual DIMENSION Dimension() const { return DIM_2D; }
  virtual bool AlignToParent() const { return false; }

  virtual void SetupProjectionMatrix() const;
};

#endif // !defined(AFX_LEGENDVIEWBASE_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_)
