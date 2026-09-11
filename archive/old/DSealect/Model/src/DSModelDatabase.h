// DSModelDatabase.h: interface for the CDSModelDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSMODELDATABASE_H__B961E426_0646_46B7_BE27_5C1C052888B7__INCLUDED_)
#define AFX_DSMODELDATABASE_H__B961E426_0646_46B7_BE27_5C1C052888B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdao.h>
#include <memory>
#include "DSTable.h"
#include "VersionInfo.h"




class CDSModelDatabase
{
private:
	CVersionInfo m_VersionInfo;
	std::auto_ptr<CDaoDatabase>	m_pModelDatabase;
	std::auto_ptr<CDSTable> m_pFormationTable;	
	std::auto_ptr<CDSTable> m_pVersionTable;
	
	std::auto_ptr<CDSTable> m_pDrillingDirectionTable;
	std::auto_ptr<CDSTable> m_pDrillingOperationTable;
	std::auto_ptr<CDSTable> m_pModelOverburdenTable;
	std::auto_ptr<CDSTable> m_pAnalysisPointTable;
	std::auto_ptr<CDSTable> m_pControlTable;
	std::auto_ptr<CDSTable> m_pCementTable;

	std::auto_ptr<CDSTable> m_pBatchTableTable;
  std::auto_ptr<CDSTable> m_pPorePressureProfileTable;
  std::auto_ptr<CDSTable> m_pDensityProfileTable;
  std::auto_ptr<CDSTable> m_pTemperatureProfileTable;
  std::auto_ptr<CDSTable> m_pModelTable;

	
public:
	BOOL FieldExistsInTable(CString strTable, CString FieldName);
	BOOL TableExists(CString strTableName);
	CDSModelDatabase();
	virtual ~CDSModelDatabase();

	CVersionInfo VersionInfo();
	void VersionInfo(CVersionInfo VersionInfo);

	void Create( LPCTSTR lpszName,CVersionInfo versioninfo);
	void Open( LPCTSTR lpszName, BOOL bExclusive = FALSE, BOOL bReadOnly = FALSE, LPCTSTR lpszConnect = _T("") );
	void Close();

	CDaoDatabase& Database() const { return (CDaoDatabase&)*m_pModelDatabase;}
	CDSTable& Formation() const { return (CDSTable&)*m_pFormationTable; }
	CDSTable& DrillingDirection() const { return (CDSTable&)*m_pDrillingDirectionTable; }
	CDSTable& DrillingOperation() const { return (CDSTable&)*m_pDrillingOperationTable; }
	CDSTable& ModelOverburden() const { return (CDSTable&)*m_pModelOverburdenTable; }
	CDSTable& AnalysisPoint() const { return (CDSTable&)*m_pAnalysisPointTable; }
	CDSTable& Control() const { return (CDSTable&)*m_pControlTable; }
	CDSTable& Cement() const { return (CDSTable&)*m_pCementTable; }
	CDSTable& BatchTable() const { return (CDSTable&)*m_pBatchTableTable; }
  CDSTable& PorePressureProfileTable() const { return (CDSTable&)*m_pPorePressureProfileTable; }
  CDSTable& DensityProfileTable() const { return (CDSTable&)*m_pDensityProfileTable; }
  CDSTable& TemperatureProfileTable() const { return (CDSTable&)*m_pTemperatureProfileTable; }
  CDSTable& ModelTable() const { return (CDSTable&)*m_pModelTable; }

};

#endif // !defined(AFX_DSMODELDATABASE_H__B961E426_0646_46B7_BE27_5C1C052888B7__INCLUDED_)
