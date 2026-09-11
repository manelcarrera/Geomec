#ifndef _OPTIMIZATION_SETTINGS_H_
#define _OPTIMIZATION_SETTINGS_H_

#include "afxwin.h"

class CTetraMesh;
class CHexaMesh;

class COptimizationSettingsDlg : public CDialog
{
public:
  COptimizationSettingsDlg(CWnd* pParent = NULL);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  virtual BOOL OnInitDialog();
  virtual void OnOK();

private:
  CTetraMesh *m_pTetraMesh;
  CHexaMesh  *m_pHexaMesh;

  int m_nVisualMapping;
  int m_nDianaMapping;
  int m_nTetraMesher;
  int m_nHexaMesher;
  int m_nDSFStressSmoothening;
  int m_nDSFInterSurfGeneration;

  BOOL m_dsa;

public:
  BOOL dsa(){ return m_dsa; }
};


#endif