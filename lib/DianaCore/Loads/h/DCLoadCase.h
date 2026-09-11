// DCLoadCase.h: interface for the CLoadCase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCLOADCASE_H__6403E735_B8EE_4B4E_B43A_B5D0D93EC528__INCLUDED_)
#define AFX_DCLOADCASE_H__6403E735_B8EE_4B4E_B43A_B5D0D93EC528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCCase.h"

namespace dia {

class CLoadManager;
class ILoad;

class CLoadCase : public ICase {
  friend class CLoadManager;

  // only called by (friend) CLoadManager
  CLoadCase(CLoadManager &manager, int index);

protected:
  virtual std::string FilosDirName() const;
};

} // namespace dia

#endif // !defined(AFX_DCLOADCASE_H__6403E735_B8EE_4B4E_B43A_B5D0D93EC528__INCLUDED_)
