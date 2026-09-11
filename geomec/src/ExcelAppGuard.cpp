#include "stdafx.h"

#include "ExcelAppGuard.h"

CExcelAppGuard::CExcelAppGuard()
: m_ExcelApp()
{
  if (!m_ExcelApp.CreateDispatch("Excel.Application"))
  {
    throw CNoExcelException();
  }

  assert(m_ExcelApp.m_lpDispatch);
}

CExcelAppGuard::~CExcelAppGuard()
{
  assert(m_ExcelApp.m_lpDispatch);
	Workbooks wbs;
	wbs.AttachDispatch(excelApp().GetWorkbooks());

  for(long l = 1; l <= wbs.GetCount(); ++l)
  {
    _Workbook wb;
    VARIANT var;
    var.vt = VT_I4;
    var.lVal = l;
    wb.AttachDispatch(wbs.GetItem(var));
    var.vt = VT_BOOL;
    var.boolVal = VARIANT_FALSE;
    VARIANT varEmpty;
    varEmpty.vt = VT_EMPTY;
    wb.Close(var, varEmpty, varEmpty);
  }
  m_ExcelApp.Quit();
}

_Application& CExcelAppGuard::excelApp()
{
  assert(m_ExcelApp.m_lpDispatch);

  return m_ExcelApp;
}
