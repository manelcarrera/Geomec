/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DrawVisitorBase.cpp: implementation of the CDrawVisitorBase class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawVisitorBase.h"
#include "ISymbol.h"
#include "LabelPoint.h"


CDrawVisitorBase::CDrawVisitorBase()
{
}

CDrawVisitorBase::~CDrawVisitorBase()
{
}


bool CDrawVisitorBase::HandleLabelPoint(const CLabelPoint &LabelPoint)
{
	return HandlePoint(LabelPoint);
}

bool CDrawVisitorBase::HandleSymbol(const ISymbol &Symbol)
{
	return HandleObject(Symbol);
}

