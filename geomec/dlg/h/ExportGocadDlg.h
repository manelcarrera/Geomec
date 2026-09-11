// ExportGocadDlg.h: interface for the CExportGocadDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTGOCADDLG_H__3247C381_5D3B_4735_B615_094B8ACB2219__INCLUDED_)
#define AFX_EXPORTGOCADDLG_H__3247C381_5D3B_4735_B615_094B8ACB2219__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <vector>

//#include "ExportCategories.h"
#include "IExportDlg.h"
#include "resultobserver.h"
#include "derivedresult.h"
#include "GocadExport.h"
#include "LocalResultObserver.h"

class IResult;
class CResultGroup;
class CResultTree;
class CDepletionStage;
class CModelBase;
class CFormationBase;
class CTetraSubHorizon;
class CPointSet;

#include "treectrlbase.h"
#include "ListCtrlBase.h"

class CExportGocadDlg : public IExportDlg  
{
  CLocalResultGroupObserver<CExportGocadDlg> *m_MatRGObserver;
  CLocalResultGroupObserver<CExportGocadDlg> *m_RGObserver;
  CLocalResultGroupObserver<CExportGocadDlg> *m_GVTRGObserver;
  const CResultGroup *m_InitiallyOn;

  std::set<const IValueComponentBase *> m_selection;

  class CObjectSelection
  {
  CExportGocadDlg& m_dlg;

  public:
  CObjectSelection(CExportGocadDlg& dlg);

 	  void populateTree();
  void select_formation(const CFormationBase& formation, bool bSelect);
  bool formation_selected(const CFormationBase& formation) const;
  void select_fault(const CTetraSubHorizon& fault, bool bSelect);
  bool fault_selected(const CTetraSubHorizon& fault) const;
  void select_pointset(const CPointSet& ptSet, bool bSelect);
  bool pointset_selected(const CPointSet& fault) const;

  bool MultipleValueComponentsAllowed()
  {
    return true;
  }
  // called by CSelectionBranchObserver_Delegate
  virtual void BranchToggle(bool bBranchToggle) {}
  };

  friend class CObjectSelection;

  CListCtrlBase	m_lbTimeStep;
  CTreeCtrlBase	m_tcResults;
  CTreeCtrlBase m_tcObjects;

  CGocadExport& m_source;
  CGocadExport* m_pCopy;

  CObjectSelection* m_current;

private:
  void CollectDepletionStages();
  void UpdateTimeStep();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  void OnExport();
  void OnAnalysisTypeChanged();

  DECLARE_MESSAGE_MAP()

public:
  CExportGocadDlg(CGocadExport& gocadexport, const CResultGroup* pResultGroup = 0, CWnd* pParent = 0);
  virtual ~CExportGocadDlg();

  CModelBase& Model();
  const CModelBase& Model() const;

  // to satisfy the pure virtual function in  IExportDlg
  virtual void OnSelect(const COpenGLNode& node) { assert(false); }

  bool MultipleValueComponentsAllowed()
  {
      return true;
  }

  // Timestep functions
  virtual bool TimeStep(const CDepletionStage& stage) const;
  virtual void TimeStep(const CDepletionStage& stage, bool bEnable);

  // Formation functions
  bool Formation(const CFormationBase& formation) const;
  void Formation(const CFormationBase& formation, bool bEnable);

  // Fault functions
  bool Fault(const CTetraSubHorizon& fault) const;
  void Fault(const CTetraSubHorizon& fault, bool bEnable);

  //PoinSet functions
  bool PointSet(const CPointSet& ptSet) const;
  void PointSet(const CPointSet& ptSet, bool bEnable);

  // Is a result valid?
  BOOL Valid(const IValueComposite& composite) const;
  BOOL Valid(const IValueComponentBase& component) const;

  bool ResultComposite(const IValueComposite& composite) const;
  void ResultComposite(const IValueComposite& composite, bool bEnable);

  virtual void toggleState() const {;}

  bool Selected(const IValueComponentBase *pComponent) const
  {
  return m_selection.find(pComponent) != m_selection.end();
  }

  void Select(const IValueComponentBase *pComponent) // toggle
  {
  std::pair<std::set<const IValueComponentBase *>::iterator, bool> retval = m_selection.insert(pComponent);
  if (!retval.second)
      m_selection.erase(retval.first);
  }
};

#endif // !defined(AFX_EXPORTGOCADDLG_H__3247C381_5D3B_4735_B615_094B8ACB2219__INCLUDED_)
