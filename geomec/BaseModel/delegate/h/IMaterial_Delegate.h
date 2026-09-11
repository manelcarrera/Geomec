#ifndef _IMaterial_Delegate_h_
#define _IMaterial_Delegate_h_

class IMaterial;

#include "IMaterialBase_Delegate.h"

class IMaterial_Delegate : public IMaterialBase_Delegate
{
public:
  IMaterial_Delegate(IMaterial* material);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanDestroy() const;
  virtual bool Destroy();

private:
  IMaterial_Delegate(const IMaterial_Delegate& rhs);
  IMaterial_Delegate& operator = (const IMaterial_Delegate& rhs);

  IMaterial* m_material;

  REGISTER_DELEGATE(IMaterial, IMaterial_Delegate);
};

#endif  // _IMaterial_Delegate_h_
