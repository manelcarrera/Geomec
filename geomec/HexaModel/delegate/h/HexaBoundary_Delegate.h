#ifndef _HexaBoundary_Delegate_h_
#define _HexaBoundary_Delegate_h_

class CHexaBoundary;

#include "BoundaryBase_Delegate.h"

class CHexaBoundary_Delegate : public CBoundaryBase_Delegate
{
public:
  CHexaBoundary_Delegate(CHexaBoundary* hexaBoundary);

  virtual bool Attributes();
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  bool CanCreateInterfaces() const;
  void InterfaceAttributes();

  CHexaBoundary_Delegate(const CHexaBoundary_Delegate& rhs);
  CHexaBoundary_Delegate& operator = (const CHexaBoundary_Delegate& rhs);

  CHexaBoundary* m_hexaBoundary;

  REGISTER_DELEGATE(CHexaBoundary, CHexaBoundary_Delegate);
};

#endif  // _HexaBoundary_Delegate_h_
