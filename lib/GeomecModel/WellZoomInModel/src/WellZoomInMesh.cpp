
#include <QElapsedTimer>

#include "WellZoomInMesh.h"

#include "WellZoomInModel.h"
#include "WellPath.h"
#include "NewWellPath.h"
#include "InterfaceElement.h"
#include "FormationVolume.h"
#include "WellFormation.h"
#include "WellFormationVolume.h"
#include "ISupport.h"
#include "BodyGroup.h"
#include "Hexahedron.h"
#include "Tetrahedron.h"
#include "IProgressFactory.h"
#include "IProgressBase.h"
#include "unitnode.h"
#include "ModifiedHint.h"

#ifdef _DEBUG
const int nDefaultRadialElements = 2;
const int nDefaultElementsPerSection = 2;
#else
const int nDefaultRadialElements = 4;
const int nDefaultElementsPerSection = 4;
#endif

// factor between length of center line and smallest side
const double CURVATURE_FACTOR = 20;

CWellZoomInMesh::CWellZoomInMesh(const QString& sName, CWellZoomInModel& zoominmodel)
: IWellMesh(sName, zoominmodel),
  m_zoominmodel(zoominmodel),
  m_nRadialElements(nDefaultRadialElements),
  m_dLongElementSize(zoominmodel.Radius()),
  m_isHexaMesh(true),
  m_tetraRadius(zoominmodel.Radius()),
  m_tetraIdealElementSize(10)
{
  LinkTo(zoominmodel);
}

CWellZoomInMesh::CWellZoomInMesh(const CWellZoomInMesh& rhs)
: IWellMesh(rhs),
  m_zoominmodel(rhs.m_zoominmodel),
  m_nRadialElements(rhs.m_nRadialElements),
  m_dLongElementSize(rhs.m_dLongElementSize),
  m_isHexaMesh(rhs.m_isHexaMesh),
  m_tetraRadius(rhs.m_tetraRadius),
  m_tetraIdealElementSize(rhs.m_tetraIdealElementSize)
{
}

CWellZoomInMesh::~CWellZoomInMesh()
{
  ClearDefinitionPointList();
}

CWellZoomInMesh& CWellZoomInMesh::operator=(const CWellZoomInMesh& rhs)
{
  IWellMesh::operator=(rhs);

  assert(&m_zoominmodel == &rhs.m_zoominmodel);
  m_nRadialElements       = rhs.m_nRadialElements;
  m_dLongElementSize      = rhs.m_dLongElementSize;
  m_isHexaMesh            = rhs.m_isHexaMesh;
  m_tetraRadius           = rhs.m_tetraRadius;
  m_tetraIdealElementSize = rhs.m_tetraIdealElementSize;

  m_bInvalidateAfterDlg = true;

  return *this;
}

bool CWellZoomInMesh::operator==(const CWellZoomInMesh& rhs) const
{
  return (
    &m_zoominmodel          == &rhs.m_zoominmodel         &&
    m_nRadialElements       == rhs.m_nRadialElements      &&
    m_dLongElementSize      == rhs.m_dLongElementSize     &&
    m_isHexaMesh            == rhs.m_isHexaMesh           &&
    m_tetraRadius           == rhs.m_tetraRadius          &&
    m_tetraIdealElementSize == rhs.m_tetraIdealElementSize
  );
}

bool CWellZoomInMesh::CanCreateMesh() const
{
  return !IsMesh();
}

bool CWellZoomInMesh::IsMesh() const
{
  return !m_bMeshing && Mesh().ElementSize() > 0;
}

namespace
{

const QString CREATE_WELL_ZOOM_IN_MESH = QObject::tr("Creating zoom-in mesh");

} // anonymous namespace

void CWellZoomInMesh::CreateMesh()
{
  ClearDefinitionPointList();

  if (m_isHexaMesh)
  {
    CreateDefinitionPointList(m_dLongElementSize);
    
    // check for curvature in definition points
    if (!CheckCurvature())
      return;

    wellCreateMesh(CREATE_WELL_ZOOM_IN_MESH);
  }
  else
  {
    CWellZoomInModel& model = ZoomInModel();

    CNewWellPath *newWellPath = model.NewWellPath();
    if (!newWellPath)
      return;

    CreateDefinitionPointList(10);

    int pointSize = newWellPath->PolyLine().PointSize();

    std::vector<geo::CPoint> points;
    points.reserve(pointSize);

    for (int i = 0; i < pointSize; ++i)
      points.push_back(newWellPath->PolyLine().Point(i));

    CreateTetraMesh(model, points, TetraRadius(), TetraIdealElementSize());
  }
}

void CWellZoomInMesh::InvalidateMesh()
{
  Lock();

  COperation oper(Model());

  IWellMesh::InvalidateMesh();

  m_mpBody2MidPoint.clear();
  m_mpNode2MidPoint.clear();
  m_mpMesh2Boundary.clear();
  m_mpBoundary2Mesh.clear();
/*
  m_vcDbg.clear();
*/
  Modified(MeshCleared);

  Unlock();
}

bool CWellZoomInMesh::Empty() const
{
  return false;
}

int CWellZoomInMesh::DisplayListSize() const
{
/*
  if(m_vcDbg.empty())
  {
    int iXIndex, iYIndex;
    for(iXIndex = -m_nRadialElements; iXIndex <= m_nRadialElements; ++iXIndex)
    {
      for(iYIndex = -m_nRadialElements; iYIndex <= m_nRadialElements; ++iYIndex)
      {
        m_vcDbg.push_back(Grid2DPointAt(iXIndex, iYIndex));
      }
    }
  }

  assert(m_vcDbg.size() == (2 * m_nRadialElements + 1) * (2 * m_nRadialElements + 1));
  return (2 * m_nRadialElements + 1) * (2 * m_nRadialElements + 1);
*/
/*
  // code to display the 2D grid for debugging
  int i;
  int sz = 0;
  for(size_t i = 0; i < m_vc2DGrid.size(); ++i)
  {
    sz += m_vc2DGrid[i].size();
  }
  return sz;
*/
  return 1;
}

const geo::IObject& CWellZoomInMesh::DisplayList(int /*nIndex*/) const
{
/*
  return m_vcDbg[nIndex];
*/
/*
  // code to display the 2D grid for debugging
  int i;
  for(size_t i = 0; i < m_vc2DGrid.size(); ++i)
  {
    int j;
    for(size_t j = 0; j < m_vc2DGrid[i].size(); ++j)
    {
      if(nIndex-- == 0)
        return *m_vc2DGrid[i][j];
    }
  }
  assert(FALSE);
*/
  return Mesh();
}

CWellZoomInMesh::TColor CWellZoomInMesh::Color() const
{
  if ( ZoomInModel().WellPath() )
    return ZoomInModel().WellPath()->Color();
  else
    return ZoomInModel().NewWellPath()->Color();
}

const CWellZoomInModel& CWellZoomInMesh::ZoomInModel() const
{
  return m_zoominmodel;
}

CWellZoomInModel& CWellZoomInMesh::ZoomInModel()
{
  return m_zoominmodel;
}

int CWellZoomInMesh::NrOfRadialElements() const
{
  return m_nRadialElements;
}

void CWellZoomInMesh::NrOfRadialElements(int nRadialElements)
{
  m_nRadialElements = nRadialElements;
  InvalidateMesh();
}

double CWellZoomInMesh::TargetLongitudinalElementSize() const
{
  return m_dLongElementSize;
}

void CWellZoomInMesh::TargetLongitudinalElementSize(double dSize)
{
  if(fabs(dSize - m_dLongElementSize) > 1e-8)
  {
    m_dLongElementSize = dSize;
    InvalidateMesh();
  }
}

void CWellZoomInMesh::setHexaMesh(bool hexa)
{
  if (hexa != m_isHexaMesh)
  {
    m_isHexaMesh = hexa;
    if (hexa)
      SwitchToHexaMesh();
    else
      SwitchToTetraMesh();
  }
}

bool CWellZoomInMesh::isHexaMesh() const
{
  return m_isHexaMesh;
}

bool CWellZoomInMesh::isTetraMesh() const
{
  return !m_isHexaMesh;
}

double CWellZoomInMesh::TetraRadius() const
{
  return m_tetraRadius;
}

void CWellZoomInMesh::TetraRadius(double radius)
{
  if (fabs(radius - m_tetraRadius) > 1e-8)
  {
    m_tetraRadius = radius;
    InvalidateMesh();
  }
}

double CWellZoomInMesh::TetraIdealElementSize() const
{
  return m_tetraIdealElementSize;
}

void CWellZoomInMesh::TetraIdealElementSize(double size)
{
  if (fabs(size - m_tetraIdealElementSize) > 1e-8)
  {
    m_tetraIdealElementSize = size;
    InvalidateMesh();
  }
}

int CWellZoomInMesh::MeshNodeToBoundaryNode(int nGridNodeIndex) const
{
  TBoundaryNodeMap::const_iterator it = m_mpMesh2Boundary.find(nGridNodeIndex);
  if(it == m_mpMesh2Boundary.end())
    return -1;

  return it->second;
}

int CWellZoomInMesh::BoundaryNodeToMeshNode(int nBoundaryNodeIndex) const
{
  TBoundaryNodeMap::const_iterator it = m_mpBoundary2Mesh.find(nBoundaryNodeIndex);
  if(it == m_mpBoundary2Mesh.end())
    return -1;

  return it->second;
}

void CWellZoomInMesh::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  IWellMesh::LoadStream(stream, version, progress);
  progress.StatusMessage("Loading well zoomin mesh");

  int nIsMesh;
  stream >> nIsMesh;

  if (version >= CStreamVersion(4, 1, 77))
  {
    int nIsHexaMesh;
    stream >> nIsHexaMesh;
    m_isHexaMesh = nIsHexaMesh == 1;
    if (isTetraMesh())
      SwitchToTetraMesh();
  }

  if(nIsMesh != 0)
  {
    int nSize;
    CWellZoomInModel& model = ZoomInModel();
    CWellZoomInModel::CBoundary& boundary = static_cast<CWellZoomInModel::CBoundary&>(model.Boundary());

    int i;

    // load the geometry mesh

    // load the nodes
    int nNodes;
    stream >> nNodes;
    for(i = 0; i < nNodes; ++i)
    {
      double x, y, z;
      stream >> x >> y >> z;
      Mesh().RegisterNode(geo::CPoint(x, y, z), false);
      progress.Step();
    }

    // load the hexa elements
    int nIfEltSize = 0;
    if (version >= CStreamVersion(4, 1, 77) && isTetraMesh())
    {
      nIfEltSize = 6;
      int nTetra;
      stream >> nTetra;
      for (i = 0; i < nTetra; ++i)
      {
        std::vector<int> vcNode(4);
        int j;
        for (j = 0; j < 4; ++j)
          stream >> vcNode[j];

        new geo::CTetrahedron(Mesh(), vcNode);
        progress.Step();
      }
    }
    else
    {
      nIfEltSize = 8;
      int nHexa;
      stream >> nHexa;
      for (i = 0; i < nHexa; ++i)
      {
        std::vector<int> vcNode(8);
        int j;
        for (j = 0; j < 8; ++j)
          stream >> vcNode[j];

        new geo::CHexahedron(Mesh(), vcNode);
        progress.Step();
      }
    }

    // load the interface elements
    int nIface;
    stream >> nIface;
    for(i = 0; i < nIface; ++i)
    {
      std::vector<int> vcNode(nIfEltSize);
      int j;
      for(j = 0; j < nIfEltSize; ++j)
        stream >> vcNode[j];

      int nParentIndex;
      stream >> nParentIndex;

      int nFaceIndex;
      stream >> nFaceIndex;

      int nSurfDef;
      stream >> nSurfDef;

      assert(dynamic_cast<const geo::IBody*>(&Mesh().Element(nParentIndex)));
      const geo::IBody& parent = static_cast<const geo::IBody&>(Mesh().Element(nParentIndex));

      boundary.AddBoundaryElement(*new geo::CInterfaceElement(Mesh(), &parent.Face(nFaceIndex), &parent.Face(nFaceIndex), vcNode),
        (CBoundaryInterfaceDef::TBoundarySurface)nSurfDef);
      progress.Step();
    }

    // load supports
    int nSupports;
    stream >> nSupports;
    for(i = 0; i < nSupports; ++i)
    {
      int idx;
      int nDir;
      stream >> idx;
      stream >> nDir;
      int j;
      for(j = 0; j < nDir; ++j)
      {
        double x, y, z;
        stream >> x >> y >> z;
        Mesh().CreateTranslationSupport(idx, geo::CVector(x, y, z));
      }
      progress.Step();
    }

    LoadFormationVolumes(stream, version, progress);

    stream >> nSize;
    for(i = 0; i < nSize; ++i)
    {
      int n1, n2;
      stream >> n1 >> n2;
      m_mpMesh2Boundary.insert(TBoundaryNodeMap::value_type(n1, n2));
      progress.Step();
    }

    stream >> nSize;
    for(i = 0; i < nSize; ++i)
    {
      int n1, n2;
      stream >> n1 >> n2;
      m_mpBoundary2Mesh.insert(TBoundaryNodeMap::value_type(n1, n2));
      progress.Step();
    }

    stream >> nSize;
    for(i = 0; i < nSize; ++i)
    {
      int nIndex;
      double x, y, z;
      stream >> nIndex; // body index
      stream >> x >> y >> z; // point coordinates
      assert(dynamic_cast<const geo::CHexahedron*>(&Mesh().Element(nIndex)));
      m_mpBody2MidPoint.insert(TBody2MidPointMap::value_type(static_cast<const geo::IBody*>(&Mesh().Element(nIndex)), geo::CPoint(x, y, z)));
      progress.Step();
    }

    stream >> nSize;
    for(i = 0; i < nSize; ++i)
    {
      int nIndex;
      double x, y, z;
      stream >> nIndex; // node index
      stream >> x >> y >> z;
      m_mpNode2MidPoint.insert(TNode2MidPointMap::value_type(&Mesh().Node(nIndex), geo::CPoint(x, y, z)));
      progress.Step();
    }
  }

  stream >> m_nRadialElements;

  if(version < CStreamVersion(3, 0, 112))
  {
    int nDummy;
    stream >> nDummy;
  }
  else
    stream >> m_dLongElementSize;

  if (version >= CStreamVersion(4, 1, 77))
  {
    stream >> m_tetraRadius;
    stream >> m_tetraIdealElementSize;
  }

  progress.Step();

  if(IsMesh())
    Modified(MeshCreated);
}

void CWellZoomInMesh::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IWellMesh::SaveStream(stream, progress);
  progress.StatusMessage("Saving well zoomin mesh");

  CWellZoomInModel& model = ZoomInModel();
  CWellZoomInModel::CBoundary& boundary = static_cast<CWellZoomInModel::CBoundary&>(model.Boundary());

  stream << int(IsMesh() ? 1 : 0);

  stream << int(isHexaMesh() ? 1 : 0);

  if(IsMesh())
  {
    // save the geometry mesh
    int nNodes = Mesh().NodeSize();
    stream << nNodes;
    int i;
    for(i = 0; i < nNodes; ++i)
    {
      const geo::INode& node = Mesh().Node(i);
      stream << node.X() << node.Y() << node.Z();
      progress.Step();
    }

    int nElements = Mesh().ElementSize();
    int nIface = boundary.BoundaryElementSize();
    int nHexaTetra = nElements - nIface;

    // save hexa elements
    stream << nHexaTetra;
    for(i = 0; i < nHexaTetra; ++i)
    {
      const geo::IElement& el = Mesh().Element(i);
      assert((isHexaMesh() && dynamic_cast<const geo::CHexahedron*>(&el)) || (isTetraMesh() && dynamic_cast<const geo::CTetrahedron *>(&el)));

      int j;
      for(j = 0; j < el.NrOfPoints(); ++j)
        stream << el.PointIndex(j);

      progress.Step();
    }

    // save interface elements
    stream << nIface;
    for(i = nHexaTetra; i < nElements; ++i)
    {
      const geo::IElement& el = Mesh().Element(i);
      assert(dynamic_cast<const geo::CInterfaceElement*>(&el));
      const geo::CInterfaceElement* pIface = static_cast<const geo::CInterfaceElement*>(&el);

      int j;
      for(j = 0; j < el.NrOfPoints(); ++j)
        stream << el.PointIndex(j);

      const geo::IFace& face = BodyFace(*pIface);
      stream << face.Parent()->Index();
      stream << face.Index();

      CBoundaryInterfaceDef::TBoundarySurface surfdef = boundary.BoundarySurfaceDef(*pIface);
      stream << int(surfdef);

      progress.Step();
    }

    // save supports
    int nSupports = Mesh().SupportSize();
    stream << nSupports;
    for(i = 0; i < nSupports; ++i)
    {
      const geo::ISupport& support = Mesh().Support(i);
      assert(dynamic_cast<const geo::CTranslationSupport*>(&support));
      stream << support.NodeIndex();
      int nDirections = support.DirectionSize();
      stream << nDirections;
      int j;
      for(j = 0; j < nDirections; ++j)
      {
        const geo::IVector& vecDir = support.Direction(j);
        stream << vecDir.X() << vecDir.Y() << vecDir.Z();
      }

      progress.Step();
    }

    SaveFormationVolumes(stream, progress);

    TBoundaryNodeMap::iterator it;

    stream << int(m_mpMesh2Boundary.size());
    for(it = m_mpMesh2Boundary.begin(); it != m_mpMesh2Boundary.end(); ++it)
    {
      stream << it->first << it->second;
      progress.Step();
    }

    stream << int(m_mpBoundary2Mesh.size());
    for(it = m_mpBoundary2Mesh.begin(); it != m_mpBoundary2Mesh.end(); ++it)
    {
      stream << it->first << it->second;
      progress.Step();
    }

    stream << int(m_mpBody2MidPoint.size());
    TBody2MidPointMap::iterator itbp;
    for(itbp = m_mpBody2MidPoint.begin(); itbp != m_mpBody2MidPoint.end(); ++itbp)
    {
      stream << itbp->first->Index(); // body's index
      stream << itbp->second.X() << itbp->second.Y() << itbp->second.Z();
      progress.Step();
    }

    stream << int(m_mpNode2MidPoint.size());
    TNode2MidPointMap::iterator itnp;
    for(itnp = m_mpNode2MidPoint.begin(); itnp != m_mpNode2MidPoint.end(); ++itnp)
    {
      stream << itnp->first->Index(); // node index
      stream << itnp->second.X() << itnp->second.Y() << itnp->second.Z();
      progress.Step();
    }
  }

  stream << m_nRadialElements;
  stream << m_dLongElementSize;

  stream << m_tetraRadius;
  stream << m_tetraIdealElementSize;

  progress.Step();
}

long CWellZoomInMesh::SavedItems() const
{
  long lRet = 1 + IWellMesh::SavedItems();
  if(IsMesh())
  {
    lRet += Mesh().NodeSize();
    lRet += Mesh().ElementSize();
    lRet += Mesh().SupportSize();
    lRet += m_mpMesh2Boundary.size();
    lRet += m_mpBoundary2Mesh.size();
    lRet += m_mpBody2MidPoint.size();
    lRet += m_mpNode2MidPoint.size();
  }

  return lRet;
}

bool CWellZoomInMesh::Create2DGrid(IProgressBase& prog)
{
  assert(m_nRadialElements > 0);

  prog.NextJob("Creating 2D grid"); // TEST HIT
  int nProgSteps = (m_nRadialElements + 1) * (m_nRadialElements + 1);
  prog.AddSteps(nProgSteps);

  // single quadrant

  // x and y coordinate of the fourth node (on the circle)
  double dCircle = 0.70710678118654752440084436210485 * ZoomInModel().Radius();

  Clear2DGrid();
  m_vc2DGrid.resize(m_nRadialElements + 1);

  int iCol;
  int iRow;
  for(iRow = 0; iRow < m_nRadialElements + 1; ++iRow)
  {
    m_vc2DGrid[iRow].resize(m_nRadialElements + 1);
    for(iCol = 0; iCol < m_nRadialElements + 1; ++iCol)
    {
      double dXFraction = double(iCol) / double(m_nRadialElements);
      double dYFraction = double(iRow) / double(m_nRadialElements);
      double dX = (1.0 - dYFraction) * dXFraction * ZoomInModel().Radius() + dYFraction * dXFraction * dCircle;
      double dY = (1.0 - dXFraction) * dYFraction * ZoomInModel().Radius() + dXFraction * dYFraction * dCircle;
      m_vc2DGrid[iRow][iCol] = new geo::CPoint(dX, dY);

      try
      {
        prog.Step();
      }

      catch (CProgressCancel* c)
      {
        delete c;
        return false;
      }
    }
  }

  return true;
}

void CWellZoomInMesh::Clear2DGrid()
{
  for(size_t iRow = 0; iRow < m_vc2DGrid.size(); ++iRow)
  {
    for(size_t iCol = 0; iCol < m_vc2DGrid[iRow].size(); ++iCol)
    {
      delete m_vc2DGrid[iRow][iCol];
    }
    m_vc2DGrid[iRow].clear();
  }

  m_vc2DGrid.clear();
}

// nXIndex and nYIndex run from -m_nRadialElements to +m_nRadialElements
geo::CPoint CWellZoomInMesh::Grid2DPointAt(int iXIndex, int iYIndex) const
{
  const geo::IPoint* pPoint = m_vc2DGrid[abs(iYIndex)][abs(iXIndex)];
  double dSignX = (iXIndex < 0) ? -1. : 1.;
  double dSignY = (iYIndex < 0) ? -1. : 1.;
  return geo::CPoint(dSignX * pPoint->X(), dSignY * pPoint->Y());
}

void CWellZoomInMesh::CreateHexaElements(const std::vector<int>& vcGrid1, const std::vector<int>& vcGrid2, double uppertmd, double lowertmd, TBoundaryElementVec& vcBoundaryElements, const SegmentInfo& segmentinfo)
{
  const CFormationBase* pFormation = m_zoominmodel.FormationContaining((uppertmd + lowertmd) / 2.);
  geo::CBodyGroup* pVol = getBodyGroup4Formation(m_zoominmodel, pFormation);

  if(pVol)
  {
    geo::CPoint ptUpper;
    geo::CPoint ptMid;
    geo::CPoint ptLower;
    if ( m_zoominmodel.WellPath() )
    {
      ptUpper= well::CWellPoint(*(m_zoominmodel.WellPath()), uppertmd);
      ptMid= well::CWellPoint(*(m_zoominmodel.WellPath()), (uppertmd + lowertmd) / 2.);
      ptLower= well::CWellPoint(*(m_zoominmodel.WellPath()), lowertmd);
    }
    else
    {
      ptUpper= CNewWellPoint(*(m_zoominmodel.NewWellPath()), uppertmd);
      ptMid= CNewWellPoint(*(m_zoominmodel.NewWellPath()), (uppertmd + lowertmd) / 2.);
      ptLower= CNewWellPoint(*(m_zoominmodel.NewWellPath()), lowertmd);
    }
    int nXIndex, nYIndex;
    int sz = 2 * m_nRadialElements;
    for(nXIndex = 0; nXIndex < sz; ++nXIndex)
    {
      for(nYIndex = 0; nYIndex < sz; ++nYIndex)
      {
        // the vector of node indices for this element
        std::vector<int> vcNode(8);
        int index[4];
        index[0] = nXIndex + nYIndex * (sz + 1) + 1;
        index[1] = nXIndex + nYIndex * (sz + 1);
        index[2] = nXIndex + (nYIndex + 1) * (sz + 1);
        index[3] = nXIndex + (nYIndex + 1) * (sz + 1) + 1;

        vcNode[0] = vcGrid1[index[0]];
        vcNode[1] = vcGrid1[index[1]];
        vcNode[2] = vcGrid1[index[2]];
        vcNode[3] = vcGrid1[index[3]];
        vcNode[4] = vcGrid2[index[0]];
        vcNode[5] = vcGrid2[index[1]];
        vcNode[6] = vcGrid2[index[2]];
        vcNode[7] = vcGrid2[index[3]];

        for(int i = 0; i < 8; ++i)
          InsertNode2MidPoint(Mesh().Node(vcNode[i]), (i < 4 ? ptUpper : ptLower));

        // ownership of element is transferred to Mesh()
        geo::CHexahedron* pHexa = new geo::CHexahedron(Mesh(), vcNode);
        InsertBody2MidPoint(*pHexa, ptMid);

        geo::CBodyGroup *pActualVol = findBodyGroupContainingMidPointOfHexahedron(m_zoominmodel, pVol, pFormation, pHexa);
        pActualVol->AddBody(*pHexa);

        // put top faces into top side surface when at top of formation
        if(segmentinfo.bFormationTop)
        {
          int iSide = 8;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(0));

          if(segmentinfo.bModelStart)
          {
            std::vector<int> vcInterfaceNode(8);
            // inside nodes (connected to the hexa element)
            vcInterfaceNode[0] = vcGrid1[index[0]];
            vcInterfaceNode[1] = vcGrid1[index[1]];
            vcInterfaceNode[2] = vcGrid1[index[2]];
            vcInterfaceNode[3] = vcGrid1[index[3]];
            // outside nodes
            vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
            vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
            vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
            vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);
            assert(vcInterfaceNode[4] != -1);
            vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(0), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_TOP));
          }
        }

        // put bottom faces into bottom side surface when at bottom of formation
        if(segmentinfo.bFormationBottom)
        {
          int iSide = 9;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(5));

          if(segmentinfo.bModelEnd)
          {
            std::vector<int> vcInterfaceNode(8);
            // inside nodes (connected to the hexa element)
            vcInterfaceNode[0] = vcGrid2[index[0]];
            vcInterfaceNode[1] = vcGrid2[index[3]];
            vcInterfaceNode[2] = vcGrid2[index[2]];
            vcInterfaceNode[3] = vcGrid2[index[1]];
            // outside nodes
            vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
            vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
            vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
            vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);
            assert(vcInterfaceNode[4] != -1);
            vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(5), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_BOTTOM));
          }
        }

        // create boundary interface elements when we're on the edge of the mesh
        if(nXIndex == 0) // left
        {
          std::vector<int> vcInterfaceNode(8);
          // inside nodes (connected to the hexa element)
          vcInterfaceNode[0] = vcGrid1[index[2]];
          vcInterfaceNode[1] = vcGrid1[index[1]];
          vcInterfaceNode[2] = vcGrid2[index[1]];
          vcInterfaceNode[3] = vcGrid2[index[2]];
          // outside nodes
          vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
          vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
          vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
          vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);

          int iSide = (nYIndex < m_nRadialElements) ? 2 : 4;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(2));
          vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(2), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_SIDE));
        }
        else if(nXIndex == sz - 1) // right
        {
          std::vector<int> vcInterfaceNode(8);
          // inside nodes (connected to the hexa element)
          vcInterfaceNode[0] = vcGrid1[index[0]];
          vcInterfaceNode[1] = vcGrid1[index[3]];
          vcInterfaceNode[2] = vcGrid2[index[3]];
          vcInterfaceNode[3] = vcGrid2[index[0]];
          // outside nodes
          vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
          vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
          vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
          vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);

          int iSide = (nYIndex < m_nRadialElements) ? 3 : 5;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(4));
          vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(4), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_SIDE));
        }

        if(nYIndex == 0) // top
        {
          std::vector<int> vcInterfaceNode(8);
          // inside nodes (connected to the hexa element)
          vcInterfaceNode[0] = vcGrid1[index[1]];
          vcInterfaceNode[1] = vcGrid1[index[0]];
          vcInterfaceNode[2] = vcGrid2[index[0]];
          vcInterfaceNode[3] = vcGrid2[index[1]];
          // outside nodes
          vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
          vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
          vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
          vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);

          int iSide = (nXIndex < m_nRadialElements) ? 0 : 1;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(1));
          vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(1), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_SIDE));
        }
        else if(nYIndex == sz - 1) // bottom
        {
          std::vector<int> vcInterfaceNode(8);
          // inside nodes (connected to the hexa element)
          vcInterfaceNode[0] = vcGrid1[index[3]];
          vcInterfaceNode[1] = vcGrid1[index[2]];
          vcInterfaceNode[2] = vcGrid2[index[2]];
          vcInterfaceNode[3] = vcGrid2[index[3]];
          // outside nodes
          vcInterfaceNode[4] = MeshNodeToBoundaryNode(vcInterfaceNode[0]);
          vcInterfaceNode[5] = MeshNodeToBoundaryNode(vcInterfaceNode[1]);
          vcInterfaceNode[6] = MeshNodeToBoundaryNode(vcInterfaceNode[2]);
          vcInterfaceNode[7] = MeshNodeToBoundaryNode(vcInterfaceNode[3]);

          int iSide = (nXIndex < m_nRadialElements) ? 0 : 1;
          pActualVol->SideSurface(iSide).AddFace(pHexa->Face(3));
          vcBoundaryElements.push_back(CBoundaryElementDef(pHexa->Face(3), vcInterfaceNode, CBoundaryInterfaceDef::BSURF_SIDE));
        }
      }
    }
  }
}

geo::CPoint CWellZoomInMesh::SamplePosition(const geo::IElement& element) const
{
  if (isTetraMesh())
    return element.MidPoint();

  const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&element);
  if(pBody)
  {
    TBody2MidPointMap::const_iterator it = m_mpBody2MidPoint.find(pBody);
    assert(it != m_mpBody2MidPoint.end());
    return it->second;
  }

  const geo::CInterfaceElement* pInterface = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(pInterface)
  {
    return SamplePosition(*BodyFace(*pInterface).Parent());
  }

  assert(FALSE);
  return geo::CPoint();
}

geo::CPoint CWellZoomInMesh::SamplePosition(const geo::INode& node) const
{
  int idx = BoundaryNodeToMeshNode(node.Index());
  if(idx != -1)
    return SamplePosition(Mesh().Node(idx));

  if (isTetraMesh())
    return geo::CPoint(node.X(), node.Y(), node.Z());

  TNode2MidPointMap::const_iterator it = m_mpNode2MidPoint.find(&node);
  if(it != m_mpNode2MidPoint.end())
    return it->second;

  assert(FALSE);
  return geo::CPoint();
}

bool CWellZoomInMesh::getInvalidateAfterDlg() const
{
  return m_bInvalidateAfterDlg;
}

void CWellZoomInMesh::setInvalidateAfterDlg(bool invalidateAfterDlg)
{
  m_bInvalidateAfterDlg = invalidateAfterDlg;
}

void CWellZoomInMesh::RegisterGridNodes(const T3DGrid& vc3DGrid, std::vector<int>& vcGridNodeIndices, bool bModelStart, bool bModelEnd)
{
  // vcGridNodeIndices should aready have the correct size
  assert(vcGridNodeIndices.size() == vc3DGrid.second.size());

  int rowlength = 2 * m_nRadialElements + 1; // the length of a row of nodes

  int i;
  for(i = 0; i < vc3DGrid.second.size(); ++i)
  {
    vcGridNodeIndices[i] = Mesh().RegisterNode(vc3DGrid.second[i]);

    // do we have a boundary node?
    bool bBoundary = (i <  rowlength                   || // left edge
                      i >= rowlength * (rowlength - 1) || // right edge
                      i %  rowlength == 0              || // top edge
                      i %  rowlength == (rowlength - 1)|| // bottom edge
                      bModelStart                      ||
                      bModelEnd);

    if(bBoundary)
    {
      int iBoundaryNode = Mesh().RegisterNode(vc3DGrid.second[i], false); // duplicate node at that position (bUnique = false)
      m_mpMesh2Boundary.insert(TBoundaryNodeMap::value_type(vcGridNodeIndices[i], iBoundaryNode)); // forward map
      m_mpBoundary2Mesh.insert(TBoundaryNodeMap::value_type(iBoundaryNode, vcGridNodeIndices[i])); // reverse map
    }
  }
}

void CWellZoomInMesh::Create3DGridAt(const geo::IPoint& point, const geo::IVector& vecNormal, T3DGrid& vcGrid3D) const
{
  // transformation matrix for the 2D grid to this position and orientation
  geo::CMatrix matTransform = Create3DGridTransformationMatrix(point, vecNormal);

  // create the 3D grid
  int nNodesSide = 2 * m_nRadialElements + 1; // number of nodes along one side
  vcGrid3D.second.resize(nNodesSide * nNodesSide);

  int iXIndex, iYIndex;
  size_t iNode = 0;
  for(iXIndex = -m_nRadialElements; iXIndex <= m_nRadialElements; ++iXIndex)
  {
    for(iYIndex = -m_nRadialElements; iYIndex <= m_nRadialElements; ++iYIndex)
    {
      assert(iNode < vcGrid3D.second.size());
      geo::CPoint pt(Grid2DPointAt(iXIndex, iYIndex));
      pt.Transform(matTransform);
      vcGrid3D.second[iNode++] = pt;
    }
  }
}

void CWellZoomInMesh::CreateSupports(const std::vector<int>& vcGrid, bool bModelStart, bool bModelEnd)
{
  int sz = 2 * m_nRadialElements + 1;

  int i;
  int iNodeNr;

  // upper and lower edges (including corners)
  for(i = 0; i < sz; ++i)
  {
    // upper
    iNodeNr = vcGrid[i];
    CreateSupportAt(iNodeNr);

    // lower
    iNodeNr = vcGrid[(sz - 1) * sz + i];
    CreateSupportAt(iNodeNr);
  }

  // left and right edges (excluding corners)
  for(i = 1; i < sz - 1; ++i)
  {
    // left
    iNodeNr = vcGrid[sz * i];
    CreateSupportAt(iNodeNr);

    // right
    iNodeNr = vcGrid[sz * (i + 1) - 1];
    CreateSupportAt(iNodeNr);
  }

  if(bModelStart || bModelEnd)
  {
    int nXIndex;
    int nYIndex;

    for(nXIndex = 0; nXIndex < sz; ++nXIndex)
    {
      for(nYIndex = 0; nYIndex < sz; ++nYIndex)
      {
        CreateSupportAt(vcGrid[nXIndex + sz * nYIndex]);
      }
    }
  }
}

void CWellZoomInMesh::CreateSupportAt(int iNode)
{
  int iOuterNode = MeshNodeToBoundaryNode(iNode);
  assert(iOuterNode != -1);
  Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Xaxis);
  Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Yaxis);
  Mesh().CreateTranslationSupport(iOuterNode, geo::CVector::Zaxis);
}

bool CWellZoomInMesh::CheckCurvature()
{
  double dMaxSquaredRadius = DBL_MAX;

  const CWellPath* pWellPath = ZoomInModel().WellPath();

  if ( pWellPath )
  {
    const well::CWellPoint* pPrevPrev = 0; // point before last
    const well::CWellPoint* pPrev = 0; // last point
    geo::CVector vecLastNormal; // empty vector

    well::CWellPointList::Iterator it;
    int i;
    for(i = 0; i < DefinitionPointSize(); ++i)
    {
      const well::CWellPoint* pPoint = &DefinitionPoint(i);

      // get the average normal for the current definition point
      geo::CVector vecNormal = geo::CVector::NullVector;
      if(pPrevPrev)
      {
        assert(pPrev);
        vecNormal += geo::CVector(*pPrevPrev, *pPrev).UnitVector();
      }

      if(pPrev)
        vecNormal += geo::CVector(*pPrev, *pPoint).UnitVector();

      if(pPrev)
        vecNormal = vecNormal.UnitVector();

      if(!vecLastNormal.Empty())
      {
        assert(pPrevPrev);

        // calculate the maximum radius
        double dMaxSqRad = MaximumSquaredRadius(vecLastNormal, vecNormal, geo::CVector(*pPrevPrev, *pPrev).Length());
        dMaxSquaredRadius = std::min(dMaxSqRad, dMaxSquaredRadius);
      }

      if(pPrev)
        vecLastNormal = vecNormal;

      pPrevPrev = pPrev;
      pPrev = pPoint;
    }

    if(pPrevPrev && pPrev && !vecLastNormal.Empty())
    {
      // last point
      geo::CVector vecNormal = geo::CVector(*pPrevPrev, *pPrev).UnitVector();
      double dMaxSqRad = MaximumSquaredRadius(vecLastNormal, vecNormal, geo::CVector(*pPrevPrev, *pPrev).Length());
      dMaxSquaredRadius = std::min(dMaxSqRad, dMaxSquaredRadius);
    }

    double dRadius = ZoomInModel().Radius();
    if(dMaxSquaredRadius + EPS < (dRadius * dRadius))
    {
      QString msg;
      double dMaxRad = sqrt(dMaxSquaredRadius);
      CUnitNode unitNode;
      CUnitNode::TUnitType unittype = unitNode.Unit();
      CLengthQuantity lqmax(dMaxRad);
      CLengthQuantity lqcur(dRadius);
      msg = QString("Due to the curvature of the well the maximum allowable radius is %1 %2.\nThe current radius is %3 %4.\n\nWould you like the model's radius to be adapted?").
        arg(lqmax.Value(unittype),0, 'f', 2).arg(lqmax.UnitName(unittype).c_str()).arg(lqcur.Value(unittype), 0, 'f', 2).arg(lqcur.UnitName(unittype).c_str());
      if(_m()->msg(msg, MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {
        ZoomInModel().Radius(dMaxRad);
      }
      return false;
    }

    return true;
  }
  else // wjrx mantis 3401
  {
    //assert( pNewWellPath);
    const CNewWellPoint* pPrevPrev = 0; // point before last
    const CNewWellPoint* pPrev = 0; // last point
    geo::CVector vecLastNormal; // empty vector

    //well::CWellPointList::Iterator it;
    // const well::CWellPointList& defpointlist = pWellPath->DefPointList();
    int i;
    for(i = 0; i < NewDefinitionPointSize(); ++i)
    {
      const CNewWellPoint* pPoint = &NewDefinitionPoint(i);

      // get the average normal for the current definition point
      geo::CVector vecNormal = geo::CVector::NullVector;
      if(pPrevPrev)
      {
        assert(pPrev);
        vecNormal += geo::CVector(*pPrevPrev, *pPrev).UnitVector();
      }

      if(pPrev)
        vecNormal += geo::CVector(*pPrev, *pPoint).UnitVector();

      if(pPrev)
        vecNormal = vecNormal.UnitVector();

      if(!vecLastNormal.Empty())
      {
        assert(pPrevPrev);

        // calculate the maximum radius
        double dMaxSqRad = MaximumSquaredRadius(vecLastNormal, vecNormal, geo::CVector(*pPrevPrev, *pPrev).Length());
        dMaxSquaredRadius = std::min(dMaxSqRad, dMaxSquaredRadius);
      }

      if(pPrev)
        vecLastNormal = vecNormal;

      pPrevPrev = pPrev;
      pPrev = pPoint;
    }

    if(pPrevPrev && pPrev && !vecLastNormal.Empty())
    {
      // last point
      geo::CVector vecNormal = geo::CVector(*pPrevPrev, *pPrev).UnitVector();
      double dMaxSqRad = MaximumSquaredRadius(vecLastNormal, vecNormal, geo::CVector(*pPrevPrev, *pPrev).Length());
      dMaxSquaredRadius = std::min(dMaxSqRad, dMaxSquaredRadius);
    }

    double dRadius = ZoomInModel().Radius();
    if(dMaxSquaredRadius + EPS < (dRadius * dRadius))
    {
      QString msg;
      double dMaxRad = sqrt(dMaxSquaredRadius);
      CUnitNode unitNode;
      CUnitNode::TUnitType unittype = unitNode.Unit();
      CLengthQuantity lqmax(dMaxRad);
      CLengthQuantity lqcur(dRadius);
      msg = QString("Due to the curvature of the well the maximum allowable radius is %1 %2.\nThe current radius is %3 %4.\n\nWould you like the model's radius to be adapted?").
        arg(lqmax.Value(unittype), 0, 'f', 2).arg(lqmax.UnitName(unittype).c_str()).arg(lqcur.Value(unittype), 0, 'f', 2).arg(lqcur.UnitName(unittype).c_str());
      if(_m()->msg(msg, MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
      {
        ZoomInModel().Radius(dMaxRad);
      }
      return false;
    }

    return true;
  }
}

double CWellZoomInMesh::MaximumSquaredRadius(const geo::IVector& vec1, const geo::IVector& vec2, double dDist) const
{
  // cosine of the angle (vec1 and vec2 are normalized)
  double cosa = vec1.DotProduct(vec2);

  // enlarge distance with curvature factor
  dDist *= (1. + 1. / CURVATURE_FACTOR);

  // squared radius based on this angle
  if(1 - cosa < 1e-8)
    return DBL_MAX;

  double R2 = dDist / (2 * (1 - cosa));

  return R2;
}

void CWellZoomInMesh::InsertNode2MidPoint(const geo::INode& node, const geo::IPoint& midpoint)
{
  m_mpNode2MidPoint.insert(TNode2MidPointMap::value_type(&node, midpoint));
}

void CWellZoomInMesh::InsertBody2MidPoint(const geo::IBody& body, const geo::IPoint& midpoint)
{
  m_mpBody2MidPoint.insert(TBody2MidPointMap::value_type(&body, midpoint));
}

void CWellZoomInMesh::RegisterTetraBoundaryNodes(const std::map<int, int>& boundaryNodes)
{
  for (std::map<int, int>::const_iterator it = boundaryNodes.begin(); it != boundaryNodes.end(); ++it)
  {
    int meshNode = it->first;
    int boundary = it->second;

    m_mpMesh2Boundary.insert(std::make_pair(meshNode, boundary));
    m_mpBoundary2Mesh.insert(std::make_pair(boundary, meshNode));
  }
}