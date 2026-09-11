#include "stdafx.h"

#include "FractureMatrixPressure_Delegate.h"
#include "FractureMatrixPressure.h"
#include "AttriFractureMatrixPressureDlg.h"

CFractureMatrixPressure_Delegate::CFractureMatrixPressure_Delegate(
  CFractureMatrixPressure* fractureMatrixPressure)
: TFractureMatrixPressureBase_Delegate(fractureMatrixPressure)
, m_fractureMatrixPressure(fractureMatrixPressure)
{
}

bool CFractureMatrixPressure_Delegate::Attributes()
{
  m_fractureMatrixPressure->AssertValid();

  CAttriFractureMatrixPressureDlg dlg(*m_fractureMatrixPressure);

  if (dlg.DoModal() == IDOK)
  {
  m_fractureMatrixPressure->AssertValid();

  return true;
  }

  return false;
}
