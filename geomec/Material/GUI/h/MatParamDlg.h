#ifndef _MATPARAMDLG_H_
#define _MATPARAMDLG_H_

class CTabExperiment;
class Worksheets;
class CExcelCell;
class CModelBase;

#include "ExperimentData.h"
#include "HistoryList.h"
#include "Materials.h"

class CAnalysisLogger;
class CLibraryMaterial;
class CGammaView;
class CResultData;

namespace {
const int NR_TABS = 2;
}

namespace ml {
class CMaterialLibrary;
}

#include "MatParamDlgDianaSignals.h"
//
// DialogBase now does nothing but in the future some functionality can be added
//
#include "DialogBase.h"

class CMatParamDlg : public CDialogBase {
  // BOOL PreTranslateMessage(MSG* pMsg);

public:
  CMatParamDlg(CLibraryMaterial &mat, ml::CMaterialLibrary &matlib, int modelfilter, CAnalysisLogger &logger,
               CWnd *pParent = 0);
  ~CMatParamDlg();

  void OnSave();
  void OnSaveAs();
  void OnReset();
  void OnUpdateGraphs();

  const CLibraryMaterial *Material() const;
  CLibraryMaterial *Material();
  const CLibraryMaterial *OriginalMaterial() const;

  int UnitDef() const;
  int MaterialModelFilter() const;

  void InitializeModel(mlMatModel nModel);
  virtual void OnCancel();
  void OnChildViewSized();
  void OnExperimentFileListUpdated(int nItems, int nSelected, const int *piSelected, bool useInitialLowerLimit,
                                   const std::vector<int> &initialLowerLimit);
  void UpdatePQRadios(bool bHasItems);
  void AppendToHistory(const std::vector<QString> &materialParameterError = std::vector<QString>());
  void adjustCalculatedData(int nSelected, const int *piSelected);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);

private:
  bool checkAllowMaterialChange();

  void UpdateGraphs(int nItems, int nSelected, const int *piSelected, bool useInitialLowerLimit,
                    const std::vector<int> &initialLowerLimit);
  void UpdateSigmaEpsilonGraph(const CExperimentArray &aData, const CResultData *pResData, bool useInitialLowerLimit,
                               const std::vector<int> &initialLowerLimit);
  void UpdatePQGraph(const CExperimentArray &aData, const CResultData *pResData, const CLibraryMaterial *pMat);
  void CreateTabs();
  void CreateViews();
  void CreateTabSheets();
  void StoreControlPositions();
  void UpdateCaption();
  void SetTabSheetData();
  void GetTabSheetData();
  void MoveControls();
  void ResizeViews();
  bool Calibrate();
  void WriteToFilos(const char *filos_path);
  void UpdateToCurrentHistoryItem();
  void UpdateHistoryButtons();

  void collectExperimentData(CTabExperiment *pTab, CExperimentArray &aData, CResultData **pResData, int nItems,
                             int nSelected, const int *piSelected) const;
  void exportCalibratedData(const QString &fileName) const;
  void exportCalibratedData(const QString &fileName, const CExperimentArray &experimentArray,
                            const CResultData *resultData) const;
  void exportCalibratedData(Worksheets &wss, const CExperimentArray &experimentArray, const CResultData *resultData,
                            short s) const;
  void exportCalibratedData(_Worksheet &ws, const CExperimentData &experimentData, const CResultData *resultData,
                            short s) const;
  void exportCalibratedDataHeader(CExcelCell &cell, const CStressStrainArray &strainSteps,
                                  const CResultData *resultData) const;
  void exportCalibratedDataUnits(CExcelCell &cell, const CStressStrainArray &strainSteps,
                                 const CResultData *resultData) const;

  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnWindowPosChanging(WINDOWPOS *lpwndpos);
  afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnSigmaepsilon();
  afx_msg void OnPq();
  afx_msg void OnFitElastic();
  afx_msg void OnFitPlastic();
  afx_msg void OnSettings();
  afx_msg void OnNext();
  afx_msg void OnPrevious();
  afx_msg void OnSI();
  afx_msg void OnField();
  afx_msg void OnExport();
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnDianaXCloseDialog();
  afx_msg void OnDianaXDiaMessage(LPCTSTR strMessage);
  afx_msg void OnDianaXDiaWarning(LPCTSTR strMessage);
  afx_msg void OnDianaXDiaError(LPCTSTR strMessage);

private:
  CModelBase *m_pModel;
  CLibraryMaterial *m_pOriginalMaterial;
  CLibraryMaterial *m_pMaterial;
  ml::CMaterialLibrary &m_matlib;
  int m_modelfilter;
  bool m_bInitialized;
  int m_iGraphType;
  CDialog *m_pTab[NR_TABS];
  int m_nUnitDef;
  CTabCtrl m_Tabs;
  CSplitterWnd *m_pViews;
  CGammaView *m_pGammaView[2];
  CResultData *m_pResData;
  bool m_bFitElastic;
  CMatParamDlgDianaSignals m_matParamDlgDianaSignals;

  class CHistoryItem {
  public:
    CHistoryItem(const CLibraryMaterial &mat, const CResultData *pRes,
                 const std::vector<QString> &materialParameterError);
    CHistoryItem(const CHistoryItem &rhs);
    ~CHistoryItem();
    const CLibraryMaterial &Material() const;
    const CResultData *ResultData() const;
    const std::vector<QString> &materialParameterError() const;

  private:
    CLibraryMaterial *m_pMat;
    CResultData *m_pRes;
    std::vector<QString> m_materialParameterError;
  };

  CHistoryList<CHistoryItem> m_History;

  // control positions and sizes for resizing
  CSize m_sizCurSize;
  CPoint m_ptClose;
  CPoint m_ptHelp;
  CPoint m_ptSettings;
  CPoint m_ptFitElastic;
  CPoint m_ptFitPlastic;
  CPoint m_ptUnits;
  CPoint m_ptGraphFrame;
  CPoint m_ptSigEps;
  CPoint m_ptPQ;
  CPoint m_ptUnitsFrame;
  CPoint m_ptSI;
  CPoint m_ptField;
  CPoint m_ptExport;
  CPoint m_ptSplitterWnd;
  CSize m_sizSplitterWnd;
};

#endif // _MATPARAMDLG_H_
