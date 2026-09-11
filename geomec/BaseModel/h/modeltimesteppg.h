#if !defined(AFX_MODELTIMESTEPPG_H__D618CC39_281A_4D08_AB62_CABD723EDD16__INCLUDED_)
#define AFX_MODELTIMESTEPPG_H__D618CC39_281A_4D08_AB62_CABD723EDD16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// modeltimesteppg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelTimestepPg dialog
#include <vector>
#include "ComboListCtrl.h"
#include "ISubListObject.h"
//#include "DepletionStage.h"
#include "geomectime.h"
#include "DepletionStage.h"

class CModelBase;
class CModelTimestepPg : public CPropertyPage
{
	class CTimeStepListObject : public IListObject
	{
    CModelTimestepPg& m_dlg;
		CGeomecTime m_Time;
		CString m_sName;
		CDepletionStage* m_pStage;
		CDepletionStage::eOutputType m_OutputType;
    bool m_bInserted;

	public:
		// Construction
		CTimeStepListObject(CModelTimestepPg& dlg, CListCtrl& ctrl, CDepletionStage& stage);
    CTimeStepListObject(CModelTimestepPg& dlg, CListCtrl& ctrl, int nIndex);
		CTimeStepListObject(CModelTimestepPg& dlg, CListCtrl& ctrl, const CString& sName, const CGeomecTime &time, CDepletionStage::eOutputType output_type);
		CTimeStepListObject(CTimeStepListObject& list_object);

    void Destroy();
		// Functions
		virtual QString Text() const;
		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString& strText);
		virtual unsigned int Icon() const;

    void Inserted(bool value) { m_bInserted = value; }
    const bool Inserted() const { return m_bInserted; }

    const CString &Name() { return m_sName; }
		// Time
		const CGeomecTime &Time() const;
    CGeomecTime &Time();
		void Time(const CGeomecTime &time);
  

		// Output type
		CDepletionStage::eOutputType OutputType() const;
		void setOutputType(CDepletionStage::eOutputType output_type);

		void Apply();
  private:
    void ShiftTimes(CGeomecTime time, int fromIndex);

	};
	
	class CYearSubObject : public ISubListObject
	{
		CTimeStepListObject &m_parent;
	public:
		CYearSubObject(CTimeStepListObject &parent);
		virtual QString Text() const;
		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString& strText);
	};

	class CMonthSubObject : public ISubListObject
	{
		CTimeStepListObject &m_parent;
	public:
		CMonthSubObject(CTimeStepListObject &parent);
		virtual QString Text() const;
		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString& strText);
	};

	class COutputSubObject : public ISubListObject
	{
		CTimeStepListObject& m_parent;

	public:
		COutputSubObject(CTimeStepListObject& parent);
		virtual QString Text() const;

		void setOutputType(CDepletionStage::eOutputType output_type);
		CDepletionStage::eOutputType OutputType() const;

		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString& strText);
	};

	CModelBase &m_model;
	int m_nTimeSteps;
	bool m_bCancelling;
public:
	//##ModelId=3BC55D6101DC
	CModelTimestepPg(CModelBase& model);
	//##ModelId=3BC55D6101EA
	~CModelTimestepPg();

	class CTSComboListCtrl : public CComboListCtrl
	{
	public:
		CTSComboListCtrl();

	protected:
		virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems);
	};

// Dialog Data
	//{{AFX_DATA(CModelTimestepPg)
	enum { IDD = IDD_ATTRI_MODEL_TIMESTEP };
	CTSComboListCtrl	m_lbTimestep;
	int		m_dd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CModelTimestepPg)
public:
	virtual void OnOK();
  virtual BOOL OnApply();
	virtual BOOL OnQueryCancel();
  void UpdateCleanupCheckbox();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnKillActive();
	// Generated message map functions
	//{{AFX_MSG(CModelTimestepPg)
	afx_msg void OnChangeTimesteps();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocuTimesteps();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void Apply();
	int Year(const int nIndex) const;
	void Decrease(const int n);
	void Increase(const int n);
  void Insert(const int prev, const int next);
	bool IsNumerique(const CString& s);
  void CheckOutputTypeLastStage();
  bool CheckTimes() const;
  bool HasRestarts();

public:
  afx_msg void OnPopupInsertbefore();
  afx_msg void OnNMRclickLbTimestep(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnPopupInsertafter();
  afx_msg void OnPopupDelete();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELTIMESTEPPG_H__D618CC39_281A_4D08_AB62_CABD723EDD16__INCLUDED_)
