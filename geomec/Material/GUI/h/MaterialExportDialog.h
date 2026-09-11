#ifndef _MaterialExportDialog_H_
#define _MaterialExportDialog_H_

#include "MaterialImportDialog.h"

class CMaterialExportDialog : public CMaterialImportDialog
{
public:
  CMaterialExportDialog(CMaterialEntry_Delegate* materialEntry_Delegate,
  CMaterialEntry* materialEntry, ml::CMaterialLibrary& matlib,
  int modelfilter, CAnalysisLogger& logger, bool bImportMode = false,
  UINT nIDTemplate = IDD_MATERIAL_IMPORT_EXPORT_DIALOG, CWnd* pParent = 0);

protected:
  virtual BOOL OnInitDialog();

private:
  afx_msg void OnItemchangedListmaterials(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnButDelete();

  DECLARE_MESSAGE_MAP()
};

#endif // _MaterialExportDialog_H_
