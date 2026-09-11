#ifndef _MaterialImportDialog_H_
#define _MaterialImportDialog_H_

class CAnalysisLogger;
class CMaterialEntry;
class CMaterialEntry_Delegate;

#include "MatLibDlg.h"
#include "resource.h"

class CMaterialImportDialog : public CMatLibDlg {
public:
  CMaterialImportDialog(CMaterialEntry_Delegate *materialEntry_Delegate, CMaterialEntry *materialEntry,
                        ml::CMaterialLibrary &matlib, int modelfilter, CAnalysisLogger &logger,
                        bool bImportMode = false, UINT nIDTemplate = IDD_MATERIAL_IMPORT_EXPORT_DIALOG,
                        CWnd *pParent = 0);

protected:
  virtual BOOL OnInitDialog();

  CMaterialEntry *m_materialEntry;
  CMaterialEntry_Delegate *m_materialEntry_Delegate;

private:
  afx_msg void OnBrowseMaterialLibrary();

  DECLARE_MESSAGE_MAP()
};

#endif // _MaterialImportDialog_H_
