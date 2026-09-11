// TetraSufarce.cpp: implementation of the CTetraSurface class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraSurface.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSuperHorizon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraSurface::CTetraSurface(CFemAppModel &model) : CSurfaceBase(model) {}

CTetraSurface::CTetraSurface(CSurfaceValueType &value_type) : CSurfaceBase(value_type) {}

CTetraSurface::CTetraSurface(const QString &strName, const std::vector<const geo::IFace *> &vcFace, CFemAppModel &model)
    : CSurfaceBase(strName, vcFace, model) {}

CTetraSurface::CTetraSurface(const QString &strName, const geo::ISurface &surface, CFemAppModel &model)
    : CSurfaceBase(strName, surface, model) {}

CTetraSurface::CTetraSurface(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel &model)
    : CSurfaceBase(strName, vcPoint, model) {}

void CTetraSurface::CreateSideSurface() {
  CTetraModel &model = static_cast<CTetraModel &>(Model());
  CTetraSubBoundary &sub = model.SubBoundary();
  sub.LinkTo(*this);
  model.InvalidateMesh();
}

void CTetraSurface::CreateSubHorizon() {
  CTetraModel &model = static_cast<CTetraModel &>(Model());
  new CTetraSubHorizon(*this, model, false);
  model.InvalidateMesh();
}

void CTetraSurface::CreateSuperHorizon() {
  CTetraModel &model = static_cast<CTetraModel &>(Model());
  new CTetraSuperHorizon(*this, model, false);
  model.InvalidateMesh();
}

void CTetraSurface::CreateSubFault() {
  CTetraModel &model = static_cast<CTetraModel &>(Model());
  new CTetraSubHorizon(*this, model, true);
  model.InvalidateMesh();
}

const CGraphNode *CTetraSurface::UsedByModelOrChildModel(const CModelBase &model) const {
  const CTetraModel *pModel = dynamic_cast<const CTetraModel *>(&model);

  // Is it used by boundary?
  if (pModel) {
    for (int i = 0; i < pModel->SubBoundary().SideSurfaceSize(); i++) {
      if (&pModel->SubBoundary().SideSurface(i) == this)
        return &pModel->SubBoundary();
    }
  }

  // try child models
  const CGraphNode *pNode = 0;
  for (int i = 0; i < model.childModelSize() && !pNode; ++i)
    pNode = UsedByModelOrChildModel(model.childModel(i));

  return pNode;
}

const CGraphNode *CTetraSurface::Used() const {
  const CModelBase &model = static_cast<const CModelBase &>(Model());
  const CGraphNode *pNode = UsedByModelOrChildModel(model);
  if (pNode)
    return pNode;

  return CSurfaceBase::Used();
}

CSurfaceBase *CTetraSurface::OnCreateSurface(const QString &sName, const geo::CPtrArray<geo::IPoint> &vcPoint) {
  return new CTetraSurface(sName, vcPoint, Model());
}

CSurfaceBase *CTetraSurface::OnCreateSurface(const QString &sName, const std::vector<const geo::IFace *> &vcFaces) {
  return new CTetraSurface(sName, vcFaces, Model());
}