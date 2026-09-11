#if !defined(VERSION_INFO_INCLUDED)
#define VERSION_INFO_INCLUDED


class AFX_EXT_CLASS CVersionInfo 
{
private:
  short m_Version;
  short m_MinorRevision;
  short m_Revision;
  CString m_Name;

public:
  CString GetVersionString();
  CVersionInfo();
  CVersionInfo(short version,short  revision , short minorrevision =0);

  ~CVersionInfo();

  short Version() const {return m_Version;}
  short Revision() const {return m_Revision;}
  short MinorRevision() const {return m_MinorRevision;}
  

  BOOL operator == (const CVersionInfo& rhs) const;
  BOOL operator > (const CVersionInfo& rhs) const;
  BOOL operator >= (const CVersionInfo& rhs) const;
  BOOL operator <= (const CVersionInfo& rhs) const;
  BOOL operator < (const CVersionInfo& rhs) const;

};

#endif 