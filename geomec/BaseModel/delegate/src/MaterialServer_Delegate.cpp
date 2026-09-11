#include "stdafx.h"

#include "MaterialServer.h"
#include "MaterialServer_Delegate.h"

CMaterialServer_Delegate::CMaterialServer_Delegate(CMaterialServer *materialServer)
    : IRockMaterialServerTempl_Delegate<TMaterialServerParent>(materialServer), m_materialServer(materialServer) {}
