// HorizonBase.cpp: implementation of the CHorizonBase class.
//
//////////////////////////////////////////////////////////////////////

#include "HorizonBase.h"
#include "BaseEntryTypes.h"
#include "DepletionStage.h"
#include "FaultParametersNode.h"
#include "FaultPressure.h"
#include "FemAppEntryTypes.h"
#include "FemAppModel.h"
#include "GeoSurface.h"
#include "GeomecStringTable.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "StreamVersion.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Default constructor...
CHorizonBase::CHorizonBase(CFemAppModel &model) : CColorNode(model), m_bSlip(false), m_SlipType(SLIP) {
  CreatePressures();
}

// ##ModelId=3BC55D620087
CHorizonBase::CHorizonBase(const QString &strInstanceName, CFemAppModel &model, bool bAttachToEntry)
    : CColorNode(strInstanceName, model), m_bSlip(false), m_SlipType(SLIP) {
  CreatePressures();

  if (bAttachToEntry)
    LinkToEntry();
}

void CHorizonBase::LinkToEntry() {
  // Link to entry .. The upper class links to boundary
  assert(Model().GraphEntry(MD_BASE_HORIZON));
  LinkTo(*Model().GraphEntry(MD_BASE_HORIZON));
  CModelBase &modelBase = dynamic_cast<CModelBase &>(Model());
  modelBase.InvalidateMesh();
}

CHorizonBase::CHorizonBase(const CHorizonBase &rhs)
    : CColorNode(rhs), m_bSlip(rhs.m_bSlip), m_SlipType(rhs.m_SlipType) {
  if (rhs.Slip()) {
    TPressureMap::const_iterator it;
    for (it = rhs.m_mpPressure.begin(); it != rhs.m_mpPressure.end(); ++it)
      m_mpPressure.insert(TPressureMap::value_type(it->first, new CFaultPressure(*it->second)));

    TFaultParametersMap::const_iterator itf;
    for (itf = rhs.m_mpFaultParameters.begin(); itf != rhs.m_mpFaultParameters.end(); ++itf)
      m_mpFaultParameters.insert(TFaultParametersMap::value_type(itf->first, new CFaultParametersNode(*itf->second)));
  }
}

// ##ModelId=3BC55D6200D4
CHorizonBase::~CHorizonBase() { ClearMaterials(); }

void CHorizonBase::CreatePressures() {
  // create fault pressure objects
  CModelBase &model = static_cast<CModelBase &>(Model());
  CDepletionStage *pStage = &model.InitialDepletionStage();
  while (pStage) {
    CFaultPressure *pFaultPressure = new CFaultPressure(*this, *pStage);
    assert(IsLinkedTo(*pFaultPressure));
    assert(m_mpPressure.find(&pFaultPressure->DepletionStage()) != m_mpPressure.end());
    assert(m_mpPressure.find(&pFaultPressure->DepletionStage())->second == pFaultPressure);

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
}

void CHorizonBase::UpdateFaultParameters() {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CDepletionStage *pStage = &model.InitialDepletionStage();
  while (pStage) {
    if (canHaveFaultParameters(*pStage)) {
      if (!hasFaultParameters(*pStage)) {
        CFaultParametersNode *pNode = new CFaultParametersNode(*this, *pStage);
        assert(IsLinkedTo(*pNode));
        assert(m_mpFaultParameters.find(pStage) != m_mpFaultParameters.end());
        assert(m_mpFaultParameters.find(pStage)->second == pNode);
      }
    } else {
      TFaultParametersMap::iterator it = m_mpFaultParameters.find(pStage);
      if (it != m_mpFaultParameters.end())
        delete it->second;
      assert(m_mpFaultParameters.find(pStage) == m_mpFaultParameters.end());
    }

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

bool CHorizonBase::canHaveFaultParameters(const CDepletionStage &stage) const {
  return Slip() && (stage.Initial() || stage.Previous().OutputType() == CDepletionStage::BRANCH ||
                    stage.Previous().OutputType() == CDepletionStage::PHASE);
}

bool CHorizonBase::hasFaultParameters(const CDepletionStage &stage) const {
  return (m_mpFaultParameters.find(&stage) != m_mpFaultParameters.end());
}

// ##ModelId=3BC55D6200D6
unsigned int CHorizonBase::IconId() const {
  if (Slip())
    return IDI_SLIP_HORIZON;

  return IDI_HORIZON;
}

void CHorizonBase::OnNeighbourModified(const CGraphNode &item, enum ModifiedHint /*uHint*/) {
  bool bModified = false;
  TDepletionStageEntry *pEntry = (TDepletionStageEntry *)(Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  assert(IsLinkedTo(*pEntry));
  if (pEntry == &item) {
    // Get first depletion stage
    CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
    assert(pModel);
    CDepletionStage *pStage = &pModel->InitialDepletionStage();
    while (pStage) {
      if (m_mpPressure.find(pStage) == m_mpPressure.end()) {
        // Create new one ...
        CFaultPressure *pPressure = new CFaultPressure(*this, *pStage);
        assert(pPressure == m_mpPressure.find(pStage)->second);
        bModified = true;
      }

      if (canHaveFaultParameters(*pStage) && m_mpFaultParameters.find(pStage) == m_mpFaultParameters.end()) {
        CFaultParametersNode *pNode = new CFaultParametersNode(*this, *pStage);
        assert(m_mpFaultParameters.find(pStage)->second == pNode);
        bModified = true;
      }

      if (pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
    }
  }

  if (bModified)
    Modified();
}

void CHorizonBase::OnNeighbourDeleted(const CGraphNode &item) {
  TPressureMap::iterator it;
  for (it = m_mpPressure.begin(); it != m_mpPressure.end(); ++it) {
    if (it->second == &item) {
      m_mpPressure.erase(it);
      break;
    }
  }

  TFaultParametersMap::iterator itf;
  for (itf = m_mpFaultParameters.begin(); itf != m_mpFaultParameters.end(); ++itf) {
    if (itf->second == &item) {
      m_mpFaultParameters.erase(itf);
      break;
    }
  }

  CColorNode::OnNeighbourDeleted(item);
}

// ##ModelId=3BC55D6200C3
unsigned int CHorizonBase::TypeId() const { return IDT_TREE_HORIZONS; }

QString CHorizonBase::TypeName() const { return getStringTableEntry(IDS_TREE_HORIZONS); }

bool CHorizonBase::Destroy() {
  // Invalidate the mesh ...
  CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
  assert(pModel);
  pModel->InvalidateMesh();

  delete this;

  return true;
}

bool CHorizonBase::operator==(const CHorizonBase &rhs) const {
  if (!CColorNode::operator==(rhs))
    return false;

  if (Slip() != rhs.Slip())
    return false;

  if (Slip() && m_SlipType != rhs.m_SlipType)
    return false;

  return true;
}

CHorizonBase::TSlipType CHorizonBase::SlipType() const { return m_SlipType; }

void CHorizonBase::SlipType(TSlipType slip_type) { m_SlipType = slip_type; }

CHorizonBase &CHorizonBase::operator=(const CHorizonBase &rhs) {
  AssertValid();
  rhs.AssertValid();

  CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
  assert(pModel);

  bool bInvalidateMesh(Slip() != rhs.Slip());

  CColorNode::operator=(rhs);

  Slip(rhs.Slip());

  if (Slip()) {
    if (m_SlipType != rhs.m_SlipType) {
      pModel->ResultRegister().ClearLinear(false);
      pModel->ResultRegister().ClearNonLinear(false);
      pModel->ResultRegister().ClearMixture();
    }

    m_SlipType = rhs.m_SlipType;
  }

  AssertValid();
  rhs.AssertValid();

  if (bInvalidateMesh) {
    pModel->InvalidateMesh();
  }

  return *this;
}

bool CHorizonBase::Less(const CGraphNode &node) const {
  const CHorizonBase *pHorizon = dynamic_cast<const CHorizonBase *>(&node);
  if (pHorizon) {
    if (!Min().Empty() && !pHorizon->Min().Empty()) {
      if (fabs(Min().Z() - pHorizon->Min().Z()) > EPS)
        return Min().Z() < pHorizon->Min().Z();
    }
  }

  return CColorNode::Less(node);
}

void CHorizonBase::OnNewNeighbour(const CGraphNode &node) {
  const CFaultPressure *pFaultPressure = dynamic_cast<const CFaultPressure *>(&node);
  if (pFaultPressure) {
    CFaultPressure *p = const_cast<CFaultPressure *>(pFaultPressure);
    assert(!p->IsCopy());
    VERIFY(m_mpPressure.insert(TPressureMap::value_type(&p->DepletionStage(), p)).second);
    Modified();
  }

  const CFaultParametersNode *pFaultParamNode = dynamic_cast<const CFaultParametersNode *>(&node);
  if (pFaultParamNode) {
    CFaultParametersNode *p = const_cast<CFaultParametersNode *>(pFaultParamNode);
    VERIFY(m_mpFaultParameters.insert(TFaultParametersMap::value_type(&p->Stage(), p)).second);
    Modified();
  }

  CColorNode::OnNewNeighbour(node);
}

bool CHorizonBase::Slip() const { return m_bSlip; }

void CHorizonBase::Slip(bool bSlip) {
  if (bSlip != m_bSlip) {
    m_bSlip = bSlip;

    if (!IsCopy()) {
      OnSlipToggled();
      if (!IsCopied())
        UpdateFaultParameters();
    }

    if (!IsCopied())
      Modified();
  }
}

void CHorizonBase::OnSlipToggled() {
  CModelBase &model = static_cast<CModelBase &>(Model());
  if (model.Mesh().IsMesh() && !model.Mesh().ImportedMesh())
    model.Mesh().InvalidateMesh();
}

bool CHorizonBase::DoubleSidedFault() const { return false; }

bool CHorizonBase::IgnoreFault() const { return false; }

const CFaultPressure &CHorizonBase::Pressure(const CDepletionStage &stage) const {
  assert(Slip());
  TPressureMap::const_iterator it = m_mpPressure.find(&stage);
  assert(it != m_mpPressure.end());

  return *it->second;
}

CFaultPressure &CHorizonBase::Pressure(const CDepletionStage &stage) {
  assert(Slip());
  TPressureMap::const_iterator it = m_mpPressure.find(&stage);
  assert(it != m_mpPressure.end());

  return *it->second;
}

#ifdef _DEBUG
void CHorizonBase::AssertValid() const {

  for (size_t i = 0; i < referenceSize(); i++) {
    const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase *>(&referenceAt(i));
    if (pSurface) {
      if (IsCopy()) {
        if (pSurface->IsCopied())
          assert(IsLinkedTo(pSurface->Copy()));
      } else {
        assert(IsLinkedTo(*pSurface));
      }
    }
  }

  CColorNode::AssertValid();
}
#endif //_DEBUG

void CHorizonBase::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Load base first (Loads name, color and index)
  CColorNode::LoadStream(stream, version, progress);

  const CModelBase &model = static_cast<const CModelBase &>(Model());

  // Set l'indicateur du progress
  std::string pr("Loading Horizon ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  if (version < CStreamVersion(3, 0, 118)) {
    // Slip
    int bSlip;
    stream >> bSlip;
    Slip(bSlip != false);

    double dCohesion;
    double dFrictionAngle;
    double dStickCohesion;
    double dStickFrictionAngle;

    if (bSlip) {
      stream >> dCohesion;
      stream >> dFrictionAngle;

      FaultParameters(model.InitialDepletionStage()).Cohesion(dCohesion);
      FaultParameters(model.InitialDepletionStage()).FrictionAngle(dFrictionAngle);

      if (CStreamVersion(3, 0, 87) < version) {
        // load fault pressures
        const CDepletionStage *pStage = &model.InitialDepletionStage();
        while (pStage) {
          Pressure(*pStage).LoadStream(stream, version, progress);

          if (pStage->Last())
            pStage = 0;
          else
            pStage = &pStage->Next();
        }
      }
    }

    // Loads the slip settings
    if (version > CStreamVersion(3, 0, 75)) {
      int nSlipType;
      stream >> nSlipType;
      if (nSlipType == 0)
        SlipType(STICK);
      else if (nSlipType == 1)
        SlipType(SLIP);
      else if (nSlipType == 2)
        SlipType(USER);
      else {
        assert(nSlipType == 3);
        SlipType(FRACTURE);
      }

      stream >> dStickCohesion;
      stream >> dStickFrictionAngle;

      if (SlipType() == STICK) {
        if (bSlip) {
          FaultParameters(model.InitialDepletionStage()).Cohesion(dStickCohesion);
          FaultParameters(model.InitialDepletionStage()).FrictionAngle(dStickFrictionAngle);
        }
      }
    }

    int nDistributedSize;
    stream >> nDistributedSize;
    for (int i = 0; i < nDistributedSize; i++) {
      TValueCompositeEntry &composite_entry = (TValueCompositeEntry &)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);
      assert(Slip());
      int nIndex;
      stream >> nIndex;
      assert(composite_entry.FindIndex(nIndex));
      FaultParameters(model.InitialDepletionStage()).LinkTo(*composite_entry.FindIndex(nIndex));
    }
  } else {
    int bSlip;
    stream >> bSlip;
    Slip(bSlip != false);

    if (bSlip) {
      const CDepletionStage *pStage = &model.InitialDepletionStage();
      while (pStage) {
        Pressure(*pStage).LoadStream(stream, version, progress);

        int bParams;
        stream >> bParams;
        if (bParams) {
          assert(hasFaultParameters(*pStage));
          TFaultParametersMap::iterator it = m_mpFaultParameters.find(pStage);
          FaultParameters(*pStage).LoadStream(stream, version, progress);
        }

        if (pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }
    }

    int nSlipType;
    stream >> nSlipType;
    switch (nSlipType) {
    case 0:
      SlipType(STICK);
      break;
    case 1:
      SlipType(SLIP);
      break;
    case 2:
      SlipType(USER);
      break;
    case 3:
      SlipType(FRACTURE);
      break;
    default:
      assert(FALSE);
      SlipType(STICK);
    }
  }

  progress.Step();

  assert(Model().GraphEntry(MD_BASE_HORIZON));
  LinkTo(*Model().GraphEntry(MD_BASE_HORIZON));
}

void CHorizonBase::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  // Save base first
  CColorNode::SaveStream(stream, progress);

  std::string pr("Saving Horizon ");
  pr += Name().toStdString();

  progress.StatusMessage(pr.c_str());

  // Save slip
  if (Slip()) {
    stream << (int)TRUE; // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
                         /*
                             stream << m_pCohesion->Value();
                             stream << m_pFrictionAngle->Value();
                         */
    // since version 3.0.88
    const CModelBase &model = static_cast<const CModelBase &>(Model());
    const CDepletionStage *pStage = &model.InitialDepletionStage();
    while (pStage) {
      Pressure(*pStage).SaveStream(stream, progress);

      // since version 3.0.118
      TFaultParametersMap::iterator it = m_mpFaultParameters.find(pStage);
      bool bParams = (it != m_mpFaultParameters.end());
      stream << (int)bParams;
      if (bParams)
        it->second->SaveStream(stream, progress);

      if (pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
    }
  } else {
    stream << (int)FALSE; // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.
  }

  // Save the slip settings
  switch (m_SlipType) {
  case STICK:
    stream << 0;
    break;
  case SLIP:
    stream << 1;
    break;
  case USER:
    stream << 2;
    break;
  case FRACTURE:
    stream << 3;
    break;
  default:
    assert(false);
  }
  /*
    stream << m_StickCohesion.Value();
    stream << m_StickFrictionAngle.Value();

    // Save distributed fault parameters
    int nDistriSize = 0;
    if(m_pValueTypeCohesion) nDistriSize++;
    if(m_pValueTypeFriction) nDistriSize++;

    stream << nDistriSize;

    if(m_pValueTypeCohesion) stream << m_pValueTypeCohesion->Index();
    if(m_pValueTypeFriction) stream << m_pValueTypeFriction->Index();
  */
  progress.Step();
}

CFaultParametersNode &CHorizonBase::FaultParameters(const CDepletionStage &stage) {
  assert(Slip());

  TFaultParametersMap::iterator it = m_mpFaultParameters.find(&stage);
  if (it != m_mpFaultParameters.end())
    return *it->second;

  assert(!stage.Initial());
  return FaultParameters(stage.Previous());
}

const CFaultParametersNode &CHorizonBase::FaultParameters(const CDepletionStage &stage) const {
  return (const_cast<CHorizonBase *>(this))->FaultParameters(stage);
}

long CHorizonBase::SavedItems() const { return CColorNode::SavedItems() + 1; }

const dia::IMaterial &CHorizonBase::InterfaceMaterial(const geo::IElement & /*element*/,
                                                      const CDepletionStage & /*stage*/) const {
  assert(false);
  dia::IMaterial *pBogus = 0;
  return *pBogus;
}

void CHorizonBase::ClearMaterials() {
  for (TMaterialSet::iterator it = m_stMaterials.begin(); it != m_stMaterials.end(); it++)
    delete *it;
  m_stMaterials.clear();
}

void CHorizonBase::PrepareForAnalysis() {
  if (Slip())
    m_dAverageDStif = CalculateAverageDStif();
}

const double &CHorizonBase::AverageDStif() const { return m_dAverageDStif; }
