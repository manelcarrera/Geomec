// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IRegistryXDllCtrl wrapper class

//##ModelId=3C2043FA0271
class IRegistryXDllCtrl : public COleDispatchDriver
{
public:
	//##ModelId=3C2043FA0291
	IRegistryXDllCtrl() {}		// Calls COleDispatchDriver default constructor
	//##ModelId=3C2043FA0292
	IRegistryXDllCtrl(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	//##ModelId=3C2043FA0294
	IRegistryXDllCtrl(const IRegistryXDllCtrl& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	//##ModelId=3C2043FA0296
	CString GetUserKey();
	//##ModelId=3C2043FA0297
	void SetUserKey(LPCTSTR);
	//##ModelId=3C2043FA02A0
	CString GetSystemKey();
	//##ModelId=3C2043FA02A1
	void SetSystemKey(LPCTSTR);
	//##ModelId=3C2043FA02A3
	BOOL GetReportErrors();
	//##ModelId=3C2043FA02A4
	void SetReportErrors(BOOL);

// Operations
public:
	//##ModelId=3C2043FA02B1
	BOOL WriteUserString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value);
	//##ModelId=3C2043FA02B5
	BOOL WriteUserLong(LPCTSTR Section, LPCTSTR Entry, long Value);
	//##ModelId=3C2043FA02C3
	BOOL WriteSystemString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value);
	//##ModelId=3C2043FA02C7
	BOOL WriteSystemLong(LPCTSTR Section, LPCTSTR Entry, long Value);
	//##ModelId=3C2043FA02D0
	CString GetSystemString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default);
	//##ModelId=3C2043FA02D4
	long GetSystemLong(LPCTSTR Section, LPCTSTR Entry, long Default);
	//##ModelId=3C2043FA02D8
	CString GetUserString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default);
	//##ModelId=3C2043FA02E2
	long GetUserLong(LPCTSTR Section, LPCTSTR Entry, long Default);
};
