#ifndef _DCBOUNDACASE_H_
#define _DCBOUNDACASE_H_

#include "IDCCase.h"

namespace dia {

class CLoadManager;
class ILoad;

class CBoundaCase : public ICase
{
	friend class CLoadManager;

	// only called by (friend) CLoadManager
	CBoundaCase(CLoadManager &manager, int index);

protected:
  virtual std::string FilosDirName() const;
};

}

#endif // _DCBOUNDACASE_H_
