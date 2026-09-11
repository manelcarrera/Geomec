#ifndef _ExportResultDataEntry_Delegate_h_
#define _ExportResultDataEntry_Delegate_h_

class CExportResultData;
class CExportResultDataEntry;

#include "StorageNode_Delegate.h"

class CExportResultDataEntry_Delegate : public CStorageNodeEntry_Delegate<CExportResultData> {
public:
  CExportResultDataEntry_Delegate(CExportResultDataEntry *exportResultDataEntry);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  void createExportMacro();
  void Import();
  void Export();

private:
  CExportResultDataEntry_Delegate(const CExportResultDataEntry_Delegate &rhs);
  CExportResultDataEntry_Delegate &operator=(const CExportResultDataEntry_Delegate &rhs);

  CExportResultDataEntry *m_exportResultDataEntry;

  REGISTER_DELEGATE(CExportResultDataEntry, CExportResultDataEntry_Delegate);
};

#endif // _ExportResultDataEntry_Delegate_h_
