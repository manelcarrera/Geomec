
#include "BoundaryInterfaceDef.h"
#include "BoundaryBase.h"
#include "BoundaryInterfaceMaterial.h"
#include "FormationBase.h"
#include "IWellModel.h"
#include "InterfaceElement.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "StreamVersion.h"

CBoundaryInterfaceDef::CBoundaryInterfaceDef(CBoundaryBase &boundary)
    : CStorageNode(boundary.Model()), m_boundary(boundary), m_ShearQuantity(1.E7, 0.0, DOUBLE_MAX_, true, true),
      m_lambda(100), m_KradTop(0.001, 0.0, DOUBLE_MAX_, true, true), m_KradBottom(100., 0.0, DOUBLE_MAX_, true, true),
      m_KradSides(100., 0.0, DOUBLE_MAX_, true, true), m_KtanTop(0.001, 0.0, DOUBLE_MAX_, true, true),
      m_KtanBottom(0.001, 0.0, DOUBLE_MAX_, true, true), m_KtanSides(0.001, 0.0, DOUBLE_MAX_, true, true),
      m_eIntMatType(K), m_pBoundIntMatTop(0), m_pBoundIntMatBottom(0), m_pBoundIntMatSides(0),
      m_updateLastSideInterfaceElement(true), m_lastSideInterfaceElement(0) {}

CBoundaryInterfaceDef::CBoundaryInterfaceDef(const CBoundaryInterfaceDef &rhs)
    : CStorageNode(rhs), m_boundary(rhs.m_boundary), m_ShearQuantity(rhs.m_ShearQuantity), m_lambda(100),
      m_KradTop(rhs.m_KradTop), m_KradBottom(rhs.m_KradBottom), m_KradSides(rhs.m_KradSides), m_KtanTop(rhs.m_KtanTop),
      m_KtanBottom(rhs.m_KtanBottom), m_KtanSides(rhs.m_KtanSides), m_eIntMatType(rhs.m_eIntMatType),
      m_pBoundIntMatTop(0), m_pBoundIntMatBottom(0), m_pBoundIntMatSides(0), m_updateLastSideInterfaceElement(true),
      m_lastSideInterfaceElement(0) {}

CBoundaryInterfaceDef::~CBoundaryInterfaceDef() { ClearInterfaceMaterials(); }

bool CBoundaryInterfaceDef::operator==(const CBoundaryInterfaceDef &rhs) const {
  return (CStorageNode::operator==(rhs) && m_ShearQuantity == rhs.m_ShearQuantity && m_lambda == rhs.m_lambda &&
          m_KradTop == rhs.m_KradTop && m_KradBottom == rhs.m_KradBottom && m_KradSides == rhs.m_KradSides &&
          m_KtanTop == rhs.m_KtanTop && m_KtanBottom == rhs.m_KtanBottom && m_KtanSides == rhs.m_KtanSides &&
          m_eIntMatType == rhs.m_eIntMatType);
}

CBoundaryInterfaceDef &CBoundaryInterfaceDef::operator=(const CBoundaryInterfaceDef &rhs) {
  CStorageNode::operator=(rhs);

  m_ShearQuantity = rhs.m_ShearQuantity;
  m_lambda = rhs.m_lambda;
  m_KradTop = rhs.m_KradTop;
  m_KradBottom = rhs.m_KradBottom;
  m_KradSides = rhs.m_KradSides;
  m_KtanTop = rhs.m_KtanTop;
  m_KtanBottom = rhs.m_KtanBottom;
  m_KtanSides = rhs.m_KtanSides;
  m_eIntMatType = rhs.m_eIntMatType;

  return *this;
}

void CBoundaryInterfaceDef::IntMatType(eIntMatType val) {
  if (m_eIntMatType == val)
    return;

  m_eIntMatType = val;
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::Shear(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_ShearQuantity.Value(unit) == val)
    return;

  m_ShearQuantity.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::Lambda(double val) {
  if (m_lambda == val)
    return;

  m_lambda = val;
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KradTop(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KradTop.Value(unit) == val)
    return;

  m_KradTop.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KradBottom(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KradBottom.Value(unit) == val)
    return;

  m_KradBottom.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KradSides(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KradSides.Value(unit) == val)
    return;

  m_KradSides.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KtanTop(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KtanTop.Value(unit) == val)
    return;

  m_KtanTop.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KtanBottom(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KtanBottom.Value(unit) == val)
    return;

  m_KtanBottom.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CBoundaryInterfaceDef::KtanSides(const double &val, CDoubleQuantity::UNIT unit) {
  if (m_KtanSides.Value(unit) == val)
    return;

  m_KtanSides.Value(val, unit);
  CModelBase &model = (CModelBase &)(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

// functions to calculate from G to Ktan or Krad and vice versa using the corrected sphere
// approach
double CBoundaryInterfaceDef::Krad(CDoubleQuantity::UNIT unit) const {
  return ((4.0 * m_ShearQuantity.Value(unit)) / m_boundary.R().Value(unit)) * 0.76;
}

double CBoundaryInterfaceDef::Ktan(CDoubleQuantity::UNIT unit) const {
  return ((3.0 * m_ShearQuantity.Value(unit)) / m_boundary.R().Value(unit)) * 0.52;
}

CBoundaryInterfaceMaterial &CBoundaryInterfaceDef::FindOrCreateInterfaceMaterial(const geo::CInterfaceElement &iface,
                                                                                 bool isBottom) const {
  static double last_E = 0;
  static double last_G = 0;

  CBoundaryInterfaceMaterial *mat = 0;

  std::map<const geo::CInterfaceElement *, CBoundaryInterfaceMaterial *>::iterator it = m_materials.find(&iface);

  if (it != m_materials.end()) {
    mat = it->second;
  } else {
    CModelBase *model = static_cast<CModelBase *>(const_cast<CFemAppModel *>(&Model()));
    CModelBase *parentModel = dynamic_cast<CModelBase *>(model->parentModel());

    const CDepletionStage &initialStage = model->InitialDepletionStage();

    const geo::IBody *body = dynamic_cast<const geo::CBodyQuadrilateral *>(&iface.Front())
                                 ? static_cast<const geo::CBodyQuadrilateral *>(&iface.Front())->Parent()
                                 : static_cast<const geo::CBodyTriangle *>(&iface.Front())->Parent();

    const geo::CPoint point = body ? body->MidPoint() : iface.MidPoint();

    double E = 10000, G = 4166; // defaults in linear material

    if (!parentModel) {
      const std::set<int> elts = model->Mesh().Mesh().Candidates(point);

      if (!elts.empty()) {
        const geo::IElement &elt = model->Mesh().Mesh().Element(*elts.begin());
        const CFormationBase &formationBase = *model->Mesh().Formation(elt);

        const CFFMaterial &cffmat = formationBase.Material(initialStage).Material(elt);

        E = cffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
        G = cffmat.ParameterValue(IDT_VALUETYPE_SHEARMODULUS);
      }
    } else {
      const CDepletionStage &parentInitialStage = model->ParentEquivalentDepletionStage(initialStage);
      const std::set<int> elts = parentModel->Mesh().Mesh().Candidates(point);

      if (!elts.empty()) {
        const CFormationBase *myformation = model->Mesh().Formation(body ? *body : iface.Front());

        E = G = 0;
        int count = 0;

        for (std::set<int>::const_iterator ite = elts.begin(); ite != elts.end(); ++ite) {
          const geo::IElement &elt = parentModel->Mesh().Mesh().Element(*ite);

          if (elt.IsInterfaceElement())
            continue;

          const CFormationBase &formationBase = *parentModel->Mesh().Formation(elt);

          if (myformation && formationBase.Name() != myformation->Name())
            continue;

          if (!elt.Contains(point, true))
            continue;

          const CFFMaterial &cffmat = formationBase.Material(parentInitialStage).Material(elt);

          E += cffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
          G += cffmat.ParameterValue(IDT_VALUETYPE_SHEARMODULUS);

          ++count;
        }

        if (count > 0) {
          E /= count;
          G /= count;

          last_E = E;
          last_G = G;
        } else {
          E = last_E;
          G = last_G;
        }
      }
    }

    const geo::CInterfaceElement *useIFace = &iface;

    if (isBottom && m_lastSideInterfaceElement && model->IsCylindricalModel()) {
      useIFace = m_lastSideInterfaceElement;
      m_updateLastSideInterfaceElement = false;
    }

    double delta = 0;
    for (int i = 0; i < useIFace->NrOfLines(); ++i)
      delta += useIFace->Line(i).Length();
    delta /= useIFace->NrOfLines();

    mat = new CBoundaryInterfaceMaterial(m_lambda * E / delta * 1e6, m_lambda * G / delta * 1e6);

    m_materials.insert(std::make_pair(&iface, mat));
  }

  assert(mat);
  return *mat;
}

const CBoundaryInterfaceMaterial &
CBoundaryInterfaceDef::InterfaceMaterialTop(const geo::CInterfaceElement &iface) const {
  if (!m_pBoundIntMatTop)
    GenerateInterfaceMaterials();

  return *m_pBoundIntMatTop;
}

const CBoundaryInterfaceMaterial &
CBoundaryInterfaceDef::InterfaceMaterialBottom(const geo::CInterfaceElement &iface) const {
  if (IntMatType() != L) {
    if (!m_pBoundIntMatBottom)
      GenerateInterfaceMaterials();

    return *m_pBoundIntMatBottom;
  } else {
    return FindOrCreateInterfaceMaterial(iface, true);
  }
}

const CBoundaryInterfaceMaterial &
CBoundaryInterfaceDef::InterfaceMaterialSides(const geo::CInterfaceElement &iface) const {
  if (!m_lastSideInterfaceElement || m_updateLastSideInterfaceElement)
    m_lastSideInterfaceElement = &iface;

  if (IntMatType() != L) {
    if (!m_pBoundIntMatSides)
      GenerateInterfaceMaterials();

    return *m_pBoundIntMatSides;
  } else {
    return FindOrCreateInterfaceMaterial(iface);
  }
}

void CBoundaryInterfaceDef::ClearInterfaceMaterials() {
  if (m_pBoundIntMatTop) {
    delete m_pBoundIntMatTop;
    m_pBoundIntMatTop = 0;
  }

  if (m_pBoundIntMatBottom) {
    delete m_pBoundIntMatBottom;
    m_pBoundIntMatBottom = 0;
  }

  if (m_pBoundIntMatSides) {
    delete m_pBoundIntMatSides;
    m_pBoundIntMatSides = 0;
  }

  for (std::map<const geo::CInterfaceElement *, CBoundaryInterfaceMaterial *>::iterator it = m_materials.begin();
       it != m_materials.end(); ++it)
    delete it->second;
  m_materials.clear();

  m_updateLastSideInterfaceElement = true;
  m_lastSideInterfaceElement = 0;
}

unsigned int CBoundaryInterfaceDef::IconId() const { return 0; }

unsigned int CBoundaryInterfaceDef::TypeId() const { return 0; }

bool CBoundaryInterfaceDef::Empty() const { return false; }

long CBoundaryInterfaceDef::SavedItems() const { return 0; }

void CBoundaryInterfaceDef::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS & /*progress*/) {
  int temp;
  double dVal;

  bool bSetLoadedValues = true;
  if (version < CStreamVersion(3, 0, 108)) {
    stream >> temp; // wedx 24-04-2009: "user defined or not" was removed in 3.0.108
    if (!temp)
      bSetLoadedValues = false; // don't load old defaults
  }

  stream >> temp;

  if (version > CStreamVersion(4, 1, 69)) {
    switch (temp) {
    case 0:
      m_eIntMatType = K;
      break;
    case 1:
      m_eIntMatType = G;
      break;
    case 2:
      m_eIntMatType = L;
      break;
    }
  } else {
    m_eIntMatType = (temp == 1) ? G : K;
  }

  stream >> dVal;
  if (bSetLoadedValues)
    m_ShearQuantity.SetValue(dVal);

  if (version > CStreamVersion(4, 1, 69)) {
    stream >> dVal;
    m_lambda = dVal;
  }

  if (version < CStreamVersion(3, 0, 107)) {
    // single K values for all boundary surfaces
    stream >> dVal;
    if (bSetLoadedValues) {
      m_KradTop.SetValue(dVal);
      m_KradBottom.SetValue(dVal);
      m_KradSides.SetValue(dVal);
    }

    stream >> dVal;
    if (bSetLoadedValues) {
      m_KtanTop.SetValue(dVal);
      m_KtanBottom.SetValue(dVal);
      m_KtanSides.SetValue(dVal);
    }
  } else {
    stream >> dVal;
    if (bSetLoadedValues)
      m_KradTop.SetValue(dVal);

    stream >> dVal;
    if (bSetLoadedValues)
      m_KradBottom.SetValue(dVal);

    stream >> dVal;
    if (bSetLoadedValues)
      m_KradSides.SetValue(dVal);

    stream >> dVal;
    if (bSetLoadedValues)
      m_KtanTop.SetValue(dVal);

    stream >> dVal;
    if (bSetLoadedValues)
      m_KtanBottom.SetValue(dVal);

    stream >> dVal;
    if (bSetLoadedValues)
      m_KtanSides.SetValue(dVal);
  }
}

void CBoundaryInterfaceDef::SaveStream(TSTREAM &stream, TPROGRESS & /*progress*/) {
  switch (m_eIntMatType) {
  case K:
    stream << 0;
    break;
  case G:
    stream << 1;
    break;
  case L:
    stream << 2;
    break;
  }

  stream << m_ShearQuantity.Value();

  stream << m_lambda;

  stream << m_KradTop.Value();
  stream << m_KradBottom.Value();
  stream << m_KradSides.Value();

  stream << m_KtanTop.Value();
  stream << m_KtanBottom.Value();
  stream << m_KtanSides.Value();
}

void CBoundaryInterfaceDef::GenerateInterfaceMaterials() const {
  double kradtop = 0, kradbottom = 0, kradsides = 0, ktantop = 0, ktanbottom = 0, ktansides = 0;
  switch (IntMatType()) {
  case G:
    kradtop = kradbottom = kradsides = Krad(CDoubleQuantity::SI_UNIT);
    ktantop = ktanbottom = ktansides = Ktan(CDoubleQuantity::SI_UNIT);
    break;
  case K:
  case L:
    kradtop = KradTop().Value();
    ktantop = KtanTop().Value();
    kradbottom = KradBottom().Value();
    ktanbottom = KtanBottom().Value();
    kradsides = KradSides().Value();
    ktansides = KtanSides().Value();
    break;
  default:
    assert(false);
  }

  assert(!m_pBoundIntMatTop && !m_pBoundIntMatBottom && !m_pBoundIntMatSides);
  m_pBoundIntMatTop = new CBoundaryInterfaceMaterial(kradtop * 1e6, ktantop * 1e6);
  m_pBoundIntMatBottom = new CBoundaryInterfaceMaterial(kradbottom * 1e6, ktanbottom * 1e6);
  m_pBoundIntMatSides = new CBoundaryInterfaceMaterial(kradsides * 1e6, ktansides * 1e6);
}
