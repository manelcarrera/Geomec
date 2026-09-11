// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_ICOMPONENT_3B6576CE02A1_INCLUDED
#define _INC_ICOMPONENT_3B6576CE02A1_INCLUDED


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CDataModel;

//##ModelId=3C0DDDA9030E
class IComponent 
{
private:

public:

	//##ModelId=3C0DDDA9031D
	 virtual bool  CheckComplete() ;

	//##ModelId=3C0DDDA9031F
	 virtual CString GetInfo();

   CDataModel& Model();
   const CDataModel& Model() const;

protected:
	//##ModelId=3C60D563006E
	IComponent();

	//##ModelId=3C0DDDA90321
	CString m_strInfo;

	//##ModelId=3C0DDDA9033D
	bool m_bComplete;


};

#endif /* _INC_ICOMPONENT_3B6576CE02A1_INCLUDED */
