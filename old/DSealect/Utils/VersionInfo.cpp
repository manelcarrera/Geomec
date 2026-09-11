
#include "stdafx.h"
#include "VersionInfo.h"


CVersionInfo::CVersionInfo()
{
	m_nVersion=-1;
	m_nRevision=-1;
}

CVersionInfo::CVersionInfo(short version,short revision)
{
	m_nVersion=version;
	m_nRevision=revision;
}

CVersionInfo::~CVersionInfo()
{

}


BOOL CVersionInfo::operator == (const CVersionInfo& rhs)
{
	if ( &rhs != this )
	{
		return (m_nVersion==rhs.m_nVersion && m_nRevision==rhs.m_nRevision);
	}
	return TRUE;
}

BOOL CVersionInfo::operator > (const CVersionInfo& rhs)
{
	if ( &rhs != this )
	{
		if ( m_nVersion > rhs.m_nVersion )
			return TRUE;
		else if ( m_nVersion < rhs.m_nVersion )
			return FALSE;
		else{
			if ( m_nRevision > rhs.m_nRevision )
				return TRUE;
			else 
				return FALSE;
		}
	}
	return FALSE;
}


BOOL CVersionInfo::operator >= (const CVersionInfo& rhs)
{
	if ( &rhs != this )
	{
		if ( m_nVersion > rhs.m_nVersion )
			return TRUE;
		else if ( m_nVersion < rhs.m_nVersion )
			return FALSE;
		else{
			if ( m_nRevision > rhs.m_nRevision )
				return TRUE;
			else if ( m_nRevision < rhs.m_nRevision )
				return FALSE;
			else
				return TRUE;
		}

	}
	return FALSE;
}


BOOL CVersionInfo::operator < (const CVersionInfo& rhs)
{
	if ( &rhs != this )
	{
		if ( m_nVersion < rhs.m_nVersion )
			return TRUE;
		else if ( m_nVersion > rhs.m_nVersion )
			return FALSE;
		else
		{
			if ( m_nRevision < rhs.m_nRevision )
				return TRUE;
			else 
				return FALSE;
		}

	}
	return FALSE;
}

BOOL CVersionInfo::operator <= (const CVersionInfo& rhs)
{
  return operator<(rhs) || operator==(rhs);
}

CString CVersionInfo::GetVersionString()
{
	CString str;
	str.Format("%i.%i",m_nVersion,m_nRevision);
	return str;
}
