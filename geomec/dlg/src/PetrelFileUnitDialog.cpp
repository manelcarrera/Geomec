// PetrelFileUnitDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "PetrelFileUnitDialog.h"
#include "afxdialogex.h"

// CPetrelFileUnitDialog dialog

CPetrelFileUnitDialog::CPetrelFileUnitDialog(const CString& petrelFile,
  CQuantity::UNIT lateralUnit, CQuantity::UNIT depthUnit,
  CWnd* pParent /*=NULL*/)
	: CDialog(CPetrelFileUnitDialog::IDD, pParent)
  , m_LateralUnit(lateralUnit == CQuantity::SI_UNIT ? 0 : 1)
  , m_DepthUnit(depthUnit == CQuantity::SI_UNIT ? 0 : 1)
  , m_PetrelFile(petrelFile)
{
}

CPetrelFileUnitDialog::~CPetrelFileUnitDialog()
{
}

CQuantity::UNIT CPetrelFileUnitDialog::getLateralUnit() const
{
  return (m_LateralUnit == 0 ? CQuantity::SI_UNIT : CQuantity::FIELD_UNIT);
}

CQuantity::UNIT CPetrelFileUnitDialog::getDepthUnit() const
{
  return (m_DepthUnit == 0 ? CQuantity::SI_UNIT : CQuantity::FIELD_UNIT);
}

// protected

void CPetrelFileUnitDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Radio(pDX, IDC_RADIO_SI_NE, m_LateralUnit);
  DDX_Radio(pDX, IDC_RADIO_SI_D, m_DepthUnit);
  DDX_Text(pDX, IDC_PETREL_FILE, m_PetrelFile);
}

BEGIN_MESSAGE_MAP(CPetrelFileUnitDialog, CDialog)
END_MESSAGE_MAP()

// CPetrelFileUnitDialog message handlers
