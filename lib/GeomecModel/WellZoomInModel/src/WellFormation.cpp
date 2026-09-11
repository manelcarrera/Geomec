
#include "WellFormation.h"
#include "IWellModel.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "Temperature.h"
#include "WellModelEntryTypes.h"

CWellFormation::CWellFormation(const CFormationBase &parent, unsigned int /*nEntryType*/, IWellModel &model)
    : C3DFormation(parent.Name().toStdString().c_str(), model) {
  m_pParent = &parent;

  create(model.GraphEntry(MD_WELLMODEL_FORMATION));
  Color(parent.Color());

  DuplicateParentMaterials();
  DuplicateLoads();
}

CWellFormation::CWellFormation(CFemAppModel &model) : C3DFormation(model) {}

bool CWellFormation::ShowCenterPoints() const {
  const CWellFormationEntry &entry =
      static_cast<const CWellFormationEntry &>(*Model().GraphEntry(MD_WELLMODEL_FORMATION));
  return entry.ShowMidpoints();
}

bool CWellFormation::Less(const CGraphNode &node) const {
  const CWellFormation *pFor = dynamic_cast<const CWellFormation *>(&node);
  if (pFor) {
    return ParentFormation()->Less(*pFor->ParentFormation());
  }

  return C3DFormation::Less(node);
}

void CWellFormation::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  C3DFormation::LoadStream(stream, version, progress);
  int nIndex;
  stream >> nIndex;
  IWellModel &model = static_cast<IWellModel &>(Model());
  TFormationBaseEntry &form_entry = (TFormationBaseEntry &)(*model.ParentModel().GraphEntry(MD_BASE_FORMATION));
  m_pParent = form_entry.FindIndex(nIndex);
  assert(m_pParent);

  Color(m_pParent->Color());

  //  DuplicateParentMaterials();
  //  DuplicateLoads();

  progress.Step();

  assert(Model().GraphEntry(MD_WELLMODEL_FORMATION));
  reParent(Model().GraphEntry(MD_WELLMODEL_FORMATION));
}

void CWellFormation::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  C3DFormation::SaveStream(stream, progress);
  stream << m_pParent->Index();
  progress.Step();
}

long CWellFormation::SavedItems() const { return C3DFormation::SavedItems() + 1; }

void CWellFormation::FindParentFormation() {
  // Overrides base, doing nothing: we receive parent information upon construction/load
}

void CWellFormation::LoadMaterial(CDepletionStage &stage, TSTREAM &stream, CStreamVersion &version,
                                  TPROGRESS &progress) {
  if (version < CStreamVersion(4, 0, 2)) {
    if (HasMaterial(stage)) {
      // create material server for this stage
      CMaterialServer *pMatServer = new CMaterialServer(*this, stage);
      int nHasMat;
      stream >> nHasMat;
      if (nHasMat != 0) {
        IWellModel &model = static_cast<IWellModel &>(Model());
        CMaterialEntry &parentMatEntry = (CMaterialEntry &)(*model.RootModel().GraphEntry(MD_ROCK_MATERIAL));
        int idx;
        stream >> idx;
        IMaterial *pMat = parentMatEntry.FindIndex(idx);
        assert(pMat);
        assert(dynamic_cast<IMaterialRock *>(pMat));
        pMatServer->LinkTo(*pMat);
      }
      progress.Step();
    }
  } else {
    C3DFormation::LoadMaterial(stage, stream, version, progress);
  }
}

void CWellFormation::DuplicateParentMaterials() {
  assert(m_pParent);

  CModelBase &mymodel = static_cast<CModelBase &>(Model());
  const CModelBase &parentmodel = static_cast<const CModelBase &>(m_pParent->Model());

  CDepletionStage *pMyStage = &mymodel.InitialDepletionStage();
  const CDepletionStage *pParentStage = &parentmodel.InitialDepletionStage();

  while (pMyStage && pParentStage) {
    CMaterialServer *pMyMatServer = ConnectedMaterial(*pMyStage);
    if (pMyMatServer) {
      // fetch material data from parent formation
      const CMaterialServer &parentMatServer = m_pParent->Material(*pParentStage);
      const IMaterialRock *pParentMat = parentMatServer.LibraryMaterial();
      if (pParentMat) {
        (const_cast<IMaterialRock *>(pParentMat))->LinkTo(*pMyMatServer);
        const CMaterialServer::TValueTypePairVec &vcVT = parentMatServer.ValueTypePairVec();
        for (size_t i = 0; i < vcVT.size(); ++i) {
          pMyMatServer->LinkTo(const_cast<CValueType &>(*vcVT[i].first));
          std::pair<CMaterialServer::TValueTypeSet::iterator, bool> result =
              pMyMatServer->getExtrapolatingValueTypes().insert(vcVT[i].first);
          assert(result.second);
        }
      }
    }

    if (pMyStage->Last() || pParentStage->Last()) {
      pMyStage = 0;
      pParentStage = 0;
    } else {
      pMyStage = &pMyStage->Next();
      pParentStage = &pParentStage->Next();
    }
  }
}

void CWellFormation::DuplicateLoads() {
  assert(m_pParent);

  CModelBase &mymodel = static_cast<CModelBase &>(Model());
  const CModelBase &parentmodel = static_cast<const CModelBase &>(m_pParent->Model());

  CDepletionStage *pMyStage = &mymodel.InitialDepletionStage();
  const CDepletionStage *pParentStage = &parentmodel.InitialDepletionStage();

  while (pMyStage && pParentStage) {
    const CPressure &parentPressure = m_pParent->Pressure(*pParentStage);
    CPressure &myPressure = Pressure(*pMyStage);
    myPressure.Type(parentPressure.Type());
    myPressure.Constant().Set(parentPressure.Constant().ReferenceValue().Value(),
                              parentPressure.Constant().ReferenceDepth().Value(),
                              parentPressure.Constant().Gradient().Value());
    myPressure.GWC().Set(parentPressure.GWC().ContactPressure().Value(),
                         parentPressure.GWC().TransitionPressure().Value(), parentPressure.GWC().ContactDepth().Value(),
                         parentPressure.GWC().TransitionDepth().Value(), parentPressure.GWC().UpperGradient().Value(),
                         parentPressure.GWC().LowerGradient().Value(), parentPressure.GWC().TransitionFromPrevious());

    for (int i = 0; i < parentPressure.DistributedSize(); ++i) {
      const TPressure &distripressure = parentPressure.DistributedValue(i);
      myPressure.LinkTo(const_cast<TPressure &>(distripressure));
    }

    const CTemperature &parentTemp = m_pParent->UserTemperature(*pParentStage);
    CTemperature &myTemp = UserTemperature(*pMyStage);
    myTemp.Type(parentTemp.Type());
    myTemp.Constant().Set(parentTemp.Constant().ReferenceValue().Value(),
                          parentTemp.Constant().ReferenceDepth().Value(), parentTemp.Constant().Gradient().Value());

    for (int i = 0; i < parentTemp.DistributedSize(); ++i) {
      const TTemperature &distritemp = parentTemp.DistributedValue(i);
      myTemp.LinkTo(const_cast<TTemperature &>(distritemp));
    }

    if (!pMyStage->Initial()) {
      const CStrainLoad &parentStrain = m_pParent->Strain(*pParentStage);
      CStrainLoad &myStrain = Strain(*pMyStage);
      myStrain.Type(parentStrain.Type());
      myStrain.Constant().Set(parentStrain.Constant().ReferenceValue().Value(),
                              parentStrain.Constant().ReferenceDepth().Value(),
                              parentStrain.Constant().Gradient().Value());
      myStrain.Constant().UseVolumetric(parentStrain.Constant().UseVolumetric());
      myStrain.Constant().UseInPlane(parentStrain.Constant().UseInPlane());
      myStrain.Constant().ReferenceValueNormal(parentStrain.Constant().ReferenceValueNormal().Value());
      myStrain.Constant().ReferenceValueLateral(parentStrain.Constant().ReferenceValueLateral().Value());

      for (int i = 0; i < parentStrain.DistributedSize(); ++i) {
        const CGraphNode *distriStrain = &parentStrain.DistributedValue(i);
        myStrain.LinkTo(*const_cast<CGraphNode *>(distriStrain));
      }
    }

    if (pMyStage->Last() || pParentStage->Last()) {
      pMyStage = 0;
      pParentStage = 0;
    } else {
      pMyStage = &pMyStage->Next();
      pParentStage = &pParentStage->Next();
    }
  }
}

/////

CWellFormationEntry::CWellFormationEntry(int nEntryId, IWellModel &model)
    : CFormationEntryTempl<CWellFormation>::CFormationEntryTempl(nEntryId, model) {}

IMaterialRock &CWellFormationEntry::CloneMaterial(const IMaterialRock &parentmat) {
  TMaterialMap::iterator it = m_mpMaterials.find(&parentmat);
  if (it == m_mpMaterials.end()) {
    // create a copy in this model
    CModelBase &mymodel = static_cast<CModelBase &>(Model());
    CMaterialEntry &myMatEntry = static_cast<CMaterialEntry &>(*mymodel.GraphEntry(MD_ROCK_MATERIAL));
    IMaterial *pMatCopy = &myMatEntry.CreateMaterial(parentmat.MaterialModel(), parentmat.Name());
    pMatCopy->CloneValues(parentmat);
    assert(dynamic_cast<IMaterialRock *>(pMatCopy));
    it = m_mpMaterials.insert(TMaterialMap::value_type(&parentmat, static_cast<IMaterialRock *>(pMatCopy))).first;
  }

  IMaterialRock *pMyMat = it->second;
  return *pMyMat;
}
