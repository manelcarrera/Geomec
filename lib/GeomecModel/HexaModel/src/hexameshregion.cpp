// MeshRegion.cpp: implementation of the CHexaMeshRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "HexaEntryTypes.h"
#include "hexameshregion.h"
#include "HexaModel.h"
#include "HexaBoundary.h"
#include "BranchState.h"
#include "Line.h"
#include "Box.h"
#include "resourceIDI.h"
#include "StreamVersion.h"
#include "resourceIDS.h"
#include "RotatedSystem.h"
#include "HexaFormation.h"

////////////////////////////////////////////////////////////////////
// Implemention of meshregion base
//////////////////////////////////////////////////////////////////////
CHexaMeshRegionBase::CHexaMeshRegionBase(CFemAppModel& model)
: CMeshRegionBase(model)
{
}

CHexaMeshRegionBase::CHexaMeshRegionBase(const QString& sName, CFemAppModel& model)
: CMeshRegionBase(sName, model)
{
}

CHexaMeshRegionBase::CHexaMeshRegionBase(const CHexaMeshRegionBase& rhs)
: CMeshRegionBase(rhs), m_hull(rhs.m_hull), m_vcGridPoints(rhs.m_vcGridPoints)
{
}

CHexaMeshRegionBase::~CHexaMeshRegionBase()
{
  // Simple clean up mesh by invalidating
  Invalidate();
}

CHexaMeshRegionBase& CHexaMeshRegionBase::operator=(const CHexaMeshRegionBase& rhs)
{
  CMeshRegionBase::operator=(rhs);
  return *this;
}

bool CHexaMeshRegionBase::operator==(const CHexaMeshRegionBase& rhs) const
{
  return CMeshRegionBase::operator ==(rhs);
}

bool CHexaMeshRegionBase::Destroy()
{
  CHexaModel* pModel = (CHexaModel*)(&Model());
  UnLinkAll();
  bool bRet = CStorageNode::Destroy();
  if (bRet)
    pModel->InvalidateMesh();

  return bRet;
}

const geo::CArray<geo::CPoint>& CHexaMeshRegionBase::MeshGrid() const
{
  if(IsInvalid())
  {
    CHexaMeshRegionBase *pRegion = (CHexaMeshRegionBase*)(this);
    pRegion->OnCreateMeshGrid();
  }

  return m_vcGridPoints;
}

void CHexaMeshRegionBase::Invalidate()
{
  m_hull.Clear();
  m_vcGridPoints.Clear();
  m_points.ClearAndDelete();
  m_lines.ClearAndDelete();
  m_displayList.Clear();

  Modified();
}

geo::CPoint CHexaMeshRegionBase::Min() const
{
  return Hull().Min();
}

geo::CPoint CHexaMeshRegionBase::Max() const
{
  return Hull().Max();
}

const geo::CPolygon& CHexaMeshRegionBase::Hull() const
{
  if(IsInvalid())
  {
    CHexaMeshRegionBase *pRegion = (CHexaMeshRegionBase*)(this);
    pRegion->OnCreateMeshGrid();
  }

  return m_hull;
}

bool CHexaMeshRegionBase::IsInvalid() const
{
  return ((m_vcGridPoints.Size() == 0) ||
      (m_hull.NrOfPoints() == 0));
}

//=============================================================================
// Create the meshgrid for a surface (z-value is not known yet).
void CHexaMeshRegionBase::OnCreateMeshGrid()
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CBoundaryBase& boundary = model.Boundary();
  CRotatedSystem rotsys(boundary.BoxAzimuth(), boundary.Box().MidPoint());

  if (m_hull.NrOfPoints() > 3)
  {
    double depth = m_hull.Min().Z();

    geo::CPoint Min = rotsys.LocalMin(m_hull);
    geo::CPoint Max = rotsys.LocalMax(m_hull);

    // All the points in the bounding box can be defined but a check
    // is needed to see if the point is also inside the polygon.
    for (double y = Min.Y(); y <= Max.Y() + EPS; y += GridSizeY())
    {
      for (double x = Min.X(); x <= Max.X() + EPS; x += GridSizeX())
      {
        geo::CPoint p(rotsys.ToGlobal(geo::CPoint(x,y,depth)));
        if (m_hull.Contains(p,true))
        {
          m_vcGridPoints.PushBack(p);
        }
      }
    }


    for(size_t i = 0; i < m_vcGridPoints.Size(); i++)
      m_points.PushBack(*new geo::CPoint(m_vcGridPoints.Object(i)));

    for(int i = 0; i < m_hull.NrOfLines(); i++)
    {
      m_lines.PushBack(*new geo::CLine(m_hull.Line(i)));
    }

    m_displayList.PushBack(m_points);
    m_displayList.PushBack(m_lines);
    //#if defined _DEBUG
#if 0
#endif
  }
}

//=============================================================================
bool CHexaMeshRegionBase::InsideRegion(const geo::IPoint& pt, bool bIncludeEdge) const
{
  // Is it in the hull?
  assert(m_hull.NrOfPoints() > 0);

  double depth = m_hull.Point(0).Z();

  return m_hull.Contains(geo::CPoint(pt.X(), pt.Y(), depth), bIncludeEdge);
}

bool CHexaMeshRegionBase::Empty() const
{
  return !IsLinkedTo(*((CModelBase&)Model()).GraphEntry(MD_HEXA_MESH_ZONE));
}

int CHexaMeshRegionBase::DisplayListSize() const
{
  return 1;
}

const geo::IObject& CHexaMeshRegionBase::DisplayList(int /*nIndex*/) const
{
  if(m_displayList.Empty())
  {
    CHexaMeshRegionBase *pRegion = (CHexaMeshRegionBase*)(this);
    pRegion->OnCreateMeshGrid();
  }

  return m_displayList;
}

bool CHexaMeshRegionBase::CanDestroy() const
{
  if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
    return false;

  return CMeshRegionBase::CanDestroy();
}

////////////////////////////////////////////////////////////////////
// Implemention of main meshregion 
//////////////////////////////////////////////////////////////////////

CHexaMainMeshRegion::CHexaMainMeshRegion(CModelBase& model, const double& dXValue, const double& dYValue)
: CHexaMeshRegionBase("Main", model), m_size_x(dXValue), m_size_y(dYValue), m_pBoundary(0) 
{
  // We expect to be the first meshregion ...
  assert(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  assert(Model().GraphEntry(MD_HEXA_MESH_ZONE)->GraphEntryNodes().size() == 0);

  m_pBoundary = dynamic_cast<CHexaBoundary*>(&model.Boundary());
  assert(m_pBoundary);
  LinkTo(*m_pBoundary);

  // Link to the entry
  reParent(Model().GraphEntry(MD_HEXA_MESH_ZONE));
}

CHexaMainMeshRegion::CHexaMainMeshRegion(CModelBase& model)
: CHexaMeshRegionBase(model), m_size_x(-1), m_size_y(-1), m_pBoundary(0)
{
  assert(Empty());
}

CHexaMainMeshRegion::CHexaMainMeshRegion(const CHexaMainMeshRegion& rhs)
: CHexaMeshRegionBase(rhs), m_size_x(rhs.m_size_x), m_size_y(rhs.m_size_y), m_pBoundary(rhs.m_pBoundary)
{
}

double CHexaMainMeshRegion::GridSizeX() const
{
  return m_size_x;
}


double CHexaMainMeshRegion::GridSizeY() const
{
  return m_size_y;
}

void CHexaMainMeshRegion::Invalidate()
{
  COperation oper(Model());
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  assert(pEntry);
    
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {
    if((*itMeshRegion) != this)
    {
      (*itMeshRegion)->Invalidate();
    }
  }
  
  CHexaMeshRegionBase::Invalidate();
}

unsigned int CHexaMainMeshRegion::IconId() const
{
  return IDI_MESH_MASTER;
}

int CHexaMainMeshRegion::NrOfElemX() const
{
  return int((m_pBoundary->Max().X() - m_pBoundary->Min().X()) / m_size_x);
}

int CHexaMainMeshRegion::NrOfElemY() const
{
  return int((m_pBoundary->Max().Y() - m_pBoundary->Min().Y()) / m_size_y);
}

CHexaMainMeshRegion& CHexaMainMeshRegion::operator=(const CHexaMainMeshRegion& rhs)
{
  CHexaMeshRegionBase::operator=(rhs);

  m_pBoundary = rhs.m_pBoundary;
  if((m_size_x != rhs.m_size_x) || (m_size_y != rhs.m_size_y))
  {
    m_size_x = rhs.m_size_x;
    m_size_y = rhs.m_size_y;

    // Invalidate our selves 
    Invalidate();
  
    // Correct boundary
//		CBoundaryBase::TMinMax minmax = m_pBoundary->SnapToGrid(CBoundaryBase::TMinMax(m_pBoundary->Min(), m_pBoundary->Max()));
//		m_pBoundary->Set(minmax.first, minmax.second);
  m_pBoundary->OnGridModified();

    // Invalidate the mesh ....
    CHexaModel* pModel = (CHexaModel*)(&Model());
    pModel->InvalidateMesh();
  }

  return *this;
}

bool CHexaMainMeshRegion::operator==(const CHexaMainMeshRegion& rhs) const
{
  if(!CHexaMeshRegionBase::operator ==(rhs))
    return false;

  if(m_size_x != rhs.m_size_x)
    return false;

  if(m_size_y != rhs.m_size_y)
    return false;

  return (m_pBoundary == rhs.m_pBoundary);

}

void CHexaMainMeshRegion::OnCreateMeshGrid()
{
  // We initialize the boundary and are invalid ...
  assert(IsInvalid());
  assert(m_pBoundary);

//	geo::CPoint ptMin = m_pBoundary->Min();
//	geo::CPoint ptMax = m_pBoundary->Max();

  for(int i = 0; i < 4; ++i)
  m_hull.PushBack(m_pBoundary->Box().Point(i));

  // Get Boundary points
//	m_hull.PushBack(geo::CPoint(ptMin.X(), ptMin.Y(), ptMin.Z()));
//	m_hull.PushBack(geo::CPoint(ptMax.X(), ptMin.Y(), ptMin.Z()));
//	m_hull.PushBack(geo::CPoint(ptMax.X(), ptMax.Y(), ptMin.Z()));
//	m_hull.PushBack(geo::CPoint(ptMin.X(), ptMax.Y(), ptMin.Z()));

  // We also do the displaylist here. Should be empty by the invalidate
  assert(m_displayList.Size() == 0);	
//	for(int i = 0; i < m_hull.NrOfLines(); i++)
//		m_displayList.PushBack(*new geo::CLine(m_hull.Line(i)));

  CHexaMeshRegionBase::OnCreateMeshGrid();
}

void CHexaMainMeshRegion::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  // Is the boundary changed? 
  const CBoundaryBase* pBoundary = dynamic_cast<const CBoundaryBase*>(&node);
  if(pBoundary && (!IsInvalid()))
  {
    // We should invalidate if the boundary changes
    if(( Hull().Min() != pBoundary->Min() ) ||
       ( Hull().Max() != pBoundary->Max() ) )
    {
      // Boundary changed, invalidate main region, mesh
      Invalidate();
    }
  }

  CHexaMeshRegionBase::OnNeighbourModified(node, uHint);
}

// Main mesh region cannot be destroyed...
bool CHexaMainMeshRegion::CanDestroy() const
{
  return false;
}

void CHexaMainMeshRegion::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
  // Save base first ...
  CHexaMeshRegionBase::SaveStream(stream,progress);

  // Inform user
  progress.StatusMessage("Saving MeshRegion Main");

  // Saving the sizes
  stream << m_size_x;
  stream << m_size_y;

  progress.Step();
}

void CHexaMainMeshRegion::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  // Invalidate first
  Invalidate();

  // Load base first ...
  if(version < CStreamVersion(3, 0, 24))
  {
    COpenGLNode::LoadStream(stream,version,progress);
    Color(qRgb(0,0xFF,0xFF));
  }
  else
    CMeshRegionBase::LoadStream(stream, version, progress);

  // Loading sizes
  stream >> m_size_x;
  stream >> m_size_y;

  // Link to boundary ...
  m_pBoundary = dynamic_cast<CHexaBoundary*>(&((CModelBase&)Model()).Boundary());
  LinkTo(*m_pBoundary);

  // Link to the entry
  reParent(Model().GraphEntry(MD_HEXA_MESH_ZONE));
}

long CHexaMainMeshRegion::SavedItems() const
{
  return 1 + CHexaMeshRegionBase::SavedItems();
}

void CHexaMainMeshRegion::SetMainGrid(const double& dXValue /* = 1 */, const double& dYValue /* = 1 */)
{
  if((m_size_x == dXValue) && (m_size_y == dYValue))
    return ;	// Nothing to do ...

  assert(dXValue > 0);
  assert(dYValue > 0);

  m_size_x = dXValue;
  m_size_y = dYValue;

  // if we have no copy, invalidate
  if(!IsCopy())
    Invalidate();
}

// static

CHexaMainMeshRegion* CHexaMainMeshRegion::getHexaMainMeshRegion(
  const CModelBase& modelBase)
{
  CHexaMainMeshRegion* hexaMainMeshRegion = 0;
  const CHexaMeshRegionEntry* hexaMeshRegionEntry =
  dynamic_cast <const CHexaMeshRegionEntry*> (
      modelBase.GraphEntry(MD_HEXA_MESH_ZONE));

  assert(hexaMeshRegionEntry != 0);

  const CHexaMeshRegionEntry::TNodeSet& entryNodes =
  hexaMeshRegionEntry->EntryNodes();

  for (CHexaMeshRegionEntry::TNodeSet::const_iterator entryNode =
  entryNodes.begin(); (entryNode != entryNodes.end()) &&
      (hexaMainMeshRegion == 0); ++entryNode)
  {
  hexaMainMeshRegion = dynamic_cast <CHexaMainMeshRegion*> (*entryNode);
  }

  return hexaMainMeshRegion;
}

////////////////////////////////////////////////////////////////////
// Implemention of sub meshregion 
//////////////////////////////////////////////////////////////////////
CHexaSubMeshRegion::CHexaSubMeshRegion(CModelBase& model, const geo::CPolygon& poly, int nExpX, int nExpY)
: CHexaMeshRegionBase("", model), m_exp_x(nExpX), m_exp_y(nExpY), m_polygon(poly)
{
  assert(m_polygon.NrOfPoints() > 2);
  // Append to mesh entry
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  assert(pEntry->EntryNodes().size() > 0);
  m_pMain = &pEntry->Main();

  // create name
  QString sName;
  sName = QString("Region%1").arg(pEntry->EntryNodes().size());
  Name(sName);

  // Link to the entry and invalidate the mesh
  LinkTo(*pEntry);

  //model.InvalidateMesh();
}

CHexaSubMeshRegion::CHexaSubMeshRegion(CModelBase& model)
: CHexaMeshRegionBase(model), m_exp_x(-1), m_exp_y(-1), m_pMain(0) 
{
  assert(Empty());
}

CHexaSubMeshRegion::~CHexaSubMeshRegion()
{
}

CHexaSubMeshRegion::CHexaSubMeshRegion(const CHexaSubMeshRegion& rhs)
: CHexaMeshRegionBase(rhs), m_exp_x(rhs.m_exp_x), m_exp_y(rhs.m_exp_y), m_pMain(rhs.m_pMain), m_formations(rhs.m_formations)
{
}

int CHexaSubMeshRegion::ExpX() const
{
  return m_exp_x;
}

int CHexaSubMeshRegion::ExpY() const
{
  return m_exp_y;
}

unsigned int CHexaSubMeshRegion::IconId() const
{
  return IDI_MESH_REGION;
}

double CHexaSubMeshRegion::GridSizeX() const
{
  return Main().GridSizeX() / pow(double(2),m_exp_x);
}

double CHexaSubMeshRegion::GridSizeY() const
{
  return Main().GridSizeY() / pow(double(2),m_exp_y);
}

CHexaSubMeshRegion& CHexaSubMeshRegion::operator=(const CHexaSubMeshRegion& rhs)
{
  CHexaMeshRegionBase::operator=(rhs);

  if((m_exp_x != rhs.m_exp_x) || (m_exp_y != rhs.m_exp_y) || m_formations != rhs.m_formations)
  {
    m_exp_x = rhs.m_exp_x;
    m_exp_y = rhs.m_exp_y;

  m_formations = rhs.m_formations;
  
    CHexaModel* pModel = (CHexaModel*)(&Model());
    pModel->InvalidateMesh();
  }

  return *this;
}

bool CHexaSubMeshRegion::operator==(const CHexaSubMeshRegion& rhs) const
{
  if(!CHexaMeshRegionBase::operator ==(rhs))
    return false;

  if (m_formations != rhs.m_formations)
  return false;

  if(m_exp_x != rhs.m_exp_x)
    return false;

  return (m_exp_y == rhs.m_exp_y);
}

const CHexaMainMeshRegion& CHexaSubMeshRegion::Main() const
{
  return *m_pMain;
}

CHexaMainMeshRegion& CHexaSubMeshRegion::Main()
{
  return *m_pMain;
}

void CHexaSubMeshRegion::ClearFormations()
{
  m_formations.clear();
}

void CHexaSubMeshRegion::Formation(CHexaFormation& formation, int elements)
{
  m_formations.insert(std::make_pair(&formation, TFormationInfo(m_exp_x, m_exp_y, elements)));
}

int CHexaSubMeshRegion::Formation(CHexaFormation& formation)
{
  TFormationInfoMap::iterator it = m_formations.find(&formation);
  if (it != m_formations.end())
  return it->second.gridZ;
  return -1;
}

bool CHexaSubMeshRegion::FullModel() const
{
  for (TFormationInfoMap::const_iterator it = m_formations.begin(); it != m_formations.end(); ++it)
  if (it->second.gridZ != it->first->Elements())
      return false;
  return true;
}

void CHexaSubMeshRegion::Invalidate()
{
  m_polygonLines.ClearAndDelete();
  CHexaMeshRegionBase::Invalidate();
}

void CHexaSubMeshRegion::OnCreateMeshGrid()
{
  // We assert that there are no points, because we're the routine creating points
  assert(m_vcGridPoints.Size() == 0);

  // We want to start on the right level ...
  CBoundaryBase* pBoundary = &((CModelBase*)(&Model()))->Boundary();
  for (int m = 0; m < m_polygon.NrOfPoints(); m++)
    m_polygon.Point(m, geo::CPoint(m_polygon.Point(m).X(), m_polygon.Point(m).Y(), pBoundary->Min().Z()));

  // We also do the displaylist here. Should be empty by the invalidate
  assert(m_polygonLines.Size() == 0);
  assert(m_displayList.Size() == 0);	
  
  for(int i = 0; i < m_polygon.NrOfLines(); i++)
    m_polygonLines.PushBack(*new geo::CLine(m_polygon.Line(i)));
  m_displayList.PushBack(m_polygonLines);
  
  RecalcMeshRegion();

  CHexaMeshRegionBase::OnCreateMeshGrid();
}

void CHexaSubMeshRegion::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
  // Inform user ...
  progress.StatusMessage("Saving MeshRegion ");

  // Save base first ...
  CHexaMeshRegionBase::SaveStream(stream,progress);

  // Save exponents and polygon ...
  stream << m_exp_x;
  stream << m_exp_y;

  // Save the polygon 
  stream << Polygon().NrOfPoints();
  for (int i = 0; i < Polygon().NrOfPoints(); i++)
  {
    stream << Polygon().Point(i).X() << Polygon().Point(i).Y() << Polygon().Point(i).Z();
    progress.Step();
  }

  stream << (int)m_formations.size();
  for (TFormationInfoMap::iterator it = m_formations.begin(); it != m_formations.end(); ++it)
  {
  stream << it->first->Index();
  stream << it->second.gridX;
  stream << it->second.gridY;
  stream << it->second.gridZ;
  }
}

void CHexaSubMeshRegion::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  // Invalidate first
  Invalidate();
  assert(IsInvalid());
  m_polygon.Clear();

  // Load base first ...
  if(version < CStreamVersion(3, 0, 24))
  {
    COpenGLNode::LoadStream(stream,version,progress);
    Color(qRgb(0,0xFF,0xFF));
  }
  else
    CMeshRegionBase::LoadStream(stream, version, progress);

  // Load exponents ...
  stream >> m_exp_x;
  stream >> m_exp_y;

  // Load polygon ...
  int nSize;
  stream >> nSize;

  double X,Y,Z;
  for (int i = 0; i < nSize; i++)
  {
    stream >> X >> Y >> Z;
    m_polygon.PushBack(geo::CPoint(X,Y,Z));
    progress.Step();
  }

  if (version > CStreamVersion(4, 1, 59))
  {
  TFormationBaseEntry *entry = static_cast<TFormationBaseEntry *>(Model().GraphEntry(MD_BASE_FORMATION));

  stream >> nSize;
  for (int i = 0; i < nSize; ++i)
  {
      int formation, gridX, gridY, gridZ;
      stream >> formation;
      stream >> gridX;
      stream >> gridY;
      stream >> gridZ;
      CHexaFormation *hexaFormation = dynamic_cast<CHexaFormation *>(entry->FindIndex(formation));
      if (hexaFormation)
    m_formations.insert(std::make_pair(hexaFormation, TFormationInfo(gridX, gridY, gridZ)));
  }
  }

  // Link to the entry
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  m_pMain = &pEntry->Main();
  LinkTo(*pEntry);
}

long CHexaSubMeshRegion::SavedItems() const
{
  return m_polygon.NrOfPoints() + CHexaMeshRegionBase::SavedItems();
}

// Load and save
  


/*
//=============================================================================
// A zone must consists of a minimum of 4 points.
void CHexaMeshRegion::AddZone(geo::CArray<geo::CPoint> points)
{
  assert(points.Size() >= 4);

  m_vcPolyPoint.Clear();
  for(int i = 0; i < points.Size(); i++)
    m_vcPoint.PushBack(points.Object(i));
}

bool CHexaMainMeshRegion::OnMainGrid(const geo::IPoint& point) const
{
  int leftIndex = int((point.X() - m_pBoundary->Min().X()) / m_size_x);

  if(!(leftIndex * m_size_x == point.X()))
    return false;

  int topIndex = int((point.Y() - m_pBoundary->Min().Y()) / m_size_y);

  if(!(topIndex * m_size_y == point.Y()))
    return false;

  return true;
}
*/
void CHexaMainMeshRegion::GetElemPoints(int IndexX, int IndexY, std::vector<geo::CPoint>& vec) const
{
  double x1 = m_pBoundary->Min().X() + IndexX * m_size_x;
  double x2 = m_pBoundary->Min().X() + (IndexX + 1) * m_size_x;

  double y1 = m_pBoundary->Min().Y() + IndexY * m_size_y;
  double y2 = m_pBoundary->Min().Y() + (IndexY+1) * m_size_y;
  
  vec[0]=geo::CPoint(x1,y1);
  vec[1]=geo::CPoint(x2,y1);
  vec[2]=geo::CPoint(x1,y2);
  vec[3]=geo::CPoint(x2,y2);

}

void CHexaMainMeshRegion::GetElemPoints(const geo::IPoint& point, std::vector<geo::CPoint>& vec) const
{
  int IndexX = int((point.X() - m_pBoundary->Min().X()) / m_size_x);
  int IndexY = int((point.Y() - m_pBoundary->Min().Y()) / m_size_y);
  
  GetElemPoints(IndexX,IndexY,vec);
}

CHexaBoundary* CHexaMainMeshRegion::getBoundary() const
{
  return m_pBoundary;
}

void CHexaSubMeshRegion::SetSubGrid(int ExpX, int ExpY)
{
  m_exp_x = ExpX;
  m_exp_y = ExpY;

  if(!IsCopy())
    Invalidate();
}

// Calculate the largest bounding rectangle of this polygon.
geo::CBox GetBoundingMeshBox(geo::CArray<geo::CPoint> polygon)
{
  double minx,miny,maxx,maxy;
  minx = miny = DBL_MAX;
  maxx = maxy = -DBL_MAX;

  for(size_t i = 0; i < polygon.Size(); i++)
  {
    double x = polygon.Object(i).X();
    if (polygon.Object(i).X() < minx) minx = x;
    else if (polygon.Object(i).X() > maxx) maxx = x;
//		if (polygon.Object(i).X() < minx) minx = polygon.Object(i).X();
//		else if (polygon.Object(i).X() > maxx) maxx = polygon.Object(i).X();
    if (polygon.Object(i).Y() < miny) miny = polygon.Object(i).Y();
    else if (polygon.Object(i).Y() > maxy) maxy = polygon.Object(i).Y();
  }

  return geo::CBox(geo::CPoint(minx,miny),geo::CPoint(maxx,maxy));
}

const geo::CArray<geo::CPoint> &CHexaMeshRegionBase::GetMeshRegion()
{
  return MeshGrid();
}

// Check if a point is dangling (at the grid of the given meshzone but not at the mastergrid) and 
// if this point is not inside the given meshzone.
int CHexaMeshRegionBase::IsDangling(const CHexaMeshRegionBase& meshzone, const geo::IPoint& pt, bool check_region) const
{
  //if(this != &MainRegion)
  int ret = MESH_NO_DANG;

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  if(InsideRegion(pt, false))
  {
    geo::CPoint ptMin = rs.LocalMin(Hull());
  geo::CPoint point = rs.ToLocal(pt);
    bool bHorDang = fabs(fmod(point.X() - ptMin.X() + (EPS / 2), GridSizeX())) > EPS; 
    bool bVertDang = fabs(fmod(point.Y() - ptMin.Y() + (EPS / 2), GridSizeY())) > EPS;

    if(bHorDang && bVertDang)
    {
      ret = MESH_NOT_ON_GRID;
    }
    else if (bHorDang || bVertDang)
    {
      // Check if point is inside given meshzone.
      
      if (!check_region || !meshzone.InsideRegion(pt, false))
      {
        ret = (bHorDang) ? MESH_HOR_DANG : MESH_VERT_DANG;
      }
    }
  }

  return ret;
}

const geo::CPolygon& CHexaSubMeshRegion::Polygon() const
{
  return m_polygon;
}

void CHexaSubMeshRegion::Polygon(const geo::CPolygon &polygon)
{
  // Is it the same poly?
//	if(polygon == Polygon())
//		return;

  // The defining polygon needs to be on the same plane as the boundary, so polygon is transferred to boundary Min().Z()
  CBoundaryBase* pBoundary = &((CModelBase*)(&Model()))->Boundary();
  m_polygon.Clear();
  for (int m = 0; m < polygon.NrOfPoints(); m++)
    m_polygon.PushBack(geo::CPoint(polygon.Point(m).X(), polygon.Point(m).Y(), pBoundary->Min().Z()));

  // Define mesh and invalidate the mesh
  Invalidate();

  Modified();
}

bool CHexaSubMeshRegion::IsValidNextPoint(const geo::IPoint& point)
{
  if(m_polygon.Contains/*InXYPlane*/(point, true))
  {
    // good enough if it's the first point
    if(m_hull.Empty())
      return true;

    // it's a candidate, see if the edge doesn't intersect any of the polygon's edges
    const geo::IPoint& prev_point = m_hull.Point(m_hull.NrOfPoints() - 1);
    geo::CLine l(prev_point, point);

    int i;
    for(i = 0; i < m_polygon.NrOfLines(); ++i)
    {
      geo::CPoint ptIntersect = l.Intersection(m_polygon.Line(i));
      if(!ptIntersect.Empty() && l.Contains(ptIntersect) && m_polygon.Line(i).Contains(ptIntersect))
      {
        // not allowed
        return false;
      }
    }

    // no intersections, point is ok
    return true;
  }

  return false;
}

void CHexaSubMeshRegion::RecalcMeshRegion()
{
  assert(IsInvalid());
  CHexaMainMeshRegion *pMain = &Main();
  assert(pMain);
  
  geo::CArray<geo::CPoint> vcGridPoints = Main().MeshGrid();
  assert(vcGridPoints.Size() > 0);

  int direction = 1;
  int firstIndex= -1;
  int index = -1;
  int x = 0;
  int y = 0;
  int dXGrid = Main().NrOfElemX();
  int dYGrid = Main().NrOfElemY();

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  int t;
  for(t = 1; t < vcGridPoints.Size(); t++)
  {
    if((rs.ToLocal(vcGridPoints.Object(t)).X() - rs.ToLocal(vcGridPoints.Object(t-1)).X()) < -EPS / 2)
    {
      dXGrid = t;
      dYGrid = vcGridPoints.Size() / dXGrid;
      break;
    }
  }

  int iNotFoundCount = 0;
  while (true)
  {
    if ((iNotFoundCount > 4) && (firstIndex != -1))
      break; // The algorithm searches in four directions for the next point. To prevent an infinite loop we count
    // te misses and after four tries we break out.
    index = y * dXGrid + x;
    if (index >= vcGridPoints.Size())
      break;
    bool bFound=true;

    if (index >= 0 && IsValidNextPoint(vcGridPoints.Object(index)))
    {
      bFound = true;
      iNotFoundCount = 0;
      if (firstIndex != -1 && (firstIndex == index))
        break;
      if (firstIndex == -1)
        firstIndex = index;
      m_hull.PushBack(vcGridPoints.Object(index));
        // We have found another
    }
    else 
    {
      iNotFoundCount++;
      if (firstIndex != -1)
      {
        bFound = false;
        // Not found so change direction
        // Locate another
      }
      else
      {
        x++;
        if (x == dXGrid)
        {
          y++;
          x=0;
        }
        continue;
      }
    }
    if (bFound)
    {
      if (direction == 1)
        direction = -dYGrid;
      else if (direction == dYGrid)
        direction = 1;
      else if (direction == -dYGrid)
        direction = -1;
      else if (direction == -1)
        direction = dYGrid;
    }
    else
    {
      if (direction == 1)
      {
        x--;
        direction = dYGrid;
      }
      else if (direction == dYGrid)
      {
        y--;
        direction = -1;
      }
      else if (direction == -1)
      {
        x++;
        direction = -dYGrid;
      }
      else if (direction == -dYGrid)
      {
        y++;
        direction = 1;
      }
    }
    bool bNewDirectionFound = false;
    while (!bNewDirectionFound)
    {
      if (direction == 1)
      {
        x++;
        if (x == dXGrid)
        {
          x--;
          direction = dYGrid;
          y++;
        }
      }
      else if (direction == dYGrid)
      {
        y++;
        if (y == dYGrid)
        {
          y--;
          direction = -1;
          x--;
        }
      }
      else if (direction == -1)
      {
        x--;
        if (x< 0)
        {
          x++;
          direction = -dYGrid;
          y--;
        }
      }
      else if (direction == -dYGrid)
      {
        y--;
        if (y<0)
        {
          y++;
          direction = 1;
          x++;
        }
      }
      if (!(x < 0 || x >= dXGrid || y < 0 || y >= dYGrid))
      {
        bNewDirectionFound = true;
      }
      else
      {
        if (x<0)
          x=0;
        if (x>=dXGrid)
          x = dXGrid-1;
        if (y<0)
          y=0;
        if (y>=dYGrid)
          y = dYGrid-1;
      }
    }
  }

  bool bRemovedLines;

  do
  {
    bRemovedLines = false;
    if (m_hull.NrOfPoints()>2)
    {
      // Now check for lines where the next-next point is the same as the current point. If so the next point and the next-next point can be deleted
      int i = 0;
      int j = 1;
      int k = 2;

      while(i < m_hull.NrOfPoints())
      {
        if(m_hull.Point(i) == m_hull.Point(k))
        {
          int imax = (j > k) ? j : k;
          int imin = (j < k) ? j : k;
          m_hull.Remove(imax);
          m_hull.Remove(imin);
          bRemovedLines = true;
          break;
        }
        i++;
        j++; if(j == m_hull.NrOfPoints()) j = 0;
        k++; if(k == m_hull.NrOfPoints()) k = 0;
      }
    }
  } while(bRemovedLines);
}

CHexaMeshRegionEntry::CHexaMeshRegionEntry(C3DModel& model)
: CGraphEntryTemp<CHexaMeshRegionBase>(MD_HEXA_MESH_ZONE, IDI_MESH, IDS_TREE_MESH, model)
{
}

const CHexaMainMeshRegion& CHexaMeshRegionEntry::Main() const
{
  CHexaMainMeshRegion* hexaMainMeshRegion =
  CHexaMainMeshRegion::getHexaMainMeshRegion(
      dynamic_cast <const CModelBase&> (Model()));

  assert(hexaMainMeshRegion != 0);

  return *hexaMainMeshRegion;
}

CHexaMainMeshRegion& CHexaMeshRegionEntry::Main()
{
  CHexaMainMeshRegion* hexaMainMeshRegion =
  CHexaMainMeshRegion::getHexaMainMeshRegion(
  dynamic_cast <const CModelBase&> (Model()));
  
  assert(hexaMainMeshRegion != 0);

  return *hexaMainMeshRegion;
}
