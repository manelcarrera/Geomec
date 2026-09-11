#ifndef _ATTRIGLOBALFORMATIONLOADDLG_H_
#define _ATTRIGLOBALFORMATIONLOADDLG_H_

#include "AttributeTempl.h"

template <class LOAD_TYPE>
class CAttriGlobalFormationLoadDlg : public CAttributesTemplate<LOAD_TYPE>
{
public:
  CAttriGlobalFormationLoadDlg(unsigned int uDialogTemplate, LOAD_TYPE& load, CWnd* pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  BOOL OnInitDialog();

  afx_msg void OnPreDefined();
  afx_msg void OnExtraPolate();

private:
  void EnableControls();
};

template <class LOAD_TYPE>
CAttriGlobalFormationLoadDlg<LOAD_TYPE>::CAttriGlobalFormationLoadDlg(unsigned int uDialogTemplate, LOAD_TYPE& load, CWnd* pParent)
: CAttributesTemplate<LOAD_TYPE>(uDialogTemplate, load, pParent)
{
}

template <class LOAD_TYPE>
void CAttriGlobalFormationLoadDlg<LOAD_TYPE>::DoDataExchange(CDataExchange* pDX)
{
  CString strLoadUnit, strGradientUnit, strDepthUnit;
  double dConstantValue = 0;
  double dConstantDepth = 0;
  double dGradient = 0;
  IValueDomainScalar::TMinMax min_max(0,0);
  int nDistributed;

  assert(Copy().ModeSize() == 1);
  assert(Copy().ComponentSize() == 1);

  CAttributesTemplate<LOAD_TYPE>::DoDataExchange(pDX);

  if(!pDX->m_bSaveAndValidate)
  {
    strLoadUnit = Copy().Component().UnitName(UnitNode().Unit()).toLocal8Bit().data();

    strDepthUnit = CString(Copy().Constant().ReferenceDepth().UnitName(UnitNode().Unit()).c_str());
    strGradientUnit = CString(Copy().Constant().Gradient().UnitName(UnitNode().Unit()).c_str());
    
    dConstantValue = Copy().Constant().ReferenceValue().Value(UnitNode().Unit());	// Pressure
    dConstantDepth = Copy().Constant().ReferenceDepth().Value(UnitNode().Unit());			// Depth
    dGradient = Copy().Constant().Gradient().Value(UnitNode().Unit());						// Gradient

    if(Copy().DistributedSize() > 0)
      nDistributed = Copy().DistributedOnly() ? 0 : 1;
    else
      nDistributed = -1;

  }

  DDX_Radio(pDX, IDC_RD_EXTRA_POLATE, nDistributed);

  if(!Copy().DistributedOnly() || Copy().DistributedSize() == 0)
  {
    DDX_Text(pDX, IDC_ED_CONSTANT_DEPTH, dConstantDepth);
    DDX_Text(pDX, IDC_ED_CONSTANT_GRADIENT, dGradient);
    DDX_Text(pDX, IDC_ED_CONSTANT_VALUE, dConstantValue);

    DDV_MinMaxDouble(pDX,
             dConstantDepth,
             Copy().Constant().ReferenceDepth().MinValue( UnitNode().Unit()),
             Copy().Constant().ReferenceDepth().MaxValue( UnitNode().Unit()));
    DDV_MinMaxDouble(pDX,
             dGradient,
             Copy().Constant().Gradient().MinValue( UnitNode().Unit()),
             Copy().Constant().Gradient().MaxValue( UnitNode().Unit()));
    DDV_MinMaxDouble(pDX,
             dConstantValue,
             Copy().Constant().ReferenceValue().MinValue( UnitNode().Unit()),
             Copy().Constant().ReferenceValue().MaxValue( UnitNode().Unit()));
  }
  else if(!pDX->m_bSaveAndValidate)
  {
    // empty the boxes
    CString strEmpty;
    DDX_Text(pDX, IDC_ED_CONSTANT_DEPTH, strEmpty);
    DDX_Text(pDX, IDC_ED_CONSTANT_GRADIENT, strEmpty);
    DDX_Text(pDX, IDC_ED_CONSTANT_VALUE, strEmpty);
  }

  if(!pDX->m_bSaveAndValidate)
  {
    DDX_Text(pDX, IDC_ST_GRADIENT_UNIT, strGradientUnit);
    DDX_Text(pDX, IDC_LOAD_UNIT, strLoadUnit);
    DDX_Text(pDX, IDC_ST_DEPTH_UNIT, strDepthUnit);
  }

  if(pDX->m_bSaveAndValidate)
  {
    if(!Copy().DistributedOnly() || Copy().DistributedSize() == 0)
      Copy().Constant().Set(dConstantValue, dConstantDepth, dGradient, UnitNode().Unit());

    if(Copy().DistributedSize() > 0)
      Copy().DistributedOnly(nDistributed == 0);
  }
}

template <class LOAD_TYPE>
BOOL CAttriGlobalFormationLoadDlg<LOAD_TYPE>::OnInitDialog()
{
  CAttributesTemplate<LOAD_TYPE>::OnInitDialog();

  EnableControls();

  return FALSE;
}

template <class LOAD_TYPE>
void CAttriGlobalFormationLoadDlg<LOAD_TYPE>::OnPreDefined() 
{
  UpdateData(TRUE);
  Copy().DistributedOnly(false);
  EnableControls();
}

template <class LOAD_TYPE>
void CAttriGlobalFormationLoadDlg<LOAD_TYPE>::OnExtraPolate() 
{
  UpdateData(TRUE);
  Copy().DistributedOnly(true);
  EnableControls();
}

template <class LOAD_TYPE>
void CAttriGlobalFormationLoadDlg<LOAD_TYPE>::EnableControls()
{
  GetDlgItem(IDC_RD_PRE_DEFINED)->EnableWindow(Copy().DistributedSize() > 0 && !Copy().ReadOnly());
  GetDlgItem(IDC_RD_EXTRA_POLATE)->EnableWindow(Copy().DistributedSize() > 0 && !Copy().ReadOnly());

  GetDlgItem(IDC_LBL_CONSTANT_PRESSURE)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_CONSTANT_DEPTH)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_CONSTANT_GRADIENT)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_ED_CONSTANT_VALUE)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ED_CONSTANT_DEPTH)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ED_CONSTANT_GRADIENT)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_LOAD_UNIT)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ST_DEPTH_UNIT)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ST_GRADIENT_UNIT)->EnableWindow(!Copy().ReadOnly() && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  UpdateData(FALSE);
}

#endif // _ATTRIGLOBALFORMATIONLOADDLG_H_
