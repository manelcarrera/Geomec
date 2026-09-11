#ifndef _CONSISTENCY_GUARD_SETTINGS_PG_H_
#define _CONSISTENCY_GUARD_SETTINGS_PG_H_

#include "afxdlgs.h"
#include "afxwin.h"

class CModelBase;
#include "ConsistencyGuard.h"

class CConsistencyGuardSettingsPg : public CPropertyPage {
  CModelBase *m_pModel;

  CConsistencyGuardOptions m_options;

public:
  CConsistencyGuardSettingsPg(CModelBase *pModel);

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();

  afx_msg void OnSetModified();

  DECLARE_MESSAGE_MAP()
};

#endif