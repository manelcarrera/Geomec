#ifndef _ATTRIFORMATIONLOADDLG_H_
#define _ATTRIFORMATIONLOADDLG_H_

#include "AttributesTemplate.h"
#include "BranchState.h"
#include "unitnode.h"

// template dialog class for formation temperature and pressure attributes

template <class LOAD_TYPE>
class CAttriFormationLoadDlg : public CAttributesTemplate<LOAD_TYPE>
{
public:
  CAttriFormationLoadDlg(LOAD_TYPE& load, unsigned int uDialogTemplate, CWnd* pParent = 0);

protected:
  virtual void UpdateControls();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  BOOL OnInitDialog();
  virtual CString UndefinedLabel() const = 0;
  virtual CString previousUndefinedLabel() const;

protected:
  afx_msg void OnConstant();
  afx_msg void OnUndefined();
  afx_msg void OnPreDefined();
  afx_msg void OnExtraPolate();
};

template <class LOAD_TYPE>
CAttriFormationLoadDlg<LOAD_TYPE>::CAttriFormationLoadDlg(LOAD_TYPE& load, unsigned int uDialogTemplate, CWnd* pParent /*=NULL*/)
  : CAttributesTemplate<LOAD_TYPE>(uDialogTemplate, load, pParent)
{
}


//##ModelId=3BC55D650153
template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<LOAD_TYPE>::DoDataExchange(pDX);

  CString strFormation, strDepletionStage, strGradientUnit, strDepthUnit, strValueUnit;
  int nType, nDistributed;
  double dConstantValue = 0;
  double dConstantDepth = 0;
  double dGradient = 0;

  assert(Copy().ModeSize() == 1);
  assert(Copy().ComponentSize() == 1);

  if(!pDX->m_bSaveAndValidate)
  {
  strValueUnit = Copy().Component().UnitName(UnitNode().Unit()).toLocal8Bit().data();

    strFormation = Copy().Formation()->Name().toStdString().c_str();
    strDepletionStage = Copy().DepletionStage().Name().toStdString().c_str();

    // Do Previous button ...
  if (GetDlgItem(IDC_RD_UNDEFINED))
  {
      if(Copy().IsFirstValidDepletionStage())
        GetDlgItem(IDC_RD_UNDEFINED)->SetWindowText(UndefinedLabel());
      else
        GetDlgItem(IDC_RD_UNDEFINED)->SetWindowText(previousUndefinedLabel());
  }

    if(Copy().DistributedSize() == 0)
      nDistributed = -1;
    else
      nDistributed = Copy().DistributedOnly() ? 0 : 1;

    strDepthUnit = CString(Copy().Constant().ReferenceDepth().UnitName(UnitNode().Unit()).c_str());
    strGradientUnit = CString(Copy().Constant().Gradient().UnitName(UnitNode().Unit()).c_str());

    switch(Copy().Type())
    {
    case LOAD_TYPE::REPEAT_MODE:
      nType = 0;
      break;
    case LOAD_TYPE::CONSTANT_MODE:
      dConstantValue = Copy().Constant().ReferenceValue().Value(UnitNode().Unit());	// Value
      dConstantDepth = Copy().Constant().ReferenceDepth().Value(UnitNode().Unit());			// Depth
      dGradient = Copy().Constant().Gradient().Value(UnitNode().Unit());						// Gradient
      nType = 1;
      break;
    }
  }

  DDX_Text(pDX, IDC_ST_FORMATION_NAME, strFormation);
  DDX_Text(pDX, IDC_ST_TIMESTEP_NAME, strDepletionStage);

  DDX_Radio(pDX, IDC_RD_EXTRA_POLATE, nDistributed);

  if (GetDlgItem(IDC_RD_UNDEFINED))
  {
    if(nDistributed == 0 && !pDX->m_bSaveAndValidate)
    {
      int iDummy = -1;
      DDX_Radio(pDX, IDC_RD_UNDEFINED, iDummy);
    }
    else
    {
      DDX_Radio(pDX, IDC_RD_UNDEFINED, nType);
    }
  }

  if(Copy().Type() == LOAD_TYPE::CONSTANT_MODE && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0))
  {
  if (GetDlgItem(IDC_ED_CONSTANT_PRESSURE))
      DDX_Text(pDX, IDC_ED_CONSTANT_PRESSURE, dConstantValue);
  if(GetDlgItem(IDC_ED_CONSTANT_DEPTH))
      DDX_Text(pDX, IDC_ED_CONSTANT_DEPTH, dConstantDepth);
  if(GetDlgItem(IDC_ED_CONSTANT_GRADIENT))
      DDX_Text(pDX, IDC_ED_CONSTANT_GRADIENT, dGradient);

    DDV_MinMaxDouble(pDX, 
             dConstantValue, 
             Copy().Constant().ReferenceValue().MinValue( UnitNode().Unit()),
             Copy().Constant().ReferenceValue().MaxValue( UnitNode().Unit()));
    DDV_MinMaxDouble(pDX, 
             dConstantDepth, 
             Copy().Constant().ReferenceDepth().MinValue( UnitNode().Unit()),
             Copy().Constant().ReferenceDepth().MaxValue( UnitNode().Unit())); 
    DDV_MinMaxDouble(pDX, 
             dGradient, 
             Copy().Constant().Gradient().MinValue( UnitNode().Unit()),
             Copy().Constant().Gradient().MaxValue( UnitNode().Unit())); 
  }
  else if(!pDX->m_bSaveAndValidate)
  {
    // empty edit boxes
    CString strEmpty;
  if(GetDlgItem(IDC_ED_CONSTANT_PRESSURE))
  		DDX_Text(pDX, IDC_ED_CONSTANT_PRESSURE, strEmpty);
  if(GetDlgItem(IDC_ED_CONSTANT_DEPTH))
      DDX_Text(pDX, IDC_ED_CONSTANT_DEPTH, strEmpty);
  if(GetDlgItem(IDC_ED_CONSTANT_GRADIENT))
      DDX_Text(pDX, IDC_ED_CONSTANT_GRADIENT, strEmpty);
  }

  if(GetDlgItem(IDC_ST_GRADIENT_UNIT_1))
    DDX_Text(pDX, IDC_ST_GRADIENT_UNIT_1, strGradientUnit);
  if(GetDlgItem(IDC_PRESSURE_UNIT_1))
    DDX_Text(pDX, IDC_PRESSURE_UNIT_1, strValueUnit);
  if(GetDlgItem(IDC_ST_DEPTH_UNIT_1))
    DDX_Text(pDX, IDC_ST_DEPTH_UNIT_1, strDepthUnit);

  if(pDX->m_bSaveAndValidate)
  {
    if((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) && Copy().Type() == LOAD_TYPE::CONSTANT_MODE)
      Copy().Constant().Set(dConstantValue, dConstantDepth, dGradient, UnitNode().Unit());

    if(Copy().DistributedSize() > 0)
      Copy().DistributedOnly(nDistributed == 0);
  }
}

template <class LOAD_TYPE>
BOOL CAttriFormationLoadDlg<LOAD_TYPE>::OnInitDialog()
{
  CAttributesTemplate<LOAD_TYPE>::OnInitDialog();

  UpdateControls();

  return FALSE;
}

template <class LOAD_TYPE>
CString CAttriFormationLoadDlg<LOAD_TYPE>::previousUndefinedLabel() const
{
  return _T("Same as previous depletion stage");
}

template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::OnPreDefined() 
{
  UpdateData(TRUE);
  Copy().DistributedOnly(false);
  UpdateControls();
}

template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::OnExtraPolate() 
{
  UpdateData(TRUE);
  Copy().DistributedOnly(true);
  UpdateControls();	
}

template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::OnUndefined() 
{
  UpdateData(TRUE);
  Copy().Type(LOAD_TYPE::REPEAT_MODE);
  UpdateControls();
}

template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::OnConstant() 
{
  UpdateData(TRUE);
  Copy().Type(LOAD_TYPE::CONSTANT_MODE);
  UpdateControls();
}

template <class LOAD_TYPE>
void CAttriFormationLoadDlg<LOAD_TYPE>::UpdateControls()
{
  const CBranchState& branchstate = (static_cast<const CModelBase&>(Copy().Model())).BranchState();
  bool bDisableAll = (branchstate.IsBranch() && branchstate.IsFixedStage(Copy().DepletionStage()));

  GetDlgItem(IDC_RD_PRE_DEFINED)->EnableWindow(!bDisableAll && Copy().DistributedSize() > 0);
  GetDlgItem(IDC_RD_EXTRA_POLATE)->EnableWindow(!bDisableAll && Copy().DistributedSize() > 0);

  if (GetDlgItem(IDC_RD_UNDEFINED))
    GetDlgItem(IDC_RD_UNDEFINED)->EnableWindow(!bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  if (GetDlgItem(IDC_RB_CONSTANT))
    GetDlgItem(IDC_RB_CONSTANT)->EnableWindow(!bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  if (GetDlgItem(IDC_ED_CONSTANT_PRESSURE))
  {
    GetDlgItem(IDC_ED_CONSTANT_PRESSURE)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
    GetDlgItem(IDC_LBL_CONSTANT_PRESSURE)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  if(GetDlgItem(IDC_PRESSURE_UNIT_1))
    GetDlgItem(IDC_PRESSURE_UNIT_1)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  }

  if(GetDlgItem(IDC_ED_CONSTANT_DEPTH))
  {
    GetDlgItem(IDC_ED_CONSTANT_DEPTH)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
    GetDlgItem(IDC_LBL_CONSTANT_DEPTH)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
    GetDlgItem(IDC_ST_DEPTH_UNIT_1)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  }

  if(GetDlgItem(IDC_ED_CONSTANT_GRADIENT))
  {
    GetDlgItem(IDC_ED_CONSTANT_GRADIENT)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
    GetDlgItem(IDC_LBL_CONSTANT_GRADIENT)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
    GetDlgItem(IDC_ST_GRADIENT_UNIT_1)->EnableWindow((Copy().Type() == LOAD_TYPE::CONSTANT_MODE) && !bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  }

  UpdateData(FALSE);
}

#endif // _ATTRIFORMATIONLOADDLG_H_
