// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// DsbTS wrapper class

class DsbTS : public COleDispatchDriver {
public:
  DsbTS() {} // Calls COleDispatchDriver default constructor
  DsbTS(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
  DsbTS(const DsbTS &dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
public:
  // Operations
public:
  void OpenTemplate(LPCTSTR TemplateFileName);
  void WriteLine(long linenr, LPDISPATCH *dsb);
  void SaveWorkBook(LPCTSTR filename);
  void ShowExcel(LPCTSTR filename);
  void CloseTemplate();
};
