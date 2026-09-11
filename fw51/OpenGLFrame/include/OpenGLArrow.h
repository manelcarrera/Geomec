// OpenGLArrow.h: interface for the COpenGLArrow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLARROW_H__3521E59E_BAA4_4107_B20D_C6CC9297CAF4__INCLUDED_)
#define AFX_OPENGLARROW_H__3521E59E_BAA4_4107_B20D_C6CC9297CAF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorTempl.h"

#include "OpenGLFrameExports.h"

namespace geo {
class CLine;
class CConus;
class CVector;
}


//*******************************************************************
//              
//  FILE:       openglarrow.h
//  AUTHOR:     htg
//  PROJECT:    DrillTest
//  COMPONENT:  COpenGLArrow
//  DATE:       08.06.2004
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  COMMENTS:   Describing the shape of the arrow

//*******************************************************************


//a struct describing the shape of the arrow
struct OPENGLFRAME_EXPORT TOpenGLArrowShapeDef
{
	TOpenGLArrowShapeDef(   const double& head_ratio=0.7,
							const double& radius_ratio=0.4,
							const double& width_ratio=0.3 )
	{
		HeadRatio   = head_ratio;
		RadiusRatio = radius_ratio;
		WidthRatio  = width_ratio;
	}
	double HeadRatio;
	double RadiusRatio;
	double WidthRatio;
};

class OPENGLFRAME_EXPORT COpenGLArrow  : public geo::IArray
{
	geo::CConus* m_pLine;
	geo::CConus* m_pConus;

public:
	COpenGLArrow(const geo::IPoint& base_point, const geo::IVector& vector,const TOpenGLArrowShapeDef& shape = TOpenGLArrowShapeDef());
	COpenGLArrow( const COpenGLArrow& rhs );
	virtual ~COpenGLArrow();
	virtual const geo::IObject& ObjectBase(size_t nIndex) const;
	virtual geo::IObject& ObjectBase(size_t nIndex);
	virtual void Remove(int nIndex);
	virtual size_t Size() const;
	virtual void Clear();
	const geo::CVector Dir() const;
	void Change(const geo::IPoint& base_point, const geo::IVector& vector,const TOpenGLArrowShapeDef& shape = TOpenGLArrowShapeDef());
};

#endif // !defined(AFX_OPENGLARROW_H__3521E59E_BAA4_4107_B20D_C6CC9297CAF4__INCLUDED_)
