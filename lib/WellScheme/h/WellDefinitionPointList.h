// WellDefinitionPointList.h: interface for the CWellDefinitionPointList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLDEFINITIONPOINTLIST_H__86473673_F2AF_4C07_9828_FDE003B877AC__INCLUDED_)
#define AFX_WELLDEFINITIONPOINTLIST_H__86473673_F2AF_4C07_9828_FDE003B877AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//*******************************************************************
//              
//  FILE:       WellDefinitionPointList.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellDefinitionPointList
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//              
//*******************************************************************


#include "WellSchemeInclude.h"
#include "WellPointList.h"

namespace well {

class CWellPathBase;
class CWellPointList;

//##ModelId=3F7AA15701CC
class CWellDefinitionPointList :  public CWellPointList  
{
public:
	//returns the number of removed points
	long RemoveRedundantPoints();

friend class CWellPathBase;

private:
	//##ModelId=3F7AA15701DC
	CWellDefinitionPointList(CWellPathBase& WellPath);
	//##ModelId=3F7AA15701DE
	virtual ~CWellDefinitionPointList();


};

}

#endif // !defined(AFX_WELLDEFINITIONPOINTLIST_H__86473673_F2AF_4C07_9828_FDE003B877AC__INCLUDED_)
