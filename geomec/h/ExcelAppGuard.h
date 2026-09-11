#ifndef _ExcelAppGuard_h_
#define _ExcelAppGuard_h_

#include "excel8.h"

class CExcelAppGuard
{
public:
  class CNoExcelException
  {
  public:
    CNoExcelException() {}
  };

  CExcelAppGuard();
  ~CExcelAppGuard();

  _Application& excelApp();

private:
  _Application m_ExcelApp;
};

#endif  // _ExcelAppGuard_h_
