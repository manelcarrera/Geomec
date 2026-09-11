
#include "stdafx.h"
#include "VersionInfo.h"


CVersionInfo::CVersionInfo()
{
	m_Version=0;
	m_MinorRevision=0;
	m_Revision=0;
}

CVersionInfo::CVersionInfo(short version, short  revision , short minorrevision )
{
	m_Version=version;
	m_Revision=revision;
	m_MinorRevision=minorrevision;
}

CVersionInfo::~CVersionInfo()
{

}


BOOL CVersionInfo::operator == (const CVersionInfo& rhs) const
{
	if(rhs < *this)
		return FALSE;

	if(*this < rhs)
		return FALSE;

	return TRUE;
}

BOOL CVersionInfo::operator > (const CVersionInfo& rhs) const
{
	if(rhs < *this)
		return TRUE;

	return FALSE;
}


BOOL CVersionInfo::operator >= (const CVersionInfo& rhs) const
{
	if(*this < rhs)
		return FALSE;
	
	return TRUE;
}

BOOL CVersionInfo::operator <= (const CVersionInfo& rhs) const
{
	if(rhs < *this)
		return FALSE;
	
	return TRUE;

}

BOOL CVersionInfo::operator < (const CVersionInfo& rhs) const
{
	if(m_Version < rhs.m_Version)
		return TRUE;

	if(m_Version > rhs.m_Version)
		return FALSE;

	//Version is equal 

	if(m_Revision < rhs.m_Revision)
		return TRUE;

	if(m_Revision > rhs.m_Revision)
		return FALSE;

	//revision is equal, hence all equal

	if(m_MinorRevision < rhs.m_MinorRevision)
		return TRUE;

	if(m_MinorRevision > rhs.m_MinorRevision)
		return FALSE;
	
	//minor revision is equal 

	
	return FALSE;

}

CString CVersionInfo::GetVersionString()
{
	CString str;
	str.Format("%i.%i.%i",m_Version,m_Revision,m_MinorRevision);
	return str;
}
