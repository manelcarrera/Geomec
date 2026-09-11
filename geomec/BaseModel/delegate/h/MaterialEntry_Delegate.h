#ifndef _MaterialEntry_Delegate_h_
#define _MaterialEntry_Delegate_h_

#include "GraphEntry_Delegate.h"
#include "IMaterial.h"
#include "MaterialEntry.h"

class CMaterialEntry_Delegate : public CGraphEntryTemp_Delegate<IMaterial> {
public:
  CMaterialEntry_Delegate(CMaterialEntry *materialEntry);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  void ImportFromMatLib();
  void ExportToMatLib(std::vector<ml::CMaterial *> selection);
  void SelectMatLib();

private:
  CMaterialEntry_Delegate(const CMaterialEntry_Delegate &rhs);
  CMaterialEntry_Delegate &operator=(const CMaterialEntry_Delegate &rhs);

  void CreateMatLib();

  CMaterialEntry *m_materialEntry;

  REGISTER_DELEGATE(CMaterialEntry, CMaterialEntry_Delegate);
};

#endif // _MaterialEntry_Delegate_h_
