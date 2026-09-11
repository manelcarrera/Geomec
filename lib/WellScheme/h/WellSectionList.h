// WellSectionList.h: interface for the CWellSectionList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSECTIONLIST_H__0F6C4504_1FA7_4D1C_BEB4_173A5B9B3277__INCLUDED_)
#define AFX_WELLSECTIONLIST_H__0F6C4504_1FA7_4D1C_BEB4_173A5B9B3277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <qptrvector.h> 
#include "WellSchemeInclude.h"
#include "WellSectionListBase.h"
#include "IWellSection.h"

//*******************************************************************
//              
//  FILE:       WellSectionList.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellSectionList
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   Sections are sorted on top and bottom.
//              this class is derived from QPtrList<IWellSection> for the virtual override of compareItems.
//*******************************************************************

namespace well {

class CWellPathBase;

//##ModelId=3F7AA15602CA
class CWellSectionList : public  CWellSectionListBase
{

	Q_OBJECT;

public:

	CWellSectionList(const CWellPathBase& WellPath,  bool bAutoDelete=true);
	virtual ~CWellSectionList();
	CWellSectionList& operator=(const CWellSectionList& rhs);
	CWellSectionList(const CWellSectionList& rhs);
		
	virtual int AddSection(IWellSection* section);

	IWellSection*	First() const;
	IWellSection*	Last() const;

public slots:
	void RemoveSection(int index);
	void RemoveAllSections();

};

}

#endif // !defined(AFX_WELLSECTIONLIST_H__0F6C4504_1FA7_4D1C_BEB4_173A5B9B3277__INCLUDED_)
