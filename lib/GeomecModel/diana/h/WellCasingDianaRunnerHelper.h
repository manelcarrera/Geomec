#ifndef _WELLCASINGDIANARUNNERHELPER_H_
#define _WELLCASINGDIANARUNNERHELPER_H_

const int LOADS_PROGRESS_SCALE = 20;

class IWellModel;
class CDianaRunController;
class CVectorValueSet;
class CDepletionStage;

namespace dia {
class CLoadCase;
}

namespace geo {
class IElement;
}

class IProgressBase;

#include <map>

#include "Hexahedron.h"
#include "IDCElementProperty.h"
#include "WellCasingModel.h"
#include "ivaluecomponent.h"

class CWellCasingDianaRunnerHelper {
private:
  typedef std::map<const CDepletionStage *, dia::CLoadCase *> TStageMap;
  IProgressBase *m_progress;

public:
  CWellCasingDianaRunnerHelper(const IWellModel &model, const CDianaRunController &controller,
                               bool bParentLinearResults);
  ~CWellCasingDianaRunnerHelper();

  const dia::IMaterial *Material(const geo::IElement &element) const;
  const dia::IMaterial *InterfaceMaterial(const geo::IElement &element) const;

  bool WriteBoundaryLoads(const TStageMap &mpStages);

  const CWellCasingModel &CasingModel() const;
  const CDianaRunController &Controller() const;

  template <class RUNNER>
  void InterfaceElementPressures(RUNNER &runner, const geo::IElement &elm, const CDepletionStage &stage,
                                 IValueDomainScalar::TValueVec &vcValues) const;

private:
  bool WriteBoundaryLoadsFromDistributions(const TStageMap &mpStages);
  bool WriteBoundaryLoadsFromParentModel(const TStageMap &mpStages);
  geo::CVector GetDisplacementVector(const CVectorValueSet &displa, const geo::IElement &elm,
                                     const geo::IPoint &ptSample) const;

private:
  const IWellModel &m_model;
  const CDianaRunController &m_controller;
  bool m_bParentLinearResults;
};

template <class RUNNER>
void CWellCasingDianaRunnerHelper::InterfaceElementPressures(RUNNER &runner, const geo::IElement &elm,
                                                             const CDepletionStage &stage,
                                                             IValueDomainScalar::TValueVec &vcValues) const {
  assert(dynamic_cast<const geo::CInterfaceElement *>(&elm));
  const geo::CInterfaceElement &iface = static_cast<const geo::CInterfaceElement &>(elm);

  assert(iface.FrontFace() != iface.BackFace());

  // try front
  assert(dynamic_cast<const geo::CBodyQuadrilateral *>(iface.FrontFace()));
  const geo::IBody *pBody = (static_cast<const geo::CBodyQuadrilateral &>(*iface.FrontFace())).Parent();
  int iNodeOffset = 0;
  if (!pBody || CasingModel().Mesh().IsSteelElement(*pBody)) {
    // use back
    assert(dynamic_cast<const geo::CBodyQuadrilateral *>(iface.BackFace()));
    pBody = (static_cast<const geo::CBodyQuadrilateral &>(*iface.BackFace())).Parent();
    iNodeOffset = 4;
  }

  assert(!CasingModel().Mesh().IsSteelElement(*pBody));
  assert(dynamic_cast<const geo::CHexahedron *>(pBody));
  IValueDomainScalar::TValueVec vcBodyPressures;
  runner.ElementPressures(*pBody, stage, vcBodyPressures);
  vcValues.resize(8);
  int i;
  for (i = 0; i < 4; ++i)
    vcValues[i] = vcValues[i + 4] = vcBodyPressures[iface.BodyNode(i + iNodeOffset)];
}

#endif // _WELLCASINGDIANARUNNERHELPER_H_
