// FormationPropertiesDraw.h: interface for the CFormationPropertiesDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONPROPERTIESDRAW_H__0FF331A7_6555_4C7B_9903_7B44D812D7A0__INCLUDED_)
#define AFX_FORMATIONPROPERTIESDRAW_H__0FF331A7_6555_4C7B_9903_7B44D812D7A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC15A0203
class CFormationPropertiesDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC15A0205
	CFormationPropertiesDraw();
	//##ModelId=3C0DC15A0213
	virtual ~CFormationPropertiesDraw();
protected:
	//{{AFX_VIRTUAL(CFormationPropertiesDraw)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CFormationPropertiesDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CFormationPropertiesDraw)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CFormationPropertiesDraw)
};

#endif // !defined(AFX_FORMATIONPROPERTIESDRAW_H__0FF331A7_6555_4C7B_9903_7B44D812D7A0__INCLUDED_)
