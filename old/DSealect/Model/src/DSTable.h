// DSTable.h: interface for the CDSTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSTABLE_H__CAA48002_5889_4CDE_A8BD_FDDA6E4D279B__INCLUDED_)
#define AFX_DSTABLE_H__CAA48002_5889_4CDE_A8BD_FDDA6E4D279B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdao.h>


class CVersionDatabase;
class CDSModelDatabase;

class CDSTable  
{
private:
	CDaoDatabase& m_Database;
	CDaoRecordset m_TableRS;
	CString m_sTableName;
	// Prevent Default Constructor
	CDSTable();
	//Prevent copy
	CDSTable(const CDSTable&);
	// Prevent assignment
	CDSTable& operator=(const CDSTable&);

protected:
CDaoDatabase& Database() const;

public:

	virtual void SetField(const CString& sFieldName, const long iVal);
	virtual void SetField(const CString& sFieldName, const double dVal);
	virtual void SetField(const CString& sFieldName, const CString& sVal);
	virtual void SetField(const CString& sFieldName, const CByteArray& sVal);
	virtual void SetField(const CString& sFieldName, const BOOL& bVal);
	virtual void SetField(const CString& sFieldName, const CDoubleQuantity& qVal);

	virtual void GetField(const CString& sFieldName, long& iVal);
	virtual void GetField(const CString& sFieldName, double& dVal);
	virtual void GetField(const CString& sFieldName, CString& sVal);
	virtual void GetField(const CString& sFieldName, CByteArray& sVal);
	virtual void GetField(const CString& sFieldName, BOOL& bVal);
	virtual void GetField(const CString& sFieldName, CDoubleQuantity& qVal);

	CDSTable(CDaoDatabase& database);
	virtual ~CDSTable();
	virtual void Open() = 0;
	virtual void Create() = 0;
	virtual CString TableName();
	
	virtual void Close();
	
	CDaoRecordset& RS() const {return ((CDaoRecordset&)m_TableRS);}
};

#endif // !defined(AFX_DSTABLE_H__CAA48002_5889_4CDE_A8BD_FDDA6E4D279B__INCLUDED_)



