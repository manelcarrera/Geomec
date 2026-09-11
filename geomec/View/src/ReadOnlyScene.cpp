#include "StdAfx.h"

#include "ReadOnlyScene.h"

CReadOnlyScene::CReadOnlyScene(const QString& type, CFemAppModel& femAppModel)
: COpenGLSceneBase(femAppModel)
, m_type(type)
{
}

CReadOnlyScene::~CReadOnlyScene()
{
}

namespace
{

const QString BIRDEYE_SCENE = "BIRDEYE";
const QString XSEC_SCENE = "XSEC";
const QString WPGRPH_SCENE = "WPGRPH";
const QString HISTORY_SCENE = "HISTORY";
const QString RTCI_SCENE = "RTCI";
const QString TOP_SCENE = "TOP";
const QString DEFAULT_SCENE = "DEFAULT";

} // anonymous namespace

void CReadOnlyScene::LoadStream(TSTREAM& stream, CStreamVersion &version,
  TPROGRESS& progress)
{
  if ((m_type == BIRDEYE_SCENE) || (m_type == XSEC_SCENE) ||
  (m_type == WPGRPH_SCENE) || (m_type == HISTORY_SCENE) ||
  (m_type == RTCI_SCENE) || (m_type == TOP_SCENE) ||
  (m_type == DEFAULT_SCENE))
  {
  COpenGLSceneBase::LoadStream(stream, version, progress);
  }
}

void CReadOnlyScene::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  assert(false);

  if ((m_type == BIRDEYE_SCENE) || (m_type == XSEC_SCENE) ||
  (m_type == WPGRPH_SCENE) || (m_type == HISTORY_SCENE) ||
  (m_type == RTCI_SCENE) || (m_type == TOP_SCENE) ||
  (m_type == DEFAULT_SCENE))
  {
  COpenGLSceneBase::SaveStream(stream, progress);
  }
}
