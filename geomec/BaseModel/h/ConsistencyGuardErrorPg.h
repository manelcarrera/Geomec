#ifndef _CONSISTENCY_GUARD_ERROR_PG_H_
#define _CONSISTENCY_GUARD_ERROR_PG_H_


#include "afxwin.h"
#include "afxdlgs.h"

class CModelBase;
#include "ConsistencyGuard.h"


class CConsistencyGuardErrorPg : public CPropertyPage
{
  CModelBase *m_pModel;

public:
  CConsistencyGuardErrorPg(CModelBase *pModel);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

  void OnClearErrors();

  DECLARE_MESSAGE_MAP()
};





#endif