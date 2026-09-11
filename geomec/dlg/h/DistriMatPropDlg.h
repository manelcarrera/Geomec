#ifndef _DISTRIMATPROPDLG_H_
#define _DISTRIMATPROPDLG_H_

class CValueType;

#include "treectrlbase.h"

class CDistriMatPropDlg : public CDialog
{
public:
  CDistriMatPropDlg(const std::vector<const CValueType*>& vcValueTypes, std::set<const CValueType*>& stExtrapolatingValueTypes, CWnd* pParent = 0);
  ~CDistriMatPropDlg();

  void OnValueTypeSelected(const CValueType& valuetype);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()

private:
  void PopulateTree();
  void OnApplyToAll();
  void OnExtrapolate();
  void OnMatLibVal();
  void UpdateRadioButtons();

private:
  class CMatPropObserver : public ITreeObject
  {
  public:
  CMatPropObserver(const CValueType& valuetype, CTreeCtrl& ctrl, CDistriMatPropDlg& dlg);
  virtual BOOL CanDelete();
  	virtual BOOL OnSelect();
    virtual QString Text() const;
    virtual unsigned int Icon() const;

  private:
  const CValueType& m_valuetype;
  CDistriMatPropDlg& m_dlg;
  };

private:
  const std::vector<const CValueType*>& m_vcValueTypes;
  std::set<const CValueType*>& m_stSource;
  std::set<const CValueType*> m_stCopy;
  CTreeCtrlBase m_tcProperties;
  const CValueType* m_pCurrent;
};

#endif // _DISTRIMATPROPDLG_H_
