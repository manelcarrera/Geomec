#ifndef _SELECTFORMATIONSDLG_H_
#define _SELECTFORMATIONSDLG_H_

class IFormationFilter;

#include "IListObject.h"
#include "ListCtrlBase.h"

class CSelectFormationsDlg : public CDialog
{
public:
  CSelectFormationsDlg(CStorageNode& container, const IFormationFilter *formationFilter = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();

private:
  class CFormationObserver : public IListObject
  {
  public:
  CFormationObserver(const CFormationBase& formation, CSelectFormationsDlg& dlg, CListCtrl& ctrl);
  virtual unsigned int Icon() const;
  virtual QString Text() const;
  virtual unsigned int StateIcon() const;
  virtual void ToggleState();

  private:
  const CFormationBase& m_formation;
  CSelectFormationsDlg& m_dlg;
  };

  friend class CFormationObserver;

private:
  const IFormationFilter *m_formationFilter;
  CStorageNode& m_container;
  CListCtrlBase m_lbFormations;
  typedef std::set<const CFormationBase*> TFormationSet;
  TFormationSet m_stFormations;

private:
  void UpdateFormationsListBox();

private:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnBnClickedSelectAll();
  afx_msg void OnBnClickedSelectNone();
  afx_msg void OnBnClickedInvert();
};

// supply an instance of this class to the above dialog
// if you just need a set of formations
class CSelectFormationsSetNode : public CStorageNode
{
public:
  typedef std::set<const CFormationBase*> TFormationsSet;

public:
  CSelectFormationsSetNode(TFormationsSet& stFormations, CFemAppModel& model);

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

 	virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual bool Empty() const;
  virtual long SavedItems() const;

private:
  TFormationsSet& m_stFormations;
};

#endif // _SELECTFORMATIONSDLG_H_
