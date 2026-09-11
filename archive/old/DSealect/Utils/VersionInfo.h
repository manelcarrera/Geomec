#if !defined(VERSION_INFO_INCLUDED)
#define VERSION_INFO_INCLUDED


class CVersionInfo 
{
private:
	short m_nVersion;
	short m_nRevision;
	CString m_Name;

public:
	CString GetVersionString();
	CVersionInfo();
	CVersionInfo(short version,short revision);
	~CVersionInfo();

	short Version() const {return m_nVersion;}
	short Revision() const {return m_nRevision;}

	BOOL operator == (const CVersionInfo& rhs);
	BOOL operator > (const CVersionInfo& rhs);
	BOOL operator >= (const CVersionInfo& rhs);
	BOOL operator < (const CVersionInfo& rhs);
  BOOL operator <= (const CVersionInfo& rhs);
};

#endif 