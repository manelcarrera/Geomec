// ValueType.cpp: implementation of the CValueType class.
//
//////////////////////////////////////////////////////////////////////

#include "ValueType.h"
#include "IPointSet.h"
#include "ModifiedHint.h"
#include "PointSet.h"
#include "StreamVersion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueType::CValueType(IPointSet &point_set, const QString &strName)
    : IValueComposite(strName, point_set.Model()), m_map_type(MT_POINT) {
  // Link to the source point always
  if (point_set.IsCopy())
    m_pPointSet = (CPointSet *)&point_set.Source();
  else {
    m_pPointSet = &point_set;
    reParent(&point_set);
    assert(IsLinkedTo(PointSet()));
  }
}

CValueType::CValueType(IPointSet &point_set, unsigned int uName)
    : IValueComposite(uName, point_set.Model()), m_map_type(MT_POINT) {
  // Link to the source point always
  if (point_set.IsCopy())
    m_pPointSet = (CPointSet *)&point_set.Source();
  else {
    m_pPointSet = &point_set;
    reParent(&point_set);
    assert(IsLinkedTo(PointSet()));
  }
}

CValueType::CValueType(const CValueType &rhs)
    : IValueComposite(rhs), m_pPointSet(rhs.m_pPointSet), m_map_type(rhs.m_map_type) {
  assert(!m_pPointSet->IsCopy());

  if (!IsLinkedTo(PointSet()))
    reParent(&PointSet());

  for (unsigned int uMode = 0; uMode < ModeSize(); uMode++) {
    for (unsigned int uComponent = 0; uComponent < ComponentSize(uMode); uComponent++) {
      CValueComponent *pComponent = dynamic_cast<CValueComponent *>(&Component(uComponent, uMode));
      if (pComponent) {
        assert(!pComponent->IsCopied());
        new CValueComponent(*pComponent);
      }
    }
  }
}

CValueType::~CValueType() {}

bool CValueType::SingleElementValue() const { return false; }

bool CValueType::CanDestroy() const {
  if (IsCopy())
    return Source().CanDestroy();
  return IValueComposite::CanDestroy();
}

const IPointSet &CValueType::PointSet() const {
  assert(!m_pPointSet->IsCopy());
  if (IsCopy() && m_pPointSet->IsCopied())
    return (const IPointSet &)(m_pPointSet->Copy());

  return *m_pPointSet;
}

IPointSet &CValueType::PointSet() {
  assert(!m_pPointSet->IsCopy());
  if (IsCopy() && m_pPointSet->IsCopied())
    return (IPointSet &)(m_pPointSet->Copy());

  return *m_pPointSet;
}

CValueType &CValueType::operator=(const CValueType &rhs) {
  IValueComposite::operator=(rhs);

  for (unsigned int uMode = 0; uMode < rhs.ModeSize(); uMode++) {
    for (unsigned int uComponent = 0; uComponent < rhs.ComponentSize(uMode); uComponent++) {
      const CValueComponent *pComponent = dynamic_cast<const CValueComponent *>(&rhs.Component(uComponent, uMode));
      if (pComponent) {
        assert(!pComponent->IsCopied());
        if (pComponent->IsCopy())
          ((CValueComponent &)pComponent->Source()) = *pComponent;
      }
    }
  }

  bool bModified = false;
  if (m_map_type != rhs.m_map_type) {
    m_map_type = rhs.m_map_type;
    bModified = true;
  }

  if (Name() != rhs.Name()) {
    Name(rhs.Name());
    bModified = true;
  }

  if (bModified) {
    Modified();
    Component().Modified();
  }

  return *this;
}

bool CValueType::operator==(const CValueType &rhs) const {
  if (!IValueComposite::operator==(rhs))
    return false;

  for (unsigned int uMode = 0; uMode < rhs.ModeSize(); uMode++) {
    for (unsigned int uComponent = 0; uComponent < rhs.ComponentSize(uMode); uComponent++) {
      const CValueComponent *pComponent = dynamic_cast<const CValueComponent *>(&rhs.Component(uComponent, uMode));
      if (pComponent) {
        assert(!pComponent->IsCopied());
        if (pComponent->IsCopy())
          if (!(((CValueComponent &)pComponent->Source()) == *pComponent))
            return false;
      }
    }
  }

  return m_map_type == rhs.m_map_type;
}

void CValueType::OnNeighbourDeleted(const CGraphNode &node) {
  if (&PointSet() == &node) {
    IValueComposite::OnNeighbourDeleted(node);
    delete this;
    return;
  }

  IValueComposite::OnNeighbourDeleted(node);
}

unsigned int CValueType::IconId() const {
  assert(IsCopy());
  return Source().IconId();
}

unsigned int CValueType::TypeId() const {
  assert(IsCopy());
  return Source().TypeId();
}

QString CValueType::TypeName() const {
  assert(IsCopy());
  return Source().TypeName();
}

QString CValueType::ExportType() const { return ""; }

bool CValueType::CanExport() const {
  for (unsigned int nComponent = 0; nComponent < ComponentSize(); nComponent++) {
    const CValueComponent *pComponent = dynamic_cast<const CValueComponent *>(&Component(nComponent));
    if (pComponent)
      return false;

    if (!pComponent->Defined())
      return false;
  }

  return true;
}

//=============================================================================
// Weigthing
enum CValueType::TMapEnum CValueType::MapType() const { return m_map_type; }

void CValueType::MapType(enum CValueType::TMapEnum type) {
  bool bModified = false;

  if (m_map_type != type) {
    m_map_type = type;
    bModified = true;
  }

  if (bModified) {
    Modified(MapTypeChanged);
    Component().Modified();
  }
}

bool CValueType::Destroy() {
  assert(CanDestroy() || IsCopy());
  if (IsCopy()) {
    CValueType *pSource = (CValueType *)(&Source());

    // Destroy ourselves first ....
    delete this;

    // Is source made by a copied pointset?
    if (!pSource->IsLinkedTo(pSource->PointSet()))
      delete pSource;

    return true;
  }

  delete this;
  return true;
}

bool CValueType::Empty() const {
  return false; // Never empty
}

void CValueType::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  IValueComposite::SaveStream(stream, progress);

  stream << (int)m_map_type;

  unsigned int uiTemp;
  uiTemp = ComponentSize(0);
  stream << uiTemp;
  for (unsigned int iCount = 0; iCount < ComponentSize(0); iCount++) {
    CValueComponent &ValComponent = dynamic_cast<CValueComponent &>(Component(iCount, 0));
    if (ValComponent.Defined())
      stream << ValComponent.ValueSet().Index();
    else
      stream << -1;
  }
}

void CValueType::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  IValueComposite::LoadStream(stream, version, progress);

  // From version 3.0.29 we load weighting type
  if (CStreamVersion(3, 0, 28) < version) {
    int nType;
    stream >> nType;
    m_map_type = (TMapEnum)nType;
    /*
    #ifdef MAPPING_KEYFILE
        CGeomecApp &App = dynamic_cast<CGeomecApp &> (*AfxGetApp());
        if(!App.KeyFile().Unlocked() && m_map_type != MT_POINT)
        {
          _m()->msg(IDP_MAPPING_LOCKED_LOAD);
          m_map_type = MT_POINT;
        }
    #endif
    */
  }

  unsigned int uiTemp;
  stream >> uiTemp;
  for (unsigned int iCount = 0; iCount < uiTemp; iCount++) {
    int iSetIndex;
    stream >> iSetIndex;
    if (iSetIndex > -1) {
      // Find ValuSet in current points
      IValueSet *pValueSet = 0;
      for (int iSet = 0; iSet < PointSet().ValueSetSize(); iSet++) {
        if (PointSet().ValueSet(iSet).Index() == iSetIndex)
          pValueSet = &PointSet().ValueSet(iSet);
      }
      assert(pValueSet);
      Component(iCount).LinkTo(*pValueSet);
    }
  }
}

long CValueType::SavedItems() const { return 0; }

void CValueType::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  // wjrx mantis 3335
  if (&node == m_pPointSet && ModeSize() > 0 && ComponentSize() == 1)
    Component().Modified();

  IValueComposite::OnNeighbourModified(node, uHint);
}

bool CValueType::PrepareMapping(const geo::IElementSet *pElementSet) {
  if (m_pPointSet)
    return m_pPointSet->PrepareMapping(pElementSet);

  return false;
}
