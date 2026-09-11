#include "stdafx.h"

#include "DocumentGUI.h"

CDocumentGUI::CDocumentGUI(CDocumentBase* documentBase)
: m_documentBase(documentBase)
{
}

CDocumentGUI::~CDocumentGUI()
{
  // All scenes are destructed by the function (at this moment)
  // CFemAppGUI::DeleteScenes()!
}

CDocumentBase* CDocumentGUI::model() const
{
  return m_documentBase;
}

