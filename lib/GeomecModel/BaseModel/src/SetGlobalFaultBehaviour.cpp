#include "SetGlobalFaultBehaviour.h"
#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "FemAppModel.h"
#include "HorizonBase.h"
#include "ModelBase.h"
#include "unitnode.h"

CSetGlobalFaultBehaviour::CSetGlobalFaultBehaviour(CFemAppModel *femAppModel)
    : m_modelBase(dynamic_cast<CModelBase *>(femAppModel)), m_faultPressures(collectFaultPressures(m_modelBase)) {}

// canModifyPressure() will return true if and only if all CFaultPressure
// objects satisfy the test conditions

bool CSetGlobalFaultBehaviour::canModifyPressure() const {
  for (TFaultPressures::const_iterator faultPressure = m_faultPressures.begin();
       faultPressure != m_faultPressures.end(); ++faultPressure) {
    if (!canModifyPressure(*faultPressure)) {
      return false;
    }
  }

  return true;
}

std::vector<QString> CSetGlobalFaultBehaviour::getUnitNames(const CUnitNode &unitNode) const {
  std::vector<QString> unitNames;

  for (TFaultPressures::const_iterator faultPressure = m_faultPressures.begin();
       faultPressure != m_faultPressures.end(); ++faultPressure) {
    std::string referenceDepthUnitName = (*faultPressure)->Gradient().ReferenceDepth().UnitName(unitNode.Unit());
    std::string referencePressureUnitName = (*faultPressure)->Gradient().ReferencePressure().UnitName(unitNode.Unit());
    std::string gradientUnitName = (*faultPressure)->Gradient().Gradient().UnitName(unitNode.Unit());

    if (unitNames.size() > 0) {
      assert(referenceDepthUnitName == unitNames[DEPTH].toStdString());
      assert(referencePressureUnitName == unitNames[PRESSURE].toStdString());
      assert(gradientUnitName == unitNames[GRADIENT].toStdString());
    } else {
      unitNames.push_back(referenceDepthUnitName.c_str());
      unitNames.push_back(referencePressureUnitName.c_str());
      unitNames.push_back(gradientUnitName.c_str());
    }
  }

  return unitNames;
}

bool CSetGlobalFaultBehaviour::getModeType(CFaultPressure::TModeType &modeType) const {
  bool firstTime = true;

  for (TFaultPressures::const_iterator faultPressure = m_faultPressures.begin();
       faultPressure != m_faultPressures.end(); ++faultPressure) {
    if (firstTime) {
      firstTime = false;
      modeType = (*faultPressure)->Mode();
    } else if (modeType != (*faultPressure)->Mode()) {
      return false;
    }
  }

  return true;
}

bool CSetGlobalFaultBehaviour::getValues(std::vector<double> &values, const CUnitNode &unitNode) const {
  bool firstTime = true;

  for (TFaultPressures::const_iterator faultPressure = m_faultPressures.begin();
       faultPressure != m_faultPressures.end(); ++faultPressure) {
    if (firstTime) {
      firstTime = false;
      values[CSetGlobalFaultBehaviour::DEPTH] = (*faultPressure)->Gradient().ReferenceDepth().Value(unitNode.Unit());
      values[CSetGlobalFaultBehaviour::PRESSURE] =
          (*faultPressure)->Gradient().ReferencePressure().Value(unitNode.Unit());
      values[CSetGlobalFaultBehaviour::GRADIENT] = (*faultPressure)->Gradient().Gradient().Value(unitNode.Unit());
    } else {
      if (values[CSetGlobalFaultBehaviour::DEPTH] !=
          (*faultPressure)->Gradient().ReferenceDepth().Value(unitNode.Unit())) {
        return false;
      } else if (values[CSetGlobalFaultBehaviour::PRESSURE] !=
                 (*faultPressure)->Gradient().ReferencePressure().Value(unitNode.Unit())) {
        return false;
      } else if (values[CSetGlobalFaultBehaviour::GRADIENT] !=
                 (*faultPressure)->Gradient().Gradient().Value(unitNode.Unit())) {
        return false;
      }
    }
  }

  return true;
}

void CSetGlobalFaultBehaviour::setGlobalFaultBehaviour(const std::vector<double> &values, const CUnitNode &unitNode,
                                                       CFaultPressure::TModeType modeType) {
  for (TFaultPressures::const_iterator faultPressure = m_faultPressures.begin();
       faultPressure != m_faultPressures.end(); ++faultPressure) {
    (*faultPressure)->Mode(modeType);

    if (modeType == CFaultPressure::MT_GRADIENT) {
      (*faultPressure)->Gradient().ReferenceDepth(values[CSetGlobalFaultBehaviour::DEPTH], unitNode.Unit());
      (*faultPressure)->Gradient().ReferencePressure(values[CSetGlobalFaultBehaviour::PRESSURE], unitNode.Unit());
      (*faultPressure)->Gradient().Gradient(values[CSetGlobalFaultBehaviour::GRADIENT], unitNode.Unit());
    }
  }
}

// private

bool CSetGlobalFaultBehaviour::isLocked(const CFaultPressure *faultPressure) const {
  return m_modelBase->BranchState().IsFixedStage(faultPressure->DepletionStage());
}

bool CSetGlobalFaultBehaviour::canModifyPressure(const CFaultPressure *faultPressure) const {
  if ((!faultPressure->DistributedOnly() || (faultPressure->DistributedSize() == 0)) && !isLocked(faultPressure)) {
    return true;
  }

  return false;
}

CSetGlobalFaultBehaviour::TFaultPressures CSetGlobalFaultBehaviour::collectFaultPressures(CModelBase *modelBase) {
  TFaultPressures faultPressures;
  THorizonBaseEntry::TEntryNodeSet entryNodeSet = modelBase->GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();

  for (THorizonBaseEntry::TEntryNodeSet::iterator entryNode = entryNodeSet.begin(); entryNode != entryNodeSet.end();
       ++entryNode) {
    CHorizonBase *horizonBase = dynamic_cast<CHorizonBase *>(*entryNode);

    if (horizonBase->Slip()) {
      const CDepletionStageEntry &depletionStageEntry = modelBase->DepletionStageEntry();

      for (CDepletionStageEntry::const_iterator depletionStage = depletionStageEntry.begin();
           depletionStage != depletionStageEntry.end(); ++depletionStage) {
        faultPressures.push_back(&(horizonBase->Pressure(*depletionStage)));
      }
    }
  }

  return faultPressures;
}
