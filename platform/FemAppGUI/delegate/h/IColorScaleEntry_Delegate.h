#ifndef _IColorScaleEntry_Delegate_h_
#define _IColorScaleEntry_Delegate_h_

class IColorScaleNode;
class IColorScaleEntry;

#include "GraphEntry_Delegate.h"

class IColorScaleEntry_Delegate : public CGraphEntryTemp_Delegate<IColorScaleNode> {
public:
  IColorScaleEntry_Delegate(IColorScaleEntry *colorScaleEntry);

private:
  IColorScaleEntry_Delegate(const IColorScaleEntry_Delegate &rhs);
  IColorScaleEntry_Delegate &operator=(const IColorScaleEntry_Delegate &rhs);

  IColorScaleEntry *m_colorScaleEntry;

  REGISTER_DELEGATE(IColorScaleEntry, IColorScaleEntry_Delegate);
};

#endif // _IColorScaleEntry_Delegate_h_
