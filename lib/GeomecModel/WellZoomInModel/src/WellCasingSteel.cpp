
#include "WellCasingSteel.h"

#include "BodyGroup.h"
#include "ModifiedHint.h"
#include "WellCasingCement.h"
#include "WellCasingModel.h"
#include "WellCasingSteelMaterial.h"

namespace {

const QRgb LIGHT_BROWN = qRgb(181, 101, 29);

}

CWellCasingSteel::CWellCasingSteel(const QString &strName, CWellCasingModel &model)
    : TBase(strName, model), m_bShowMidpoints(false), m_color(LIGHT_BROWN) {
  LinkTo(model.Mesh());
}

CWellCasingSteel::CWellCasingSteel(CWellCasingModel &model)
    : TBase(model), m_bShowMidpoints(false), m_color(LIGHT_BROWN) {
  LinkTo(model.Mesh());
}

bool CWellCasingSteel::operator==(const CWellCasingSteel &rhs) const {
  if (m_color != rhs.m_color)
    return false;

  return TBase::operator==(rhs);
}

CWellCasingSteel &CWellCasingSteel::operator=(const CWellCasingSteel &rhs) {
  TBase::operator=(rhs);
  m_color = rhs.m_color;
  Modified();

  return *this;
}

unsigned int CWellCasingSteel::IconId() const { return IDI_WELLCASINGMODEL; }

unsigned int CWellCasingSteel::TypeId() const { return 0; }

QString CWellCasingSteel::TypeName() const {
  // TODO MAGIC
  return "Well";
}

bool CWellCasingSteel::Less(const CGraphNode & /*node*/) const { return true; }

void CWellCasingSteel::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (&node == &(static_cast<const CModelBase &>(Model())).Mesh() && (uHint == MeshCleared || uHint == MeshCreated)) {
    m_centerpoints.Clear();
    Modified();
    return;
  }

  TBase::OnNeighbourModified(node, uHint);
}

bool CWellCasingSteel::Empty() const { return false; }

long CWellCasingSteel::SavedItems() const { return 0; }

int CWellCasingSteel::DisplayListSize() const {
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  if (model.Mesh().IsMesh())
    return 1;

  return 0;
}

const geo::IObject &CWellCasingSteel::DisplayList(int /*nIndex*/) const {
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  if (model.Mesh().IsMesh()) {
    if (m_bShowMidpoints) {
      if (m_centerpoints.Empty())
        m_centerpoints.Create(model.Mesh().SteelElements());
      return m_centerpoints.Get();
    } else {
      return model.Mesh().SteelElements();
    }
  }

  assert(FALSE);
  geo::IObject *pBogus = 0;
  return *pBogus;
}

CWellCasingSteel::TColor CWellCasingSteel::Color() const { return m_color; }

void CWellCasingSteel::Color(TColor nColor) { m_color = nColor; }

void CWellCasingSteel::OnShowElements() {
  if (m_bShowMidpoints) {
    m_bShowMidpoints = false;
    Modified();
  }
}

void CWellCasingSteel::OnShowMidpoints() {
  if (!m_bShowMidpoints) {
    m_bShowMidpoints = true;
    Modified();
  }
}

void CWellCasingSteel::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  TBase::LoadStream(stream, version, progress);
}

/////

CWellCasingMaterialServer::CWellCasingMaterialServer(TWellCasingMaterialServerParent &parent,
                                                     const CDepletionStage &stage)
    : TBase(parent, stage) {
  reParent(&parent);
  LinkTo(const_cast<CDepletionStage &>(stage));
}

const CFFMaterial &CWellCasingMaterialServer::Material(const geo::IElement &element) const {
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());

  // get the adjacent cement element
  const geo::IElement *pCementElement = 0;
  int i;
  for (i = element.Index() + 1; i < model.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &cement_element = model.Mesh().Mesh().Element(i);
    if (model.Mesh().IsCementElement(cement_element)) {
      pCementElement = &cement_element;
      break;
    }
  }

  assert(pCementElement);
  CFFMaterial::TValueMap mpValue;

  if (pCementElement) {
    // get the material for the cement element
    const CFFMaterial &cement_mat = model.CasingCement().Material(Stage()).Material(*pCementElement);
    assert(cement_mat.IsParameter(IDT_VALUETYPE_PERMEA));
    double dPermea = cement_mat.ParameterValue(IDT_VALUETYPE_PERMEA);
    assert(cement_mat.IsParameter(IDT_VALUETYPE_VISCOSITY));
    double dVisco = cement_mat.ParameterValue(IDT_VALUETYPE_VISCOSITY);

    // map values and force mapping of permeability and viscosity
    MapValueTypes(element, mpValue);
    mpValue[IDT_VALUETYPE_PERMEA] = dPermea;
    mpValue[IDT_VALUETYPE_VISCOSITY] = dVisco;
  }

  // get the material for this element from base class
  return FetchFFMaterial(mpValue);
}

CWellCasingSteelMaterial *CWellCasingMaterialServer::FindIndexInEntry(int nIndex) const {
  CWellCasingSteelMaterialEntry &material_entry =
      (CWellCasingSteelMaterialEntry &)*((CModelBase &)Model()).GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY);
  assert(material_entry.FindIndex(nIndex));
  return material_entry.FindIndex(nIndex);
}