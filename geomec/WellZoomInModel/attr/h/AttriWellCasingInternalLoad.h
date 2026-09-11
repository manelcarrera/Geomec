#ifndef _ATTRIWELLCASINGINTERNALLOAD_H_
#define _ATTRIWELLCASINGINTERNALLOAD_H_


///// define 'BEGIN_MESSAGE_MAP' macro for a template with 2 parameters

#define BEGIN_TEMPLATE_2_MESSAGE_MAP(theClass, type_name1, type_name2, baseClass)			\
  PTM_WARNING_DISABLE														\
  template < typename type_name1, typename type_name2 >											\
  const AFX_MSGMAP* theClass< type_name1, type_name2 >::GetMessageMap() const			\
    { return GetThisMessageMap(); }										\
  template < typename type_name1, typename type_name2 >											\
  const AFX_MSGMAP* PASCAL theClass< type_name1, type_name2 >::GetThisMessageMap()		\
  {																		\
    typedef theClass< type_name1, type_name2 > ThisClass;							\
    typedef baseClass TheBaseClass;										\
    static const AFX_MSGMAP_ENTRY _messageEntries[] =					\
    {

/////


#include "AttributeTempl.h"
#include "unitnode.h"
#include "GlobalMessage.h"


template <class LOAD, class GRADIENT_QTY>
class CAttriWellCasingInternalLoadDlg : public CAttributesTemplate<LOAD>
{
public:
  CAttriWellCasingInternalLoadDlg(LOAD& source, CWnd* pParent = 0);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnRadio();

  DECLARE_MESSAGE_MAP()

private:
  double ControlValue(CDataExchange* pDX, const CString& strValue, unsigned int idc) const;
  void EnableControls();
};

template <class LOAD, class GRADIENT_QTY>
CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::CAttriWellCasingInternalLoadDlg(LOAD& source, CWnd* pParent)
: CAttributesTemplate<LOAD>(IDD_ATTRI_WELLCASINGINTERNALLOAD, source, pParent)
{
}

template <class LOAD, class GRADIENT_QTY>
BOOL CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::OnInitDialog()
{
  CAttributesTemplate<LOAD>::OnInitDialog();

  SetWindowText((Copy().LoadName() + " Attributes").toStdString().c_str());

  GetDlgItem(IDC_ST_TIMESTEP_NAME)->SetWindowText(Copy().Stage().Name().toStdString().c_str());

  CString str;
  GetDlgItem(IDC_RD_PRE_DEFINED)->GetWindowText(str);
  CString strLoadName = Copy().LoadName().toStdString().c_str();
  strLoadName.MakeLower();
  str.Replace("**VALUE**", strLoadName);
  GetDlgItem(IDC_RD_PRE_DEFINED)->SetWindowText(str);

  GetDlgItem(IDC_LBL_CONSTANT_PRESSURE)->GetWindowText(str);
  str.Replace("**VALUE**", Copy().LoadName().toStdString().c_str());
  GetDlgItem(IDC_LBL_CONSTANT_PRESSURE)->SetWindowText(str);

  EnableControls();

  return TRUE;
}

template <class LOAD, class GRADIENT_QTY>
void CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<LOAD>::DoDataExchange(pDX);

  int iDistributed;
  int iGradient;

  CString strReferenceValue;
  CString strReferenceDepth;
  CString strGradient;

  CDoubleQuantity::UNIT unit = UnitNode().Unit();
  CLengthQuantity qnLength;
  GRADIENT_QTY qnGradient;

  CString strValueUnit;
  CString strDepthUnit;
  CString strGradientUnit;

  if(!pDX->m_bSaveAndValidate)
  {
  iDistributed = (Copy().DistributedOnly() ? 0 : 1);
  if(Copy().DistributedSize() == 0)
      iDistributed = -1;

  iGradient = (Copy().LoadingMode() == LOAD::LM_REPEATER ? 0 : 1);
  if(iDistributed == 0)
      iGradient = -1;

  strValueUnit = Copy().Gradient().UnitName(unit).toStdString().c_str();
  strDepthUnit = qnLength.UnitName(unit).c_str();
  strGradientUnit = qnGradient.UnitName(unit).c_str();

  strReferenceValue.Format("%g", Copy().Gradient().ReferenceValue() * Copy().Gradient().UnitFactor(unit));
  strReferenceDepth.Format("%g", qnLength.Convert(Copy().Gradient().ReferenceDepth(), unit, CDoubleQuantity::SI_UNIT));
  strGradient.Format("%g", qnGradient.Convert(Copy().Gradient().Gradient(), unit, CDoubleQuantity::SI_UNIT));

  if(iDistributed == 0 || iGradient == 0)
  {
      // disabled
      strReferenceValue = "";
      strReferenceDepth = "";
      strGradient = "";
  }
  }

  DDX_Radio(pDX, IDC_RD_EXTRA_POLATE, iDistributed);
  DDX_Radio(pDX, IDC_RD_UNDEFINED, iGradient);
  DDX_Text(pDX, IDC_ED_CONSTANT_PRESSURE, strReferenceValue);
  DDX_Text(pDX, IDC_PRESSURE_UNIT_1, strValueUnit);
  DDX_Text(pDX, IDC_ED_CONSTANT_DEPTH, strReferenceDepth);
  DDX_Text(pDX, IDC_ST_DEPTH_UNIT_1, strDepthUnit);
  DDX_Text(pDX, IDC_ED_CONSTANT_GRADIENT, strGradient);
  DDX_Text(pDX, IDC_ST_GRADIENT_UNIT_1, strGradientUnit);

  if(pDX->m_bSaveAndValidate)
  {
  if((Copy().DistributedSize() == 0 || !Copy().DistributedOnly()) && Copy().LoadingMode() == LOAD::LM_GRADIENT)
  {
      Copy().Gradient().ReferenceValue(ControlValue(pDX, strReferenceValue, IDC_ED_CONSTANT_PRESSURE) / Copy().Gradient().UnitFactor(unit));
      Copy().Gradient().ReferenceDepth(qnLength.Convert(ControlValue(pDX, strReferenceDepth, IDC_ED_CONSTANT_DEPTH), CDoubleQuantity::SI_UNIT, unit));
      Copy().Gradient().Gradient(qnGradient.Convert(ControlValue(pDX, strGradient, IDC_ED_CONSTANT_GRADIENT), CDoubleQuantity::SI_UNIT, unit));
  }

  if(iDistributed >= 0)
      Copy().DistributedOnly(iDistributed == 0);

  if(iGradient >= 0)
      Copy().LoadingMode(iGradient == 0 ? LOAD::LM_REPEATER : LOAD::LM_GRADIENT);
  }
}

BEGIN_TEMPLATE_2_MESSAGE_MAP(CAttriWellCasingInternalLoadDlg, LOAD, GRADIENT_QTY, CDialog)
  ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnRadio)
  ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnRadio)
  ON_BN_CLICKED(IDC_RD_UNDEFINED, OnRadio)
  ON_BN_CLICKED(IDC_RB_CONSTANT, OnRadio)
END_MESSAGE_MAP()

template <class LOAD, class GRADIENT_QTY>
double CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::ControlValue(CDataExchange* pDX, const CString& strValue, unsigned int idc) const
{
  QString qsValue = strValue;
  bool ok;
  double d = qsValue.toDouble(&ok);
  if(!ok)
  {
  _m()->msg("Please enter a valid value");
  pDX->PrepareEditCtrl(idc);
  pDX->Fail();
  }

  return d;
}

template <class LOAD, class GRADIENT_QTY>
void CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::EnableControls()
{
  bool bInitial = (Copy().Stage().Initial() != FALSE);
  bool bExtrapolate = Copy().DistributedOnly();
  bool bHasDistributed = (Copy().DistributedSize() != 0);
  bool bGradient = (Copy().LoadingMode() == LOAD::LM_GRADIENT);

  GetDlgItem(IDC_RD_EXTRA_POLATE)->EnableWindow(bHasDistributed);
  GetDlgItem(IDC_RD_PRE_DEFINED)->EnableWindow(bHasDistributed);

  GetDlgItem(IDC_RD_UNDEFINED)->EnableWindow(!bInitial && (!bExtrapolate || !bHasDistributed));
  GetDlgItem(IDC_RB_CONSTANT)->EnableWindow(!bExtrapolate || !bHasDistributed);

  GetDlgItem(IDC_LBL_CONSTANT_PRESSURE)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_ED_CONSTANT_PRESSURE)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_PRESSURE_UNIT_1)->EnableWindow(!bExtrapolate && bGradient);

  GetDlgItem(IDC_LBL_CONSTANT_DEPTH)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_ED_CONSTANT_DEPTH)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_ST_DEPTH_UNIT_1)->EnableWindow(!bExtrapolate && bGradient);

  GetDlgItem(IDC_LBL_CONSTANT_GRADIENT)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_ED_CONSTANT_GRADIENT)->EnableWindow(!bExtrapolate && bGradient);
  GetDlgItem(IDC_ST_GRADIENT_UNIT_1)->EnableWindow(!bExtrapolate && bGradient);
}

template <class LOAD, class GRADIENT_QTY>
void CAttriWellCasingInternalLoadDlg<LOAD, GRADIENT_QTY>::OnRadio()
{
  UpdateData(TRUE);
  EnableControls();
  UpdateData(FALSE);
}

#endif // _ATTRIWELLCASINGINTERNALLOAD_H_
