// DSealectDoc.h : interface of the CDSealectDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSEALECTDOC_H__C6FB9130_2D00_43C6_89B6_8F2DCDF508E8__INCLUDED_)
#define AFX_DSEALECTDOC_H__C6FB9130_2D00_43C6_89B6_8F2DCDF508E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataModel;

//hint parameter for update all views
#define UPDATE_UNITS		1
#define UPDATE_DOC_SAVED	2


//##ModelId=3C0DC15C01F4
typedef CDoubleQuantity QU;

//##ModelId=3C0DC15C0222
class CDSealectDoc : public	CDocument		
{
public:
	//##ModelId=3DDA0C57015B
	BOOL m_bShowToolTip;

private:
	//##ModelId=3DDA0C57015C
	CString m_sTempDir;
	//##ModelId=3DDA0C57016A
	CString m_sFileWorkDir;
	//##ModelId=3DDA0C57017A
	void WriteToRegistry();

	//##ModelId=3CAAA66B0311
	CDataModel* m_pDSealectModel;
	//##ModelId=3CAAA66B0340
	QU::UNIT m_eUnitSystem;

	//##ModelId=3C60D578035C
	CString m_sFormationMatDatabasePath;
	//##ModelId=3C60D578036B
	CString m_sCementMatDatabasePath;
	//##ModelId=3C60D578037B
	CString m_sCasingMatDatabasePath;

protected: // create from serialization only
	//##ModelId=3C0DC15C0246
	CDSealectDoc();
	DECLARE_DYNCREATE(CDSealectDoc)

// Attributes
public:
	//overide of the CDocument
	//##ModelId=3DDA0C57018A
	void SetModifiedFlag(BOOL bModified = TRUE);

	//##ModelId=3C0DC15C0251
	CDataModel *DSealectModel()       {return m_pDSealectModel;       }
	//##ModelId=3C20440202C0
	static CDSealectDoc *GetMe();
	//##ModelId=3C60D578039A
	CString FormationMatDatabasePath() {return m_sFormationMatDatabasePath;}
	//##ModelId=3C60D57803AA
	CString CasingMatDatabasePath() {return m_sCasingMatDatabasePath;}
	//##ModelId=3C60D57803BA
	CString CementMatDatabasePath() {return m_sCementMatDatabasePath;}

	//##ModelId=3DDA0C570199
	CString TempDir() {return m_sTempDir;}

	//##ModelId=3DDA0C57019A
	CString FileWorkDir() {return m_sFileWorkDir;}

	//##ModelId=3C0DC15C0252
	QU::UNIT UnitSystem()
	{
		return m_eUnitSystem;
	}
	//##ModelId=3C20440202DF
	void SetUnitSystem(QU::UNIT newsystem,BOOL bUpdateViews=true);
	//##ModelId=3DDA0C57019B
	void StartAnalysis();
	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSealectDoc)
	public:
	//##ModelId=3C0DC15C0253
	virtual BOOL OnNewDocument();

	//##ModelId=3C60D57803C9
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//##ModelId=3C60D57803D9
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	//##ModelId=3C0DC15C0255
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3DDA0C5701A9
	void SetTempDir(CString sPath);
	//##ModelId=3DDA0C5701B8
	void SetFileWorkDir(CString sPath);
	//##ModelId=3DDA0C5701BA
	void SetCasingMatDatabasePath(CString sPath);
	//##ModelId=3DDA0C5701C9
	void SetCementMatDatabasePath(CString sPath);
	//##ModelId=3DDA0C5701D8
	void SetFormationMatDatabasePath(CString sPath);
	//##ModelId=3DDA0C5701E7
	long GetIndexUnitCombo();
	//##ModelId=3DDA0C5701E8
	BOOL ContinueAndLoseResultsWarning();
	
	//##ModelId=3C0DC15C0261
	virtual ~CDSealectDoc();
#ifdef _DEBUG
	//##ModelId=3C0DC15C0263
	virtual void AssertValid() const;
	//##ModelId=3C0DC15C0265
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDSealectDoc)
	//##ModelId=3DDA0C5701E9
	afx_msg void OnAppRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSEALECTDOC_H__C6FB9130_2D00_43C6_89B6_8F2DCDF508E8__INCLUDED_)
