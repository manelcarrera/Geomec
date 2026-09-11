 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _OBJECTMODIFIER_H_
#define _OBJECTMODIFIER_H_

#include "MouseListener.h"
#include "DrawDef.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CPolygonModifierBase : public CMouseListener
{
public:
	typedef QRgb TColor;
protected:
	// The polygon is presenting the polygon in the display lists displayed in the 
	// 
	class OPENGLFRAME_EXPORT CPolygon
	{
		// Drawdefs for lines and points
		CDrawDef m_ddPoint, m_ddLine, m_ddSelectedPoint;
		std::vector<geo::CPoint> m_vcPoint;
		CPolygonModifierBase& m_mouse_listener;
		void UpdateLines();
		int m_nSelectedPoint;
		bool m_bAutoClose;
	public:
//		typedef QRgb TColor;
		// Construction / Destruction
		CPolygon(CPolygonModifierBase& mouse_listener, 
				 TColor point_color, 
				 TColor line_color, 
				 TColor point_selected_color,
				 bool bAutoClose);
		virtual ~CPolygon();
		// Point Handling
		bool PushBack(const geo::IPoint& point);
		const geo::IPoint& FirstPoint() const;
		const geo::IPoint& LastPoint() const;
		const geo::IPoint& Previous(int nIndex) const;
		const geo::IPoint& Next(int nIndex) const;
		int PointSize() const;
		const geo::IPoint& Point(int nIndex) const;
		// Returns the index of the selected point .. if no point is selected 
		// -1 is returned.
		int PointHitTest(const TScreenPoint& screen_point) const;
		int LineHitTest(const TScreenPoint& screen_point) const;
		void PointSelection(int nIndex);
		int PointSelection() const;

		void ReplacePoint(int nIndex, const geo::IPoint& point);
		void DeletePoint(int nIndex);
		void InsertPoint(int nLineIndex, const geo::IPoint& point);

		IDrawDef& SelectedPointDrawDef() { return m_ddSelectedPoint; }

		void DrawScene();

		// Handling
		int LineSize() const;
		geo::CLine Line(int nIndex) const;

		geo::CPolygon Polygon() const;
	};

	CPolygon *m_pPolygon;

	// Constructor
	CPolygonModifierBase(TFrame &frame,
						 TColor point_color,
						 TColor line_color,
						 TColor point_selected_color,
						 bool bAutoClose = true);
	virtual ~CPolygonModifierBase();

	virtual void DrawScene();

	// Return the current polygon 
	geo::CPolygon Polygon() const;

	// Common validation routines
	bool ValidatePolygon(const geo::CPolygon &polygon) const;
	virtual bool ValidatePoint(const geo::IPoint &point) const;
};

#endif
