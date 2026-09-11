#ifndef _MaterialServer_Delegate_h_
#define _MaterialServer_Delegate_h_

class CMaterialServer;

#include "MaterialServerParent.h"
#include "IRockMaterialServerTempl_Delegate.h"

typedef CMaterialServerParent <CColorNode, CMaterialServer>
  TMaterialServerParent;

class CMaterialServer_Delegate :
  public IRockMaterialServerTempl_Delegate <TMaterialServerParent>
{
public:
  CMaterialServer_Delegate(CMaterialServer* materialServer);

private:
  CMaterialServer_Delegate(const CMaterialServer_Delegate& rhs);
  CMaterialServer_Delegate& operator = (const CMaterialServer_Delegate& rhs);

  CMaterialServer* m_materialServer;

  REGISTER_DELEGATE(CMaterialServer, CMaterialServer_Delegate);
};

#endif  // _MaterialServer_Delegate_h_
