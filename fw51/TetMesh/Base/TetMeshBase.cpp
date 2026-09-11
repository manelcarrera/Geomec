/* Copyright (c) 2017 DIANA FEA BV                              Confidential */
#include "TetMeshBase.h"
#include "TetMeshCM2.h"
#include "TetSurface.h"
#include "SurfaceDesc.h"
#include "tet_utils.h"
#include "tet_mesh.h"
#include "tet_surface.h"
#include "Tetrahedron.h"
#include "IProgressBase.h"
#include "InterfaceElement.h"
#include "IFace.h"

namespace geo {
/////////////////////////////////////////////////////////////////
// Implementation of tetmesh progress
/////////////////////////////////////////////////////////////////
CTetMeshProgress::CTetMeshProgress()
{
}

/*!
Function is called at beginning of the meshing
procedure. nJobs defines how many time the
new job function is called.
*/
void CTetMeshProgress::StartMesh(int /*nJobs*/)
{
}

/*!
Function is called when a new job in mesher starts.
*/
void CTetMeshProgress::NewJob(std::string /*sJobName*/)
{
}

/*!
Function when the meshing job is end up
*/
void CTetMeshProgress::StopMesh()
{
}

/////////////////////////////////////////////////////////////////
// Implementation of tetmesh base
/////////////////////////////////////////////////////////////////
/*!
Copies a point in a 3D double array
*/
void CTetMeshBase::CopyPoint(const IPoint& point, double* pPoint) const
{
  pPoint[0] = point.X();
  pPoint[1] = point.Y();
  pPoint[2] = point.Z();
}

CTetMeshBase::CTetMeshBase()
  : m_nBodySize(-1),
  m_nContactSize(0),
  m_nInterfaceSize(0),
  m_dMeshTolerance(1E-5)
{
}

CTetMeshBase::~CTetMeshBase()
{
  Clear();
}

/*!
Returns the number of surfaces in the mesh
*/
int CTetMeshBase::NrOfSurfaces() const
{
  return (int)m_vcSurfaceDesc.size();
}

/*!
Returns a surface descriptor based on an index
*/
const CSurfaceDesc& CTetMeshBase::SurfaceDesc(int nIndex) const
{
  return *m_vcSurfaceDesc[nIndex];
}

/*!
Returns a surface descriptor based on an index
*/
CSurfaceDesc& CTetMeshBase::SurfaceDesc(int nIndex)
{
  return *m_vcSurfaceDesc[nIndex];
}

/*!
Adds a non-slipping surface
Returns the index of the surface.
\param bFault  TRUE when the surface is modelled as an fault FALSE when it is a normal surface.
*/
int CTetMeshBase::AddSurface( const ISurface&    surface,
  const std::string& sName,
  bool               bFault)
{
  int nIndex = (int)m_vcSurfaceDesc.size();

  // Add surface to the mesh
  new CSurfaceDesc( *this, surface, sName, bFault );

  return nIndex;
}

/*!
Adds a point to the main mesh and returns the index that can be used
for creating hexahedrons with the function 'RegisterTetrahedronInMainMesh'
*/
int CTetMeshBase::RegisterNodeInMainMesh(IPoint& point, int nElements)
{
  return RegisterNode(point, false, nElements);
}

/*!
Adds a hexahedron element in the main mesh and returns the element index
*/
int CTetMeshBase::RegisterTetrahedronInMainMesh(int nNode0,
  int nNode1,
  int nNode2,
  int nNode3)
{
  CTetrahedron* pTetrahedron =
    new CTetrahedron(*this, nNode0, nNode1, nNode2, nNode3);
  assert(this == pTetrahedron->IndexingElementSet());
  return pTetrahedron->Index();
}

/*!
Adds a hexahedron element in the main mesh and returns the element index
*/
int CTetMeshBase::RegisterTetrahedronInMainMesh(const std::vector<int>& vcNode)
{
  CTetrahedron* pTetrahedron = new CTetrahedron(*this, vcNode);
  assert(this == pTetrahedron->IndexingElementSet());
  return pTetrahedron->Index();
}

/*!
Adds a tet volume to the mesh and returns the volume index. the tet volume is
defined by the tetrahedron element indices.
*/
int CTetMeshBase::RegisterTetVolume(const std::vector<int>& vcElement)
{
  // Create a volume
  int nIndex = (int)m_vcVolume.size();
  CBodyGroup *pVolume = new CBodyGroup(*this);
  m_vcVolume.push_back(pVolume);

  pVolume->reserveBodySize(vcElement.size());

  // Fill the volume with tets
  for(int i = 0; i < vcElement.size(); i++) {
    const IBody *pBody = dynamic_cast<const IBody*>(&Element(vcElement[i]));
    assert(pBody);
    pVolume->AddBody(*pBody);
  }

  return nIndex;
}

/*!
Adds a tet surfaces to the main mesh and is defined by the tet volume, surface
descriptor and the node indexes of nodes in the main mesh
*/
bool CTetMeshBase::RegisterTetSurface( CSurfaceDesc&           desc,
                                       CBodyGroup&             body_group,
                                       const std::vector<int>& vcNode )
{
  CTetSurface *pSurface = new CTetSurface(desc, body_group);
  pSurface->CreateSurface(vcNode);
  return true;
}

/*!
Invalidate the mesh and flush all the data. Also the input data
*/
void CTetMeshBase::Clear()
{
  // Volumes are deleted by the IMesh interface
  m_vcVolume.clear();

  CMesh3D::Clear();  // Clear the whole mesh

                     // Delete all surfaces
  for(int i = 0; i < m_vcSurfaceDesc.size(); i++)
    delete m_vcSurfaceDesc[i];

  // clear in- and output
  m_vcSurfaceDesc.clear();

  m_nBodySize = -1;
}

/*!
Internal function to add a function to the mesh.
*/
bool CTetMeshBase::AddSurfaceToMesh( void*              pMesh,
                                     const ISurface&    surface,
                                     int                nSurfaceId,
                                     const std::string& sName,
                                     bool               bSlip)
{
  // Is the mesh OK?
  if(MESH_ERROR(pMesh))
    return false;

  // Open the surface ...
  MeshSurfaceOpen(pMesh, nSurfaceId, sName.c_str(), bSlip);

  // Loop triangles
  for(int nTriangle = 0; nTriangle < surface.FaceSize(); nTriangle++)
  {
    const IFace& face = surface.Face(nTriangle);
    assert(face.NrOfPoints() == 3);

    double point0[3];
    double point1[3];
    double point2[3];
    CopyPoint(face.Point(0), point0);
    CopyPoint(face.Point(1), point1);
    CopyPoint(face.Point(2), point2);

    MeshSurfaceAdd(pMesh, point0, point1, point2);
  }

  // Just close the mesh
  MeshSurfaceClose(pMesh);

  // Is the mesh OK?
  if(MESH_ERROR(pMesh))
    return false;

  return true;
}

/*!
Add the points of the surfaces to the mesher
*/
void CTetMeshBase::AddSurfacePointsToMesh(void* pMesh, const ISurface& surface)
{
  for(int i = 0; i < surface.PointSize(); i++)
  {
    MeshPointAdd( pMesh,
                  surface.Point(i).X(),
                  surface.Point(i).Y(),
                  surface.Point(i).Z());
  }
}

/*!
Retrieves the mesh errors from the C mesher
*/
CTetMeshBase::TRet CTetMeshBase::OnMeshError(void* pMesh)
{
  TRet status;
  for(int i = 0; i < MeshMessageSize(pMesh); i++)
  {
    if(MeshMessageStatus(pMesh, i) > 0)
      status.first.push_back(std::string("ERROR   : ") + std::string(MeshMessageText(pMesh, i)));
    else
      status.first.push_back(std::string("WARNING : ") + std::string(MeshMessageText(pMesh, i)));

  }
  status.second = false;
  MeshDelete(pMesh);
  return status;
}

CTetMeshBase* CTetMeshBase::GetTetMesher(const std::string type)
{
#if !defined(_DEBUG) && defined(WIN32)
  if (type == "CM2")
  {
    return new CTetMeshCM2();
  }
  else if (type == "CM2 Repair")
  {
    CTetMeshCM2 *mesh = new CTetMeshCM2;
    mesh->Option(CTetMeshCM2::NO_STRICT_MODE, true);
    mesh->Option(CTetMeshCM2::NO_CRASHING, true);

    return mesh;
  }
#endif
  {
    return new CTetMeshBase();
  }
}

const geo::INode& CTetMeshBase::NearestNode(const geo::IPoint& point,
  const geo::IFace* face)
{
  typedef std::map<double, const geo::INode*> TNodeMap;
  TNodeMap mpNode;
  for (int i = 0; i < face->NrOfPoints(); i++) {
    double dSquareDistance = point.DistanceSquared(face->Node(i));
    mpNode.insert(TNodeMap::value_type(dSquareDistance, &face->Node(i)));
  }
  return *mpNode.begin()->second;
}

/*!
Generates the mesh, fails if no correct input was given
The diapaths must be valid.
*/
CTetMeshBase::TRet CTetMeshBase::CreateMesh(const std::string &sDianaPath,
  const std::string &sDiaSharePath,
  const std::string &sDianaLibPath,
  const std::string &sDianaSLibPath,
  const std::string &sFgvExeName,
  CTetMeshProgress& progress)
{
  // The progress indicator destincs 5 jobs
  // I    Generate input data
  // II   Evaluate input data
  // III  Generate mesh structure
  // IV   Generate mesh
  // V    Generate output
  progress.StartMesh(5);
  progress.NewJob( "Generate input data (legacy)" );

  void* pMesh = MeshCreate(m_dMeshTolerance);

  // Fill model first with points of the surfaces
  int i;
  for(i = 0; i < m_vcSurfaceDesc.size(); i++)
  {
    const CSurfaceDesc& surf_desc = *m_vcSurfaceDesc[i];
    AddSurfacePointsToMesh(pMesh, surf_desc.Surface());
  }

  // Fill model with surfaces
  for(i = 0; i < m_vcSurfaceDesc.size(); i++)
  {
    const CSurfaceDesc& surf_desc = *m_vcSurfaceDesc[i];
    // Export one single surface ....
    if ( !AddSurfaceToMesh( pMesh,
                            surf_desc.Surface(),
                            i,
                            surf_desc.Name(),
                            surf_desc.Slip() ) ) return OnMeshError( pMesh );
  }

  if ( MESH_ERROR(pMesh) ) return OnMeshError(pMesh);

  progress.NewJob("Evaluate input data (legacy)");
  MeshGenerate( sDianaPath.c_str(),
                sDiaSharePath.c_str(),
                sDianaLibPath.c_str(),
                sDianaSLibPath.c_str(),
                sFgvExeName.c_str(),
                pMesh );

  if ( MESH_ERROR(pMesh) ) return OnMeshError(pMesh);

  // Get output from mesher : Nodes, elements and groups

  // Tetra elements
  // Tet side surfaces

  progress.NewJob( "Generate mesh structure (legacy)" );
  // Creating tet volume and tet surfaces of the volume
  TTetSurfaceMap mpTetSurface;

  progress.NewJob("Generate mesh (legacy)");
#if 0
  RunFemGV( sDianaPath,
            sDiaSharePath,s
            sDianaLibPath,
            sDianaSLibPath,
            sFgvExeName );
#endif

  progress.NewJob("Generate output (legacy)");

  /* Generate the nodes */
  for ( i = 0; i < NodeCount( pMesh ); i++ )
  {
    const ftn_double_t* pCoordinate = NodeAt( i, pMesh );
    VERIFY( RegisterNode(geo::CPoint(pCoordinate[0], pCoordinate[1], pCoordinate[2]), false) == i);
  }

  /* Generate the thetrahedrons */
  for(i = 0; i < TetrahedronCount( pMesh ); i++)
  {
    int nNode[4];
    int j;
    for( j = 0; j < 4; j++)
      nNode[j] = TetrahedronNodeAt(i, j, pMesh);
    new CTetrahedron(*this, nNode[0], nNode[1], nNode[2], nNode[3] );
  }

  // Create a point map to back
  typedef std::pair<int, int> TInterfaceKey;
  typedef std::pair<const geo::IFace*, const geo::IFace*> TFacePair;
  typedef std::map<TInterfaceKey, TFacePair> TInterfaceMap;
  TInterfaceMap interface_map;

  /* Generate mesh bodies, surfaces and side surfaces */
  for(i = 0; i < MeshBodyCount( pMesh ); i++)
  {
    typedef std::set<int> TTriangle;
    typedef std::pair<const IBody*, int> TBodyFace;
    typedef std::map<TTriangle, TBodyFace> TFaceMap;
    TFaceMap mpFace;
    CBodyGroup *pVolume = new CBodyGroup(*this);
    m_vcVolume.push_back(pVolume);
    /* Fill body group with tetrahedrons */
    int j;
    for(j = 0; j < MeshBodyTetrahedronCount( i, pMesh ); j++)
    {
      int nIndex = MeshBodyTetrahedronAt(i, j, pMesh);
      const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&Element( nIndex ));
      assert(pBody);
      pVolume->AddBody( *pBody );
      int k;
      for(k = 0; k < pBody->NrOfFaces(); k++)
      {
        int l;
        TTriangle stTriangle;
        for(l = 0; l < pBody->FacePointIndices(k).size(); l++)
        {
          int nFaceNodeIndex = pBody->FacePointIndices(k)[l];

          VERIFY(stTriangle.insert( pBody->PointIndex(nFaceNodeIndex) ).second);
        }
        mpFace.insert(TFaceMap::value_type(stTriangle, TBodyFace(pBody, k)));
      }
    }

    /* Do the side surfaces */
    for(j = 0; j < MeshBodySideSurfaceCount( i, pMesh ); j++)
    {

      /* Create the side surface fill it with triangles */
      int nSurfaceDescIndex = MeshBodySideSurfaceParentIndex(i, j, pMesh);
      int nSurfaceType = MeshBodySideSurfaceParentType(i, j, pMesh);
      CSurfaceDesc *pDesc = m_vcSurfaceDesc[nSurfaceDescIndex];
      assert(pDesc);
      CTetSurface *pSideSurface = new CTetSurface(*pDesc, *pVolume);
      int k;
      for(k = 0; k < MeshBodySideSurfaceTriangleCount(i, j, pMesh); k++)
      {
        /* Do it for normal surfaces */
        TTriangle stTriangle;
        int l;
        for(l = 0; l < 3; l++)
          VERIFY(stTriangle.insert(MeshBodySideSurfaceTriangleNode(i, j, k, l, pMesh)).second);
        TFaceMap::iterator it1 = mpFace.find( stTriangle );
        assert(it1 != mpFace.end());
        pSideSurface->AddFace( it1->second.first->Face(it1->second.second) );
        if(nSurfaceType != surfNormal)
        {
          const IFace& face = it1->second.first->Face(it1->second.second);
          int nInterfaceIndex = MeshBodySideSurfaceTriangleInterfaceIdx(i, j, k, pMesh);
          assert(nInterfaceIndex > -1);
          TInterfaceKey key( nSurfaceDescIndex, nInterfaceIndex );
          TFacePair face_pair( (const geo::IFace*)0, (const geo::IFace*)0 );
          TInterfaceMap::iterator it2 = interface_map.insert(TInterfaceMap::value_type(key, face_pair)).first;

          if(nSurfaceType == surfBackFault) {
            // Back triangle
            assert(!it2->second.second);
            it2->second.second = &face;
          } else {
            // Front triangle
            assert(!it2->second.first);
            it2->second.first = &face;
          }
        }
      }
    }
  }

  for ( TInterfaceMap::iterator it = interface_map.begin();
    it != interface_map.end();
    it++) {
    assert(it->second.first);
    assert(it->second.second);
    const geo::IFace* front  = it->second.first;
    const geo::IFace* back   = it->second.second;
    CSurfaceDesc      &desc  = *m_vcSurfaceDesc[it->first.first];

    AddInterfaceElement(desc, front, back);
  }

  assert ( MeshBodyCount( pMesh ) == NrOfVolumes() );

  progress.StopMesh();

  // Add the strings supplied by the mesher
  TRet status;
  for ( i = 0; i < MeshMessageSize(pMesh); i++) {
    if(MeshMessageStatus(pMesh, i) > 0)
      status.first.push_back(std::string("ERROR   : ") + std::string(MeshMessageText(pMesh, i)));
    else
      status.first.push_back(std::string("WARNING : ") + std::string(MeshMessageText(pMesh, i)));
  }

  status.second = true;
  return status;
}

/*!
Get the volume at the given index (Volumes are generated with the mesh)
*/
const CBodyGroup &CTetMeshBase::Volume(int nIndex) const
{
  assert(nIndex > -1);
  assert(nIndex < NrOfVolumes());
  return *m_vcVolume[nIndex];
}

/*!
Get the volume at the given index (Volumes are generated with the mesh)
*/
CBodyGroup &CTetMeshBase::Volume(int nIndex)
{
  assert(nIndex > -1);
  assert(nIndex < NrOfVolumes());
  return *m_vcVolume[nIndex];
}

/*!
Get the number of volumes in the mesh (Volumes are generated with the mesh)
*/
int CTetMeshBase::NrOfVolumes() const
{
  return (int)m_vcVolume.size();
}

/*!
Returns number of generated contact elements
*/
int CTetMeshBase::ContactElementSize() const
{
  return m_nContactSize;
}

/*!
Returns the number of generated interfaces
*/
int CTetMeshBase::InterfaceElementSize() const
{
  return m_nInterfaceSize;
}

/*!
Returns the number of generated bodys (elements)
*/
int CTetMeshBase::BodySize() const
{
  if(m_nBodySize == -1)
    return ElementSize();
  return m_nBodySize;
}


void CTetMeshBase::Option(int /*option*/, bool /*flag*/)
{
}

bool CTetMeshBase::Option(int /*option*/) const
{
  return false;
}


/*!
Registers an element in the mesh
*/
int CTetMeshBase::RegisterElement(IElement& element)
{
  int nIndex = CMesh3D::RegisterElement(element);
  if(element.NrOfPoints() == 3)  // Insertion of contact element
  {
    if(m_nBodySize == -1)
      m_nBodySize = nIndex;
    m_nContactSize++;
  }
  else if(element.NrOfPoints() == 6)
  {
    if(m_nBodySize == -1)
      m_nBodySize = nIndex;
    m_nInterfaceSize++;
  }
  else
  {
    // First insert the bodies then the interface elements
    assert(m_nBodySize == -1);
  }

  return nIndex;
}

const IInterfaceElement *CTetMeshBase::AddInterfaceElement(CSurfaceDesc& desc,
  const geo::IFace* front, const geo::IFace* back)
{
  std::vector <int> vcNode(6);

  for (int i = 0; i < 3; ++i)
  {
    vcNode[i] = front->Node(i).Index();
    vcNode[i + 3] = NearestNode(front->Node(i), back).Index();
  }

  CInterfaceElement* pElement =
    new CInterfaceElement(*this, front, back, vcNode);

  desc.AddInterfaceElement(*pElement);

  return pElement;
}

}
