#ifndef _WELLCASINGSTEELOBSERVER_H_
#define _WELLCASINGSTEELOBSERVER_H_

class CValueType_Delegate;

#include "IMaterialServerTempl.h"
#include "MaterialServerParent.h"
#include "WellCasingSteelMaterial.h"
#include "OpenGLNodeObserver.h"

class CWellCasingModel;

class CWellCasingMaterialServer;
typedef CMaterialServerParent<COpenGLNode, CWellCasingMaterialServer> TWellCasingMaterialServerParent;

#include "WellCasingSteel_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "MaterialServerObserver_Delegate.h"

typedef CNodeObserver_Delegate<CValueType, CValueType_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TValueTypeObserver;
typedef CMaterialServerObserver_Delegate<CWellCasingMaterialServer, CWellCasingMaterialServer_Delegate, CValueType, TValueTypeObserver, FALSE, FIXED_ITEM> TWellCasingMaterialServerObs;
typedef COpenGLNodeObserver_Delegate<CWellCasingSteel, CWellCasingSteel_Delegate, CWellCasingMaterialServer, TWellCasingMaterialServerObs, FALSE, FIXED_ITEM> TWellCasingSteelObserver;

#endif // _WELLCASINGSTEELOBSERVER_H_
