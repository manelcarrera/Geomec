#if !defined(AFX_ResultSingleSelectDialog_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_)
#define AFX_ResultSingleSelectDialog_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DerivedResult.h"
#include "ResultObserver.h"
#include "ValueType.h"
#include "ValueComponent.h"
#include "MeshResultTree.h"

class CResultGroup;
class IPointSet;
class CModelBase;
class CDepletionStage;

#include "treectrlbase.h"

// This dialog is used to retrieve operands for the RPN formula. The dialog distincs input
// data like point- and elementsets and output params like results. When a result is selected
// the user has specify or he wants a linear or non-linear and which deplection stage. When the
// global flag is setted the user is editing a formula for global use i.o.w. he can also use the
// formula in other project databases. When the global flag is set the user can only access the 
// predefined result in the program. These result are always available.


class CResultSingleSelectDialog : public CDialog
{
	BOOL m_bGlobal;
	BOOL m_bInput;			// Selection between input and output
	CAnalysisType m_antype;
	void BuildTree();		// Builds the value tree with ITreeObjects
	void UpdateSelection();	// When the lin / non lin button or timestep combo is used the 
							// selection needs an update
	void UpdateName();
	const CDepletionStage* m_pTimeStep;
	const IValueComponentBase* m_pSelection;
	CModelBase& m_model;
	BOOL m_bGenerateName;
	BOOL m_bSetLinStage;
  const CDerivedResult::CDerivedResultComponent* m_pResultBeingCreated;
public:
  bool MultipleValueComponentsAllowed()
  {
    return false;
  }

	bool FixedLinStage() { return m_bSetLinStage != 0; }
	class CResultGroupObserver : public IResultGroupObserver
	{
		CResultSingleSelectDialog& m_dlg;
	public:
		CResultGroupObserver(CResultGroup& group, CResultSingleSelectDialog& dlg);

		virtual unsigned int ResultComponentStateIconId(const IValueComposite& result, int nComponent) const;
    virtual void ResultComponentToggleState(IValueComposite& result, int nComponent);
		virtual void AppendResultComponentContextMenu(IResult_Delegate& result, int nComponent, CContextMenuInvoker &invoker);
    virtual BOOL CanInsertResult(const IResult& result) const;
  	virtual BOOL CanInsertResultGroup(const CResultGroup& group) const;

    virtual BOOL Valid(const CResultGroup& group) const;
    virtual CTreeNode* InsertChild(CGraphNode& node);
    virtual BOOL OnFilter(const CGraphNode& node) const;

    unsigned int ResultStateIconId(const IValueComposite& value_type) const
    {
      if (value_type.ComponentSize() > 1)
        return 0;

      if (&value_type.Component() == m_dlg.Selection())
        return IDI_RADIO_CHECKED;
      return IDI_RADIO_UNCHECKED;
    }

    void ResultToggleState(const IValueComposite& value_type)
    {
      if (value_type.ComponentSize() == 1)
      {
        m_dlg.Select(&value_type.Component());
      }
    }
	};

	// The result group node 

  // TODO AppendContextMenu
  // CPointSetObserver is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However
  // one could verify that this function is never used!

	class CPointSetObserver : public CChildEnumerator<CValueType>
	{
	public:
    // TODO AppendContextMenu
    // CValueTypeObserver is not derived from CGraphNode, hence
    // the function AppendContextMenu does not ask for a delegate. However
    // one could verify that this function is never used!

	private:
		IPointSet& m_point_set;
		CResultSingleSelectDialog& m_dlg;
	public:
		CPointSetObserver(IPointSet& point_set, CResultSingleSelectDialog& dlg);

		virtual const CGraphNode& ObservedItem() const;
		virtual CGraphNode& ObservedItem();

		virtual const CGraphNode_Delegate& Delegate() const;
		virtual CGraphNode_Delegate& Delegate();

		CResultSingleSelectDialog& Dlg() { return m_dlg; }
		const CResultSingleSelectDialog& Dlg() const { return m_dlg; }

		virtual CTreeNode* InsertChild(CValueType& value_type);
		virtual BOOL OnFilter(const CValueType& t) const;	

		virtual void AppendContextMenu(CContextMenuInvoker &invoker){}

    unsigned int ResultStateIconId(const IValueComposite& value_type) const
    {
      if (value_type.ComponentSize() > 1)
        return 0;

      if (&value_type.Component() == Dlg().Selection())
        return IDI_RADIO_CHECKED;
      return IDI_RADIO_UNCHECKED;
    }

    void ResultToggleState(const IValueComposite& value_type)
    {
      if (value_type.ComponentSize() == 1)
      {
        Dlg().Select(&value_type.Component());
      }
    }
  };



  CResultSingleSelectDialog(CModelBase& model, BOOL bGlobal, const CDerivedResult::CDerivedResultComponent* pResultBeingCreated = 0, CWnd* pParent = NULL);   // standard constructor
	CResultSingleSelectDialog(CDerivedResult::CDerivedResultComponent::CValueComponentProxy& proxy, BOOL bGlobal, CWnd* pParent = NULL);
	const CModelBase& Model() const { return m_model; }
	CModelBase& Model() { return m_model; }
	const CAnalysisType& AnalysisType() const { return m_antype; } 
	const CDepletionStage& TimeStep() const { assert(m_pTimeStep); return *m_pTimeStep; } 
	const IValueComponentBase* Selection() const { return m_pSelection; }
	const IValueComponentBase* Selection() { return m_pSelection; }
  bool Selected(const IValueComponentBase *pComponent) const { return m_pSelection == pComponent; }
	void Select(const IValueComponentBase* pSelection);
	const CString& Name() const { return m_sName; }
	BOOL Global() const;

	bool Composite(const IValueComposite& composite) const;
	void Composite(const IValueComposite& composite, bool bEnable);

  const CDerivedResult::CDerivedResultComponent* ResultBeingCreated() const;

  bool Valid(const IValueComposite& composite) const;
  bool Valid(const IValueComponentBase& component) const;

// Dialog Data
	//{{AFX_DATA(CResultSingleSelectDialog)
	enum { IDD = IDD_SINGLE_SELECT_RESULT };
	CComboBox	m_cbTimeStep;
	CTreeCtrlBase	m_lbValue;
	CString	m_sName;
	//}}AFX_DATA

	// called by CSelectionBranchObserver_Delegate
	virtual void BranchToggle(bool bBranchToggle) {}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultSingleSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultSingleSelectDialog)
	afx_msg void OnInput();
	afx_msg void OnLinear();
	afx_msg void OnOutput();
	afx_msg void OnNonLinear();
  afx_msg void OnHeat();
  afx_msg void OnMixture();
  afx_msg void OnMixtureContainment();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTimeStep();
	afx_msg void OnCkName();
	afx_msg void OnSetLinStage();
	afx_msg void OnNameChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ResultSingleSelectDialog_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_)
