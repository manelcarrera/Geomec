#ifndef _DerivedMatLibDlg_H_
#define _DerivedMatLibDlg_H_

class CAnalysisLogger;
class CMaterialEntry;
class CMaterialEntry_Delegate;

#include "MatLibDlg.h"
#include "resource.h"

class CDerivedMatLibDlg : public CMatLibDlg
{
public:
  CDerivedMatLibDlg(CMaterialEntry_Delegate* materialEntry_Delegate,
    ml::CMaterialLibrary& matlib, int modelfilter, CAnalysisLogger& logger,
    bool bImportMode = false, UINT nIDTemplate = IDD_DERIVEDMATLIBDLG,
    CWnd* pParent = 0);

protected:
  virtual BOOL OnInitDialog();

private:
  afx_msg void OnImportMaterial();
  afx_msg void OnExportMaterial();
  afx_msg void OnItemchangedListmaterials(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_MESSAGE_MAP()

  CMaterialEntry_Delegate* m_materialEntry_Delegate;
};

#endif // _DerivedMatLibDlg_H_
