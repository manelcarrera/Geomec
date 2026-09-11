// GlobalPressure.cpp: implementation of the CGlobalPressure class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalPressure.h"
#include "FemAppModel.h"
#include "ModelBase.h"
#include "PressureBase.h"
#include "ValueTypeFactory.h"
#include "resourceIDI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalPressure::CGlobalPressure(CFemAppModel &model, const double &dReferencePressure, const double &dReferenceDepth,
                                 const double &dPressureGradient)
    : CGlobalFormationLoad<TPressure, CComponentConstant<TPressure>>("Global pore pressure", model) {
  new CComponentConstant<TPressure>(*this);
  Constant().Set(dReferencePressure, dReferenceDepth, dPressureGradient);
  reParent(&model);
}

CGlobalPressure::CGlobalPressure(const CGlobalPressure &rhs)
    : CGlobalFormationLoad<TPressure, CComponentConstant<TPressure>>(rhs) {
  new CComponentConstant<TPressure>(rhs.Constant());
}

unsigned int CGlobalPressure::IconId() const { return IDI_COMPONENT_PRESSURE_GLOBAL; }

void CGlobalPressure::OnNewNeighbour(const CGraphNode &node) {
  if (dynamic_cast<const TPressure *>(&node)) {
    CModelBase *pModel = static_cast<CModelBase *>(&Model());
    pModel->ResultRegister().ClearAll();
  }

  CGlobalFormationLoad<TPressure, CComponentConstant<TPressure>>::OnNewNeighbour(node);
}

void CGlobalPressure::OnNeighbourDeleted(const CGraphNode &node) {
  if (dynamic_cast<const TPressure *>(&node)) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
    if (!pModel->getDeleting())
      pModel->ResultRegister().ClearAll();
  }

  CGlobalFormationLoad<TPressure, CComponentConstant<TPressure>>::OnNeighbourDeleted(node);
}
