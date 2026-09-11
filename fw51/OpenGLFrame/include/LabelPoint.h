 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// LabelPoint.h: interface for the CLabelPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LABELPOINT_H__D32EC734_E89E_4706_A185_872BF5978E6E__INCLUDED_)
#define AFX_LABELPOINT_H__D32EC734_E89E_4706_A185_872BF5978E6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point.h"
#include <qstring.h>
#include <qfont.h>
#include "DrawVisitorBase.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CLabelPoint : public geo::CPoint
{
public:
  enum eLabelAlign{LA_TOP_LEFT,
          LA_TOP_RIGHT,
          LA_BOTTOM_LEFT,
          LA_BOTTOM_RIGHT,
          LA_CENTER,
          LA_CENTER_RIGHT,
          LA_CENTER_LEFT};

  CLabelPoint(const double& dX,
        const double& dY,
        const double& dZ,
        const QString &label,
        const QFont& font = QFont(),
        eLabelAlign Alignment = LA_BOTTOM_LEFT,
        const double& dRotate = 0);
  CLabelPoint(const geo::IPoint& point, 
        const QString &label,
        const QFont& font = QFont(),
        eLabelAlign Alignment = LA_BOTTOM_LEFT,
        const double& dRotate = 0);
  CLabelPoint(const CLabelPoint &labelpoint);
  
  void SetPoint(const double& dX,	const double& dY, const double& dZ = 0);

  int GetAlignment() const;
  void SetAlignment(eLabelAlign Alignment);
  double GetRotate() const;
  void SetRotate(const double& dRotate);

//	int LabelPixelWidth() const;
//	int LabelPixelHeight() const;
//	Use QFontMetrics( font() ).size( 0, Label() );

  virtual ~CLabelPoint();

  //Get...
  const QString& Label() const; 
  //Set...
  void Label(const QString& sLabel);

//	int NrOfLines() const;
//	Use Label().contains( '\n' ) + 1;

  const QFont& font() const;

  virtual bool Visit(geo::IVisitor &visitor)
  {
    assert(dynamic_cast<CDrawVisitorBase *>(&visitor));
    return static_cast<CDrawVisitorBase &>(visitor).HandleLabelPoint(*this);
  }

private:
  QString m_Label;
  eLabelAlign m_eAlignment;
  double m_dRotate;
  QFont m_font;
};

#endif // !defined(AFX_LABELPOINT_H__D32EC734_E89E_4706_A185_872BF5978E6E__INCLUDED_)
