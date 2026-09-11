#include <time.h>

#include "BaseEntryTypes.h"
#include "DepletionStage.h"
#include "DerivedResult.h"
#include "ElementSet.h"
#include "ExportResultData.h"
#include "FemAppEntryTypes.h"
#include "FemAppModel.h"
#include "FormationBase.h"
#include "FormationPlane.h"
#include "GVTResult.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HorizonBase.h"
#include "HotSpot.h"
#include "IColorScaleEntry.h"
#include "LocalResult.h"
#include "MaterialResultTree.h"
#include "MeshResultTree.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "NonMeshedSurface.h"
#include "PointSet.h"
#include "ResultTree.h"
#include "ValueTypes.h"
#include "ivaluecomposite.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

namespace {

void loadBoolean(bool &boolean, CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                 CStorageNode::TPROGRESS & /*progress*/) {
  int isBoolean = 0;

  stream >> isBoolean;

  boolean = (isBoolean == 1);
}

void saveBoolean(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/, bool boolean) {
  stream << (int)(boolean ? 1 : 0);
}

void loadHotSpot(const CHotSpot *hotSpot, CFemAppModel &model, CStorageNode::TSTREAM &stream,
                 CStreamVersion & /*version*/, CStorageNode::TPROGRESS & /*progress*/) {
  int boolean = 0;

  stream >> boolean;

  if (boolean == 1) {
    int index = 0;

    stream >> index;

    IColorScaleEntry *colorScaleEntry = dynamic_cast<IColorScaleEntry *>(model.GraphEntry(MD_BASE_COLOR_SCALE));

    hotSpot = (CHotSpot *)colorScaleEntry->FindIndex(index);
  }
}

void saveHotSpot(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/, const CHotSpot *hotSpot) {
  stream << (int)(hotSpot != 0 ? 1 : 0);

  if (hotSpot != 0) {
    stream << hotSpot->Index();
  }
}

void loadTimeStep(const CDepletionStage **timeStep, CFemAppModel &model, CStorageNode::TSTREAM &stream,
                  CStreamVersion & /*version*/, CStorageNode::TPROGRESS & /*progress*/) {
  int boolean = 0;

  stream >> boolean;

  if (boolean == 1) {
    int index = 0;

    stream >> index;

    CDepletionStageEntry *depletionStageEntry =
        dynamic_cast<CDepletionStageEntry *>(model.GraphEntry(MD_BASE_DEPLETION_STAGE));

    *timeStep = (CDepletionStage *)depletionStageEntry->FindIndex(index);
  }
}

void saveTimeStep(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/,
                  const CDepletionStage *timeStep) {
  stream << (int)(timeStep != 0 ? 1 : 0);

  if (timeStep != 0) {
    stream << timeStep->Index();
  }
}

void loadTimeStep(CExportResultData::TTimeStepSet &timeStepSet, CFemAppModel &model, CStorageNode::TSTREAM &stream,
                  CStreamVersion & /*version*/, CStorageNode::TPROGRESS & /*progress*/) {
  CDepletionStageEntry *depletionStageEntry =
      dynamic_cast<CDepletionStageEntry *>(model.GraphEntry(MD_BASE_DEPLETION_STAGE));
  int index = 0;
  unsigned long size = 0;

  stream >> size;

  for (unsigned long s = 0; s < size; ++s) {
    stream >> index;

    const CDepletionStage *depletionStage = (CDepletionStage *)depletionStageEntry->FindIndex(index);

    timeStepSet.insert(depletionStage);
  }
}

void saveTimeStep(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/,
                  const CExportResultData::TTimeStepSet &timeStepSet) {
  stream << (unsigned long)timeStepSet.size();

  for (CExportResultData::TTimeStepSet::const_iterator iterator = timeStepSet.begin(); iterator != timeStepSet.end();
       ++iterator) {
    const CDepletionStage *depletionStage = *iterator;

    stream << depletionStage->Index();
  }
}

void loadResultComponentSet(TResultComponentSet &resultComponentSet, CFemAppModel &model, CStorageNode::TSTREAM &stream,
                            CStreamVersion & /*version*/, CStorageNode::TPROGRESS & /*progress*/) {
  TValueCompositeEntry *valueCompositeEntry = (TValueCompositeEntry *)model.GraphEntry(MD_BASE_VALUE_COMPOSITE);
  unsigned long size = 0;

  stream >> size;

  for (unsigned long s = 0; s < size; ++s) {
    int valueCompositeIndex = 0, index = 0;

    stream >> valueCompositeIndex;
    stream >> index;

    const IValueComposite *valueComposite = valueCompositeEntry->FindIndex(valueCompositeIndex);

    resultComponentSet.insert(std::make_pair(valueComposite, index));
  }
}

void saveResultComponentSet(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/,
                            const TResultComponentSet &resultComponentSet) {
  stream << (unsigned long)resultComponentSet.size();

  for (TResultComponentSet::const_iterator iterator = resultComponentSet.begin(); iterator != resultComponentSet.end();
       ++iterator) {
    TResultComponent resultComponent = *iterator;
    const IValueComposite *valueComposite = resultComponent.first;
    int index = resultComponent.second;
    int valueCompositeIndex = valueComposite->Index();

    stream << valueCompositeIndex;
    stream << index;
  }
}

void fillResultComponentSet(TResultComponentSet &resultComponentSet, const CResultGroup &tree,
                            unsigned int valueCompositeIndex, int index) {
  for (int child = 0; child < tree.ChildSize(); ++child) {
    if (dynamic_cast<const CMaterialResult *>(&tree.Child(child)) == 0) {
      fillResultComponentSet(resultComponentSet, dynamic_cast<const CResultGroup &>(tree.Child(child)),
                             valueCompositeIndex, index);
    } else {
      if (tree.Child(child).TypeId() == valueCompositeIndex) {
        resultComponentSet.insert(std::make_pair(&(dynamic_cast<const CMaterialResult &>(tree.Child(child))), index));
        break;
      }
    }
  }
}

void loadMaterialResultComponentSet(TResultComponentSet &resultComponentSet, CFemAppModel &model,
                                    CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                    CStorageNode::TPROGRESS & /*progress*/) {
  CModelBase &modelBase = dynamic_cast<CModelBase &>(model);
  unsigned long size = 0;

  stream >> size;

  for (unsigned long s = 0; s < size; ++s) {
    unsigned int valueCompositeIndex = 0;
    int index = 0;

    stream >> valueCompositeIndex;
    stream >> index;

    fillResultComponentSet(resultComponentSet, modelBase.MaterialResultTree(), valueCompositeIndex, index);
  }
}

void saveMaterialResultComponentSet(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/,
                                    const TResultComponentSet &resultComponentSet) {
  stream << (unsigned long)resultComponentSet.size();

  for (TResultComponentSet::const_iterator iterator = resultComponentSet.begin(); iterator != resultComponentSet.end();
       ++iterator) {
    TResultComponent resultComponent = *iterator;
    const IValueComposite *valueComposite = resultComponent.first;
    unsigned int valueCompositeIndex = valueComposite->TypeId();
    int index = resultComponent.second;

    stream << valueCompositeIndex;
    stream << index;
  }
}

} // anonymous namespace

CExportResultDataSettings::CExportResultDataSettings()
    : m_folder(), m_file(), m_export(), m_nUnit(), m_nTZ(), m_bLinear(), m_bNonLinear(), m_bHeat(), m_bMixture(),
      m_bMixtureContainment(), m_nCenterPoints(), m_nWellPoints(), m_nFormationNames(), m_nWeightingType(),
      m_bReservoirMacro() {}

void CExportResultDataSettings::bind(CExportResultData &exportResultData) {
  exportResultData.m_folder = m_folder;
  exportResultData.m_file = m_file;
  exportResultData.m_export = m_export;

  exportResultData.m_nUnit = m_nUnit;
  exportResultData.m_nTZ = m_nTZ;

  exportResultData.m_bLinear = m_bLinear;
  exportResultData.m_bNonLinear = m_bNonLinear;
  exportResultData.m_bHeat = m_bHeat;
  exportResultData.m_bMixture = m_bMixture;
  exportResultData.m_bMixtureContainment = m_bMixtureContainment;

  exportResultData.m_nCenterPoints = m_nCenterPoints;
  exportResultData.m_nWellPoints = m_nWellPoints;
  exportResultData.m_nFormationNames = m_nFormationNames;
  exportResultData.m_nWeightingType = m_nWeightingType;
  exportResultData.m_bReservoirMacro = m_bReservoirMacro;
}

void CExportResultDataSettings::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_folder = exportResultData.m_folder;
    m_file = exportResultData.m_file;
    m_export = exportResultData.m_export;

    m_nUnit = exportResultData.m_nUnit;
    m_nTZ = exportResultData.m_nTZ;

    m_bLinear = exportResultData.m_bLinear;
    m_bNonLinear = exportResultData.m_bNonLinear;
    m_bHeat = exportResultData.m_bHeat;
    m_bMixture = exportResultData.m_bMixture;
    m_bMixtureContainment = exportResultData.m_bMixtureContainment;

    m_nCenterPoints = exportResultData.m_nCenterPoints;
    m_nWellPoints = exportResultData.m_nWellPoints;
    m_nFormationNames = exportResultData.m_nFormationNames;
    m_nWeightingType = exportResultData.m_nWeightingType;
    m_bReservoirMacro = exportResultData.m_bReservoirMacro;
  }
}

void CExportResultDataSettings::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                           CStorageNode::TPROGRESS &progress) {
  stream >> m_folder;
  stream >> m_file;
  loadBoolean(m_export, stream, version, progress);
  stream >> m_nUnit;
  stream >> m_nTZ;
  loadBoolean(m_bLinear, stream, version, progress);
  loadBoolean(m_bNonLinear, stream, version, progress);
  loadBoolean(m_bHeat, stream, version, progress);
  loadBoolean(m_bMixture, stream, version, progress);
  loadBoolean(m_bMixtureContainment, stream, version, progress);
  stream >> m_nCenterPoints;
  stream >> m_nWellPoints;
  stream >> m_nFormationNames;
  stream >> m_nWeightingType;
  if (version > CStreamVersion(4, 1, 64))
    loadBoolean(m_bReservoirMacro, stream, version, progress);
}

void CExportResultDataSettings::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << m_folder;
  stream << m_file;
  saveBoolean(stream, progress, m_export);
  stream << m_nUnit;
  stream << m_nTZ;
  saveBoolean(stream, progress, m_bLinear);
  saveBoolean(stream, progress, m_bNonLinear);
  saveBoolean(stream, progress, m_bHeat);
  saveBoolean(stream, progress, m_bMixture);
  saveBoolean(stream, progress, m_bMixtureContainment);
  stream << m_nCenterPoints;
  stream << m_nWellPoints;
  stream << m_nFormationNames;
  stream << m_nWeightingType;
  saveBoolean(stream, progress, m_bReservoirMacro);
}

CExportResultDataDepletionStageFilter::CExportResultDataDepletionStageFilter() : m_nType(NONE), m_nSetValue(INITIAL) {}

void CExportResultDataDepletionStageFilter::bind(CExportResultData &exportResultData) {
  CDepletionStage *initStage =
      &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().InitialStage();
  CDepletionStage *lastStage = &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().LastStage();

  switch (m_nType) {
  case SET:
    switch (m_nSetValue) {
    case INITIAL:
      exportResultData.m_stTimeStep.insert(initStage);
      break;
    case LAST:
      exportResultData.m_stTimeStep.insert(lastStage);
      break;
    case ALL: {
      CDepletionStage *current = initStage;
      while (current) {
        exportResultData.m_stTimeStep.insert(current);

        current = current->Last() ? 0 : &current->Next();
      }
    } break;
    }
    break;
  case BYTYPE: {
    CDepletionStage *current = initStage;
    while (current) {
      if (current->OutputType() == m_nTypeValue)
        exportResultData.m_stTimeStep.insert(current);

      current = current->Last() ? 0 : &current->Next();
    }
  } break;
  case BYNAME: {
    CDepletionStage *current = initStage;
    while (current) {
      if (current->Name() == m_sName) {
        exportResultData.m_stTimeStep.insert(current);
        break;
      }

      current = current->Last() ? 0 : &current->Next();
    }
  } break;
  }
}

void CExportResultDataDepletionStageFilter::unbind(CExportResultData & /*exportResultData*/) {
  // not called
}

void CExportResultDataDepletionStageFilter::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                                       CStorageNode::TPROGRESS & /*progress*/) {
  int temp;
  stream >> temp;
  m_nType = (TType)temp;

  switch (m_nType) {
  case SET:
    stream >> temp;
    m_nSetValue = (TSetValue)temp;
    break;
  case BYTYPE:
    stream >> temp;
    m_nTypeValue = (CDepletionStage::eOutputType)temp;
    break;
  case BYNAME:
    stream >> m_sName;
    break;
  }
}

void CExportResultDataDepletionStageFilter::SaveStream(CStorageNode::TSTREAM &stream,
                                                       CStorageNode::TPROGRESS & /*progress*/) {
  stream << (int)m_nType;

  switch (m_nType) {
  case SET:
    stream << (int)m_nSetValue;
    break;
  case BYTYPE:
    stream << (int)m_nTypeValue;
    break;
  case BYNAME:
    stream << m_sName;
    break;
  }
}

CExportResultDataDepletionStageFilterVector::CExportResultDataDepletionStageFilterVector() {}

void CExportResultDataDepletionStageFilterVector::bind(CExportResultData &exportResultData) {
  int size = (int)m_depletionStageFilters.size();
  for (int i = 0; i < size; ++i)
    m_depletionStageFilters[i].bind(exportResultData);
}

void CExportResultDataDepletionStageFilterVector::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_depletionStageFilters.clear();

    if (!exportResultData.m_stTimeStep.empty()) {
      CDepletionStage *initStage =
          &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().InitialStage();
      CDepletionStage *lastStage =
          &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().LastStage();

      // we're going to try to generalize the selection, so that it may be applicable to other models
      int nTotal = static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().EntryNodes().size();
      int nCovered = 0;

      bool bCoveredFirst = false;
      bool bCoveredLast = false;
      bool bCoveredResults = false;
      bool bCoveredBranches = false;
      bool bCoveredPhases = false;

      if (exportResultData.m_stTimeStep.size() == nTotal) {
        CExportResultDataDepletionStageFilter filter;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataDepletionStageFilter::ALL;

        m_depletionStageFilters.push_back(filter);

        nCovered = nTotal;
      }

      if (nCovered < exportResultData.m_stTimeStep.size()) {
        if (exportResultData.m_stTimeStep.find(initStage) != exportResultData.m_stTimeStep.end()) {
          CExportResultDataDepletionStageFilter filter;
          filter.m_nType = SET;
          filter.m_nSetValue = CExportResultDataDepletionStageFilter::INITIAL;

          m_depletionStageFilters.push_back(filter);

          ++nCovered;
          bCoveredFirst = true;
        }

        if (exportResultData.m_stTimeStep.find(lastStage) != exportResultData.m_stTimeStep.end()) {
          CExportResultDataDepletionStageFilter filter;
          filter.m_nType = SET;
          filter.m_nSetValue = CExportResultDataDepletionStageFilter::LAST;

          m_depletionStageFilters.push_back(filter);

          ++nCovered;
          bCoveredLast = true;
        }
      }

      if (nCovered < exportResultData.m_stTimeStep.size()) {
        int nResultsSelected = 0;
        int nResultsTotal = 0;
        int nBranchesSelected = 0;
        int nBranchesTotal = 0;
        int nPhasesSelected = 0;
        int nPhasesTotal = 0;

        for (CExportResultData::TTimeStepSet::iterator it = exportResultData.m_stTimeStep.begin();
             it != exportResultData.m_stTimeStep.end(); ++it) {
          switch ((*it)->OutputType()) {
          case CDepletionStage::RESULTS:
            ++nResultsSelected;
            break;
          case CDepletionStage::BRANCH:
            ++nBranchesSelected;
            break;
          case CDepletionStage::PHASE:
            ++nPhasesSelected;
            break;
          }
        }

        CDepletionStage *current = initStage;
        while (current) {
          switch (current->OutputType()) {
          case CDepletionStage::RESULTS:
            ++nResultsTotal;
            break;
          case CDepletionStage::BRANCH:
            ++nBranchesTotal;
            break;
          case CDepletionStage::PHASE:
            ++nPhasesTotal;
            break;
          }
          current = current->Last() ? 0 : &current->Next();
        }

        if (nResultsTotal > 0 && nResultsTotal == nResultsSelected) {
          CExportResultDataDepletionStageFilter filter;
          filter.m_nType = BYTYPE;
          filter.m_nTypeValue = CDepletionStage::RESULTS;

          m_depletionStageFilters.push_back(filter);

          nCovered += nResultsSelected;
          if (initStage->OutputType() == CDepletionStage::RESULTS)
            --nCovered;
          if (lastStage->OutputType() == CDepletionStage::RESULTS)
            --nCovered;

          bCoveredResults = true;
        }

        if (nBranchesTotal > 0 && nBranchesTotal == nBranchesSelected) {
          CExportResultDataDepletionStageFilter filter;
          filter.m_nType = BYTYPE;
          filter.m_nTypeValue = CDepletionStage::BRANCH;

          m_depletionStageFilters.push_back(filter);

          nCovered += nBranchesSelected;
          if (initStage->OutputType() == CDepletionStage::BRANCH)
            --nCovered;
          if (lastStage->OutputType() == CDepletionStage::BRANCH)
            --nCovered;

          bCoveredBranches = true;
        }

        if (nPhasesTotal > 0 && nPhasesTotal == nPhasesSelected) {
          CExportResultDataDepletionStageFilter filter;
          filter.m_nType = BYTYPE;
          filter.m_nTypeValue = CDepletionStage::PHASE;

          m_depletionStageFilters.push_back(filter);

          nCovered += nPhasesSelected;
          if (initStage->OutputType() == CDepletionStage::PHASE)
            --nCovered;
          if (lastStage->OutputType() == CDepletionStage::PHASE)
            --nCovered;

          bCoveredPhases = true;
        }
      }

      if (nCovered < exportResultData.m_stTimeStep.size()) {
        CDepletionStage *current = initStage;
        while (current) {
          if (exportResultData.m_stTimeStep.find(current) != exportResultData.m_stTimeStep.end()) {
            bool bSkip = false;

            switch (current->OutputType()) {
            case CDepletionStage::RESULTS:
              if (bCoveredResults)
                bSkip = true;
              break;
            case CDepletionStage::BRANCH:
              if (bCoveredBranches)
                bSkip = true;
              break;
            case CDepletionStage::PHASE:
              if (bCoveredPhases)
                bSkip = true;
              break;
            }

            if (bCoveredFirst && current == initStage)
              bSkip = true;
            if (bCoveredLast && current == lastStage)
              bSkip = true;

            if (!bSkip) {
              CExportResultDataDepletionStageFilter filter;
              filter.m_nType = BYNAME;
              filter.m_sName = current->Name();

              m_depletionStageFilters.push_back(filter);
            }
          }

          current = current->Last() ? 0 : &current->Next();
        }
      }
    }
  }

  exportResultData.m_stTimeStep.clear();
}

void CExportResultDataDepletionStageFilterVector::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                                             CStorageNode::TPROGRESS &progress) {
  int size;
  stream >> size;
  m_depletionStageFilters.resize(size);
  for (int i = 0; i < size; ++i)
    m_depletionStageFilters[i].LoadStream(stream, version, progress);
}

void CExportResultDataDepletionStageFilterVector::SaveStream(CStorageNode::TSTREAM &stream,
                                                             CStorageNode::TPROGRESS &progress) {
  int size = (int)m_depletionStageFilters.size();
  stream << size;
  for (int i = 0; i < size; ++i)
    m_depletionStageFilters[i].SaveStream(stream, progress);
}

CExportResultDataModelFilter::CExportResultDataModelFilter()
    : m_nFilter(FAULTS), m_nType(NONE), m_nSetValue(ALL), m_nPlaneMask(PLANE_NONE) {}

void CExportResultDataModelFilter::bind(CExportResultData &exportResultData) {
  switch (m_nFilter) {
  case FAULTS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        THorizonBaseEntry::TNodeSet stNodes =
            static_cast<THorizonBaseEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_HORIZON))
                ->EntryNodes();
        for (THorizonBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          if ((*it)->Slip())
            exportResultData.m_selection.insert(*const_cast<const CHorizonBase *>((*it)));
        }
      }
      break;
    case BYNAME: {
      THorizonBaseEntry::TNodeSet stNodes =
          static_cast<THorizonBaseEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_HORIZON))
              ->EntryNodes();
      for (THorizonBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if ((*it)->Slip() && (*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const CHorizonBase *>((*it)));
      }
    } break;
    }
    break;
  case HORIZONS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        THorizonBaseEntry::TNodeSet stNodes =
            static_cast<THorizonBaseEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_HORIZON))
                ->EntryNodes();
        for (THorizonBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          if (!(*it)->Slip())
            exportResultData.m_selection.insert(*const_cast<const CHorizonBase *>(*it));
        }
      }
      break;
    case BYNAME: {
      THorizonBaseEntry::TNodeSet stNodes =
          static_cast<THorizonBaseEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_HORIZON))
              ->EntryNodes();
      for (THorizonBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if (!(*it)->Slip() && (*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const CHorizonBase *>((*it)));
      }
    } break;
    }
    break;
  case FORMATIONS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        TFormationBaseEntry::TNodeSet stNodes =
            static_cast<TFormationBaseEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_FORMATION))
                ->EntryNodes();
        for (TFormationBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          exportResultData.m_selection.insert(*const_cast<const CFormationBase *>(*it));
        }
      }
      break;
    case BYNAME: {
      TFormationBaseEntry::TNodeSet stNodes =
          static_cast<TFormationBaseEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_FORMATION))
              ->EntryNodes();
      for (TFormationBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if ((*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const CFormationBase *>((*it)));
      }
    } break;
    }
    break;
  case FORMATIONPLANES:
    switch (m_nType) {
    case BYNAME: {
      CHexaFormationEntry *hexaEntry = dynamic_cast<CHexaFormationEntry *>(
          static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_HEXA_FORMATION));
      if (hexaEntry) {
        CHexaFormationEntry::TNodeSet stNodes = hexaEntry->EntryNodes();

        for (CHexaFormationEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          if ((*it)->Name() == m_sName) {
            if (m_nPlaneMask & PLANE_TOP)
              exportResultData.m_selection.insert(*(*it)->FormationPlane(CFormationPlane::FP_TOP).data());
            if (m_nPlaneMask & PLANE_MIDDLE)
              exportResultData.m_selection.insert(*(*it)->FormationPlane(CFormationPlane::FP_MIDDLE).data());
            if (m_nPlaneMask & PLANE_BOTTOM)
              exportResultData.m_selection.insert(*(*it)->FormationPlane(CFormationPlane::FP_BOTTOM).data());
          }
        }
      }
    } break;
    }
    break;
  case WELLPATHS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        CNewWellPathEntry::TNodeSet stNodes =
            static_cast<CNewWellPathEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_NEW_WELLPATH))
                ->EntryNodes();
        for (CNewWellPathEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          exportResultData.m_selection.insert(*const_cast<const CNewWellPath *>(*it));
        }
      }
      break;
    case BYNAME: {
      CNewWellPathEntry::TNodeSet stNodes =
          static_cast<CNewWellPathEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_NEW_WELLPATH))
              ->EntryNodes();
      for (CNewWellPathEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if ((*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const CNewWellPath *>((*it)));
      }
    } break;
    }
    break;
  case POINTSETS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        TPointSetEntry::TNodeSet stNodes =
            static_cast<TPointSetEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_POINTSET))
                ->EntryNodes();
        for (TPointSetEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          if (!dynamic_cast<IElementSet *>(*it))
            exportResultData.m_selection.insert(*const_cast<const IPointSet *>(*it));
        }
      }
      break;
    case BYNAME: {
      TPointSetEntry::TNodeSet stNodes =
          static_cast<TPointSetEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_POINTSET))
              ->EntryNodes();
      for (TPointSetEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if (!dynamic_cast<IElementSet *>(*it) && (*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const IPointSet *>((*it)));
      }
    } break;
    }
    break;
  case ELEMENTSETS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        TPointSetEntry::TNodeSet stNodes =
            static_cast<TPointSetEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_POINTSET))
                ->EntryNodes();
        for (TPointSetEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          if (dynamic_cast<IElementSet *>(*it))
            exportResultData.m_selection.insert(*const_cast<const IPointSet *>(*it));
        }
      }
      break;
    case BYNAME: {
      TPointSetEntry::TNodeSet stNodes =
          static_cast<TPointSetEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_POINTSET))
              ->EntryNodes();
      for (TPointSetEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if (dynamic_cast<IElementSet *>(*it) && (*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const IPointSet *>((*it)));
      }
    } break;
    }
    break;
  case NONMESHEDSURFACES:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        CNonMeshedSurfaceEntry::TNodeSet stNodes =
            static_cast<CNonMeshedSurfaceEntry *>(
                static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_NONMESHEDSURFACE))
                ->EntryNodes();
        for (CNonMeshedSurfaceEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
          exportResultData.m_selection.insert(*const_cast<const CNonMeshedSurface *>(*it));
        }
      }
      break;
    case BYNAME: {
      CNonMeshedSurfaceEntry::TNodeSet stNodes =
          static_cast<CNonMeshedSurfaceEntry *>(
              static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_NONMESHEDSURFACE))
              ->EntryNodes();
      for (CNonMeshedSurfaceEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
        if ((*it)->Name() == m_sName)
          exportResultData.m_selection.insert(*const_cast<const CNonMeshedSurface *>((*it)));
      }
    } break;
    }
    break;
  }
}

void CExportResultDataModelFilter::unbind(CExportResultData & /*exportResultData*/) {}

void CExportResultDataModelFilter::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                              CStorageNode::TPROGRESS & /*progress*/) {
  int temp;
  stream >> temp;
  m_nType = (TType)temp;

  if (m_nType != IExportResultDataFilter::NONE) {
    stream >> temp;
    m_nFilter = (TFilter)temp;
    switch (m_nType) {
    case SET:
      stream >> temp;
      m_nSetValue = (TSetValue)temp;
      break;
    case BYNAME:
      stream >> m_sName;
      break;
    }
    if (m_nFilter == FORMATIONPLANES)
      stream >> m_nPlaneMask;
  }
}

void CExportResultDataModelFilter::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/) {
  stream << (int)m_nType;

  if (m_nType != IExportResultDataFilter::NONE) {
    stream << (int)m_nFilter;
    switch (m_nType) {
    case SET:
      stream << (int)m_nSetValue;
      break;
    case BYNAME:
      stream << m_sName;
      break;
    }
    if (m_nFilter == FORMATIONPLANES)
      stream << m_nPlaneMask;
  }
}

CExportResultDataModelFilterVector::CExportResultDataModelFilterVector() {}

void CExportResultDataModelFilterVector::bind(CExportResultData &exportResultData) {
  int size = (int)m_modelFilters.size();
  for (int i = 0; i < size; ++i)
    m_modelFilters[i].bind(exportResultData);
}

void CExportResultDataModelFilterVector::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_modelFilters.clear();

    THorizonBaseEntry::TNodeSet stTotalHorizons =
        static_cast<THorizonBaseEntry *>(
            static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_HORIZON))
            ->EntryNodes();
    TFormationBaseEntry::TNodeSet stTotalFormations =
        static_cast<TFormationBaseEntry *>(
            static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_FORMATION))
            ->EntryNodes();
    CNewWellPathEntry::TNodeSet stTotalWellpaths =
        static_cast<CNewWellPathEntry *>(
            static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_NEW_WELLPATH))
            ->EntryNodes();
    TPointSetEntry::TNodeSet stTotalPointsets =
        static_cast<TPointSetEntry *>(static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_POINTSET))
            ->EntryNodes();
    CNonMeshedSurfaceEntry::TNodeSet stTotalNonMeshed =
        static_cast<CNonMeshedSurfaceEntry *>(
            static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_NONMESHEDSURFACE))
            ->EntryNodes();

    size_t nTotalFaults = 0;

    for (THorizonBaseEntry::TNodeSet::const_iterator it = stTotalHorizons.begin(); it != stTotalHorizons.end(); ++it) {
      if ((*it)->Slip())
        ++nTotalFaults;
    }

    size_t nTotalElementsets = 0;

    for (TPointSetEntry::TNodeSet::const_iterator it = stTotalPointsets.begin(); it != stTotalPointsets.end(); ++it) {
      if (dynamic_cast<const IElementSet *>(*it))
        ++nTotalElementsets;
    }

    size_t nSelectedHorizons = 0;
    size_t nSelectedFaults = 0;
    size_t nSelectedFormations = 0;
    size_t nSelectedFormationPlanes = 0;
    size_t nSelectedWellpaths = 0;
    size_t nSelectedPointsets = 0;
    size_t nSelectedElementsets = 0;
    size_t nSelectedNonMeshedSurfaces = 0;

    for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
         it != exportResultData.m_selection.end(); ++it) {
      if (dynamic_cast<const CHorizonBase *>(*it)) {
        if (static_cast<const CHorizonBase *>(*it)->Slip())
          ++nSelectedFaults;
        else
          ++nSelectedHorizons;
      } else if (dynamic_cast<const CFormationBase *>(*it))
        ++nSelectedFormations;
      else if (dynamic_cast<const CFormationPlane *>(*it))
        ++nSelectedFormationPlanes;
      else if (dynamic_cast<const CNewWellPath *>(*it))
        ++nSelectedWellpaths;
      else if (dynamic_cast<const IPointSet *>(*it)) {
        if (dynamic_cast<const IElementSet *>(*it))
          ++nSelectedElementsets;
        else
          ++nSelectedPointsets;
      } else if (dynamic_cast<const CNonMeshedSurface *>(*it))
        ++nSelectedNonMeshedSurfaces;
    }

    if (nSelectedFaults > 0) {
      if (nSelectedFaults == nTotalFaults) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::FAULTS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const CHorizonBase *>(*it)) {
            if (static_cast<const CHorizonBase *>(*it)->Slip()) {
              CExportResultDataModelFilter filter;
              filter.m_nFilter = CExportResultDataModelFilter::FAULTS;
              filter.m_nType = BYNAME;
              filter.m_sName = (*it)->Name();

              m_modelFilters.push_back(filter);
            }
          }
        }
      }
    }

    if (nSelectedHorizons > 0) {
      if (nSelectedHorizons == stTotalHorizons.size() - nTotalFaults) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::HORIZONS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const CHorizonBase *>(*it)) {
            if (!static_cast<const CHorizonBase *>(*it)->Slip()) {
              CExportResultDataModelFilter filter;
              filter.m_nFilter = CExportResultDataModelFilter::HORIZONS;
              filter.m_nType = BYNAME;
              filter.m_sName = (*it)->Name();

              m_modelFilters.push_back(filter);
            }
          }
        }
      }
    }

    if (nSelectedFormations > 0) {
      if (nSelectedFormations == stTotalFormations.size()) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::FORMATIONS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const CFormationBase *>(*it)) {
            CExportResultDataModelFilter filter;
            filter.m_nFilter = CExportResultDataModelFilter::FORMATIONS;
            filter.m_nType = BYNAME;
            filter.m_sName = (*it)->Name();

            m_modelFilters.push_back(filter);
          }
        }
      }
    }

    if (nSelectedFormationPlanes > 0) {
      // ByName for now
      TFormationPlaneTypeSet planes;

      for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
           it != exportResultData.m_selection.end(); ++it) {
        if (dynamic_cast<const CFormationPlane *>(*it)) {
          const CFormationPlane *plane = static_cast<const CFormationPlane *>(*it);

          std::pair<TFormationPlaneTypeSet::iterator, bool> retval =
              planes.insert(std::make_pair(&plane->Formation(), TFormationPlaneTypes()));
          retval.first->second.push_back(plane->PlaneType());
        }
      }

      for (TFormationPlaneTypeSet::iterator it = planes.begin(); it != planes.end(); ++it) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::FORMATIONPLANES;
        filter.m_nType = BYNAME;
        filter.m_sName = it->first->Name();

        for (TFormationPlaneTypes::iterator t = it->second.begin(); t != it->second.end(); ++t) {
          switch (*t) {
          case CFormationPlane::FP_BOTTOM:
            filter.m_nPlaneMask |= (int)CExportResultDataModelFilter::PLANE_BOTTOM;
            break;
          case CFormationPlane::FP_MIDDLE:
            filter.m_nPlaneMask |= (int)CExportResultDataModelFilter::PLANE_MIDDLE;
            break;
          case CFormationPlane::FP_TOP:
            filter.m_nPlaneMask |= (int)CExportResultDataModelFilter::PLANE_TOP;
            break;
          }
        }

        m_modelFilters.push_back(filter);
      }
    }

    if (nSelectedWellpaths > 0) {
      if (nSelectedWellpaths == stTotalWellpaths.size()) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::WELLPATHS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const CNewWellPath *>(*it)) {
            CExportResultDataModelFilter filter;
            filter.m_nFilter = CExportResultDataModelFilter::WELLPATHS;
            filter.m_nType = BYNAME;
            filter.m_sName = (*it)->Name();

            m_modelFilters.push_back(filter);
          }
        }
      }
    }

    if (nSelectedPointsets > 0) {
      if (nSelectedPointsets == stTotalPointsets.size() - nTotalElementsets) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::POINTSETS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const IPointSet *>(*it) && !dynamic_cast<const IElementSet *>(*it)) {
            CExportResultDataModelFilter filter;
            filter.m_nFilter = CExportResultDataModelFilter::POINTSETS;
            filter.m_nType = BYNAME;
            filter.m_sName = (*it)->Name();

            m_modelFilters.push_back(filter);
          }
        }
      }
    }

    if (nSelectedElementsets > 0) {
      if (nSelectedElementsets == nTotalElementsets) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::ELEMENTSETS;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const IElementSet *>(*it)) {
            CExportResultDataModelFilter filter;
            filter.m_nFilter = CExportResultDataModelFilter::ELEMENTSETS;
            filter.m_nType = BYNAME;
            filter.m_sName = (*it)->Name();

            m_modelFilters.push_back(filter);
          }
        }
      }
    }

    if (nSelectedNonMeshedSurfaces > 0) {
      if (nSelectedNonMeshedSurfaces == stTotalNonMeshed.size()) {
        CExportResultDataModelFilter filter;
        filter.m_nFilter = CExportResultDataModelFilter::NONMESHEDSURFACES;
        filter.m_nType = SET;
        filter.m_nSetValue = CExportResultDataModelFilter::ALL;

        m_modelFilters.push_back(filter);
      } else {
        for (COpenGLNodeSelection::iterator it = exportResultData.m_selection.begin();
             it != exportResultData.m_selection.end(); ++it) {
          if (dynamic_cast<const CNonMeshedSurface *>(*it)) {
            CExportResultDataModelFilter filter;
            filter.m_nFilter = CExportResultDataModelFilter::NONMESHEDSURFACES;
            filter.m_nType = BYNAME;
            filter.m_sName = (*it)->Name();

            m_modelFilters.push_back(filter);
          }
        }
      }
    }
  }

  exportResultData.m_selection.clear();
}

void CExportResultDataModelFilterVector::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                                    CStorageNode::TPROGRESS &progress) {
  int size;
  stream >> size;
  m_modelFilters.resize(size);
  for (int i = 0; i < size; ++i)
    m_modelFilters[i].LoadStream(stream, version, progress);
}

void CExportResultDataModelFilterVector::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  int size = (int)m_modelFilters.size();
  stream << size;
  for (int i = 0; i < size; ++i)
    m_modelFilters[i].SaveStream(stream, progress);
}

namespace {

struct CollectResults : IGeomecModelVisitor {
  std::vector<std::vector<IValueComponentBase *>> groups;
  std::stack<size_t> index;
  size_t nTotal;

  std::vector<std::vector<IValueComponentBase *>> derivedGroups;
  std::stack<size_t> derivedIndex;

  bool processDerived;

  COpenGLNodeSelection *nodes;

  CollectResults() : nTotal(0), processDerived(false), nodes(0) {}

  virtual bool VisitDefault(CGraphNode & /*node*/) { return false; }
  virtual bool VisitMeshResultTree(CMeshResultTree &tree) {
    index.push(groups.size());
    groups.push_back(std::vector<IValueComponentBase *>());

    for (size_t i = 0; i < tree.childSize(); ++i)
      tree.childAt(i).Accept(*this);

    index.pop();
    return true;
  }
  virtual bool VisitDerivedResult(CDerivedResult &result) {
    processDerived = true;
    for (size_t i = 0; i < result.ComponentSize(); ++i)
      result.Component(i).Accept(*this);
    processDerived = false;
    return true;
  }
  virtual bool VisitDerivedResultGroup(CDerivedResultGroup &group) {
    derivedIndex.push(derivedGroups.size());
    derivedGroups.push_back(std::vector<IValueComponentBase *>());

    for (size_t i = 0; i < group.ChildSize(); ++i)
      group.Child(i).Accept(*this);

    derivedIndex.pop();
    return true;
  }
  virtual bool VisitResultGroup(CResultGroup &group) {
    index.push(groups.size());
    groups.push_back(std::vector<IValueComponentBase *>());

    for (size_t i = 0; i < group.ChildSize(); ++i)
      group.Child(i).Accept(*this);

    index.pop();
    return true;
  }
  virtual bool VisitIValueComposite(IValueComposite &composite) {
    for (size_t i = 0; i < composite.ComponentSize(); ++i)
      composite.Component(i).Accept(*this);
    return true;
  }
  virtual bool VisitIValueComponentBase(IValueComponentBase &component) {
    if (component.Type() == IValueComponentBase::SCALAR) {
      bool bSkip = false;

      if (nodes) {
        bSkip = true;

        for (COpenGLNodeSelection::const_iterator it = nodes->begin(); it != nodes->end(); ++it) {
          if (component.CanMap(**it)) {
            bSkip = false;
            break;
          }
        }
      }
      if (!bSkip) {
        if (processDerived) {
          derivedGroups[derivedIndex.top()].push_back(&component);
        } else {
          groups[index.top()].push_back(&component);
          ++nTotal;
        }
      }
    }
    return true;
  }
};

} // namespace

CExportResultDataResultFilter::CExportResultDataResultFilter()
    : m_nFilter(MATERIALS), m_nType(NONE), m_nSetValue(ALL) {}

void CExportResultDataResultFilter::bind(CExportResultData &exportResultData) {
  if (m_nType == NONE)
    return;

  CollectResults &matResults = static_cast<CollectResults &>(*exportResultData.m_resultFilterVector.m_matResults);
  CollectResults &meshResults = static_cast<CollectResults &>(*exportResultData.m_resultFilterVector.m_meshResults);
  std::vector<IValueComponentBase *> &flatMatResults = *exportResultData.m_resultFilterVector.m_flatMatResults;
  std::vector<const IValueComponentBase *> &flatResults = *exportResultData.m_resultFilterVector.m_flatResults;
  std::vector<const IValueComponentBase *> &flatDerivedResults =
      *exportResultData.m_resultFilterVector.m_flatDerivedResults;
  std::vector<const IValueComponentBase *> &flatGVTResults = *exportResultData.m_resultFilterVector.m_flatGVTResults;
  std::vector<const IValueComponentBase *> &flatGVTDerivedResults =
      *exportResultData.m_resultFilterVector.m_flatGVTDerivedResults;

  switch (m_nFilter) {
  case MATERIALS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL || m_nSetValue == NORMAL) {
        for (std::vector<IValueComponentBase *>::const_iterator it = matResults.groups[0].begin();
             it != matResults.groups[0].end(); ++it) {
          exportResultData.m_MatRGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
        }
      }
      if (m_nSetValue == ALL || m_nSetValue == FRACTUREINTENSITY) {
        for (std::vector<IValueComponentBase *>::const_iterator it = matResults.groups[1].begin();
             it != matResults.groups[1].end(); ++it) {
          exportResultData.m_MatRGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
        }
      }
      break;
    case BYNAME:
      for (std::vector<IValueComponentBase *>::const_iterator it = flatMatResults.begin(); it != flatMatResults.end();
           ++it) {
        if (static_cast<CMaterialResult &>(static_cast<CMaterialResult::CMaterialResultComponent *>(*it)->Parent())
                .StoredExportLabel() == m_sName) {
          exportResultData.m_MatRGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
          break;
        }
      }
      break;
    }
    break;
  case MESH:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        for (std::vector<IValueComponentBase *>::const_iterator it = meshResults.groups[0].begin();
             it != meshResults.groups[0].end(); ++it) {
          exportResultData.m_RGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
        }
      }
      break;
    case BYNAME:
      for (std::vector<IValueComponentBase *>::const_iterator it = meshResults.groups[0].begin();
           it != meshResults.groups[0].end(); ++it) {
        if ((*it)->ExportLabel() == m_sName) {
          exportResultData.m_RGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
          break;
        }
      }
      break;
    }
    break;
  case RESULTS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        for (std::vector<const IValueComponentBase *>::const_iterator it = flatResults.begin(); it != flatResults.end();
             ++it) {
          exportResultData.m_RGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
        }
      }
      break;
    case BYNAME:
      for (std::vector<const IValueComponentBase *>::const_iterator it = flatResults.begin(); it != flatResults.end();
           ++it) {
        if (static_cast<const IResult *>(&(*it)->Parent())->ExportLabel((*it)->ComponentIndex()) == m_sName) {
          exportResultData.m_RGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
          break;
        }
      }
      for (std::vector<const IValueComponentBase *>::const_iterator it = flatDerivedResults.begin();
           it != flatDerivedResults.end(); ++it) {
        if (static_cast<const IResult *>(&(*it)->Parent())->ExportLabel((*it)->ComponentIndex()) == m_sName) {
          exportResultData.m_RGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
          break;
        }
      }
      break;
    }
    break;
  case GVTRESULTS:
    switch (m_nType) {
    case SET:
      if (m_nSetValue == ALL) {
        for (std::vector<const IValueComponentBase *>::const_iterator it = flatGVTResults.begin();
             it != flatGVTResults.end(); ++it) {
          exportResultData.m_GVTRGObserverData.insert(TResultComponent(&(*it)->Parent(), (*it)->ComponentIndex()));
        }
      }
      break;
    case BYNAME:
      for (std::vector<const IValueComponentBase *>::const_iterator it = flatGVTResults.begin();
           it != flatGVTResults.end(); ++it) {
        const IResult *pResult = dynamic_cast<const IResult *>(&(*it)->Parent());
        const CValueType *pValueType = dynamic_cast<const CValueType *>(&(*it)->Parent());

        if (pResult && pResult->ExportLabel((*it)->ComponentIndex()) == m_sName) {
          exportResultData.m_GVTRGObserverData.insert(TResultComponent(pResult, (*it)->ComponentIndex()));
          break;
        }
        if (pValueType && pValueType->ExportLabel() == m_sName) {
          exportResultData.m_GVTRGObserverData.insert(TResultComponent(pValueType, (*it)->ComponentIndex()));
          break;
        }
      }
      for (std::vector<const IValueComponentBase *>::const_iterator it = flatGVTDerivedResults.begin();
           it != flatGVTDerivedResults.end(); ++it) {
        const IResult *pResult = dynamic_cast<const IResult *>(&(*it)->Parent());
        const CValueType *pValueType = dynamic_cast<const CValueType *>(&(*it)->Parent());

        if (pResult && pResult->ExportLabel((*it)->ComponentIndex()) == m_sName) {
          exportResultData.m_GVTRGObserverData.insert(TResultComponent(pResult, (*it)->ComponentIndex()));
          break;
        }
        if (pValueType && pValueType->ExportLabel() == m_sName) {
          exportResultData.m_GVTRGObserverData.insert(TResultComponent(pValueType, (*it)->ComponentIndex()));
          break;
        }
      }
      break;
    }
    break;
  }
}

void CExportResultDataResultFilter::unbind(CExportResultData & /*exportResultData*/) {}

void CExportResultDataResultFilter::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                               CStorageNode::TPROGRESS & /*progress*/) {
  int temp;
  stream >> temp;
  m_nType = (TType)temp;

  if (m_nType != IExportResultDataFilter::NONE) {
    stream >> temp;
    m_nFilter = (TFilter)temp;
    switch (m_nType) {
    case SET:
      stream >> temp;
      m_nSetValue = (TSetValue)temp;
      break;
    case BYNAME:
      stream >> m_sName;
      break;
    }
  }
}

void CExportResultDataResultFilter::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/) {
  stream << (int)m_nType;

  if (m_nType != IExportResultDataFilter::NONE) {
    stream << (int)m_nFilter;
    switch (m_nType) {
    case SET:
      stream << (int)m_nSetValue;
      break;
    case BYNAME:
      stream << m_sName;
      break;
    }
  }
}

CExportResultDataResultFilterVector::CExportResultDataResultFilterVector() {}

void CExportResultDataResultFilterVector::bind(CExportResultData &exportResultData) {
  CollectResults matResults;
  static_cast<CModelBase &>(exportResultData.Model()).MaterialResultTree().Accept(matResults);

  assert(matResults.groups.size() == 2);

  std::vector<IValueComponentBase *> flatMatResults;
  flatMatResults.reserve(matResults.nTotal);
  for (std::vector<IValueComponentBase *>::const_iterator it = matResults.groups[0].begin();
       it != matResults.groups[0].end(); ++it)
    flatMatResults.push_back(*it);
  for (std::vector<IValueComponentBase *>::const_iterator it = matResults.groups[1].begin();
       it != matResults.groups[1].end(); ++it)
    flatMatResults.push_back(*it);

  CollectResults meshResults;
  static_cast<CModelBase &>(exportResultData.Model()).MeshResultTree().Accept(meshResults);

  CollectResults results;
  results.nodes = &exportResultData.m_selection;
  static_cast<CModelBase &>(exportResultData.Model()).ResultTree().Accept(results);

  std::vector<const IValueComponentBase *> flatResults;
  flatResults.reserve(results.nTotal);
  for (size_t i = 0; i < results.groups.size(); ++i)
    for (size_t j = 0; j < results.groups[i].size(); ++j)
      flatResults.push_back(results.groups[i][j]);

  std::vector<const IValueComponentBase *> flatDerivedResults;
  flatDerivedResults.reserve(results.nTotal);
  for (size_t i = 0; i < results.derivedGroups.size(); ++i)
    for (size_t j = 0; j < results.derivedGroups[i].size(); ++j)
      flatResults.push_back(results.derivedGroups[i][j]);

  CollectResults gvtResults;
  static_cast<CModelBase &>(exportResultData.Model()).GVTResultTree().Accept(gvtResults);

  std::vector<const IValueComponentBase *> flatGVTResults;
  flatGVTResults.reserve(gvtResults.nTotal);
  for (size_t i = 0; i < gvtResults.groups.size(); ++i)
    for (size_t j = 0; j < gvtResults.groups[i].size(); ++j)
      flatGVTResults.push_back(gvtResults.groups[i][j]);

  std::vector<const IValueComponentBase *> flatGVTDerivedResults;
  flatGVTDerivedResults.reserve(gvtResults.nTotal);
  for (size_t i = 0; i < gvtResults.derivedGroups.size(); ++i)
    for (size_t j = 0; j < gvtResults.derivedGroups[i].size(); ++j)
      flatGVTDerivedResults.push_back(gvtResults.derivedGroups[i][j]);

  m_matResults = &matResults;
  m_meshResults = &meshResults;
  m_results = &results;
  m_gvtResults = &gvtResults;

  m_flatMatResults = &flatMatResults;
  m_flatResults = &flatResults;
  m_flatDerivedResults = &flatDerivedResults;
  m_flatGVTResults = &flatGVTResults;
  m_flatGVTDerivedResults = &flatGVTDerivedResults;

  int size = (int)m_resultFilters.size();
  for (int i = 0; i < size; ++i)
    m_resultFilters[i].bind(exportResultData);
}

void CExportResultDataResultFilterVector::unbindMaterialResults(CExportResultData &exportResultData) {
  // Material results; either All or a subset ByName
  CollectResults matResults;
  static_cast<CModelBase &>(exportResultData.Model()).MaterialResultTree().Accept(matResults);

  assert(matResults.groups.size() == 2);

  if (exportResultData.m_MatRGObserverData.size() == matResults.nTotal) {
    CExportResultDataResultFilter filter;

    filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
    filter.m_nType = SET;
    filter.m_nSetValue = CExportResultDataResultFilter::ALL;

    m_resultFilters.push_back(filter);
  } else {
    size_t nNormalSelected = 0;
    size_t nFractureSelected = 0;

    for (TResultComponentSet::const_iterator it = exportResultData.m_MatRGObserverData.begin();
         it != exportResultData.m_MatRGObserverData.end(); ++it) {
      for (size_t i = 0; i < it->first->ComponentSize(); ++i) {
        if (std::find(matResults.groups[1].begin(), matResults.groups[1].end(), &it->first->Component(i)) !=
            matResults.groups[1].end())
          ++nFractureSelected;
      }
    }

    nNormalSelected = exportResultData.m_MatRGObserverData.size() - nFractureSelected;

    if (matResults.groups[0].size() == nNormalSelected) {
      CExportResultDataResultFilter filter;

      filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
      filter.m_nType = SET;
      filter.m_nSetValue = CExportResultDataResultFilter::NORMAL;

      m_resultFilters.push_back(filter);
    } else {
      for (TResultComponentSet::const_iterator it = exportResultData.m_MatRGObserverData.begin();
           it != exportResultData.m_MatRGObserverData.end(); ++it) {
        for (size_t i = 0; i < it->first->ComponentSize(); ++i) {
          if (std::find(matResults.groups[1].begin(), matResults.groups[1].end(), &it->first->Component(i)) ==
              matResults.groups[1].end()) {
            CExportResultDataResultFilter filter;

            filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
            filter.m_nType = BYNAME;
            filter.m_sName = static_cast<const CMaterialResult *>(it->first)->StoredExportLabel();

            m_resultFilters.push_back(filter);
          }
        }
      }
    }

    if (matResults.groups[1].size() == nFractureSelected) {
      CExportResultDataResultFilter filter;

      filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
      filter.m_nType = SET;
      filter.m_nSetValue = CExportResultDataResultFilter::FRACTUREINTENSITY;

      m_resultFilters.push_back(filter);
    } else {
      for (TResultComponentSet::const_iterator it = exportResultData.m_MatRGObserverData.begin();
           it != exportResultData.m_MatRGObserverData.end(); ++it) {
        for (size_t i = 0; i < it->first->ComponentSize(); ++i) {
          if (std::find(matResults.groups[1].begin(), matResults.groups[1].end(), &it->first->Component(i)) !=
              matResults.groups[1].end()) {
            CExportResultDataResultFilter filter;

            filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
            filter.m_nType = BYNAME;
            filter.m_sName = static_cast<const CMaterialResult *>(it->first)->StoredExportLabel();

            m_resultFilters.push_back(filter);
          }
        }
      }
    }
  }
}

void CExportResultDataResultFilterVector::unbindResults(
    CExportResultData &exportResultData) // TODO: split mesh and normal results
{
  CollectResults meshResults;
  static_cast<CModelBase &>(exportResultData.Model()).MeshResultTree().Accept(meshResults);

  CollectResults results;
  results.nodes = &exportResultData.m_selection;
  static_cast<CModelBase &>(exportResultData.Model()).ResultTree().Accept(results);

  size_t nResultsSelected = 0;
  size_t nMeshSelected = 0;

  for (TResultComponentSet::const_iterator it = exportResultData.m_RGObserverData.begin();
       it != exportResultData.m_RGObserverData.end(); ++it) {
    if (dynamic_cast<const IResult *>(it->first)) {
      if (static_cast<const IResultComponent &>(it->first->Component(it->second)).Type() == IValueComponentBase::SCALAR)
        ++nResultsSelected;
    } else
      ++nMeshSelected;
  }

  // Results; either All or ByName
  // Note: we only check for nodes that can be mapped to the current model, but we don't differentiate between
  // unselected and unmappable;
  //       this may impact the selection when the macro is imported in another model where the unmappables are mappable
  // TODO: extend with subsets
  if (nResultsSelected == results.nTotal) {
    CExportResultDataResultFilter filter;

    filter.m_nFilter = CExportResultDataResultFilter::RESULTS;
    filter.m_nType = SET;
    filter.m_nSetValue = CExportResultDataResultFilter::ALL;

    m_resultFilters.push_back(filter);
  } else {
    std::vector<const IValueComponentBase *> flat;
    flat.reserve(results.nTotal);
    for (size_t i = 0; i < results.groups.size(); ++i)
      for (size_t j = 0; j < results.groups[i].size(); ++j)
        flat.push_back(results.groups[i][j]);

    for (TResultComponentSet::const_iterator it = exportResultData.m_RGObserverData.begin();
         it != exportResultData.m_RGObserverData.end(); ++it) {
      if (dynamic_cast<const IResult *>(it->first)) {
        bool inTree = std::find(flat.begin(), flat.end(), &it->first->Component(it->second)) != flat.end();

        if (inTree) {
          assert(it->first->Component(it->second).Type() == IValueComponentBase::SCALAR);

          CExportResultDataResultFilter filter;

          filter.m_nFilter = CExportResultDataResultFilter::RESULTS;
          filter.m_nType = BYNAME;
          filter.m_sName = static_cast<const IResult *>(it->first)->ExportLabel(it->second);

          m_resultFilters.push_back(filter);
        }
      }
    }
  }

  // Derived results always ByName
  std::vector<const IValueComponentBase *> flatDerived;
  flatDerived.reserve(10);
  for (size_t i = 0; i < results.derivedGroups.size(); ++i)
    for (size_t j = 0; j < results.derivedGroups[i].size(); ++j)
      flatDerived.push_back(results.derivedGroups[i][j]);

  for (TResultComponentSet::const_iterator it = exportResultData.m_RGObserverData.begin();
       it != exportResultData.m_RGObserverData.end(); ++it) {
    if (dynamic_cast<const IResult *>(it->first)) {
      bool inTree =
          std::find(flatDerived.begin(), flatDerived.end(), &it->first->Component(it->second)) != flatDerived.end();

      if (inTree) {
        assert(it->first->Component(it->second).Type() == IValueComponentBase::SCALAR);

        CExportResultDataResultFilter filter;

        filter.m_nFilter = CExportResultDataResultFilter::RESULTS;
        filter.m_nType = BYNAME;
        filter.m_sName = static_cast<const IResult *>(it->first)->ExportLabel(it->second);

        m_resultFilters.push_back(filter);
      }
    }
  }

  // Mesh results; either All or a subset ByName
  if (nMeshSelected == meshResults.nTotal) {
    CExportResultDataResultFilter filter;

    filter.m_nFilter = CExportResultDataResultFilter::MESH;
    filter.m_nType = SET;
    filter.m_nSetValue = CExportResultDataResultFilter::ALL;

    m_resultFilters.push_back(filter);
  } else {
    for (TResultComponentSet::const_iterator it = exportResultData.m_RGObserverData.begin();
         it != exportResultData.m_RGObserverData.end(); ++it) {
      if (dynamic_cast<const CMeshResult *>(it->first)) {
        CExportResultDataResultFilter filter;

        filter.m_nFilter = CExportResultDataResultFilter::MESH;
        filter.m_nType = BYNAME;
        filter.m_sName = it->first->Component(it->second).ExportLabel();

        m_resultFilters.push_back(filter);
      }
    }
  }
}

void CExportResultDataResultFilterVector::unbindGVTResults(CExportResultData &exportResultData) {
  CollectResults gvtResults;
  static_cast<CModelBase &>(exportResultData.Model()).GVTResultTree().Accept(gvtResults);

  if (exportResultData.m_GVTRGObserverData.size() == gvtResults.nTotal) {
    CExportResultDataResultFilter filter;

    filter.m_nFilter = CExportResultDataResultFilter::GVTRESULTS;
    filter.m_nType = SET;
    filter.m_nSetValue = CExportResultDataResultFilter::ALL;

    m_resultFilters.push_back(filter);
  } else {
    std::vector<const IValueComponentBase *> flat;
    flat.reserve(gvtResults.nTotal);
    for (size_t i = 0; i < gvtResults.groups.size(); ++i)
      for (size_t j = 0; j < gvtResults.groups[i].size(); ++j)
        flat.push_back(gvtResults.groups[i][j]);

    for (TResultComponentSet::const_iterator it = exportResultData.m_GVTRGObserverData.begin();
         it != exportResultData.m_GVTRGObserverData.end(); ++it) {
      bool inTree = std::find(flat.begin(), flat.end(), &it->first->Component(it->second)) != flat.end();

      if (inTree) {
        assert(it->first->Component(it->second).Type() == IValueComponentBase::SCALAR);

        CExportResultDataResultFilter filter;

        filter.m_nFilter = CExportResultDataResultFilter::GVTRESULTS;
        filter.m_nType = BYNAME;

        if (dynamic_cast<const CValueType *>(it->first))
          filter.m_sName = it->first->ExportLabel();
        else
          filter.m_sName = static_cast<const IResult *>(it->first)->ExportLabel(it->second);

        m_resultFilters.push_back(filter);
      }
    }

    // Derived results always ByName
    std::vector<const IValueComponentBase *> flatDerived;
    flatDerived.reserve(10);
    for (size_t i = 0; i < gvtResults.derivedGroups.size(); ++i)
      for (size_t j = 0; j < gvtResults.derivedGroups[i].size(); ++j)
        flatDerived.push_back(gvtResults.derivedGroups[i][j]);

    for (TResultComponentSet::const_iterator it = exportResultData.m_GVTRGObserverData.begin();
         it != exportResultData.m_GVTRGObserverData.end(); ++it) {
      if (dynamic_cast<const IResult *>(it->first)) {
        bool inTree =
            std::find(flatDerived.begin(), flatDerived.end(), &it->first->Component(it->second)) != flatDerived.end();

        if (inTree) {
          assert(it->first->Component(it->second).Type() == IValueComponentBase::SCALAR);

          CExportResultDataResultFilter filter;

          filter.m_nFilter = CExportResultDataResultFilter::GVTRESULTS;
          filter.m_nType = BYNAME;
          filter.m_sName = static_cast<const IResult *>(it->first)->ExportLabel(it->second);

          m_resultFilters.push_back(filter);
        }
      }
    }
  }
}

void CExportResultDataResultFilterVector::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_resultFilters.clear();

    unbindMaterialResults(exportResultData);
    unbindResults(exportResultData);
    unbindGVTResults(exportResultData);
  }

  exportResultData.m_RGObserverData.clear();
  exportResultData.m_MatRGObserverData.clear();
  exportResultData.m_GVTRGObserverData.clear();
}

void CExportResultDataResultFilterVector::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                                     CStorageNode::TPROGRESS &progress) {
  int size;
  stream >> size;
  m_resultFilters.resize(size);
  for (int i = 0; i < size; ++i)
    m_resultFilters[i].LoadStream(stream, version, progress);
}

void CExportResultDataResultFilterVector::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  int size = (int)m_resultFilters.size();
  stream << size;
  for (int i = 0; i < size; ++i)
    m_resultFilters[i].SaveStream(stream, progress);
}

CExportResultDataDeformation::CExportResultDataDeformation()
    : m_nType(NONE), m_nSetValue(INITIAL), m_nCalculation(LINEAR) {}

void CExportResultDataDeformation::bind(CExportResultData &exportResultData) {
  switch (m_nType) {
  case SET:
    switch (m_nSetValue) {
    case INITIAL:
      exportResultData.m_pTimeStep =
          &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().InitialStage();
      break;
    case LAST:
      exportResultData.m_pTimeStep =
          &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().LastStage();
      break;
    }
    break;
  case BYNAME: {
    CDepletionStage *current =
        &static_cast<CModelBase &>(exportResultData.Model()).DepletionStageEntry().InitialStage();
    while (current) {
      if (current->Name() == m_sName) {
        exportResultData.m_pTimeStep = current;
        break;
      }

      current = current->Last() ? 0 : &current->Next();
    }
  } break;
  }

  exportResultData.m_nLinear = m_nCalculation;
}

void CExportResultDataDeformation::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_nType = NONE;

    if (exportResultData.m_pTimeStep) {
      if (exportResultData.m_pTimeStep->Initial()) {
        m_nType = SET;
        m_nSetValue = INITIAL;
      } else if (exportResultData.m_pTimeStep->Last()) {
        m_nType = SET;
        m_nSetValue = LAST;
      } else {
        m_nType = BYNAME;
        m_sName = exportResultData.m_pTimeStep->Name();
      }
    }

    m_nCalculation = (TCalculation)exportResultData.m_nLinear;
  }

  exportResultData.m_pTimeStep = 0;
}

void CExportResultDataDeformation::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                              CStorageNode::TPROGRESS & /*progress*/) {
  int temp;
  stream >> temp;
  m_nType = (TType)temp;
  switch (m_nType) {
  case SET:
    stream >> temp;
    m_nSetValue = (TSetValue)temp;
    break;
  case BYNAME:
    stream >> m_sName;
  }
  stream >> temp;
  m_nCalculation = (TCalculation)temp;
}

void CExportResultDataDeformation::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS & /*progress*/) {
  stream << (int)m_nType;
  switch (m_nType) {
  case SET:
    stream << (int)m_nSetValue;
    break;
  case BYNAME:
    stream << m_sName;
    break;
  }
  stream << (int)m_nCalculation;
}

CExportResultDataHotSpot::CExportResultDataHotSpot()
    : m_nType(NONE), m_bCenterIsTransparent(false), m_nMinColor(), m_nMaxColor(), m_dMinMaxValue(0, 0) {}

void CExportResultDataHotSpot::bind(CExportResultData &exportResultData) {
  if (m_nType == NONE)
    return;

  IColorScaleEntry *colorScaleEntry = dynamic_cast<IColorScaleEntry *>(
      static_cast<CModelBase &>(exportResultData.Model()).GraphEntry(MD_BASE_COLOR_SCALE));
  if (colorScaleEntry) {
    IColorScaleEntry::TNodeSet stNodes = colorScaleEntry->EntryNodes();

    for (IColorScaleEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
      if (dynamic_cast<CHotSpot *>(*it)) {
        CHotSpot *hs = static_cast<CHotSpot *>(*it);

        bool tryCenterIsTransparent = hs->CenterIsTransparent();
        IColorScaleNode::TColor tryMinColor = hs->MinColor();
        IColorScaleNode::TColor tryMaxColor = hs->MaxColor();
        std::pair<double, double> tryMMValue = hs->MinMaxValue();

        if (m_bCenterIsTransparent == tryCenterIsTransparent && m_nMinColor == tryMinColor &&
            m_nMaxColor == tryMaxColor && std::abs(m_dMinMaxValue.first - tryMMValue.first) < 1E-6 &&
            std::abs(m_dMinMaxValue.second - tryMMValue.second) < 1E-6) {
          exportResultData.m_pHotSpot = hs;
          break;
        }
      }
    }

    if (!exportResultData.m_pHotSpot) {
      CHotSpot *hs = new CHotSpot(IDS_NEW_HOTSPOT, exportResultData.Model(), m_nMinColor, m_nMaxColor);
      hs->Name(m_sName);
      hs->CenterIsTransparent(m_bCenterIsTransparent);
      hs->SetMinMaxValue(m_dMinMaxValue.first, m_dMinMaxValue.second);

      exportResultData.m_pHotSpot = hs;
    }
  }
}

void CExportResultDataHotSpot::unbind(CExportResultData &exportResultData, bool bStore) {
  if (bStore) {
    m_nType = NONE;

    if (exportResultData.m_pHotSpot) {
      m_nType = BYNAME;
      m_sName = exportResultData.m_pHotSpot->Name();
      m_bCenterIsTransparent = exportResultData.m_pHotSpot->CenterIsTransparent();
      m_nMinColor = exportResultData.m_pHotSpot->MinColor();
      m_nMaxColor = exportResultData.m_pHotSpot->MaxColor();
      m_dMinMaxValue = exportResultData.m_pHotSpot->MinMaxValue();
    }
  }

  exportResultData.m_pHotSpot = 0;
}

void CExportResultDataHotSpot::LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                          CStorageNode::TPROGRESS &progress) {
  int temp;
  stream >> temp;
  m_nType = (TType)temp;
  if (m_nType != NONE) {
    stream >> m_sName;
    loadBoolean(m_bCenterIsTransparent, stream, version, progress);
    int nRed, nGreen, nBlue;
    stream >> nRed;
    stream >> nGreen;
    stream >> nBlue;
    m_nMinColor = qRgb(nRed, nGreen, nBlue);
    stream >> nRed;
    stream >> nGreen;
    stream >> nBlue;
    m_nMaxColor = qRgb(nRed, nGreen, nBlue);
    stream >> m_dMinMaxValue.first;
    stream >> m_dMinMaxValue.second;
  }
}

void CExportResultDataHotSpot::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  stream << (int)m_nType;
  if (m_nType != NONE) {
    stream << m_sName;
    saveBoolean(stream, progress, m_bCenterIsTransparent);
    int nRed = qRed(m_nMinColor);
    int nGreen = qGreen(m_nMinColor);
    int nBlue = qBlue(m_nMinColor);
    stream << nRed;
    stream << nGreen;
    stream << nBlue;
    nRed = qRed(m_nMaxColor);
    nGreen = qGreen(m_nMaxColor);
    nBlue = qBlue(m_nMaxColor);
    stream << nRed;
    stream << nGreen;
    stream << nBlue;
    stream << m_dMinMaxValue.first;
    stream << m_dMinMaxValue.second;
  }
}

CExportResultData::CExportResultData(CFemAppModel &model)
    : CColorNode(model), m_settings(), m_depletionStageFilterVector(), m_modelFilterVector(), m_resultFilterVector(),
      m_deformation(), m_hotspot(), m_folder(), m_file(), m_export(), m_selection(), m_nUnit(), m_nTZ(), m_pHotSpot(),
      m_bLinear(), m_bNonLinear(), m_bHeat(), m_bMixture(), m_bMixtureContainment(), m_RGObserverData(), m_pTimeStep(),
      m_nLinear(), m_nCenterPoints(), m_nWellPoints(), m_nFormationNames(), m_nWeightingType(), m_stTimeStep(),
      m_MatRGObserverData(), m_bReservoirMacro() {}

CExportResultData::CExportResultData(const QString &exportResultName, CFemAppModel &model)
    : CColorNode(exportResultName, model), m_settings(), m_depletionStageFilterVector(), m_modelFilterVector(),
      m_resultFilterVector(), m_deformation(), m_hotspot(), m_folder(), m_file(), m_export(), m_selection(), m_nUnit(),
      m_nTZ(), m_pHotSpot(), m_bLinear(), m_bNonLinear(), m_bHeat(), m_bMixture(), m_bMixtureContainment(),
      m_RGObserverData(), m_pTimeStep(), m_nLinear(), m_nCenterPoints(), m_nWellPoints(), m_nFormationNames(),
      m_nWeightingType(), m_stTimeStep(), m_MatRGObserverData(), m_bReservoirMacro() {
  assert(Model().GraphEntry(MD_BASE_EXPORT_MACROS));
  reParent(Model().GraphEntry(MD_BASE_EXPORT_MACROS));
}

unsigned int CExportResultData::IconId() const { return IDI_EXPORT_MACROS; }

unsigned int CExportResultData::TypeId() const { return IDT_TREE_EXPORT_MACROS; }

int CExportResultData::DisplayListSize() const { return 0; }

const geo::IObject &CExportResultData::DisplayList(int /*nIndex*/) const {
  static const geo::CPoint point;

  // Should never be called (DisplayListSize() == 0)!

  assert(false);

  return point;
}

bool CExportResultData::ReservoirMacro() const { return m_settings.m_bReservoirMacro; }

void CExportResultData::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CColorNode::LoadStream(stream, version, progress);

  reParent(Model().GraphEntry(MD_BASE_EXPORT_MACROS));

  if (version < CStreamVersion(4, 1, 50)) {
    stream >> m_folder;
    stream >> m_file;
    loadBoolean(m_export, stream, version, progress);
    m_selection.LoadStream(stream, version, progress, Model());
    stream >> m_nUnit;
    stream >> m_nTZ;
    loadHotSpot(m_pHotSpot, Model(), stream, version, progress);
    loadBoolean(m_bLinear, stream, version, progress);
    loadBoolean(m_bNonLinear, stream, version, progress);
    loadBoolean(m_bHeat, stream, version, progress);
    loadBoolean(m_bMixture, stream, version, progress);

    if (version >= CStreamVersion(4, 1, 22))
      loadBoolean(m_bMixtureContainment, stream, version, progress);

    loadResultComponentSet(m_RGObserverData, Model(), stream, version, progress);
    loadTimeStep(&m_pTimeStep, Model(), stream, version, progress);
    stream >> m_nLinear;
    stream >> m_nCenterPoints;
    if (version >= CStreamVersion(4, 1, 38)) {
      stream >> m_nWellPoints;
    }
    stream >> m_nFormationNames;
    stream >> m_nWeightingType;
    loadTimeStep(m_stTimeStep, Model(), stream, version, progress);
    loadMaterialResultComponentSet(m_MatRGObserverData, Model(), stream, version, progress);

    unbind(true);
  } else {
    m_settings.LoadStream(stream, version, progress);
    m_depletionStageFilterVector.LoadStream(stream, version, progress);
    m_modelFilterVector.LoadStream(stream, version, progress);
    m_resultFilterVector.LoadStream(stream, version, progress);
    m_deformation.LoadStream(stream, version, progress);
    m_hotspot.LoadStream(stream, version, progress);
  }
}

void CExportResultData::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  CColorNode::SaveStream(stream, progress);

  m_settings.SaveStream(stream, progress);
  m_depletionStageFilterVector.SaveStream(stream, progress);
  m_modelFilterVector.SaveStream(stream, progress);
  m_resultFilterVector.SaveStream(stream, progress);
  m_deformation.SaveStream(stream, progress);
  m_hotspot.SaveStream(stream, progress);
}

void CExportResultData::bind() {
  m_settings.bind(*this);
  m_depletionStageFilterVector.bind(*this);
  m_modelFilterVector.bind(*this);
  m_resultFilterVector.bind(*this);
  m_deformation.bind(*this);
  m_hotspot.bind(*this);
}

void CExportResultData::unbind(bool bStore) {
  m_settings.unbind(*this, bStore);
  m_depletionStageFilterVector.unbind(*this, bStore);
  m_resultFilterVector.unbind(
      *this, bStore); // results before model, as we still need the latter to check for allowed results (upon saving)
  m_modelFilterVector.unbind(*this, bStore);
  m_deformation.unbind(*this, bStore);
  m_hotspot.unbind(*this, bStore);
}