// OpenGLStar.h: interface for the COpenGLStar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLSTAR_H__7D2764A8_188A_49D2_B141_3FBDFD401982__INCLUDED_)
#define AFX_OPENGLSTAR_H__7D2764A8_188A_49D2_B141_3FBDFD401982__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPolygon.h"

#include "OpenGLFrameExports.h"

//*******************************************************************
//  FILE      : OpenGLStar.h
//  AUTHOR    : htg
//  PROJECT   : OpenGLLib
//  COMPONENT : COpenGLStar
//  DATE      : 28.11.2002
//  COMMENTS  : Just a litle class to create a star symbol
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//*******************************************************************
class OPENGLFRAME_EXPORT COpenGLStar  
{
public:
	COpenGLStar(long NrOfStarPoints = 5,double StarPointAngle=30.0);
	geo::IPolygon* CreateStar(double Size=1.0);
	virtual ~COpenGLStar();

private:
	long m_NrOfStarPoints;
	double m_StarPointAngle;

};

#endif // !defined(AFX_OPENGLSTAR_H__7D2764A8_188A_49D2_B141_3FBDFD401982__INCLUDED_)
