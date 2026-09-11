#include "stdafx.h"

#include "OpenInventorSceneNode.h"
#include "OpenInventorEventsHandler.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"
#include "GeoSurface.h"
#include "ZoominModelPlaceHolder.h"
#include "CrossSection.h"
#include "IProgressFactory.h"
#include "MeshResultTree.h"
#include "IColorScaleNode.h"
#include "hexameshregion.h"
#include "ModelView.h"
#include "ColorScaleEntry.h"
#include "OpenInventorSceneNodeAction.h"
#include "HexaBoundary.h"
#include "3dmodel.h"
#include "attrinewwellpathdlg.h"
#include "HexaMeshRegion_Delegate.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "WellCasingModel.h"
#include "treectrlbase.h"
#include "GlobalMessage.h"

//can_mesh_refinement
#include "TetraModel.h"
//#include "TetMeshCM2.h"
#include "TetraMesh.h"

COpenInventorSceneNode::COpenInventorSceneNode(CFemAppModel& model)
  : COIVSceneBase(model),
  m_pValue(0),
  m_pValueInterrupted(0),
  m_pValueBlocked(0),
  m_pIsoValueBlocked(0),
  m_forceSettings(false),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_pUnit(0),
  m_unitCurrent(),
  m_pHandler(new COpenInventorEventsHandler(*this)),
  m_meshNodeAssistant(m_pHandler, this),
  m_formationNodeAssistant(m_pHandler),
  m_allowExclusiveDisplay(true),
  m_adjustView(true),
  m_redrawing(false),
  m_steelFormation(0)
{
  init(model);
}

COpenInventorSceneNode::COpenInventorSceneNode(const QString& strName, CFemAppModel& model)
  : COIVSceneBase(strName, model),
  m_pValue(0),
  m_pValueInterrupted(0),
  m_pValueBlocked(0),
  m_pIsoValueBlocked(0),
  m_forceSettings(false),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_pUnit(0),
  m_unitCurrent(),
  m_pHandler(new COpenInventorEventsHandler(*this)),
  m_meshNodeAssistant(m_pHandler, this),
  m_formationNodeAssistant(m_pHandler),
  m_allowExclusiveDisplay(true),
  m_adjustView(true),
  m_redrawing(false),
  m_steelFormation(0)
{
  init(model);
}

COpenInventorSceneNode::COpenInventorSceneNode(unsigned int uName, CFemAppModel& model)
  : COIVSceneBase(uName, model),
  m_pValue(0),
  m_pValueInterrupted(0),
  m_pValueBlocked(0),
  m_pIsoValueBlocked(0),
  m_forceSettings(false),
  m_pComposite(0),
  m_pColorScale(0),
  m_pColorScaleEntry(0),
  m_pUnit(0),
  m_unitCurrent(),
  m_pHandler(new COpenInventorEventsHandler(*this)),
  m_meshNodeAssistant(m_pHandler, this),
  m_formationNodeAssistant(m_pHandler),
  m_allowExclusiveDisplay(true),
  m_adjustView(true),
  m_redrawing(false),
  m_steelFormation(0)
{
  init(model);
}

COpenInventorSceneNode::~COpenInventorSceneNode()
{
  if (!IsCopy())
  {
  delete m_pHandler;
  delete m_steelFormation;
  }
}

bool COpenInventorSceneNode::can_mesh_refinement()
{
  CTetraModel* pModel = dynamic_cast< CTetraModel* > ( &Model() );
  if( pModel )
  {
    CTetraMesh* mesh = static_cast< CTetraMesh* >( &pModel->Mesh() );
    CTetraMesh::eMesherType type_ = mesh->CurrentMesherType();
    if( type_ == CTetraMesh::CM2 )
      return true;
  }
  return false;
}


void COpenInventorSceneNode::Activate()
{
  m_pHandler->UpdateLegend();
}

int COpenInventorSceneNode::Width() const
{
  return m_sizeX <= 0 ? 800 : m_sizeX;
}

int COpenInventorSceneNode::Height() const
{
  return m_sizeY <= 0 ? 800 : m_sizeY;
}

void COpenInventorSceneNode::SaveColorToStream (QRgb (COpenInventorSceneNode::*func_ptr)() const, TSTREAM& stream)
{
  int iTemp;
  QColor color = QColor((this->* func_ptr)());
  iTemp = color.red();
  stream << iTemp;
  iTemp = color.green();
  stream << iTemp;
  iTemp = color.blue();
  stream << iTemp;
}

void COpenInventorSceneNode::RestoreColorFromStream (void (COpenInventorSceneNode::*func_ptr)(QRgb), TSTREAM& stream)
{
  int R,G,B;
  stream >> R >> G >> B;
  (this->* func_ptr)(qRgb(R,G,B));
}

// no loading/saving implemented for now...
bool COpenInventorSceneNode::Empty() const
{
  return true;
}

long COpenInventorSceneNode::SavedItems() const
{
  return 0;
}

void COpenInventorSceneNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  progress.Step();
  
  SaveConnectedNodes(stream, progress);

  SaveValueToStream (& COpenInventorSceneNode::PointSize, stream);
  SaveValueToStream (& COpenInventorSceneNode::Lighting, stream);
  SaveValueToStream (& COpenInventorSceneNode::DisplayCoordinateAxis, stream);
  SaveValueToStream (& COpenInventorSceneNode::DisplayGnomon, stream);
  SaveValueToStream (& COpenInventorSceneNode::DisplayPerspective, stream);
  SaveValueToStream (& COpenInventorSceneNode::Fill, stream);
  SaveValueToStream (& COpenInventorSceneNode::MeshModeSelected, stream);
  SaveValueToStream (& COpenInventorSceneNode::AllowDynamicRotate, stream);
  SaveValueToStream (& COpenInventorSceneNode::ShowBoundary, stream);
  SaveValueToStream (& COpenInventorSceneNode::ShowValuesPerElement, stream);
  SaveValueToStream (& COpenInventorSceneNode::LegendShown, stream);
  SaveValueToStream (& COpenInventorSceneNode::TrackElement, stream);
  SaveValueToStream (& COpenInventorSceneNode::ContourLineModeSelected, stream);
  SaveValueToStream (& COpenInventorSceneNode::IsoCount, stream);
  SaveValueToStream (& COpenInventorSceneNode::Transparency, stream);
  SaveValueToStream (& COpenInventorSceneNode::DraggerTimeOut, stream);
  SaveValueToStream (& COpenInventorSceneNode::IsoGap, stream);


  SaveColorToStream(&COpenInventorSceneNode::AxisColor, stream);
  SaveColorToStream(&COpenInventorSceneNode::TextColor, stream);
  SaveColorToStream(&COpenInventorSceneNode::BackgroundColor, stream);
  SaveColorToStream(&COpenInventorSceneNode::BackgroundColor2, stream);
  

  progress.Step();

}

void COpenInventorSceneNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  progress.StatusMessage("Loading Main Scene ");

  if (version > CStreamVersion(4, 1, 19))
  {
    progress.Step();

    RestoreConnectedNodes(stream, progress);

    RestoreValueFromStream(& COpenInventorSceneNode::PointSize, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::Lighting, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::DisplayCoordinateAxis, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::DisplayGnomon, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::DisplayPerspective, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::Fill, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::SelectMeshMode, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::AllowDynamicRotate, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::ShowBoundary, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::ShowValuesPerElement, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::ShowLegend, stream);

    RestoreValueFromStream(& COpenInventorSceneNode::TrackElement, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::SelectContourLineMode, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::IsoCount, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::Transparency, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::DraggerTimeOut, stream);
    RestoreValueFromStream(& COpenInventorSceneNode::IsoGap, stream);
       
    RestoreColorFromStream(static_cast<void (COpenInventorSceneNode::*)(QRgb)>(& COpenInventorSceneNode::AxisColor), stream);
    RestoreColorFromStream(static_cast<void (COpenInventorSceneNode::*)(QRgb)>(& COpenInventorSceneNode::TextColor), stream);
    RestoreColorFromStream(static_cast<void (COpenInventorSceneNode::*)(QRgb)>(& COpenInventorSceneNode::BackgroundColor), stream);
    RestoreColorFromStream(static_cast<void (COpenInventorSceneNode::*)(QRgb)>(& COpenInventorSceneNode::BackgroundColor2), stream);
       
    //**
    progress.Step();
    //**

  }
  // ...
}

bool COpenInventorSceneNode::NodeCanBeDisplayed (const COpenGLNode * oglNode) const
{
  if (!oglNode) return false;

  if (dynamic_cast<const CNewWellPath *>(oglNode) != 0) return m_meshNodeAssistant.canBeDisplayed();
  if (dynamic_cast<const CPointSet*> (oglNode) != 0) return static_cast<const CPointSet *>(oglNode)->isDrawable();
  if (dynamic_cast<const CSurfaceBase*> (oglNode) != 0) return true;
  if (dynamic_cast<const CHorizonBase*> (oglNode) != 0) return true;
  if (dynamic_cast<const CZoominModelPlaceHolder*> (oglNode) != 0) return true;
  if (dynamic_cast<const CFormationBase*> (oglNode) != 0) return true;
  
  bool notEmpty = false;
  for (int i = 0; i < oglNode->DisplayListSize(); ++i)
  {
    geo::IObject *object = const_cast<geo::IObject *>(&oglNode->DisplayList(0));
    if (!object->Empty())
    {
      notEmpty = true;
      break;
    }
  }

  return notEmpty;
}

bool COpenInventorSceneNode::CanConnectToMultipleItems(const CGraphNode& node) const
{
  return true;
}

bool COpenInventorSceneNode::CanConnectItem(const CGraphNode& item) const
{
  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&item);
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&item);

  bool mainNodeCanBeDisplayed = m_meshNodeAssistant.canBeDisplayed();

  // we can link to single component, single mode composites
  if(pComposite)
  {
    assert(pComponent == 0);
    if((pComposite->ModeSize() == 1) && (pComposite->ComponentSize() == 1))
      pComponent = &pComposite->Component();
  }

  if(pComponent)
  {
    // Can we map at least one node to the scene?
    bool canMap = false;

    if (!m_stOpenGLNodes.empty())
    {
      auto predicate = [&pComponent](const COpenGLNode * node)->bool
      {
        return pComponent->CanMap(*node);
      };
      canMap = std::find_if(m_stOpenGLNodes.begin(), m_stOpenGLNodes.end(), predicate) != m_stOpenGLNodes.end();    
    }

    if (!canMap)
    {
          auto predicate = [](const CGraphNode * node)->bool
          {
      return dynamic_cast<const CCrossSection *>(node);
          };
          canMap = std::find_if(m_stGraphNodes.begin(), m_stGraphNodes.end(), predicate) != m_stGraphNodes.end();
    }

       return canMap && pComponent->Defined() && (!IsLinkedTo(*pComponent) ||  ContourLineModeSelected());
  }

  // Entries we support in this scene if they contain opengl nodes
  const CGraphEntry* pEntry = dynamic_cast<const CGraphEntry*>(&item);
  if(pEntry)
  {
    TGraphNodeSet stNode = pEntry->GraphEntryNodes();
    for(CGraphNode::const_iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      if(IsLinkedTo(**it) || CanConnectItem(**it))
        return true;
    }
  }

  // Cross section ...
  const CCrossSection* pXSec = dynamic_cast<const CCrossSection*>(&item);
  if(pXSec)
  {
    if(!mainNodeCanBeDisplayed || IsLinkedTo(*pXSec))
      return false;

    return true;
  }

  // OpenGL nodes
  const COpenGLNode* pOpenGLNode = dynamic_cast<const COpenGLNode*>(&item);
  if (pOpenGLNode != 0)
  {
    return NodeCanBeDisplayed(pOpenGLNode);
  }

  // to find out if we can select depletion stages
  const CDepletionStage * depletionStage = dynamic_cast<const CDepletionStage*>(&item);
  if (depletionStage != 0)
    return true;

  return false;
}

bool COpenInventorSceneNode::ConnectItem(const CGraphNode& node)
{
  assert(CanConnectItem(node));

  // shortcoming in the API
  CGraphNode* pNode = const_cast<CGraphNode*>(&node);

  // Can't handle these nodes ...
  //if (dynamic_cast<const CPressureSupportNode *> (&node) != 0) return false;
  //if (dynamic_cast<const CDisplacementSupportNode *> (&node) != 0) return false;
  // Gonna try anyway

  COpenGLNode* pOpenGLNode = dynamic_cast<COpenGLNode*>(pNode);
  if(pOpenGLNode)
  {
    if (ToBeDisplayedByItself(pOpenGLNode) == EXCLUSIVE)
    {
      ClearAll();
      dehighlightMesh();
      clearValueTrackerCellFacetSet();

    }
    else if (ToBeDisplayedByItself(pOpenGLNode) == EXCLUSIVEBYTYPE)
    { 
      ClearAll (pOpenGLNode);
    }
    else
    {
      ClearNodesToBeDisplayedByItself();
    }

    LinkTo(*pOpenGLNode);
    return true;
  }


  CGraphEntry* pEntry = dynamic_cast<CGraphEntry*>(pNode);
  if(pEntry)
  {
    // We want to keep what is already there, not have it removed.
    // All the nodes that would be added (again) by the code below would already have been added by the code above.
    // I don't understand the need for doing ClearExceptResult (once!) within a loop anyway.
#if 0
    COperation oper(pEntry->Model());
    bool bInit(false);

    // Add node to scene
    CGraphEntry::TEntryNodeSet stNode = pEntry->GraphEntryNodes();
    for(CGraphEntry::TEntryNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      COpenGLNode* pOpenGLNode = dynamic_cast<COpenGLNode*>(*it);
      if(pOpenGLNode)
      {
        if(!bInit)
        {
          ClearExceptResult();
          bInit = true;
        }

        LinkTo(**it);
      }

    }
    if(!bInit)
      return false;
#endif
    return true;
  }

  IValueComponentBase *pComponent = dynamic_cast<IValueComponentBase*>(pNode);
  IValueComposite* pComposite = dynamic_cast<IValueComposite*>(pNode);
  if (pComposite)
  {
    assert(pComponent == 0);
    if((pComposite->ModeSize() == 1) && (pComposite->ComponentSize() == 1))
      pComponent = &pComposite->Component();
  }

  if(pComponent)
  {
      IValueComponentBase* pPrevComponent = m_pValue ? m_pValue : m_pValueBlocked;
      try
      {           
        LinkTo(*pComponent);
        if (pPrevComponent && IsLinkedTo(*pPrevComponent))
          UnLink(*pPrevComponent);
      }
      catch(CProgressCancel *p)
      {
        delete p;

        if (pPrevComponent && IsLinkedTo(*pPrevComponent))
        {
                  UnLink(*pPrevComponent);
        }
        if (IsLinkedTo(*pComponent))
        {
          m_pValueInterrupted = pComponent;
          UnLink(*pComponent);
          m_pValueInterrupted = 0;
        }
        m_pValue = 0;
        m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit(), true);
        ShowColor(false);
      }
      ClearInvisibleNodes();

#if 0
    // This can most likely be removed as we handle iso lines in the On*Neighbour methods; just keeping it around just in case 5.2 tests reveal its necessity
    // Even if it can't be removed, it should follow after un/linking a value component (Bug 123173)

    if (ContourLineModeSelected())
    {
      try
      { 
        CModelBase& model = static_cast<CModelBase&>(Model());
        if(model.AutomaticallyDisableDrawingFor(* pComponent))
        {
          m_pIsoValueBlocked = pComponent;
        }
        else
        {
          m_pIsoValueBlocked = 0;
          OnIsoValueComponentChanged(pComponent, m_unitCurrent.Unit());
        }
      }
      catch(CProgressCancel *p)
      {
        delete p;
      }
    }
#endif

    return true;
  }

  CMeshResult  *pMeshResult = dynamic_cast<CMeshResult*>(pNode);
  if(pMeshResult && pMeshResult->ComponentSize() > 0)
  {
    pComponent = & pMeshResult->Component(0);
    if(pComponent)
    {
      LinkTo(*pComponent);
      return true;
    }
  }

  CCrossSection* pXSec = dynamic_cast<CCrossSection*>(pNode);
  if(pXSec)
  {
    LinkTo(*pXSec);
    return true;
  }

  assert(false);

  return false;
}

void COpenInventorSceneNode::OnNewNeighbour(const CGraphNode& node)
{
  COIVSceneBase::OnNewNeighbour(node);

  const COpenGLNode* pOpenGLNode = dynamic_cast<const COpenGLNode*>(&node);
  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&node);
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&node);
  const IColorScaleNode* pColorScale = dynamic_cast<const IColorScaleNode*>(&node);
  const CCrossSection* pXSec = dynamic_cast<const CCrossSection*>(&node);
/*
  const CNewWellPath* pNewWellPath = dynamic_cast<const CNewWellPath*>(&node);
  if(pNewWellPath)
  {
    assert(m_stGraphNodes.find(&node) == m_stGraphNodes.end());
    m_stGraphNodes.insert(&node);
    m_pHandler->OnShowWellPath(*pNewWellPath);
    return;
  }
*/
  if(pOpenGLNode)
  {
    assert(m_stOpenGLNodes.find(pOpenGLNode) == m_stOpenGLNodes.end());
    m_stOpenGLNodes.insert(pOpenGLNode);
    m_pHandler->OnNewOpenGLNode(*pOpenGLNode);

    // keeping this for now to mimic current behavior, but should be done by linking valueset to this new node
    // Added: a blocked value should not be drawn
    m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit(), m_forceSettings);
    m_pHandler->ActUponNoValuesDisplayedWhenAdded();
    m_pHandler->UpdateValueTrackerDisplay();
    if (m_adjustView)
    {
          m_pHandler->AdjustView();
          m_adjustView = false;
    }

    // this was done in the event handler; adjusted for blocked values
    if (m_pValue)
          m_pHandler->OnIsoValueComponentChanged(m_pValue, m_unitCurrent.Unit());
    /* Don't update the legend if we are not redrawing (too confusing)
    else if (m_pValueBlocked)
          m_pHandler->UpdateValuesLegend(m_pValueBlocked);
    */

    m_forceSettings = false;
  }

  if(pColorScale)
  {
    if(m_pColorScale)
    {
      UnLink(*m_pColorScale);
      assert(m_pColorScale == 0);
    }

    m_pColorScale = const_cast<IColorScaleNode*>(pColorScale);

    assert(m_stGraphNodes.find(&node) == m_stGraphNodes.end());
    m_stGraphNodes.insert(&node);

    m_pHandler->OnColorScaleChanged();
  }

  // Single component, single mode composite's
  if(pComposite)
  {
    assert(pComposite->ModeSize() == 1);
    assert(pComposite->ComponentSize() == 1);
    assert(pComponent == 0);
    m_pComposite = const_cast<IValueComposite*>(pComposite);
    if (!IsLinkedTo(m_pComposite->Component()))
    {
      assert(m_stGraphNodes.find(&m_pComposite->Component()) == m_stGraphNodes.end());
      m_stGraphNodes.insert(&m_pComposite->Component());
      LinkTo(m_pComposite->Component());
    }
    else
      pComponent = & m_pComposite->Component();

    assert(m_pValue == &pComposite->Component());
  }

  if(pComponent)
  {
    COperation operation(Model());
    assert(pComponent->Defined());

    if (m_pValue != & node) // cannot unlink node while it is being linked
    {
      if(m_pValue)
      {
        // Set the component first to zero to prevent a switch to depth...
        IValueComponentBase* pDelete = m_pValue;
        m_pValue = 0;

        // if (IsLinkedTo(*pDelete))
        // UnLink(*pDelete);
      }

      assert(m_pValue == 0);

      m_pValue = const_cast<IValueComponentBase*> (pComponent);
      assert(m_pValue);
    }

    m_stGraphNodes.insert(&node);
#if 0
    else
    {
      m_pValue = m_pValue; // to be able to put a breakpoint
    }
#endif



    if (!IsLinkedTo(m_pColorScaleEntry->CurrentColorScale()))
      LinkTo(m_pColorScaleEntry->CurrentColorScale());

    CModelBase& model = static_cast<CModelBase&>(Model());
    if(model.AutomaticallyDisableDrawingFor(* m_pValue))
    {
      m_pValueBlocked = m_pValue;
      m_pIsoValueBlocked = m_pValue;
      m_pValue = 0;
      //m_pHandler->OnValueComponentChanged(0, m_unitCurrent.Unit());
      m_pHandler->UpdateValuesLegend(m_pValueBlocked);
    }
    else
    {
      m_pValueBlocked = 0;
      m_pIsoValueBlocked = 0;
      m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit(), m_forceSettings);
      m_pHandler->OnIsoValueComponentChanged(m_pValue, m_unitCurrent.Unit());

      m_forceSettings = false;
    }
  }

  if(pXSec)
  {
    assert(m_stGraphNodes.find(&node) == m_stGraphNodes.end());
    m_stGraphNodes.insert(&node);
    m_pHandler->OnShowCrossSection(*pXSec);
    AddFormationNodesToGeologyLegend(); // uses ref. counting
  }

  m_pHandler->UpdateLegend(m_pValueBlocked != 0, pColorScale != 0);
}

void COpenInventorSceneNode::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  // Unit modified
  if(&node == m_pUnit && m_unitCurrent.Unit() != m_pUnit->Unit())
  {
      if (m_pValueBlocked)
    m_forceSettings = true;
      else
    m_pHandler->OnValueComponentChanged(m_pValue, m_pUnit->Unit(), true);
      m_unitCurrent.Unit(m_pUnit->Unit());
      m_pHandler->UpdateValueTrackerDisplay();
      m_pHandler->OnUnitChanged();
  }

  const COpenGLNode* pOpenGLNode = dynamic_cast<const COpenGLNode*>(&node);
//    const CNewWellPath* pNewWellPath = dynamic_cast<const CNewWellPath*>(&node);
/*
  if(pNewWellPath)
  {
    m_pHandler->OnWellPathChanged(*pNewWellPath);
  }
  else */if(pOpenGLNode)
  {
    m_pHandler->OnOpenGLNodeModified(*pOpenGLNode, uHint);
    m_pHandler->ModifyGeologyLegend(pOpenGLNode);

    // if (uHint == MeshCleared) UnLink(const_cast<CGraphNode &> (node));
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
    if(!m_pValue->Defined())
    {
      UnLink(*m_pValue);
    }

    m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit(), true);

    if(!m_pValue)
    {
      // Further processing is forbidden because node is unlinked and the base class
      // could expect that the node is still connected.
      return;
    }
  }

  m_pColorScaleEntry =
    dynamic_cast <IColorScaleEntry*> (Model().GraphEntry(MD_BASE_COLOR_SCALE));

  // Color scale entry changed -> redraw
  bool bUpdatedColorScale = false;
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

      m_pHandler->OnColorScaleChanged();
      bUpdatedColorScale = true;
    }
  }

  // Colorscale modified 
  if(m_pColorScale == &node)
  {
    m_pHandler->OnColorScaleChanged();
    bUpdatedColorScale = true;
  }

  const CCrossSection* pXSec = dynamic_cast<const CCrossSection*>(&node);
  if(pXSec)
  {
    m_pHandler->OnCrossSectionChanged(*pXSec);
  }

  m_pHandler->UpdateLegend(m_pValueBlocked != 0);
  COIVSceneBase::OnNeighbourModified(node, uHint);

}

void COpenInventorSceneNode::OnNeighbourDeleted(const CGraphNode& node)
{
  if(&node == m_pUnit)
  {
  m_pUnit = 0;
  return;
  }

  if (m_stGraphNodes.find(&node) != m_stGraphNodes.end())
      m_stGraphNodes.erase(&node);

  if(m_pValue)
  {
    if(m_pValue == &node)
    {
      if(IsLinkedTo(m_pValue->Parent()))
        UnLink(m_pValue->Parent());

      m_pValue = 0;
      ShowColor(true);
      m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit());
      COIVSceneBase::OnNeighbourDeleted(node);
      return;
    }

    if(&m_pValue->Parent() == &node)
    {
      if(IsLinkedTo(*m_pValue))
        UnLink(*m_pValue);
    }
  }

  if (m_pValueInterrupted)
  {
    if (m_pValueInterrupted == &node)
    {
      if(IsLinkedTo(m_pValueInterrupted->Parent()))
        UnLink(m_pValueInterrupted->Parent());
    }

    if(&m_pValueInterrupted->Parent() == &node)
    {
      if(IsLinkedTo(*m_pValueInterrupted))
        UnLink(*m_pValueInterrupted);
    }
  }

  if (!CTreeCtrlBase::AllowPaint() && m_pValueBlocked)
  {
      if (m_pValueBlocked == &node)
      {
    if (IsLinkedTo(m_pValueBlocked->Parent()))
          UnLink(m_pValueBlocked->Parent());

    m_pValueBlocked = 0;
    ShowColor(true);
    m_pHandler->OnValueComponentChanged(m_pValueBlocked, m_unitCurrent.Unit());
    COIVSceneBase::OnNeighbourDeleted(node);
    return;
      }

      if (&m_pValueBlocked->Parent() == &node)
      {
    if (IsLinkedTo(*m_pValueBlocked))
          UnLink(*m_pValueBlocked);
      }
  }

  // Is the color scale deleted?
  if(m_pColorScale == &node)
  {
    m_pColorScale = 0;
    // m_pHandler->OnColorScaleChanged();
  }
/*
  const CNewWellPath* pNewWellPath = dynamic_cast<const CNewWellPath*>(&node);
  if(pNewWellPath)
  {
    m_pHandler->OnHideWellPath(*pNewWellPath);
    return;
  }
*/
  const COpenGLNode* pOpenGLNode = dynamic_cast<const COpenGLNode*>(&node);
  if(pOpenGLNode)
  {
    assert(m_stOpenGLNodes.find(pOpenGLNode) != m_stOpenGLNodes.end());
    m_stOpenGLNodes.erase(pOpenGLNode);
    m_pHandler->OnOpenGLNodeDeleted(*pOpenGLNode);
  }

  const IValueComponentBase* pComponent = dynamic_cast<const IValueComponentBase*>(&node);
  const IValueComposite* pComposite = dynamic_cast<const IValueComposite*>(&node);
  if(pComposite)
  {
    assert(pComposite->ModeSize() == 1);
    assert(pComposite->ComponentSize() == 1);
    assert(pComponent == 0);

    if (IsLinkedTo(m_pComposite->Component()))
    {
      UnLink(m_pComposite->Component());

    }
  }

  const CCrossSection* pXSec = dynamic_cast<const CCrossSection*>(&node);
  if(pXSec)
  {
    m_pHandler->OnHideCrossSection(*pXSec);
    RemoveFormationNodesFromGeologyLegend(); // uses ref. counting
  }

#if 0
  // if we have nothing to show, we still want to keep the component (and depletion stage/analysis type) selected
  if (!m_redrawing && m_stOpenGLNodes.empty() && m_pValue)
  {
      bool haveXSections = false;
      for (TLinkedGraphNodeSet::iterator it = m_stGraphNodes.begin(); it != m_stGraphNodes.end(); ++it)
      {
    if (dynamic_cast<const CCrossSection*>(*it))
    {
          haveXSections = true;
          break;
    }
      }

      if (!haveXSections)
      {
    if (IsLinkedTo(m_pValue->Parent()))
          UnLink(m_pValue->Parent());
    else if (IsLinkedTo(*m_pValue))
          UnLink(*m_pValue);
    else
          m_pValue = 0;
      }
  }
#endif

  // TEMPORARY FIX
  // We can still end up here with sliced OpenGLNodes, so we're going to check for them explicitly
  const COpenGLNode *pFakeOpenGLNode = reinterpret_cast<const COpenGLNode *>(&node);
  if (m_stOpenGLNodes.find(pFakeOpenGLNode) != m_stOpenGLNodes.end())
  {
      m_stOpenGLNodes.erase(pFakeOpenGLNode);
      m_pHandler->OnOpenGLNodeDeleted(*pFakeOpenGLNode);
  }

  m_pHandler->UpdateLegend(m_pValueBlocked != 0 || m_pColorScale == 0);
  COIVSceneBase::OnNeighbourDeleted(node);
}

void COpenInventorSceneNode::SetNonDrawing()
{
  if (m_pValue)
  {
  m_pValueBlocked = m_pValue;
  m_pIsoValueBlocked = m_pValue;
  m_pValue = 0;
  }
}

void COpenInventorSceneNode::SetDrawing()
{
  m_pValueBlocked = 0;
  m_pIsoValueBlocked = 0;
}

void COpenInventorSceneNode::SetRedrawing(bool redrawing)
{
  m_redrawing = redrawing;
}

bool COpenInventorSceneNode::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  bool ret;
  if ((ret = m_pHandler->MousePress(state, button, point)) == true)
  {
    UpdateModelView();
  }
  return ret;
}

bool COpenInventorSceneNode::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  bool ret;
  if ((ret = m_pHandler->MouseRelease(state, button, point)) == true)
  {
    UpdateModelView();
  }
  return ret;
}

bool COpenInventorSceneNode::MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  return m_pHandler->MouseDblClk(state, button, point);
}

bool COpenInventorSceneNode::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  return m_pHandler->MouseMove(state, button, point);
}

bool COpenInventorSceneNode::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  bool ret;
  if ((ret = m_pHandler->MouseWheel(state, nDelta, point)) == true)
    UpdateModelView();
  return ret;
}

bool COpenInventorSceneNode::KeyPress(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  /*if( nChar == 16 )
    return false;*/

  m_keyPressed[nChar] = true;
   
  return m_pHandler->KeyPress(state, nChar, key, nRepCount);
}

bool COpenInventorSceneNode::KeyRelease(TKeyboardModifiers state, int nChar, TKey key, int nRepCount)
{
  m_keyPressed[nChar] = false;

  //FIXME: testing
  /*if( nChar == 16 )
    return false;*/

  switch (nChar)
  {
  case 'L':
    HideLegend(!LegendHidden());
    break;
  case 'M':
    SelectMeshLinesDisplayed(!MeshLinesDisplayed());
    break;
  case 'O':
    SelectFormationOutlinesDisplayed(!FormationOutlinesDisplayed());
    break;
  case 'T':
    Transparency(Transparency() + (m_keyPressed[VK_SHIFT] ? 10 : -10));
    break;
  case VK_OEM_PLUS /* + */:
    if (m_keyPressed[VK_CONTROL]) ZoomIn();
    if (m_keyPressed[VK_SHIFT]) MouseWheel(0, 1, TScreenPoint(100, 100));
    break;
  case VK_OEM_MINUS /* - */:
    if (m_keyPressed[VK_CONTROL]) ZoomOut();
    if (m_keyPressed[VK_SHIFT]) MouseWheel(0, -1, TScreenPoint(100, 100));
    break;
  case '0':
    if (m_keyPressed[VK_CONTROL]) ZoomReset();
    if (m_keyPressed[VK_SHIFT]) ZoomReset();
    break;
  default:
    break;
  }

  return m_pHandler->KeyRelease(state, nChar, key, nRepCount);
}

void COpenInventorSceneNode::EnterView()
{
  m_pHandler->EnterView();
}

void COpenInventorSceneNode::LeaveView()
{
  m_pHandler->LeaveView();
}

void COpenInventorSceneNode::OnZoomWindow()
{
  m_pHandler->OnZoomRectangle();
}

bool COpenInventorSceneNode::CanZoomWindow() const
{
  return m_pHandler->CanZoomWindow();
}

void COpenInventorSceneNode::SwitchToTopView()
{
  m_pHandler->SwitchToTopView();
}

void COpenInventorSceneNode::DisplayPerspective(bool bPerspective)
{
  m_pHandler->DisplayPerspective(bPerspective);
}

void COpenInventorSceneNode::OnDeformation( Deformation::Data::Input deformation ){ m_pHandler->OnDeformation( deformation ); }
Deformation::Data::Input COpenInventorSceneNode::Deformation(){ return m_pHandler->Deformation(); }

bool COpenInventorSceneNode::DisplayPerspective() const
{	
  return m_pHandler->DisplayPerspective(); 
}

bool COpenInventorSceneNode::CanDisplayPerspective() const
{
  return m_pHandler->CanDisplayPerspective();
}

void COpenInventorSceneNode::ShowBoundary(bool bShow)
{
  m_pHandler->ShowBoundary(bShow);
}

bool COpenInventorSceneNode::ShowBoundary() const
{
  return m_pHandler->ShowBoundary();
}

bool COpenInventorSceneNode::CanShowBoundary() const
{
  return m_pHandler->CanShowBoundary();
}

void COpenInventorSceneNode::TrackElement(bool bShow)
{
  m_pHandler->TrackElement(bShow);
}

bool COpenInventorSceneNode::TrackElement() const
{
  return m_pHandler->TrackElement();
}

bool COpenInventorSceneNode::CanTrackElement() const
{
  return m_pHandler->CanTrackElement();
}

void COpenInventorSceneNode::AllowDynamicRotate(bool bAllow)
{
  m_pHandler->AllowDynamicRotate(bAllow);
}

bool COpenInventorSceneNode::AllowDynamicRotate() const
{
  return m_pHandler->AllowDynamicRotate();
}

bool COpenInventorSceneNode::CanAllowDynamicRotate() const
{
  return m_pHandler->CanAllowDynamicRotate();
}

void COpenInventorSceneNode::DisplayGnomon(bool bDisplay)
{
  m_pHandler->DisplayGnomon(bDisplay);
}

bool COpenInventorSceneNode::DisplayGnomon() const
{
  return m_pHandler->DisplayGnomon();
}

bool COpenInventorSceneNode::CanDisplayGnomon() const
{
  return m_pHandler->CanDisplayGnomon();
}

QRgb COpenInventorSceneNode::AxisColor() const
{
  float rgb[3];
   
  m_pHandler->GetCoordinateAxisColor (rgb);

  QRgb color = GetQColor(rgb);

  return color;
}

void COpenInventorSceneNode::AxisColor(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);

  m_pHandler->SetCoordinateAxisColor (rgb);
}

QRgb COpenInventorSceneNode::BackgroundColor() const
{
  float rgb[3];

  m_pHandler->GetBackgroundColor (rgb);

  QRgb color = GetQColor(rgb);

  return color;
}

void COpenInventorSceneNode::BackgroundColor(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);
  m_pHandler->SetBackgroundColor (rgb);

  CModelView* pView = GetGeomecDoc()->GetModelView();
  if (pView && pView->LegendView())
  {
      pView->LegendView()->BackgroundColor(color);
      pView->LegendView()->UpdateFrame();
  }
}

QRgb COpenInventorSceneNode::BackgroundColor2() const
{
  float rgb[3];

  m_pHandler->GetBackgroundColor2 (rgb);

  QRgb color = GetQColor(rgb);

  return color;
}

void COpenInventorSceneNode::BackgroundColor2(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);
  m_pHandler->SetBackgroundColor2 (rgb);

  CModelView* pView = GetGeomecDoc()->GetModelView();
  if (pView && pView->LegendView())
  {
      pView->LegendView()->BackgroundColor2(color);
      pView->LegendView()->UpdateFrame();
  }
}

QRgb COpenInventorSceneNode::TextColor() const
{
  float rgb[3];

  m_pHandler->GetTextColor (rgb);
  QRgb color = GetQColor(rgb);

  return color;
}

void COpenInventorSceneNode::TextColor(QRgb color)
{
  float rgb[3];
  GetRGBColor(rgb, color);
  m_pHandler->SetTextColor (rgb);

  CModelView* pView = GetGeomecDoc()->GetModelView();
  if (pView && pView->LegendView())
  {
      pView->LegendView()->TextColor(color);
      pView->LegendView()->UpdateFrame();
  }
}

IOpenGLFrame::DIMENSION COpenInventorSceneNode::Dimension() const
{
  return IOpenGLFrame::DIM_3D;
}

void COpenInventorSceneNode::Dimension(IOpenGLFrame::DIMENSION dim)
{

}

const double & COpenInventorSceneNode::PointSize() const
{
  return m_pHandler->PointSize();
}

void COpenInventorSceneNode::PointSize(double size)
{
  m_pHandler->PointSize (size);
}

int COpenInventorSceneNode::Transparency() const
{
  return m_pHandler->Transparency();
}

void COpenInventorSceneNode::Transparency(int transparency)
{
  m_pHandler->Transparency (transparency);
  UpdateModelView();
}

int COpenInventorSceneNode::DraggerTimeOut() const
{
  return m_pHandler->DraggerTimeOut();
}

void COpenInventorSceneNode::DraggerTimeOut(int timeOut)
{
  m_pHandler->DraggerTimeOut (timeOut);
}

int COpenInventorSceneNode::IsoCount() const
{
  return m_pHandler->IsoCount();
}

void COpenInventorSceneNode::IsoCount(int count)
{
  m_pHandler->IsoCount (count);
  UpdateModelView();
}

double COpenInventorSceneNode::IsoGap() const
{
  return m_pHandler->IsoGap();
}

void COpenInventorSceneNode::IsoGap(double gap)
{
  m_pHandler->IsoGap (gap);
  UpdateModelView();
}
void COpenInventorSceneNode::DisplayCoordinateAxis(bool bDisplay)
{
  m_pHandler->DisplayCoordinateAxis(bDisplay);
}

bool COpenInventorSceneNode::DisplayCoordinateAxis() const
{
  return m_pHandler->DisplayCoordinateAxis();
}

bool COpenInventorSceneNode::CanDisplayCoordinateAxis() const
{
  return m_pHandler->CanDisplayCoordinateAxis();
}

void COpenInventorSceneNode::SelectMeshMode( MeshMode mode )
{
  m_pHandler->SelectMeshMode( mode );
}

MeshMode COpenInventorSceneNode::MeshModeSelected() const
{
  return m_pHandler->MeshModeSelected();
}

bool COpenInventorSceneNode::CanSelectMeshMode() const
{
  return m_pHandler->CanSelectMeshMode();
}

void COpenInventorSceneNode::SelectDraggerTracker(bool yesOrNo)
{
  m_pHandler->SelectDraggerTracker(yesOrNo);
}

bool COpenInventorSceneNode::DraggerTrackerSelected() const
{
  return m_pHandler->DraggerTrackerSelected();
}

bool COpenInventorSceneNode::CanSelectDraggerTracker() const
{
  return m_meshNodeAssistant.canBeDisplayed();
}

void COpenInventorSceneNode::SelectMouseTracker(bool yesOrNo)
{
  m_pHandler->SelectMouseTracker(yesOrNo);
}

bool COpenInventorSceneNode::MouseTrackerSelected() const
{
  return m_pHandler->MouseTrackerSelected();
}

bool COpenInventorSceneNode::CanSelectMouseTracker() const
{
  return m_meshNodeAssistant.canBeDisplayed();
}

bool COpenInventorSceneNode::DraggerConfined() const
{
  if (!m_pHandler) return false;
  return m_pHandler->DraggerConfined();
}

bool COpenInventorSceneNode::CanSetDraggerConfined() const
{
  if (!m_pHandler) return false;
  return m_pHandler->CanSetDraggerConfined();
}

void COpenInventorSceneNode::SetDraggerConfined(bool yesOrNo)
{
  m_pHandler->SetDraggerConfined(yesOrNo);
}

bool COpenInventorSceneNode::LegendShown() const
{
  return !LegendHidden();
}

bool COpenInventorSceneNode::LegendHidden() const
{
  return m_pHandler->LegendHidden();
}

bool COpenInventorSceneNode::CanSelectHideLegend() const
{
  return m_pHandler->CanSelectHideLegend();
}

void COpenInventorSceneNode::ShowLegend(bool yesOrNo)
{
  HideLegend(!yesOrNo);
}

void COpenInventorSceneNode::HideLegend(bool yesOrNo)
{
  m_pHandler->HideLegend(yesOrNo);
  UpdateModelView();
}

void COpenInventorSceneNode::UpdateLegend()
{
  m_pHandler->UpdateLegend(false, true);
}

void COpenInventorSceneNode::SelectHandMode(bool yesOrNo)
{
  m_pHandler->SelectHandMode(yesOrNo);
}

bool COpenInventorSceneNode::HandMode() const
{
  return m_pHandler->HandMode();
}

bool COpenInventorSceneNode::CanSelectHandMode() const
{
  return m_pHandler->CanSelectHandMode();
}

bool COpenInventorSceneNode::ClipPlaneModeSelected() const
{
  return m_pHandler->ClipPlaneModeSelected();
}

bool COpenInventorSceneNode::CanSelectClipPlaneMode() const
{
  return m_pHandler->CanSelectClipPlaneMode();
}

void COpenInventorSceneNode::SelectClipPlaneMode(bool yesOrNo)
{
  m_pHandler->SelectClipPlaneMode(yesOrNo);
}

bool COpenInventorSceneNode::ContourLineModeSelected() const
{
  return m_pHandler->ContourLineModeSelected();
}

bool COpenInventorSceneNode::CanSelectContourLineMode() const
{
  return m_pHandler->CanSelectContourLineMode();
}

void COpenInventorSceneNode::SelectContourLineMode(bool yesOrNo)
{
  m_pHandler->SelectContourLineMode(yesOrNo);
  UpdateModelView();
}

bool COpenInventorSceneNode::MeshLinesDisplayed() const
{
  return m_pHandler->MeshLinesDisplayed();
}

bool COpenInventorSceneNode::CanSelectMeshLineDisplayMode() const
{
  return m_pHandler->CanSelectMeshLineDisplayMode();
}

void COpenInventorSceneNode::SelectMeshLinesDisplayed(bool yesOrNo)
{
  m_pHandler->SelectMeshLinesDisplayed(yesOrNo);
  UpdateModelView();
}

bool COpenInventorSceneNode::FormationOutlinesDisplayed() const
{
  return m_pHandler->FormationOutlinesDisplayed();
}

bool COpenInventorSceneNode::CanSelectFormationOutlineDisplayMode() const
  {
  return m_pHandler->CanSelectFormationOutlineDisplayMode();
  }

void COpenInventorSceneNode::SelectFormationOutlinesDisplayed(bool yesOrNo)
{
  m_pHandler->SelectFormationOutlinesDisplayed(yesOrNo);
  UpdateModelView();
}

void COpenInventorSceneNode::Fill(bool bFill)
{
  m_pHandler->Fill(bFill);
}

bool COpenInventorSceneNode::Fill() const
{
  return m_pHandler->Fill();
}

bool COpenInventorSceneNode::CanFill() const
{
  return m_pHandler->CanFill();
}

void COpenInventorSceneNode::ZoomIn()
{
  m_pHandler->ZoomIn();
  UpdateModelView();
}

void COpenInventorSceneNode::ZoomOut()
{
  m_pHandler->ZoomOut();
  UpdateModelView();
}

void COpenInventorSceneNode::ZoomReset()
{
  m_pHandler->UpdateTranslation();
  m_pHandler->ZoomReset();
  UpdateModelView();
}

void COpenInventorSceneNode::LockRotationVector(const geo::IVector& vector)
{
  m_pHandler->LockRotationVector(vector);
}

const geo::IVector& COpenInventorSceneNode::LockRotationVector() const
{
  return m_pHandler->LockRotationVector();
}

bool COpenInventorSceneNode::CanLockRotationVector() const
{
  return m_pHandler->CanLockRotationVector();
}

void COpenInventorSceneNode::ViewTensorVector( TensorVectorMode mode )
{
  m_pHandler->ViewTensorVector(mode);
  UpdateModelView();
}

TensorVectorMode COpenInventorSceneNode::ViewTensorVector()
{
  return m_pHandler->ViewTensorVector();
}

void COpenInventorSceneNode::ViewEDirection()
{
  m_pHandler->ViewEDirection();
}

void COpenInventorSceneNode::ViewNDirection()
{
  m_pHandler->ViewNDirection();
}

void COpenInventorSceneNode::ViewZDirection()
{
  m_pHandler->ViewZDirection();
}

void COpenInventorSceneNode::SaveHome()
{
  m_pHandler->SaveHome();
}
void COpenInventorSceneNode::GoHome()
{
  m_pHandler->GoHome();
}

void COpenInventorSceneNode::ViewBirdEyeDirection()
{
  m_pHandler->ViewBirdEyeDirection();
}

void COpenInventorSceneNode::ViewProportional(bool bProportional)
{
  m_pHandler->ViewProportional(bProportional);
}

bool COpenInventorSceneNode::ViewProportional() const
{
  return m_pHandler->ViewProportional();
}

void COpenInventorSceneNode::Lighting(bool bLighting)
{
  m_pHandler->Lighting(bLighting);
}

bool COpenInventorSceneNode::Lighting() const
{
  return m_pHandler->Lighting();
}

bool COpenInventorSceneNode::CanEnableLighting() const
{
  return m_pHandler->CanEnableLighting();
}

void COpenInventorSceneNode::OnSize(int cx, int cy)
{
  m_sizeX = cx;
  m_sizeY = cy;
  m_pHandler->OnViewportSizeChanged(cx, cy);
}

void COpenInventorSceneNode::ShowValuesPerElement(bool perElement)
{
  m_pHandler->ShowValuesPerElement(perElement);
  m_pHandler->OnValueComponentChanged(m_pValue ? m_pValue : m_pValueBlocked, m_unitCurrent.Unit(), true);
}

bool COpenInventorSceneNode::ShowValuesPerElement() const
{
  return m_pHandler->ShowValuesPerElement();
}

bool COpenInventorSceneNode::CanShowValuesPerElement()
{
  return m_pHandler->CanShowValuesPerElement();
}

void COpenInventorSceneNode::ViewBeachBalls(bool view)
{
  m_pHandler->ViewBeachBalls(view);
}

bool COpenInventorSceneNode::BeachBallsOnView() const
{
  return m_pHandler->BeachBallsOnView();
}

bool COpenInventorSceneNode::CanViewBeachBalls() const
{
  return m_pHandler->CanViewBeachBalls();
}

void COpenInventorSceneNode::Render()
{
  m_pHandler->Render();
}

void COpenInventorSceneNode::Clear()
{
}


bool COpenInventorSceneNode::ShowColor() const
{
  return m_pHandler->ShowColor();
}

void COpenInventorSceneNode::ShowColor(bool bShowColor)
{
  m_pHandler->ShowColor(bShowColor);
}

bool COpenInventorSceneNode::ShowValue() const
{
  return !ShowColor();
}

void COpenInventorSceneNode::ShowValue(bool bShowValue)
{
  ShowColor (!bShowValue);
}

bool COpenInventorSceneNode::CanShowValue()
  {
  return m_pHandler->CanShowValue();
  }

const IValueComponentBase* COpenInventorSceneNode::ValueComponent() const
{
  return m_pValue ? m_pValue : m_pValueBlocked;
}

IValueComponentBase* COpenInventorSceneNode::ValueComponent()
{
  return m_pValue ? m_pValue : m_pValueBlocked;
}

const IValueComposite* COpenInventorSceneNode::ValueComposite() const
{
  return m_pComposite;
}

IValueComposite* COpenInventorSceneNode::ValueComposite()
{
  return m_pComposite;
}

const IColorScaleNode* COpenInventorSceneNode::ColorScale() const
{
  return m_pColorScale;
}

const CUnitNode* COpenInventorSceneNode::UnitNode() const
{
  return m_pUnit;
}

unsigned int COpenInventorSceneNode::IconId() const
{
  return 0;
}

unsigned int COpenInventorSceneNode::TypeId() const
{
  return 0;
}

void COpenInventorSceneNode::ClearAll(CGraphNode * node)
{
  auto stNodes = Links<COpenGLNode>();

  for(auto it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    if (node && dynamic_cast<CHexaMeshRegionBase *> (node) != 0 && dynamic_cast<CHexaMeshRegionBase *> (*it) != 0)
      continue;

    assert(IsLinkedTo(**it));
    UnLink(**it);
  }

  auto stCrossSections = Links<CCrossSection>();

  for(auto it = stCrossSections.begin(); it != stCrossSections.end(); ++it)
  {
    assert(IsLinkedTo(**it));
    UnLink(**it);
  }

  //     IResultComponent *pResult = dynamic_cast<IResultComponent*> (ValueComponent());
  //     if(pResult == 0)
  {
    // If we have a quantity unlink it ...
    if(ValueComponent() && IsLinkedTo(*ValueComponent()))
    {
      UnLink(*ValueComponent());
    }
  }

  SelectDraggerTracker(false);
}

void COpenInventorSceneNode::DisconnectUnconnectables()
{
  auto stNodes = Links<COpenGLNode>();

  for(auto it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    assert(IsLinkedTo(**it));
    if (!CanConnectItem(**it))
      UnLink(**it);
    else
      OnNeighbourModified(**it, ModifiedHint::Default);
  }

  auto stCrossSections = Links<CCrossSection>();

  for(auto it = stCrossSections.begin(); it != stCrossSections.end(); ++it)
  {
    assert(IsLinkedTo(**it));
    if (!CanConnectItem(**it))
      UnLink(**it);
    else
      OnNeighbourModified(**it, ModifiedHint::Default);
  }

  //     IResultComponent *pResult = dynamic_cast<IResultComponent*> (ValueComponent());
  //     if(pResult == 0)
  {
    // If we have a quantity unlink it ...

    IValueComponentBase * vc = ValueComponent();
    if(vc && IsLinkedTo(*vc))
    {
      UnLink(*vc);
      if (CanConnectItem(*vc))
        ConnectItem(*vc);
      else
        OnNeighbourModified(*vc, ModifiedHint::Default);
    }
  }

  SelectDraggerTracker(false);
}

void COpenInventorSceneNode::ClearNodesToBeDisplayedByItself()
{
  typedef std::set<COpenGLNode*> TOpenGLNodeSet_nonconst;
  TOpenGLNodeSet_nonconst stNodes = Links<COpenGLNode>();

  for(auto node : stNodes)
  {
    if (ToBeDisplayedByItself( node ) != NONEXCLUSIVE)
    {
      assert(IsLinkedTo(*node));
      UnLink(*node);

      dehighlightMesh();
      clearValueTrackerCellFacetSet();

    }
  }
}

void COpenInventorSceneNode::ClearInvisibleNodes()
{
  typedef std::set<COpenGLNode*> TOpenGLNodeSet_nonconst;
  TOpenGLNodeSet_nonconst stNodes = Links<COpenGLNode>();

  for(auto oglNode : stNodes)
  {
    if (!IsVisible(oglNode))
    {
      char debugstr[128];
      sprintf (debugstr, "unlinking invisible node %s\n", oglNode->Name().toStdString().c_str());
      OutputDebugString(debugstr);
      assert(IsLinkedTo(*oglNode));
      UnLink(*oglNode);
    }
  }
}


void COpenInventorSceneNode::SetCursor (Qt::CursorShape shape)
{
  m_pHandler->SetCursor(shape);
}


void COpenInventorSceneNode::UpdateModelView ()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  if (!pDoc) return;

  CModelView *mv = pDoc->GetModelView();
  if (mv) mv->Invalidate();
}

void COpenInventorSceneNode::UpdateAllViews ()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  if (!pDoc) return;
  pDoc->UpdateAllViews(0, CURRENT_SCENE_MODIFIED, 0);
}

void COpenInventorSceneNode::SetVectorScale(float scale)
{
  m_pHandler->SetVectorScale(scale);
}

void COpenInventorSceneNode::SetVectorVisibleFraction(float factor)
{
  m_pHandler->SetVectorVisibleFraction(factor);
}

void COpenInventorSceneNode::init(CFemAppModel& model)
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

  Name("Main View");

  m_sizeX = 0;
  m_sizeY = 0;

  for (size_t i = 0; i < 256; ++i)
    m_keyPressed[i] = false;
}

const CColorScaleEntry * COpenInventorSceneNode::GetColorScaleEntry() const
{
  return dynamic_cast<const CColorScaleEntry*>(Model().GraphEntry(MD_BASE_COLOR_SCALE));
}

const double COpenInventorSceneNode::MinValue()
{
  return m_pHandler->MinValue();
}

const double COpenInventorSceneNode::MaxValue()
{
  return m_pHandler->MaxValue();
}

BOOL COpenInventorSceneNode::ViewComponent( TValueComponentObserver & observer )
{
  COpenGLNode *pNode = FindProjectionNode(observer);
  IValueComponentBase *pComponent = dynamic_cast<IValueComponentBase*> (&observer.ObservedItem());

  assert(pComponent);

  if(pNode)
  {
    // Don't want this, will deal with this in OIDIMeshNodeManager
    // Clear();
    if (IsLinkedTo(*pNode))
    {
      UnLink(*pNode);
    }
    LinkTo(*pNode);
    // m_pProject = pNode;
    if (IsLinkedTo(*pComponent))
    {
      UnLink(*pComponent);
    }
    LinkTo(*pComponent);
    return TRUE;
  }
  return FALSE;
}

BOOL COpenInventorSceneNode::ViewComposite( CValueCompositeObserver & observer )
{
  IValueComposite *pComposite = (IValueComposite*) (&observer.ObservedItem());

  if((pComposite->ModeSize() == 1) && (pComposite->ComponentSize() == 1) && pComposite->Component().Defined())
  {
    COpenGLNode *pNode = FindProjectionNode(observer);
    if(pNode)
    {
      // Don't want this, will deal with this in OIDIMeshNodeManager
      // Clear();

      if (IsLinkedTo(*pNode))
      {
        UnLink(*pNode);
      }
      if (IsLinkedTo(*pComposite))
      {
        UnLink (*pComposite);
      }

      if (CanConnectItem(*pNode) /* && CanConnectItem(*pComposite) */)
      {  
      ConnectItem(*pNode);           

        if (CanConnectItem(*pComposite))
      ConnectItem(*pComposite);
      }

      return TRUE;
    }
  }

  return FALSE;
}

void COpenInventorSceneNode::SetMainMeshNode()
{
  CModelBase & modelBase = static_cast<CModelBase &> (Model());
  CMeshBase & meshBase = modelBase.Mesh();
  
  TOpenGLNodeSet stNodes = m_stOpenGLNodes; // copy of linked nodes

  m_meshNodeAssistant.LinkTo(meshBase);

  // the meshNodeAssistant removes the links, but in case we got here via a 'switch model' we want them back
  for (TOpenGLNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  if (!IsLinkedTo(**it))
      LinkTo(const_cast<COpenGLNode&>(**it));
  }

}

/*void COpenInventorSceneNode::ConnectFormationNodeAssistant()
{
  assert(Model().GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();
  for(auto formationBaseNode : stNode)
  {
      if (!formationBaseNode->IsCopy())
    m_formationNodeAssistant.LinkTo(* formationBaseNode);
  }
}*/

void COpenInventorSceneNode::ConnectFormationNodeAssistant()
{
  CWellCasingModel *casingModel = dynamic_cast<CWellCasingModel *>(&Model());

  assert(Model().GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();
  for(auto formationBaseNode : stNode)
  {
      bool do_link = !formationBaseNode->IsCopy();

      // in casing, don't add the normal formations to the formationNodeAssistant
      // as then they will always be part of the cross sections
      if (casingModel)
      {
    if (dynamic_cast<C3DFormation *>(formationBaseNode))
          do_link = false;
    else if (!IsLinkedTo(*formationBaseNode))
          do_link = false;
      }

  	  if (do_link)
    	  m_formationNodeAssistant.LinkTo(*formationBaseNode);
  }

  if (casingModel && casingModel->IsMesh())
  {
      m_steelFormation = new Casing3DFormation(*casingModel, casingModel->Mesh().SteelElements());
      m_formationNodeAssistant.LinkTo(*m_steelFormation);
  }
}



void COpenInventorSceneNode::DisconnectFormationNodeAssistant(enum ModifiedHint uHint)
{

  CWellCasingModel *casingModel = dynamic_cast<CWellCasingModel *>(&Model());

  assert(Model().GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();

  // See ConnectFormationNodeAssistant: since we didn't link the normal formations to the assistant
  // when we invalidated the mesh, we need to call OnFormationNodeDeleted manually
  // otherwise OIV will try to access non-existing meshes (and crash)
  for(auto formationBaseNode : stNode)
  {
      if (!formationBaseNode->IsCopy())
      {
    if (m_formationNodeAssistant.IsLinkedTo(*formationBaseNode))
          m_formationNodeAssistant.UnLink(*formationBaseNode);
    else if (casingModel && uHint == MeshCleared && IsLinkedTo(*formationBaseNode))
          m_pHandler->OnFormationNodeDeleted(*formationBaseNode);
      }
  }

  if (m_steelFormation)
  {
      if (m_formationNodeAssistant.IsLinkedTo(*m_steelFormation))
    m_formationNodeAssistant.UnLink(*m_steelFormation);
      delete m_steelFormation;
      m_steelFormation = 0;
  }
}
COpenGLNode* COpenInventorSceneNode::FindProjectionNode(CTreeNode &observer)
{
  for(CTreeNode *pParent = &observer; pParent != 0; pParent = dynamic_cast<CTreeNode*>(pParent->Parent()))
  {
    COpenGLNode *pNode = dynamic_cast<COpenGLNode*>(&pParent->ObservedItem());
    if(pNode)
      return pNode;
  }

  return 0;
}

void COpenInventorSceneNode::ShowGeologyLegend()
{
   m_pHandler->ShowGeologyLegend();
}

void COpenInventorSceneNode::SaveConnectedNodes( TSTREAM& stream, TPROGRESS &progress )
{
  std::vector<const COpenGLNode*> vcNode;
  for(size_t i = 0; i < referenceSize(); i++)
  {
    COpenGLNode *pNode = dynamic_cast<COpenGLNode*>(&referenceAt(i));
    if(pNode && pNode->Index() != -1) vcNode.push_back( pNode );
  }
  stream << int(vcNode.size()); // Nr of indices
  for(size_t i = 0; i < vcNode.size(); i++)
  {
    stream << vcNode[i]->Index();
    progress.Step();
  }
}

void COpenInventorSceneNode::RestoreConnectedNodes( TSTREAM& stream, TPROGRESS &progress )
{
  TOpenGLNodeBaseEntry& opengl_entry = (TOpenGLNodeBaseEntry&)*Model().GraphEntry(MD_BASE_OPENGL_NODE);
  int iNrOfNodes;
  stream >> iNrOfNodes;
  int iTypeIndex;
  for(int i = 0; i< iNrOfNodes; i++)
  {
    stream >> iTypeIndex;


    // We only link from version 3.0.49 because the model is then fully load and all
    // objects are initialized.
    if (iTypeIndex != -1)
    {
          COpenGLNodeBase *pNode = opengl_entry.FindIndex(iTypeIndex);
          if (pNode && CanConnectItem(*pNode))
          {
      ScheduleConnectItem(*pNode);
          }
    }

    progress.Step();
  }
}

void COpenInventorSceneNode::unlinkValueComponent()
{
  if (m_pValue && IsLinkedTo(* m_pValue))
  {
    UnLink(* m_pValue);
  }
}

void COpenInventorSceneNode::OnIsoValueComponentChanged( IValueComponentBase * pComponent, CDoubleQuantity::UNIT unit )
{
  if (m_pHandler) m_pHandler->OnIsoValueComponentChanged( pComponent, unit );
}

void COpenInventorSceneNode::RemoveAllMeshes()
{
  if (m_pHandler) m_pHandler->RemoveAllMeshes();
}

void COpenInventorSceneNode::ResetRenderAction()
{
  if (m_pHandler) m_pHandler->ResetRenderAction();
}

bool COpenInventorSceneNode::CanRefreshVectorBaseScale()
{
  if (m_pHandler) return m_pHandler->CanRefreshVectorBaseScale(); 
  return false;
}

void COpenInventorSceneNode::RefreshVectorBaseScale()
{
  if (m_pHandler)  m_pHandler->RefreshVectorBaseScale();
}

void COpenInventorSceneNode::AllowExclusiveDisplay( bool allow )
{
  m_allowExclusiveDisplay = allow;
}

bool COpenInventorSceneNode::AllowExclusiveDisplay()
{
  return m_allowExclusiveDisplay;
}

COpenInventorSceneNode::DisplayMode COpenInventorSceneNode::ToBeDisplayedByItself( COpenGLNode* pOpenGLNode )
{
  // return NONEXCLUSIVE; // As requested by Peter Fokker

  if (!AllowExclusiveDisplay()) 
  {
    return NONEXCLUSIVE;
  }
#if 0
  else if ( dynamic_cast<CPointSet *> (pOpenGLNode) != 0
    || dynamic_cast<CPressureSupportNode *> (pOpenGLNode) != 0
    || dynamic_cast<CDisplacementSupportNode *> (pOpenGLNode) != 0
    || dynamic_cast<CHexaMesh *> (pOpenGLNode) != 0
    || dynamic_cast<CTetraMesh *> (pOpenGLNode) != 0
    || dynamic_cast<CHexaBoundary *> (pOpenGLNode) != 0
    || dynamic_cast<CTetraBoundary *> (pOpenGLNode) != 0
    || pOpenGLNode == m_meshNodeAssistant.getMeshNode())
  {
    return EXCLUSIVE;
  }
#else
  else if (
    dynamic_cast<CPressureSupportNode *> (pOpenGLNode) != 0
    || dynamic_cast<CDisplacementSupportNode *> (pOpenGLNode) != 0
    )
  {
    return EXCLUSIVE;
  }
#endif
#if 0
  // turning this off to allow other stuff to be displayed during mesh region creation (as "guides")
  else if ( dynamic_cast<CHexaMeshRegionBase *> (pOpenGLNode) != 0)
  {
    return EXCLUSIVEBYTYPE;
  }
#endif
  else
  {
    return NONEXCLUSIVE;
  }
}

bool COpenInventorSceneNode::IsVisible( COpenGLNode* pOpenGLNode )
{
  assert (m_pHandler);
  return m_pHandler->IsVisible(pOpenGLNode);
}

QRgb COpenInventorSceneNode::GetQColor( float * rgb ) const
{
  return qRgb ((int) (rgb[0] * 255), (int) (rgb[1] * 255), (int) (rgb[2] * 255) );
}

void COpenInventorSceneNode::GetRGBColor( float * rgb, QRgb color ) const
{
  rgb[0] = qRed(color) / 255.0f;
  rgb[1] = qGreen(color) / 255.0f;
  rgb[2] = qBlue(color) / 255.0f;
}

void COpenInventorSceneNode::RemoveFormationNodesFromGeologyLegend()
{
  assert(Model().GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();

  for(TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    const COpenGLNode * pOpenGLNode = * it;
    m_pHandler->removeFromGeologyLegend(pOpenGLNode);
  }
}

void COpenInventorSceneNode::AddFormationNodesToGeologyLegend()
{
  assert(Model().GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();

  for(TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    const COpenGLNode * pOpenGLNode = * it;
    m_pHandler->addToGeologyLegend(pOpenGLNode);
  }
}

void COpenInventorSceneNode::TimerCalled()
{
  m_pHandler->TimerCalled();
}

void COpenInventorSceneNode::ForceValueRedraw()
{
  if (m_pValueBlocked)
  {
    m_pValue = m_pValueBlocked;
  }

  if (m_pValue)
  {
    m_pHandler->OnValueComponentChanged(m_pValue, m_unitCurrent.Unit());
  }

  if (m_pIsoValueBlocked)
  {
    m_pHandler->OnIsoValueComponentChanged(m_pIsoValueBlocked, m_unitCurrent.Unit());
    m_pIsoValueBlocked = 0;
  }

  m_pHandler->UpdateLegend();
}

void COpenInventorSceneNode::CreateHexaMeshRegion( bool rectangular )
{
  ClearAll();
  AllowExclusiveDisplay(false);
  ShowHexaFormationTopMesh();
  ShowHexaMeshRegions();
  m_pHandler->ShowTopView(true);
  m_pHandler->Fill(false);
  m_pHandler->ShowInFull();
  AllowExclusiveDisplay(true);

  SetCursor(Qt::CrossCursor);


  COpenInventorSceneNodeAction * action = new COpenInventorSceneNodeAction(this, &COpenInventorSceneNode::CreatePolygonHexaMeshRegion, 0);
  m_pHandler->ActivatePolygonScreenDrawer(action, rectangular ? GeomecPolygonScreenDrawer::Rectangular : GeomecPolygonScreenDrawer::Polygon); // action to be executed (and deleted) upon completion of a rectanglescreendrawer
  
}

void COpenInventorSceneNode::EditHexaMainMeshRegion( CHexaMainMeshRegion & hexaMainMeshRegion )
{
  ClearAll();
  AllowExclusiveDisplay(false);
  ShowHexaFormationTopMesh();
  ShowHexaMeshRegions();
  m_pHandler->ShowTopView(true);
  m_pHandler->Fill(false);
  m_pHandler->ShowInFull();
  AllowExclusiveDisplay(true);

  SetCursor(Qt::CrossCursor);

  COpenInventorSceneNodeAction * action = new COpenInventorSceneNodeAction(this, &COpenInventorSceneNode::ModifyRectangleHexaMainMeshRegion, & hexaMainMeshRegion);

  std::vector<geo::CPoint> points;

  const geo::CRectangle & rectangle = hexaMainMeshRegion.getBoundary()->RectangleXY();

  for (size_t p = 0; p < rectangle.NrOfPoints(); ++p)
  {
    points.push_back(rectangle.Point(p));
  }

  m_pHandler->ActivatePolygonScreenDrawer(action, GeomecPolygonScreenDrawer::Rectangular, & points); // action to be executed (and deleted) upon completion of a polygonscreendrawer
}

void COpenInventorSceneNode::EditVerticalNewWellPath(CNewWellPath* pWellPath)
{
  ClearAll();
  AllowExclusiveDisplay(false);

  m_pHandler->SelectMeshMode(FullMesh);

  ShowHexaFormationTopMesh();
  ShowTetraFormationTopMesh();

  m_pHandler->ShowTopView(true);
  m_pHandler->Fill(false);
  m_pHandler->ShowInFull();
  AllowExclusiveDisplay(true);

  SetCursor(Qt::CrossCursor);

  std::vector<geo::CPoint> points;

  if (pWellPath)
    points.push_back(geo::CPoint(pWellPath->WellPoint(0)));

  COpenInventorSceneNodeAction * action = new COpenInventorSceneNodeAction(this, &COpenInventorSceneNode::EditVerticalNewWellPath, pWellPath);
  m_pHandler->ActivatePolygonScreenDrawer(action, GeomecPolygonScreenDrawer::SinglePoint, & points);
}

void COpenInventorSceneNode::EditVerticalNewWellPath( void * user_data, const std::vector<geo::CPoint> & points )
{
  C3DModel& model = (C3DModel&) Model();
  CNewWellPath * wellPath = 0;

  geo::CPoint wellPathPoint = points[0];
  geo::CPoint min = model.Boundary().Min();
  geo::CPoint max = model.Boundary().Max();
  if (wellPathPoint.X() < min.X() || wellPathPoint.X() > max.X() ||
    wellPathPoint.Y() < min.Y() || wellPathPoint.Y() > max.Y())
  {
    _m()->msg("Point outside mesh region, rejected.");
  }
  else
  {   
    if (user_data)
    {
      wellPath = (CNewWellPath *) user_data;
      assert(wellPath->WellPathInput());
      const_cast<CNewWellPathInput *>(wellPath->WellPathInput())->Recalculate(wellPathPoint);
    }
    else
    {
      QString sName = dynamic_cast <const CNewWellPathEntry*> (
              model.GraphEntry(MD_NEW_WELLPATH))->createUniqueName(
        CNewWellPathEntry::DEFAULT_NEWWELL_NAME,
        CNewWellPathEntry::DEFAULT_INITIAL_NUMBER);

      // No wellpath, so create one ...
      CNewWellPathInput *input = new CNewWellPathInput(sName.toStdString().c_str(), model, wellPathPoint);

      CAttriNewWellPathDlg dlg(*input);
      if(dlg.DoModal() == IDCANCEL)
      {
        input->Destroy();
        input = 0;
      }
      else
      {
              std::vector<std::string> messages;

              const CNewWellPath *pWellPath = CNewWellPathInput::CreateWellPath(input, model, &messages);

              if (messages.size())
        _m()->msg(messages);

              if (pWellPath)
        wellPath = const_cast<CNewWellPath *>(pWellPath);
      }
    }
  }
  m_pHandler->SelectMeshMode(SkinMesh);

  HideHexaFormationTopMesh();
  HideTetraFormationTopMesh();
  m_pHandler->ShowTopView(false);
  if (wellPath) 
  {
    if (CanConnectItem( *wellPath)) ConnectItem(* wellPath);
    m_pHandler->ShowBoundary(true);
  }
}

void COpenInventorSceneNode::EditHexaSubMeshRegion( CHexaSubMeshRegion & hexaSubMeshRegion )
{
  ClearAll();
  AllowExclusiveDisplay(false);
  ShowHexaFormationTopMesh();
  ShowHexaMeshRegions();
  m_pHandler->ShowTopView(true);
  m_pHandler->Fill(false);
  m_pHandler->ShowInFull();
  AllowExclusiveDisplay(true);

  SetCursor(Qt::CrossCursor);

  COpenInventorSceneNodeAction * action = new COpenInventorSceneNodeAction(this, &COpenInventorSceneNode::ModifyPolygonHexaSubMeshRegion, & hexaSubMeshRegion);

  std::vector<geo::CPoint> points;
  
  const geo::CPolygon & polygon = hexaSubMeshRegion.Polygon();

  for (size_t p = 0; p < polygon.NrOfPoints(); ++p)
  {
    points.push_back(polygon.Point(p));
  }

  m_pHandler->ActivatePolygonScreenDrawer(action, GeomecPolygonScreenDrawer::Polygon, & points); // action to be executed (and deleted) upon completion of a polygonscreendrawer 
}

void COpenInventorSceneNode::CreatePolygonHexaMeshRegion( void * user_data, const std::vector<geo::CPoint> & points )
{
  SetCursor(Qt::ArrowCursor);

  HideHexaFormationTopMesh();

  C3DModel& mymodel = (C3DModel&) Model();

  if (points.size() > 2) 
  {
    geo::CPolygon polygon;
    std::vector<geo::CPoint>::const_iterator pointsIter;

    for (pointsIter = points.begin(); pointsIter != points.end(); ++pointsIter)
    {
      polygon.PushBack(* pointsIter);
    }
    // Create a new mesh region ....
    CHexaSubMeshRegion *pRegion = new CHexaSubMeshRegion(mymodel, polygon);
    if(pRegion->Hull().NrOfPoints() > 3)
    {
      CHexaSubMeshRegion_Delegate* pRegion_Delegate =
        dynamic_cast <CHexaSubMeshRegion_Delegate*> (pRegion->getDelegate());

      if (pRegion_Delegate->PropertiesOK())
      {           
        ConnectItem(*pRegion);
        ShowHexaMeshRegions();
      }
      else
      {
        // Cancel has been pressed so clean up the newly created mesh region.
        delete pRegion;
      }
      return;
    }
    else
      delete pRegion;
  }
  _m()->msg("No valid mesh region.");
}

void COpenInventorSceneNode::ModifyPolygonHexaSubMeshRegion( void * user_data, const std::vector<geo::CPoint> & points )
{
  assert (user_data);
  CHexaSubMeshRegion & editRegion = * (CHexaSubMeshRegion *) user_data;

  HideHexaFormationTopMesh();

  C3DModel& mymodel = (C3DModel&) Model();

  if (points.size() > 2) 
  {
    geo::CPolygon polygon;
    std::vector<geo::CPoint>::const_iterator pointsIter;

    for (pointsIter = points.begin(); pointsIter != points.end(); ++pointsIter)
    {
      polygon.PushBack(* pointsIter);
    }

    CHexaSubMeshRegion *  pRegion = new CHexaSubMeshRegion(mymodel, polygon);
    int nrOfHullPoints = pRegion->Hull().NrOfPoints();
    pRegion->Destroy();// can't use delete ...

    if(nrOfHullPoints > 3)
    {
      editRegion.Polygon(polygon);
      mymodel.InvalidateMesh();
      ShowHexaMeshRegions();
      return;
    }

  }
  _m()->msg("No valid mesh region.");
}

void COpenInventorSceneNode::ModifyRectangleHexaMainMeshRegion( void * user_data, const std::vector<geo::CPoint> & points )
{
  assert (user_data);
  CHexaMainMeshRegion & editRegion = * (CHexaMainMeshRegion *) user_data;
  CHexaBoundary & meshRegionBoundary = * editRegion.getBoundary();

  HideHexaFormationTopMesh();

  C3DModel& mymodel = (C3DModel&) Model();

  if (points.size() != 4) return; 
  
  if (!geo::CRectangle::ValidRectangleData( points[0], points[2])) return;

  geo::CRectangle rectangle(points[0], points[2]);


  if(!((rectangle.Min().X() == meshRegionBoundary.Min().X()) &&
    (rectangle.Min().Y() == meshRegionBoundary.Min().Y()) &&
    (rectangle.Max().X() == meshRegionBoundary.Max().X()) &&
    (rectangle.Max().Y() == meshRegionBoundary.Max().Y())))
  {
    std::vector<CGraphNode*> vc_ptrs = meshRegionBoundary.ItemsOutsideBoundary(rectangle);
    if(vc_ptrs.size() > 0)
    {
      CString message;
      message.Format("One or more items like vertical wells, analysis points and/or cross sections\nare outside the new boundary and will be deleted.\nContinue?");
      if(_m()->msg((LPCSTR)(message), MB_YESNO) == IDNO)
        return;

      for(size_t i = 0; i < vc_ptrs.size(); i++)
      {
        vc_ptrs[i]->Destroy();
      }
    }

    meshRegionBoundary.State(CBoundaryBase::USER_DEFINED);
    meshRegionBoundary.RectangleXY(rectangle);

    mymodel.InvalidateMesh();
    ShowHexaMeshRegions();

  }
}

void COpenInventorSceneNode::ShowHexaFormationTopMesh()
{
  if (!Model().GraphEntry(MD_HEXA_FORMATION)) return;
  // if(FormationPlane() == 0)
  {
    // Select upper formation plane
    CHexaFormationEntry *pEntry = (CHexaFormationEntry*)(Model().GraphEntry(MD_HEXA_FORMATION));
    CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
    if(stNode.size() > 0)
    {
      CHexaFormation *pFormation = *stNode.begin();
      while(pFormation->UpperFormation() != 0)
      {
        pFormation = (CHexaFormation*)(pFormation->UpperFormation());
      }
      if (!IsLinkedTo(*(pFormation->FormationPlane(CFormationPlane::FP_TOP))))
        LinkTo(*(pFormation->FormationPlane(CFormationPlane::FP_TOP)));
    }
  }
  UpdateAllViews();
}

void COpenInventorSceneNode::ShowTetraFormationTopMesh()
{
  CModelBase& model = (CModelBase&)(Model());
   
  // We show a formation when we have a mesh and a horizon when we have not a mesh
  if(model.Mesh().IsMesh())
  {
    CTetraFormationEntry *pEntry = (CTetraFormationEntry*)(Model().GraphEntry(MD_TETRA_FORMATION));
    if (!pEntry) return;
    CTetraFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
    assert(stNode.size() > 0);
    
    LinkTo(**stNode.begin());

  }
  else
  {
    THorizonBaseEntry *pEntry = (THorizonBaseEntry*)(Model().GraphEntry(MD_BASE_HORIZON));
    if (!pEntry) return;
    if(pEntry->EntryNodes().size() > 0)
      LinkTo(**pEntry->EntryNodes().begin());
  }
  UpdateAllViews();
}

void COpenInventorSceneNode::HideTetraFormationTopMesh()
{
  CModelBase& model = (CModelBase&)(Model());

  // We show a formation when we have a mesh and a horizon when we have not a mesh
  if(model.Mesh().IsMesh())
  {
    CTetraFormationEntry *pEntry = (CTetraFormationEntry*)(Model().GraphEntry(MD_TETRA_FORMATION));
    if (!pEntry) return;
    CTetraFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
    assert(stNode.size() > 0);
    for(CTetraFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      if(IsLinkedTo(**it))
        UnLink(**it);
    }
  }
  else
  {
    THorizonBaseEntry *pEntry = (THorizonBaseEntry*)(Model().GraphEntry(MD_BASE_HORIZON));
    if (!pEntry) return;
    if(pEntry->EntryNodes().size() > 0)
    {
      if (IsLinkedTo(**pEntry->EntryNodes().begin()))
      {
        UnLink(**pEntry->EntryNodes().begin());
      }
    }
  }
  m_pHandler->ShowTopView(false);
  UpdateAllViews();
}

void COpenInventorSceneNode::HideHexaFormationTopMesh()
{
  if(!Model().GraphEntry(MD_HEXA_FORMATION)) return;
  // if(FormationPlane() == 0)
  {
    // Select upper formation plane
    CHexaFormationEntry *pEntry = (CHexaFormationEntry*)(Model().GraphEntry(MD_HEXA_FORMATION));
    CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
    if(stNode.size() > 0)
    {
      CHexaFormation *pFormation = *stNode.begin();
      while(pFormation->UpperFormation() != 0)
      {
        pFormation = (CHexaFormation*)(pFormation->UpperFormation());
      }
      if (IsLinkedTo(*(pFormation->FormationPlane(CFormationPlane::FP_TOP))))
      UnLink(*(pFormation->FormationPlane(CFormationPlane::FP_TOP)));
    }
  }
  m_pHandler->ShowTopView(false);
  UpdateAllViews();
}
void COpenInventorSceneNode::ShowHexaMeshRegions()
{
  CHexaMeshRegionEntry *pRegionEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode2 = pRegionEntry->EntryNodes();
  for(CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode2.begin(); itMeshRegion != stNode2.end(); itMeshRegion++)
  {
    if (!IsLinkedTo(** itMeshRegion))
    {
      LinkTo(** itMeshRegion);
    }
  }
  UpdateAllViews();
}

int COpenInventorSceneNode::NumberOfOpenGLNodesLinked() const
{
  auto stNodes = Links<COpenGLNode>();
  return stNodes.size();
}

void COpenInventorSceneNode::dehighlightMesh()
{
  m_pHandler->dehighlightMesh();
}

void COpenInventorSceneNode::clearValueTrackerCellFacetSet()
{
  m_pHandler->clearValueTrackerCellFacetSet();
}

bool COpenInventorSceneNode::containsSupportNodes() const
{
  auto supportNodes = Links<C3DSupportNode>();
  return !supportNodes.empty();
}

void COpenInventorSceneNode::ScheduleConnectItem( COpenGLNodeBase & node )
{
  m_scheduledItems.push_back(& node);
}

void COpenInventorSceneNode::ConnectScheduledItems()
{
  for (auto node : m_scheduledItems)
  {
    ConnectItem(* node);
  }
  m_scheduledItems.clear();
  UpdateAllViews();
}

const COpenInventorSceneNode::TOpenGLNodeSet& COpenInventorSceneNode::NodesInScene() const
{
  return m_stOpenGLNodes;
}

const COpenInventorSceneNode::TLinkedGraphNodeSet& COpenInventorSceneNode::GraphNodesInScene() const
{
  return m_stGraphNodes;
}



