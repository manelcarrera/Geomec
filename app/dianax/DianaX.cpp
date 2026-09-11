// DianaX.cpp : Implementation of CDianaXApp and DLL registration.

#include "stdafx.h"
#include "DianaX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


CDianaXApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
    { 0x3a01bc14, 0xba9f, 0x11d2, { 0xb3, 0xb4, 0, 0x10, 0x4b, 0x25, 0x56, 0xd9 } };
const WORD _wVerMajor = 2;
const WORD _wVerMinor = 2;


////////////////////////////////////////////////////////////////////////////
// CDianaXApp::InitInstance - DLL initialization

BOOL CDianaXApp::InitInstance()
{
  BOOL bInit = COleControlModule::InitInstance();

  if (bInit)
  {
    // TODO: Add your own module initialization code here.
  }

  return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CDianaXApp::ExitInstance - DLL termination

int CDianaXApp::ExitInstance()
{
  // TODO: Add your own module termination code here.

  return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
  AFX_MANAGE_STATE(_afxModuleAddrThis);

  if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
    return ResultFromScode(SELFREG_E_TYPELIB);

  if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
    return ResultFromScode(SELFREG_E_CLASS);

  return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
  AFX_MANAGE_STATE(_afxModuleAddrThis);

  if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
    return ResultFromScode(SELFREG_E_TYPELIB);

  if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
    return ResultFromScode(SELFREG_E_CLASS);

  return NOERROR;
}
