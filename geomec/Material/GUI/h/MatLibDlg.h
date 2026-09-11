#ifndef _MATLIBDLG_H_
#define _MATLIBDLG_H_

namespace ml {
  class CMaterialLibrary;
  class CMaterial;
}

class CMatLibDlg : public CDialog
{
public:
  CMatLibDlg(ml::CMaterialLibrary& matlib, int modelfilter, CAnalysisLogger& logger, bool bImportMode = false, UINT nIDTemplate = IDD_MATLIBDLG, CWnd* pParent = 0);
  std::vector<ml::CMaterial*> SelectedMaterials();

protected:
	virtual BOOL OnInitDialog();
  virtual void OnOK();

  afx_msg void OnItemchangedListmaterials(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnButDelete();

  std::vector<ml::CMaterial*> CurrentSelection();

private:
  int GetColumnCount();

	void InitListControls();
	void FillListControls();
	static int CALLBACK CompareFuncMatList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFuncParamList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
  void UpdateParameterList();
  void DisplayMaterial(const ml::CMaterial& mat);
  void SelectMaterial(const ml::CMaterial& mat);

	afx_msg void OnEndlabeleditListmaterials(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListmaterials(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListparameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButNew();
	afx_msg void OnButEdit();
	afx_msg void OnDblclkListmaterials(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
  ml::CMaterialLibrary& m_matlib;
  CImageList m_ImageList;

  int m_nModelFilter;
  CAnalysisLogger& m_logger;
	static bool m_bMatListAscending;
	static bool m_bParamListAscending;
	static int m_iSortColumn;
  int m_nUnitDef;
  bool m_bImportMode;
  std::vector<ml::CMaterial*> m_vcSelection;
};

#endif // _MATLIBDLG_H_
