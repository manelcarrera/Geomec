#if !defined(AFX_RESULTSELECTDLG_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_)
#define AFX_RESULTSELECTDLG_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_

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
#include "LocalResultObserver.h"
#include "ListCtrlBase.h"

// This dialog is used to retrieve operands for the RPN formula. The dialog distincs input
// data like point- and elementsets and output params like results. When a result is selected
// the user has specify or he wants a linear or non-linear and which deplection stage. When the
// global flag is setted the user is editing a formula for global use i.o.w. he can also use the
// formula in other project databases. When the global flag is set the user can only access the 
// predefined result in the program. These result are always available.
class CResultSelectDlg : public CDialog
{
	BOOL m_bGlobal;
	BOOL m_bInput;			// Selection between input and output
	void BuildTree();		// Builds the value tree with ITreeObjects
  void UpdateSelection();	// When the lin / non lin button or timestep combo is used the 
  // selection needs an update
  void UpdateName();
	CModelBase& m_model;
	BOOL m_bGenerateName;
	BOOL m_bSetLinStage;
  const CDerivedResult::CDerivedResultComponent* m_pResultBeingCreated;

  typedef std::set <const CDepletionStage*> TTimeStepSet;
  typedef std::map <CAnalysisType::TAnalysisType, size_t> TAnalysisTypes;

public:
	std::vector< const IValueComponentBase*> m_selected_inputs_v;

public:
	bool FixedLinStage() { return m_bSetLinStage != 0; }
  bool MultipleValueComponentsAllowed()
  {
      return true;
  }

  void AddResults(const TResultComponentSet& results)
  {
    m_Results.insert(results.begin(), results.end());
  }

	class CResultGroupObserver : public CLocalResultGroupObserver <CResultSelectDlg>
	{
		CResultSelectDlg& m_dlg;
	public:
		CResultGroupObserver(CResultGroup& group, CResultSelectDlg& dlg);

    virtual BOOL CanInsertResult(const IResult& result) const;
  	virtual BOOL CanInsertResultGroup(const CResultGroup& group) const;
  };

  class CPointSetObserver : public CChildEnumerator<CValueType>
	{
	public:
    // TODO AppendContextMenu
    // CValueTypeObserver is not derived from CGraphNode, hence
    // the function AppendContextMenu does not ask for a delegate. However
    // one could verify that this function is never used!


	private:
		IPointSet& m_point_set;
		CResultSelectDlg& m_dlg;
	public:
		CPointSetObserver(IPointSet& point_set, CResultSelectDlg& dlg);

		virtual const CGraphNode& ObservedItem() const;
		virtual CGraphNode& ObservedItem();

		virtual const CGraphNode_Delegate& Delegate() const;
		virtual CGraphNode_Delegate& Delegate();

		CResultSelectDlg& Dlg() { return m_dlg; }
		const CResultSelectDlg& Dlg() const { return m_dlg; }

		virtual CTreeNode* InsertChild(CValueType& value_type);
		virtual BOOL OnFilter(const CValueType& t) const;	

		virtual void AppendContextMenu(CContextMenuInvoker &invoker){}

    virtual void ResultToggleState(IValueComposite& result);
    virtual unsigned int ResultStateIconId(const IValueComposite& result) const;
 	};



  CResultSelectDlg(CModelBase& model, BOOL bGlobal, const CDerivedResult::CDerivedResultComponent* pResultBeingCreated = 0, CWnd* pParent = NULL);   // standard constructor
	const CModelBase& Model() const { return m_model; }
	CModelBase& Model() { return m_model; }
  bool Selected(const IValueComponentBase *pSelection) const;
	void Select(const IValueComponentBase* pSelection);
  QString Name(const IValueComponentBase* valueComponent) const;
	BOOL Global() const;

	bool Composite(const IValueComposite& composite) const;
	void Composite(const IValueComposite& composite, bool bEnable);

  const CDerivedResult::CDerivedResultComponent* ResultBeingCreated() const;

  bool Valid(const IValueComposite& composite) const;
  bool Valid(const IValueComponentBase& component) const;

  bool TimeStep(const CDepletionStage& stage) const;
  void TimeStep(const CDepletionStage& stage, bool bEnable);

  virtual void toggleState() const { const_cast<CResultSelectDlg *>(this)->UpdateSelection(); }
  // called by CSelectionBranchObserver_Delegate
  virtual void BranchToggle(bool bBranchToggle) {}

  class CResultComponentSetIterator
  {
    public:
      static struct TResultsFromDialog {} resultsFromDialog;

      CResultComponentSetIterator(CResultSelectDlg* resultSelectDlg);
      CResultComponentSetIterator(CResultSelectDlg* resultSelectDlg,
        TResultsFromDialog& resultsFromDialog);

      const IValueComponentBase* get();

    private:
      CResultComponentSetIterator(const CResultComponentSetIterator& rhs);
      CResultComponentSetIterator& operator = (
        const CResultComponentSetIterator& rhs);

      TResultComponentSet m_ResultComponentSet;
      TResultComponentSet::const_iterator m_ResultComponent;
      unsigned int m_Mode;
      unsigned int m_Component;
      bool m_Ended;
      std::set <CAnalysisType> m_AnalysisTypes;
      bool m_SetLinStage;
      TTimeStepSet m_stTimeStep;
  };

// Dialog Data
	//{{AFX_DATA(CResultSelectDlg)
	enum { IDD = IDD_SELECT_RESULT };
	CTreeCtrlBase	m_lbValue;
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultSelectDlg)
	afx_msg void OnInput();
	afx_msg void OnLinear();
	afx_msg void OnOutput();
	afx_msg void OnNonLinear();
  afx_msg void OnHeat();
  afx_msg void OnMixture();
  afx_msg void OnMixtureContainment();
	virtual BOOL OnInitDialog();
	afx_msg void OnCkName();
	afx_msg void OnSetLinStage();
	afx_msg void OnNameChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  void init();
  void CollectDepletionStages();
  void UpdateTimeStep();
  void onAnalysisType(int dialogItem,
    CAnalysisType::TAnalysisType analysisType);
  void mapAnalysisTypes2GUI(const TAnalysisTypes& analysisTypes) const;

  CResultGroupObserver *m_GVTRGObserver;
  CResultGroupObserver* m_RGObserver;
  CListCtrlBase m_lbTimeStep;
  TTimeStepSet m_stTimeStep;
  TResultComponentSet m_Results;
  std::set <CAnalysisType> m_AnalysisTypes;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTSELECTDLG_H__FA184D98_0D88_40A3_83D1_A2201706F02E__INCLUDED_)
