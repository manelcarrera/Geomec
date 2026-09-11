#include "GVTResult.h"

#include "ModelBase.h"
#include "ValueMapper.h"
#include "ivaluecomponent.h"
#include "DerivedResult.h"
#include "GVTSettings.h"


CGVTFactor::CGVTFactorComponent::CGVTFactorComponent(const QString& strName, CGVTFactor& parent, const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
  : IResultComponent(strName, parent, stage, antype, nRegister, 0)
{
}

CGVTFactor::CGVTFactorComponent::~CGVTFactorComponent()
{
}

unsigned int CGVTFactor::CGVTFactorComponent::IconId() const
{
  return Parent().IconId();
}

unsigned int CGVTFactor::CGVTFactorComponent::TypeId() const
{
  return Parent().TypeId();
}

QString CGVTFactor::CGVTFactorComponent::UnitName(const UNIT unit) const
{
  return "-";
}

long CGVTFactor::CGVTFactorComponent::SavedItems() const
{
  return 0;
}


geo::CValue CGVTFactor::CGVTFactorComponent::getElementFactor(const geo::IElement& elm) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CMeshBase& mesh = model.Mesh();
  const CFormationBase *f = mesh.Formation(elm);
  const CGVTSettings& settings = model.GVTSettings();
  std::map<CFormationBase*, CGVTVelocityModel*> &mp = const_cast<CGVTSettings&>(settings).FormationToVelModelMp();

  std::map<CFormationBase*, CGVTVelocityModel*>::iterator it = mp.find(const_cast<CFormationBase *>(f));
  if (it != mp.end())
  {
    CGVTVelocityModel *vm = it->second;

    if (vm->ModelName() == "R")
    {
      switch (Parent().TypeId())
      {
      case IDT_VALUETYPE_GVT_FACTOR_R_EX:
        return vm->Parameter(RFactor_EX);
      case IDT_VALUETYPE_GVT_FACTOR_R_C:
        return vm->Parameter(RFactor_C);
      }
    }
    else
    {
      assert(vm->ModelName() == "S");
      switch (Parent().TypeId())
      {
      case IDT_VALUETYPE_GVT_FACTOR_S_S:
        return vm->Parameter(SFactor_S);
      case IDT_VALUETYPE_GVT_FACTOR_S_NB:
        return vm->Parameter(SFactor_NB);
      }
    }
  }

  return geo::CValue();
}


IValueDomainScalar::TValue CGVTFactor::CGVTFactorComponent::ValuePoint(const geo::IPoint& pt, const UNIT /* unit */, geo::IParallelInitializationCallback * /* cb */) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const geo::IMesh& mesh = model.Mesh().Mesh();
  const std::set<int>& stCandidates = mesh.Candidates(pt);

   for (std::set<int>::const_iterator it = stCandidates.begin(); it != stCandidates.end(); ++it)
  {
    const geo::IElement& elm = mesh.Element(*it);
    if (elm.Contains(pt, true))
    {
      return getElementFactor(elm);
    }
  }

  return geo::CValue();
}

IValueDomainScalar::TValueVec CGVTFactor::CGVTFactorComponent::ValueElement(const geo::IElement& elm, const UNIT /* unit */, geo::IParallelInitializationCallback * /* cb */) const
{
  geo::CValue val = getElementFactor(elm);
  IValueDomainScalar::TValueVec vcValues(elm.NrOfNodes(), val);
  return vcValues;
}

void CGVTFactor::CGVTFactorComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType /*map_type*/, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  values = ValueElement(elm, unit, cb);
}

QString CGVTFactor::CGVTFactorComponent::ExportLabel() const
{
  switch (Parent().TypeId())
  {
  case IDT_VALUETYPE_GVT_FACTOR_R_EX:
    return getStringTableEntry(IDS_ET_GVT_FACTOR_R_EX);
  case IDT_VALUETYPE_GVT_FACTOR_R_C:
    return getStringTableEntry(IDS_ET_GVT_FACTOR_R_C);
  case IDT_VALUETYPE_GVT_FACTOR_S_NB:
    return getStringTableEntry(IDS_ET_GVT_FACTOR_S_NB);
  case IDT_VALUETYPE_GVT_FACTOR_S_S:
    return getStringTableEntry(IDS_ET_GVT_FACTOR_S_S);
  }
  return Parent().ExportLabel();
}


bool CGVTFactor::CGVTFactorComponent::PrepareMapping(const geo::IElementSet *pElementSet)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  CMeshBase& mesh = model.Mesh();

  bool retval = true;

  CValueMapper *vm = CValueMapper::instance();

  if (!vm->PrepareMappingElementSet(pElementSet, &mesh.Mesh()))
    retval = false;

  if (!vm->PrepareMappingSurfaces(model, pElementSet, Stage()))
    retval = false;

  if (!vm->PrepareMappingMaterial(model, pElementSet, Stage()))
    retval = false;

  return retval;
}






CGVTFactor::CGVTFactor(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group)
  : IResult(uName, group),
  m_uiValueTypeID(uiValueTypeID),
  m_uiIconID(uiIconID)
{
  BuildComponents();
}

CGVTFactor::~CGVTFactor()
{
}

bool CGVTFactor::Empty() const
{
  return false;
}

long CGVTFactor::SavedItems() const
{
  return 0;
}

unsigned int CGVTFactor::IconId() const
{
  return IDI_RESULT;
}

unsigned int CGVTFactor::TypeId() const
{
  return m_uiValueTypeID;
}

bool CGVTFactor::Less(const CGraphNode &node) const
{
  return IResult::Less(node);
}

bool CGVTFactor::OnDefined(const IResultComponent& /*component*/) const
{
  return true;
}

bool CGVTFactor::CanMap(const COpenGLNode& /*node*/, int /*nRegister*/) const
{
  return true;
}

QString CGVTFactor::ExportLabel(int /*nComponent*/) const
{
  if (m_vcComponents.size() == 1)
    return m_vcComponents[0]->ExportLabel();
  assert(FALSE);
  return QString();
}

void CGVTFactor::OnNeighbourDeleted(const CGraphNode& node)
{
  const CGVTFactorComponent *c = reinterpret_cast<const CGVTFactorComponent *>(&node);
  TComponentVector::iterator it = std::find(m_vcComponents.begin(), m_vcComponents.end(), c);
  if (it != m_vcComponents.end())
    m_vcComponents.erase(it);
  IResult::OnNeighbourDeleted(node);
}

const QString& CGVTFactor::StoredExportLabel() const
{
  return m_strExportLabel;
}

QString CGVTFactor::UnitName(const UNIT unit) const
{
  return "-";
}

double CGVTFactor::UnitFactor(const UNIT unit) const
{
  return 1;
}

void CGVTFactor::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
  if (m_vcComponents.size() == 0)
    m_vcComponents.push_back(new CGVTFactorComponent(Name(), *this, stage, antype, nRegister));
  else
    RegisterMode(stage, antype, nRegister);
}

IResultComponent* CGVTFactor::OnGetResultComponent(int nTimeStep, const CAnalysisType& /*antype*/, int /*nRegister*/, unsigned int /*nComponent*/) const
{
  if (m_vcComponents.size() > 0)
    return m_vcComponents[0];

  return 0;
}

bool CGVTFactor::ValidName
(const std::string &/*name*/
, CResultInfo &/*resultInfo*/
) const
{
  return true;
}

IValueComponentBase& CGVTFactor::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/)
{
  return **m_vcComponents.begin();
}

const IValueComponentBase& CGVTFactor::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/) const
{
  return **m_vcComponents.begin();
}





CGVTResultGroup::CGVTResultGroup(CResultGroup& group)
  : CResultGroup("Output", group)
{
}

unsigned int CGVTResultGroup::IconId() const
{
  return IDI_RESULT_MAP;
}

bool CGVTResultGroup::CanConnectItem(const CGraphNode& item) const
{
  if (dynamic_cast<const TDeltaV *>(&item) || dynamic_cast<const TDeltaT *>(&item))
  {
    for (int i = 0; i < ChildSize(); ++i)
    {
      if (&Child(i) == &item)
        return false;
    }
    return true;
  }
  return false;
}

bool CGVTResultGroup::ConnectItem(const CGraphNode& item)
{
  assert(CanConnectItem(item));

  if (CResultGroup::ConnectItem(item))
  {
    PushBack(const_cast<CGraphNode&>(item));
    Modified();
  }

  return true;
}

long CGVTResultGroup::SavedItems() const
{
  return 0;
}

void CGVTResultGroup::LoadStream(CStorageNode::TSTREAM& stream, CGVTResultVersion& /* version */, CStorageNode::TPROGRESS& prog)
{
  int nComponents;
  stream >> nComponents;

  TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*(static_cast<CModelBase&>(Model())).RootModel().GraphEntry(MD_BASE_VALUE_COMPOSITE);

  for (int i = 0; i < nComponents; ++i)
  {
    int nIdx;
    stream >> nIdx;
    IValueComposite *pComposite = composite_entry.FindIndex(nIdx);
    if (pComposite)
      ConnectItem(*pComposite);
  }
}

void CGVTResultGroup::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prog)
{
  std::set<IValueComposite *> stNodes = Links<IValueComposite>();

  stream << (int)stNodes.size();

  for (std::set<IValueComposite *>::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    stream << (*it)->Index();
  }
}



CGVTResultTree::CGVTResultTree(CModelBase& model)
  : CResultGroup("GVT Results", model)
{
  CResultGroup *pFactors = new CResultGroup("Factors", *this);
  new CGVTFactor(IDS_VALUENAME_GVT_FACTOR_R_EX, IDT_VALUETYPE_GVT_FACTOR_R_EX, IDI_VALUETYPE_GVT_FACTOR_R_EX, *pFactors);
  new CGVTFactor(IDS_VALUENAME_GVT_FACTOR_R_C, IDT_VALUETYPE_GVT_FACTOR_R_C, IDI_VALUETYPE_GVT_FACTOR_R_C, *pFactors);
  new CGVTFactor(IDS_VALUENAME_GVT_FACTOR_S_NB, IDT_VALUETYPE_GVT_FACTOR_S_NB, IDI_VALUETYPE_GVT_FACTOR_S_NB, *pFactors);
  new CGVTFactor(IDS_VALUENAME_GVT_FACTOR_S_S, IDT_VALUETYPE_GVT_FACTOR_S_S, IDI_VALUETYPE_GVT_FACTOR_S_S, *pFactors);

  m_pOutput = new CGVTResultGroup(*this);

  m_pDerived = new CDerivedResultGroup(IDS_RG_FILE_RESULT, *this, true);
}

unsigned int CGVTResultTree::IconId() const
{
  return IDI_RESULT_MAP;
}

long CGVTResultTree::SavedItems() const
{
  return m_pOutput->SavedItems() + m_pDerived->SavedItems();
}

void CGVTResultTree::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prog)
{
  int nVersion, nMajor, nMinor;
  stream >> nVersion;
  stream >> nMajor;
  stream >> nMinor;

  CGVTResultVersion gvtResultVersion(nVersion, nMajor, nMinor);

  m_pOutput->LoadStream(stream, gvtResultVersion, prog);
  m_pDerived->LoadStream(stream, version, prog);
}

void CGVTResultTree::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prog)
{
  int nVersion = 0, nMajor = 0, nMinor = 0;
  stream << nVersion;
  stream << nMajor;
  stream << nMinor;

  m_pOutput->SaveStream(stream, prog);
  m_pDerived->SaveStream(stream, prog);
}
