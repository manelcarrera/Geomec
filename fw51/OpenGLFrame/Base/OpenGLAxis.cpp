 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// OpenGLAxis.cpp: implementation of the COpenGLAxis class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "OpenGLAxis.h"
#include "LabelPoint.h"
#include "DrawDef.h"
#include "OpenGLArrow.h"
#include "Sphere.h"
#include <qgl.h>

#define ARROW_RANGE_MAX 1.0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLAxis::COpenGLAxis(TColor color,
						 const QString& sX,
						 const QString& sY,
						 const QString& sZ)
: COpenGLScene(false)
{
	m_RotateSmoothFactor=3.0; // see COpenGLScene
	m_AllowRotateParent=true;
	m_Visible=true;
	m_LockAxis=0;
	EnableLighting(true);

	double scale=2.2;
	ScaleVector(geo::CVector(scale,scale,scale));

	// Insert our three arrows in the scene
	m_Arrow[0] = new COpenGLArrow(geo::CPoint::NullPoint, geo::CVector::Xaxis);
	m_Arrow[1] = new COpenGLArrow(geo::CPoint::NullPoint, geo::CVector::Yaxis);
	m_Arrow[2] = new COpenGLArrow(geo::CPoint::NullPoint, geo::CVector::Zaxis);
	double rad = (geo::CVector::Xaxis + geo::CVector::Yaxis + geo::CVector::Zaxis).Length();
	m_pSphere = new geo::CSphere(geo::CPoint::NullPoint, rad);

	// Insert our label in the scene
	double dLabelPos = geo::CVector::Xaxis.Length()*1.1;
	m_Label[0] = new CLabelPoint(geo::CPoint(dLabelPos, 0, 0), sX);
	m_Label[1] = new CLabelPoint(geo::CPoint(0, dLabelPos, 0), sY);
	m_Label[2] = new CLabelPoint(geo::CPoint(0, 0, dLabelPos), sZ);
	
	for(int i = 0; i < 3; i++)
	{
		CDrawDef *pDrawDef = new CDrawDef(color);
		pDrawDef->RequireCompile(FALSE);
		pDrawDef->TextColor(color);
		insert(*m_Arrow[i], *pDrawDef);
		insert(*m_Label[i], *pDrawDef);
	}

	CDrawDef* pDrawDef = new CDrawDef();
	pDrawDef->Visible(false);
	insert(*m_pSphere, *pDrawDef);

	AxisColor(geo::CVector::Xaxis, color);
	AxisColor(geo::CVector::Yaxis, color);
	AxisColor(geo::CVector::Zaxis, color);

	AllowDynamicRotate(true);
	AllowDynamicPan(false);
	AllowDynamicZoom(false);
}

/*virtual*/ void COpenGLAxis::ViewVector(const geo::IVector &vec)
{
	COpenGLScene::ViewVector(vec);
	COpenGLScene* pScene = dynamic_cast<COpenGLScene*>(Parent());
	if(pScene && m_AllowRotateParent &&  Visible()) 
	{
		pScene->ViewVector(vec);
		pScene->UpdateFrame();

	}
}

/*virtual*/ void COpenGLAxis::UpVector(const geo::CVector &vec)
{
	COpenGLScene::UpVector(vec);
	COpenGLScene* pScene = dynamic_cast<COpenGLScene*>(Parent());
	if(pScene && m_AllowRotateParent &&  Visible()) 
	{
		pScene->UpVector(vec);
		pScene->UpdateFrame();
	}
}

COpenGLAxis::~COpenGLAxis()
{
	for(int i = 0; i < 3; i++)
	{
		IDrawDef* pDrawDef1 = const_cast<IDrawDef*>(&DrawDef(*m_Arrow[i]));
		erase(*m_Arrow[i], false);
		erase(*m_Label[i], false);
		delete pDrawDef1;
		delete m_Arrow[i];
		delete m_Label[i];
	}

	IDrawDef* pDrawDef2 = const_cast<IDrawDef*>(&DrawDef(*m_pSphere));
	erase(*m_pSphere, false);
	delete pDrawDef2;
	delete m_pSphere;

	if(m_LockAxis)
	{
		IDrawDef* pDrawDef3 = const_cast<IDrawDef*>(&DrawDef(*m_LockAxis));
		erase(*m_LockAxis, false);
		delete pDrawDef3;
		delete m_LockAxis;
	}
}

/*virtual*/ void COpenGLAxis::SetupModelMatrix() const
{

	const COpenGLScene* pScene = dynamic_cast<const COpenGLScene*>(Parent());
	if(pScene)
	{
		COpenGLAxis* pThis = const_cast<COpenGLAxis*>(this);
		pThis->m_ViewVector=pScene->ViewVector();
		pThis->m_UpVector=pScene->UpVector();
	}
	COpenGLScene::SetupModelMatrix();
}

int COpenGLAxis::Index(const geo::IVector& axis) const
{
	if(axis == geo::CVector::Xaxis)
		return 0;
	if(axis == geo::CVector::Yaxis)
		return 1;
	if(axis == geo::CVector::Zaxis)
		return 2;
	return -1;
}

bool COpenGLAxis::Visible() const
{
	return m_Visible;
}

void COpenGLAxis::Visible(bool visible)
{
	m_Visible = visible;
}

bool COpenGLAxis::AllowRotateParent() const
{
	return m_AllowRotateParent;
}

void COpenGLAxis::AllowRotateParent(bool allow)
{
	m_AllowRotateParent = allow;
}

void COpenGLAxis::DrawScene()
{

	if(!m_Visible)
		return;

	if(!Parent())
	{
		COpenGLScene::DrawScene();
		return;
	}

	//handle the drawing/color of the lock-axis
	COpenGLScene* pScene= dynamic_cast<COpenGLScene*>(Parent());
	if(pScene)
	{

		if( ( pScene->RotateLockVector() == geo::CVector::NullVector && m_LockAxis) ||
			(m_LockAxis && pScene->RotateLockVector() != m_LockAxis->Dir() ) )
		{
			//the arrow exists but is not up to date, thus delete it
			IDrawDef* pDrawDef = const_cast<IDrawDef*>(&DrawDef(*m_LockAxis));
			erase(*m_LockAxis, false);
			delete pDrawDef;
			delete m_LockAxis;
			m_LockAxis=0;
		}


		if(pScene->RotateLockVector() != geo::CVector::NullVector && !m_LockAxis)
		{
			//there is a lock-vector but no arrow, thus create a new one
			m_LockAxis = new COpenGLArrow(geo::CPoint::NullPoint, pScene->RotateLockVector());
			CDrawDef* pDrawDef = new CDrawDef();
			pDrawDef->SetFixedColor(qRgb(0,0,255));
			insert(*m_LockAxis, *pDrawDef);
		}

		//see if one of the orthogonal axis(x,y,z) is the lock-vector
		//to prevent drawing over each other.
		int index = Index(pScene->RotateLockVector());
		for(int i=0;i<3;i++)
		{
			CDrawDef *pDrawDef = (CDrawDef*)&DrawDef(*m_Arrow[i]);
			
			if( index == i  )
			{
				if(pDrawDef->Visible())
				{
					//one of the orthogonal axis is the lock-axis and is visible,
					//make it invisible
					pDrawDef->Visible(false);
					invalidate(*m_Arrow[i],false);
				}

			}
			else
			{
				if(!pDrawDef->Visible())
				{
					//one of the orthogonal axis is NOT the lock-axis but is NOT visible,
					//make it visible
					pDrawDef->Visible(true);
					invalidate(*m_Arrow[i],false);
				}
			}
		}
	}
	

	//let the scene do its thing..
	COpenGLScene::DrawScene();

}

void COpenGLAxis::AxisText(const geo::IVector& axis, const QString& text)
{
	assert(Index(axis) != -1);
	m_Label[Index(axis)]->Label(text);
}

const QString& COpenGLAxis::AxisText(const geo::IVector& axis)
{
	assert(Index(axis) != -1);
	return m_Label[Index(axis)]->Label();
}

void COpenGLAxis::AxisColor(const geo::IVector& axis, TColor color)
{
	CDrawDef *pDrawDef = (CDrawDef*)&DrawDef(*m_Arrow[Index(axis)]);
	pDrawDef->SetFixedColor(color);
	pDrawDef->TextColor(color);
	UpdateFrame();
}

COpenGLAxis::TColor COpenGLAxis::AxisColor(const geo::IVector& axis) const
{
	CDrawDef *pDrawDef = (CDrawDef*)&DrawDef(*m_Arrow[Index(axis)]);
	return pDrawDef->GetFixedColor();
}

/*virtual*/ COpenGLAxis::TColor COpenGLAxis::BackgroundColor() const
{
	assert(Parent());
	return Parent()->BackgroundColor(); 
}
