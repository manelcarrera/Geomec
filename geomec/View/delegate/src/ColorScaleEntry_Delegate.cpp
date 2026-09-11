#include "stdafx.h"

#include "ColorScaleEntry_Delegate.h"
#include "ColorScaleEntry.h"
#include "AttriColorScaleEntryDlg.h"
#include "resourceIDS.h"
#include "ColorGradient.h"
#include "AttriColorGradientDlg.h" 
#include "HotSpot.h"
#include "AttriHotSpotDlg.h"
#include "GeomecDoc.h"
#include "OpenGLSceneBase.h"
#include "ISceneWrapper.h"
#include "OpenInventorSceneNode.h"

CColorScaleEntry_Delegate::CColorScaleEntry_Delegate(
  CColorScaleEntry* colorScaleEntry)
: IColorScaleEntry_Delegate(colorScaleEntry)
, m_colorScaleEntry(colorScaleEntry)
{
}

bool CColorScaleEntry_Delegate::Attributes()
{
  CAttriColorScaleEntryDlg dlg(*m_colorScaleEntry);

  bool retval = dlg.DoModal() == IDOK;

  CFemAppDoc& doc = *GetGeomecDoc();
  ISceneWrapper* pScene = doc.CurrentScene();
  COpenInventorSceneNode *node = pScene->OpenInventorScene();

  if (node)
  node->UpdateLegend();

  return retval;
}

void CColorScaleEntry_Delegate::OnNewColorGradient()
{
  // Get the current scene

  CFemAppDoc& doc = *GetGeomecDoc();
  ISceneWrapper* pScene = doc.CurrentScene();

  assert(pScene);
  // assert(pScene->MinMaxValue().first.Valid());
  // assert(pScene->MinMaxValue().second.Valid());
  assert(pScene->ValueComponent());

  // Create a new global gradient

  CColorGradient* pGradient = new CColorGradient(IDS_NEW_COLOR_GRADIENT,
  m_colorScaleEntry->Model(),
  pScene->MinValue(), pScene->MaxValue(),
  m_colorScaleEntry->LocalColorGradient()->MinColor(),
  m_colorScaleEntry->LocalColorGradient()->MaxColor());
  CAttriColorGradientDlg dlg(*pGradient);

  if (dlg.DoModal() == IDCANCEL)
  {
  delete pGradient;
  }
  else
  {
  // Make the dialog the current selection

  m_colorScaleEntry->SelectGlobalColorGradient(*pGradient);
  }
}

void CColorScaleEntry_Delegate::OnNewHotSpot()
{
  // Create a new global gradient

  CHotSpot* pHotSpot = new CHotSpot(IDS_NEW_HOTSPOT, m_colorScaleEntry->Model(),
  m_colorScaleEntry->LocalColorGradient()->MinColor(),
  m_colorScaleEntry->LocalColorGradient()->MaxColor()); 
  CAttriHotSpotDlg dlg(*pHotSpot);

  if (dlg.DoModal() == IDCANCEL)
  {
  delete pHotSpot;
  }
  else
  {
  // Make the dialog the current selection

  m_colorScaleEntry->SelectHotSpot(*pHotSpot);
  }
}
