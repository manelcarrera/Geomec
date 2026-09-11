#include "stdafx.h"

#include <Inventor/nodes/SoGroup.h>

#include "Clipboard.h"
#include "ContextMenuInvoker.h"
#include "FemAppDoc.h"
#include "FemAppMainWindow.h"
#include "GeomecDoc.h"
#include "GlobalMessage.h"
#include "ModelView.h"
#include "NewWellPath.h"
#include "NewWellPathCurve.h"
#include "OIV2DGraph.h"
#include "OIV2DLegend.h"
#include "OIV2DWellPathGraph.h"
#include "OIV2DWellPathGraphScene.h"
#include "OIV2DWellPathLegend.h"
#include "Result.h"
#include "vcbfunctions.h"

bool OIV2DWellPathGraphScene::operator==(const OIV2DWellPathGraphScene &node) const { return false; }

void OIV2DWellPathGraphScene::unlinkWellPaths() {
  std::vector<const CNewWellPath *> newWellPathsCopy(m_newWellPaths);
  for (auto wellPath : newWellPathsCopy)
    UnLink(const_cast<CNewWellPath &>(*wellPath));

  assert(m_newWellPaths.empty());
}

void OIV2DWellPathGraphScene::unlinkValueComponents() {
  // Iterate over a copy of m_valueComponents, since UnLink() will remove
  // elements from m_valueComponents while iterating
  std::vector<const IValueComponentBase *> components = m_valueComponents;
  for (auto p : components)
    UnLink(*const_cast<IValueComponentBase *>(p));

  assert(m_valueComponents.empty());
}

void OIV2DWellPathGraphScene::updateGraph() {
  if (!m_newWellPaths.empty()) {
    SoGroup *root = getSceneGraph();
    if (m_graph != 0)
      root->removeChild(m_graph);

    OIV2DWellPathGraph *wellPathGraph = new OIV2DWellPathGraph(m_unit->Unit(), getAspect(), m_preferredCurveGroup);
    m_graph = wellPathGraph;

    for (auto wellPath : m_newWellPaths)
      wellPathGraph->addWellPath(wellPath);

    for (auto valueComponent : m_valueComponents)
      wellPathGraph->addValueComponent(valueComponent);

    wellPathGraph->showFormationLines(m_formationLinesVisible);

    root->addChild(m_graph);

    OIV2DGraphValueColorScene::updateGraph();
  }
}

const CGraphNode *getMainGroup(const IValueComponentBase *valueComponent);

void OIV2DWellPathGraphScene::updateLegend() {
  if (!m_legend)
    return;
  m_legend->clear();

  if (m_newWellPaths.size() <= 1) {
    std::string wellPathName = "Well Path: ";
    if (!m_newWellPaths.empty()) {
      static_cast<OIV2DWellPathLegend *>(m_legend)->setupFormationSections(*m_newWellPaths[0]);

      wellPathName += m_newWellPaths[0]->Name().toStdString();
    }

    m_legend->addLeader(0, wellPathName);
  } else if (!m_valueComponents.empty()) {
    assert(m_valueComponents.size() == 1);
    const CGraphNode *group = getMainGroup(m_valueComponents[0]);

    std::string legendSectionTitle = group->Name().toStdString();
    std::string resultName = vcbfunctions::getResultPath(*m_valueComponents[0], true).toStdString();

    std::string fullLeaderName = legendSectionTitle + ": " + resultName;

    fullLeaderName += " [" + m_valueComponents[0]->UnitName(m_unit->Unit()).toStdString() + "]";
    m_legend->addLeader(0, fullLeaderName);
  }

  std::string unitType = "Unit type: ";
  unitType += m_unit->UnitName(m_unit->Unit()).toStdString();
  m_legend->addLeader(0, unitType);

  if (!m_valueComponents.empty()) {
    const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(m_valueComponents[0]);
    if (resultComponent) {
      std::string analysis = "Analysis: ";
      analysis += resultComponent->AnalysisType().Label().toStdString();
      m_legend->addLeader(0, analysis);
    }
  }

  size_t numStages = countStages();
  if (numStages == 1) {
    std::string stageString = "Depletion stage: ";

    for (auto valueComponent : m_valueComponents) {
      const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(valueComponent);
      if (resultComponent) {
        stageString += resultComponent->Stage().Name().toStdString();
        break;
      }
    }
    m_legend->addLeader(0, stageString);
  }

  for (auto wellPath : m_newWellPaths) {
    for (auto valueComponent : m_valueComponents) {
      const Curve *curve = static_cast<OIV2DWellPathGraph *>(m_graph)->getCurve(wellPath, valueComponent);
      if (curve)
        addCurveToLegend(curve, numStages);
    }
  }
}

void OIV2DWellPathGraphScene::addCurveToLegend(const Curve *curve, size_t numStages) {
  QColor color = curve->getCurveColor();
  const IValueComponentBase *valueComponent = curve->valueComponent();

  std::string name;
  int legendSectionIndex = 0;
  std::string legendSectionTitle;

  if (m_newWellPaths.size() > 1) {
    const NewWellPathCurve *newWellPathCurve = dynamic_cast<const NewWellPathCurve *>(curve);

    if (newWellPathCurve) {
      legendSectionIndex = 2;
      legendSectionTitle = "Wellpaths (ps)";
      name = newWellPathCurve->getWellPathName().toStdString();
    } else {
      assert(false);
      legendSectionIndex = 3;
      legendSectionTitle = "Wellpaths of unknown type";
      name = "Unknown Wellpath";
    }
  } else if (numStages > 1 && dynamic_cast<const IResultComponent *>(valueComponent) != 0) {
    legendSectionIndex = 1;
    legendSectionTitle = "Depletion stages";
    const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(valueComponent);
    name = resultComponent->Stage().Name().toStdString();
  } else {
    const CGraphNode *group = getMainGroup(valueComponent);

    if (group->Name() == "Mesh results")
      legendSectionIndex = 1;
    if (group->Name() == "Material Parameters")
      legendSectionIndex = 2;
    else if (group->Name() == "Results")
      legendSectionIndex = 3;
    legendSectionTitle = group->Name().toStdString();

    name = curve->getFullResultPath().toStdString();
  }

  if (m_legend->getNumLines(legendSectionIndex) == 0)
    m_legend->addLeader(legendSectionIndex, legendSectionTitle);

  m_legend->addItem(legendSectionIndex, name, color.redF(), color.greenF(), color.blueF());

  curve->setLabel(QString(name.c_str()));
}

void OIV2DWellPathGraphScene::init() {
  CFemAppDoc *doc = GetGeomecDoc();
  assert(doc);
  m_unit = &doc->UnitNode();
  assert(m_unit);
  LinkTo(*m_unit);

  m_rejectedComponent = 0;

  m_formationLinesVisible = true;

  createSceneGraph();

  m_legend = new OIV2DWellPathLegend();
  getSceneGraph()->addChild(m_legend);

  m_graph = new OIV2DWellPathGraph(m_unit->Unit(), getAspect(), m_preferredCurveGroup);
  getSceneGraph()->addChild(m_graph);
}

bool OIV2DWellPathGraphScene::CanConnectValueComponent(const IValueComponentBase *component) const {
  if (!component->Defined())
    return false;

  // Only allow results that can be mapped on wellpath
  if (!((!m_newWellPaths.empty() && component->CanMap(*m_newWellPaths[0])))) {
    return false;
  }

  //     if (m_newWellPaths.size() > 1 && !m_valueComponents.empty())
  //         return false;

  bool bIsVectorComponent = component->Type() == IValueComponentBase::VECTOR;
  bool bIsTensorComponent = component->Type() == IValueComponentBase::TENSOR;

  if (bIsTensorComponent || bIsVectorComponent)
    return false;

  // Anything is allowed when there is nothing else
  if (m_valueComponents.empty()) {
    return true;
  } else {
    auto newResultComponent = dynamic_cast<const IResultComponent *>(component);
    if (newResultComponent != 0) {
      bool sameDepStage = false;
      bool sameComponent = true;

      for (auto p : m_valueComponents) {
        auto resultComponent = dynamic_cast<const IResultComponent *>(p);
        if (resultComponent != 0) {
          sameDepStage = sameDepStage || (&resultComponent->Stage() == &newResultComponent->Stage());
          sameComponent =
              sameComponent && ((&resultComponent->Parent() == &newResultComponent->Parent()) &&
                                (resultComponent->ComponentIndex() == newResultComponent->ComponentIndex()));
        }
      }

      return sameComponent ||
             (sameDepStage && static_cast<OIV2DWellPathGraph *>(m_graph)->canAddValueComponent(component));
    } else {
      return static_cast<OIV2DWellPathGraph *>(m_graph)->canAddValueComponent(component);
    }
  }
}

void OIV2DWellPathGraphScene::OnNewWellPath(const CNewWellPath *wellPath) {
  assert(std::find(m_newWellPaths.begin(), m_newWellPaths.end(), wellPath) == m_newWellPaths.end());
  static_cast<OIV2DWellPathGraph *>(m_graph)->addWellPath(wellPath);
  m_newWellPaths.push_back(wellPath);
}

void OIV2DWellPathGraphScene::UnLinkIncompatibleValueComponents(const IValueComponentBase *valueComponent) {
  // A new result component can only be shown together with the already visible components if
  // it is from the same depletion stage, or in the case of multiple depletion stages, if it
  // is the same component (but another mode) as the current ones. If not, unlink all current
  // components first.
  if (!m_valueComponents.empty()) {
    auto newResultComponent = dynamic_cast<const IResultComponent *>(valueComponent);
    auto firstResultComponent = dynamic_cast<const IResultComponent *>(m_valueComponents[0]);

    // For IResultComponents we need to check if it's the same component or depletion stage
    // as the currently displayed components.
    // obsolete here as unlinking stuff in a link cycle is not a good idea.
    if (newResultComponent) {
      bool sameDepStage = true;
      bool sameComponent = true;

      for (auto p : m_valueComponents) {
        auto resultComponent = dynamic_cast<const IResultComponent *>(p);
        if (resultComponent == 0)
          continue;

        sameDepStage = sameDepStage && (&resultComponent->Stage() == &newResultComponent->Stage());
        sameComponent = sameComponent && ((&resultComponent->Parent() == &newResultComponent->Parent()) &&
                                          (resultComponent->ComponentIndex() == newResultComponent->ComponentIndex()));

        // If it can't be shown together with the current components, then unlink those first
        if (!sameComponent && !sameDepStage) {
          unlinkValueComponents();
          break;
        }
      }
    }
  }

  updateLegend();
  Modified();
}

OIV2DWellPathGraphScene::OIV2DWellPathGraphScene(CFemAppModel &model) : OIV2DGraphValueColorScene(model) {
  Name("Wellpath Results View");

  init();
}

OIV2DWellPathGraphScene::OIV2DWellPathGraphScene(const QString &strName, CFemAppModel &model)
    : OIV2DGraphValueColorScene(strName, model) {
  init();
}

OIV2DWellPathGraphScene::OIV2DWellPathGraphScene(unsigned int uName, CFemAppModel &model)
    : OIV2DGraphValueColorScene(uName, model) {
  init();
}

OIV2DWellPathGraphScene::~OIV2DWellPathGraphScene() {}

const CUnitNode &OIV2DWellPathGraphScene::UnitNode() const { return *m_unit; }

void OIV2DWellPathGraphScene::toggleFormationLines() {
  m_formationLinesVisible = !m_formationLinesVisible;

  if (m_graph)
    static_cast<OIV2DWellPathGraph *>(m_graph)->showFormationLines(m_formationLinesVisible);
}

bool OIV2DWellPathGraphScene::canToggleFormationLines() const {
  return m_graph != 0 && static_cast<OIV2DWellPathGraph *>(m_graph)->getDepthAxis() != OIV2DWellPathGraph::DEPTH_AXIS_Z;
}

bool OIV2DWellPathGraphScene::formationLinesVisible() const { return m_formationLinesVisible; }

void OIV2DWellPathGraphScene::toggleDepthAxis() {
  if (m_graph != 0) {
    OIV2DWellPathGraph *graph = static_cast<OIV2DWellPathGraph *>(m_graph);

    graph->toggleDepthAxis();
  }
}

bool OIV2DWellPathGraphScene::canToggleDepthAxis() const {
  // Depth axis can only be toggled when displaying a single wellpath. If we have
  // zero or more than one wellpaths, the axis is always TVD
  return m_graph != 0 && (m_newWellPaths.size() == 1);
}

void OIV2DWellPathGraphScene::copyDataToClipboard() {
  QString clip = copyDataToString();
  CClipboardText::SetText(const_cast<char *>(clip.toStdString().c_str()));
}

bool OIV2DWellPathGraphScene::canCopyDataToClipboard() const {
  return ((!m_newWellPaths.empty()) && !m_valueComponents.empty());
}

QString OIV2DWellPathGraphScene::copyDataToString() const {
  OIV2DWellPathGraph *wpGraph = static_cast<OIV2DWellPathGraph *>(m_graph);

  QString clip;

  for (auto wellPath : m_newWellPaths) {
    std::vector<const NewWellPathCurve *> curves;
    curves.reserve(30);

    for (auto vc : m_valueComponents) {
      const NewWellPathCurve *curve = wpGraph->getCurve(wellPath, vc);
      curves.push_back(curve);
    }

    clip += NewWellPathCurve::getTextRepresentation(curves);
  }

  return clip;
}

unsigned int OIV2DWellPathGraphScene::IconId() const { return 0; }

unsigned int OIV2DWellPathGraphScene::TypeId() const { return 0; }

bool OIV2DWellPathGraphScene::CanConnectToMultipleItems(const CGraphNode &node) const {
  const CNewWellPath *newWellPath = dynamic_cast<const CNewWellPath *>(&node);
  if (newWellPath != 0) {
    return m_valueComponents.size() <= 1 && countStages() <= 1;
  }

  return false;
}

bool OIV2DWellPathGraphScene::CanConnectItem(const CGraphNode &item) const {
  const IValueComponentBase *result = dynamic_cast<const IValueComponentBase *>(&item);
  if (result != 0)
    return CanConnectValueComponent(result);

  const CNewWellPath *newWellPath = dynamic_cast<const CNewWellPath *>(&item);
  if (newWellPath != 0) {
    return const_cast<CNewWellPath *>(newWellPath)->GraphPoints().size() !=
           0; // these points are recalculated every time if ....size() == 0, sloppy!!!
  }

  const CDepletionStage *depletionStage = dynamic_cast<const CDepletionStage *>(&item);
  if (depletionStage != 0) {
    //         if ((m_valueComponents.size() > 1 && m_valueComponents.size() > countStages()) ||  m_newWellPaths.size()
    //         > 1)
    //             return false;
    //         else
    return true;
  }

  return false;
}

namespace {

const QString NO_VALUES_FOUND = QObject::tr("No values found for '%1', ignored");

} // anonymous namespace

bool OIV2DWellPathGraphScene::ConnectItem(const CGraphNode &node) {
  if (!CanConnectItem(node))
    return false;
  if (IsLinkedTo(node))
    return false;

  const CNewWellPath *newWellPath = dynamic_cast<const CNewWellPath *>(&node);
  if (newWellPath != 0) {
    if (!CanConnectToMultipleItems(*newWellPath))
      UnLinkAllWellPaths();
    LinkTo(const_cast<CNewWellPath &>(*newWellPath));
    return true;
  }

  const IValueComponentBase *valueComponent = dynamic_cast<const IValueComponentBase *>(&node);
  if (valueComponent != 0) {
    UnLinkIncompatibleValueComponents(valueComponent);

    if (dynamic_cast<const IResultComponent *>(&node)) {
      if (!MultipleStagesAllowed() && countStages() > 1) {
        UnLinkAllResultComponents();
      }

      if (!IsNewStage(*static_cast<const IResultComponent *>(&node)) && !MultipleValueComponentsAllowed()) {
        UnLinkAllValueComponents();
      }
    } else {
      if (!MultipleValueComponentsAllowed()) {
        UnLinkAllValueComponents();
      }
    }

    m_rejectedComponent = 0;
    LinkTo(const_cast<IValueComponentBase &>(*valueComponent));
    if (m_rejectedComponent && m_rejectedComponent == valueComponent) {
      QString message = NO_VALUES_FOUND.arg(vcbfunctions::getResultPath(*m_rejectedComponent, 0));
      _m()->msg(message);
      UnLink(const_cast<IValueComponentBase &>(*valueComponent));
    }
    return true;
  }

  return false;
}

void OIV2DWellPathGraphScene::OnNewNeighbour(const CGraphNode &node) {
  CWaitCursor cursor;

  const CNewWellPath *newWellPath = dynamic_cast<const CNewWellPath *>(&node);
  const IValueComponentBase *valueComponent = dynamic_cast<const IValueComponentBase *>(&node);
  const CGraphPtr<CGraphNode> *graphPtr = dynamic_cast<const CGraphPtr<CGraphNode> *>(&node);

  if (graphPtr) {
    startIVTune();
  } else if (newWellPath) {
    OnNewWellPath(newWellPath);
  } else if (valueComponent) {
    assert(std::find(m_valueComponents.begin(), m_valueComponents.end(), valueComponent) == m_valueComponents.end());
    m_valueComponents.push_back(valueComponent);

    if (!static_cast<OIV2DWellPathGraph *>(m_graph)->addValueComponent(valueComponent)) {
      m_rejectedComponent = valueComponent;
    }
  } else {
    return;
  }

  updateLegend();
  Modified();
}

void OIV2DWellPathGraphScene::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (m_graph == 0)
    return;
  CStorageNode::OnNeighbourModified(node, uHint);

  const CUnitNode *unitNode = dynamic_cast<const CUnitNode *>(&node);
  if (unitNode != 0) {
    m_graph->setUnit(unitNode->Unit());
    updateDomains();
    updateLegend();
  } else if (!CanConnectItem(node)) {
    UnLink(const_cast<CGraphNode &>(node));
  } else {
    const IValueComponentBase *valueComponent = dynamic_cast<const IValueComponentBase *>(&node);
    if (valueComponent)
      static_cast<OIV2DWellPathGraph *>(m_graph)->updateCurves(valueComponent);
  }
}

void OIV2DWellPathGraphScene::OnNeighbourDeleted(const CGraphNode &node) {
  // might not be an identifiable node anymore
  CStorageNode::OnNeighbourDeleted(node);

  auto wpGraph = static_cast<OIV2DWellPathGraph *>(m_graph);

  auto iter =
      std::find(m_valueComponents.begin(), m_valueComponents.end(), static_cast<const IValueComponentBase *>(&node));
  if (iter != m_valueComponents.end()) {
    if (wpGraph != 0)
      wpGraph->removeCurves(*iter);

    m_valueComponents.erase(iter);
    updateDomains();
    updateLegend();
    return;
  }

  auto nwpIter = std::find(m_newWellPaths.begin(), m_newWellPaths.end(), static_cast<const CNewWellPath *>(&node));
  if (nwpIter != m_newWellPaths.end()) {
    wpGraph->removeWellPath(*nwpIter);
    m_newWellPaths.erase(nwpIter);
    updateDomains();
    updateLegend();
    return;
  }

  const CGraphPtr<CGraphNode> *graphPtr = dynamic_cast<const CGraphPtr<CGraphNode> *>(&node);
  if (graphPtr) {
    stopIVTune();
  }
}

bool OIV2DWellPathGraphScene::Empty() const { return true; }

long OIV2DWellPathGraphScene::SavedItems() const { return 0; }

int OIV2DWellPathGraphScene::SelectMode() const { return 2; }

void OIV2DWellPathGraphScene::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  OIV2DGraphValueColorScene::SaveStream(stream, progress);
}

void OIV2DWellPathGraphScene::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  progress.StatusMessage("Loading Well Path Scene ");
  OIV2DGraphValueColorScene::LoadStream(stream, version, progress);
}

void OIV2DWellPathGraphScene::SaveColorToStream(QRgb (OIV2DWellPathGraphScene::*func_ptr)() const, TSTREAM &stream) {
  int iTemp;
  QColor color = QColor((this->*func_ptr)());
  iTemp = color.red();
  stream << iTemp;
  iTemp = color.green();
  stream << iTemp;
  iTemp = color.blue();
  stream << iTemp;
}

void OIV2DWellPathGraphScene::RestoreColorFromStream(void (OIV2DWellPathGraphScene::*func_ptr)(QRgb), TSTREAM &stream) {
  int R, G, B;
  stream >> R >> G >> B;
  (this->*func_ptr)(qRgb(R, G, B));
}

void OIV2DWellPathGraphScene::setSize(int width, int height) {
  // If this is the first time that the view gets a size, then do a viewAll() afterwards
  bool needViewAll = (getWidth() == 0 || getHeight() == 0);

  OIV2DGraphValueColorScene::setSize(width, height);

  if (m_graph) {
    m_graph->setAspect(getAspect());
    static_cast<OIV2DWellPathGraph *>(m_graph)->refreshVisualization();
    Modified();
  }

  if (needViewAll)
    viewAll();
}

bool OIV2DWellPathGraphScene::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) {
  if (OIV2DGraphValueColorScene::MousePress(state, button, point))
    return true;

  if (button == Qt::RightButton) {
    if (m_newWellPaths.empty())
      return false;

    showContextMenu(point);

    return true;
  }

  return false;
}

bool OIV2DWellPathGraphScene::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point) {
  if (OIV2DGraphValueColorScene::MouseMove(state, button, point))
    return true;

  return UpdateLabel(point);
}

void OIV2DWellPathGraphScene::showContextMenu(const TScreenPoint &point) {
  typedef CSingleCommandTemplate<OIV2DWellPathGraphScene> TSceneCommand;
  CContextMenuInvoker invoker;

  TSceneCommand *markerCmd =
      new TSceneCommand(*this, &OIV2DWellPathGraphScene::toggleMarkers, 0, &OIV2DWellPathGraphScene::getMarkersVisible);

  TSceneCommand *formationLinesCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::toggleFormationLines,
                                                       &OIV2DWellPathGraphScene::canToggleFormationLines,
                                                       &OIV2DWellPathGraphScene::formationLinesVisible);

  TSceneCommand *rangeCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::SetGraphRanges);

  TSceneCommand *clipboardCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::copyDataToClipboard,
                                                  &OIV2DWellPathGraphScene::canCopyDataToClipboard);

  TSceneCommand *clearBottomCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::clearBottomAxis,
                                                    &OIV2DWellPathGraphScene::bottomAxisFilled, 0);

  TSceneCommand *clearTopCmd =
      new TSceneCommand(*this, &OIV2DWellPathGraphScene::clearTopAxis, &OIV2DWellPathGraphScene::topAxisFilled, 0);

  TSceneCommand *toggleDepthAxisCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::toggleDepthAxis,
                                                        &OIV2DWellPathGraphScene::canToggleDepthAxis, 0);
  TSceneCommand *tryTopAxisCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::preferTopOrLeftAxis,
                                                   &OIV2DWellPathGraphScene::canPreferTopOrLeftAxis,
                                                   &OIV2DWellPathGraphScene::topOrLeftAxisPreferred);
  TSceneCommand *tryBottomAxisCmd = new TSceneCommand(*this, &OIV2DWellPathGraphScene::preferBottomOrRightAxis,
                                                      &OIV2DWellPathGraphScene::canPreferBottomOrRightAxis,
                                                      &OIV2DWellPathGraphScene::bottomOrRightAxisPreferred);

  invoker.AddCommand(IDS_GRAPH_SCENE_MARKERS, *markerCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_FORMATION_LINES, *formationLinesCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_RANGE, *rangeCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_COPY_DATA, *clipboardCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_CLEAR_TOP, *clearTopCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_CLEAR_BOTTOM, *clearBottomCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_PREFER_TOP_AXIS, *tryTopAxisCmd);
  invoker.AddCommand(IDS_GRAPH_SCENE_PREFER_BOTTOM_AXIS, *tryBottomAxisCmd);
  invoker.AddSeparator();
  invoker.AddCommand(IDS_GRAPH_SCENE_TOGGLE_DEPTH_AXIS, *toggleDepthAxisCmd);

  // Show context menu using global screen coordinates
  POINT p = {point.x(), getHeight() - point.y()};
  GetGeomecDoc()->GetModelView()->ClientToScreen(&p);
  invoker.TrackMenu(p.x, p.y, *FemAppGetMainWnd());
}

void OIV2DWellPathGraphScene::clearAxis(int group) {
  std::vector<const IValueComponentBase *> components;

  static_cast<OIV2DWellPathGraph *>(m_graph)->collectComponents(group, components);

  for (auto component : components)
    UnLink(const_cast<IValueComponentBase &>(*component));

  UpdateAllViews();
}

void OIV2DWellPathGraphScene::clearBottomAxis() { clearAxis(1); }

bool OIV2DWellPathGraphScene::bottomAxisFilled() const {
  int numCurves = m_graph->getNumCurves(1);
  return numCurves != 0;
}

void OIV2DWellPathGraphScene::clearTopAxis() { clearAxis(0); }

bool OIV2DWellPathGraphScene::topAxisFilled() const {
  int numCurves = m_graph->getNumCurves(0);
  return numCurves != 0;
}

size_t OIV2DWellPathGraphScene::NumberOfResultComponents() const {
  return std::count_if(m_valueComponents.begin(), m_valueComponents.end(),
                       [](const IValueComponentBase *valueComponent) -> bool {
                         return dynamic_cast<const IResultComponent *>(valueComponent) != 0;
                       });
}

size_t OIV2DWellPathGraphScene::NumberOfValueComponents() const { return m_valueComponents.size(); }

bool OIV2DWellPathGraphScene::MultipleValueComponentsAllowed() const {
  return m_newWellPaths.size() <= 1 && (countStages() <= 1 /* || MultipleStagesAllowed() */);
}

IValueComponentBase *OIV2DWellPathGraphScene::ValueComponent(size_t index) const {
  if (index >= m_valueComponents.size())
    return 0;

  return const_cast<IValueComponentBase *>(m_valueComponents[index]);
}

size_t OIV2DWellPathGraphScene::countStages() const {
  std::vector<const CDepletionStage *> stages;
  collectStages(stages);

  return stages.size();
}

bool OIV2DWellPathGraphScene::MultipleStagesAllowed() const {
  return NumberOfValueComponents() == countStages() && m_newWellPaths.size() <= 1;
}

void OIV2DWellPathGraphScene::UnLinkAllWellPaths() {
  while (!m_newWellPaths.empty()) {
    UnLink(*const_cast<CNewWellPath *>(m_newWellPaths[0]));
  }
}

void OIV2DWellPathGraphScene::UnLinkAllValueComponents() {
  while (!m_valueComponents.empty()) {
    UnLink(*const_cast<IValueComponentBase *>(m_valueComponents[0]));
  }
}

void OIV2DWellPathGraphScene::UnLinkAllResultComponents() {
  auto isResultComponent = [](const IValueComponentBase *component) -> bool {
    return dynamic_cast<const IResultComponent *>(component) != 0;
  };

  decltype(m_valueComponents.begin()) componentIter;
  while ((componentIter = std::find_if(m_valueComponents.begin(), m_valueComponents.end(), isResultComponent)) !=
         m_valueComponents.end()) {
    UnLink(*const_cast<IValueComponentBase *>(*componentIter));
  }
}

void OIV2DWellPathGraphScene::collectStages(std::vector<const CDepletionStage *> &stages) const {
  for (auto valueComponent : m_valueComponents) {
    auto resultComponent = dynamic_cast<const IResultComponent *>(valueComponent);
    if (resultComponent != 0) {
      if (std::find(stages.begin(), stages.end(), &resultComponent->Stage()) == stages.end()) {
        stages.push_back(&resultComponent->Stage());
      }
    }
  }
}

bool OIV2DWellPathGraphScene::IsNewStage(const IResultComponent &resultComponent) {
  std::vector<const CDepletionStage *> stages;
  collectStages(stages);
  return (!stages.empty() && std::find(stages.begin(), stages.end(), &resultComponent.Stage()) == stages.end());
}
