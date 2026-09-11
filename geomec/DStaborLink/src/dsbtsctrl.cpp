// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "dsbtsctrl.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// DsbTS properties

/////////////////////////////////////////////////////////////////////////////
// DsbTS operations

void DsbTS::OpenTemplate(LPCTSTR TemplateFileName) {
  static BYTE parms[] = VTS_BSTR;
  InvokeHelper(0x60030001, DISPATCH_METHOD, VT_EMPTY, NULL, parms, TemplateFileName);
}

void DsbTS::WriteLine(long linenr, LPDISPATCH *dsb) {
  static BYTE parms[] = VTS_I4 VTS_PDISPATCH;
  InvokeHelper(0x60030002, DISPATCH_METHOD, VT_EMPTY, NULL, parms, linenr, dsb);
}

void DsbTS::SaveWorkBook(LPCTSTR filename) {
  static BYTE parms[] = VTS_BSTR;
  InvokeHelper(0x60030003, DISPATCH_METHOD, VT_EMPTY, NULL, parms, filename);
}

void DsbTS::ShowExcel(LPCTSTR filename) {
  static BYTE parms[] = VTS_BSTR;
  InvokeHelper(0x60030004, DISPATCH_METHOD, VT_EMPTY, NULL, parms, filename);
}

void DsbTS::CloseTemplate() { InvokeHelper(0x60030005, DISPATCH_METHOD, VT_EMPTY, NULL, NULL); }
