#ifndef _WellCasingCementInterface_Delegate_h_
#define _WellCasingCementInterface_Delegate_h_

class CWellCasingCementInterfaceMaterialServer;
class CInterfaceMaterial;
class CWellCasingCementInterface;

#include "MaterialServerParent_Delegate.h"
#include "IMaterialServerTempl_Delegate.h"
#include "OpenGLNode_Delegate.h"
#include "InterfaceMaterial_Delegate.h"

// TODO
// Why can I not use CMaterialServerParent_Delegate?
// Is it necessary?

typedef CMaterialServerParent <COpenGLNode,
  CWellCasingCementInterfaceMaterialServer>
    TWellCasingCementInterfaceMaterialServerParent;

class CWellCasingCementInterfaceMaterialServer_Delegate :
  public IMaterialServerTempl_Delegate <CInterfaceMaterial,
    CInterfaceMaterial_Delegate,
    TWellCasingCementInterfaceMaterialServerParent>
{
public:
  CWellCasingCementInterfaceMaterialServer_Delegate(
    CWellCasingCementInterfaceMaterialServer*
      wellCasingCementInterfaceMaterialServer);

private:
  CWellCasingCementInterfaceMaterialServer_Delegate(
    const CWellCasingCementInterfaceMaterialServer_Delegate& rhs);
  CWellCasingCementInterfaceMaterialServer_Delegate& operator = (
    const CWellCasingCementInterfaceMaterialServer_Delegate& rhs);

  CWellCasingCementInterfaceMaterialServer*
    m_wellCasingCementInterfaceMaterialServer;

  REGISTER_DELEGATE(CWellCasingCementInterfaceMaterialServer,
    CWellCasingCementInterfaceMaterialServer_Delegate);
};

typedef CMaterialServerParent_Delegate <COpenGLNode_Delegate, COpenGLNode,
  CWellCasingCementInterfaceMaterialServer>
    TWellCasingCementInterfaceMaterialServerParent_Delegate;

class CWellCasingCementInterface_Delegate :
  public TWellCasingCementInterfaceMaterialServerParent_Delegate
{
public:
  CWellCasingCementInterface_Delegate(
    CWellCasingCementInterface* wellCasingCementInterface);

  virtual bool Attributes();

private:
  CWellCasingCementInterface_Delegate(
    const CWellCasingCementInterface_Delegate& rhs);
  CWellCasingCementInterface_Delegate& operator = (
    const CWellCasingCementInterface_Delegate& rhs);

  CWellCasingCementInterface* m_wellCasingCementInterface;

  REGISTER_DELEGATE(CWellCasingCementInterface,
    CWellCasingCementInterface_Delegate);
};

#endif  // _WellCasingCementInterface_Delegate_h_
