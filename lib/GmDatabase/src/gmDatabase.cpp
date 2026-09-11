// 3DDatabase.cpp: implementation of the C3DDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GmDatabase.h"
#include "gm3IncompleteException.h"

namespace gm
{
//////////////////////////////////////////////////////////////////////
// Implementation of CGmDatabase Base class
//////////////////////////////////////////////////////////////////////
CGmDatabase::CGmDatabase()
: m_dVersion(-1), m_bComplete(FALSE)
{
}

const double& CGmDatabase::Version() const
{
	assert(m_dVersion > 0);
	return m_dVersion;
}

BOOL CGmDatabase::FieldExist(const CString& sTable, const CString& sField)
{
#ifndef _WIN64
	CDaoTableDef td( this );
	td.Open(sTable);
	try
	{
		CDaoFieldInfo cdaoinfo;
		td.GetFieldInfo(sField,cdaoinfo);
	}
	catch(CDaoException* e)
	{
		e->Delete();
		td.Close();
		return FALSE;
	}

	td.Close();
	return TRUE;
#else
  return FALSE;
#endif
}

BOOL CGmDatabase::Complete() const
{
	return m_bComplete;
}

long CGmDatabase::TableSize(const CString& sTableName)
{
	long lRet = 0;

#ifndef _WIN64
	CDaoRecordset rs(this);
	CString	strQuery = _T("[");
	strQuery += sTableName;
	strQuery += _T("]");
	rs.Open(dbOpenTable, strQuery, dbReadOnly);
	if (!rs.IsBOF())
	{
		rs.MoveFirst();
		rs.MoveLast();
		lRet = rs.GetRecordCount();
	}
	rs.Close();
#endif

	return lRet;
}



} // End of namespace gm ...
