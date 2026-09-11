
#include "CalculatedTemperature.h"
#include "AnalysisType.h"
#include "DepletionStage.h"
#include "FormationBase.h"
#include "IWellModel.h"
#include "InterfaceElement.h"
#include "Result.h"
#include "ResultTree.h"
#include "ValueTypes.h"

CCalculatedTemperature::CCalculatedTemperature(CFormationBase &formation, CDepletionStage &stage)
    : IValueComposite(stage.Name().toStdString().c_str(), formation.Model()), m_formation(formation), m_stage(stage) {
  LinkTo(formation);
  LinkTo(stage);

  IWellModel *pWellModel = dynamic_cast<IWellModel *>(&formation.Model());
  if (pWellModel)
    new CCalculatedTemperatureComponent(*this, pWellModel->RootModel());
  else
    new CCalculatedTemperatureComponent(*this);
}

const CDepletionStage &CCalculatedTemperature::DepletionStage() const { return m_stage; }

CDepletionStage &CCalculatedTemperature::DepletionStage() { return m_stage; }

const CFormationBase *CCalculatedTemperature::Formation() const { return &m_formation; }

CFormationBase *CCalculatedTemperature::Formation() { return &m_formation; }

unsigned int CCalculatedTemperature::IconId() const { return IDI_CALC_TEMPER; }

unsigned int CCalculatedTemperature::TypeId() const { return IDT_COMPONENT_CALC_TEMPER; }

bool CCalculatedTemperature::Less(const CGraphNode &rhs) const {
  const CCalculatedTemperature *pRhs = dynamic_cast<const CCalculatedTemperature *>(&rhs);
  if (pRhs)
    return m_stage.isBefore(pRhs->m_stage);

  return IValueComposite::Less(rhs);
}

long CCalculatedTemperature::SavedItems() const { return 0; }

void CCalculatedTemperature::LoadStream(TSTREAM & /*stream*/, CStreamVersion & /*version*/, TPROGRESS & /*progress*/) {}

void CCalculatedTemperature::SaveStream(TSTREAM & /*stream*/, TPROGRESS & /*progress*/) {}

void CCalculatedTemperature::OnNeighbourDeleted(const CGraphNode &node) {
  bool bDelete = false;

  if (&node == &m_formation)
    bDelete = true;

  if (&node == &m_stage)
    bDelete = true;

  IValueComposite::OnNeighbourDeleted(node);

  if (bDelete)
    delete this;
}

///// CCalculatedTemperatureComponent

CCalculatedTemperature::CCalculatedTemperatureComponent::CCalculatedTemperatureComponent(CCalculatedTemperature &parent)
    : IValueComponentBase(parent), m_parent(parent), m_ResultSource(static_cast<CModelBase &>(parent.Model())),
      m_Data(*this) {
  LinkTo(m_ResultSource.ResultRegister());
}

CCalculatedTemperature::CCalculatedTemperatureComponent::CCalculatedTemperatureComponent(CCalculatedTemperature &parent,
                                                                                         CModelBase &ResultSource)
    : IValueComponentBase(parent), m_parent(parent), m_ResultSource(ResultSource), m_Data(*this) {
  LinkTo(m_ResultSource.ResultRegister());
}

IValueComponentBase::TValueComponentType CCalculatedTemperature::CCalculatedTemperatureComponent::Type() const {
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar &CCalculatedTemperature::CCalculatedTemperatureComponent::ScalarData() const {
  return m_Data;
}

IValueDataInterfaceScalar &CCalculatedTemperature::CCalculatedTemperatureComponent::ScalarData() { return m_Data; }

QString CCalculatedTemperature::CCalculatedTemperatureComponent::ExportLabel() const { return QString(); }

QString CCalculatedTemperature::CCalculatedTemperatureComponent::UnitName(const UNIT /*unit*/) const {
  return getStringTableEntry(IDS_UNIT_TEMPERATURE);
}

bool CCalculatedTemperature::CCalculatedTemperatureComponent::Defined() const {
  return m_ResultSource.ResultRegister().ResultsAvailable(CAnalysisType::AT_HEAT, m_parent.DepletionStage());
}

IValueDomainScalar::TMinMax CCalculatedTemperature::CCalculatedTemperatureComponent::MinMax(IProgressBase &progressBase,
                                                                                            const UNIT unit) const {
  assert(Defined());
  return ResultComponent().MinMax(progressBase, unit);
}

IValueDomainScalar::TValue CCalculatedTemperature::CCalculatedTemperatureComponent::Average(IProgressBase &progressBase,
                                                                                            const UNIT unit) const {
  assert(Defined());
  return ResultComponent().Average(progressBase, unit);
}

geo::CValue
CCalculatedTemperature::CCalculatedTemperatureComponent::ValuePoint(const geo::IPoint &pt, const UNIT unit,
                                                                    geo::IParallelInitializationCallback *cb) const {
  assert(Defined());
  return ResultComponent().ValuePoint(pt, unit, cb);
}

IValueDomainScalar::TValueVec
CCalculatedTemperature::CCalculatedTemperatureComponent::ValueElement(const geo::IElement &elm, const UNIT unit,
                                                                      geo::IParallelInitializationCallback *cb) const {
  assert(Defined());
  return ResultComponent().ValueElement(elm, unit, cb);
}

unsigned int CCalculatedTemperature::CCalculatedTemperatureComponent::IconId() const { return m_parent.IconId(); }

unsigned int CCalculatedTemperature::CCalculatedTemperatureComponent::TypeId() const { return m_parent.TypeId(); }

bool CCalculatedTemperature::CCalculatedTemperatureComponent::Empty() const { return false; }

long CCalculatedTemperature::CCalculatedTemperatureComponent::SavedItems() const { return 0; }

void CCalculatedTemperature::CCalculatedTemperatureComponent::OnNeighbourModified(const CGraphNode &node,
                                                                                  enum ModifiedHint uHint) {
  if (dynamic_cast<const CResultRegister *>(&node))
    Modified(uHint);
}

const IResultComponent &CCalculatedTemperature::CCalculatedTemperatureComponent::ResultComponent() const {
  const IResultComponent *pComp =
      m_ResultSource.ResultTree().Temperature().ResultComponent(m_parent.DepletionStage(), CAnalysisType::AT_HEAT);
  assert(pComp);
  return *pComp;
}
