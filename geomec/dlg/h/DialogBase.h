#pragma once

class CWnd;

class CDialogBase : public CDialog {
public:
  CDialogBase(UINT nIDTemplate, CWnd *pParentWnd = NULL);
  virtual ~CDialogBase();
};
