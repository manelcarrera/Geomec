#include "stdafx.h"

#include "HexaModel.h"
#include "HexaModel_Delegate.h"

CHexaModel_Delegate::CHexaModel_Delegate(CHexaModel *aHexaModel)
    : C3DModel_Delegate(aHexaModel), m_HexaModel(aHexaModel) {}
