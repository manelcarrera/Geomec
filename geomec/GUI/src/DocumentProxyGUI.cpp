#include "stdafx.h"

#include "DocumentProxyGUI.h"
#include "GeomecDoc.h"
#include "ISceneWrapper.h"

CDocumentProxyGUI::CDocumentProxyGUI(CFemAppDoc& document)
: CDocumentProxy()
, m_document(document)
{
}

CDocumentProxyGUI::~CDocumentProxyGUI()
{
}

ISceneWrapper* CDocumentProxyGUI::currentScene()
{
  return m_document.CurrentScene();
}

void CDocumentProxyGUI::currentScene(ISceneWrapper* sceneWrapper)
{
  m_document.CurrentScene(sceneWrapper);
}

void CDocumentProxyGUI::onAnalysisProperties()
{
  dynamic_cast <CGeomecDoc&> (m_document).OnAnalysisProperties();
}

void CDocumentProxyGUI::onInvalidateStorage()
{
  m_document.OnInvalidateStorage();
}

void CDocumentProxyGUI::setModifiedFlag(bool modified)
{
  m_document.SetModifiedFlag(modified);
}
