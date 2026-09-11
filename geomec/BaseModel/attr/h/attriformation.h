#if !defined(AFX_ATTRITETRAFORMATION_H__DD8819AE_7761_4FFD_B44E_A055DB103225__INCLUDED_)
#define AFX_ATTRITETRAFORMATION_H__DD8819AE_7761_4FFD_B44E_A055DB103225__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AttriFormationTemplate.h"
#include "TetraFormation.h"
#include "ISubListObject.h"
#include "ListCtrlBase.h"
#include "ElementOrderSubDialog.h"

class CAttriFormation : public CAttriFormationTemplate<CFormationBase>
{
	// Object displays the volume in listobject
	class CElementSetObject : public IListObject
	{
		class CPressureObject : public ISubListObject
		{
			const CElementSetObject& m_parent;
		public:
			CPressureObject(CElementSetObject& parent);
			virtual QString Text() const;
		};
		// Object display the size of the volume in the list object
		class CSizeObject : public ISubListObject
		{
			const CElementSetObject& m_parent;
		public:
			CSizeObject(CElementSetObject& parent);
			virtual QString Text() const;
		};

		const IFormationElementSet& m_elementset;
	public:
		CElementSetObject(CListCtrlBase& ctrl, const IFormationElementSet& elementset);
		virtual QString Text() const;
		virtual unsigned int Icon() const;
		const IFormationElementSet& ElementSet() const;
	};
public:
	CAttriFormation(CFormationBase& formation, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttriFormation)
	enum { IDD = IDD_ATTRI_BODY_FORMATION };
	CListCtrlBase	m_lcElementSet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriFormation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttriFormation)
	virtual BOOL OnInitDialog();
	afx_msg void OnStColor();
	afx_msg void OnPaint();
  afx_msg void OnBnClickedInheritFromModel();
  virtual void OnOK();
  //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  CElementOrderSubDialog m_elementOrderSubDialog;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRITETRAFORMATION_H__DD8819AE_7761_4FFD_B44E_A055DB103225__INCLUDED_)
