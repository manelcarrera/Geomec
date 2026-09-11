// DSealectWellPath.cpp: implementation of the CDSealectWellPath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSealectWellPath.h"
#include "CDataModel.h"


#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSealectWellPath::CDSealectWellPath( CDataModel* pModel)
{
	m_pModel=pModel;
}


CDSealectWellPath::~CDSealectWellPath()
{

}




double CDSealectWellPath::GetLength()
{
	return m_PolyLine.GetLength();	
}
