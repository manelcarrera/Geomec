// GocadElementSet.cpp: implementation of the CGocadElementSet class.
//
//////////////////////////////////////////////////////////////////////

#include "GocadElementSet.h"
#include "GoCadModel.h"
#include "TetraMesh.h"

#ifdef SKUA_NEW
#include "SKUAParseData.h"
#include "SKUAParserDataIO.h"
#endif

#ifdef SKUA_NEW
CGocadElementSet::CGocadElementSet(const QString &sName, eElementType type, const TPointVec &vcPoint,
                                   const TElementVec &vcElement, CFemAppModel &model,
                                   std::vector<const gm_skua::SKUAParseData *> &solids)
    : CElementSet(sName, type, vcPoint, vcElement, model), m_Solids(solids) {}
#else
CGocadElementSet::CGocadElementSet(const QString &sName, eElementType type, const TPointVec &vcPoint,
                                   const TElementVec &vcElement, CFemAppModel &model,
                                   std::vector<QSharedPointer<CGocadData::CTSolid>> &solids)
    : CElementSet(sName, type, vcPoint, vcElement, model), m_Solids(solids) {}
#endif

CGocadElementSet::CGocadElementSet(CFemAppModel &model) : CElementSet(model), m_Solids() {}

CGocadElementSet::~CGocadElementSet() {}

bool CGocadElementSet::canImportInGocadModel() const {
  const CGoCadModel *model = dynamic_cast<const CGoCadModel *>(&Model());

  return (model != 0) && model->CanImportMesh();
}

bool CGocadElementSet::importInGocadModel() {
  const CGoCadModel &model = dynamic_cast<const CGoCadModel &>(Model());
  CTetraMesh &mesh = const_cast<CTetraMesh &>(dynamic_cast<const CTetraMesh &>(model.Mesh()));

  return mesh.CreateMeshFromSolids("", m_Solids, 0);
}

#ifdef SKUA_NEW
bool CGocadElementSet::importInGocadModel(
    bool (*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *> &solids, std::vector<int> &vcSolids))
#else
bool CGocadElementSet::importInGocadModel(bool (*selectGocadSolidDlg)(
    const std::vector<QSharedPointer<CGocadData::CTSolid>> &solids, std::vector<int> &vcSolids))
#endif
{
  const CGoCadModel &model = dynamic_cast<const CGoCadModel &>(Model());
  CTetraMesh &mesh = const_cast<CTetraMesh &>(dynamic_cast<const CTetraMesh &>(model.Mesh()));

  return mesh.CreateMeshFromSolids("", m_Solids, selectGocadSolidDlg);
}

void CGocadElementSet::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  unsigned long solidsSize = 0;

  stream >> solidsSize;
#ifdef SKUA_NEW
  if (version > CStreamVersion(4, 1, 72)) {
    for (int i = 0; i < solidsSize; ++i) {
      gm_skua::SKUAParseData *solid = new gm_skua::SKUAParseData(gm_skua::SKUAParseData::Unknown);
      gm_skua::LoadStream(stream, version, progress, *solid);
      m_Solids.push_back(solid);
    }
  } else {
    for (int i = 0; i < solidsSize; ++i) {
      gm_skua::SKUAParseData *solid = new gm_skua::SKUAParseData(gm_skua::SKUAParseData::Unknown);
      gm_skua::LoadStreamLegacy(stream, version, progress, *solid);
      m_Solids.push_back(solid);
    }
  }
#else
  for (unsigned long l = 0; l < solidsSize; ++l) {
    QSharedPointer<CGocadData::CTSolid> solid = QSharedPointer<CGocadData::CTSolid>(new CGocadData::CTSolid());

    solid->LoadStream(stream, version, progress);
    m_Solids.push_back(solid);
  }
#endif
  CElementSet::LoadStream(stream, version, progress);
}

void CGocadElementSet::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  stream << (unsigned long)m_Solids.size();
#ifdef SKUA_NEW
  for (size_t i = 0; i < m_Solids.size(); ++i) {
    gm_skua::SaveStream(stream, progress, *m_Solids[i]);
  }
#else
  std::vector<QSharedPointer<CGocadData::CTSolid>>::const_iterator iterator = m_Solids.begin();

  for (; iterator != m_Solids.end(); ++iterator) {
    (*iterator)->SaveStream(stream, progress);
  }
#endif
  CElementSet::SaveStream(stream, progress);
}
