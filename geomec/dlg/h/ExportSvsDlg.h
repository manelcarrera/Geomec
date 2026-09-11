#if !defined(__EXPORT_SVS_DLG_H__)
#define __EXPORT_SVS_DLG_H__

//#include <afxcmn.h>

#include "IExportDlg.h"
#include "treectrlbase.h"
#include "ListCtrlBase.h"
#include "ExportSelection.h"
#include "ExportFormat.h"
#include "SelectionObserver_Delegate.h"
#include "HorizonBase.h"
#include "PointSetEntryObserver.h"

class CModelBase;

// CExportSvsDlg dialog

class CExportSvsDlg : public IExportDlg
{
  DECLARE_DYNAMIC(CExportSvsDlg)

  CModelBase &m_Model;
  IOpenGLNodeSubSelection* m_pCurrentSelection;
  std::set<const CDepletionStage*> m_stDeplStages;
  TFormationSelection* m_pFormationSelection;
  TPointSetSelection* m_pPointSetSelection;
  std::vector<IOpenGLNodeSubSelection*> m_vcSelection;

public:
  CExportSvsDlg(CModelBase &model, CWnd* pParent = NULL);   // standard constructor
  virtual ~CExportSvsDlg();

  // Dialog Data
  //{{AFX_DATA(CExportSvsDlg)
  enum { IDD = IDD_EXPORT_SVS };
  BOOL m_bLinear;
  BOOL m_bNonLinear;
  BOOL m_bMixture;
  BOOL m_bMixtureContainment;
  BOOL m_bFormationNames;
  //}}AFX_DATA

  virtual void OnSelect(const COpenGLNode& node);
  virtual bool TimeStep(const CDepletionStage& stage) const;
  virtual void TimeStep(const CDepletionStage& stage, bool bEnable);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  //{{AFX_MSG(CExportSvsDlg)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
private:
  CTreeCtrlBase m_tcCategories;
  CListCtrlBase m_lcDeplStages;

  void UpdateTimeSteps();
  bool OnExport(const CString& sFileName, bool bExcel, bool bOld);
  void FillResultVector(IExportFormat::TDataVec& vcRet, const CAnalysisType& antype);
  
public:
  afx_msg void OnExport();
};
#endif