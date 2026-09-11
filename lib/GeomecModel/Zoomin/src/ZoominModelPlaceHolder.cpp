#include "ZoominModelPlaceHolder.h"

#include "BoundaryBase.h"
#include "VersionNumbers.h"

#include "BaseEntryTypes.h"
#include "GlobalMessage.h"
#include "GoCadModel.h"
#include "HexaModel.h"
#include "ModelLifetimeFacade.h"

CZoominModelPlaceHolder::CZoominModelPlaceHolder(CFemAppModel &model) : COpenGLNode(model), m_pChildModel(0) {}

CZoominModelPlaceHolder::CZoominModelPlaceHolder(CFemAppModel &model, CModelBase &childModel)
    : COpenGLNode(model), m_pChildModel(&childModel) {}

CZoominModelPlaceHolder::~CZoominModelPlaceHolder() {
  if (m_pChildModel)
    IModelLifetimeFacade::Close(m_pChildModel);

  // update surface icons (used state)
  Model().GraphEntry(MD_BASE_SURFACE)->Modified();
}

CModelBase &CZoominModelPlaceHolder::ChildModel() { return *m_pChildModel; }

const CModelBase &CZoominModelPlaceHolder::ChildModel() const { return *m_pChildModel; }

int CZoominModelPlaceHolder::DisplayListSize() const { return m_pChildModel->Boundary().DisplayListSize(); }

const geo::IObject &CZoominModelPlaceHolder::DisplayList(int nIndex) const {
  return m_pChildModel->Boundary().DisplayList(nIndex);
}

CZoominModelPlaceHolder::TColor CZoominModelPlaceHolder::Color() const { return m_pChildModel->Boundary().Color(); }

unsigned int CZoominModelPlaceHolder::IconId() const { return m_pChildModel->IconId(); }

unsigned int CZoominModelPlaceHolder::TypeId() const { return m_pChildModel->TypeId(); }

bool CZoominModelPlaceHolder::Empty() const { return false; }

long CZoominModelPlaceHolder::SavedItems() const { return COpenGLNode::SavedItems() + m_pChildModel->SavedItems(); }

void CZoominModelPlaceHolder::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  COpenGLNode::LoadStream(stream, version, progress);

  int nModelType;
  stream >> nModelType;

  CModelBase &model = static_cast<CModelBase &>(Model());

  if (version == CStreamVersion(4, 1, 34)) {
    if (nModelType != HEXA_MODEL && nModelType != GOCAD_MODEL) {
      int dummy;
      for (int i = 0; i < 19; ++i)
        stream >> dummy;

      QString name;
      stream >> name;
      Name(name);
      int index;
      stream >> index;
      Index(index);

      stream >> nModelType;

      if (nModelType != HEXA_MODEL && nModelType != GOCAD_MODEL) {
        _m()->msg("Warning: an error occurred while loading the zoom-in models. Please contact Geomec support.");
        return;
      }
    }
  }

  switch (nModelType) {
  case HEXA_MODEL:
  case GOCAD_MODEL:
    m_pChildModel = IModelLifetimeFacade::NewChild(nModelType, model.Logger(), model.getVersionManager());
    break;
  default:
    assert(false);
  }

  reParent(model.GraphEntry(MD_BASE_ZOOMIN_MODEL));
  model.OnNewChildModel(*m_pChildModel, false);
  model.LoadChildModel(stream, version, progress, *m_pChildModel);
}

void CZoominModelPlaceHolder::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  COpenGLNode::SaveStream(stream, progress);

  CHexaModel *pHexa = dynamic_cast<CHexaModel *>(m_pChildModel);
  CGoCadModel *pGoCad = dynamic_cast<CGoCadModel *>(m_pChildModel);

  if (pHexa) {
    stream << HEXA_MODEL;
  } else {
    assert(pGoCad);
    stream << GOCAD_MODEL;
  }

  CModelBase &model = static_cast<CModelBase &>(Model());
  model.SaveChildModel(stream, progress, *m_pChildModel);
}
