// DaoWellInterface.h: interface for the CDaoWellInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DAOWELLINTERFACE_H__505E166C_F203_4A77_8C81_6DCBA8478020__INCLUDED_)
#define AFX_DAOWELLINTERFACE_H__505E166C_F203_4A77_8C81_6DCBA8478020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN64
#include <afxdao.h>

class  well::CWellPathBase;


class AFX_EXT_CLASS CDaoWellInterface  
{
public:
	CDaoWellInterface(well::CWellPathBase& wellpath);
	virtual ~CDaoWellInterface();
	
	bool LoadFromDatabase(const CString & filename);
	bool SaveToDatabase(const CString & filename);

private: 
	well::CWellPathBase* m_pWellPath;

};

#endif // _WIN64

#endif // !defined(AFX_DAOWELLINTERFACE_H__505E166C_F203_4A77_8C81_6DCBA8478020__INCLUDED_)
