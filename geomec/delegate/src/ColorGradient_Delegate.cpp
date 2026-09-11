#include "stdafx.h"

#include "ColorGradient_Delegate.h"
#include "ColorGradient.h"
#include "AttriColorGradientDlg.h"
#include "GeomecDoc.h"
#include "OpenGLSceneBase.h"
#include "ISceneWrapper.h"

CColorGradient_Delegate::CColorGradient_Delegate(CColorGradient* colorGradient)
: IColorScaleNode_Delegate(colorGradient)
, m_colorGradient(colorGradient)
{
}

bool CColorGradient_Delegate::Attributes()
{
  CAttriColorGradientDlg dlg(*m_colorGradient);

  if (dlg.DoModal() == IDOK)
  {
  if (!m_colorGradient->IsGlobal())
  {
      m_colorGradient->SaveRegistry("ColorScale", "Local");
  }

  m_colorGradient->Modified();

  return true;
  }

  return false;
}

bool CColorGradient_Delegate::CanFindExtents() const
{
  // Get the current scene

  CFemAppDoc& doc = *GetGeomecDoc();

  if (doc.CurrentScene() && m_colorGradient->IsGlobal())
  {
  ISceneWrapper* pScene = doc.CurrentScene();

  assert(pScene);

  return pScene->ValueComponent() && pScene->MinMaxValue().first.Valid() &&
      pScene->MinMaxValue().second.Valid();
  }

  return false;
}

bool CColorGradient_Delegate::FindExtents()
{
  if (!CanFindExtents())
  {
  return false;
  }

  // Get the current scene

  CFemAppDoc& doc = *GetGeomecDoc();
  ISceneWrapper* pScene = doc.CurrentScene();

  assert(pScene);
  assert(pScene->MinMaxValue().first.Valid());
  assert(pScene->MinMaxValue().second.Valid());
  assert(m_colorGradient->IsGlobal());
  assert(pScene->ValueComponent());

  return m_colorGradient->FindExtents(pScene->MinMaxValue().first.Value(),
  pScene->MinMaxValue().second.Value());
}
