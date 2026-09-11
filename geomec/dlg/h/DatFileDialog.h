#pragma once

class CDianaRunController;
class CDepletionStage;
class CModelBase;

#include "TNOFileDialog.h"
#include "AnalysisType.h"

class CDatFileDialog : public CTnoFileDialog
{
public:

  struct Data
  {
    //
    // CDatFileDialog
    //
    CAnalysisType::TAnalysisType analysisType;
    bool writeInputFiles;
    bool writeQuadDat;
    const CModelBase& model;
    int timeout;
    //
    // CTnoFileDialog
    //
    BOOL bOpenFileDialog;  // TRUE FileOpen, FALSE FileSaveAs
    LPCTSTR lpszDefExt;// = NULL,
    LPCTSTR lpszFileName;// = NULL,
    DWORD dwFlags;// = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    LPCTSTR lpszFilter;// = NULL,
    CWnd* pParentWnd;// = NULL
  };

  //for convenience
  CDatFileDialog(const Data* d);

public:
  CDatFileDialog(CAnalysisType::TAnalysisType analysisType,
                 bool writeInputFiles,
                 bool writeQuadDat,
                 const CModelBase& model,
         int timeout,
         //
         // CTnoFileDialog
         //
                 BOOL bOpenFileDialog,  // TRUE FileOpen, FALSE FileSaveAs
                 LPCTSTR lpszDefExt = NULL,
                 LPCTSTR lpszFileName = NULL,
                 DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 LPCTSTR lpszFilter = NULL,
                 CWnd* pParentWnd = NULL);

#ifdef KK
protected:
  virtual BOOL OnFileNameOK();
#endif

private:
#ifdef KK
  const CDepletionStage* fileNames2GenerateExist(
  const QString& extension) const;
  QString stripExtension(const QString& string, const QString& extension) const;
#endif


  CAnalysisType::TAnalysisType m_analysisType;
  bool m_writeInputFiles;
  bool m_writeQuadDat;
  const CModelBase& m_model;
  int m_timeout;

protected:
  //{{AFX_MSG(SelSurfDlg)
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

const int MAX_BASENAME_LENGTH = 70;