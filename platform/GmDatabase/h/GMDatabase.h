// 3DDatabase.h: interface for the C3DDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DDATABASE_H__AE1ADDB1_5E5E_456B_BEA0_8CAA49CF0A14__INCLUDED_)
#define AFX_3DDATABASE_H__AE1ADDB1_5E5E_456B_BEA0_8CAA49CF0A14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace gm {

#ifdef _WIN64 // no DAO support for 64 bits windows

// dummy
class CDaoDatabase {
public:
  CDaoDatabase();
};
#endif

class CGmDatabase : public CDaoDatabase {
protected:
  double m_dVersion;
  BOOL m_bComplete;

public:
  CGmDatabase();

  virtual void OpenDB(const CString &strPathName, const BOOL bComplete) = 0;

  // Return value:
  // TRUE		A complete database is required. If not throw a CIncompleteException
  // FALSE	Incomplete databases are accepted.
  BOOL Complete() const;
  const double &Version() const;
  BOOL FieldExist(const CString &sTable, const CString &sField);
  long TableSize(const CString &sTableName);
};

} // namespace gm

#endif // !defined(AFX_3DDATABASE_H__AE1ADDB1_5E5E_456B_BEA0_8CAA49CF0A14__INCLUDED_)
