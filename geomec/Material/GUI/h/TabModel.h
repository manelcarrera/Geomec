#ifndef _TABMODEL_H_
#define _TABMODEL_H_

#include "Materials.h"
#include "MatLibListCtrl.h"

class CMatParamDlg;
class CLibraryMaterial;
class CLibraryMaterialParameter;

class CTabModel : public CDialog
{
public:
  CTabModel(CMatParamDlg& dlg, CWnd* pParent = NULL);
	unsigned int ID();

	void SelectedModel(mlMatModel nModel);
	mlMatModel SelectedModel();

	CMatParamDlg& Dlg();

	void Type(int iType);
	int Type();

  void UpdateControls();
  void VerticalResize( const int vdiff);

  static const int VALUE_COLUMN;

  const std::vector <QString> getMaterialParameterError() const;
  void setMaterialParameterError(
    const std::vector <QString>& materialParameterError);
  bool hasMaterialParameterErrors() const;

  typedef std::map<int, CLibraryMaterialParameter*> TItemParameterMap; // map from an item in the listctrl to the corresponding parameters

//  void SetLockedParameterNames(const TNameSet& stLockedParameterNames);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
  virtual void OnCancel();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnReset();
	afx_msg void OnSelchangeCboModel();
	afx_msg void OnSelchangeCboType();
	afx_msg void OnClickParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnBeginlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUpdateUnits(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnModelChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CLibraryMaterial* GetMaterial();
  const CLibraryMaterial* GetOriginalMaterial() const;
	void UpdateModelFrames();
	BOOL EnableMatModel(mlFilterOption nFilter);
//	void SendMessageToDialog(unsigned int uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
	void InputTypeChanged();
	int GetGroup();
	
	void FillModelListBox();
	void FillTypeListBox();
	void InitializeModel(mlMatModel nModel);
	void AddModelToCombo(mlMatModel nModel);
  void UpdateResetButton();
	void FillParameterListCtrl();
  int GetColumnCount();
		
private:
	enum { IDD = IDD_TABMODEL };
        int m_nParamTotal;

	CMatLibListCtrl	m_ListCtrl;
	int		m_iType;
	int		m_iModel;

	std::vector<mlMatModel> m_vcListedModels;

	CImageList m_ImageList;
	CMatParamDlg& m_Dlg;

	TItemParameterMap m_mpItemParameter;
	
	double m_dCurrent;
  std::vector <QString> m_materialParameterError;
public:
	afx_msg void OnLvnItemchangedParamList(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // _TABMODEL_H_
