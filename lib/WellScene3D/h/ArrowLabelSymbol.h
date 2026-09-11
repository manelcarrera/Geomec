// ArrowLabelSymbol.h: interface for the CArrowLabelSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARROWLABELSYMBOL_H__A3A5E196_7C64_4F4D_B823_37364E5F3975__INCLUDED_)
#define AFX_ARROWLABELSYMBOL_H__A3A5E196_7C64_4F4D_B823_37364E5F3975__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellSymbolBase.h"

class CArrowLabelSymbol  : public CWellSymbolBase
{
public:
  CArrowLabelSymbol(const geo::CPoint& position , const geo::IVector &dir , const QString& description,bool autoDelete=true);
};

#endif // !defined(AFX_ARROWLABELSYMBOL_H__A3A5E196_7C64_4F4D_B823_37364E5F3975__INCLUDED_)
