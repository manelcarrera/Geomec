
#include "DocumentProxyConsole.h"

CDocumentProxyConsole::CDocumentProxyConsole() : CDocumentProxy() {}

CDocumentProxyConsole::~CDocumentProxyConsole() {}

ISceneWrapper *CDocumentProxyConsole::currentScene() { return 0; }

void CDocumentProxyConsole::currentScene(ISceneWrapper * /*sceneWrapper*/) {}

void CDocumentProxyConsole::onAnalysisProperties() {}

void CDocumentProxyConsole::onInvalidateStorage() {}

void CDocumentProxyConsole::setModifiedFlag(bool /*modified*/) {}
