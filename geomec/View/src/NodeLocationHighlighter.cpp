#include "NodeLocationHighlighter.h"
#include "geomec.h"
#include "stdafx.h"

#include "FemAppMainWindow.h"
#include "MainFrm.h"
#include "OpenGLSceneBase.h"
#include "OpenGLView.h"

const int HIT_DELTA = 5;

static CView *GetParentView(IOpenGLFrame &frame) {
  COpenGLView *pView = 0;
  IOpenGLFrame *pFrame = &frame;
  while (pFrame && !pView) {
    pView = dynamic_cast<COpenGLView *>(pFrame);
    pFrame = pFrame->Parent();
  }

  return pView;
}

CNodeLocationHighlighter::CNodeLocationHighlighter(COpenGLSceneBase &scene) : m_scene(scene), m_pPointHighlightNode(0) {
  m_tooltip.Create(GetParentView(scene), TTS_ALWAYSTIP | TTS_NOPREFIX);
}

CNodeLocationHighlighter::~CNodeLocationHighlighter() {
  if (m_pPointHighlightNode)
    delete m_pPointHighlightNode;
}

bool CNodeLocationHighlighter::MouseMove(const IOpenGLFrame::TScreenPoint &point, bool bShowLocation) {
  const CModelBase &model = static_cast<const CModelBase &>(m_scene.Model());
  if (bShowLocation && model.SceneCanDraw()) {
    IOpenGLFrame::TObjectVec vcSelection = m_scene.ProcessSelection(point);

    TNodeSet stNodes;

    for (size_t i = 0; i < vcSelection.size(); ++i) {
      ProcessEntry(*vcSelection[i], stNodes);
    }

    typedef std::map<double, const geo::INode *> TNodeSquareDistMap;
    TNodeSquareDistMap mpNodes;

    TNodeSet::iterator it;
    for (it = stNodes.begin(); it != stNodes.end(); ++it) {
      IOpenGLFrame::TScreenPointDepth ptNode = m_scene.Project(**it);
      int dx = abs(ptNode.first.x() - point.x());
      int dy = abs(ptNode.first.y() - point.y());
      double z = ptNode.second;
      if (dx < HIT_DELTA && dy < HIT_DELTA) {
        TRACE("Insert (%d, %d, %g)\n", dx, dy, z);
        mpNodes.insert(TNodeSquareDistMap::value_type(dx * dx + dy * dy + z * z, *it));
      }
    }

    const geo::INode *pNode = (mpNodes.size() > 0) ? mpNodes.begin()->second : 0;

    if (m_pPointHighlightNode) {
      if (&m_pPointHighlightNode->Node() != pNode) {
        delete m_pPointHighlightNode;
        m_pPointHighlightNode = 0;

        if (pNode)
          m_pPointHighlightNode = new CPointHighlightNode(*pNode, m_scene);
      } else {
        m_pPointHighlightNode->UpdateStatusBarText();
      }
    } else if (pNode) {
      m_pPointHighlightNode = new CPointHighlightNode(*pNode, m_scene);
    }
  } else {
    if (m_pPointHighlightNode) {
      delete m_pPointHighlightNode;
      m_pPointHighlightNode = 0;
    }
  }

  return (m_pPointHighlightNode != 0);
}

void CNodeLocationHighlighter::ProcessEntry(const geo::IObject &object, TNodeSet &stNodes) {
  const geo::INode *pNode = dynamic_cast<const geo::INode *>(&object);
  if (pNode) {
    stNodes.insert(pNode);
    return;
  }

  const geo::IElement *pElement = dynamic_cast<const geo::IElement *>(&object);
  if (pElement) {
    int i;
    for (i = 0; i < pElement->NrOfNodes(); ++i)
      stNodes.insert(&pElement->Node(i));
    return;
  }

  const geo::IElementSet *pElementSet = dynamic_cast<const geo::IElementSet *>(&object);
  if (pElementSet) {
    int i;
    for (i = 0; i < pElementSet->PointSize(); ++i)
      ProcessEntry(pElementSet->Point(i), stNodes);
  }

  const geo::IArray *pArray = dynamic_cast<const geo::IArray *>(&object);
  if (pArray) {
    int i;
    for (i = 0; i < pArray->Size(); ++i)
      ProcessEntry(pArray->ObjectBase(i), stNodes);
  }
}

/////

CNodeLocationHighlighter::CPointHighlightNode::CPointHighlightNode(const geo::INode &node, COpenGLSceneBase &scene)
    : COpenGLNode(scene.Model()), m_node(node), m_scene(scene), m_pView(0) {
  UpdateStatusBarText();
  LinkTo(scene);
}

CNodeLocationHighlighter::CPointHighlightNode::~CPointHighlightNode() {
  CMainFrame &mainframe = (CMainFrame &)(*FemAppGetMainWnd());
  mainframe.m_wndStatusBar.SetPaneText(1, "");
}

const geo::INode &CNodeLocationHighlighter::CPointHighlightNode::Node() const { return m_node; }

void CNodeLocationHighlighter::CPointHighlightNode::UpdateStatusBarText() {
  // create the label
  CString strLabel;
  CFemAppDoc &doc = *GetGeomecDoc();
  CLengthQuantity qn;
  double dNorthing = qn.Convert(m_node.X(), doc.UnitNode().Unit(), CQuantity::SI_UNIT);
  double dEasting = qn.Convert(m_node.Y(), doc.UnitNode().Unit(), CQuantity::SI_UNIT);
  double dDepth = qn.Convert(m_node.Z(), doc.UnitNode().Unit(), CQuantity::SI_UNIT);

  strLabel.Format("N %.0f, E %.0f, Z %.0f, Index %d", dNorthing, dEasting, dDepth, m_node.Index() + 1);

  CMainFrame &mainframe = (CMainFrame &)(*FemAppGetMainWnd());

  int nNeededWidth = mainframe.m_wndStatusBar.GetNeededWidth(strLabel);

  mainframe.m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_COOR, SBPS_NORMAL, nNeededWidth);
  mainframe.m_wndStatusBar.SetPaneText(1, strLabel);
}

COpenGLNode::CDrawDef *
CNodeLocationHighlighter::CPointHighlightNode::OnCreateDrawDef(const geo::IObject &object) const {
  return new CPointDrawDef(*this);
}

int CNodeLocationHighlighter::CPointHighlightNode::DisplayListSize() const { return 1; }

const geo::IObject &CNodeLocationHighlighter::CPointHighlightNode::DisplayList(int nIndex) const { return m_node; }

COpenGLNode::TColor CNodeLocationHighlighter::CPointHighlightNode::Color() const {
  return QColor(255, 0, 0).rgb(); // red
}

unsigned int CNodeLocationHighlighter::CPointHighlightNode::IconId() const { return 0; }

unsigned int CNodeLocationHighlighter::CPointHighlightNode::TypeId() const { return 0; }

bool CNodeLocationHighlighter::CPointHighlightNode::Empty() const { return true; }

long CNodeLocationHighlighter::CPointHighlightNode::SavedItems() const { return 0; }

/////

CNodeLocationHighlighter::CPointHighlightNode::CPointDrawDef::CPointDrawDef(const COpenGLNode &node)
    : COpenGLNode::CDrawDef(node) {}

CNodeLocationHighlighter::CPointHighlightNode::CPointDrawDef::DrawDecisionFloat
CNodeLocationHighlighter::CPointHighlightNode::CPointDrawDef::PointSize() const {
  return std::make_pair<DrawDecisionFloat::first_type, DrawDecisionFloat::second_type>(true, 8.0);
}

bool CNodeLocationHighlighter::CPointHighlightNode::Value() const // wjrx mantis 2568
{
  return false;
}
