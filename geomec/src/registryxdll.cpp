// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "registryxdll.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// IRegistryXDllCtrl properties

// ##ModelId=3C2043FA0296
CString IRegistryXDllCtrl::GetUserKey() {
  CString result;
  GetProperty(0x1, VT_BSTR, (void *)&result);
  return result;
}

// ##ModelId=3C2043FA0297
void IRegistryXDllCtrl::SetUserKey(LPCTSTR propVal) { SetProperty(0x1, VT_BSTR, propVal); }

// ##ModelId=3C2043FA02A0
CString IRegistryXDllCtrl::GetSystemKey() {
  CString result;
  GetProperty(0x2, VT_BSTR, (void *)&result);
  return result;
}

// ##ModelId=3C2043FA02A1
void IRegistryXDllCtrl::SetSystemKey(LPCTSTR propVal) { SetProperty(0x2, VT_BSTR, propVal); }

// ##ModelId=3C2043FA02A3
BOOL IRegistryXDllCtrl::GetReportErrors() {
  BOOL result;
  GetProperty(0x3, VT_BOOL, (void *)&result);
  return result;
}

// ##ModelId=3C2043FA02A4
void IRegistryXDllCtrl::SetReportErrors(BOOL propVal) { SetProperty(0x3, VT_BOOL, propVal); }

/////////////////////////////////////////////////////////////////////////////
// IRegistryXDllCtrl operations

// ##ModelId=3C2043FA02B1
BOOL IRegistryXDllCtrl::WriteUserString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value) {
  BOOL result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR;
  InvokeHelper(0x4, DISPATCH_METHOD, VT_BOOL, (void *)&result, parms, Section, Entry, Value);
  return result;
}

// ##ModelId=3C2043FA02B5
BOOL IRegistryXDllCtrl::WriteUserLong(LPCTSTR Section, LPCTSTR Entry, long Value) {
  BOOL result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
  InvokeHelper(0x5, DISPATCH_METHOD, VT_BOOL, (void *)&result, parms, Section, Entry, Value);
  return result;
}

// ##ModelId=3C2043FA02C3
BOOL IRegistryXDllCtrl::WriteSystemString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value) {
  BOOL result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR;
  InvokeHelper(0x6, DISPATCH_METHOD, VT_BOOL, (void *)&result, parms, Section, Entry, Value);
  return result;
}

// ##ModelId=3C2043FA02C7
BOOL IRegistryXDllCtrl::WriteSystemLong(LPCTSTR Section, LPCTSTR Entry, long Value) {
  BOOL result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
  InvokeHelper(0x7, DISPATCH_METHOD, VT_BOOL, (void *)&result, parms, Section, Entry, Value);
  return result;
}

// ##ModelId=3C2043FA02D0
CString IRegistryXDllCtrl::GetSystemString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default) {
  CString result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR;
  InvokeHelper(0x8, DISPATCH_METHOD, VT_BSTR, (void *)&result, parms, Section, Entry, Default);
  return result;
}

// ##ModelId=3C2043FA02D4
long IRegistryXDllCtrl::GetSystemLong(LPCTSTR Section, LPCTSTR Entry, long Default) {
  long result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
  InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void *)&result, parms, Section, Entry, Default);
  return result;
}

// ##ModelId=3C2043FA02D8
CString IRegistryXDllCtrl::GetUserString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default) {
  CString result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR;
  InvokeHelper(0xa, DISPATCH_METHOD, VT_BSTR, (void *)&result, parms, Section, Entry, Default);
  return result;
}

// ##ModelId=3C2043FA02E2
long IRegistryXDllCtrl::GetUserLong(LPCTSTR Section, LPCTSTR Entry, long Default) {
  long result;
  static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
  InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void *)&result, parms, Section, Entry, Default);
  return result;
}
