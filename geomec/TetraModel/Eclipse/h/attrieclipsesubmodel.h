#if !defined(AFX_ATTRIECLIPSESUBMODEL_H__9CF6B869_5993_4852_BEEA_546317A3F824__INCLUDED_)
#define AFX_ATTRIECLIPSESUBMODEL_H__9CF6B869_5993_4852_BEEA_546317A3F824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrieclipsesubmodel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseSubModel dialog
class CEclipseModel;

#include "treectrlbase.h"

class CAttriEclipseSubModel : public CDialog
{
public:
	// The classes for tree visualisation
	class CReservoirTreeObject : public ITreeObject
	{
	public:
		class CHorizonTreeObject : public ITreeObject
		{
			int m_nIndex;
		public:
			CHorizonTreeObject(CReservoirTreeObject& parent, int nHorizonIndex);
			virtual unsigned int StateIcon() const;
			virtual void ToggleState();
			virtual unsigned int Icon() const;
			virtual QString Text() const;
			virtual BOOL operator<(const ICtrlObjectBase &object) const;
			CAttriEclipseSubModel& Dlg();
			const CAttriEclipseSubModel& Dlg() const;
		};
	private:
		CAttriEclipseSubModel& m_dlg;
		int m_nIndex;
	public:
		CReservoirTreeObject(CAttriEclipseSubModel& dlg, int nReservoirIndex);
		CAttriEclipseSubModel& Dlg();
		const CAttriEclipseSubModel& Dlg() const;
		int Index() const;
		virtual unsigned int Icon() const;
		virtual QString Text() const;
		virtual BOOL operator<(const ICtrlObjectBase &object) const;
	};
private:
// Construction
	CEclipseModel& m_model;
	std::vector<BOOL> m_vcHorizon;
	int SelectCount() const;
	void Validate();
public:
	// Tree class
	CAttriEclipseSubModel(CEclipseModel& model, CWnd* pParent = NULL);   // standard constructor
	// Tree item go here
	BOOL HorizonEnable(int nHorizonIndex) const;
	void HorizonEnable(int nHorizonIndex, BOOL bHorizonEnable);
	// Model access
	CEclipseModel& Model();
	const CEclipseModel& Model() const;

// Dialog Data
	//{{AFX_DATA(CAttriEclipseSubModel)
	enum { IDD = IDD_ATTRI_ECLIPSE_SUB_MODEL };
	CTreeCtrlBase	m_tcHorizon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriEclipseSubModel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttriEclipseSubModel)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIECLIPSESUBMODEL_H__9CF6B869_5993_4852_BEEA_546317A3F824__INCLUDED_)
