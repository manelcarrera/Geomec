#if !defined(AFX_RENAMEMATERIALDLG_H__6DC39C41_CFF9_11D3_B3AC_00A0C941C60B__INCLUDED_)
#define AFX_RENAMEMATERIALDLG_H__6DC39C41_CFF9_11D3_B3AC_00A0C941C60B__INCLUDED_

#include <vector>

#include "Materials.h"

namespace ml {
  class CMaterial;
}

class CRenameMaterialDlg : public CDialog
{
public:
  CRenameMaterialDlg(const QString& sName = QString(), mlMatModel matmodel = MM_LINEAR, bool bAllowModelChange = false, int nFilter = 0, CWnd* pParent = NULL);   // standard constructor
  const QString& Name() const;
  mlMatModel MaterialModel() const;
 
protected:
  virtual void DoDataExchange(CDataExchange* pDX);

private:
  QString m_strName;
  mlMatModel m_matmodel;

  std::vector<mlMatModel> m_vcListedModels;
  bool m_bAllowModelChange;
  int m_nModelFilter;
  int m_iModelComboIndex;

  enum { IDD = IDD_RENAMEMATERIALDLG };


  void AddListedModel(mlMatModel mm);
  void FillModelListBox();
  void AddModelToCombo(mlMatModel nModel);

protected:
  virtual void OnOK();
  virtual BOOL OnInitDialog();

public:
  CComboBox m_MaterialTypeCombo;
};

#endif // !defined(AFX_RENAMEMATERIALDLG_H__6DC39C41_CFF9_11D3_B3AC_00A0C941C60B__INCLUDED_)
