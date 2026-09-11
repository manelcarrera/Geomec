#include "stdafx.h"

#include "GoCadGUI.h"
#include "GoCadModel.h"
#include "HexaModel.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "GeomecDoc.h"

CGoCadGUI::CGoCadGUI(CFemAppGUI* parentGUI, CGoCadModel* goCadModel)
: CTetraGUI(parentGUI, goCadModel)
, m_goCadModel(goCadModel)
{
}

void CGoCadGUI::Convert(const CHexaModel& model)
{
  assert(model.IsMesh());

  // Create new model unattached to the document, convert existing
  // hexa model to tetra model in OnConvert routine. Attach the tetra
  // model to the document.

  NewModel(false);
  std::auto_ptr <IProgressBase> prog(_g->prog()->create( eProgress::Geo, "Convert to tetramodel.."));
  m_goCadModel->OnConvert(model, *prog);

  GetGeomecDoc()->OnAttachModel(*m_goCadModel, *this);

  LinkModelToOpenInventor();

  // Init all views

  POSITION pos = GetGeomecDoc()->GetFirstViewPosition();

  while (pos != NULL)
  {
  GetGeomecDoc()->GetNextView(pos)->OnInitialUpdate();
  }
}

void CGoCadGUI::OnCloseModel()
{
  CTetraGUI::OnCloseModel();
}

CGoCadModel* CGoCadGUI::model() const
{
  return m_goCadModel;
}
