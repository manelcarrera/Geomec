#ifndef _ATTRIWELLCASINGMESHDLG_H_
#define _ATTRIWELLCASINGMESHDLG_H_

#include "AttributesTemplate.h"
//#include "WellCasingMesh.h"
class CWellCasingMesh;

class CAttriWellCasingMeshDlg : public CAttributesTemplate<CWellCasingMesh>
{
public:
	CAttriWellCasingMeshDlg(CWellCasingMesh& mesh, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttriWellCasingMeshDlg();

protected:
  virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
  CString FormattedDiameter(int nIndex) const;

  size_t m_numberOfSteelElements;
  size_t m_numberOfCementElements;
  CSpinButtonCtrl m_spinNumberOfSteelElements;
  CSpinButtonCtrl m_spinNumberOfCementElements;
  double m_outerDiameterRock;
};

#endif // _ATTRIWELLCASINGMESHDLG_H_
