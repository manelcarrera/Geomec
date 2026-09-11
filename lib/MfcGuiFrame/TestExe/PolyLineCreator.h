// PolyLineCreator.h: interface for the CPolyLineCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYLINECREATOR_H__12683905_549A_411E_9B20_E72605E54166__INCLUDED_)
#define AFX_POLYLINECREATOR_H__12683905_549A_411E_9B20_E72605E54166__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mouselistener.h"

class C2DDocument;
class CPolyLineCreator : public CMouseListener  
{
	std::vector<geo::CPoint> m_points;
	std::vector<geo::CPoint> m_crossings;
	C2DDocument& m_document;
public:
	CPolyLineCreator(IOpenGLFrame& frame, C2DDocument& document);
	virtual ~CPolyLineCreator();

	virtual bool OnOK();

	virtual void DrawScene();
	virtual bool MouseRelease(TButtonState state, TButtonState button, const TScreenPoint& point);
	virtual bool MouseMove(TButtonState state, TButtonState button, const TScreenPoint& point);
};

#endif // !defined(AFX_POLYLINECREATOR_H__12683905_549A_411E_9B20_E72605E54166__INCLUDED_)
