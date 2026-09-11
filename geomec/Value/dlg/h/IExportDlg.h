#ifndef _IEXPORTDLG_H_
#define _IEXPORTDLG_H_

class CDepletionStage;

#include "resourceIDI.h"
#include "OpenGLNodeSelection.h"
#include "LocalResult.h"

class IExportDlg : public CDialog
{
public:
  IExportDlg(unsigned int nID, CWnd* pParent);
  virtual void OnSelect(const COpenGLNode& node) = 0;
  virtual bool TimeStep(const CDepletionStage& stage) const = 0;
  virtual void TimeStep(const CDepletionStage& stage, bool bEnable) = 0;

  COpenGLNodeSelection& Selection();
  const COpenGLNodeSelection& Selection() const;

  // called by CSelectionBranchObserver_Delegate
  virtual void BranchToggle(bool bBranchToggle) {}

  virtual void AddResults(const TResultComponentSet&) {}

private:
  class CSelectionObserver : public COpenGLNodeSelection::CObserver
  {
  public:
  CSelectionObserver(IExportDlg& dlg);
  virtual void OnNodeErased(const COpenGLNode& node);
  virtual void OnNodeInserted(const COpenGLNode& node);

  private:
  IExportDlg& m_dlg;
  };

private:
  COpenGLNodeSelection m_selection;
};

#endif // _IEXPORTDLG_H_
