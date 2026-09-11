#ifndef _ATTRIWELLZOOMINMESHDLG_H_
#define _ATTRIWELLZOOMINMESHDLG_H_

#include "AttributesTemplate.h"
#include "WellZoomInMesh.h"

class CAttriWellZoomInMeshDlg : public CAttributesTemplate<CWellZoomInMesh>
{
public:
  CAttriWellZoomInMeshDlg(CWellZoomInMesh& src, CWnd* pParent = 0);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnMeshTypeHexa();
  afx_msg void OnMeshTypeTetra();

  DECLARE_MESSAGE_MAP();

private:
  void UpdateControls();
};

#endif // _ATTRIWELLZOOMINMESHDLG_H_
