#include "ExportSelection.h"

#include "FormationPlane.h"
#include "HorizonBase.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "WellCasingRTCI.h"

bool CSlipHorizonSelectionFilter::operator()(const CHorizonBase &hor) const { return hor.Slip(); }

bool CHorizonSelectionFilter::operator()(const CHorizonBase &hor) const { return !hor.Slip(); }

bool CElementSetSelectionFilter::operator()(const IElementSet &elset) const {
  const CModelBase &model = static_cast<const CModelBase &>(elset.Model());
  return &elset.ElementSet() != &model.Mesh().Mesh();
}
