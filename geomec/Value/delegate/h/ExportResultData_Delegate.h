#ifndef _ExportResultData_Delegate_h_
#define _ExportResultData_Delegate_h_

class CExportResultData;

#include "ColorNode_Delegate.h"

class CExportResultData_Delegate : public CColorNode_Delegate {
public:
  CExportResultData_Delegate(CExportResultData *exportResultData);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  CExportResultData_Delegate(const CExportResultData_Delegate &rhs);
  CExportResultData_Delegate &operator=(const CExportResultData_Delegate &rhs);

  CExportResultData *m_exportResultData;

  REGISTER_DELEGATE(CExportResultData, CExportResultData_Delegate);
};

#endif // _ExportResultData_Delegate_h_
