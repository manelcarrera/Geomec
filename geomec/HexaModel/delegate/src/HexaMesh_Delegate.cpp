#include "stdafx.h"

#include "HexaMesh.h"
#include "HexaMesh_Delegate.h"

CHexaMesh_Delegate::CHexaMesh_Delegate(CHexaMesh *hexaMesh) : CMeshBase_Delegate(hexaMesh), m_hexaMesh(hexaMesh) {}

bool CHexaMesh_Delegate::IsMesh() const { return m_hexaMesh->IsMesh(); }
