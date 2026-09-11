#include "stdafx.h"

#include "AnalysisPoint.h"
#include "Clipboard.h"
#include "ContextMenuInvoker.h"
#include "Curve.h"
#include "FemAppDoc.h"
#include "FemAppMainWindow.h"
#include "GeomecDoc.h"
#include "GlobalMessage.h"
#include "MeshResultTree.h"
#include "ModelView.h"
#include "OIV2DHistoryGraph.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DLegend.h"
#include "Result.h"
#include "StaborResult.h"
#include "resourceIDS.h"
#include "vcbfunctions.h"

OIV2DHistoryScene::OIV2DHistoryScene(CFemAppModel &model) : OIV2DGraphValueColorScene(model) {
  Name("History View");

  init();
}

OIV2DHistoryScene::OIV2DHistoryScene(const QString &strName, CFemAppModel &model)
    : OIV2DGraphValueColorScene(strName, model) {
  Name("History View");
  init();
}

OIV2DHistoryScene::OIV2DHistoryScene(unsigned int uName, CFemAppModel &model)
    : OIV2DGraphValueColorScene(uName, model) {
  Name("History View");
  init();
}

OIV2DHistoryScene::~OIV2DHistoryScene() {}

void OIV2DHistoryScene::unlinkAnalysisPoints() {
  auto analysisPoints = m_analysisPoints;

  for (auto analysisPoint : analysisPoints) {
    UnLink(const_cast<CAnalysisPoint &>(*analysisPoint));
  }
  assert(m_analysisPoints.empty());
}

void OIV2DHistoryScene::unlinkValueComponents() {
  auto valueComponents = m_valueComponents;

  for (auto valueComponent : valueComponents) {
    UnLink(const_cast<IValueComponentBase &>(*valueComponent));
  }
  assert(m_valueComponents.empty());
}

void OIV2DHistoryScene::updateGraph() {
  SoGroup *root = getSceneGraph();
  if (!root)
    return;

  if (m_graph == 0) {
    m_graph = new OIV2DHistoryGraph(getAspect(), m_preferredCurveGroup);
    root->addChild(m_graph);

    viewAll();
  } else {
    m_graph->setAspect(getAspect());
    updateDomains();
  }

  OIV2DGraphValueColorScene::updateGraph();
}

const CGraphNode *getMainGroup(const IValueComponentBase *valueComponent) {
  const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(valueComponent);

  // Get result of component
  const IResult *result = dynamic_cast<const IResult *>(&valueComponent->Parent());
  const CMeshResult *meshResult = dynamic_cast<const CMeshResult *>(&valueComponent->Parent());

  const CResultGroup *group;
  if (result) {
    group = result->Parent();
  } else {
    assert(meshResult);
    return (meshResult->parent());
  }

  while (group->Parent()) {
    group = group->Parent();
  }

  return group;
}

void OIV2DHistoryScene::updateLegend() {
  if (!m_legend)
    return;

  m_legend->clear();

  const std::string defaultHeader = "Results:";
  std::string itemsHeader = defaultHeader;

  if (!m_valueComponents.empty()) {
    std::string analysis = "Analysis: ";
    const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(m_valueComponents[0]);
    analysis += resultComponent->AnalysisType().Label().toStdString();
    m_legend->addLeader(0, analysis);
  }

  std::string unitType = "Unit type: ";
  unitType += m_unit->UnitName(m_unit->Unit()).toStdString();
  m_legend->addLeader(0, unitType);

  m_legend->addLeader(0, "");

  if (m_analysisPoints.size() == 1 || m_valueComponents.empty()) {
    for (auto analysisPoint : m_analysisPoints) {
      std::string name = "Analysis Points::" + analysisPoint->Name().toStdString();
      m_legend->addLeader(0, name);
    }
  } else
    itemsHeader = "Analysis Points:";

  if ((m_valueComponents.size() == 1 && itemsHeader != defaultHeader) || m_analysisPoints.empty()) {
    for (auto valueComponent : m_valueComponents) {
      std::string name = vcbfunctions::getResultPath(*valueComponent, 0).toStdString() + " [" +
                         valueComponent->UnitName(m_unit->Unit()).toStdString() + "]";
      m_legend->addLeader(0, name);
    }
  } else
    itemsHeader = "Results:";

  if (m_graph && m_graph->getNumCurves() == 0)
    return;

  // m_legend->addLeader(0, "");
  //  m_legend->addLeader(0, itemsHeader);

  bool is_first_value = true;

  for (auto valueComponent : m_valueComponents) {
    for (auto analysisPoint : m_analysisPoints) {
      Curve *curve = static_cast<OIV2DHistoryGraph *>(m_graph)->findCurve(valueComponent, analysisPoint);
      if (!curve)
        continue;

      if (is_first_value) {
        COpenGLNode::TColor color_ = analysisPoint->Color();
        curve->setColor(color_, color_);
      }

      QColor color = curve->getCurveColor();
      std::string name;
      int legendSectionIndex = 0;
      std::string legendSectionTitle;
      if (m_analysisPoints.size() > 1) {
        legendSectionIndex = 1;
        legendSectionTitle = "Analysis Points:";
        name = analysisPoint->Name().toStdString();
      } else {
        name = curve->getFullResultPath().toStdString();

        const CGraphNode *group = getMainGroup(valueComponent);

        if (group->Name() == "Material Parameters")
          legendSectionIndex = 1;
        else if (group->Name() == "Results")
          legendSectionIndex = 2;
        legendSectionTitle = group->Name().toStdString();
      }

      if (m_legend->getNumLines(legendSectionIndex) == 0) {
        m_legend->addLeader(legendSectionIndex, legendSectionTitle);
      }
      m_legend->addItem(legendSectionIndex, name, color.redF(), color.greenF(), color.blueF());

      curve->setLabel(QString(name.c_str()));
    }
    is_first_value = false;
  }
}

void OIV2DHistoryScene::init() {
  m_graph = 0;

  m_rejectedComponent = 0;
  CFemAppDoc *doc = GetGeomecDoc();
  assert(doc);
  m_unit = &doc->UnitNode();
  assert(m_unit);
  LinkTo(*m_unit);

  createSceneGraph();

  m_legend = new OIV2DLegend();
  m_legend->addLegendSection();
  m_legend->addLegendSection();
  m_legend->addLegendSection();

  getSceneGraph()->addChild(m_legend);

  if (m_graph == 0) {
    m_graph = new OIV2DHistoryGraph(getAspect(), m_preferredCurveGroup);
    getSceneGraph()->addChild(m_graph);

    viewAll();
  }

  //// Link to color scale entry
  // m_pColorScaleEntry = dynamic_cast<IColorScaleEntry*>(Model().GraphEntry(MD_BASE_COLOR_SCALE));
  // LinkTo(*m_pColorScaleEntry);

  //// Link to the current color scale
  // LinkTo(m_pColorScaleEntry->CurrentColorScale());
  // assert(&m_pColorScaleEntry->CurrentColorScale() == m_pColorScale);
}

bool OIV2DHistoryScene::operator==(const OIV2DHistoryScene &node) const { return false; }

const CUnitNode &OIV2DHistoryScene::UnitNode() const { return *m_unit; }

void OIV2DHistoryScene::clearAxis(int group) {
  std::vector<const IValueComponentBase *> components;

  static_cast<OIV2DHistoryGraph *>(m_graph)->collectComponents(group, components);

  for (auto component : components) {
    UnLink(const_cast<IValueComponentBase &>(*component));
  }
  UpdateAllViews();
}

void OIV2DHistoryScene::clearLeftAxis() {
  if (m_analysisPoints.size() >
      1) // only left axis and only one valuecomponent is used in this case, so we just unlink all analysis points
  {
    auto analysisPoints = m_analysisPoints;
    for (auto analysisPoint : analysisPoints) {
      UnLink(const_cast<CAnalysisPoint &>(*analysisPoint));
    }
    UpdateAllViews();
  } else {
    clearAxis(0);
  }
}

bool OIV2DHistoryScene::leftAxisFilled() const { return m_graph->getNumCurves(0) != 0; }

void OIV2DHistoryScene::clearRightAxis() { clearAxis(1); }

bool OIV2DHistoryScene::rightAxisFilled() const { return m_graph->getNumCurves(1) != 0; }

QString OIV2DHistoryScene::leftAxisUnit() const {
  return static_cast<OIV2DHistoryGraph *>(m_graph)->getGroupUnitName(0);
}
QString OIV2DHistoryScene::rightAxisUnit() const {
  return static_cast<OIV2DHistoryGraph *>(m_graph)->getGroupUnitName(1);
}

namespace {
geo::CValue getValue(const std::vector<DepletionStageValuePair> &valueMap, const CDepletionStage *stage) {
  auto iter = std::find_if(valueMap.begin(), valueMap.end(),
                           [stage](const DepletionStageValuePair &pair) { return pair.first == stage; });

  if (iter != valueMap.end())
    return iter->second;
  else
    return geo::CValue();
}
} // namespace

void OIV2DHistoryScene::copyDataToClipboard() {
  QString clip;

  // Collect all stages
  std::vector<const CDepletionStage *> stages;
  const CDepletionStage *stage = &static_cast<CModelBase *>(GetGeomecDoc()->Model())->InitialDepletionStage();
  while (stage != 0) {
    stages.push_back(stage);
    stage = (stage->Last()) ? 0 : &stage->Next();
  }

  for (auto p : m_analysisPoints) {
    clip += QString("Analysis point \"%1\": Northing = %2, Easting = %3, Depth = %4\r\n\r\n")
                .arg(p->Name())
                .arg(p->Point().X(), 0, 'f', 0)
                .arg(p->Point().Y(), 0, 'f', 0)
                .arg(p->Point().Z(), 0, 'f', 0);

    // Print header line
    clip += "Depletion stage";
    // Print names of value components
    for (auto vc : m_valueComponents) {
      clip += QString("\t%1 [%2]").arg(vcbfunctions::getResultPath(*vc)).arg(vc->UnitName(m_unit->Unit()));
    }
    clip += "\r\n";

    // Print values for each stage
    std::vector<std::vector<DepletionStageValuePair>> valueMaps(m_valueComponents.size());
    for (size_t i = 0; i < m_valueComponents.size(); ++i)
      OIV2DHistoryGraph::PopulateDepletionStageValueMap(valueMaps[i], m_valueComponents[i], p, m_unit->Unit());

    for (size_t i = 0; i < stages.size(); ++i) {
      const CDepletionStage *currentStage = stages[i];

      clip += QString("\"%1\"").arg(currentStage->Name());
      for (size_t j = 0; j < m_valueComponents.size(); ++j) {
        clip += "\t";
        geo::CValue value = getValue(valueMaps[j], currentStage);
        clip += value.Valid() ? QString::number(value.Value()) : "-";
      }

      clip += "\r\n";
    }

    clip += "\r\n";
  }

  CClipboardText::SetText(const_cast<char *>(clip.toStdString().c_str()));
}

unsigned int OIV2DHistoryScene::IconId() const { return 0; }

unsigned int OIV2DHistoryScene::TypeId() const { return 0; }

int OIV2DHistoryScene::SelectMode() const { return 2; }

bool OIV2DHistoryScene::CanConnectToMultipleItems(const CGraphNode &node) const {
  if (dynamic_cast<const CAnalysisPoint *>(&node) != 0)
    return NumberOfValueComponents() <= 1;

  return false;
}

bool OIV2DHistoryScene::CanConnectItem(const CGraphNode &node) const {
  if (dynamic_cast<const CAnalysisPoint *>(&node))
    return true;

  if (static_cast<const CModelBase &>(Model()).Mesh().IsMesh()) {
    const IResultComponent *pResultComponent = dynamic_cast<const IResultComponent *>(&node);
    if (pResultComponent)
      return pResultComponent->Defined() &&
             CanDisplayResultComponent(*pResultComponent); // keep order, as CanDisplay... can be very expensive when we
                                                           // have no results and many depletion stages
  }

  return false;
}

bool OIV2DHistoryScene::ConnectItem(const CGraphNode &node) {
  bool linked = false;

  if (IsLinkedTo(node))
    return false;
  if (!CanConnectItem(node))
    return false;
  const IValueComponentBase *result = dynamic_cast<const IValueComponentBase *>(&node);
  if (result != 0) {
    if (NumberOfAnalysisPoints() > 1) {
      unlinkValueComponents();
    }
    LinkTo(const_cast<IValueComponentBase &>(*result));
    linked = true;
  }

  auto analysisPoint = dynamic_cast<const CAnalysisPoint *>(&node);

  if (analysisPoint) {
    if (NumberOfValueComponents() > 1) {
      unlinkAnalysisPoints();
    }
    LinkTo(const_cast<CAnalysisPoint &>(*analysisPoint));
    linked = true;
    ;
  }

  if (linked) {
    if (m_rejectedComponent) {
      char message[128];
      sprintf(message, "No values found for '%s', ignored",
              vcbfunctions::getResultPath(*m_rejectedComponent, 0).toStdString().c_str());
      _m()->msg(message);

      UnLink(const_cast<IValueComponentBase &>(*m_rejectedComponent));
      m_rejectedComponent = 0;

      linked = false;
    }
  }

  return linked;
}

void OIV2DHistoryScene::OnNewNeighbour(const CGraphNode &node) {
  CWaitCursor cursor;

  bool modified = false;
  const IValueComponentBase *valueComponent = dynamic_cast<const IValueComponentBase *>(&node);
  if (valueComponent) {
    m_valueComponents.push_back(valueComponent);
    modified = true;

    bool curveAdded = false;
    m_rejectedComponent = 0;
    for (auto ap : m_analysisPoints) {
      if (static_cast<OIV2DHistoryGraph *>(m_graph)->addCurve(valueComponent, ap, m_unit))
        curveAdded = true;
    }

    if (!curveAdded && !m_analysisPoints.empty())
      m_rejectedComponent = valueComponent;
  }

  auto analysisPoint = dynamic_cast<const CAnalysisPoint *>(&node);
  if (analysisPoint) {
    m_analysisPoints.push_back(analysisPoint);
    modified = true;

    for (auto vc : m_valueComponents) {
      bool curveAdded = static_cast<OIV2DHistoryGraph *>(m_graph)->addCurve(vc, analysisPoint, m_unit);
    }
  }

  const CGraphPtr<CGraphNode> *graphPtr = dynamic_cast<const CGraphPtr<CGraphNode> *>(&node);
  if (graphPtr) {
    startIVTune();
  }

  if (modified) {
    updateDomains();
    updateLegend();
    Modified();
  }

  CStorageNode::OnNewNeighbour(node);
}

void OIV2DHistoryScene::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  const IValueComponentBase *valueComponent = dynamic_cast<const IValueComponentBase *>(&node);
  auto analysisPoint = dynamic_cast<const CAnalysisPoint *>(&node);
  const CGraphPtr<CGraphNode> *graphPtr = dynamic_cast<const CGraphPtr<CGraphNode> *>(&node);
  const CUnitNode *unitNode = dynamic_cast<const CUnitNode *>(&node);

  if (m_graph == 0)
    return;
  if (unitNode != 0) {
    m_graph->setUnit(unitNode->Unit());
    updateDomains();
    updateLegend();
  } else if (!CanConnectItem(node)) {
    UnLink(const_cast<CGraphNode &>(node));
  }

  else if (valueComponent || analysisPoint || graphPtr) {
    updateDomains();
    updateLegend();

    CStorageNode::OnNeighbourModified(node, uHint);
  }
  Modified();
}

void OIV2DHistoryScene::OnNeighbourDeleted(const CGraphNode &node) {
  bool modified = false;

  decltype(m_valueComponents.begin()) vcIter;
  if ((vcIter = std::find(m_valueComponents.begin(), m_valueComponents.end(), &node)) != m_valueComponents.end()) {
    for (auto ap : m_analysisPoints) {
      static_cast<OIV2DHistoryGraph *>(m_graph)->removeCurve(*vcIter, ap);
    }
    int index = vcIter - m_valueComponents.begin();
    m_valueComponents.erase(vcIter);
    modified = true;
  }

  decltype(m_analysisPoints.begin()) apIter;
  if ((apIter = std::find(m_analysisPoints.begin(), m_analysisPoints.end(), &node)) != m_analysisPoints.end()) {
    for (auto vc : m_valueComponents) {
      static_cast<OIV2DHistoryGraph *>(m_graph)->removeCurve(vc, *apIter);
    }
    m_analysisPoints.erase(apIter);
    modified = true;
  }

  const CGraphPtr<CGraphNode> *graphPtr = dynamic_cast<const CGraphPtr<CGraphNode> *>(&node);
  if (graphPtr) {
    stopIVTune();
  }
  if (modified) {
    updateDomains();
    updateLegend();
    Modified();
  }

  CStorageNode::OnNeighbourDeleted(node);
}

bool OIV2DHistoryScene::CanDisplayResultComponent(const IResultComponent &result_component) const {
  if (dynamic_cast<const CStaborResult::CStaborResultComponent *>(&result_component))
    return false;

  if (!ValueComponent(0))
    return true;

  if (!canAddUnit(result_component.UnitName()))
    return false;

  IResultComponent *currentResultComponent = dynamic_cast<IResultComponent *>(ValueComponent(0));
  assert(currentResultComponent);

  IResult *pResult = dynamic_cast<IResult *>(result_component.parent());
  assert(pResult);

  IResultComponent *pRes;
  const CDepletionStage *initialStage = &currentResultComponent->Stage();

  for (const CDepletionStage *stage = initialStage; /* empty */; stage = &stage->Next()) {
    assert(stage);
    pRes = pResult->ResultComponent(*stage, currentResultComponent->AnalysisType(), result_component.RegisterIndex(),
                                    result_component.ComponentIndex());

    if (pRes && pRes->Defined())
      return true;
    if (stage->Last())
      return false;
  }

  return false;
}

bool OIV2DHistoryScene::Empty() const { return true; }

long OIV2DHistoryScene::SavedItems() const { return 0; }

void OIV2DHistoryScene::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  OIV2DGraphValueColorScene::SaveStream(stream, progress);
}

void OIV2DHistoryScene::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  progress.StatusMessage("Loading History Scene ");
  OIV2DGraphValueColorScene::LoadStream(stream, version, progress);
}

bool OIV2DHistoryScene::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) {
  if (OIV2DGraphValueColorScene::MousePress(state, button, point))
    return true;

  if (button == Qt::RightButton) {
    if (!m_graph)
      return false;

    showContextMenu(point);

    return true;
  }

  return false;
}

bool OIV2DHistoryScene::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) {
  if (OIV2DGraphValueColorScene::MouseMove(state, button, point))
    return true;

  return UpdateLabel(point);
}

void OIV2DHistoryScene::showContextMenu(const TScreenPoint &point) {
  typedef CSingleCommandTemplate<OIV2DHistoryScene> TSceneCommand;
  CContextMenuInvoker invoker;

  TSceneCommand *markerCmd =
      new TSceneCommand(*this, &OIV2DHistoryScene::toggleMarkers, 0, &OIV2DHistoryScene::getMarkersVisible);
  TSceneCommand *clipboardCmd = new TSceneCommand(*this, &OIV2DHistoryScene::copyDataToClipboard);

  TSceneCommand *clearLeftCmd =
      new TSceneCommand(*this, &OIV2DHistoryScene::clearLeftAxis, &OIV2DHistoryScene::leftAxisFilled, 0);
  TSceneCommand *clearRightCmd =
      new TSceneCommand(*this, &OIV2DHistoryScene::clearRightAxis, &OIV2DHistoryScene::rightAxisFilled, 0);
  TSceneCommand *tryLeftAxisCmd =
      new TSceneCommand(*this, &OIV2DHistoryScene::preferTopOrLeftAxis, &OIV2DHistoryScene::canPreferTopOrLeftAxis,
                        &OIV2DHistoryScene::topOrLeftAxisPreferred);
  TSceneCommand *tryRightAxisCmd =
      new TSceneCommand(*this, &OIV2DHistoryScene::preferBottomOrRightAxis,
                        &OIV2DHistoryScene::canPreferBottomOrRightAxis, &OIV2DHistoryScene::bottomOrRightAxisPreferred);

  invoker.AddCommand(IDS_GRAPH_SCENE_MARKERS, *markerCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_COPY_DATA, *clipboardCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_CLEAR_LEFT, *clearLeftCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_CLEAR_RIGHT, *clearRightCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_PREFER_LEFT_AXIS, *tryLeftAxisCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_PREFER_RIGHT_AXIS, *tryRightAxisCmd);

  // Show context menu using global screen coordinates
  POINT p = {point.x(), getHeight() - point.y()};
  GetGeomecDoc()->GetModelView()->ClientToScreen(&p);
  invoker.TrackMenu(p.x, p.y, *FemAppGetMainWnd());
}

size_t OIV2DHistoryScene::NumberOfValueComponents() const { return m_valueComponents.size(); }

IValueComponentBase *OIV2DHistoryScene::ValueComponent(size_t index) const {
  if (index < NumberOfValueComponents()) {
    return const_cast<IValueComponentBase *>(m_valueComponents[index]);
  } else
    return 0;
}

bool OIV2DHistoryScene::MultipleValueComponentsAllowed() const { return NumberOfAnalysisPoints() <= 1; }

size_t OIV2DHistoryScene::NumberOfAnalysisPoints() const { return m_analysisPoints.size(); }

CAnalysisPoint *OIV2DHistoryScene::AnalysisPoint(size_t index) const {
  if (index < NumberOfAnalysisPoints()) {
    return const_cast<CAnalysisPoint *>(m_analysisPoints[index]);
  } else
    return 0;
}

// determine if a property with the specified unit can be added to the graph
bool OIV2DHistoryScene::canAddUnit(QString unitName) const {
  return (leftAxisUnit() == "empty" || leftAxisUnit() == unitName || rightAxisUnit() == "empty" ||
          rightAxisUnit() == unitName);

  //     std::vector<QString> unitNames;
  //     for (size_t i = 0; i < NumberOfValueComponents(); ++i)
  //     {
  //         IResultComponent * currentResultComponent = dynamic_cast<IResultComponent  *> (ValueComponent(i));
  //         assert (currentResultComponent);
  //         if (currentResultComponent->UnitName() == unitName)
  //             return true;
  //         if (unitNames.size() < 2 && std::find (unitNames.begin(), unitNames.end(),
  //         currentResultComponent->UnitName()) == unitNames.end())
  //             unitNames.push_back(currentResultComponent->UnitName());
  //     }
  //     return unitNames.size() < 2;
}

void OIV2DHistoryScene::setSize(int width, int height) {
  OIV2DGraphValueColorScene::setSize(width, height);

  updateGraph();
  viewAll();
  Modified();
}

bool OIV2DHistoryScene::MultipleStagesAllowed() const { return false; }

void OIV2DHistoryScene::SaveColorToStream(QRgb (OIV2DHistoryScene::*func_ptr)() const, TSTREAM &stream) {
  int iTemp;
  QColor color = QColor((this->*func_ptr)());
  iTemp = color.red();
  stream << iTemp;
  iTemp = color.green();
  stream << iTemp;
  iTemp = color.blue();
  stream << iTemp;
}

void OIV2DHistoryScene::RestoreColorFromStream(void (OIV2DHistoryScene::*func_ptr)(QRgb), TSTREAM &stream) {
  int R, G, B;
  stream >> R >> G >> B;
  (this->*func_ptr)(qRgb(R, G, B));
}
