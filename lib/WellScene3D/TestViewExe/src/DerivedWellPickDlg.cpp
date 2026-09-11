// DerivedWellPickDlg.cpp: implementation of the CDerivedWellPickDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestViewExe.h"
#include "DerivedWellPickDlg.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDerivedWellPoint::CDerivedWellPoint(well::CWellPathBase& WellPath, double TMD, int extra)
:well::CWellPoint(WellPath,TMD)
{
	// 
}

/*virtual*/ 
CDerivedWellPoint::~CDerivedWellPoint()
{
	// Destruct
}

	

CDerivedWellPickDlg::CDerivedWellPickDlg(	well::CWellPointList* pList,
						CWellSceneInterMed* pSceneInterMed,
						QPtrVector<well::CWellSectionList> Sections,
						CDoubleQuantity::UNIT unit,
						CWnd* pParent)
:CWellPointPickDlg(pList,pSceneInterMed,Sections,unit,pParent)						
{

}

/*virtual*/
well::CWellPoint* CDerivedWellPickDlg::OnCreateWellPoint(well::CWellPathBase& wellpath, double tmd)
{
	return new CDerivedWellPoint(wellpath,tmd,0);
}

CDerivedWellPickDlg::~CDerivedWellPickDlg()
{

}
