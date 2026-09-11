#ifndef _MATERIALSERVERPARENT_H_
#define _MATERIALSERVERPARENT_H_

class CDepletionStage;

#include "BaseEntryTypes.h"

#include "StorageNode.h"

namespace geo {
class IObject;
}

#include "BranchState.h"
#include "DepletionStage.h"
#include "IValueSet.h"
#include "ModelBase.h"
#include "StreamVersion.h"
#include "ValueComponent.h"

typedef CStorageNode::TSTREAM TSTREAM;
typedef CStorageNode::TPROGRESS TPROGRESS;

template <class BASE, class MATERIALSERVER> class CMaterialServerParent : public BASE {
public:
  CMaterialServerParent(const QString &strName, CModelBase &model);
  CMaterialServerParent(CFemAppModel &model);
  CMaterialServerParent(const CMaterialServerParent &rhs);

  virtual ~CMaterialServerParent();

  CMaterialServerParent &operator=(const CMaterialServerParent &rhs);

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // returns the 'active' material for the given depletion stage
  MATERIALSERVER &Material(const CDepletionStage &stage);
  const MATERIALSERVER &Material(const CDepletionStage &stage) const;

  // returns a material when it is actually defined for the stage
  // (always returns a valid pointer for the initial stage)
  MATERIALSERVER *ConnectedMaterial(const CDepletionStage &stage);
  const MATERIALSERVER *ConnectedMaterial(const CDepletionStage &stage) const;

  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  bool HasMaterial(const CDepletionStage &stage) const;

  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual long SavedItems() const;

  // When BASE is CGraphNode or derived from CGraphNode!

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  // When BASE is COpenGLNode or derived from COpenGLNode!

  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;

protected:
  virtual void SaveMaterial(CDepletionStage &stage, TSTREAM &stream, TPROGRESS &progress);
  virtual void LoadMaterial(CDepletionStage &stage, TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual long SavedMaterialItems(const CDepletionStage &stage) const;

private:
  typedef std::map<const CDepletionStage *, MATERIALSERVER *> TMaterialMap;
  TMaterialMap m_mpMaterial;
};

///// Implementation

template <class BASE, class MATERIALSERVER>
CMaterialServerParent<BASE, MATERIALSERVER>::CMaterialServerParent(const QString &strName, CModelBase &model)
    : BASE(strName, model) {
  BASE::LinkTo(model.DepletionStageEntry());

  const CDepletionStageEntry &entry = model.DepletionStageEntry();
  for (CDepletionStageEntry::const_iterator it = entry.begin(); it != entry.end(); ++it) {
    if (HasMaterial(*it))
      new MATERIALSERVER(*this, *it);
  }
}

template <class BASE, class MATERIALSERVER>
CMaterialServerParent<BASE, MATERIALSERVER>::CMaterialServerParent(CFemAppModel &model) : BASE(model) {}

template <class BASE, class MATERIALSERVER>
CMaterialServerParent<BASE, MATERIALSERVER>::CMaterialServerParent(const CMaterialServerParent &rhs)
    : BASE(rhs), m_mpMaterial(rhs.m_mpMaterial) {}

template <class BASE, class MATERIALSERVER> CMaterialServerParent<BASE, MATERIALSERVER>::~CMaterialServerParent() {
  if (!BASE::IsCopy()) {
    std::vector<MATERIALSERVER *> vcMaterials;
    typename TMaterialMap::iterator itm;
    for (itm = m_mpMaterial.begin(); itm != m_mpMaterial.end(); ++itm)
      vcMaterials.push_back(itm->second);

    for (size_t i = 0; i < vcMaterials.size(); ++i)
      vcMaterials[i]->UnLink(*this);
  }
}

template <class BASE, class MATERIALSERVER>
CMaterialServerParent<BASE, MATERIALSERVER> &
CMaterialServerParent<BASE, MATERIALSERVER>::operator=(const CMaterialServerParent &rhs) {
  BASE::operator=(rhs);

  m_mpMaterial = rhs.m_mpMaterial;

  return *this;
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::OnNewNeighbour(const CGraphNode &node) {
  const MATERIALSERVER *pMatServer = dynamic_cast<const MATERIALSERVER *>(&node);
  if (pMatServer) {
    bool succeeded =
        m_mpMaterial
            .insert(typename TMaterialMap::value_type(&pMatServer->Stage(), const_cast<MATERIALSERVER *>(pMatServer)))
            .second;

    assert(succeeded);
  }
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::OnNeighbourModified(const CGraphNode &node,
                                                                      enum ModifiedHint /*uHint*/) {
  bool bModified = false;

  CDepletionStageEntry &entry = (CDepletionStageEntry &)(*BASE::Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  if (&entry == &node) {
    for (CDepletionStageEntry::iterator itd = entry.begin(); itd != entry.end(); ++itd) {
      typename TMaterialMap::iterator it = m_mpMaterial.find(&*itd);
      if (it == m_mpMaterial.end()) {
        // is it an initial or after-branch stage?
        if (HasMaterial(*itd)) {
          MATERIALSERVER *pMatServer = new MATERIALSERVER(*this, *itd); // create material server for it
          bModified = true;
          if (!itd->Last()) {
            typename TMaterialMap::iterator itnext = m_mpMaterial.find(&itd->Next());
            if (itnext != m_mpMaterial.end()) {
              MATERIALSERVER *pNextServer = itnext->second;
              if (pNextServer->LibraryMaterial()) {
                pMatServer->LinkTo(*pNextServer->LibraryMaterial());
                const typename MATERIALSERVER::TValueTypePairVec vcValueTypes = pNextServer->ValueTypePairVec();
                for (size_t i = 0; i < vcValueTypes.size(); ++i)
                  pMatServer->LinkTo((CValueType &)*vcValueTypes[i].first);
              }
            }
          }
        }
      } else {
        // is it not an initial or after-branch stage anymore?
        if (!HasMaterial(*itd)) {
          it->second->UnLink(*this); // destroy the material server
          bModified = true;
        }
      }
    }
  }

  if (bModified)
    BASE::Modified();
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::OnNeighbourDeleted(const CGraphNode &node) {
  typename TMaterialMap::iterator itm;
  for (itm = m_mpMaterial.begin(); itm != m_mpMaterial.end(); ++itm) {
    if (itm->second == &node) {
      m_mpMaterial.erase(itm);
      BASE::OnNeighbourDeleted(node);
      return;
    }
  }

  BASE::OnNeighbourDeleted(node);
}

template <class BASE, class MATERIALSERVER>
MATERIALSERVER &CMaterialServerParent<BASE, MATERIALSERVER>::Material(const CDepletionStage &stage) {
  MATERIALSERVER *pMat = ConnectedMaterial(stage);
  if (pMat && (pMat->LibraryMaterial() || stage.Initial()))
    return *pMat;

  assert(!stage.Initial());
  return Material(stage.Previous());
}

template <class BASE, class MATERIALSERVER>
const MATERIALSERVER &CMaterialServerParent<BASE, MATERIALSERVER>::Material(const CDepletionStage &stage) const {
  const MATERIALSERVER *pMat = ConnectedMaterial(stage);
  if (pMat && (pMat->LibraryMaterial() || stage.Initial()))
    return *pMat;

  assert(!stage.Initial());
  return Material(stage.Previous());
}

template <class BASE, class MATERIALSERVER>
MATERIALSERVER *CMaterialServerParent<BASE, MATERIALSERVER>::ConnectedMaterial(const CDepletionStage &stage) {
  typename TMaterialMap::iterator it = m_mpMaterial.find(&stage);
  if (it != m_mpMaterial.end())
    return it->second;

  return 0;
}

template <class BASE, class MATERIALSERVER>
const MATERIALSERVER *
CMaterialServerParent<BASE, MATERIALSERVER>::ConnectedMaterial(const CDepletionStage &stage) const {
  typename TMaterialMap::const_iterator it = m_mpMaterial.find(&stage);
  if (it != m_mpMaterial.end())
    return it->second;

  return 0;
}

template <class BASE, class MATERIALSERVER>
bool CMaterialServerParent<BASE, MATERIALSERVER>::CanConnectItem(const CGraphNode &item) const {
  const IPointSet *pPointSet = dynamic_cast<const IPointSet *>(&item);
  if (pPointSet) {
    if ((static_cast<const CModelBase &>(BASE::Model())).BranchState().IsBranch())
      return false;

    for (int i = 0; i < pPointSet->ValueSetSize(); i++) {
      if (pPointSet->ValueSet(i).Component()) {
        const CModelBase &model = static_cast<const CModelBase &>(BASE::Model());
        const IValueComposite &parent = pPointSet->ValueSet(i).Component()->Parent();
        int idxParentStage = model.DepletionStageEntry().StageIndexFromName(parent.Name().toStdString().c_str());

        // material?
        if (idxParentStage > 0) {
          const MATERIALSERVER *pMatServer =
              ConnectedMaterial(model.DepletionStageEntry().StageByIndex(idxParentStage));
          if (pMatServer && pMatServer->CanConnectItem(parent))
            return true;
        } else {
          const MATERIALSERVER &iniMatServer = Material(model.InitialDepletionStage());
          if (iniMatServer.CanConnectItem(parent))
            return true;
        }
      }
    }
  }

  return BASE::CanConnectItem(item);
}

template <class BASE, class MATERIALSERVER>
bool CMaterialServerParent<BASE, MATERIALSERVER>::ConnectItem(const CGraphNode &item) {
  const IPointSet *pPointSet = dynamic_cast<const IPointSet *>(&item);
  if (pPointSet) {
    assert(CanConnectItem(item));
    for (int i = 0; i < pPointSet->ValueSetSize(); i++) {
      if (pPointSet->ValueSet(i).Component()) {
        const IValueComposite &parent = pPointSet->ValueSet(i).Component()->Parent();
        const CModelBase &model = static_cast<const CModelBase &>(BASE::Model());
        int idxParentStage = model.DepletionStageEntry().StageIndexFromName(parent.Name().toStdString().c_str());
        if (idxParentStage >= 0) {
          const CDepletionStage &stage = model.DepletionStageEntry().StageByIndex(idxParentStage);

          MATERIALSERVER *pMatServer = ConnectedMaterial(stage);
          if (pMatServer) {
            if (pMatServer->IsLinkedTo(parent))
              continue; // nothing to be done...

            if (pMatServer->CanConnectItem(parent)) {
              pMatServer->ConnectItem(parent);
              continue;
            }
          }
        } else {
          // try initial material (no valid depletion stage suffix)
          MATERIALSERVER &iniMatServer = Material(model.InitialDepletionStage());
          if (iniMatServer.CanConnectItem(parent))
            iniMatServer.ConnectItem(parent);
        }
      }
    }

    return true;
  }

  return BASE::ConnectItem(item);
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  BASE::SaveStream(stream, progress);

  CModelBase &model = static_cast<CModelBase &>(BASE::Model());
  CDepletionStageEntry::iterator it;
  for (it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
    SaveMaterial(*it, stream, progress);
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::LoadStream(TSTREAM &stream, CStreamVersion &version,
                                                             TPROGRESS &progress) {
  BASE::LoadStream(stream, version, progress);

  if (version >= CStreamVersion(3, 7, 9)) {
    CModelBase &model = static_cast<CModelBase &>(BASE::Model());
    CDepletionStageEntry::iterator it;
    for (it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
      LoadMaterial(*it, stream, version, progress);
  }
}

template <class BASE, class MATERIALSERVER> long CMaterialServerParent<BASE, MATERIALSERVER>::SavedItems() const {
  long lRet = BASE::SavedItems();

  const CModelBase &model = static_cast<const CModelBase &>(BASE::Model());
  CDepletionStageEntry::const_iterator it;
  for (it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
    lRet += SavedMaterialItems(*it);

  return lRet;
}

template <class BASE, class MATERIALSERVER> unsigned int CMaterialServerParent<BASE, MATERIALSERVER>::IconId() const {
  assert(false);

  return 0;
}

template <class BASE, class MATERIALSERVER> unsigned int CMaterialServerParent<BASE, MATERIALSERVER>::TypeId() const {
  assert(false);

  return 0;
}

template <class BASE, class MATERIALSERVER> int CMaterialServerParent<BASE, MATERIALSERVER>::DisplayListSize() const {
  assert(false);

  return 0;
}

template <class BASE, class MATERIALSERVER>
const geo::IObject &CMaterialServerParent<BASE, MATERIALSERVER>::DisplayList(int /*nIndex*/) const {
  assert(false);

  // Use any from IObject derived class that is not abstract!

  geo::CPoint *point = new geo::CPoint();

  return *point;
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::SaveMaterial(CDepletionStage &stage, TSTREAM &stream,
                                                               TPROGRESS &progress) {
  MATERIALSERVER *pMatServer = ConnectedMaterial(stage);
  if (pMatServer)
    pMatServer->SaveStream(stream, progress);
}

template <class BASE, class MATERIALSERVER>
void CMaterialServerParent<BASE, MATERIALSERVER>::LoadMaterial(CDepletionStage &stage, TSTREAM &stream,
                                                               CStreamVersion &version, TPROGRESS &progress) {
  if (HasMaterial(stage)) {
    // create material server for this stage
    MATERIALSERVER *pMatServer = new MATERIALSERVER(*this, stage);

    if (version > CStreamVersion(3, 0, 100))
      pMatServer->LoadStream(stream, version, progress);
  }
}

template <class BASE, class MATERIALSERVER>
long CMaterialServerParent<BASE, MATERIALSERVER>::SavedMaterialItems(const CDepletionStage &stage) const {
  if (HasMaterial(stage))
    return Material(stage).SavedItems();

  return 0;
}

template <class BASE, class MATERIALSERVER>
bool CMaterialServerParent<BASE, MATERIALSERVER>::HasMaterial(const CDepletionStage &stage) const {
  return (stage.Initial() || stage.Previous().OutputType() == CDepletionStage::BRANCH ||
          stage.Previous().OutputType() == CDepletionStage::PHASE);
}

#endif // _MATERIALSERVERPARENT_H_
