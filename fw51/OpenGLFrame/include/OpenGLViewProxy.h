 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ValueProxy.h: interface for the CValueProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUEPROXY_H__472711E8_30DF_4AA0_AC8C_7DA730D2547E__INCLUDED_)
#define AFX_VALUEPROXY_H__472711E8_30DF_4AA0_AC8C_7DA730D2547E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPoint.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT IValueProxy  
{
public:
	IValueProxy() {};
	virtual const double &Min() const = 0;
	virtual const double &Max() const = 0;
	virtual double Value(const geo::IPoint &pt) const = 0;

};

class OPENGLFRAME_EXPORT IColorProxy
{
public:
	typedef QRgb TColor;
	IColorProxy() {};
	virtual TColor Color(const IValueProxy &value_proxy, const geo::IPoint& pt) const = 0; 
};

class OPENGLFRAME_EXPORT CDrawDefProxy: public IDrawDef
{
	const IColorProxy*	m_pColor;	// Defines the color
	const IValueProxy*	m_pValue;	// Defines the value
public:
	CDrawDefProxy(const IColorProxy& color_proxy, const IValueProxy& value_proxy);
	const IColorProxy& ColorProxy() const;
	void ColorProxy(const IColorProxy& color_proxy);
	const IValueProxy& ValueProxy();
	void ValueProxy(IValueProxy &proxy);

	// Interface of draw def...
	virtual TColor Color(const geo::IPoint &pt) const;
	using IDrawDef::Color;
};


#endif // !defined(AFX_VALUEPROXY_H__472711E8_30DF_4AA0_AC8C_7DA730D2547E__INCLUDED_)
