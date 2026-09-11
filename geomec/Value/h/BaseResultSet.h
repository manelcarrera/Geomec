// BaseResultSet.h: interface for the CBaseResultSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASERESULTSET_H__62786FBC_B583_4A78_ABC6_8D1FD4B28716__INCLUDED_)
#define AFX_BASERESULTSET_H__62786FBC_B583_4A78_ABC6_8D1FD4B28716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBaseResultSet {
public:
  void LoadResults();

  CBaseResultSet();
  virtual ~CBaseResultSet();
};

#endif // !defined(AFX_BASERESULTSET_H__62786FBC_B583_4A78_ABC6_8D1FD4B28716__INCLUDED_)
