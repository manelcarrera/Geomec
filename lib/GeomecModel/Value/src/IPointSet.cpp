// IPointSet.cpp: implementation of the IPointSet class.
//
//////////////////////////////////////////////////////////////////////

#include "IPointSet.h"
#include "ModelBase.h"
#include "NodalValueSet.h"
#include "RpnValueSet.h"
#include "UnitConversion.h"
#include "ValueType.h"
#include "ValueTypeFactory.h"

#include "BaseEntryTypes.h"
#include "ElementPoint.h"
#include "IElement.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"
#include "Point.h"
#include "PointSet.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"

#include "WP_Helper.h"

static const unsigned int IDT_VALUETYPE_ANISOTROPIC_VP = 40154;
static const unsigned int IDT_VALUETYPE_ANISOTROPIC_VS = 40155;
static const unsigned int IDT_VALUETYPE_DENSITY = 33074;

typedef struct {
  unsigned int uiOld;
  unsigned int uiNew;
} TVTPair;

static TVTPair prVt[] = {{IDT_VALUETYPE_ANISOTROPIC_VP, IDT_VALUETYPE_VELOCITYP},
                         {IDT_VALUETYPE_ANISOTROPIC_VS, IDT_VALUETYPE_VELOCITYS},
                         {IDT_VALUETYPE_DENSITY, IDT_VALUETYPE_RHOB},
                         {0, 0}};

static void ConvertOldValueType(unsigned int &uiValueType) {
  for (int i = 0; prVt[i].uiOld != 0; ++i) {
    if (uiValueType == prVt[i].uiOld) {
      uiValueType = prVt[i].uiNew;
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IPointSet::IPointSet(const QString &sName, CFemAppModel &model, CQuantity::UNIT coordinate_unit,
                     CQuantity::UNIT value_unit)
    : CColorNode(sName, model), m_value_unit(value_unit), m_zaxis(Z_DOWN), m_coordinate_unit(coordinate_unit) {}

IPointSet::IPointSet(CFemAppModel &model)
    : CColorNode(model), m_value_unit(CQuantity::SI_UNIT), m_zaxis(Z_DOWN), m_coordinate_unit(CQuantity::SI_UNIT) {}

IPointSet::IPointSet(const IPointSet &rhs)
    : CColorNode(rhs), m_mpUnitConversion(rhs.m_mpUnitConversion), m_vcNodalValueSet(rhs.m_vcNodalValueSet),
      m_vcRpnValueSet(rhs.m_vcRpnValueSet), m_value_unit(rhs.m_value_unit), m_zaxis(rhs.m_zaxis),
      m_coordinate_unit(rhs.m_coordinate_unit) {
  // Make value types
  std::vector<CValueType *> vcValueType = rhs.ValueTypes();

  for (size_t i = 0; i < vcValueType.size(); i++)
    new CValueType(*vcValueType[i]);

  // Make value set copies ....
  for (int i = 0; i < rhs.NodalValueSetSize(); i++)
    new CNodalValueSet(rhs.NodalValueSet(i));

  for (int i = 0; i < rhs.RpnValueSetSize(); i++)
    new CRpnValueSet(rhs.RpnValueSet(i));
}

IPointSet::IPointSet(const IPointSet &rhs, CModelBase &model, TPROGRESS &progress)
    : CColorNode(rhs.Name().toStdString().c_str(), model), m_value_unit(rhs.m_value_unit), m_zaxis(rhs.m_zaxis),
      m_coordinate_unit(rhs.m_coordinate_unit) {
  // Make value set copies ....

  for (int i = 0; i < rhs.NodalValueSetSize(); i++) {
    new CNodalValueSet(*this, rhs.NodalValueSet(i), progress);
    progress.Step();
  }
  assert(NodalValueSetSize() == rhs.NodalValueSetSize());

  for (int i = 0; i < rhs.RpnValueSetSize(); i++) {
    new CRpnValueSet(*this, rhs.RpnValueSet(i));
    progress.Step();
  }
  assert(RpnValueSetSize() == rhs.RpnValueSetSize());
}

IPointSet::~IPointSet() {
  // Kill valuetypes
  std::vector<CValueType *> vcValueType = ValueTypes();

  for (size_t i = 0; i < vcValueType.size(); i++) {
    if (vcValueType[i]->IsCopy() == IsCopy())
      vcValueType[i]->Destroy();
  }

  // Kill valuesets ....
  std::vector<CGraphNode *> vcValueSet;
  for (int i = 0; i < RpnValueSetSize(); i++) {
    if (RpnValueSet(i).IsCopy() == IsCopy()) {
      assert(RpnValueSet(i).Component() == 0);
      vcValueSet.push_back(&RpnValueSet(i));
    }
  }

  for (int i = 0; i < NodalValueSetSize(); i++) {
    if (NodalValueSet(i).IsCopy() == IsCopy()) {
      assert(NodalValueSet(i).Component() == 0);
      vcValueSet.push_back(&NodalValueSet(i));
    }
  }

  for (size_t i = 0; i < vcValueSet.size(); i++) {
    CGraphNode *pNode = vcValueSet[i];
    delete pNode;
  }
}

void IPointSet::OnNeighbourDeleted(const CGraphNode &item) {
  COpenGLNode::OnNeighbourDeleted(item);

  // Search vector for nodal valuesets
  for (size_t i = 0; i < m_vcNodalValueSet.size(); i++) {
    if (m_vcNodalValueSet[i] == &item) {
      // Item found remove and exit
      m_vcNodalValueSet.erase(m_vcNodalValueSet.begin() + i);
      return;
    }
  }

  // Search for RpnValueSets
  for (size_t i = 0; i < m_vcRpnValueSet.size(); i++) {
    if (m_vcRpnValueSet[i] == &item) {
      // Item found remove and exit
      m_vcRpnValueSet.erase(m_vcRpnValueSet.begin() + i);
      return;
    }
  }
}

std::vector<CValueType *> IPointSet::ValueTypes() const {
  std::vector<CValueType *> vcRet;

  for (size_t i = 0; i < childSize(); i++) {
    const CValueType *pType = dynamic_cast<const CValueType *>(&childAt(i));
    if (pType) {
      if (IsCopy() && pType->IsCopied()) {
        CValueType &value_type = (CValueType &)(pType->Copy());
        if (&value_type.PointSet() == this)
          vcRet.push_back(&value_type);
      } else {
        if (&pType->PointSet() == this)
          vcRet.push_back((CValueType *)pType);
      }
    }
  }

  return vcRet;
}

IPointSet &IPointSet::operator=(const IPointSet &rhs) {
  assert(rhs.IsCopy());

  // The valuesets which have no copy are deleted from the copy
  // and now from the source ...

  for (int i = 0; i < NodalValueSetSize(); i++) {
    assert(!NodalValueSet(i).IsCopy());
    if (!NodalValueSet(i).IsCopied())
      delete &NodalValueSet(i);
  }

  for (int i = 0; i < RpnValueSetSize(); i++) {
    assert(!RpnValueSet(i).IsCopy());
    if (!RpnValueSet(i).IsCopied())
      delete &RpnValueSet(i);
  }

  // The valuetypes which have no copy are deleted from the copy
  // and now from the source ...
  std::vector<CValueType *> vcValueType = ValueTypes();
  for (size_t i = 0; i < vcValueType.size(); i++) {
    assert(!vcValueType[i]->IsCopy());
    if (!vcValueType[i]->IsCopied())
      delete vcValueType[i];
  }

  m_coordinate_unit = rhs.m_coordinate_unit;
  m_value_unit = rhs.m_value_unit;

  CColorNode::operator=(rhs);

  m_mpUnitConversion.clear();
  m_vcNodalValueSet = rhs.m_vcNodalValueSet;
  m_vcRpnValueSet = rhs.m_vcRpnValueSet;
  m_zaxis = rhs.m_zaxis;

  // Validate all
  for (int i = 0; i < NodalValueSetSize(); i++) {
    assert(NodalValueSet(i).IsCopied());
    NodalValueSet(i) = ((CNodalValueSet &)NodalValueSet(i).Copy());
  }

  for (int i = 0; i < RpnValueSetSize(); i++) {
    assert(RpnValueSet(i).IsCopied());
    RpnValueSet(i) = ((CRpnValueSet &)RpnValueSet(i).Copy());
  }

  // Validate valuetypes
  vcValueType = rhs.ValueTypes();
  for (size_t i = 0; i < vcValueType.size(); i++) {
    assert(vcValueType[i]->IsCopy());
    ((CValueType &)vcValueType[i]->Source()) = *vcValueType[i];
  }

  return *this;
}

int IPointSet::NodalValueSetSize() const { return m_vcNodalValueSet.size(); }

bool IPointSet::Overlap(const IPointSet &point_set) const {
  return (Min().X() < point_set.Max().X()) && (Min().Y() < point_set.Max().Y()) && (Min().Z() < point_set.Max().Z()) &&
         (point_set.Min().X() < Max().X()) && (point_set.Min().Y() < Max().Y()) && (point_set.Min().Z() < Max().Z());
}

const CNodalValueSet &IPointSet::NodalValueSet(int nIndex) const {
  assert(nIndex > -1);
  assert(nIndex < m_vcNodalValueSet.size());

  if (IsCopy())
    return (const CNodalValueSet &)m_vcNodalValueSet[nIndex]->Copy();

  return *m_vcNodalValueSet[nIndex];
}

CNodalValueSet &IPointSet::NodalValueSet(int nIndex) {
  assert(nIndex > -1);
  assert(nIndex < m_vcNodalValueSet.size());

  if (IsCopy())
    return (CNodalValueSet &)m_vcNodalValueSet[nIndex]->Copy();

  return *m_vcNodalValueSet[nIndex];
}

int IPointSet::RpnValueSetSize() const { return m_vcRpnValueSet.size(); }

size_t IPointSet::UnitConversionSize() const { return m_mpUnitConversion.size(); }

bool IPointSet::AnyUnitConversionDefined() const { return UnitConversionSize() != 0; }

const IUnitConversion *IPointSet::UnitConversion(const std::string &name) const {
  TUnitConversionMap::const_iterator it = m_mpUnitConversion.find(name);
  if (it != m_mpUnitConversion.end())
    return it->second;

  return NULL;
}

void IPointSet::RemoveUnitConversion(const std::string &name) {
  TUnitConversionMap::iterator it = m_mpUnitConversion.find(name);
  if (it == m_mpUnitConversion.end())
    return;

  IUnitConversion::Remove(it->second);
  m_mpUnitConversion.erase(it);
}

void IPointSet::RemoveAllUnitConversions() {
  for (TUnitConversionMap::iterator it = m_mpUnitConversion.begin(); it != m_mpUnitConversion.end(); ++it)
    delete it->second;
  m_mpUnitConversion.clear();
}

void IPointSet::AddUnitConversion(const std::string &name, IUnitConversion *conversion) {
  m_mpUnitConversion.insert(make_pair(name, conversion));
}

int IPointSet::AddNodalValueSet() {
  int nIndex = ValueSetSize();

  // Create new name
  QString sName;
  QString stringTableEntry = getStringTableEntry(IDS_PROPERTY_NAME);
  sName = QString(stringTableEntry).arg(nIndex);
  new CNodalValueSet(sName, CQuantity::SI_UNIT, *this);

  return nIndex;
}

const CRpnValueSet &IPointSet::RpnValueSet(int nIndex) const {
  assert(nIndex > -1);
  assert(nIndex < m_vcRpnValueSet.size());

  if (IsCopy())
    return (const CRpnValueSet &)m_vcRpnValueSet[nIndex]->Copy();

  return *m_vcRpnValueSet[nIndex];
}

CRpnValueSet &IPointSet::RpnValueSet(int nIndex) {
  assert(nIndex > -1);
  assert(nIndex < m_vcRpnValueSet.size());

  if (IsCopy())
    return (CRpnValueSet &)m_vcRpnValueSet[nIndex]->Copy();

  return *m_vcRpnValueSet[nIndex];
}

CQuantity::UNIT IPointSet::CoordinateUnit() const { return m_coordinate_unit; }

void IPointSet::CoordinateUnit(CQuantity::UNIT unit) {
  if (m_coordinate_unit != unit) {
    OnCoordinateUnitChanged(m_coordinate_unit, unit);
    m_coordinate_unit = unit;
  }
}

// wjrx mantis 3336
void IPointSet::CoordinateInverseUnit(CQuantity::UNIT unit) {
  if (m_coordinate_unit != unit) {
    OnCoordinateUnitChanged(unit, m_coordinate_unit); // reversed
    m_coordinate_unit = unit;
  }
}

void IPointSet::CoordinateUnitEx(CQuantity::UNIT unit) {
  if (m_coordinate_unit != unit) {
    OnCoordinateUnitChanged(m_coordinate_unit, unit);
    m_coordinate_unit = unit;
  }
}

CQuantity::UNIT IPointSet::ValueUnit(bool bForceStoredValue) const {
  // wjrx mantis 3125
  // if .dat file contains an explicit unit conversion,
  // UNIT FIELD command is ignored
  //
  if (!bForceStoredValue && AnyUnitConversionDefined())
    return CQuantity::SI_UNIT;
  else
    return m_value_unit;
}

void IPointSet::ValueUnit(CQuantity::UNIT unit) { m_value_unit = unit; }

bool IPointSet::operator==(const IPointSet &rhs) const {
  if (!(CColorNode::operator==(rhs)))
    return false;

  if (m_value_unit != rhs.m_value_unit)
    return false;

  if (m_coordinate_unit != rhs.m_coordinate_unit)
    return false;

  if (m_mpUnitConversion != rhs.m_mpUnitConversion)
    return false;

  if (m_vcNodalValueSet != rhs.m_vcNodalValueSet)
    return false;

  if (m_vcRpnValueSet != rhs.m_vcRpnValueSet)
    return false;

  if (m_zaxis != rhs.m_zaxis)
    return false;

  // Validate Nodal value sets
  for (int i = 0; i < NodalValueSetSize(); i++) {
    assert(NodalValueSet(i).IsCopied());
    if (!((CNodalValueSet &)NodalValueSet(i) == ((CNodalValueSet &)NodalValueSet(i).Copy())))
      return false;
  }

  // Validate Rpn value sets
  for (int i = 0; i < RpnValueSetSize(); i++) {
    assert(RpnValueSet(i).IsCopied());
    if (!((CRpnValueSet &)RpnValueSet(i) == ((CRpnValueSet &)RpnValueSet(i).Copy())))
      return false;
  }

  // Validate valuetypes
  std::vector<CValueType *> vcValueType = rhs.ValueTypes();
  for (size_t i = 0; i < vcValueType.size(); i++) {
    assert(vcValueType[i]->IsCopy());
    if (!(((CValueType &)vcValueType[i]->Source()) == *vcValueType[i]))
      return false;
  }

  return true;
}

int IPointSet::ValueSetSize() const { return NodalValueSetSize() + RpnValueSetSize(); }

const IValueSet &IPointSet::ValueSet(int nIndex) const {
  if (nIndex < NodalValueSetSize())
    return NodalValueSet(nIndex);

  return RpnValueSet(nIndex - NodalValueSetSize());
}

IValueSet &IPointSet::ValueSet(int nIndex) {
  if (nIndex < NodalValueSetSize())
    return NodalValueSet(nIndex);

  return RpnValueSet(nIndex - NodalValueSetSize());
}

bool IPointSet::ConnectItem(const CGraphNode &item) {
  // We don't support results anymore
  const CRpnValueSet *pRpnValueSet = dynamic_cast<const CRpnValueSet *>(&item);
  if (pRpnValueSet) {
    CRpnValueSet *pTarget = new CRpnValueSet(*this);
    pTarget->InsertFormula(*pRpnValueSet);
    pTarget->EnableProxy(&(reinterpret_cast<const CModelBase &>(Model())));
    Modified();
    return true;
  }

  return COpenGLNode::ConnectItem(item);
}

// Accept rpn calculators
bool IPointSet::CanConnectItem(const CGraphNode &item) const {
  // We don't support results anymore
  const CRpnValueSet *pRpnValueSet = dynamic_cast<const CRpnValueSet *>(&item);
  if (pRpnValueSet)
    return true;

  return COpenGLNode::CanConnectItem(item);
}

void IPointSet::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Load the base class
  //	assert( Empty() );
  CColorNode::LoadStream(stream, version, progress);

  if (CStreamVersion(4, 1, 74) < version) {
    stream >> m_guid;
  }

  // We didn't load the name until after loading the base, so we recover what we can h
  progress.StatusMessage("Loading pointset " + Name());

  if (CStreamVersion(3, 0, 10) < version) {
    int bZaxis;
    stream >> bZaxis;
    if (bZaxis)
      m_zaxis = Z_UP;
    else
      m_zaxis = Z_DOWN;
  }

  // Load the node value sets first
  int nValueSetSize;
  stream >> nValueSetSize;
  int i;
  for (i = 0; i < nValueSetSize; i++) {
    int nValueSetIndex = AddNodalValueSet();
    NodalValueSet(nValueSetIndex).LoadStream(stream, version, progress);
    progress.Step();
  }

  // From version 3.0.5 the RpnValueSet is saved in the pointset
  if (CStreamVersion(3, 0, 4) < version) {
    // Load the rpn value sets
    stream >> nValueSetSize;
    for (i = 0; i < nValueSetSize; i++) {
      CRpnValueSet *pValueSet = new CRpnValueSet(*this);
      pValueSet->LoadStream(stream, version, progress);
      progress.Step();
    }
  }

  // Load the value types
  int nValueTypeSize;
  stream >> nValueTypeSize;
  const CValueTypeFactory *ValueTypeFactory = CValueTypeFactory::instance();
  for (i = 0; i < nValueTypeSize; i++) {
    QString strTemp;
    unsigned int uiTemp;

    stream >> strTemp;

    stream >> uiTemp;
    ConvertOldValueType(uiTemp);

    CValueType *pValueType = ValueTypeFactory->BuildValueType(*this, uiTemp, strTemp);
    assert(IsLinkedTo(*pValueType));
    pValueType->LoadStream(stream, version, progress);
  }

  // In the old days the point set data was saved here. So for 3.0.4 and earlier the save function
  // stops here
  if (CStreamVersion(3, 0, 4) < version) {
    int iResultCount;
    stream >> iResultCount;
    for (int count = 0; count < iResultCount; count++) {
      // The result adapter only saves the name and index (CStorageNode::LoadStream(..) base
      // implementation). We only support this, because we don't want a corrupt stream ...
      QString sName;
      stream >> sName;
      int iIndex;
      stream >> iIndex;
    }
  }
}

void IPointSet::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  // Save the base first
  CColorNode::SaveStream(stream, progress);

  stream << m_guid;

  // Saving the zaxis tag from 3.0.11
  bool bZaxis = m_zaxis == Z_UP;
  stream << (int)bZaxis;

  // From version 3.0.5 the valuesets are saved in the IPointSet base
  // class. Earlier versions did nothing here...

  // Save nodal value sets first
  stream << NodalValueSetSize();

  for (int i = 0; i < NodalValueSetSize(); i++) {
    // Set the number first and serialize
    NodalValueSet(i).FileIndex(i + 1);
    NodalValueSet(i).SaveStream(stream, progress);
    progress.Step();
  }

  // Save RpnValueSet
  stream << RpnValueSetSize();
  for (int i = 0; i < RpnValueSetSize(); i++) {
    // Set the number first and serialize
    RpnValueSet(i).FileIndex(NodalValueSetSize() + i + 1);
    RpnValueSet(i).SaveStream(stream, progress);
    progress.Step();
  }

  // Save all value types, which belonging to this pointset.
  int iTemp;
  iTemp = ValueTypes().size();
  stream << iTemp;
  std::vector<CValueType *> vcOwnedTypes = ValueTypes();
  for (size_t i = 0; i < vcOwnedTypes.size(); i++) {
    // This is a ValueTypes that references this pointset so it must belong to me
    stream << vcOwnedTypes[i]->Name();
    stream << vcOwnedTypes[i]->TypeId();

    vcOwnedTypes[i]->SaveStream(stream, progress);
  }

  int nResultAdapterSize = 0;
  stream << nResultAdapterSize; // We don't have any result adapters ... Use the export dialog
}

long IPointSet::SavedItems() const {
  long lRet = CColorNode::SavedItems();

  lRet += NodalValueSetSize();

  int i;
  for (i = 0; i < NodalValueSetSize(); i++)
    lRet += NodalValueSet(i).SavedItems();

  lRet += RpnValueSetSize();

  for (i = 0; i < RpnValueSetSize(); i++)
    lRet += RpnValueSet(i).SavedItems();

  return lRet;
}

IPointSet::Z_AXIS IPointSet::ZAxis() const { return m_zaxis; }

void IPointSet::ZAxis(Z_AXIS axis) {
  assert(IsCopy());
  m_zaxis = axis;
}

bool IPointSet::ElementInConvexHull(const geo::IElement &element) const {
  for (int i = 0; i < element.NrOfPoints(); i++) {
    if (PointInConvexHull(element.Point(i)))
      return true;
  }

  return false;
}

bool IPointSet::PrepareMapping(const geo::IElementSet *pElementSet) {
  for (size_t i = 0; i < ValueSetSize(); ++i) {
    if (!ValueSet(i).PrepareMapping(pElementSet))
      return false;
  }

  return ValueSetSize() > 0;
}

bool IPointSet::isDrawable() const {
  return !((Links<CValueType>().size() > 0) && (NodalValueSetSize() > 0) && (NodalValueSet(0).ValueSize() == 2));
}

bool IPointSet::isUsed() const {
  std::vector<CValueType *> vcOwnedTypes = ValueTypes();
  for (int i = 1; i < vcOwnedTypes.size(); ++i) {
    if (vcOwnedTypes[i]->referenceSize() > 1) // ignore the graphentry
      return true;
  }

  return false;
}

void CPointSetEntry::DestroyCaches() { m_cache_manager.DestroyCaches(); }

void CPointSetEntry::OnNewNeighbour(const CGraphNode &node) {
  const IPointSet *pPointSet = dynamic_cast<const IPointSet *>(&node);

  if (pPointSet && pPointSet->HasElements()) {
    const geo::IElementSet &elt_set = static_cast<const IElementSet &>(node).ElementSet();
    m_cache_manager.RegisterElementSet(&elt_set);
  }

  CGraphEntryTemp<IPointSet>::OnNewNeighbour(node);
}

void CPointSetEntry::OnNeighbourDeleted(const CGraphNode &node) {
  const IPointSet *pPointSet = dynamic_cast<const IPointSet *>(&node);

  if (pPointSet && pPointSet->HasElements()) {
    const geo::IElementSet &elt_set = static_cast<const IElementSet &>(node).ElementSet();
    m_cache_manager.UnregisterElementSet(&elt_set);
  }

  CGraphEntryTemp<IPointSet>::OnNeighbourDeleted(node);
}

void CPointSetEntry::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint /*uHint*/) {
  const IPointSet *pPointSet = dynamic_cast<const IPointSet *>(&node);

  if (pPointSet && pPointSet->HasElements()) {
    const geo::IElementSet &elt_set = static_cast<const IElementSet &>(node).ElementSet();
    m_cache_manager.UnregisterElementSet(&elt_set);
    m_cache_manager.RegisterElementSet(&elt_set);
  }
}

void CPointSetEntry::SetCreateHullCallback(CreateHullCallback cb) { m_hullCB = cb; }

bool CPointSetEntry::CanCreateAllHulls() const {
  const TNodeSet &stNodes = EntryNodes();
  bool bPS = false;
  for (TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    const CPointSet *pPS = dynamic_cast<const CPointSet *>(*it);
    if (pPS && !pPS->HasHull()) {
      bPS = true;
      break;
    }
  }

  return bPS;
}

void CPointSetEntry::CreateAllHulls() {
  const TNodeSet &stNodes = EntryNodes();
  for (TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    CPointSet *pPS = dynamic_cast<CPointSet *>(*it);
    if (pPS && !pPS->HasHull()) {
      if (!pPS->CreateHull(m_hullCB))
        return;
    }
  }
}

bool CPointSetEntry::CanCopyAllWellpaths() const {
  const TNodeSet &stNodes = EntryNodes();
  for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
    if ((*it)->pointSetType() == IPointSet::INPUT)
      return true;
  return false;
}

void CPointSetEntry::CopyAllWellpaths() {
  WP_Helper helper(this);
  helper.CopyAllWellpaths();
  return;
#if 0
  CModelBase& rootModel = static_cast<CModelBase&>(Model()).RootModel();

  const CZoominModelEntry* zoominModelEntry = dynamic_cast<const CZoominModelEntry*>(rootModel.GraphEntry(MD_BASE_ZOOMIN_MODEL));
  const CZoominModelEntry::TNodeSet& zoominModels = zoominModelEntry->EntryNodes();

  const TNodeSet& stNodes = EntryNodes();

  CGraphNode::TNodeVec nodeVec;

  if (!rootModel.CanCreateMesh())
  for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
      if ((*it)->pointSetType() == IPointSet::INPUT)
      {
    const CNewWellPath *pWellPath = CNewWellPathInput::CreateWellPath(static_cast<CNewWellPathInput *>(*it), rootModel);
    if (pWellPath)
          nodeVec.push_back(const_cast<CNewWellPath *>(pWellPath));
      }

  for (CZoominModelEntry::TNodeSet::const_iterator node = zoominModels.begin(); node != zoominModels.end(); ++node)
  {
  if (!(*node)->ChildModel().CanCreateMesh())
      for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
    if ((*it)->pointSetType() == IPointSet::INPUT)
          CNewWellPathInput::CreateWellPath(static_cast<CNewWellPathInput *>(*it), (*node)->ChildModel());
  }

  CNewWellPathEntry::addPointSet2DataStorage(nodeVec, rootModel);
#endif
}

bool CPointSetEntry::CanDeleteAllUnusedPointsets() const {
  const TNodeSet &stNodes = EntryNodes();
  for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    if (!dynamic_cast<const CMeshBase *>(*it) && !(*it)->isUsed())
      return true;
  }
  return false;
}

void CPointSetEntry::DeleteAllUnusedPointsets() {
  const TNodeSet &stNodes = EntryNodes();
  TNodeSet stUnused;
  for (TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    if (!dynamic_cast<const CMeshBase *>(*it) && !(*it)->isUsed())
      stUnused.insert(*it);
  }

  for (TNodeSet::iterator it = stUnused.begin(); it != stUnused.end(); ++it) {
    UnLink(**it);
    delete *it;
  }
}
