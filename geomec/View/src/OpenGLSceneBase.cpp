// OpenGLSceneBase.cpp: implementation of the COpenGLSceneBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "OpenGLSceneBase.h"
#include "ColorGradient.h"
#include "BaseEntryTypes.h"
#include "geomecdoc.h"
#include "ModelBase.h"
#include "Result.h"
#include "HotSpot.h"
#include "HexaHorizon.h"
#include "FormationBase.h"
#include "pointset.h"
#include "CrossSection.h"
#include "ColorScaleEntry.h"
#include "BoundaryBase.h"
#include "DerivedResult.h"
#include "Pressure.h"
#include "ResultTree.h"
#include "ModelBase.h"
#include "openglaxis.h"
#include "DisplacementSupportNode.h"
#include "PressureSupportNode.h"
#include "ModelView.h"
#include "meshresulttree.h"
#include "NodeLocationHighlighter.h"
#include "MainFrm.h"
#include "StaborResult.h"
#include "ISceneWrapper.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

#define PATH_SEPERATOR "_"

//////////////////////////////////////////////////////////////////////
// Implementation of boundary observer
//////////////////////////////////////////////////////////////////////
COpenGLSceneBase::CBoundaryObserver::CBoundaryObserver(COpenGLSceneBase &scene)
: CGraphNode(_T("NONE")), m_scene(scene)
{
  CModelBase *pModel = dynamic_cast<CModelBase*> (&m_scene.Model());
  assert(pModel);

  // Link to boundary ....
  reParent(&scene);
  LinkTo(pModel->Boundary());
}

COpenGLSceneBase::CBoundaryObserver::~CBoundaryObserver()
{
}

void COpenGLSceneBase::CBoundaryObserver::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  CModelBase *pModel = dynamic_cast<CModelBase*> (&m_scene.Model());
  assert(pModel);
  if(!GetGeomecDoc()->IsDeleting())
  {
    if(&pModel->Boundary() == &node)
    {
      m_scene.UpdateBoundary();
    }
  }
}


unsigned int COpenGLSceneBase::CBoundaryObserver::IconId() const
{
  assert(FALSE);
  return 0;
}

unsigned int COpenGLSceneBase::CBoundaryObserver::TypeId() const
{
  assert(FALSE);
  return 0;
}

QString COpenGLSceneBase::CBoundaryObserver::TypeName() const
{
  assert(FALSE);
  return QString();
}

/////

COpenGLSceneBase::CNodeLocationListener::CNodeLocationListener(COpenGLSceneBase& scene)
: COpenGLValueScene::CDefaultListener(scene),
  m_pHighlighter(new CNodeLocationHighlighter(scene))
{
}

COpenGLSceneBase::CNodeLocationListener::~CNodeLocationListener()
{
  delete m_pHighlighter;
}

bool COpenGLSceneBase::CNodeLocationListener::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  m_pHighlighter->MouseMove(point, (state == Qt::NoButton && button == Qt::NoButton));

  return false;
}


//////////////////////////////////////////////////////////////////////
// Implementation of openglscenebase
//////////////////////////////////////////////////////////////////////



COpenGLSceneBase::COpenGLSceneBase(CFemAppModel &model)
: COpenGLValueScene(model)
{
  Initialize();
}

//##ModelId=3BE7AEBD0342
COpenGLSceneBase::COpenGLSceneBase(const QString &strName, CFemAppModel& model)
: COpenGLValueScene(strName, model)
{
  Initialize();
}

//##ModelId=3BE7AEBD03AE
COpenGLSceneBase::COpenGLSceneBase(const unsigned int uName, CFemAppModel& model)
: COpenGLValueScene(uName, model)
{
  Initialize();
}

//##ModelId=3BE7AEBE0024
COpenGLSceneBase::COpenGLSceneBase(const COpenGLSceneBase& rhs)
: COpenGLValueScene(rhs), m_pBoundaryObserver(0), m_pNodeLocationListener(0)
{
  if(DisplayAxis())
    InitAxis();
}

void DianaAssertionFuncIgnore(const char*, const char*, unsigned int)
{
  // ignore
}

//##ModelId=3BC55D6002D4
COpenGLSceneBase::~COpenGLSceneBase()
{
  if(m_pBoundaryObserver) delete m_pBoundaryObserver;
  if(m_pNodeLocationListener)
  {
  // ignore assertions caused by the destruction of the mouse listener
  DianaAssertFunction_t oldfunc = DianaAssertFunction();
  SetDianaAssertFunction(DianaAssertionFuncIgnore);
    delete m_pNodeLocationListener;
  SetDianaAssertFunction(oldfunc);
  }
}

void COpenGLSceneBase::Initialize()
{
  // TODO TO GUI due to CNodeLocationListener->CDefaultListener->CMouseListener

  // Init vector
  COpenGLScene::ViewVector(geo::CVector::Yaxis*-1.0);
  COpenGLScene::UpVector(geo::CVector::Zaxis*-1.0);

  // Switch off bounding box viewing
  DisplayBoundingBox(FALSE);
  m_pBoundaryObserver = new CBoundaryObserver(*this);

  InitAxis();

  m_pNodeLocationListener = new CNodeLocationListener(*this);
}

void COpenGLSceneBase::InitAxis()
{
  CModelBase& model = (CModelBase&)Model();
  QString sAxis;
  sAxis = getStringTableEntry(IDS_RC_NORTHING);
  Axis().AxisText(geo::CVector::Xaxis, sAxis);
  sAxis = getStringTableEntry(IDS_RC_EASTING);
  Axis().AxisText(geo::CVector::Yaxis, sAxis);
  sAxis = getStringTableEntry(IDS_RC_DEPTH);
  Axis().AxisText(geo::CVector::Zaxis, sAxis);
}

//##ModelId=3BC55D6002D6
void COpenGLSceneBase::OnNewNeighbour(const CGraphNode &node)
{
  if(dynamic_cast<const COpenGLNode*>    (&node) ||
     dynamic_cast<const IValueComponentBase*>(&node) ||
     dynamic_cast<const IValueComposite*>(&node) ||
     dynamic_cast<const IColorScaleNode*>(&node))
  {
  CModelBase& model = static_cast<CModelBase&>(Model());
  if(model.AutomaticallyDisableDrawingFor(node))
      model.DisableRedrawing();
  }

 	// Let the graphnode do it's work ...
  COpenGLValueScene::OnNewNeighbour(node);

  const CCrossSection* pXSection = dynamic_cast<const CCrossSection*>(&node);

  // Cross Section
  if(pXSection)
  {
    COperation operation(Model());
    m_vcXSection.push_back((CCrossSection*)pXSection);
    const std::vector<CCrossSection::CCSObject*>& vcCSObject = pXSection->CSObjects();

    for(size_t i = 0; i < vcCSObject.size(); i++)
    {
      if (!IsLinkedTo(*vcCSObject[i]))
        LinkTo(*vcCSObject[i]);
    }
  }
}

//##ModelId=3BC55D6002D9
void COpenGLSceneBase::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if(dynamic_cast<const COpenGLNode*>    (&node) ||
     dynamic_cast<const IValueComponentBase*>(&node) ||
     dynamic_cast<const IValueComposite*>(&node) ||
     dynamic_cast<const IColorScaleNode*>(&node))
  {
  CModelBase& model = static_cast<CModelBase&>(Model());
  if(model.AutomaticallyDisableDrawingFor(node))
      model.DisableRedrawing();
  }

  // XSections...
  if(m_vcXSection.size() > 0)
  {
    for(size_t i = 0; i < m_vcXSection.size(); i++)
    {
      if(m_vcXSection[i] == &node)
      {
        const std::vector<CCrossSection::CCSObject*>& vcCSObject = m_vcXSection[i]->CSObjects();

        for(size_t j = 0; j < vcCSObject.size(); j++)
        {
          if (!IsLinkedTo(*vcCSObject[j]))
            LinkTo(*vcCSObject[j]);
        }

      }
    }
  }

  COpenGLValueScene::OnNeighbourModified(node, uHint);
}

//##ModelId=3BC55D6002E6
void COpenGLSceneBase::OnNeighbourDeleted(const CGraphNode &node)
{
  if(dynamic_cast<const COpenGLNode*>    (&node) ||
     dynamic_cast<const IValueComponentBase*>(&node) ||
     dynamic_cast<const IValueComposite*>(&node) ||
     dynamic_cast<const IColorScaleNode*>(&node))
  {
  CModelBase& model = static_cast<CModelBase&>(Model());
  if(model.AutomaticallyDisableDrawingFor(node))
      model.DisableRedrawing();
  }

  // XSections...
  if(m_vcXSection.size() > 0)
  {
    for(std::vector<CCrossSection*>::iterator it = m_vcXSection.begin(); it != m_vcXSection.end(); it++)
    {
      if(*it == &node)
      {
        const std::vector<CCrossSection::CCSObject*>& vcCSObject = (*it)->CSObjects();

        for(size_t i = 0; i < vcCSObject.size(); i++)
          UnLink(*vcCSObject[i]);
        
        m_vcXSection.erase(it);
        Modified();
        break;

      }
    }
  }

  COpenGLValueScene::OnNeighbourDeleted(node);
}

void COpenGLSceneBase::OnSelect(CGraphNode& node)
{
}

void COpenGLSceneBase::ClearExceptResult()
{
  COpenGLSceneNode::Clear();
  // Don't clear results
  IResultComponent *pResult = dynamic_cast<IResultComponent*> (ValueComponent());
  if(pResult == 0)
  {
    // If we have a quantity unlink it ...
    if(ValueComponent())
    {
      UnLink(*ValueComponent());
      assert(ValueComponent() == 0);
    }
  }
 
  // XSections
  if(m_vcXSection.size() > 0)
  {
    for(size_t i = 0; i < m_vcXSection.size(); i++)
    {
      UnLink(*(m_vcXSection[i]));
    }
  }


  UpdateFrame();
}

void COpenGLSceneBase::Clear()
{
  COpenGLSceneNode::Clear();

  // If we have a quantity unlink it ...
  if(ValueComponent())
  {
    UnLink(*ValueComponent());
    assert(ValueComponent() == 0);
  }
 
  // XSections
  if(m_vcXSection.size() > 0)
  {
    for(size_t i = 0; i < m_vcXSection.size(); i++)
    {
      UnLink(*(m_vcXSection[i]));
    }
  }


  UpdateFrame();
}

bool COpenGLSceneBase::CanConnectItem(const CGraphNode& item) const
{
  CFemAppDoc& doc = *GetGeomecDoc();
  if((!doc.CurrentScene() || doc.CurrentScene()->OpenGLScene() != this) && doc.StickToScene())
  {
  CMainFrame* pFrame = static_cast<CMainFrame*>(FemAppGetMainWnd());
  pFrame->StatusBar().SetWindowText("GEOMEC is currently sticking to the current view. Unset this option to allow switching views and display this item");
  return false;
  }

  const IResultComponent* pResultComp = dynamic_cast<const IResultComponent*>(&item);
  if(pResultComp && !CanDisplayResultComponent(*pResultComp))
  return false;

  return COpenGLValueScene::CanConnectItem(item);
}

void COpenGLSceneBase::OnUpdateLegend()
{
  // Insert caption "legend"
  new CLegendFrame::legend_label(LegendFrame(), "Legend");
  new CLegendFrame::legend_separator(LegendFrame(), 20);

  TOpenGLNodeSet stNode = NodesInScene();
  TOpenGLNodeSet::iterator itNode;
  for(itNode = stNode.begin(); itNode != stNode.end(); itNode++)
  {
    const CDisplacementSupportNode *dnode = dynamic_cast<const CDisplacementSupportNode *> (*itNode);
    const CPressureSupportNode *pnode = dynamic_cast<const CPressureSupportNode *> (*itNode);
    assert(!(dnode && pnode));

    if(dnode != 0)
    {
      new CLegendFrame::legend_separator(LegendFrame(), 15);
      new CLegendFrame::legend_label(LegendFrame(), "Select nodes, right-click to configure");
      new CLegendFrame::legend_separator(LegendFrame(), 15);
      new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(255, 0, 0), "Selected node");
      new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(0, 255, 0), "Reference node");
      new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(0, 0, 255), "Selectable node");
      return;
    }
    else if(pnode != 0)
    {
      new CLegendFrame::legend_separator(LegendFrame(), 15);
      new CLegendFrame::legend_label(LegendFrame(), "Select surfaces, right-click to configure");
      new CLegendFrame::legend_separator(LegendFrame(), 15);
      new CLegendFrame::legend_index(LegendFrame(), 10, 10, qRgb(255, 255, 0), "Selected surface");
      return;
    }
  }

  // Give name of scene
  new CLegendFrame::legend_label(LegendFrame(), QString("Scene : ") + Name());
  new CLegendFrame::legend_separator(LegendFrame(), 3);

  // Are there items in the scene?
  if(NodesInScene().size() == 0)
  {
    new CLegendFrame::legend_separator(LegendFrame(), 15);		
    new CLegendFrame::legend_label(LegendFrame(), QString("No viewable items selected"));
    new CLegendFrame::legend_separator(LegendFrame(), 15);
    return;
  }
  if(ShowColor())
  {
    new CLegendFrame::legend_label(LegendFrame(), "View : Geology");
    COpenGLSceneNode::OnUpdateLegend();
  }
  else
  {
    new CLegendFrame::legend_label(LegendFrame(), "View : Value");
    OnUpdateValueLegend();
    UpdateFrame();
  }
}

void COpenGLSceneBase::OnUpdateValueLegend()
{
  CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*>(((CFemAppModel&)Model()).GraphEntry(MD_BASE_COLOR_SCALE));
  assert(pEntry);
  CGeomecDoc& doc = *GetGeomecDoc();

  new CLegendFrame::legend_separator(LegendFrame(), 3);
  new CLegendFrame::legend_label(LegendFrame(), QString("Unit type : ") + doc.UnitNode().Name());
  new CLegendFrame::legend_separator(LegendFrame(), 3);

  switch(pEntry->ColorScaleType())
  {
  case CColorScaleEntry::LOCAL:
    new CLegendFrame::legend_label(LegendFrame(), "Color scale : Local");
    break;
  case CColorScaleEntry::GLOBAL:
    new CLegendFrame::legend_label(LegendFrame(),"Color scale : Global");
    new CLegendFrame::legend_separator(LegendFrame(), 3);
    new CLegendFrame::legend_label(LegendFrame(), QString("Color scale name : ") + pEntry->CurrentColorScale().Name() );
    break;
  case CColorScaleEntry::HOTSPOT:
    new CLegendFrame::legend_label(LegendFrame(), "Color scale : Hotspot");
    new CLegendFrame::legend_separator(LegendFrame(), 3);
    new CLegendFrame::legend_label(LegendFrame(), QString("Hotspot name : ") + pEntry->CurrentColorScale().Name() );
    break;
  default:
    assert(FALSE);	// Unknown
    break;
  }

  // Do we have a result component
  new CLegendFrame::legend_separator(LegendFrame(), 15);

  const IResultComponent *pResultComponent = dynamic_cast<const IResultComponent*> (ValueComponent());
  if(pResultComponent)
  {
    // Result
    new CLegendFrame::legend_label(LegendFrame(), "Result tree path:");
    AddResultPath(*pResultComponent);
    new CLegendFrame::legend_separator(LegendFrame(), 6);
    new CLegendFrame::legend_label(LegendFrame(), QString("Depletion stage : ") + pResultComponent->Stage().Name());
    new CLegendFrame::legend_separator(LegendFrame(), 3);

    if(pResultComponent->AnalysisType().Valid())
    {
      new CLegendFrame::legend_label(LegendFrame(), QString("Analysis : ") + pResultComponent->AnalysisType().Label());
      if(pResultComponent->AnalysisType() == CAnalysisType::AT_NONLIN)
        new CLegendFrame::legend_label(LegendFrame(), "Step type : Time");
    }
  }
  else
  {
    // Other value object
    QString sValueName;
    if(ValueComponent()->Parent().TypeId() && !dynamic_cast<CMeshResult*>(&ValueComponent()->Parent()))
      sValueName = ValueComponent()->Parent().TypeName();
    else
      sValueName = ValueComponent()->Parent().Name();	// Derived result
    new CLegendFrame::legend_label(LegendFrame(), QString("Value :") + sValueName);
    if(ValueComponent()->Parent().ComponentSize() != 1)
    {
      new CLegendFrame::legend_separator(LegendFrame(), 3);
      new CLegendFrame::legend_label(LegendFrame(), QString("Component : ") + ValueComponent()->Name());
    }
  }

  new CLegendFrame::legend_separator(LegendFrame(), 3);
  new CLegendFrame::legend_label(LegendFrame(), QString("Unit : ") + ValueComponent()->UnitName(doc.UnitNode().Unit()));
  new CLegendFrame::legend_separator(LegendFrame(), 15);

  if(InsertLinkedNodesInLegend() == 0)
    return;

  CColorGradient *pGradient = pEntry->ColorScaleType() == CColorScaleEntry::LOCAL ? pEntry->LocalColorGradient() : pEntry->GlobalColorGradient();
  assert (pGradient);
  switch(pEntry->ColorScaleType())
  {
  case CColorScaleEntry::LOCAL:
#if 0
    {
      // Get the color gradient
      CColorGradient *pGradient = pEntry->LocalColorGradient();
      assert(pGradient);
      pGradient->SetMinMaxValue(MinMaxValue().first.Value(), MinMaxValue().second.Value());

      // Get the range of the scene ... 
      if(MinMaxValue().second.Valid() && MinMaxValue().first.Valid())
      {
        double dFraction = (MinMaxValue().second.Value() - MinMaxValue().first.Value()) / (pGradient->MinMaxValue().second - pGradient->MinMaxValue().first);
        CLegendFrame::legend_scale::TColorMap mpColor;
        for(CColorGradient::color_iterator it = pGradient->Begin(); it != pGradient->End(); it++)
        {
          // Fill color map
          double value = MinMaxValue().first.Value() + dFraction * (it->first - pGradient->MinMaxValue().first);
          mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(value, it->second));
        }

        assert(mpColor.size() != 0);

        if(mpColor.size() == 1)	// Min and max are equal -> single value
        {
          QString label = QString("%1").arg(MinMaxValue().first.Value());	
          new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
          label = QString("%1").arg(MinMaxValue().second.Value());	
          new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);			
        }
        else
        {
          // Create color scale
          new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
          new CLegendFrame::legend_separator(LegendFrame(), 15);
        }
      }
      else
      {
        // Result is invalid ....
        QString label("Not defined");
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);	
      }
    }
    break;
#endif
  case CColorScaleEntry::GLOBAL:
    {
#if 0
      // Get the color gradient

      CColorGradient *pGradient = pEntry->GlobalColorGradient();
      assert(pGradient);
#endif
      if(MinMaxValue().second.Valid() && MinMaxValue().first.Valid())
      {
        if(pGradient->AutoExtremes())
          pGradient->SetMinMaxValue(MinMaxValue().first.Value(), MinMaxValue().second.Value());

        CLegendFrame::legend_scale::TColorMap mpColor;
        for(CColorGradient::color_iterator it = pGradient->Begin(); it != pGradient->End(); it++)
        {
          // Fill color map
          mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(it->first, it->second));
        }

        assert(mpColor.size() > 0);

        // Create color scale
        new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
        new CLegendFrame::legend_separator(LegendFrame(), 15);
      }
      else
      {
        // Result is invalid ....
        QString label("Not defined"); //wjrx 
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MinColor(), label);
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pGradient->MaxColor(), label);	
      }

    }
    break;
  case CColorScaleEntry::HOTSPOT:
    {
      const CHotSpot *pHotSpot = pEntry->HotSpot(); 
      assert(pHotSpot); // has to be a hotspot...
      if(pHotSpot->CenterIsTransparent())
      {
        QString label = QString("< %1").arg(pHotSpot->MinMaxValue().first);
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MinColor(), label);
        new CLegendFrame::legend_separator(LegendFrame(), 3);
        label = QString("> %1").arg(pHotSpot->MinMaxValue().second);
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MaxColor(), label);
      }
      else
      {
        if(pHotSpot->MinMaxValue().first != pHotSpot->MinMaxValue().second)
        {
          CLegendFrame::legend_scale::TColorMap mpColor;
          mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(pHotSpot->MinMaxValue().first, pHotSpot->MinColor()));
          mpColor.insert(CLegendFrame::legend_scale::TColorMap::value_type(pHotSpot->MinMaxValue().second, pHotSpot->MaxColor()));
          new CLegendFrame::legend_scale(LegendFrame(), mpColor, 10, 15, 300);
          new CLegendFrame::legend_separator(LegendFrame(), 3);
        }

        QString label = QString("> %1").arg( pHotSpot->MinMaxValue().second);
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pHotSpot->MaxColor(), label);	
        new CLegendFrame::legend_separator(LegendFrame(), 15);
      }
    }
    break;
  default:
    assert(FALSE);	// Unknown
    break;
  }

}


//##ModelId=3BC55D600298
unsigned int COpenGLSceneBase::IconId() const
{
  return IDI_VIEW;
}

//##ModelId=3BC55D600296
unsigned int COpenGLSceneBase::TypeId() const
{
  return 0;
}

void COpenGLSceneBase::UpdateBoundary()
{
  if(ShowBoundary())
  {
    CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
    assert(pModel);
    geo::CBox box(pModel->Boundary().Min(), pModel->Boundary().Max());
    SetFixedBoundingBox(box);
    UseFixedBoundingBox(true);
  }
}



void COpenGLSceneBase::ShowBoundary(BOOL bShowBoundary)
{
  DisplayBoundingBox(bShowBoundary);
  UpdateBoundary();
}

BOOL COpenGLSceneBase::ShowBoundary() const
{
  return DisplayBoundingBox();
}


//##ModelId=3BE7AEBE013E
bool COpenGLSceneBase::operator==(const COpenGLSceneBase &rhs) const
{
  if(!CGraphNode::operator ==(rhs))
    return false;

  if(! COpenGLScene::operator==(rhs))
    return false;

  return true;
}

//##ModelId=3BE7AEBE016C
COpenGLSceneBase& COpenGLSceneBase::operator=(const COpenGLSceneBase& rhs)
{
  COpenGLSceneNode::operator=(rhs);
  DisplayBoundingBox(rhs.DisplayBoundingBox());

  UpdateFrame();
  return *this;
}

void COpenGLSceneBase::AddResultPath(const IResultComponent& result_component)
{
  std::vector<QString> vcPath;
  
  // Get result of component
  const IResult& result = dynamic_cast<const IResult&>(result_component.Parent());
  
  // Is the result a scalar?
  if(result.ComponentSize() > 1)
    vcPath.push_back(result_component.Name());

  vcPath.push_back(result.Name());
  const CResultGroup *pGroup = result.Parent();
  while(pGroup)
  {
    vcPath.push_back(pGroup->Name());
    pGroup = pGroup->Parent();
  }

  // Reverse insert of result path
  new CLegendFrame::legend_label(LegendFrame(), vcPath[vcPath.size() - 1]);

  CLegendFrame::legend_tree_item* pItem = 0;
  for(int i = 1; i < vcPath.size(); i++)
  {
    int nIndex = vcPath.size() - i - 1;
    if(pItem)
      pItem = new CLegendFrame::legend_tree_item(vcPath[nIndex], pItem->child_position(), LegendFrame());
    else
      pItem = new CLegendFrame::legend_tree_item(vcPath[nIndex], 0, LegendFrame());
  }
}

bool COpenGLSceneBase::Empty() const
{
  return false;
}

void COpenGLSceneBase::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  TOpenGLNodeBaseEntry& opengl_entry = (TOpenGLNodeBaseEntry&)*Model().GraphEntry(MD_BASE_OPENGL_NODE);

  COpenGLSceneNode::LoadStream(stream,version,progress);
  progress.StatusMessage("Loading Scene ");
  double X,Y,Z;
  stream >> X >> Y >> Z;
  ViewVector(geo::CVector(X,Y,Z));
  stream >> X >> Y >> Z;
  UpVector(geo::CVector(X,Y,Z));
  stream >> X >> Y >> Z;
  ScaleVector(geo::CVector(X,Y,Z));
  
  progress.Step();

  int iNrOfNodes;
  stream >> iNrOfNodes;
  int iTypeIndex;
  for(int i = 0; i< iNrOfNodes; i++)
  {
    stream >> iTypeIndex;

    if(CStreamVersion(3, 0, 48) < version)
    {
      // We only link from version 3.0.49 because the model is then fully load and all
      // objects are initialized.
      COpenGLNodeBase *pNode = opengl_entry.FindIndex(iTypeIndex);
      if(pNode)
        if(CanConnectItem(*pNode))
          ConnectItem(*pNode);
    }


    progress.Step();

  }

  double dTemp;
  stream >> dTemp;
  PointSize(dTemp);

  BOOL bTemp;
  stream >> bTemp;
  EnableLighting(bTemp);
  stream >> bTemp;
  DisplayAxis(bTemp);
  stream >> bTemp;
  DisplayPerspective(bTemp);
  stream >> bTemp;
  Fill(bTemp);
  stream >> bTemp;
  AllowDynamicRotate(bTemp);
  stream >> bTemp;
  ShowBoundary(bTemp);

  progress.Step();

  int iTemp;
  stream >> iTemp;	// The former color scale
  //	m_color_scale = (COLORSCALE_TYPE)iTemp;

  QString axlock;
  stream >> axlock;
  if(axlock == "NONE")
    RotateLockVector(geo::CVector::NullVector);
  else if(axlock == "X")
    RotateLockVector(geo::CVector::Xaxis);
  else if(axlock == "Y")
    RotateLockVector(geo::CVector::Yaxis);
  else if(axlock == "Z")
    RotateLockVector(geo::CVector::Zaxis);
  else
    assert(FALSE);
  
  //**
  progress.Step();
  //**

  int R,G,B;
  stream >> R >> G >> B;
  AxisColor(qRgb(R,G,B));
  stream >> R >> G >> B;
  BackgroundColor(qRgb(R,G,B));

  //**
  progress.Step();
  //**



}

void COpenGLSceneBase::SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress)
{
  COpenGLSceneNode::SaveStream(stream,progress);
  progress.StatusMessage("Saving Scene ");
  stream << ViewVector().X() << ViewVector().Y() << ViewVector().Z();
  stream << UpVector().X() << UpVector().Y() << UpVector().Z();
  stream << ScaleVector().X() << ScaleVector().Y() << ScaleVector().Z();
  
  progress.Step();

  std::vector<const COpenGLNode*> vcNode;
  for(size_t i = 0; i < referenceSize(); i++)
  {
    COpenGLNode *pNode = dynamic_cast<COpenGLNode*>(&referenceAt(i));
    if(pNode) vcNode.push_back( pNode );
  }
  stream << int(vcNode.size()); // Nr of indices
  for(size_t i = 0; i < vcNode.size(); i++)
  {
    stream << vcNode[i]->Index();
    progress.Step();
  }

  stream << PointSize();

  BOOL bTemp;
  bTemp = LightingEnabled();
  stream << bTemp;
  bTemp = DisplayAxis();
  stream << bTemp;
  bTemp = DisplayPerspective();
  stream << bTemp;
  bTemp = Fill();
  stream << bTemp;
  bTemp = AllowDynamicRotate();
  stream << bTemp;
  bTemp = DisplayBoundingBox();
  stream << bTemp;

  progress.Step();

  int iTemp = CColorScaleEntry::LOCAL;
//	iTemp = m_color_scale;
  stream << iTemp;

  if(RotateLockVector() == geo::CVector::NullVector)
    stream << QString("NONE");
  else if(RotateLockVector() == geo::CVector::Xaxis)
    stream << QString("X");
  else if(RotateLockVector() == geo::CVector::Yaxis)
    stream << QString("Y");
  else if(RotateLockVector() == geo::CVector::Zaxis)
    stream << QString("Z");
  else
    assert(FALSE);
  
  //**
  progress.Step();
  //**

  iTemp = QColor(AxisColor()).red();
  stream << iTemp;
  iTemp = QColor(AxisColor()).green();
  stream << iTemp;
  iTemp = QColor(AxisColor()).blue();
  stream << iTemp;

  iTemp = QColor(BackgroundColor()).red();
  stream << iTemp;
  iTemp = QColor(BackgroundColor()).green();
  stream << iTemp;
  iTemp = QColor(BackgroundColor()).blue();
  stream << iTemp;

  //**
  progress.Step();
  //**
}

long COpenGLSceneBase::SavedItems() const
{
  int nRet = 0;
  for(size_t i = 0; i < referenceSize(); i++)
  {
    if(dynamic_cast<const COpenGLNode*>(&referenceAt(i)))
    {
      nRet++;
    }
  }
  
  return (4 + nRet);
}

void COpenGLSceneBase::BackgroundColor(TColor color)
{
  CModelView* pView = dynamic_cast<CModelView*> (&Root());
  if(pView && pView->LegendView())
  {
    pView->LegendView()->BackgroundColor(color);
    pView->LegendView()->UpdateFrame();
  }

  COpenGLValueScene::BackgroundColor(color);
}

QString COpenGLSceneBase::GetResultPath(const IValueComponentBase& result_component) const
{
  QString ret;
  std::vector<QString> vcPath;
  

  const IResultComponent *pComponent = dynamic_cast<const IResultComponent*> (&result_component);
  
  if(!pComponent)
    return result_component.Name();


  CAnalysisType antype=CAnalysisType::AT_NONLIN;
  CDepletionStage* pStage=0;
  QString sLinear;

  

  // Get result of component
  const IResult& result = dynamic_cast<const IResult&>(result_component.Parent());
  
  // Is the result a scalar?
  if(result.ComponentSize() > 1)
    vcPath.push_back(result_component.Name());

  vcPath.push_back(result.Name());
  const CResultGroup *pGroup = result.Parent();
  while(pGroup)
  {
    vcPath.push_back(pGroup->Name());
    pGroup = pGroup->Parent();
  }

  for(int i = 1; i < vcPath.size(); i++)
  {
    int nIndex = vcPath.size() - i - 1;
    if(ret.isEmpty())
      ret = vcPath[nIndex];
    else
      ret = ret + PATH_SEPERATOR + vcPath[nIndex];
  }
  return ret;
}

// Shows the contents of an entry
// SHOW_ALL			Shows all the contents of an entry
// SHOW_SELECTED	Shows all the nodes linked to the scene
// SHOW_UNSELECTED	Shows all the nodes not linked to the scene
// Return the number of displayed nodes depending on the entry
int COpenGLSceneBase::ShowEntry(unsigned int uEntry, enum eShowMode mode)
{
  CGraphEntry::TEntryNodeSet stTemp = Model().GraphEntry(uEntry)->GraphEntryNodes();
  CGraphEntry::TEntryNodeSet stNode;
  for(CGraphEntry::TEntryNodeSet::const_iterator it = stTemp.begin(); it != stTemp.end(); it++)
  {
    if((mode == SHOW_ALL) ||
       ((mode == SHOW_SELECTED) && IsLinkedTo(**it)) ||
       ((mode == SHOW_UNSELECTED) && !IsLinkedTo(**it)))
      stNode.insert(*it);	// Linked is selected
  }

  if(stNode.size() > 0)
  {
    new CLegendFrame::legend_label(LegendFrame(), (*stNode.begin())->TypeName());
    new CLegendFrame::legend_separator(LegendFrame(), 5);
    for(CGraphEntry::TEntryNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      const COpenGLNode* pNode = dynamic_cast<const COpenGLNode*>(*it);
      if(pNode)
      {
        new CLegendFrame::legend_index(LegendFrame(), 15, 15, pNode->Color(), pNode->Name());
        new CLegendFrame::legend_separator(LegendFrame(), 3);
      }

    }
  }

  if(stNode.size() > 0)
    new CLegendFrame::legend_separator(LegendFrame(), 20);

  return stNode.size();
}

bool COpenGLSceneBase::CanDisplayResultComponent(const IResultComponent &result_component) const
{
  return !dynamic_cast<const CStaborResult::CStaborResultComponent*>(&result_component);
}

#define ZOOM_TO_MOUSEPOINTER 0

bool COpenGLSceneBase::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  if(AllowDynamicZoom())
  {
    // remember unprojected point of mouse pointer
    geo::CPoint ptWorldOld = UnProject(point);

    // perform the scaling
    double dFactor = (1000.0 + nDelta) / 1000.0;
    ScaleVector(ScaleVector() * dFactor);

#if ZOOM_TO_MOUSEPOINTER

    // wedx 08082007:
    // Unfortunately the Project and UnProject methods only work after an actual UpdateFrame(),
    // because then the transformation matrices are updated. This means that for now zooming
    // towards the mouse pointer only works if either
    //  - an UpdateFrame() is called after zooming, which shows a zoom update, and subsequently a
    //     pan update with each wheel action
    //  - matrices are calculated here, which would duplicate code

    UpdateFrame();

    // unproject mouse location again
    geo::CPoint ptWorldNew = UnProject(point);

    // move along the difference between the two world points
    geo::CVector diff(ptWorldNew - ptWorldOld);
    diff.Multiply(ModelScaleVector());
    DisplacementVector(DisplacementVector() + diff);

#endif

    UpdateFrame();

    return true;
  }

  return false;
}

bool COpenGLSceneBase::BlockDrawing() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return !model.SceneCanDraw();
}

COpenGLSceneBase::TOpenGLNodeSet COpenGLSceneBase::NodesInSceneForLegend() const
{
  TOpenGLNodeSet stNodes = COpenGLValueScene::NodesInSceneForLegend();

  // add formations
  TOpenGLNodeSet stFormations;

  for(TOpenGLNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  const IFormationElementSet* pFormationElementSet = dynamic_cast<const IFormationElementSet*>(*it);
  if(pFormationElementSet)
      stFormations.insert(&pFormationElementSet->Formation());
  }

  stNodes.insert(stFormations.begin(), stFormations.end());

  return stNodes;
}
