 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ZoomWindowDragger.h: interface for the CZoomWindowDragger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOOMWINDOWDRAGGER_H__D3AA4365_E42A_46CE_816B_741FFE2A812C__INCLUDED_)
#define AFX_ZOOMWINDOWDRAGGER_H__D3AA4365_E42A_46CE_816B_741FFE2A812C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectangleModifier.h"

#include "OpenGLFrameExports.h"

class COpenGLScene;
class OPENGLFRAME_EXPORT CZoomWindowDragger : public CRectangleDragger  
{
	COpenGLScene& m_scene;
protected:
	virtual bool OnOK();
public:
	CZoomWindowDragger(COpenGLScene& scene);
	virtual ~CZoomWindowDragger();
	virtual bool ValidatePoint(const geo::IPoint& /*point*/) const { return true; }
};

#endif // !defined(AFX_ZOOMWINDOWDRAGGER_H__D3AA4365_E42A_46CE_816B_741FFE2A812C__INCLUDED_)
