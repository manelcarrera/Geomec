/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DrawVisitorBase.h: interface for the CDrawVisitorBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWVISITORBASE_H__INCLUDED_)
#define AFX_DRAWVISITORBASE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"

#include "OpenGLFrameExports.h"


class CLabelPoint;
class ISymbol;


class OPENGLFRAME_EXPORT CDrawVisitorBase : public geo::CDispatchVisitorBase
{
protected:
	CDrawVisitorBase();

public:
	virtual ~CDrawVisitorBase();

	virtual bool HandleLabelPoint(const CLabelPoint &LabelPoint);
	virtual bool HandleSymbol(const ISymbol &Symbol);
};




#endif // !defined(AFX_DRAWVISITOR_H__INCLUDED_)
