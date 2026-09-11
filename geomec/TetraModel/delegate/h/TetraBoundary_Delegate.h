#ifndef _TetraBoundary_Delegate_h_
#define _TetraBoundary_Delegate_h_

class CTetraBoundary;

#include "BoundaryBase_Delegate.h"
#include "AttriBoundaryDlg.h"

class CTetraBoundary_Delegate : public CBoundaryBase_Delegate
{
public:
  CTetraBoundary_Delegate(CTetraBoundary* tetraBoundary);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanModify() const;

  bool CanCreateInterfaces() const;
  void InterfaceAttributes();

  bool CanExportSurface() const;
  void ExportSurfaces();

private:
  CTetraBoundary_Delegate(const CTetraBoundary_Delegate& rhs);
  CTetraBoundary_Delegate& operator = (const CTetraBoundary_Delegate& rhs);

  CTetraBoundary* m_tetraBoundary;

  REGISTER_DELEGATE(CTetraBoundary, CTetraBoundary_Delegate);
};

class CAttriTetraBoundaryDlg : public CAttriBoundaryDlg <CTetraBoundary>
{
public:
  CAttriTetraBoundaryDlg(CTetraBoundary& boundary, CWnd* pParent = 0);

  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

#endif  // _TetraBoundary_Delegate_h_
