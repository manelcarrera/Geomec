 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Marker.h: interface for the CMarker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MARKER_H__8A446935_9FD9_4E44_84E3_8960EF42B028__INCLUDED_)
#define AFX_MARKER_H__8A446935_9FD9_4E44_84E3_8960EF42B028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISymbol.h"
#include <qcolor.h>
#include <qstring.h>

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CMarker: public ISymbol
{
public:
	enum TMarkerType {MT_CIRCLE, MT_STAR, MT_POINT, MT_TRIANGLE};
	typedef QRgb TColor;
private:
	bool m_bFill;
	geo::CPoint m_location;
	QString m_sLabel;
	TColor m_color;
	TMarkerType m_type;
	geo::CPtrArray<geo::IObject> m_geometry;
	double m_dSize;
	void CreateGeometry();
public:
	CMarker(const geo::IPoint& pivot_point,
			TColor color, 
			TMarkerType type,
			const QString&  sLabel,
			bool bFill = false,
			const double& dSize = 10);

	CMarker(const CMarker& rhs);
	virtual ~CMarker();

	virtual const QString& Label() const;
	void Label(const QString& sLabel);
	virtual TColor Color() const;
	virtual TMarkerType Type() const;
	virtual bool Fill() const;
	void Fill(bool bFill);
	void Position(const geo::IPoint& location);

	// ISymbol interface
	virtual int NrOfPositions() const;
	virtual const geo::IPoint& Position(int Index) const;
	virtual const geo::IArray& Geometry() const;
	virtual const geo::IPoint& PivotPoint() const;

	// IObject interface
	virtual void Rotate(const geo::IVector &vec, const double &dAngleDeg);
	virtual void Move(const geo::IVector &vec);
	virtual void Transform(const geo::IMatrix &matrix);
	virtual void AssertValid() const;
	virtual bool Empty() const;
	virtual geo::CPoint Min() const;
	virtual geo::CPoint Max() const;
};

#endif // !defined(AFX_MARKER_H__8A446935_9FD9_4E44_84E3_8960EF42B028__INCLUDED_)
