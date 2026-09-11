#include "ExportSelection.h"

#include "FormationPlane.h"
#include "WellCasingRTCI.h"
#include "PointSet.h"
#include "HorizonBase.h"
#include "ModelBase.h"
#include "MeshBase.h"

bool CSlipHorizonSelectionFilter::operator()(const CHorizonBase& hor) const
{
  return hor.Slip();
}

bool CHorizonSelectionFilter::operator()(const CHorizonBase& hor) const
{
  return !hor.Slip();
}

bool CElementSetSelectionFilter::operator()(const IElementSet& elset) const
{
  const CModelBase& model = static_cast<const CModelBase&>(elset.Model());
  return &elset.ElementSet() != &model.Mesh().Mesh();
}
