#ifndef _EclipseModel_Delegate_h_
#define _EclipseModel_Delegate_h_

class CEclipseSubModelEntry;

#include "GraphEntry_Delegate.h"

typedef CGraphEntryTemp_Delegate <COpenGLNode> TSubModelEntry_Delegate;

class CEclipseSubModelEntry_Delegate : public TSubModelEntry_Delegate
{
public:
  CEclipseSubModelEntry_Delegate(CEclipseSubModelEntry* eclipseSubModelEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void EditSubModel();

private:
  CEclipseSubModelEntry_Delegate(const CEclipseSubModelEntry_Delegate& rhs);
  CEclipseSubModelEntry_Delegate& operator = (
    const CEclipseSubModelEntry_Delegate& rhs);

  CEclipseSubModelEntry* m_eclipseSubModelEntry;

  REGISTER_DELEGATE(CEclipseSubModelEntry, CEclipseSubModelEntry_Delegate);
};

#endif  // _EclipseModel_Delegate_h_
