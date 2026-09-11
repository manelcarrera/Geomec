#include "StdAfx.h"
#include "Geomec.h"
#include "AttriValueType.h"

#include "ElementSet.h"
#include "ValueType.h"
#include "resourceIDS.h"
#include "resource.h"

CAttriValueType::CAttriValueType(CValueType& source, CWnd* pParent)
: CAttributesTemplate<CValueType>::CAttributesTemplate(IDD_ATTRI_VALUETYPE, source, pParent)
{
}

BOOL CAttriValueType::OnInitDialog()
{
  CAttributesTemplate<CValueType>::OnInitDialog();

  const IElementSet* pElementSet = dynamic_cast<const IElementSet*>(&Copy().PointSet());
  GetDlgItem(IDC_FRA_MAPPING)->EnableWindow(pElementSet != 0);
  GetDlgItem(IDC_MAP_POINT)->EnableWindow(pElementSet != 0);
  GetDlgItem(IDC_MAP_ELEMENT)->EnableWindow(pElementSet != 0);

  if (!pElementSet)
  {
    CString label;
    switch (Copy().PointSet().Dimension())
    {
    case IPointSet::DIM_1D:
      label = "1";
      break;
    case IPointSet::DIM_2D:
      label = "2";
      break;
    default:
      label = "3";
    }
    label += "D Pointset:";
    GetDlgItem(IDC_ELTSET_LABEL)->SetWindowTextA(label);
  }


  return TRUE;
}

void CAttriValueType::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<CValueType>::DoDataExchange(pDX);

  CString strName, strSetName;
  int iMapType;

  const IElementSet* pElementSet = dynamic_cast<const IElementSet*>(&Copy().PointSet());

  if(!pDX->m_bSaveAndValidate)
  {
    strName = Copy().Name().toStdString().c_str();

    if(pElementSet)
      iMapType = (Copy().MapType() == CValueType::MT_POINT ? 0 : 1);
    else
      iMapType = -1;
  }

  strSetName = Copy().PointSet().Name().toStdString().c_str();

  DDX_Text(pDX, IDC_EC_NAME, strName);
  DDX_Text(pDX, IDC_ELTSET, strSetName);
  DDX_Radio(pDX, IDC_MAP_POINT, iMapType);

  if(pDX->m_bSaveAndValidate)
  {
    Copy().Name((LPCSTR) strName);
    if(pElementSet)
    {
      if(iMapType == 0)
        Copy().MapType(CValueType::MT_POINT);
      else if(Copy().MapType() == CValueType::MT_POINT)
        Copy().MapType(CValueType::MT_NONE);
    }
  }
}

void CAttriValueType::OnOK()
{
  if(Copy().Name().isEmpty())
  {
    AfxMessageBox(IDS_NAME_FIELD_EMPTY);
    return;
  }

  CAttributesTemplate<CValueType>::OnOK();
}
