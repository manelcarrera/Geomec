// CrossSection.cpp: implementation of the CCrossSection class.
//
//////////////////////////////////////////////////////////////////////

#include <stdexcept>

#include "NewWellPath.h"
#include "CrossSection.h"
#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "BoundaryBase.h"
#include "FormationVolume.h"
#include "MeshBase.h"
#include "TSSurfaceProgress.h"
#include "TSSurface.h"
#include "Surface.h"
#include "3DFormation.h"
#include "FemAppModel.h"
#include "BodyGroup.h"
#include "Global.h"
#include "IProgressFactory.h"

#include "ModifiedHint.h"
#include "WellCasingModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCrossSection::CCrossSection(CFemAppModel &model, const geo::IPoint &basePoint, const geo::IVector &normal)
: CStorageNode("", model), 
  m_pWellPathEntry(0), 
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  // We create a default name
  QString sName;
  sName = QString("New Cross section %1").arg(model.GraphEntry(MD_BASE_XSECTION)->GraphEntryNodes().size() + 1);
  Name(sName);
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if (m_pWellPathEntry)
    LinkTo(*m_pWellPathEntry);
  SetBookmarked(basePoint, normal, ORIENTATION_ANY);
  SetScreenFromBookmarked();
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
}

CCrossSection::CCrossSection(const QString &sName, CFemAppModel &model, const geo::IPoint &basePoint, const geo::IVector &normal)
: CStorageNode(sName, model), 
  m_pWellPathEntry(0), 
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if( m_pWellPathEntry )
    LinkTo(*m_pWellPathEntry);
  SetBookmarked(basePoint, normal, ORIENTATION_ANY);
  SetScreenFromBookmarked();
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
}

CCrossSection::CCrossSection(CFemAppModel &model, const geo::IPoint &first, const geo::IPoint &second)
: CStorageNode("", model), 
  m_pWellPathEntry(0), 
  m_orientationBookmarked(ORIENTATION_NE),
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  // We create a default name
  QString sName;
  sName = QString("New Cross section %1").arg(model.GraphEntry(MD_BASE_XSECTION)->GraphEntryNodes().size() + 1);
  Name(sName);
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if( m_pWellPathEntry )
    LinkTo(*m_pWellPathEntry);
  SetBookmarked(first, second);
  SetScreenFromBookmarked();
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
}

CCrossSection::CCrossSection(const QString &sName, CFemAppModel &model, const geo::IPoint &first, const geo::IPoint &second)
: CStorageNode(sName, model), 
  m_pWellPathEntry(0), 
  m_orientationBookmarked(ORIENTATION_NE),
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if( m_pWellPathEntry )
    LinkTo(*m_pWellPathEntry);
  SetBookmarked(first, second);
  SetScreenFromBookmarked();
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
}

CCrossSection::CCrossSection(CFemAppModel& model, const double& dDepth)
: CStorageNode("", model),
  m_pWellPathEntry(0),
  m_orientationBookmarked(ORIENTATION_DEPTH),
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  LinkTo((static_cast<CModelBase&>(model)).Mesh());
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if (m_pWellPathEntry)
  LinkTo(*m_pWellPathEntry);

  SetBookmarked(dDepth);
  SetScreenFromBookmarked();
}

CCrossSection::CCrossSection(const QString& sName, CFemAppModel& model, const double& dDepth)
: CStorageNode(sName, model),
  m_pWellPathEntry(0),
  m_orientationBookmarked(ORIENTATION_DEPTH),
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  LinkTo((static_cast<CModelBase&>(model)).Mesh());
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if (m_pWellPathEntry)
  LinkTo(*m_pWellPathEntry);

  SetBookmarked(dDepth);
  SetScreenFromBookmarked();
}

CCrossSection::CCrossSection(const QString &sName, CFemAppModel &model)
: CStorageNode(sName, model), 
  m_pWellPathEntry(0), 
  m_orientationBookmarked(ORIENTATION_ANY),
  m_pModifier(0),
  m_pExportSurface(0),
  m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if( m_pWellPathEntry )
    LinkTo(*m_pWellPathEntry);
}

CCrossSection::CCrossSection( CFemAppModel &model )
: CStorageNode("", model), 
 //m_bXsecValid(false), 
 m_pWellPathEntry(0), 
 m_orientationBookmarked(ORIENTATION_ANY),
 m_pModifier(0),
 m_pExportSurface(0),
 m_bDirty(true)
, m_crossSectionType(VERTICAL)
{
#if 0
  reParent(model.GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&model);
  LinkTo(pModel->Mesh());
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  LinkTo(*m_pWellPathEntry);
#endif
}

CCrossSection::CCrossSection(const CCrossSection &rhs)
: CStorageNode(rhs),
 //m_bXsecValid(false), 
 m_pWellPathEntry(rhs.m_pWellPathEntry), 
 m_NormalBookmarked(rhs.m_NormalBookmarked),
 m_IntersectPlaneBookmarked(rhs.m_IntersectPlaneBookmarked),
 m_orientationBookmarked(rhs.m_orientationBookmarked),
 m_pModifier(0),
 m_pExportSurface(rhs.m_pExportSurface),
 m_bDirty(true)
, m_crossSectionType(rhs.m_crossSectionType)
{
  SetScreenFromBookmarked();
}

void CCrossSection::Clear(bool bSendModified)
{
  m_bDirty = true;
  while(m_CSObjects.size() != 0)
    delete *m_CSObjects.begin();
  if(m_pExportSurface)
    delete m_pExportSurface;

  if(bSendModified)
    Modified();
}

CCrossSection& CCrossSection::operator =(const CCrossSection &rhs)
{
  Clear();

  CStorageNode::operator =(rhs);

  m_orientationBookmarked = rhs.m_orientationBookmarked;

  m_NormalBookmarked = rhs.m_NormalBookmarked;
  m_IntersectPlaneBookmarked = rhs.m_IntersectPlaneBookmarked;
  m_crossSectionType = rhs.m_crossSectionType;

  SetScreenFromBookmarked();

  m_pModifier=0;

  return *this;
}

bool CCrossSection::operator ==(const CCrossSection &rhs) const
{
  assert (false);
  if(!CStorageNode::operator ==(rhs))
    return false;

  if(m_orientationBookmarked != rhs.m_orientationBookmarked)
    return false;

  return false;

  }

void CCrossSection::OnDeselect(CGraphNode* /*NewNode*/)
{
  if(m_pModifier)
    m_pModifier->Cancel();
}

// PRE:		Cross section should be cleared (m_bDirty == true)
// POST:	Cross section is valid(m_bDirty == false)
void CCrossSection::Recalculate()
{
  assert(m_bDirty);
  assert(m_CSObjects.size() == 0);
  //m_bXsecValid = true;

  CModelBase& model = dynamic_cast<CModelBase&> (Model());

  /*CWellCasingModel* model_casing = dynamic_cast<CWellCasingModel*>( &Model() );

  if( model_casing ) // special dcasing case
  {
    TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for(TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); iter++)
    {
      if( dynamic_cast<C3DFormation*>(*iter) ) //don't take in account the weel path
      {
        C3DFormation& formation = dynamic_cast<C3DFormation&>(**iter);
        new CCSFormation(*this, formation);
      }
      else //cement and steel 
      {
        CFormationBase& formation = dynamic_cast<CFormationBase&>(**iter);
        Casing3DFormation* f3d = new Casing3DFormation( formation, (geo::CBodyGroup&)model_casing->Mesh().CementElements() ); //FIXME: it nedds to be tracked and deleted
        //SteelElements()
        new CCSFormation(*this, *f3d);
      }
    }
  }
  else // regular case
  {
    TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for(TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); iter++)
    {
      C3DFormation& formation = dynamic_cast<C3DFormation&>(**iter);
      new CCSFormation(*this, formation);
    }
  }*/

  TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
  for(TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); iter++)
  {
    if( dynamic_cast<C3DFormation*>(*iter) ) //don't take in account the weel path
    {
      C3DFormation& formation = dynamic_cast<C3DFormation&>(**iter);
      new CCSFormation(*this, formation);
    }
  }

  m_bDirty = false;
//	Modified();
}

CCrossSection::~CCrossSection()
{
  // All the CSobjects delete themselves...
  if(m_pExportSurface)
    delete m_pExportSurface;
  if(m_pModifier)
    m_pModifier->Cancel();
}


void CCrossSection::SetBookmarked(const geo::IPoint& basePoint, const geo::IVector& normal, OrientationType orientation, bool bSendModified)
{
  m_NormalBookmarked = normal;
  m_IntersectPlaneBookmarked = geo::CPlane(basePoint, normal);
  m_orientationBookmarked = orientation;

  Clear(bSendModified);
}

void CCrossSection::SetScreen(const geo::IPoint& basePoint, const geo::IVector& normal, bool bSendModified)
{
  m_NormalScreen = normal;
  m_IntersectPlaneScreen = geo::CPlane(basePoint, normal);

  Clear(bSendModified);
}

void CCrossSection::SetScreen(const geo::IPoint& basePoint, const geo::IVector& normal, OrientationType orientation, bool bSendModified)
{
  m_NormalScreen = normal;
  m_IntersectPlaneScreen = geo::CPlane(basePoint, normal);
  m_orientationScreen = orientation;

  Clear(bSendModified);
}

void CCrossSection::SetBookmarked(const geo::IPoint& first, const geo::IPoint& second, bool bSendModified)
  {
  geo::CVector Vect(first, second);
  m_NormalBookmarked = Vect.CrossProduct(geo::CVector::Zaxis);
  m_IntersectPlaneBookmarked = geo::CPlane(first, m_NormalBookmarked);
  m_orientationBookmarked = ORIENTATION_NE;

  Clear(bSendModified);
}

void CCrossSection::SetBookmarked(const double& dDepth, bool bSendModified)
{
  CModelBase& model = dynamic_cast<CModelBase&> (Model());

  m_NormalBookmarked = geo::CVector::Zaxis * -1;
  geo::CPoint pt(model.Boundary().Min());
  pt.Z(dDepth);

  m_IntersectPlaneBookmarked = geo::CPlane(pt, m_NormalBookmarked);
  m_orientationBookmarked = ORIENTATION_DEPTH;

  Clear(bSendModified);
}

void CCrossSection::CopyFromScreen()
{
  m_orientationBookmarked = m_orientationScreen;
  m_NormalBookmarked = m_NormalScreen;
  m_IntersectPlaneBookmarked =  m_IntersectPlaneScreen;
}

void CCrossSection::SetScreenFromBookmarked()
{
  m_orientationScreen = m_orientationBookmarked;
  m_NormalScreen = m_NormalBookmarked;
  m_IntersectPlaneScreen = m_IntersectPlaneBookmarked;
}



const std::vector<CCrossSection::CCSObject*>& CCrossSection::CSObjects() const
{
  if(m_bDirty)
  {
    CCrossSection* pXSec = const_cast<CCrossSection*>(this);
    pXSec->Recalculate();
  }
  return m_CSObjects;
}

void CCrossSection::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // Just load base
  CStorageNode::LoadStream(stream, version, progress);

  double dDepth;


  OrientationType orientation;
  geo::CPoint basePoint;
  geo::CVector normal;

  // Version 4.1.19 introduced cross sections that can have any orientation
  if(CStreamVersion(4, 1, 18) < version)
  {
  int orientationType;
  stream >> orientationType;

    orientation = (OrientationType)orientationType;

  double basePointX, basePointY, basePointZ;
  stream >> basePointX;
  stream >> basePointY;
  stream >> basePointZ;

  double normalX, normalY, normalZ;
  stream >> normalX;
  stream >> normalY;
  stream >> normalZ;

    basePoint = geo::CPoint (basePointX, basePointY, basePointZ);
    normal = geo::CVector(normalX, normalY, normalZ);

    stream >> dDepth;
  }
  else if(CStreamVersion(3, 0, 88) < version)
  {
    // load state and depth for horizontal x-sections
    int iHorizontal;
    stream >> iHorizontal;

    orientation = (iHorizontal != 0) 
      ? ORIENTATION_DEPTH 
      : ORIENTATION_NE;

    stream >> dDepth;
  }
  else
  {
  orientation = ORIENTATION_NE;
  }

  
  // Get the coordinates.
  double x,y,z;
  
  geo::CPoint first;
  geo::CPoint second;

  // Point 1
  stream >> x;
  stream >> y;
  stream >> z;

  first = geo::CPoint(x,y,z);

  // Point 2
  stream >> x;
  stream >> y;
  stream >> z;

  second = geo::CPoint(x,y,z);

  if (version >= CStreamVersion(4, 1, 42))
  {
      int crossSectionType = 0;

      stream >> crossSectionType;

      m_crossSectionType = TCrossSectionType(crossSectionType);
  }
  else
  {
      m_crossSectionType = MANUAL;
  }

  if (version < CStreamVersion(4, 1, 18))
  {
  // Calculate normal enz.
    switch(orientation)
  {
    case ORIENTATION_DEPTH:
      SetBookmarked(dDepth, false);
  break;

    case ORIENTATION_NE:
      SetBookmarked(first, second, false);
  break;

    default:
      SetBookmarked(basePoint, normal, orientation, false);
  break;
  }
  }
  else
  {
    SetBookmarked(basePoint, normal, orientation, false);
  }
  SetScreenFromBookmarked();

  assert(m_bDirty);

  progress.Step();
  
  // Get well path entry
  m_pWellPathEntry = (CNewWellPathEntry*)(Model().GraphEntry(MD_NEW_WELLPATH));
  if (m_pWellPathEntry)
    LinkTo(*m_pWellPathEntry);

  reParent(Model().GraphEntry(MD_BASE_XSECTION));
  CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
  LinkTo(pModel->Mesh());
}

void CCrossSection::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Just save base
  CStorageNode::SaveStream(stream, progress);

  // Save type
#if 0
  int orientationBookmarked = (int)m_orientationBookmarked;

  // This is an intentional bug, required to stay compatible with the current release
  // To be removed before the actual OIV release.
  if (orientationBookmarked > ORIENTATION_ANY)
  {
    switch (m_orientationBookmarked)
    {
    case ORIENTATION_EASTING:
    case ORIENTATION_NORTHING:
      orientationBookmarked = (int) ORIENTATION_NE;
      break;
    case ORIENTATION_CURRENT:
    default:
      orientationBookmarked = ORIENTATION_ANY;
    }
  }
  stream << orientationBookmarked;
#else           
  stream << m_orientationBookmarked;
#endif

  // Save plane parameters
  geo::CVector basePoint = m_IntersectPlaneBookmarked.BasePoint();
  stream << basePoint.X() << basePoint.Y() << basePoint.Z();
  stream << m_NormalBookmarked.X() << m_NormalBookmarked.Y() << m_NormalBookmarked.Z();

  stream << basePoint.Z();

  geo::CPoint first, second;

  if (!calculateTwoArbitraryPointsInPlane(first, second,
      m_IntersectPlaneBookmarked, dynamic_cast<CModelBase&> (Model())))
  {
    first = geo::CPoint(basePoint);//first.X(), first.Y(), model.Boundary().Min().Z());
    second = geo::CPoint(basePoint);//second.X(), second.Y(), model.Boundary().Min().Z());
  }

  stream << first.X() << first.Y() << first.Z();
  stream << second.X() << second.Y() << second.Z();

  int crossSectionType = m_crossSectionType;

  stream << crossSectionType;

  progress.Step();
}

void CCrossSection::OnNewNeighbour(const CGraphNode &node)
{
  const CCSObject *pObject = dynamic_cast<const CCSObject*>(&node);
  if(pObject)
  {
    // Add to vector
    m_CSObjects.push_back(const_cast<CCSObject*>(pObject));
  }
  
  CStorageNode::OnNewNeighbour(node);
}

void CCrossSection::OnNeighbourDeleted(const CGraphNode& node)
{
  for(std::vector<CCSObject*>::iterator it = m_CSObjects.begin(); it != m_CSObjects.end(); it++)
  {
    if(*it == &node)
    {
      m_CSObjects.erase(it);
      return;
    }
  }

  CStorageNode::OnNeighbourDeleted(node);
}

void CCrossSection::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if (m_pWellPathEntry && m_pWellPathEntry == &node)
  {
    
    CNewWellPathEntry::TNodeSet stWellpath = m_pWellPathEntry->EntryNodes();
    for(CNewWellPathEntry::TNodeSet::iterator it = stWellpath.begin(); it != stWellpath.end();it++)
    {
      CNewWellPath* pWellpath = (*it);
      if(!pWellpath->IsVertical())
      {
        bool bFound = false;
        // Try to find in the array
        for(size_t i = 0; i < m_CSObjects.size(); i++)
        {
          CCSWellPath *pCSWellPath = dynamic_cast<CCSWellPath*>(m_CSObjects[i]);
          if(pCSWellPath && (&pCSWellPath->WellPath() == pWellpath))
            bFound = true;

        }
        if(!bFound)
          Clear();
      }
    }
  }		

  if (dynamic_cast<const CMeshBase*>(&node))
  {
    if (uHint == MeshCleared)
    {
      Clear();
    }
  }
}

bool CCrossSection::CanFlip() const
{
  return (!m_NormalScreen.Empty());
}

void CCrossSection::Flip()
{
  m_NormalScreen.Flip();
  m_IntersectPlaneScreen = geo::CPlane(m_IntersectPlaneScreen.BasePoint(), m_NormalScreen);

  m_NormalBookmarked.Flip();
  m_IntersectPlaneBookmarked = geo::CPlane(m_IntersectPlaneBookmarked.BasePoint(), m_NormalBookmarked);

  Modified();
}

CCrossSection::OrientationType CCrossSection::Orientation() const
{
  return m_orientationBookmarked;
}

bool CCrossSection::Horizontal() const
{
  return m_orientationBookmarked == ORIENTATION_DEPTH;
}


long CCrossSection::SavedItems() const
{
  return CStorageNode::SavedItems() + 1;
}

unsigned int CCrossSection::IconId() const
{
  switch(m_orientationBookmarked)
  {
  case ORIENTATION_DEPTH:
  return IDI_XSECTION_HORIZONTAL;
  case ORIENTATION_NE:
  return IDI_XSECTION;
  case ORIENTATION_ANY:
  default:
  return IDI_XSECTION_ANY;
  }
}

unsigned int CCrossSection::TypeId() const
{
  return 0;
}

int CCrossSection::ElementSize() const
{
  if(m_pExportSurface)
    return m_pExportSurface->ElementSize();

  CCSFormation *pCSForm;
  int ret = 0;
  for(size_t i = 0; i < CSObjects().size(); i++)
  {
    pCSForm = dynamic_cast<CCSFormation*>(CSObjects()[i]);
    if(pCSForm)
    {
      for(int j = 0; j < pCSForm->SurfaceSize(); j++)
        ret += pCSForm->Surface(j).FaceSize();
    }
  }

  return ret;
}

CTSSurface *CCrossSection::ExportSurface() const
{
  if(m_pExportSurface)
    return m_pExportSurface;

  std::vector<const geo::IFace*> vcFaces;
  CCSFormation *pCSForm;
  for(size_t i = 0; i < CSObjects().size(); i++)
  {
    pCSForm = dynamic_cast<CCSFormation*>(CSObjects()[i]);
    if(pCSForm)
    {
      for(int j = 0; j < pCSForm->SurfaceSize(); j++)
      {
        for(int k = 0; k < pCSForm->Surface(j).FaceSize(); k++)
          vcFaces.push_back(&pCSForm->Surface(j).Face(k));
      }
    }
  }

  CTSSurfaceProgress SurfaceProgress;
  m_pExportSurface = new CTSSurface(vcFaces, SurfaceProgress);
  return m_pExportSurface;
}

geo::CPoint CCrossSection::First() const
{
  throw std::logic_error("The method or operation is not implemented.");
}

geo::CPoint CCrossSection::Second() const
{
  throw std::logic_error("The method or operation is not implemented.");
}

//***************************************************************************************
//************	CCSObject implementation	*****************************************
CCrossSection::CCSObject::CCSObject(CCrossSection& cross_section, COpenGLNode& base_node)
: COpenGLNode(cross_section.Name().toStdString().c_str(), cross_section.Model()), m_cross_section(cross_section), m_base_node(base_node)
{
}

const CCrossSection& CCrossSection::CCSObject::CrossSection() const
{
  return m_cross_section;
}

CCrossSection& CCrossSection::CCSObject::CrossSection()
{
  return m_cross_section;
}

unsigned int CCrossSection::CCSObject::IconId() const
{
  return m_base_node.IconId();
}

unsigned int CCrossSection::CCSObject::TypeId() const
{
  return m_base_node.TypeId();
}

QString CCrossSection::CCSObject::TypeName() const
{
  return m_base_node.TypeName();
}

const QString& CCrossSection::CCSObject::Name() const
{
  return m_base_node.Name();
}

CCrossSection::CCSObject::TColor CCrossSection::CCSObject::Color() const
{
  return m_base_node.Color();
}

bool CCrossSection::CCSObject::Less(const CGraphNode &node) const
{
  return m_base_node.Less(node);
}

COpenGLNode::CDrawDef* CCrossSection::CCSObject::OnCreateDrawDef(const geo::IObject& /*object*/) const
{
  return new CCSObjectDrawDef(*this);
}

void CCrossSection::CCSObject::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  COpenGLNode::OnNeighbourModified(node, uHint);

  if(&node == &CrossSection())
    Modified();
}

void CCrossSection::CCSObject::OnNeighbourDeleted(const CGraphNode &node)
{
  if(&m_base_node == &node || &m_cross_section == &node)
  {
    delete this;
    return;
  }
  COpenGLNode::OnNeighbourDeleted(node);
}

//***************************************************************************************
//************	CCSFormation implementation	*****************************************
CCrossSection::CCSFormation::CCSFormation(CCrossSection &cross_section, C3DFormation &base_formation)
: CCSObject(cross_section, base_formation), m_bDirty(true), m_BaseFormation(base_formation)
{
  if(!base_formation.IsCopy())
    LinkTo(base_formation);
  if(!cross_section.IsCopy())
    reParent(&cross_section);
}

CCrossSection::CCSFormation::~CCSFormation()
{
  Invalidate();
  assert(m_vcSurface.size() == 0);
}

void CCrossSection::CCSFormation::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  CCSObject::OnNeighbourModified(node, uHint);

  if(&m_BaseFormation == &node)
  {
    if (uHint != MeshCleared)
    {
      Invalidate();
      Modified();
    }
  }
}

void CCrossSection::CCSFormation::Invalidate()
{
  // Clear the surface
  for(size_t i = 0; i < m_vcSurface.size(); i++)
    delete m_vcSurface[i];

  m_vcSurface.clear();
  m_bDirty = true;
}

const C3DFormation &CCrossSection::CCSFormation::BaseObject() const 
{
  return m_BaseFormation;
}

C3DFormation &CCrossSection::CCSFormation::BaseObject() 
{
  return m_BaseFormation;
}

bool CCrossSection::CCSFormation::Less(const CGraphNode &node) const
{
  const CCrossSection::CCSFormation *pRhs = dynamic_cast<const CCrossSection::CCSFormation*>(&node);
  if(pRhs)
    return m_BaseFormation.Less(pRhs->BaseObject());
  return CCSObject::Less(node);
}


// PRE	: There should be a mesh available
void CCrossSection::CCSFormation::CalculateIntersection() const
{
  std::auto_ptr <IProgressBase> cursor(_g->prog()->create(eProgress::Wait));

  const CModelBase* pModel = dynamic_cast<const CModelBase*>(&Model());
  assert(pModel);
  assert(pModel->IsMesh());

  // Calculate polygons intersecting the elements
  for(int i = 0; i < BaseObject().ElementSetSize(); i++)
  {
    geo::CSurface* pSurface = 0;
    std::set<int> stBody = BaseObject().Volume(i).Volume().Candidates(CrossSection().IntersectPlaneBookmarked());

    std::set<int>::const_iterator it;
    for(it = stBody.begin(); it != stBody.end(); ++it)
    {       
      const geo::IBody& body = BaseObject().Volume(i).Volume().Body(*it);
      geo::CPolygon polygon = body.IntersectionConv(CrossSection().IntersectPlaneBookmarked());

      // wedx 10012007:
      // if the plane 'touches' the body in a face IBody::IntersectionConv returns an empty polygon
      // handle this case explicitly here
      if(polygon.Empty())
      {
        int j;
        for(j = 0; j < body.NrOfFaces(); ++j)
        {
          const geo::IFace& face = body.Face(j);
          const geo::IPlane& plane = CrossSection().IntersectPlaneBookmarked();
          if(fabs(plane.Distance(face.Point(0))) < EPS)
          {
            if(plane.Normal() == face.Normal())
            {
              int p;
              for(p = 0; p < face.NrOfPoints(); ++p)
                polygon.PushBack(face.Point(p));
              break;
            }
            else if(plane.Normal().Flipped() == face.Normal())
            {
              int p;
              for(p = face.NrOfPoints() - 1; p >= 0; --p)
              polygon.PushBack(face.Point(p));
              break;
            }
          }
        }
      }

      if(!polygon.Empty())
      {
        if(!pSurface)
          pSurface = new geo::CSurface();

        assert(polygon.NrOfPoints() > 2);
        for(int p = 2; p < polygon.NrOfPoints(); p++)
          pSurface->AddEmbeddedTriangle(body, polygon.Point(0), polygon.Point(p-1), polygon.Point(p));
      }
    }
    if(pSurface)
      m_vcSurface.push_back(pSurface);
  }
  
  m_bDirty = false;
}

bool CCrossSection::CCSFormation::IsValid() const
{
  if(!m_bDirty)
    return m_vcSurface.size() > 0;

  // Is there a mesh?
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(model.IsMesh())
    CalculateIntersection();

  return m_vcSurface.size() > 0;
}

int CCrossSection::CCSFormation::DisplayListSize() const
{
  if(!m_bDirty)
    return m_vcSurface.size();

  // Is there a mesh?
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(model.IsMesh())
    CalculateIntersection();

  return m_vcSurface.size();
}

const geo::IObject& CCrossSection::CCSFormation::DisplayList(int nIndex) const
{
  // If we have a valid cross section return direct
  assert(IsValid());
  assert(nIndex >= 0);
  assert(nIndex < m_vcSurface.size());
  return *m_vcSurface[nIndex];
}

int CCrossSection::CCSFormation::SurfaceSize() const
{
  if(!m_bDirty)
    return m_vcSurface.size();

  // Is there a mesh?
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(model.IsMesh())
    CalculateIntersection();

  return m_vcSurface.size();
}

const geo::ISurface& CCrossSection::CCSFormation::Surface(int nIndex) const
{
  if(!m_bDirty)
    return *m_vcSurface[nIndex];

  // Is there a mesh?
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(model.IsMesh())
    CalculateIntersection();

  return *m_vcSurface[nIndex];
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// CSWellPath
///////////////////////////////////////////////////////////////////////////////////////////////
CCrossSection::CCSWellPath::CCSWellPath(CCrossSection &cross_section, CNewWellPath &base_wellpath)
: CCSObject(cross_section, base_wellpath), m_WellPath(base_wellpath)
{
  if(!m_WellPath.IsCopy())
    LinkTo(m_WellPath);
  if(!cross_section.IsCopy())
    reParent(&cross_section);
}

CCrossSection::CCSWellPath::~CCSWellPath()
{
  m_DisplayList.ClearAndDelete();
}

void CCrossSection::CCSWellPath::CalculateIntersection() const
{

  assert(!m_DisplayList.Size());
  //update displaylist with new intersection ...
  //CCrossSection::CCSWellPath* pThis=const_cast<CCrossSection::CCSWellPath*>(this);
  WellPath().GetIntersectionPointsWithPlane(m_DisplayList, CrossSection().IntersectPlaneBookmarked());
}

int CCrossSection::CCSWellPath::DisplayListSize() const
{
  return 1;
}



const geo::IObject & CCrossSection::CCSWellPath::DisplayList(int /*nIndex*/) const
{
  if(m_DisplayList.Size() == 0)
    CalculateIntersection();

  return m_DisplayList;
}

void CCrossSection::CCSWellPath::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  CCSObject::OnNeighbourModified(node, uHint);

  if(&m_WellPath == &node)
  {
    m_DisplayList.ClearAndDelete();
    Modified();

  }		
}

const CNewWellPath & CCrossSection::CCSWellPath::WellPath() const
{
  return m_WellPath;
}

CNewWellPath & CCrossSection::CCSWellPath::WellPath()
{
  return m_WellPath;
}

const geo::IPoint& CCrossSection::basePoint() const
{
  return m_IntersectPlaneBookmarked.BasePoint();
}

const geo::IVector& CCrossSection::normal() const
{
  return m_NormalBookmarked;
}

CCrossSection::TCrossSectionType CCrossSection::crossSectionType() const
{
  return m_crossSectionType;
}

void CCrossSection::crossSectionType(TCrossSectionType crossSectionType)
{
  m_crossSectionType = crossSectionType;
}

// static

bool CCrossSection::calculateTwoArbitraryPointsInPlane(geo::IPoint& first,
  geo::IPoint& second, const geo::IPlane& plane, const CModelBase& modelBase)
{
  geo::CBox boundingBox(modelBase.Boundary().Min(), modelBase.Boundary().Max());
  std::set <geo::CLine, geo::ILine::CLineLess> lines;
  std::set <geo::CPoint> points;

  boundingBox.Intersection(plane, lines, &points);

  if (!points.empty())
  {
  geo::CPoint begin = *(points.begin());
  geo::CPoint end = *(points.rbegin());

  first = geo::CPoint(begin.X(), begin.Y(), modelBase.Boundary().Min().Z());
  second = geo::CPoint(end.X(), end.Y(), modelBase.Boundary().Min().Z());

  return true;
  }

  return false;
}

CCrossSectionEntry::CCrossSectionEntry(CModelBase &model) 
: CStorageNodeEntry<CCrossSection>(MD_BASE_XSECTION, IDI_XSECTIONS, IDS_TREE_XSECTIONS, model)
{
}
