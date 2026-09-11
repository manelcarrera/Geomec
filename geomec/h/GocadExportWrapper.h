// GocadExportWrapper.h: interface for the CGocadExportWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADEXPORTWRAPPER_H__7E08C775_2D65_4A9F_B1AC_CE41B59ED8F7__INCLUDED_)
#define AFX_GOCADEXPORTWRAPPER_H__7E08C775_2D65_4A9F_B1AC_CE41B59ED8F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// wrapper to get rid of constant recompiling due to include file dependencies in
//  frequently used template class

class CModelBase;
class CResultGroup;

class CGocadExportWrapper  
{
public:
  CGocadExportWrapper();

  void Export(CModelBase& model, const CResultGroup* pResultGroup = 0);
};

#endif // !defined(AFX_GOCADEXPORTWRAPPER_H__7E08C775_2D65_4A9F_B1AC_CE41B59ED8F7__INCLUDED_)
