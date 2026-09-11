// IStressTensorGroup.cpp: implementation of the IStressTensorGroup class.
//
//////////////////////////////////////////////////////////////////////
#include "IStressTensorGroup.h"
#include "BaseEntryTypes.h"
#include "FemAppModel.h"
#include "FieldFactor.h"
#include "Hexahedron.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "ResultCache.h"
#include "ResultInfo.h"
#include "ResultTree.h"
#include "TextTagUtils.h"
#include "WellCasingModel.h"
#include "WellCasingRTCI.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IStressTensorGroup::IStressTensorGroup(unsigned int uResultName, TValueSetFunction function, CResultGroup &group,
                                       bool bChange)
    : ITensorGroupTemplate<CStressTensor>(uResultName, function, group), m_bChange(bChange), m_pInVariant(0),
      m_pWPStressComposite(0), m_pVerticalGradientComposite(0) {
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX] = 0;
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED] = 0;
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN] = 0;
}

IStressTensorGroup::IStressTensorGroup(const QString &sResultName, TValueSetFunction function, CResultGroup &group,
                                       bool bChange)
    : ITensorGroupTemplate<CStressTensor>(sResultName, function, group), m_bChange(bChange), m_pInVariant(0),
      m_pWPStressComposite(0), m_pVerticalGradientComposite(0) {
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX] = 0;
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED] = 0;
  m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN] = 0;
}

IStressTensorGroup::~IStressTensorGroup() {
  delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX];
  delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED];
  delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN];
}

void IStressTensorGroup::CreateComposite() {
  ITensorGroupTemplate<CStressTensor>::CreateComposite();

  if (m_bChange) {
    CResultGroup *pPrincipalDirections = new CResultGroup(IDS_RG_PRINC_STRESS, *this);
    delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX];
    delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED];
    delete m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN];
    m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX] = new CPrincipalLengthChangeComposite(
        IDS_RG_PRINC_MAX, *pPrincipalDirections, CPrincipalLengthChangeComposite::PD_MAX);
    m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED] = new CPrincipalLengthChangeComposite(
        IDS_RG_PRINC_MED, *pPrincipalDirections, CPrincipalLengthChangeComposite::PD_MED);
    m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN] = new CPrincipalLengthChangeComposite(
        IDS_RG_PRINC_MIN, *pPrincipalDirections, CPrincipalLengthChangeComposite::PD_MIN);
  } else {
    CreatePrincipalDirections();
  }

  delete m_pInVariant;
  m_pInVariant = new CInvariantComposite(IDS_RG_INVARIANTS, *this);

  delete m_pWPStressComposite;
  m_pWPStressComposite = new CWellPathStressComposite(IDS_RG_WPSTRESS, *this);

  if (!m_bChange)
    m_pVerticalGradientComposite = new CVerticalGradientComposite(IDS_RG_VERTICALSTRESSGRADIENT, *this);
}

bool IStressTensorGroup::OnBuildComponent(const CDepletionStage &stage, const CAnalysisType & /*antype*/,
                                          int /*nRegister*/) const {
  return !(stage.Initial() && m_bChange);
}

double IStressTensorGroup::ConvertToField(const double &dValue) const { return dValue * FF_FACTOR_STRESS; }

QString IStressTensorGroup::UnitName(CQuantity::UNIT unit) const {
  QString sUnit;
  if (unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
  else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
  return sUnit;
}

const IStressTensorGroup::CPrincipalLengthChangeComposite *IStressTensorGroup::MaxPrincipleLengthChange() const {
  return m_pLengthChange[CPrincipalLengthChangeComposite::PD_MAX];
}

const IStressTensorGroup::CPrincipalLengthChangeComposite *IStressTensorGroup::MedPrincipleLengthChange() const {
  return m_pLengthChange[CPrincipalLengthChangeComposite::PD_MED];
}

const IStressTensorGroup::CPrincipalLengthChangeComposite *IStressTensorGroup::MinPrincipleLengthChange() const {
  return m_pLengthChange[CPrincipalLengthChangeComposite::PD_MIN];
}

///// IStressTensorGroup::CPrincipalLengthChangeComposite

IStressTensorGroup::CPrincipalLengthChangeComposite::CPrincipalLengthChangeComposite(unsigned int uName,
                                                                                     CResultGroup &group,
                                                                                     PRINCIPAL_DIRECTION direction)
    : ITensorGroup::CVectorComposite(uName, group, direction) {
  m_direction = direction;
  ClearComponents();
  BuildComponents();
}

IStressTensorGroup::CPrincipalLengthChangeComposite::CPrincipalLengthChangeComposite(const QString &name,
                                                                                     CResultGroup &group,
                                                                                     PRINCIPAL_DIRECTION direction)
    : ITensorGroup::CVectorComposite(name, group, direction) {
  m_direction = direction;
  ClearComponents();
  BuildComponents();
}

unsigned int IStressTensorGroup::CPrincipalLengthChangeComposite::IconId() const { return IDI_RESULT; }

QString IStressTensorGroup::CPrincipalLengthChangeComposite::ExportLabel(int /*nComponent*/) const {
  return ITensorGroup::CVectorComposite::ExportLabel(int(VC_LENGTH));
}

bool IStressTensorGroup::CPrincipalLengthChangeComposite::ValidName(const std::string &name,
                                                                    CResultInfo &resultInfo) const {
  // Get the type name of the current tensor
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent()->Parent());
  assert(pParent);

  // Get direction
  std::string sDirection;
  switch (m_direction) {
  case PD_MAX:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MAX);
    break;
  case PD_MED:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MED);
    break;
  case PD_MIN:
    sDirection = getStringTableEntry(IDS_ET_TENSOR_MIN);
    break;
  default:
    assert(false);
    break;
  };

  // Fetch names
  std::string sPreName, sName;
  if (pParent->PreExportNameId())
    sPreName = getStringTableEntry(pParent->PreExportNameId());

  sName = getStringTableEntry(pParent->ExportNameId());

  std::string sCombinedName = sPreName + std::string("P") + sName + sDirection;

  return validateName(resultInfo, name, sCombinedName, "L", "_L|_M|_C|_N");
}

bool IStressTensorGroup::CPrincipalLengthChangeComposite::OnBuildComponent(const CDepletionStage &stage,
                                                                           const CAnalysisType &antype,
                                                                           int nRegister) const {
  CDepletionStageEntry &dep_entry = (CDepletionStageEntry &)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
  return stage.Index() > dep_entry.MarkedAsInitialStage().Index() &&
         ITensorGroup::CVectorComposite::OnBuildComponent(stage, antype, nRegister);
}

void IStressTensorGroup::CPrincipalLengthChangeComposite::BuildComponent(const CDepletionStage &stage,
                                                                         const CAnalysisType &antype, int nRegister) {
  if (OnBuildComponent(stage, antype, nRegister))
    new CLengthChangeComponent(IDS_RC_LENGTH, *this, stage, antype, nRegister);
}

const IStressTensorGroup &IStressTensorGroup::CPrincipalLengthChangeComposite::AbsoluteStressTensorGroup() const {
  const CModelBase &model = static_cast<const CModelBase &>(Model());
  const IStressTensorGroup &changeGroup = ChangeStressTensorGroup();
  assert(&changeGroup == &model.ResultTree().TotalStressChange() ||
         &changeGroup == &model.ResultTree().EffectiveStressChange());
  const IStressTensorGroup *pGroup;
  if (&changeGroup == &model.ResultTree().TotalStressChange())
    pGroup = &model.ResultTree().TotalStress();
  else
    pGroup = &model.ResultTree().EffectiveStress();

  return *pGroup;
}

const IStressTensorGroup &IStressTensorGroup::CPrincipalLengthChangeComposite::ChangeStressTensorGroup() const {
  assert(Parent());
  assert(Parent()->Parent());
  assert(dynamic_cast<const IStressTensorGroup *>(Parent()->Parent()));
  return static_cast<const IStressTensorGroup &>(*Parent()->Parent());
}

bool IStressTensorGroup::CPrincipalLengthChangeComposite::OnDefined(const IResultComponent &component) const {
  if (!ITensorGroup::CVectorComposite::OnDefined(component))
    return false;

  if (dynamic_cast<const CLengthChangeComponent *>(&component)) {
    CDepletionStageEntry &dep_entry = (CDepletionStageEntry &)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    return (component.Stage().Index() > dep_entry.MarkedAsInitialStage().Index());
  }

  return true;
}

geo::IParallelInitializationCallback *
IStressTensorGroup::CPrincipalLengthChangeComposite::GetParallelInitializationCallback() {
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
    return pGroup->GetParallelInitializationCallback();
  return 0;
}

bool IStressTensorGroup::CPrincipalLengthChangeComposite::PrepareMapping(const geo::IElementSet *pElementSet,
                                                                         const IValueComponentBase *pValueComponent) {
  bool retval = true;
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (!pGroup || !pGroup->PrepareMapping(pElementSet, pValueComponent))
    retval = false;
  return retval;
}

void IStressTensorGroup::CPrincipalLengthChangeComposite::FinishMapping() {}

///// IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent

IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::CLengthChangeComponent(
    unsigned int /*uName*/, CPrincipalLengthChangeComposite &parent, const CDepletionStage &stage,
    const CAnalysisType &antype, int nRegister)
    : ITensorGroup::CVectorComposite::CLengthComponent(parent, stage, antype, nRegister, (VECTOR_COMPONENT)0) {}

IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::CLengthChangeComponent(
    const QString & /*sName*/, CPrincipalLengthChangeComposite &parent, const CDepletionStage &stage,
    const CAnalysisType &antype, int nRegister)
    : ITensorGroup::CVectorComposite::CLengthComponent(parent, stage, antype, nRegister, (VECTOR_COMPONENT)0) {}

geo::CValue IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::ValuePoint(
    const geo::IPoint &pt, const IValueComponentBase::UNIT unit, geo::IParallelInitializationCallback *cb) const {
  TSourceComponentPair prSources = SourceComponents();
  if (prSources.first == 0)
    return geo::CValue(); // invalid

  const IResultComponent &source_comp = *prSources.first;
  const IResultComponent &ini_source_comp = *prSources.second;

  const IStressTensorGroup &group = AbsoluteStressTensorGroup();

  const TPrincipal &principal = group.VectorPoint(pt, Composite().Direction(), source_comp, cb);
  const TPrincipal &initial_principal = group.VectorPoint(pt, Composite().Direction(), ini_source_comp, cb);

  double dValue = principal.second - initial_principal.second;
  if (unit == IQuantityDouble::FIELD_UNIT)
    dValue *= FF_FACTOR_STRESS;
  return geo::CValue(dValue);
}

void IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::MapValueElement(
    const geo::IElement &elm, IValueDomainScalar::TValueVec &values, IResultComponent::TMapType map_type,
    IValueComponentBase::UNIT unit, geo::IParallelInitializationCallback *cb) const {
  TSourceComponentPair prSources = SourceComponents();
  if (prSources.first == 0)
    return; // result invalid

  const IStressTensorGroup &group = AbsoluteStressTensorGroup();

  const IResultComponent &source_comp = *prSources.first;
  const IResultComponent &ini_source_comp = *prSources.second;

  int i;
  for (i = 0; i < values.size(); ++i) {
    const TPrincipal &principal = group.VectorElement(elm, Composite().Direction(), i, map_type, source_comp, cb);
    const TPrincipal &initial_principal =
        group.VectorElement(elm, Composite().Direction(), i, map_type, ini_source_comp, cb);

    double dValue = principal.second - initial_principal.second;
    if (unit == IQuantityDouble::FIELD_UNIT)
      dValue *= FF_FACTOR_STRESS;

    values[i] = geo::CValue(dValue);
  }
}

const IStressTensorGroup::CPrincipalLengthChangeComposite &
IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::Composite() const {
  return static_cast<const CPrincipalLengthChangeComposite &>(Parent());
}

const IStressTensorGroup &
IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::AbsoluteStressTensorGroup() const {
  return Composite().AbsoluteStressTensorGroup();
}

const IStressTensorGroup &
IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::ChangeStressTensorGroup() const {
  return Composite().ChangeStressTensorGroup();
}

IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::TSourceComponentPair
IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::SourceComponents() const {
  const IStressTensorGroup &group = AbsoluteStressTensorGroup();
  const CVectorComposite &source_composite = group.Direction(Composite().Direction());

  CDepletionStageEntry &dep_entry = (CDepletionStageEntry &)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
  const CDepletionStage &ini_stage = dep_entry.MarkedAsInitialStage();
  const CDepletionStage &stage = Stage();

  if (ini_stage.Index() >= stage.Index())
    return TSourceComponentPair((IResultComponent *)0, (IResultComponent *)0); // result invalid

  const IResultComponent &source_comp =
      *source_composite.ResultComponent(stage, AnalysisType(), RegisterIndex(), VC_LENGTH);
  const IResultComponent &ini_source_comp =
      *source_composite.ResultComponent(ini_stage, AnalysisType(), RegisterIndex(), VC_LENGTH);

  return TSourceComponentPair(&source_comp, &ini_source_comp);
}

bool IStressTensorGroup::CPrincipalLengthChangeComposite::CLengthChangeComponent::NeedParallelInitializationCallback()
    const {
  return true;
}

bool IStressTensorGroup::CInvariantComposite::CInvariantComponent::CanComputeOnPoints() const {
  switch (m_type) {
  case INV_I1:
  case INV_MEAN:
    return true;
  default:
    return false;
  }
}

//////////////////////////////////////////////////////////////////////
// Implementation if IStressTensorGroup::CInvariantComposite
//////////////////////////////////////////////////////////////////////
geo::CValue IStressTensorGroup::CInvariantComposite::CInvariantComponent::ValuePoint(
    const geo::IPoint &pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const {
  if (Gradient())
    return GradientValuePoint(pt, unit, cb);

  const IResult &result = (const IResult &)(Parent());
  const IStressTensorGroup *pParent = dynamic_cast<const IStressTensorGroup *>(result.Parent());
  assert(pParent);

  geo::CValue value = Value(pParent->TensorAtPoint(pt, *this, Stage(), AnalysisType(), false, cb), unit);

  if (pParent->Change()) {
    CDepletionStageEntry &dep_entry = (CDepletionStageEntry &)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    geo::CValue iniValue =
        Value(pParent->TensorAtPoint(pt, *this, dep_entry.MarkedAsInitialStage(), AnalysisType(), false), unit);

    value = value.Valid() && iniValue.Valid() ? value - iniValue : geo::CValue();
  }

  return value;
}

void IStressTensorGroup::CInvariantComposite::CInvariantComponent::MapValueElement(
    const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type, UNIT unit,
    geo::IParallelInitializationCallback *cb) const {
  assert(elm.NrOfPoints());
  const IResult &result = (const IResult &)(Parent());
  const IStressTensorGroup *pParent = dynamic_cast<const IStressTensorGroup *>(result.Parent());
  assert(pParent);

  const IStressTensorGroup::TTensorValueVec &vcTensor =
      pParent->TensorAtElement(elm, map_type, *this, Stage(), AnalysisType(), false, cb);
  for (int i = 0; i < values.size(); i++) {
    values[i] = Value(vcTensor[i], unit);
  }

  if (pParent->Change()) {
    CDepletionStageEntry &dep_entry = (CDepletionStageEntry &)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    const IStressTensorGroup::TTensorValueVec &vcIniTensor =
        pParent->TensorAtElement(elm, map_type, *this, dep_entry.MarkedAsInitialStage(), AnalysisType(), false, cb);

    for (int i = 0; i < values.size(); ++i) {
      geo::CValue valIni = Value(vcIniTensor[i], unit);
      values[i] = valIni.Valid() && values[i].Valid() ? values[i] - valIni : geo::CValue();
    }
  }
}

bool IStressTensorGroup::CInvariantComposite::CInvariantComponent::Gradient() const {
  return m_type == INV_VONMIS_GRADIENT || m_type == INV_MEAN_GRADIENT;
}

bool IStressTensorGroup::CInvariantComposite::CInvariantComponent::PrepareMapping(
    const geo::IElementSet * /*pElementSet*/) {
  bool retval = true;

  int c = ResultRegister().ColumnNumber(
      AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(),
                                CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  return retval;
}

void IStressTensorGroup::CInvariantComposite::CInvariantComponent::FinishMapping() {}

IStressTensorGroup::CInvariantComposite::CInvariantComposite(const QString &name, ITensorGroup &group)
    : IResult(name, group) {
  BuildComponents();
  Index(group.InvariantIndex()); // Index for loading and saving result refences
}

IStressTensorGroup::CInvariantComposite::CInvariantComposite(unsigned int uName, ITensorGroup &group)
    : IResult(uName, group) {
  BuildComponents();
  Index(group.InvariantIndex()); // Index for loading and saving result refences
}

bool IStressTensorGroup::CInvariantComposite::ValidName(const std::string &name, CResultInfo &resultInfo) const {
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent());

  std::string sPreName, sName, sPostName;
  if (pParent->PreExportNameId())
    sPreName = getStringTableEntry(pParent->PreExportNameId());
  sName = getStringTableEntry(pParent->ExportNameId());
  if (pParent->PostExportNameId())
    sPostName = getStringTableEntry(pParent->PostExportNameId());

  std::string sCombinedName = sPreName + sName + sPostName;

  return validateName(resultInfo, name, sCombinedName, StressInvariantResultComponentList(), "_L|_M|_C|_N", pParent);
}

void IStressTensorGroup::CInvariantComposite::BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype,
                                                             int nRegister) {
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent());
  assert(pParent);
  if (pParent->OnBuildComponent(stage, antype, nRegister)) {
    new CInvariantComponent(IDS_RC_INVPRESS, *this, INV_I1, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_INV2, *this, INV_I2, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_INV3, *this, INV_I3, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_VONMIS, *this, INV_VONMIS, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_VONMIS_GRADIENT, *this, INV_VONMIS_GRADIENT, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_TRESCA, *this, INV_TRESCA, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_MEANSTR, *this, INV_MEAN, stage, antype, nRegister);
    new CInvariantComponent(IDS_RC_MEANSTR_GRADIENT, *this, INV_MEAN_GRADIENT, stage, antype, nRegister);
  }
}

unsigned int IStressTensorGroup::CInvariantComposite::IconId() const { return IDI_RESULT_MAP; }

bool IStressTensorGroup::CInvariantComposite::OnDefined(const IResultComponent &component) const {
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent());
  assert(pParent);
  return pParent->OnDefined(component);
}

bool IStressTensorGroup::CInvariantComposite::CanMap(const COpenGLNode &node, int nRegister) const {
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent());
  assert(pParent);
  return pParent->CanMap(node, nRegister);
}

geo::IParallelInitializationCallback *IStressTensorGroup::CInvariantComposite::GetParallelInitializationCallback() {
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
    return pGroup->GetParallelInitializationCallback();
  return 0;
}

//////////////////////////////////////////////////////////////////////
// Implementation if IStressTensorGroup::CInvariantComposite::CInvariantComponent
//////////////////////////////////////////////////////////////////////
IStressTensorGroup::CInvariantComposite::CInvariantComponent::CInvariantComponent(
    unsigned int uInvariantName, CInvariantComposite &parent, enum INVARIANT_TYPE type, const CDepletionStage &stage,
    const CAnalysisType &antype, int nRegister)
    : IResultComponent(uInvariantName, parent, stage, antype, nRegister, (int)type), m_type(type) {}

unsigned int IStressTensorGroup::CInvariantComposite::CInvariantComponent::IconId() const { return IDI_RESULT; }

unsigned int IStressTensorGroup::CInvariantComposite::CInvariantComponent::TypeId() const { return 0; }

QString IStressTensorGroup::CInvariantComposite::CInvariantComponent::UnitName(const UNIT unit) const {
  const IResult &result = (const IResult &)(Parent());
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(result.Parent());
  assert(pParent);
  if (m_type == INV_I2)
    return pParent->UnitName(unit) + "^2";
  if (m_type == INV_I3)
    return pParent->UnitName(unit) + "^3";
  if (m_type == INV_VONMIS_GRADIENT || m_type == INV_MEAN_GRADIENT) {
    CStressGradientQuantity q;
    return q.UnitName(unit).c_str();
  }

  return pParent->UnitName(unit);
}

geo::CValue IStressTensorGroup::CInvariantComposite::CInvariantComponent::Value(const CStressTensor &tensor,
                                                                                CQuantity::UNIT unit) const {
  if (tensor.Empty())
    return geo::CValue();

  switch (m_type) {
  case INV_I1:
    if (unit == CQuantity::FIELD_UNIT)
      return geo::CValue(tensor.AverageStress() * FF_FACTOR_STRESS);
    return geo::CValue(tensor.AverageStress());
  case INV_I2: {
    const CMechanicalTensor *pDevTens = tensor.DeviatoricTensor();
    // return deviatoric stress invariant
    geo::CValue vRet(pDevTens->InvarI2());
    delete pDevTens;

    if (unit == CQuantity::FIELD_UNIT)
      return vRet * pow(FF_FACTOR_STRESS, 2);
    return vRet;
  }
  case INV_I3: {
    const CMechanicalTensor *pDevTens = tensor.DeviatoricTensor();
    // return deviatoric stress invariant
    geo::CValue vRet(pDevTens->InvarI3());
    delete pDevTens;

    if (unit == CQuantity::FIELD_UNIT)
      return vRet * pow(FF_FACTOR_STRESS, 3);
    return vRet;
  }
  case INV_VONMIS:
  case INV_VONMIS_GRADIENT:
    if (unit == CQuantity::FIELD_UNIT)
      return tensor.InvarVonMis() * FF_FACTOR_STRESS;
    return tensor.InvarVonMis();
  case INV_TRESCA:
    if (unit == CQuantity::FIELD_UNIT)
      return tensor.Tresca() * FF_FACTOR_STRESS;
    return tensor.Tresca();
  case INV_MEAN:
  case INV_MEAN_GRADIENT:
    if (unit == CQuantity::FIELD_UNIT)
      return tensor.MeanStress() * FF_FACTOR_STRESS;
    return tensor.MeanStress();
  default:
    assert(false);
  }

  return geo::CValue();
}

bool IStressTensorGroup::CInvariantComposite::CInvariantComponent::NeedParallelInitializationCallback() const {
  return true;
}

QString IStressTensorGroup::CInvariantComposite::ExportLabel(int nComponent) const {
  // Get the type name of the current tensor
  const ITensorGroup *pParent = dynamic_cast<const ITensorGroup *>(Parent());
  assert(pParent);

  QString sRet;
  switch (nComponent) {
  case INV_I1:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_PRESSURE);
    break;
  case INV_I2:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_2);
    break;
  case INV_I3:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_3);
    break;
  case INV_VONMIS:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES);
    break;
  case INV_TRESCA:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_TRESCA);
    break;
  case INV_MEAN:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_MEAN);
    break;
  case INV_VONMIS_GRADIENT:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT);
    break;
  case INV_MEAN_GRADIENT:
    sRet = getStringTableEntry(IDS_ET_TENSOR_INV_MEAN_GRADIENT);
    break;
  default:
    assert(false);
  }

  // Fetch names ...
  QString sPreName, sName, sPostName;
  if (pParent->PreExportNameId())
    sPreName = getStringTableEntry(pParent->PreExportNameId());
  sName = getStringTableEntry(pParent->ExportNameId());
  if (pParent->PostExportNameId())
    sPostName = getStringTableEntry(pParent->PostExportNameId());

  return sPreName + sName + sPostName + sRet;
}

///// IStressTensorGroup::CWellPathStressComposite
IStressTensorGroup::CWellPathStressComposite::CWellPathStressComposite(unsigned int uName, CResultGroup &group)
    : IResult(uName, group) {
  m_pTensorGroup = static_cast<IStressTensorGroup *>(&group);

  BuildComponents();

  // Set references for saving and loading ....
  Index(m_pTensorGroup->WellPathStressIndex());
}

IStressTensorGroup::CWellPathStressComposite::CWellPathStressComposite(const QString &sName, CResultGroup &group)
    : IResult(sName, group) {
  m_pTensorGroup = static_cast<IStressTensorGroup *>(&group);

  BuildComponents();

  // Set references for saving and loading ....
  Index(m_pTensorGroup->WellPathStressIndex());
}

unsigned int IStressTensorGroup::CWellPathStressComposite::IconId() const { return IDI_RESULT_MAP; }

bool IStressTensorGroup::CWellPathStressComposite::OnDefined(const IResultComponent &component) const {
  return m_pTensorGroup->OnDefined(component);
}

QString IStressTensorGroup::CWellPathStressComposite::ExportLabel(int nComponent) const {
  QString sCompLabel;
  switch (nComponent) {
  case CWellPathStressComponent::TC_AXIAL:
    sCompLabel = "Axial";
    break;
  case CWellPathStressComponent::TC_RADIAL_MAX:
    sCompLabel = "RadialMax";
    break;
  case CWellPathStressComponent::TC_RADIAL_MIN:
    sCompLabel = "RadialMin";
    break;
  default:
    assert(false);
  }

  // Fetch names
  QString sPreName, sName, sPostName;
  if (m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
  if (m_pTensorGroup->PostExportNameId())
    sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  return sPreName + "WP" + sName + sCompLabel + sPostName;
}

bool IStressTensorGroup::CWellPathStressComposite::CanMap(const COpenGLNode &node, int /*nRegister*/) const {
  return ((dynamic_cast<const CNewWellPath *>(&node) != 0 // waij TFS 74145
           || (dynamic_cast<const CWellCasingModel *>(&Model()) != 0)) &&
          dynamic_cast<const CWellCasingRTCI *>(&node) == 0);
}

bool IStressTensorGroup::CWellPathStressComposite::ValidName(const std::string &name, CResultInfo &resultInfo) const {
  std::string sPreName, sName;
  if (m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());

  std::string sCombinedName = sPreName + "WP" + sName;

  return validateName(resultInfo, name, sCombinedName, WellPathStressComponentList(), "_L|_M|_C|_N");
}

bool IStressTensorGroup::CWellPathStressComposite::OnBuildComponent(const CDepletionStage &stage,
                                                                    const CAnalysisType &antype, int nRegister) const {
  return m_pTensorGroup->OnBuildComponent(stage, antype, nRegister);
}

void IStressTensorGroup::CWellPathStressComposite::BuildComponent(const CDepletionStage &stage,
                                                                  const CAnalysisType &antype, int nRegister) {
  if (OnBuildComponent(stage, antype, nRegister)) {
    new CWellPathStressComponent(IDS_RC_AXIAL, *this, CWellPathStressComponent::TC_AXIAL, stage, antype, nRegister);
    new CWellPathStressComponent(IDS_RC_RADIAL_MIN, *this, CWellPathStressComponent::TC_RADIAL_MIN, stage, antype,
                                 nRegister);
    new CWellPathStressComponent(IDS_RC_RADIAL_MAX, *this, CWellPathStressComponent::TC_RADIAL_MAX, stage, antype,
                                 nRegister);
  }
}

geo::IParallelInitializationCallback *
IStressTensorGroup::CWellPathStressComposite::GetParallelInitializationCallback() {
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
    return pGroup->GetParallelInitializationCallback();
  return 0;
}

bool IStressTensorGroup::CWellPathStressComposite::PrepareMapping(const geo::IElementSet *pElementSet,
                                                                  const IValueComponentBase *pValueComponent) {
  bool retval = true;
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (!pGroup || !pGroup->PrepareMapping(pElementSet, pValueComponent))
    retval = false;
  return retval;
}

void IStressTensorGroup::CWellPathStressComposite::FinishMapping() {}

///// IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent
IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::CWellPathStressComponent(
    unsigned int uName, CWellPathStressComposite &parent, TComponent comp_type, const CDepletionStage &stage,
    const CAnalysisType &antype, int nRegister)
    : IResultComponent(uName, parent, stage, antype, nRegister, int(comp_type)), m_comp_type(comp_type) {}

unsigned int IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::IconId() const {
  return IDI_RESULT;
}

unsigned int IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::TypeId() const { return 0; }

QString IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::UnitName(const UNIT unit) const {
  if (unit == CDoubleQuantity::FIELD_UNIT)
    return QString("psi");
  return QString("MPa");
}

geo::CValue IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::ValuePoint(
    const geo::IPoint &pt, const UNIT unit, geo::IParallelInitializationCallback * /*cb*/) const {
  const IStressTensorGroup *pTensorGroup = dynamic_cast<const IStressTensorGroup *>(Parent().parent());
  assert(pTensorGroup);

  double fac = 1.0;
  if (unit == CDoubleQuantity::FIELD_UNIT)
    fac = FF_FACTOR_STRESS;

  geo::CValue val;

  switch (m_comp_type) {
  case TC_AXIAL:
    val = pTensorGroup->WellPathAxialValuePoint(pt, *this);
    break;
  case TC_RADIAL_MAX:
    val = pTensorGroup->WellPathRadialMaxValuePoint(pt, *this);
    break;
  case TC_RADIAL_MIN:
    val = pTensorGroup->WellPathRadialMinValuePoint(pt, *this);
    break;
  default:
    assert(false);
  }

  if (val.Valid())
    val *= fac;

  return val;
}

void IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::MapValueElement(
    const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type, UNIT unit,
    geo::IParallelInitializationCallback * /*cb*/) const {
  assert(elm.NrOfPoints() == values.size());
  const IStressTensorGroup *pTensorGroup = dynamic_cast<const IStressTensorGroup *>(Parent().parent());
  assert(pTensorGroup);

  switch (m_comp_type) {
  case TC_AXIAL:
    values = pTensorGroup->WellPathAxialValueElement(elm, map_type, *this);
    break;
  case TC_RADIAL_MAX:
    values = pTensorGroup->WellPathRadialMaxValueElement(elm, map_type, *this);
    break;
  case TC_RADIAL_MIN:
    values = pTensorGroup->WellPathRadialMinValueElement(elm, map_type, *this);
    break;
  default:
    assert(false);
  }

  double fac = 1.0;
  if (unit == CDoubleQuantity::FIELD_UNIT)
    fac = FF_FACTOR_STRESS;
  int i;
  for (i = 0; i < values.size(); ++i) {
    if (values[i].Valid())
      values[i] *= fac;
  }
}

bool IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent::NeedParallelInitializationCallback()
    const {
  return true;
}

///// CVerticalGradientComposite

IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComposite(unsigned int uName, CResultGroup &group)
    : IResult(uName, group) {
  m_pTensorGroup = static_cast<IStressTensorGroup *>(&group);

  BuildComponents();
}

unsigned int IStressTensorGroup::CVerticalGradientComposite::IconId() const { return IDI_RESULT; }

bool IStressTensorGroup::CVerticalGradientComposite::OnDefined(const IResultComponent &component) const {
  return m_pTensorGroup->OnDefined(component);
}

QString IStressTensorGroup::CVerticalGradientComposite::ExportLabel(int /*nComponent*/) const {
  // Fetch names
  QString sPreName, sName, sPostName;
  if (m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());
  if (m_pTensorGroup->PostExportNameId())
    sPostName = getStringTableEntry(m_pTensorGroup->PostExportNameId());

  return sPreName + sName + "Gradient" + sPostName;
}

bool IStressTensorGroup::CVerticalGradientComposite::ValidName(const std::string &name, CResultInfo &resultInfo) const {
  std::string sPreName, sName;
  if (m_pTensorGroup->PreExportNameId())
    sPreName = getStringTableEntry(m_pTensorGroup->PreExportNameId());
  sName = getStringTableEntry(m_pTensorGroup->ExportNameId());

  std::string sCombinedName = sPreName + sName + "Gradient";

  return validateName(resultInfo, name, sCombinedName, "", "_L|_M|_C|_N");
}

bool IStressTensorGroup::CVerticalGradientComposite::CanMap(const COpenGLNode &node, int nRegister) const {
  return m_pTensorGroup->CanMap(node, nRegister);
}

const IStressTensorGroup &IStressTensorGroup::CVerticalGradientComposite::TensorGroup() const {
  return *m_pTensorGroup;
}

bool IStressTensorGroup::CVerticalGradientComposite::OnBuildComponent(const CDepletionStage &stage,
                                                                      const CAnalysisType &antype,
                                                                      int nRegister) const {
  return m_pTensorGroup->OnBuildComponent(stage, antype, nRegister);
}

void IStressTensorGroup::CVerticalGradientComposite::BuildComponent(const CDepletionStage &stage,
                                                                    const CAnalysisType &antype, int nRegister) {
  if (OnBuildComponent(stage, antype, nRegister))
    new CVerticalGradientComponent(IDS_RC_VERTICALSTRESSGRADIENT, *this, stage, antype, nRegister);
}

geo::IParallelInitializationCallback *
IStressTensorGroup::CVerticalGradientComposite::GetParallelInitializationCallback() {
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (pGroup)
    return pGroup->GetParallelInitializationCallback();
  return 0;
}

bool IStressTensorGroup::CVerticalGradientComposite::PrepareMapping(const geo::IElementSet *pElementSet,
                                                                    const IValueComponentBase *pValueComponent) {
  bool retval = true;
  ITensorGroup *pGroup = dynamic_cast<ITensorGroup *>(Parent());
  if (!pGroup || !pGroup->PrepareMapping(pElementSet, pValueComponent))
    retval = false;
  return retval;
}

void IStressTensorGroup::CVerticalGradientComposite::FinishMapping() {}

///// CVerticalGradientComponent

IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::CVerticalGradientComponent(
    unsigned int uName, CVerticalGradientComposite &parent, const CDepletionStage &stage, const CAnalysisType &antype,
    int nRegister)
    : IResultComponent(uName, parent, stage, antype, nRegister, 0) {}

unsigned int IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::IconId() const {
  return IDI_RESULT;
}

unsigned int IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::TypeId() const { return 0; }

QString IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::UnitName(const UNIT unit) const {
  CStressGradientQuantity q;
  return q.UnitName(unit).c_str();
}

IValueDomainScalar::TValue IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::ValuePoint(
    const geo::IPoint &pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const {
  return GradientValuePoint(pt, unit, cb);
}

void IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::MapValueElement(
    const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type, UNIT unit,
    geo::IParallelInitializationCallback *cb) const {
  return VerticalComponent().MapValueElement(elm, values, map_type, unit, cb);
}

IValueDomainScalar::TValue IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::Value(
    const geo::IElement & /*mesh_element*/, int /*nNodeIndex*/, UNIT /*unit*/) const {
  assert(false);
  return IValueDomainScalar::TValue();
}

const IResultComponent &
IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::VerticalComponent() const {
  const CVerticalGradientComposite &parent = static_cast<const CVerticalGradientComposite &>(Parent());
  const IStressTensorGroup &tensorgroup = parent.TensorGroup();
  return *tensorgroup.Components().ResultComponent(Stage(), AnalysisType(), RegisterIndex(),
                                                   ITensorGroup::CComponentComposite::TC_ZZ);
}

bool IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::NeedParallelInitializationCallback()
    const {
  return true;
}

bool IStressTensorGroup::CVerticalGradientComposite::CVerticalGradientComponent::PrepareMapping(
    const geo::IElementSet *pElementSet) {
  bool retval = true;

  const CVerticalGradientComposite &parent = static_cast<const CVerticalGradientComposite &>(Parent());
  const IStressTensorGroup &tensorgroup = parent.TensorGroup();

  if (!const_cast<IStressTensorGroup &>(tensorgroup).PrepareMapping(pElementSet, this))
    return false;

  int c = ResultRegister().ColumnNumber(
      AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), ResultRegister().DepletionStageEntry().MarkedAsInitialStage().Index(),
                                CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  c = ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_STRESS_XX);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  ResultRegister().ColumnNumber(AnalysisType(), Stage().Index(), CI_POREPRES);
  if (!ResultRegister().Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  return retval;
}
