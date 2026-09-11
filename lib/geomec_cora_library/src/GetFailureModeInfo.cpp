#include <fstream>

#include "DepletionStageAnalysisType.h"
#include "FemAppEntryTypes.h"
#include "GetFailureModeInfo.h"
#include "GetFaultInfo.h"
#include "GetFormationInfo.h"
#include "GetHorizonInfo.h"
#include "GetSurfaceInfo.h"
#include "GetWellPathInfo.h"
#include "MaterialResultTree.h"
#include "MeshResultTree.h"
#include "ResultTree.h"

namespace cora {

CGetFailureModeInfo::CGetFailureModeInfo(CModelBase *modelBase, bool annotated)
    : m_exportLabels(), m_filterResults(), m_filterOutputProperty(),
      m_failureModes(getFailureModes(m_exportLabels, modelBase, annotated, m_filterResults, m_filterOutputProperty)),
      m_modelBase(modelBase), m_annotated(annotated) {}

std::vector<QString> CGetFailureModeInfo::getExportLabels() const { return m_exportLabels; }

/*
  TODO

  Replacing the function call with the class member 'm_failureModes' results
  in memory corruption.
*/

TFailureModes CGetFailureModeInfo::getFailureModes() {
  return getFailureModes(m_exportLabels, m_modelBase, m_annotated, m_filterResults, m_filterOutputProperty);
}

std::ostream &CGetFailureModeInfo::operator()(std::ostream &os) const {
  os << m_exportLabels.size() << std::endl;

  for (size_t e = 0; e < m_exportLabels.size(); ++e) {
    os << m_exportLabels[e].toStdString() << std::endl;
  }

  return os;
}

// private

// static

TFailureModes CGetFailureModeInfo::getFailureModes(std::vector<QString> &exportLabels, CModelBase *modelBase,
                                                   bool annotated, const CFilterResults &filterResults,
                                                   const CFilterOutputProperty &filterOutputProperty) {
  const CGraphEntryTemp<IResult> *graphEntryTemp =
      dynamic_cast<const CGraphEntryTemp<IResult> *>(modelBase->GraphEntry(MD_BASE_RESULT));

  // TODO investigate dependencies (parents) to resolve ordering.
  // TSortedNodeSet cannot be used due to a Less(...) operator that results in
  // conflicting values ((A < B) && (B < A)).

  const CGraphEntryTemp<IResult>::TNodeSet nodeSet = graphEntryTemp->EntryNodes();
  std::vector<TFailureMode> failureModes;

  exportLabels.clear();

  assert(exportLabels.size() == 0);

  for (CGraphEntryTemp<IResult>::TNodeSet::const_iterator node = nodeSet.begin(); node != nodeSet.end(); ++node) {
    if (dynamic_cast<const CMaterialResult *>(*node) == 0) {
      getFailureModes(failureModes, exportLabels, modelBase, *node, annotated, filterResults, filterOutputProperty);
    }
  }

  return failureModes;
}

namespace {

const QString COMMA = ",";
const QString QUOTE = "\"";

QString getName(const CGraphNode *graphNode) {
  if (graphNode->parent() != 0) {
    return getName(graphNode->parent()) + COMMA + QUOTE + graphNode->Name() + QUOTE;
  }

  return QUOTE + graphNode->Name() + QUOTE;
}

const QString COLON = " : ";
const QString EMPTY = "";

typedef std::pair<const IResultComponent *, const COpenGLNode *> TResultComponentObject;
typedef std::vector<TResultComponentObject> TResultComponentObjects;

void retrieveValidResultComponentObjects(TFailureModes &failureModes,
                                         TResultComponentObjects &validResultComponentObjects, const IResult *result,
                                         const TDepletionStageAnalysisTypeVector &depletionStageAnalysisTypeVector,
                                         const TObject &object, unsigned int componentIndex) {
  for (TDepletionStageAnalysisTypeVector::const_iterator depletionStageAnalysisType =
           depletionStageAnalysisTypeVector.begin();
       depletionStageAnalysisType != depletionStageAnalysisTypeVector.end(); ++depletionStageAnalysisType) {
    const CDepletionStage *depletionStage = (*depletionStageAnalysisType).first;
    const CAnalysisType &analysisType = (*depletionStageAnalysisType).second;
    const IResultComponent *resultComponent = result->ResultComponent(*depletionStage, analysisType, 0, componentIndex);

    if (resultComponent != 0) {
      if (resultComponent->CanMap(*(object->object()))) {
        validResultComponentObjects.push_back(TResultComponentObject(resultComponent, object->object()));
        failureModes.push_back(TFailureMode(new CFailureMode(resultComponent->ExportLabel(), resultComponent)));
      }
    }
  }
}

void retrieveAnalysisType(QString analysisType[], const TResultComponentObject &validResultComponentObject) {
  switch (validResultComponentObject.first->AnalysisType().AnalysisType()) {
  case CAnalysisType::AT_LINEAR:
  case CAnalysisType::AT_NONLIN:
  case CAnalysisType::AT_HEAT:
  case CAnalysisType::AT_MIXTURE:
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
    analysisType[validResultComponentObject.first->AnalysisType().AnalysisType()] =
        validResultComponentObject.first->AnalysisType().ExportCharacter();
    break;
  default:
    assert(false);
    break;
  }
}

const QString FORMAT_DEPLETION_STAGE = "D%1";

QString retrieveDepletionStage(const TResultComponentObject &validResultComponentObject) {
  return QString(FORMAT_DEPLETION_STAGE).arg(validResultComponentObject.first->Stage().Index());
}

QString retrieveFailureModeName(const TResultComponentObject &validResultComponentObject) {
  return dynamic_cast<const IResult &>(validResultComponentObject.first->Parent())
      .ExportLabel(validResultComponentObject.first->ComponentIndex());
}

void pushFailureModeExportLabel(TFailureModes &failureModes, std::vector<QString> &exportLabels, const IResult *result,
                                const TDepletionStageAnalysisTypeVector &depletionStageAnalysisTypeVector,
                                const TObjects &objects, bool annotated, const CFilterResults &filterResults,
                                const CFilterOutputProperty &filterOutputProperty) {
  QString annotation = annotated ? getName(result) + COLON : EMPTY;

  unsigned int componentSize = result->ComponentSize();

  for (unsigned int i = 0; i < componentSize; ++i) {
    if (result->Component(i).Type() == IValueComponentBase::SCALAR) {
      if (filterResults.isResultAllowed(result->ExportLabel(i))) {
        for (TObjects::const_iterator object = objects.begin(); object != objects.end(); ++object) {
          if (filterOutputProperty.isOutputPropertyAllowed(*(*object), result->ExportLabel(i))) {
            TResultComponentObjects validResultComponentObjects;
            std::size_t lastFailureMode = failureModes.size();

            retrieveValidResultComponentObjects(failureModes, validResultComponentObjects, result,
                                                depletionStageAnalysisTypeVector, (*object), i);

            QString analysisType[CAnalysisType::AT_LAST + 1];

            for (TResultComponentObjects::const_iterator validResultComponentObject =
                     validResultComponentObjects.begin();
                 validResultComponentObject != validResultComponentObjects.end(); ++validResultComponentObject) {
              retrieveAnalysisType(analysisType, *validResultComponentObject);
            }

            QString previousDepletionStage;
            std::size_t f = lastFailureMode;

            assert((failureModes.size() - lastFailureMode) == validResultComponentObjects.size());

            for (TResultComponentObjects::const_iterator validResultComponentObject =
                     validResultComponentObjects.begin();
                 (validResultComponentObject != validResultComponentObjects.end()) && (f < failureModes.size());
                 ++validResultComponentObject, ++f) {
              QString depletionStage = retrieveDepletionStage(*validResultComponentObject);

              if (depletionStage != previousDepletionStage) {
                QString failureModeName = retrieveFailureModeName(*validResultComponentObject);
                QString failureModeLabel =
                    failureModeName + COMMA + depletionStage + COMMA + analysisType[CAnalysisType::AT_LINEAR] + COMMA +
                    analysisType[CAnalysisType::AT_NONLIN] + COMMA + analysisType[CAnalysisType::AT_MIXTURE] + COMMA +
                    analysisType[CAnalysisType::AT_MIXTURE_CONTAINMENT] + COMMA + analysisType[CAnalysisType::AT_HEAT];
                QString objectLabel = (*object)->prefix() + (*object)->object()->Name();

                exportLabels.push_back(annotation + failureModeLabel + COMMA + objectLabel);

                failureModes[f]->setFailureModeLabel(failureModeLabel);
                failureModes[f]->setObject(*object);

                previousDepletionStage = depletionStage;
              }
            }
          }
        }
      }
    }
  }
}

} // anonymous namespace

void CGetFailureModeInfo::getFailureModes(std::vector<TFailureMode> &failureModes, std::vector<QString> &exportLabels,
                                          CModelBase *modelBase, const IResult *result, bool annotated,
                                          const CFilterResults &filterResults,
                                          const CFilterOutputProperty &filterOutputProperty) {
  CGetFormationInfo &getFormationInfo(CGetFormationInfo::instance(modelBase));
  TObjects objects = getFormationInfo.getObjects();
  CGetFaultInfo &getFaultInfo(CGetFaultInfo::instance(modelBase));
  TObjects faultObjects = getFaultInfo.getObjects();
  CGetSurfaceInfo &getSurfaceInfo(CGetSurfaceInfo::instance(modelBase));
  TObjects surfaceObjects = getSurfaceInfo.getObjects();
  CGetHorizonInfo &getHorizonInfo(CGetHorizonInfo::instance(modelBase));
  TObjects horizonObjects = getHorizonInfo.getObjects();
  CGetWellPathInfo &getWellPathInfo(CGetWellPathInfo::instance(modelBase));
  TObjects wellPathObjects = getWellPathInfo.getObjects();
  CDepletionStageAnalysisType depletionStageAnalysisType(modelBase);
  const TDepletionStageAnalysisTypeVector &depletionStageAnalysisTypeVector =
      depletionStageAnalysisType.getDepletionStageAnalysisType();

  objects.insert(objects.end(), faultObjects.begin(), faultObjects.end());

  // TODO SURFACE-HORIZON
  // for now do not add the surfaces, only the horizons (top-horizon)

  // objects.insert(objects.end(), surfaceObjects.begin(),
  //   surfaceObjects.end());

  objects.insert(objects.end(), horizonObjects.begin(), horizonObjects.end());
  objects.insert(objects.end(), wellPathObjects.begin(), wellPathObjects.end());

  pushFailureModeExportLabel(failureModes, exportLabels, result, depletionStageAnalysisTypeVector, objects, annotated,
                             filterResults, filterOutputProperty);
}

} // namespace cora

// global

std::ostream &operator<<(std::ostream &os, const cora::CGetFailureModeInfo &i) { return i(os); }
