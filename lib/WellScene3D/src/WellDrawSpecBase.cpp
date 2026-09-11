// WellDrawSpecBase.cpp: implementation of the CWellDrawSpecBase class.
//
//////////////////////////////////////////////////////////////////////
#include "WellDrawSpecBase.h"
#include "OpenGLScene.h"
#include "StdAfx.h"
#include "WellPointDrawSpec.h"
#include "WellSceneInterMed.h"
#include "WellSectionDrawSpec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWellDrawSpecBase::CWellDrawSpecBase(CWellSceneInterMed &WellSceneInterMed) : m_pWellSceneInterMed(&WellSceneInterMed) {
  m_Align = CLabelPoint::LA_BOTTOM_LEFT;
  m_LineWidth = 2;
  m_Visible = false;
  m_Color = qRgb(255, 255, 255);
  m_ArrowColor = qRgb(255, 255, 255);
  m_TextColor = qRgb(255, 255, 255);
  m_PointSize = 3;
}

CWellDrawSpecBase::~CWellDrawSpecBase() {}

void CWellDrawSpecBase::OnSceneAttached() {
  assert(m_pWellSceneInterMed->Scene());

  for (int i = 0; i < m_DisplayList.count(); i++) {
    CDisplayPair *p = m_DisplayList.at(i);
    m_pWellSceneInterMed->Scene()->insert(p->Array(), p->DrawDef());
  }
}

void CWellDrawSpecBase::OnSceneDetached() {
  assert(m_pWellSceneInterMed->Scene());

  for (int i = 0; i < m_DisplayList.count(); i++) {
    CDisplayPair *p = m_DisplayList.at(i);
    m_pWellSceneInterMed->Scene()->erase(p->Array(), false);
  }
}

void CWellDrawSpecBase::DeleteAllDisplayList() {
  for (int i = 0; i < m_DisplayList.count(); i++) {
    CDisplayPair *p = m_DisplayList.at(i);

    if (m_pWellSceneInterMed->Scene())
      m_pWellSceneInterMed->Scene()->erase(p->Array(), false);
  }
  RemoveAllObjects();
}

void CWellDrawSpecBase::UpdateScene() {
  if (!m_pWellSceneInterMed->Scene())
    return;

  for (int i = 0; i < m_DisplayList.count(); i++) {
    CDisplayPair *p = m_DisplayList.at(i);
    if (!m_pWellSceneInterMed->Scene()->IsDisplayList(p->Array())) {
      m_pWellSceneInterMed->Scene()->insert(p->Array(), p->DrawDef());
    } else {
      m_pWellSceneInterMed->Scene()->invalidate(p->Array(), false);
    }
  }
}

bool CWellDrawSpecBase::GetVisible() const { return m_Visible; }

void CWellDrawSpecBase::SetVisible(bool visible) {
  m_Visible = visible;
  CreateScene();
}

QRgb CWellDrawSpecBase::GetColor() const { return m_Color; }

void CWellDrawSpecBase::SetColor(QRgb color) {
  m_Color = color;
  CreateScene();
}

int CWellDrawSpecBase::GetLineWidth() const { return m_LineWidth; }

void CWellDrawSpecBase::SetLineWidth(int linewidth) {
  m_LineWidth = linewidth;
  CreateScene();
}

CWellSceneInterMed &CWellDrawSpecBase::InterMed() { return *m_pWellSceneInterMed; }

QRgb CWellDrawSpecBase::GetTextColor() const { return m_TextColor; }

void CWellDrawSpecBase::SetTextColor(QRgb color) {
  m_TextColor = color;
  CreateScene();
}

QString CWellDrawSpecBase::GetDescription() const { return m_Description; }

void CWellDrawSpecBase::SetDescription(QString description) {
  m_Description = description;
  CreateScene();
}

int CWellDrawSpecBase::GetPointSize() const { return m_PointSize; }

CLabelPoint::eLabelAlign CWellDrawSpecBase::GetAlign() const { return m_Align; }

void CWellDrawSpecBase::SetAlign(CLabelPoint::eLabelAlign align) { m_Align = align; }

void CWellDrawSpecBase::SetPointSize(int pointsize) {
  m_PointSize = pointsize;
  CreateScene();
}

QRgb CWellDrawSpecBase::ArrowColor() const { return m_ArrowColor; }

void CWellDrawSpecBase::ArrowColor(QRgb color) {
  m_ArrowColor = color;
  CreateScene();
}
