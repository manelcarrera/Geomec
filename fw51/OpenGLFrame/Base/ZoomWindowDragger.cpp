 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ZoomWindowDragger.cpp: implementation of the CZoomWindowDragger class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "OpenGLScene.h"
#include "ZoomWindowDragger.h"


#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZoomWindowDragger::CZoomWindowDragger(COpenGLScene& scene)
:CRectangleDragger(scene, geo::CPlane()), m_scene(scene)
{
}

CZoomWindowDragger::~CZoomWindowDragger()
{

}

bool CZoomWindowDragger::OnOK()
{
	m_scene.ZoomWindow(BeginScreenPoint(), CurrentScreenPoint());
	CRectangleDragger::OnOK();
	return TRUE;
}

