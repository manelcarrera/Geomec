#ifndef _IRockMaterialServerTempl_Delegate_h_
#define _IRockMaterialServerTempl_Delegate_h_

#include "IMaterialRock.h"
#include "IMaterialRock_Delegate.h"
#include "IMaterialServerTempl_Delegate.h"
#include "IRockMaterialServerTempl.h"

template <class PARENT>
class IRockMaterialServerTempl_Delegate
    : public IMaterialServerTempl_Delegate<IMaterialRock, IMaterialRock_Delegate, PARENT> {
public:
  IRockMaterialServerTempl_Delegate(IRockMaterialServerTempl<PARENT> *rockMaterialServerTempl);

private:
  IRockMaterialServerTempl_Delegate(const IRockMaterialServerTempl_Delegate &rhs);
  IRockMaterialServerTempl_Delegate &operator=(const IRockMaterialServerTempl_Delegate &rhs);

  IRockMaterialServerTempl<PARENT> *m_rockMaterialServerTempl;

  REGISTER_DELEGATE(IRockMaterialServerTempl<PARENT>, IRockMaterialServerTempl_Delegate<PARENT>);
};

template <class PARENT>
IRockMaterialServerTempl_Delegate<PARENT>::IRockMaterialServerTempl_Delegate(
    IRockMaterialServerTempl<PARENT> *rockMaterialServerTempl)
    : IMaterialServerTempl_Delegate(rockMaterialServerTempl), m_rockMaterialServerTempl(rockMaterialServerTempl) {
  ACTIVATE_TEMPLATE_DELEGATE(IRockMaterialServerTempl<PARENT>, IRockMaterialServerTempl_Delegate<PARENT>);
}

#endif // _IRockMaterialServerTempl_Delegate_h_
