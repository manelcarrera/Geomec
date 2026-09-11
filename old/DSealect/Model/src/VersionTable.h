// VersionTable.h: interface for the CVersionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_)
#define AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DSTable.h"


#define VERSION_TABLE					"VersionInfo"
#define VERSION_NAME					"VersionName"
#define VERSION							"Version"
#define REVISION						"Revision"

class CDSModelDatabase;


class CVersionTable : public CDSTable  
{
private:
	CString m_VersionName;	
public:
	CString VersionName() const;
	void    VersionName(const CString name);

	CVersionTable(CDaoDatabase& Database);
	virtual ~CVersionTable();
	virtual void Open();
	virtual void Create();

	//CDSTable interface
	virtual CString TableName();
};

#endif // !defined(AFX_VERSIONTABLE_H__58CA86C1_81B6_45F4_A5F3_697C2C20CB2D__INCLUDED_)
