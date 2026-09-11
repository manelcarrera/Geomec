 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Connect.h: interface for the IElementConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECT_H__67FDB12B_7525_48CF_8AAF_6496C8B362D3__INCLUDED_)
#define AFX_CONNECT_H__67FDB12B_7525_48CF_8AAF_6496C8B362D3__INCLUDED_

#include "dimple.h"

#include "GeometryExports.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GEOMETRY_EXPORT  IElementConnect  
{
protected:
	IElementConnect();

        virtual ~IElementConnect() {}
public:
	virtual int NodeIndex(int nIndex) const = 0;
	virtual int NodeSize() const = 0;
};

class GEOMETRY_EXPORT  INodeConnect
{
protected:
	INodeConnect();

        virtual ~INodeConnect() {}
public:
	virtual int ElementIndex(int nIndex) const = 0;
	virtual int ElementSize() const = 0;
};

#endif // !defined(AFX_CONNECT_H__67FDB12B_7525_48CF_8AAF_6496C8B362D3__INCLUDED_)
