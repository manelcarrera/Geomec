// GM3Database.h: interface for the CGM3Database class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GM3DATABASE_H__B52438FF_5D92_4949_A208_0E1B510D8E14__INCLUDED_)
#define AFX_GM3DATABASE_H__B52438FF_5D92_4949_A208_0E1B510D8E14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMDatabase.h"

namespace gm { 

// C3DDatabase adds version info to CDaoDatabase
class C3DDatabase : public CGmDatabase  
{
public:
  C3DDatabase();
  virtual void OpenDB(const CString &strPathName, const BOOL bComplete);
};

}

#endif // !defined(AFX_GM3DATABASE_H__B52438FF_5D92_4949_A208_0E1B510D8E14__INCLUDED_)
