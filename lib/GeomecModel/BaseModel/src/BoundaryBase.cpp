// BoundaryBase.cpp: implementation of the CBoundaryBase class.
//
//////////////////////////////////////////////////////////////////////


#include "BoundaryBase.h"
#include "BaseEntryTypes.h"
#include "ValueTypes.h"
#include "NewWellPath.h"
#include "AnalysisPoint.h"
#include "CrossSection.h"
#include "BaseSupportNode.h"
#include "FemAppModel.h"
#include "Box.h"
#include "ModelBase.h"
#include "GeomecStringTable.h"
#include "FemAppEntryTypes.h"
#include "ModifiedHint.h"
#include "RotatedSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//##ModelId=3BE7AEC70247
CBoundaryBase::CBoundaryBase(const geo::IPoint& ptMin,
           const geo::IPoint& ptMax,
           CFemAppModel& model,
           BOUNDARY_STATE state)
: CColorNode(IDS_BOUNDARY, model, qRgb(126, 126, 126)),
  m_bsState(state),
  m_pBox(0),
  m_ptMin(ptMin),
  m_ptMax(ptMax),
  m_ptRotation(0, 0, 0),
  m_dAzimuth(0),
  m_dLength(fabs(ptMin.X() - ptMax.X())),
  m_dWidth(fabs(ptMin.Y() - ptMax.Y()))
{
  reParent(&Model());

  // Create the boundary ...
  m_pBox = new geo::CBox(ptMin, ptMax);

  assert(ptMin == Min());
  assert(ptMax == Max());


}

CBoundaryBase::CBoundaryBase(const CBoundaryBase &rhs)
: CColorNode(rhs),
  m_bsState(rhs.State()),
  m_pBox(0),
  m_ptMin(rhs.m_ptMin),
  m_ptMax(rhs.m_ptMax),
  m_ptRotation(rhs.m_ptRotation),
  m_dAzimuth(rhs.m_dAzimuth),
  m_dLength(rhs.m_dLength),
  m_dWidth(rhs.m_dWidth)
{
  assert(rhs.IsCopied());
  assert(!rhs.IsCopy());

  // Create boundary 
  m_pBox = new geo::CBox(rhs.Min(), rhs.Max());

  if(m_bsState == ROTATED)
  Set(m_ptRotation, m_dAzimuth, m_dLength, m_dWidth, rhs.Min().Z(), rhs.Max().Z());
  else
  Set(rhs.Min(), rhs.Max());
}

//##ModelId=3BC55D6402F8
CBoundaryBase::~CBoundaryBase()
{
  delete m_pBox;
}

void CBoundaryBase::RectangleXY(const geo::CRectangle& RectangleXY)
{
  double zMax =  m_pBox->Max().Z();
  double zMin =  m_pBox->Min().Z();

  geo::CPoint max = RectangleXY.Max();
  geo::CPoint min = RectangleXY.Min();

  max.Z(zMax);
  min.Z(zMin);

  Set(min, max);
}

const geo::CBox &CBoundaryBase::Box() const
{
  return *m_pBox;
}

geo::CRectangle CBoundaryBase::RectangleXY() const
{
  CRotatedSystem rs(BoxAzimuth(), Box().MidPoint());

  geo::CPoint max(rs.LocalMax(*m_pBox));
  geo::CPoint min(rs.LocalMin(*m_pBox));

  max.Z(0);
  min.Z(0);

  geo::CVector vec1(min, max);
  vec1.Y(0);
  geo::CVector vec2(min, max);
  vec2.X(0);

  min = rs.ToGlobal(min);
  vec1 = rs.ToGlobal(vec1);
  vec2 = rs.ToGlobal(vec2);

  return geo::CRectangle(min, vec1, vec2);
}

bool CBoundaryBase::operator==(const CBoundaryBase& rhs) const
{
  if(!CColorNode::operator ==(rhs))
    return false;

  return (
  m_bsState == rhs.m_bsState &&
  m_ptMin == rhs.m_ptMin &&
  m_ptMax == rhs.m_ptMax &&
  m_ptRotation == rhs.m_ptRotation &&
  fabs(m_dAzimuth - rhs.m_dAzimuth) < EPS &&
  fabs(m_dLength - rhs.m_dLength) < EPS &&
  fabs(m_dWidth - rhs.m_dWidth) < EPS);
}
  
CBoundaryBase& CBoundaryBase::operator=(const CBoundaryBase& rhs)
{
  CColorNode::operator=(rhs);

  m_bsState = rhs.m_bsState;
  m_ptMin = rhs.m_ptMin;
  m_ptMax = rhs.m_ptMax;
  m_ptRotation = rhs.m_ptRotation;
  m_dAzimuth = rhs.m_dAzimuth;
  m_dLength = rhs.m_dLength;
  m_dWidth = rhs.m_dWidth;

  if(m_bsState == ROTATED)
  Set(m_ptRotation, m_dAzimuth, m_dLength, m_dWidth, rhs.Min().Z(), rhs.Max().Z());
  else
    Set(m_ptMin, m_ptMax);

  return *this;
}

//##ModelId=3BE7AEC702C4
bool CBoundaryBase::Set(const geo::IPoint& ptMin, const geo::IPoint& ptMax)
{
  return OnSet(TMinMax(ptMin, ptMax));
}

bool CBoundaryBase::Set(const geo::IPoint& ptMid, double dAzimuth, double dLength, double dWidth, double dMinDepth, double dMaxDepth)
{
  assert(CanRotate());
  return OnSet(TRotated(ptMid, dAzimuth, dLength, dWidth, dMinDepth, dMaxDepth));
}

bool CBoundaryBase::OnSet(const TMinMax& minmax)
{
  m_ptMin = minmax.first;
  m_ptMax = minmax.second;

  // Create new box
  geo::CBox* pBox = new geo::CBox(minmax.first, minmax.second);

  return OnNewBox(pBox);
}

geo::CBox* CBoundaryBase::CreateRotatedBox(const TRotated& rotated) const
{
  double a = DEG2RAD(rotated.dAzimuth);

  geo::CVector v1(rotated.dLength * cos(a), rotated.dLength * sin(a));
  geo::CVector v2(-rotated.dWidth * sin(a), rotated.dWidth * cos(a));

  geo::CBox* pBox = new geo::CBox(geo::CPoint(rotated.ptMid.X() + rotated.dWidth / 2. * sin(a) - rotated.dLength / 2. * cos(a),
                                              rotated.ptMid.Y() - rotated.dWidth / 2. * cos(a) - rotated.dLength / 2. * sin(a),
                                              rotated.dMinDepth),
                                  v1, v2, geo::CVector(0, 0, rotated.dMaxDepth - rotated.dMinDepth));

  return pBox;
}

bool CBoundaryBase::OnSet(const TRotated& rotated)
{
  assert(CanRotate());

  m_ptRotation = rotated.ptMid;
  m_dAzimuth = rotated.dAzimuth;
  m_dLength = rotated.dLength;
  m_dWidth = rotated.dWidth;

  return OnNewBox(CreateRotatedBox(rotated));
}

bool CBoundaryBase::OnNewBox(geo::CBox* pNewBox)
{
  COperation oper(Model());

  if(*pNewBox == *m_pBox)
  {
  // no need to update
  delete pNewBox;
  return false;
  }

  delete m_pBox;
  m_pBox = pNewBox;

  // Invalidate the mesh
  if(InvalidateMeshOnSet() && !IsCopy())
  {
    CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
    assert(pModel);
    pModel->InvalidateMesh();
  OnNeighbourModified(pModel->Mesh(), MeshCleared);
  }

  // Boundary is modified
  Modified();

  return true;
}

void CBoundaryBase::CorrectMeshDepth(const double& dMin, const double& dMax)
{
  assert(m_pBox);

  if((dMax == m_pBox->Max().Z()) && (dMin == m_pBox->Min().Z()))
    return;		// No need for updating ...

  if(m_bsState == ROTATED)
  {
  m_pBox = CreateRotatedBox(TRotated(m_ptRotation, m_dAzimuth, m_dLength, m_dWidth, dMin, dMax));
  }
  else
  {
  	TMinMax minmax = TMinMax(m_pBox->Min(), m_pBox->Max());
  m_pBox = new geo::CBox(geo::CPoint(minmax.first.X(),  minmax.first.Y(),  dMin),
                           geo::CPoint(minmax.second.X(), minmax.second.Y(), dMax));
  }

  // Boundary is modified
  Modified();
}

bool CBoundaryBase::IsCube() const
{
  double dX, dY, dZ;
  dX = Max().X() - Min().X();
  dY = Max().Y() - Min().Y();
  dZ = Max().Z() - Min().Z();

  if(fabs(dX - dY) < EPS)
  {
    return fabs(dY - dZ) < EPS;
  }
  else
    return false;
}

//##ModelId=3BE7AEC70312
void CBoundaryBase::Min(const geo::IPoint& ptMin)
{
  // Just take the max and set ..
  geo::CPoint ptMax = Max();
  Set(ptMin, ptMax);
}


//##ModelId=3BE7AEC70341
void CBoundaryBase::Max(const geo::IPoint& ptMax)
{
  // Just take the min and set ...
  geo::CPoint ptMin = Min();
  Set(ptMin, ptMax);
}

//##ModelId=3BC55D6402F5
void CBoundaryBase::OnNeighbourDeleted(const CGraphNode &node)
{
  assert(m_pBox);
  
  const COpenGLNode* pViewNode = dynamic_cast<const COpenGLNode*> (&node);

  if(pViewNode)
  {
    if(m_bsState == BEST_FIT)
    {
      TMinMax bestfit = BestFit();
      if(bestfit.first.Empty() && bestfit.second.Empty())
        State(DEFAULT_DEFINED);
      else
        Set(bestfit.first, bestfit.second);
    }
  }
  
  CColorNode::OnNeighbourDeleted(node);
}

CLengthQuantity CBoundaryBase::R() const
{
  return CLengthQuantity((fabs(Max().X() - Min().X()) + fabs(Max().Y() - Min().Y()) + fabs(Max().Z() - Min().Z())) / 6.0);
}

//##ModelId=3BC55D6402C9
CBoundaryBase::TMinMax CBoundaryBase::BestFit() const
{
  assert(IsCopy());

  CBoundaryBase *pBoundary = (CBoundaryBase*)(&Source());

  return pBoundary->BestFit();
}

void CBoundaryBase::OnGridModified()
{
  TMinMax bestfit = BestFit();
  if(bestfit.first.Empty())
  return;

  switch(State())
  {
  case ROTATED:
  {
      TRotated rotated = SnapToGrid(TRotated(m_ptRotation, m_dAzimuth, m_dLength, m_dWidth, bestfit.first.Z(), bestfit.second.Z()));
      Set(rotated.ptMid, rotated.dAzimuth, rotated.dLength, rotated.dWidth, rotated.dMinDepth, rotated.dMaxDepth);
      break;
  }
  case USER_DEFINED:
  {
      TMinMax minmax = SnapToGrid(TMinMax(m_ptMin, m_ptMax));
      Set(minmax.first, minmax.second);
      break;
  }
  case BEST_FIT:
  case DEFAULT_DEFINED:
  break;
  }
}

//##ModelId=3BC55D6402C7
unsigned int CBoundaryBase::IconId() const
{
  return IDI_BOUNDARY;
}

//##ModelId=3BC55D6402C5
unsigned int CBoundaryBase::TypeId() const
{
  return IDT_BOUNDARY;
}

QString CBoundaryBase::TypeName() const
{
  return getStringTableEntry(IDS_BOUNDARY);
}

CBoundaryBase::CDrawDef* CBoundaryBase::OnCreateDrawDef(const geo::IObject& /*object*/) const
{
  return new CBoundaryDrawDef(*this);
}

int CBoundaryBase::DisplayListSize() const
{
  return 1;
}

//##ModelId=3BE7AEC70370
const geo::IObject& CBoundaryBase::DisplayList(int /*nIndex*/) const
{
  assert(m_pBox);
  return *m_pBox;
}

bool CBoundaryBase::Empty() const
{
  return m_pBox == 0;
}



void CBoundaryBase::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Do base first
  CColorNode::SaveStream(stream, progress);

  progress.StatusMessage("Saving Boundary");
  int nState = State();
  stream << nState;
  progress.Step();
  // USER_DEFINED parameters
  stream << m_ptMax.X() << m_ptMax.Y() << m_ptMax.Z();
  progress.Step();
  stream << m_ptMin.X() << m_ptMin.Y() << m_ptMin.Z();
  progress.Step();
  // ROTATED parameters
  stream << m_ptRotation.X() << m_ptRotation.Y() << m_ptRotation.Z();
  stream << m_dAzimuth;
  stream << m_dLength;
  stream << m_dWidth;
  progress.Step();
}

void CBoundaryBase::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
{
  // Unlink from the node entries
  UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
  UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));

  // Let base do its job
  CColorNode::LoadStream(stream, version, progress);

  // Set name and job
  progress.StatusMessage("Loading Boundary");
  QString strName;
  strName = getStringTableEntry(IDS_BOUNDARY);
  Name(strName);

  // Load state ...
  int state;
  stream >> state;
  progress.Step();

  // Load max and min
  double x, y, z;

  stream >> x >> y >> z;
  m_ptMax.Set(x, y, z);
  progress.Step();

  stream >> x >> y >> z;
  m_ptMin.Set(x, y, z);
  progress.Step();

  if(version >= CStreamVersion(4, 1, 6))
  {
  stream >> x >> y >> z;
  m_ptRotation.Set(x, y, z);
  stream >> m_dAzimuth;
  stream >> m_dLength;
  stream >> m_dWidth;
  progress.Step();
  }

  m_bsState = (BOUNDARY_STATE)state;

  if(m_bsState == ROTATED)
  CBoundaryBase::OnSet(TRotated(m_ptRotation, m_dAzimuth, m_dLength, m_dWidth, m_ptMin.Z(), m_ptMax.Z()));
  else
    CBoundaryBase::OnSet(TMinMax(m_ptMin, m_ptMax));
}

long CBoundaryBase::SavedItems() const
{
  return CColorNode::SavedItems() + 4;
}

CBoundaryBase::BOUNDARY_STATE CBoundaryBase::State() const
{
  return m_bsState;
}

void CBoundaryBase::State(CBoundaryBase::BOUNDARY_STATE state)
{
  if(State() == state)
    return;

  switch(state)
  {
  case DEFAULT_DEFINED:
    m_bsState = state;
    break;
  case USER_DEFINED:
    m_bsState = state;
    break;
  case BEST_FIT:
    {
      TMinMax minmax = BestFit();
      assert(!minmax.first.Empty() && !minmax.second.Empty());
      Set(minmax.first, minmax.second);
      m_bsState = state;
      Modified();
    }
    break;
  case ROTATED:
  m_bsState = state;
  break;
  default:
    assert(false);
  }
}

geo::CPoint CBoundaryBase::Max() const
{
  assert(m_pBox);
  return m_pBox->Max();
}

geo::CPoint CBoundaryBase::Min() const
{
  assert(m_pBox);
  return m_pBox->Min();
}

double CBoundaryBase::BoxAzimuth() const
{
  if(m_bsState == ROTATED)
  return m_dAzimuth;

  return 0;
}

const geo::CPoint& CBoundaryBase::MaxPoint() const
{
  return m_ptMax;
}

const geo::CPoint& CBoundaryBase::MinPoint() const
{
  return m_ptMin;
}

const geo::CPoint& CBoundaryBase::Mid() const
{
  return m_ptRotation;
}

double CBoundaryBase::Azimuth() const
{
  return m_dAzimuth;
}

double CBoundaryBase::Length() const
{
  return m_dLength;
}

double CBoundaryBase::Width() const
{
  return m_dWidth;
}

std::vector<CGraphNode*> CBoundaryBase::ItemsOutsideBoundary(const geo::CRectangle &rectangle) const
{
  // If vertical wellpaths, analysis points or cross sections are outside the boundary, 
    // they will have to be deleted, so first collect the pointers.
  CModelBase &model = (CModelBase&)(Model());
  std::vector<CGraphNode*> vc_ptrs;

  CNewWellPathEntry::TNodeSet new_well_paths = ((CNewWellPathEntry*)model.GraphEntry(MD_NEW_WELLPATH))->EntryNodes();
  for(CNewWellPathEntry::TNodeSet::iterator it = new_well_paths.begin(); it != new_well_paths.end(); ++it)
  {
  if((*it)->IsVertical())
      if(!rectangle.Contains(geo::CPoint((*it)->GlobalNorthing(), (*it)->GlobalEasting(), 0), false))
    vc_ptrs.push_back(*it);
  }

  CAnalysisPointEntry::TNodeSet an_points = ((CAnalysisPointEntry*)model.GraphEntry(MD_BASE_ANALYSIS_POINT))->EntryNodes();
  for(CAnalysisPointEntry::TNodeSet::iterator it2 = an_points.begin(); it2 != an_points.end(); it2++)
  {
    if(!rectangle.Contains(geo::CPoint((*it2)->Point().X(), (*it2)->Point().Y(), 0.0), false))
      vc_ptrs.push_back(*it2);
  }

  CCrossSectionEntry::TNodeSet x_sects = ((CCrossSectionEntry*)model.GraphEntry(MD_BASE_XSECTION))->EntryNodes();
  for(CCrossSectionEntry::TNodeSet::iterator it3 = x_sects.begin(); it3 != x_sects.end(); it3++)
  {
    const geo::IPlane & plane = (*it3)->IntersectPlaneBookmarked();
    if(rectangle.Intersection(plane).size() == 0)
      vc_ptrs.push_back(*it3);
  } 

  return vc_ptrs;
}

bool CBoundaryBase::HasInterfaces() const
{
  return false;
}

int CBoundaryBase::InterfaceNodeSize() const
{
  return 0;
}

const geo::INode& CBoundaryBase::InterfaceNode(int /*nIndex*/) const
{
  assert(false);
  const geo::INode* pBogus = 0;
  return *pBogus;
}

bool CBoundaryBase::CanAssignElementSet(const IElementSet& /*elset*/) const
{
  return HasInterfaces();
}

void CBoundaryBase::AssignElementSet(IElementSet& /*elset*/)
{
  //iterate over all children of the element set
  //
  for (size_t ii= 0; ii< childSize(); ++ii)
  {
    CValueType * pChildValue = dynamic_cast<CValueType *>( &childAt(ii) );

    // Is it a CValueType?
    if ( ! pChildValue ) continue;

    // parse name "D0_L" and get the number 
    //
    QString paramId= pChildValue->Name();

    // Does it look like the name is correct?
    assert( paramId.left(1 ) == "D");
    if ( paramId.left(1 ) != "D")
      continue;
    assert( paramId.right(2) == "_L");
    if ( paramId.right(2) != "_L")
      continue;

    // Get the ID part
    paramId= paramId.left(paramId.length() - 2 ); //"D0"
    paramId= paramId.right(paramId.length() - 1 ); //"0"

  const CModelBase& model = static_cast<const CModelBase&>(Model());
    const CDepletionStage* pStage = &model.InitialDepletionStage() ;

    // find depletion stage id that is equal to paramId
    QString stageId= "";
    while(pStage)
    {
      static const QString prefix = "Depletion stage ";
      static const int prefixLen = prefix.length();

      if ( pStage->Name() == "Initial" )
      {
        stageId= "0";
      }
      else
      {
        stageId=
        pStage->Name().right(pStage->Name().length() - prefixLen);
      }

      if ( stageId == paramId )
    break; //found it

      if(pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
    }

    if ( stageId != paramId )
      continue; //cannot assign
    assert(pStage);

  for(size_t i = 0; i < childSize(); ++i)
  {
      CBaseSupportNode* pSupNode = dynamic_cast<CBaseSupportNode*>(&childAt(i));
      if(pSupNode && pSupNode->SupportDepletion(*pStage)->CanConnectItem(*pChildValue))
      {
    pSupNode->SupportDepletion(*pStage)->ConnectItem(*pChildValue);
    break;
      }
  }
  }
}

bool CBoundaryBase::CanRotate() const
{
  if(IsCopy())
  return ((CBoundaryBase&)(Source())).CanRotate();

  return false;
}


bool operator==(const geo::CBox& lhs, const geo::CBox& rhs)
{
  assert(lhs.NrOfPoints() == rhs.NrOfPoints());

  for(int i = 0; i < lhs.NrOfPoints(); ++i)
  {
  if(!(lhs.Point(i) == rhs.Point(i)))
      return false;
  }

  return true;
}
