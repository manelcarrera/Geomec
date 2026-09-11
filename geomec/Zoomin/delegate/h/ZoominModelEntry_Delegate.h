#ifndef _ZOOMINMODELENTRY_DELEGATE_H_
#define _ZOOMINMODELENTRY_DELEGATE_H_

#include "GraphEntry_Delegate.h"
#include "ZoominModelPlaceHolder.h"

class CZoominModelEntry;

class CZoominModelEntry_Delegate : public CGraphEntryTemp_Delegate<CZoominModelPlaceHolder>
{
public:
  CZoominModelEntry_Delegate(CZoominModelEntry* zoominModelEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CZoominModelEntry_Delegate(const CZoominModelEntry_Delegate&);
  CZoominModelEntry_Delegate& operator=(const CZoominModelEntry_Delegate&);

  void NewZoominModel();
  void Import();

  CZoominModelEntry* m_zoominModelEntry;

  REGISTER_DELEGATE(CZoominModelEntry, CZoominModelEntry_Delegate);
};

#endif // _ZOOMINMODELENTRY_DELEGATE_H_
