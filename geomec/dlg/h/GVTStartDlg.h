#if !defined(GVTSTARTDLG_H_)
#define GVTSTARTDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComboListCtrl.h"
#include "ISubListObject.h"
#include "FormationBase.h"
#include "GVTSettings.h"
#include "TimeLapseControlObjects.h"

class CGVTSettings;
class CFormationBase;

// CGVTSettingsDlg dialog

class CGVTSettingsDlg : public CDialog
{
  class CFormationComboListCtrl : public CComboListCtrl
  {
  public:
  CFormationComboListCtrl(){}

  protected:
    virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems);
  };

  class CParametersComboListCtrl : public CComboListCtrl
  {
  public:
  CParametersComboListCtrl(){}

  protected:
  virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems){}
  };

 /* class CTimeLapseComboListCtrl : public CComboListCtrl
  {
  public:
  CTimeLapseComboListCtrl();

  protected:
  virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems);
  };

  class CTimeLapseListObject : public IListObject
  {
  CDepletionStage *m_t1;
  CDepletionStage *m_t2;
  CGVTSettings *m_GVTSettings;

  CDepletionStage *Update(const CString &strDepl);
  public:
  CTimeLapseListObject(CListCtrl& ctrl, CDepletionStage *t1, CDepletionStage *t2, CGVTSettings *settings);
  virtual QString Text() const;
  virtual BOOL CanEditTet() const;
  virtual unsigned int Icon() const;

  void SetTime1(const CString &strDepl);
  void SetTime2(const CString &strDepl);
  const CDepletionStage *Time1() const { return m_t1; }
  const CDepletionStage *Time2() const { return m_t2; }

  
  };

  class CTimeLapseSubObject : public ISubListObject
  {
  CTimeLapseListObject &m_parent;
  public:
  CTimeLapseSubObject(CTimeLapseListObject &parent, int nIndex);
  virtual QString Text() const;
  virtual BOOL EditText(const QString& strText);
    virtual BOOL CanEditText() const;
  };
  */

  class CParametersListObject : public IListObject
  {
  CGVTVelocityModel *m_VelMod;
  CString m_Parameter;
  public:
  CParametersListObject(CListCtrl& ctrl, CGVTVelocityModel *velmod, CString param);
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
  virtual unsigned int Icon() const;
  
  CGVTVelocityModel *VelocityModel();
  const CString &Parameter() const;
  

  };

  class CParamSubObject : public ISubListObject
  {
  CParametersListObject &m_parent;
  public:
  CParamSubObject(CParametersListObject &parent);
  
  virtual QString Text() const;
  virtual BOOL EditText(const QString& strText);
    virtual BOOL CanEditText() const;
  };

  class CFormationListObject : public IListObject
  {
  CFormationBase *m_Formation;
  CGVTVelocityModel *m_VelModel;
  CGVTSettingsDlg &m_ParentDlg;
  public: 
  CFormationListObject(CListCtrl& ctrl, CFormationBase *formation, 
      CGVTVelocityModel *velmodel, CGVTSettingsDlg &parentDlg);
  // Text
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
    //virtual BOOL EditText(const QString& strText);
  virtual unsigned int Icon() const;

  CGVTVelocityModel *VelocityModel();
  const CGVTVelocityModel *VelocityModel() const;
  void SetVelocityModel(eVelocityModel velmodel);
  };

  class CVelModelSubObject : public ISubListObject
  {
    CFormationListObject &m_parent;
  public:
    CVelModelSubObject(CFormationListObject &parent);

  virtual QString Text() const;
  virtual BOOL EditText(const QString& strText);
    virtual BOOL CanEditText() const;
  };

  DECLARE_DYNAMIC(CGVTSettingsDlg)
  CGVTSettings &m_SourceSettings;
  CGVTSettings m_GVTSettings;
  //int m_nCurrentSurvey;

  CString m_strVelocityModel;
  CString m_strOutputPrefix;
  CString m_strOutputDirectory;
  int m_ndvp;
  int m_ndt;
  int m_nImportBack;
  int m_velocitySource;

  friend class CFormationListObject;
public:
  CGVTSettingsDlg(CGVTSettings &settings, CWnd* pParent = NULL);   // standard constructor
  virtual ~CGVTSettingsDlg();

  const CGVTSettings &GVTSettings() const { return m_GVTSettings; }
  CGVTSettings &GVTSettings() { return m_GVTSettings; }

  


// Dialog Data
  //{{AFX_DATA(CGVTSettingsDlg)
  enum { IDD = IDD_GVT_SETTINGS };
  CFormationComboListCtrl m_lbFormations;
  CParametersComboListCtrl m_lbParameters;
  CTimeLapseComboListCtrl m_lbTimeLapse;
  //}}AFX_DATA

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedDefineNew();
private:
  void UpdateSurveyBox();
  void SetupTimeLapseListCtrl();
  void SetupFormationListCtrl();
  void SetupParamsListCtrl();
  void SetParametersList(int nItem);
  bool allMaterialModelsHaveVelocityP() const;
  void setupVelocityP();
public:
  afx_msg void OnCbnSelchangeGvtSurveyCombo();
  afx_msg void OnBnClickedGvtEditBut();
  afx_msg void OnBnClickedGvtBrowseVelocity();
  afx_msg void OnBnClickedGvtBrowseOutput();
  afx_msg void OnNMClickGvtFormationList(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedAddTimelapseButton();
  afx_msg void OnBnClickedRemoveTimelapseButton();
  afx_msg void OnBnClickedGvtDeleteBut();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedGvtVelocityFromFile();
  afx_msg void OnBnClickedGvtVelocityFromMaterial();
  afx_msg void OnBnClickedGvtCheckbox();
};

#endif
