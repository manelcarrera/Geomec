// OpenGLSceneBase.cpp: implementation of the COpenGLValueScene class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "openglnode.h"
#include "OpenGLComposite.h" 
#include "OpenGLSceneNode.h"
#include "unitnode.h"
#include "ivaluecomposite.h"
#include "ivaluecomponent.h"
#include "IColorScaleNode.h"
#include "IColorScaleEntry.h"
#include "OpenGLValueScene.h"
#include "FemAppEntryTypes.h"
#include "FemAppModel.h"
#include "FemAppDoc.h"
#include "GraphMainFrm.h"
#include "GeomecDoc.h"
#include "GraphMainFrameProgress.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLValueScene::COpenGLValueScene(CFemAppModel &model)
: COpenGLSceneNode(model), 
  m_pUnit(0),
  m_pValue(0),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_bShowColor(true)

{
  init(model);
}

COpenGLValueScene::COpenGLValueScene(const QString &strName, CFemAppModel& model)
: COpenGLSceneNode(strName, model), 
  m_pUnit(0),
  m_pValue(0),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_bShowColor(true)
{
  init(model);
}

COpenGLValueScene::COpenGLValueScene(const unsigned int uName, CFemAppModel& model)
: COpenGLSceneNode(uName, model), 
  m_pUnit(0),
  m_pValue(0),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_bShowColor(true)
{
  init(model);
}

COpenGLValueScene::COpenGLValueScene(const COpenGLValueScene& rhs)
: COpenGLSceneNode(rhs),
  m_pUnit(rhs.m_pUnit),
  m_pValue(rhs.m_pValue),
  m_pComposite(rhs.m_pComposite),
 	m_pColorScale(rhs.m_pColorScale),
  m_pColorScaleEntry(rhs.m_pColorScaleEntry),
  m_bShowColor(rhs.m_bShowColor)
{
}



COpenGLValueScene::~COpenGLValueScene()
{
}

bool COpenGLValueScene::CanConnectItem(const CGraphNode &item) const
{
  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&item);
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&item);

  if(pComposite)
  {
    assert(pComponent == 0);
    if((pComposite->ModeSize() == 1) && (pComposite->ComponentSize() == 1))
      pComponent = &pComposite->Component();
  }

  if(pComponent)
  {
    // Can we map at least one node to the scene?
    TOpenGLNodeSet stNode = NodesInScene();
    TOpenGLNodeSet::iterator itNode;
    for(itNode = stNode.begin(); itNode != stNode.end(); itNode++)
    {
      if(pComponent->CanMap(**itNode))
        return !IsLinkedTo(*pComponent) && pComponent->Defined();
    }
  }

  return COpenGLSceneNode::CanConnectItem(item);
}

bool COpenGLValueScene::ConnectItem(const CGraphNode &node) 
{
  assert(CanConnectItem(node));
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&node);
  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&node);

  if(pComposite)
  {
    if((pComposite->ModeSize() == 1) && (pComposite->ComponentSize() == 1))
    {
      LinkTo((CGraphNode&)*pComposite);
      return true;
    }
  }

  if(pComponent)
  {
    LinkTo((CGraphNode&)(*pComponent));
  }

  return COpenGLSceneNode::ConnectItem(node);
}

void COpenGLValueScene::OnNewNeighbour(const CGraphNode &node)
{
 	// Let the graphnode do it's work ...
  COpenGLSceneNode::OnNewNeighbour(node);

  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&node);
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&node);
  const IColorScaleNode* pColorScale = dynamic_cast<const IColorScaleNode*>(&node);

  if(pColorScale)
  {
    if(m_pColorScale)
    {
      UnLink(*m_pColorScale);
      assert(m_pColorScale == 0);
    }

    m_pColorScale = const_cast<IColorScaleNode*>(pColorScale);

    if(ShowValue())
      InvalidateAll();

    UpdateScene();
  }

  // Single component, single mode composite's
  if(pComposite)
  {
    assert(pComposite->ModeSize() == 1);
    assert(pComposite->ComponentSize() == 1);
    assert(pComponent == 0);
  m_pComposite = const_cast<IValueComposite*>(pComposite);
    LinkTo(m_pComposite->Component());
    assert(m_pValue == &pComposite->Component());
  }

  if(pComponent)
  {
    COperation operation(Model());
    assert(pComponent->Defined());
    
    if(m_pValue)
    {
      // Set the component first to zero to prevent a switch to depth...
      IValueComponentBase* pDelete = m_pValue;
      m_pValue = 0;
      UnLink(*pDelete);
    }
    
    assert(m_pValue == 0);

    InvalidateMinMaxCache(0);
    
    m_pValue = const_cast<IValueComponentBase *> (pComponent);

    assert(m_pValue);
    if (!IsLinkedTo(m_pColorScaleEntry->CurrentColorScale()))
      LinkTo(m_pColorScaleEntry->CurrentColorScale());

    if(ShowColor())
      ShowValue(TRUE);			// Automaticly switch to value ....
    else
    {
      // We already show value, but we have to build the cache enz.
      RecalcMinMax();
      InvalidateAll();
      UpdateScene();
    }
  }
}

void COpenGLValueScene::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  assert(IsLinkedTo(node));
  
  // Unit modified
  if((&node == m_pUnit) && ShowValue())
  {
    InvalidateMinMaxCache(0);
    RecalcMinMax();
    InvalidateAll();
    UpdateScene();
  }

  if(&node == m_pComposite && !m_pComposite->IsCopied())
  {
  const IValueComponentBase& comp = m_pComposite->Component();
  if(&comp != m_pValue)
  {
      // composite switched active component
      IValueComponentBase* pValue = m_pValue;
      m_pValue = 0;
      UnLink(*pValue);
      assert(!IsLinkedTo(*pValue));
      assert(!pValue->IsLinkedTo(*this));
      if(comp.Defined())
    LinkTo(const_cast<IValueComponentBase&>(comp));
      else
    UnLink(*m_pComposite);
      return;
  }
  else // simply process component
      OnNeighbourModified(*m_pValue, uHint);
  }

  // Quantity modified
  if((&node == m_pValue) && ShowValue())
  {
    if(m_pValue->Defined())
    {
      InvalidateMinMaxCache(0);
      RecalcMinMax();
      InvalidateAll();
      UpdateScene();
    }
    else
    {
      if(m_pComposite && &m_pComposite->Component() == m_pValue)
    UnLink(*m_pComposite);
      else
        UnLink(*m_pValue);

      // Further processing is forbidden because node is unlinked and the base class
      // could expect that the node is still connected.
      return;
    }
  }

  // One sequence of events triggered the execution of this function after
  // the destruction of MD_BASE_COLOR_SCALE.

  m_pColorScaleEntry =
  dynamic_cast <IColorScaleEntry*> (Model().GraphEntry(MD_BASE_COLOR_SCALE));

  // Color scale entry changed -> readraw
  if(m_pColorScaleEntry == &node)
  {
    if(&m_pColorScaleEntry->CurrentColorScale() != m_pColorScale)
    {
      // Unlink our current color scale
      if(m_pColorScale)
        UnLink(*m_pColorScale);

      assert(m_pColorScale == 0);

      if(!IsLinkedTo(m_pColorScaleEntry->CurrentColorScale()))
        LinkTo(m_pColorScaleEntry->CurrentColorScale());
    }
  }

  // Colorscale modified 
  if(m_pColorScale == &node)
  {
    InvalidateAll();
    UpdateScene();
  }

  COpenGLSceneNode::OnNeighbourModified(node, uHint);
}

void COpenGLValueScene::OnNeighbourDeleted(const CGraphNode &node)
{
  if(m_pValue)
  {
    if(m_pValue == &node)
    {
      // Unlink the colorscale which is based on valuecomponent
      if(IsLinkedTo(m_pValue->Parent()))
        UnLink(m_pValue->Parent());

      m_pValue = 0;
      InvalidateMinMaxCache(0);			// Invalidate the whole min/max cache
      ShowColor(TRUE);
      UpdateScene();
      CGraphNode::OnNeighbourDeleted(node);
      return;
    }

    if(&m_pValue->Parent() == &node)
    {
      if(IsLinkedTo(*m_pValue))
        UnLink(*m_pValue);
    }
  }


  // Is the color scale deleted?
  if(m_pColorScale == &node)
    m_pColorScale = 0;

  COpenGLSceneNode::OnNeighbourDeleted(node);
}

void COpenGLValueScene::OnOpenGLNodeDeleted(COpenGLNode& node)
{
  InvalidateMinMaxCache(&node);

  if(ShowValue())
  {
    if(RecalcMinMax())
      InvalidateAll();
  }
}
void COpenGLValueScene::OnOpenGLNodeModified(COpenGLNode& node, enum ModifiedHint uHint)
{
  InvalidateMinMaxCache(&node);
  if(ShowValue())
  {
    if(RecalcMinMax())
      InvalidateAll();
  }
}

void COpenGLValueScene::OnNewOpenGLNode(COpenGLNode& node)
{
  if(ShowValue())
  {
    if(RecalcMinMax(&node))
      InvalidateAll();
  }
}

BOOL COpenGLValueScene::ShowColor() const
{
  return m_bShowColor;
}

void COpenGLValueScene::ShowColor(BOOL bShowColor)
{
  ShowValue(!bShowColor);
}

BOOL COpenGLValueScene::ShowValue() const
{
  return !m_bShowColor;
}

const CUnitNode& COpenGLValueScene::UnitNode() const
{
  return *m_pUnit;
}

void COpenGLValueScene::ShowValue(BOOL bShowValue)
{
  if(bShowValue == ShowValue())
    return ;

  if(bShowValue)
  {
    if((ValueComponent() == 0) || (ColorScale() == 0))
      return;
  
    InvalidateMinMaxCache(0);
    RecalcMinMax();

    EnableLighting(FALSE);
  }
  else // wjrx mantis 2383
  {
    EnableLighting(FALSE);
  }

  m_bShowColor = !bShowValue;
  InvalidateAll();
  UpdateScene();
}


void COpenGLValueScene::Clear()
{
  COpenGLSceneNode::Clear();

  // Remove value if any ...
  if(m_pValue)
  {
    UnLink(*m_pValue);
    assert(m_pValue == 0);
  }

  // Remove color scale if any ...
  if(m_pColorScale)
  {
    UnLink(*m_pColorScale);
    assert(m_pColorScale == 0);
  }

}

int COpenGLValueScene::OnCountDrawSteps() const
{
  if(BlockDrawing())
  return 0;

  return COpenGLSceneNode::OnCountDrawSteps();
}

void COpenGLValueScene::DrawScene()
{
  if(BlockDrawing())
  return;

  COpenGLSceneNode::DrawScene();
}

const IValueComponentBase* COpenGLValueScene::ValueComponent() const
{
  return m_pValue;
}

IValueComponentBase* COpenGLValueScene::ValueComponent()
{
  return m_pValue;
}

const COpenGLValueScene::TMinMax& COpenGLValueScene::MinMaxValue() const
{
  return m_MinMaxValue;
}

void COpenGLValueScene::InvalidateMinMaxCache(const COpenGLNode *pNode)
{
  if(pNode)
  {
    TMinMaxMap::iterator it = m_mpMinMaxCache.find(pNode);
    if(it != m_mpMinMaxCache.end())
    {
      m_mpMinMaxCache.erase(it);
    }
  }
  else
    m_mpMinMaxCache.clear();
}

int COpenGLValueScene::MinMaxSteps(const COpenGLNode &node) const
{
  int nRet = 0;

  if( node.Value() ) //wjrx mantis 2568
  if(m_mpMinMaxCache.find(&node) == m_mpMinMaxCache.end()) {
    for(int i = 0; i < node.DisplayListSize(); i++) {
      nRet += ValueComponent()->MinMaxSteps( node.DisplayList(i) );
    }
  }
  return nRet;
}

const COpenGLValueScene::TMinMax& COpenGLValueScene::MinMax(IProgressBase& progressBase, const COpenGLNode &node) const
{
  TMinMaxMap::iterator it = m_mpMinMaxCache.find(&node);
  if(it == m_mpMinMaxCache.end())
  {
    // Get the display list and walk over it
    TMinMax ret;

    if ( node.Value() ) //wjrx mantis 2568
    for(int i = 0; i < node.DisplayListSize(); i++)
    {
      TMinMax min_max = ValueComponent()->ScalarData().MinMax(progressBase, node.DisplayList(i), UnitNode().Unit());
      ret.first = ret.first.Min(min_max.first);
      ret.second = ret.second.Max(min_max.second);
    }

    return m_mpMinMaxCache.insert(TMinMaxMap::value_type(&node, ret)).first->second;
  }

  return it->second;
}

BOOL COpenGLValueScene::RecalcMinMax(const COpenGLNode *pNewNode)
{
  if(BlockDrawing())
  {
  InvalidateMinMaxCache(0);
  return FALSE;
  }

  CGraphMainFrm *pFrame = dynamic_cast<CGraphMainFrm *> (FemAppGetMainWnd());
  assert(pFrame != 0);

  // We only can go here when there is value in our scene.
  assert(m_pValue);
  TMinMax MinMaxOld = m_MinMaxValue;
  TMinMax minMax;

  TOpenGLNodeSet stNode = NodesInScene();

  // when new node is inserted we can savely assume the previous minmax is the result for the rest of
  // the viewmap so initialize with the old values
  if(pNewNode && (stNode.size() > 1))
  {
    if(m_pValue->CanMap(*pNewNode))
    {
      int nSteps = MinMaxSteps( *pNewNode );
      CGraphMainFrameProgress graphMainFrameProgress;
      graphMainFrameProgress.StatusMessage("Calculate bounding values ...");
      graphMainFrameProgress.AddSteps(nSteps);
      minMax = MinMax(graphMainFrameProgress, *pNewNode);
      m_MinMaxValue.first = minMax.first.Min(m_MinMaxValue.first);
      m_MinMaxValue.second = minMax.second.Max(m_MinMaxValue.second);
    }
  }
  else
  {
    int nSteps = 0;
    TOpenGLNodeSet::iterator itNode;
    for(itNode = stNode.begin(); itNode != stNode.end(); itNode++) {
      if(m_pValue->CanMap(**itNode)) nSteps += MinMaxSteps( **itNode );
    }

    m_MinMaxValue.first = TValue();		// Set to invalid
    m_MinMaxValue.second = TValue();	// Set to invalid

    CGraphMainFrameProgress graphMainFrameProgress;

    graphMainFrameProgress.StatusMessage("Calculate bounding values ...");
    graphMainFrameProgress.AddSteps(nSteps);

    for(itNode = stNode.begin(); itNode != stNode.end(); itNode++)
    {
      if(m_pValue->CanMap(**itNode))
      {
        minMax = MinMax(graphMainFrameProgress, **itNode);
        m_MinMaxValue.first = minMax.first.Min(m_MinMaxValue.first);
        m_MinMaxValue.second = minMax.second.Max(m_MinMaxValue.second);
      }
    }
  }

  if((MinMaxOld.first == m_MinMaxValue.first) && (MinMaxOld.second == m_MinMaxValue.second))
    return FALSE;

  return TRUE;
}

void COpenGLValueScene::ColorScale(IColorScaleNode& color_scale)
{
  m_pColorScale = &color_scale;
}

const IColorScaleNode* COpenGLValueScene::ColorScale() const
{
  return m_pColorScale;
}

IColorScaleNode* COpenGLValueScene::ColorScale()
{
  return m_pColorScale;
}

bool COpenGLValueScene::operator==(const COpenGLValueScene &rhs) const
{
  if(! COpenGLSceneNode::operator==(rhs))
    return false;

  return ( (m_pValue == rhs.m_pValue) &&
       (m_pColorScale == rhs.m_pColorScale) &&
       (m_MinMaxValue == rhs.m_MinMaxValue));
}

COpenGLValueScene& COpenGLValueScene::operator=(const COpenGLValueScene& rhs)
{
  COpenGLSceneNode::operator=(rhs);
  m_pValue = rhs.m_pValue;
  m_pColorScale = rhs.m_pColorScale;
  m_MinMaxValue = rhs.m_MinMaxValue;
  return *this;
}

std::vector<CDrawDef::TColor> COpenGLValueScene::OnColor(const COpenGLNode& node, const geo::IObject &object) const
{
  //wjrx mantis 2568 if(ShowColor())
  if ( ShowColor() || !node.Value() ) //wjrx mantis 2568
    return COpenGLSceneNode::OnColor(node, object);

  // Value component should exist
  assert(ValueComponent());

  if(!ValueComponent()->CanMap(node))
    return COpenGLSceneNode::OnColor(node, object);

  const geo::IElement *pElement = dynamic_cast<const geo::IElement*>(&object);
  const geo::IPoint *pPoint	  = dynamic_cast<const geo::IPoint*>(&object);

  std::vector<CDrawDef::TColor> vcColor;
  if(pElement)
  {
    IValueDomainScalar::TValueVec vcRes =  ValueComponent()->ScalarData().ValueElement(*pElement, UnitNode().Unit());
    assert(vcRes.size() == pElement->NrOfPoints());

    vcColor.resize(vcRes.size());
    for(size_t i = 0; i < vcRes.size(); i++)
      vcColor[i] = m_pColorScaleEntry->CurrentColorScale().Color(vcRes[i], MinMaxValue());
  }
  else if(pPoint)
  {
    CDrawDef::TColor col = m_pColorScaleEntry->CurrentColorScale().Color(ValueComponent()->ScalarData().ValuePoint(*pPoint, UnitNode().Unit()), MinMaxValue());
    vcColor.push_back(col);
  }
  else
  {
    assert(FALSE);
  }

  return vcColor;
}

void COpenGLValueScene::OnSelect(CGraphNode& node)
{

}

/*!
  The override remove nodes from the index list that are display with a value.
*/
bool COpenGLValueScene::OnInsertNodeInLegend(const COpenGLNode& node) const
{
  if(ShowValue() && ValueComponent() != 0)
    return !ValueComponent()->CanMap(node);
  return true;
}

// private

void COpenGLValueScene::init(CFemAppModel& model)
{
  // Needing the unit node
  CFemAppDoc *pDoc = GetGeomecDoc();
  assert(pDoc);
  m_pUnit = &pDoc->UnitNode();
  assert(m_pUnit);
  LinkTo(*m_pUnit);

  // Link to color scale entry
  m_pColorScaleEntry = dynamic_cast<IColorScaleEntry*>(Model().GraphEntry(MD_BASE_COLOR_SCALE));
  LinkTo(*m_pColorScaleEntry);

  // Link to the current color scale
  LinkTo(m_pColorScaleEntry->CurrentColorScale());
  assert(&m_pColorScaleEntry->CurrentColorScale() == m_pColorScale);
}
