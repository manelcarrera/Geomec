/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ColorScale.h: interface for the CColorScale class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCALE_H__8637EDBF_38ED_487C_A67D_D61A4943095D__INCLUDED_)
#define AFX_COLORSCALE_H__8637EDBF_38ED_487C_A67D_D61A4943095D__INCLUDED_

#include <set>
#include <utility>

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CColorScale {
  typedef QRgb TColor;
  typedef std::pair<double, TColor> TColorPair;

  class OPENGLFRAME_EXPORT CColorPairLess {
  public:
    typedef QRgb TColor;
    typedef std::pair<double, TColor> TColorPair;
    bool operator()(const TColorPair &p1, const TColorPair &p2) { return p1.first < p2.first; }
  };

  typedef std::set<TColorPair, CColorPairLess> TColorPairSet;
  TColorPairSet m_ColorPairSet;

public:
  TColor GetColor(const double &value) const;
  void Insert(const TColorPair &p);
  void Clear();

  CColorScale(const CColorScale &right);
  CColorScale &operator=(const CColorScale &right);

  CColorScale();
  virtual ~CColorScale();
};

#endif // !defined(AFX_COLORSCALE_H__8637EDBF_38ED_487C_A67D_D61A4943095D__INCLUDED_)
