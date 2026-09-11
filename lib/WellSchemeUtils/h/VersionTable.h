// VersionTable.h: interface for the CVersionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_)
#define AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN64

#include "DSTable.h"

#include "StreamVersion.h"

class AFX_EXT_CLASS  CVersionTable : public CDSTable  
{
private:
	CString m_VersionName;	
public:
	CString VersionName() const;
	void VersionName(const CString& name);

	CVersionTable(CDaoDatabase& Database);
	CVersionTable(CDaoDatabase& Database,const CString& version_name);

	CStreamVersion LoadVersionInfo() ;
	void SaveVersionInfo(CStreamVersion& versionInfo);

	virtual ~CVersionTable();

	//CDSTable interface
	virtual void Open();
	virtual void Create();
	virtual CString TableName() const;
};

#endif // _WIN64

#endif // !defined(AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_)
