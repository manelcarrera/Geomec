#ifndef _WellCasingSteel_Delegate_h_
#define _WellCasingSteel_Delegate_h_

class CWellCasingSteelMaterial;
class CWellCasingMaterialServer;
class CWellCasingSteel;
class CWellCasingSteelMaterial_Delegate;

#include "IMaterialServerTempl_Delegate.h"
#include "MaterialServerParent_Delegate.h"
#include "OpenGLNode_Delegate.h"
#include "WellCasingSteelMaterial_Delegate.h"

// TODO
// Why can I not use CMaterialServerParent_Delegate?
// Is it necessary?

typedef CMaterialServerParent<COpenGLNode, CWellCasingMaterialServer> TWellCasingMaterialServerParent;

class CWellCasingMaterialServer_Delegate
    : public IMaterialServerTempl_Delegate<CWellCasingSteelMaterial, CWellCasingSteelMaterial_Delegate,
                                           TWellCasingMaterialServerParent> {
public:
  CWellCasingMaterialServer_Delegate(CWellCasingMaterialServer *wellCasingMaterialServer);

private:
  CWellCasingMaterialServer_Delegate(const CWellCasingMaterialServer_Delegate &rhs);
  CWellCasingMaterialServer_Delegate &operator=(const CWellCasingMaterialServer_Delegate &rhs);

  CWellCasingMaterialServer *m_wellCasingMaterialServer;

  REGISTER_DELEGATE(CWellCasingMaterialServer, CWellCasingMaterialServer_Delegate);
};

typedef CMaterialServerParent_Delegate<COpenGLNode_Delegate, COpenGLNode, CWellCasingMaterialServer>
    TWellCasingMaterialServerParent_Delegate;

class CWellCasingSteel_Delegate : public TWellCasingMaterialServerParent_Delegate {
public:
  CWellCasingSteel_Delegate(CWellCasingSteel *wellCasingSteel);

  virtual bool Attributes();

private:
  CWellCasingSteel_Delegate(const CWellCasingSteel_Delegate &rhs);
  CWellCasingSteel_Delegate &operator=(const CWellCasingSteel_Delegate &rhs);

  CWellCasingSteel *m_wellCasingSteel;

  REGISTER_DELEGATE(CWellCasingSteel, CWellCasingSteel_Delegate);
};

#endif // _WellCasingSteel_Delegate_h_
