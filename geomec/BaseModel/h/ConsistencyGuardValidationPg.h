#ifndef _CONSISTENCY_GUARD_VALIDATION_PG_H_
#define _CONSISTENCY_GUARD_VALIDATION_PG_H_


#include "afxwin.h"
#include "afxdlgs.h"
#include "resource.h"

#include "ValidateModel.h"


class CConsistencyGuardValidationPg : public CPropertyPage
{
  CModelBase *m_pModel;
  CValidateModel m_ValidateModel;

  bool m_bMesh;
  bool m_bReservoir;
  bool m_bDepletion;
  bool m_bMaterial;
  bool m_bValues;
  bool m_bBoundCond;
  bool m_bEmptForm;
  bool m_bEmptFault;

  void DrawCheck(CPaintDC &dc, CStatic &IconCheck, bool bCheck);

public:
  CConsistencyGuardValidationPg(CModelBase *pModel);

  enum { IDD = IDD_MODEL_VALID_DLG };

  CStatic m_EmptyFaultsCheck;
  CStatic	m_EmptyFormationsCheck;
  CStatic	m_BoundConditionsCheck;
  CStatic	m_MaterialCheckIcon;
  CStatic	m_ValuesCheckIcon;
  CStatic	m_DepletionCheckIcon;
  CStatic	m_ResCheckIcon;
  CStatic	m_MeshCheckIcon;


protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnPaint();

  DECLARE_MESSAGE_MAP()
};





#endif