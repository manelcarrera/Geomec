#if !defined(AFX_RESULTEXPORTDLG_H__A673E124_0EF3_4FD6_8C40_878B63472DD1__INCLUDED_)
#define AFX_RESULTEXPORTDLG_H__A673E124_0EF3_4FD6_8C40_878B63472DD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The resultexport dlg is used to export the results of geomec on formations and pointsets. The dialog
// exists mainly of two tree controls. The first tree control allows the user to select a points, formations
// or the entire model. The second tree control allows the user to select the result for export. The user
// select the depletion stages, the analysis types he wants to export the results for. Results can be
// exported in ASCII, Excel, Gocad and Eclipse format. Note that only eclipse pointsets can support the
// export in eclipse format.

#include "afxwin.h"

#include "ExportFormat.h"
#include "ExportSelection.h"
#include "IExportDlg.h"
#include "ListCtrlBase.h"
#include "LocalResultObserver.h"
#include "OpenGLNodeSelection.h"
#include "ResultExportConfiguration.h"
#include "derivedresult.h"
#include "resultobserver.h"
#include "treectrlbase.h"

class CExportResultData;
class CResultGroup;
class CResultTree;
class CModelBase;

class CResultExportDlg : public IExportDlg {
  static TResultExportConfiguration defaultResultExportConfiguration;

  THorizonBaseSelection *m_pSlipHorizonSelection;
  THorizonBaseSelection *m_pHorizonSelection;
  TFormationSelection *m_pFormationSelection;
  TFormationPlaneSelection *m_pFormationPlaneSelection;
  TRTCISelection *m_pRTCISelection;
  TNewWellPathSelection *m_pNewWellPathSelection;
  TPointSetSelection *m_pPointSetSelection;
  TElementSetSelection *m_pElementSetSelection;
  TNonMeshedSurfaceSelection *m_pNonMeshedSurfaceSelection;

  const TResultExportConfiguration &m_resultExportConfiguration;

  typedef std::set<const CDepletionStage *> TTimeStepSet;
  TTimeStepSet m_stTimeStep;  // The map is looks up or a timestep is enabled.
  bool m_bExportNewWellpaths; // wjrx mantis 3322

  CModelBase &m_model;  // Reference to the model
  void UpdateHotSpot(); // Update the hotspot cb box
  void CollectDepletionStages();
  void UpdateTimeStep(); // Update the timestep list box

  CLocalResultGroupObserver<CResultExportDlg> *m_MatRGObserver;
  CLocalResultGroupObserver<CResultExportDlg> *m_RGObserver;
  CLocalResultGroupObserver<CResultExportDlg> *m_GVTRGObserver;

  std::set<const IValueComponentBase *> m_selection;

public:
  // Construction
  CResultExportDlg(CModelBase &model,
                   const TResultExportConfiguration &resultExportConfiguration = defaultResultExportConfiguration,
                   const CString &textCancel = "Close", bool buttonOK = false,
                   CWnd *pParent = 0); // no results selected initially
  CResultExportDlg(CResultGroup &result_group,
                   const TResultExportConfiguration &resultExportConfiguration = defaultResultExportConfiguration,
                   const CString &textCancel = "Close", bool buttonOK = false, CWnd *pParent = NULL);
  // When nIndex == -1 all the components of the result are selected
  CResultExportDlg(const IValueComposite &composite, int nIndex,
                   const TResultExportConfiguration &resultExportConfiguration = defaultResultExportConfiguration,
                   const CString &textCancel = "Close", bool buttonOK = false, CWnd *pParent = NULL);
  virtual ~CResultExportDlg();

  bool MultipleValueComponentsAllowed() { return true; }

  bool Selected(const IValueComponentBase *pComponent) const {
    return m_selection.find(pComponent) != m_selection.end();
  }

  void Select(const IValueComponentBase *pComponent) // toggle
  {
    std::pair<std::set<const IValueComponentBase *>::iterator, bool> retval = m_selection.insert(pComponent);
    if (!retval.second)
      m_selection.erase(retval.first);
  }

  // Is a result valid?
  BOOL Valid(const IValueComposite &composite) const;
  BOOL Valid(const IValueComponentBase &component) const;

  virtual void BranchToggle(bool bBranchToggle);

  // Timestep functions
  virtual bool TimeStep(const CDepletionStage &stage) const;
  virtual void TimeStep(const CDepletionStage &stage, bool bEnable);

  bool ResultComposite(const IValueComposite &composite) const;
  void ResultComposite(const IValueComposite &composite, bool bEnable);
  void MeshResultComposite(const IValueComposite &composite, bool bEnable);

  void SelectAllNewWellpaths(bool bSelect); // wjrx mantis 3322

  // Model Access
  CModelBase &Model() { return m_model; }
  const CModelBase &Model() const { return m_model; }

  // Update
  void UpdateButtons();

  virtual void OnSelect(const COpenGLNode &node);
  void setExportResultData(CExportResultData *exportResultData, bool updateData = false);
  INT_PTR getPathName(CString &pathName, DWORD &filterIndex);

private:
  void Init();
  // ASCII, EXCEL, GOCAD
  bool OnExport(const CString &sFileName, bool bExcel, bool bOld);
  bool OnExportGoCad(const CString &sFileName);
  void adjustVisibleResultComponents(CLocalResultGroupObserver<CResultExportDlg> *observer);
  void getExportResultData(CExportResultData *exportResultData, bool bUnbind = true);
  bool ValueComponentCanMapSelection(const IValueComponentBase &component) const;

public:
  // Dialog Data
  //{{AFX_DATA(CResultExportDlg)
  enum { IDD = IDD_EXPORT_DLG };
  CListCtrlBase m_lbTimeStep;
  CComboBox m_cbTimeStep;
  CComboBox m_cbHotSpot;
  CTreeCtrlBase m_tcObject;
  CTreeCtrlBase m_tcValue;
  int m_nWeightingType;
  int m_nUnit;
  int m_nTZ;
  BOOL m_bLinear;
  BOOL m_bNonLinear;
  BOOL m_bHeat;
  BOOL m_bMixture;
  BOOL m_bMixtureContainment;
  int m_nLinear;
  //}}AFX_DATA

  int m_nCenterPoints;
  int m_nFormationNames;
  int m_nWellPoints;

  const CGraphNode *m_InitiallyOn;
  int m_nInitialIndex;

  const CString m_textCancel;
  bool m_buttonOK;

private:
  CExportResultData *m_exportResultData;
  bool m_updateData;
  bool m_bBranchToggle;

  void initializeDialog(bool firstTime);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CResultExportDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CResultExportDlg)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  virtual void OnExport();
  void OnAnalysisTypeChanged();
  DECLARE_MESSAGE_MAP()
  virtual void OnOK();

public:
  afx_msg void OnImportExportMacro();
  afx_msg void OnSaveExportMacro();

private:
  CEdit m_macroName;

public:
  afx_msg void OnMacroName();
  afx_msg void OnEnableExport();

  virtual void toggleState() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTEXPORTDLG_H__A673E124_0EF3_4FD6_8C40_878B63472DD1__INCLUDED_)
