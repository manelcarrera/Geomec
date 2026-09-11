#include "HexaMesher.h"
#include "IProgressBase.h"
#include "GlobalMessage.h"

#include "HexaMesh.h"
#include "HexaHorizon.h"
#include "HexaFormation.h"
#include "HexaEntryTypes.h"
#include "HexaBoundary.h"
#include "hexameshregion.h"
#include "HexaModel.h"
#include "InterfaceElement.h"
#include "BaseEntryTypes.h"
#include "Global.h"
#include "IPlatform.h"
#include "ElementGroup.h"
#include "Hexahedron.h"
#include "ISupport.h"
#include "ModifiedHint.h"
#include "PressureSupportNode.h"
#include "RotatedSystem.h"

#include <algorithm>




CHexaMesher::CHexaMesher(CHexaMesh& mesh)
  : IHexaMesher(mesh)
{
}


int CHexaMesher::MesherType() const
{
  return OLD_MESHER;
}


void CHexaMesher::Invalidate()
{
  m_pPointsInternal.clear();
  m_pPoints.clear();
  m_pSlipPoints.clear();
  m_aMeshGrid.Clear();
}


bool CHexaMesher::CanCreateMesh()
{
  if (m_pPoints.size() == 0)
    return true;

  return false;
}

#ifdef DBG_HEXAMESHER
void print_grid(QString fn, geo::CArray<geo::CPoint>& grid)
{
  fn = QString("E:\\rgi\\") + fn + ".txt";
  FILE *fp = fopen(fn.toStdString().c_str(), "w");
  for (int i = 0; i < grid.Size(); ++i)
  {
    geo::CPoint p = grid.Object(i);
    fprintf(fp, "%i\t%f\t%f\t%f\n", i, p.X(), p.Y(), p.Z());
  }
  fclose(fp);
}
#endif


bool CHexaMesher::CreateMesh(IProgressBase& progress)
{
  QString errmsg;
  progress.NextJob("Create horizon mesh grid");

  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

#ifdef DBG_HEXAMESHER
  while (pFormation)
  {
    CreateHorizonMeshGrid(*pFormation, progress, &errmsg);

    print_grid(pFormation->Name(), m_aMeshGrid);

    CHexaHorizon *pHorizon = &pFormation->LowerHorizon();
    pFormation = pHorizon->LowerFormation();

  }

#endif
    if (!CreateHorizonMeshGrid(progress, &errmsg))
  {
    _m()->msg(errmsg);
    return false;
  }
#ifdef DBG_HEXAMESHER
  print_grid(QString("total"), m_aMeshGrid); 
#endif

  progress.NextJob("Create meshnodes");
  CreateMeshNodes(progress);
  progress.NextJob("Modify meshnodes");
  ModifyMeshNodes(progress);
  progress.NextJob("Insert sliplayers");
  InsertSlipLayers(progress);
  progress.NextJob("Insert meshnodes");
  InsertMeshNodes(progress);
  progress.NextJob("Create hexahedron elements");
  CreateHexahedronMesh(progress);
  progress.NextJob("Create interface elements");
  CreateInterfaceElements(progress);
  progress.NextJob("Create tyings");
  CreateTyings(progress);

  return true;
}


// Create a pointsarray of all the meshzones within the horizon. This is used as a
// projectionlayer for all the horizons.
bool CHexaMesher::CreateHorizonMeshGrid(IProgressBase& progress, QString* pErrMsg)
{
  typedef std::map<geo::CPoint, geo::CPoint, CXYLess> TPointMap2D;
  //typedef std::set<geo::CPoint, CMarcoXYLess> TPointSet2D;
  geo::CArray<geo::CPoint> pointszone;

  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  TPointMap2D point_map;
  long count = 0;
  progress.AddSteps(stNode.size());

  CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion;
  CHexaMeshRegionEntry::TNodeSet::iterator itRegion;
  std::vector<CHexaMeshRegionBase*> vcRegsInCurrentReg;
  geo::CArray<geo::CPoint>::iterator PtIt;
  m_vcRegions.clear();

  itMeshRegion = stNode.begin();

  for (itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {
    if ((*itMeshRegion)->Hull().NrOfPoints() < 4)
      delete *itMeshRegion;
  }

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  stNode = pEntry->EntryNodes();
  for (itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {
    geo::CPoint ptMeshRegionMin = rs.LocalMin((*itMeshRegion)->Hull());
    geo::CPoint ptMeshRegionMax = rs.LocalMax((*itMeshRegion)->Hull());
    vcRegsInCurrentReg.clear();
    if (dynamic_cast<CHexaSubMeshRegion*>(*itMeshRegion))
    {
      // only when current meshregion is not the main mesh region.
      for (itRegion = stNode.begin(); itRegion != stNode.end(); itRegion++)
      {
        if (*itRegion == *itMeshRegion)
          continue;

        geo::CPoint ptRegionMin = rs.LocalMin((*itRegion)->Hull());
        geo::CPoint ptRegionMax = rs.LocalMax((*itRegion)->Hull());
        bool bInside = (ptRegionMin.X() > ptMeshRegionMin.X() && ptRegionMin.Y() > ptMeshRegionMin.Y() &&
          ptRegionMax.X() < ptMeshRegionMax.X() && ptRegionMax.Y() < ptMeshRegionMax.Y());

        if (bInside)
        {
          // don't allow smaller gridsizes
          if ((*itRegion)->GridSizeX() > (*itMeshRegion)->GridSizeX() || (*itRegion)->GridSizeY() > (*itMeshRegion)->GridSizeY())
          {
            if (pErrMsg)
            {
              pErrMsg->append(QString("The grid of region '%1' is courser than the grid of region '%2', "
                "but region '%3' is completely inside region '%4'.\nThis is not allowed.\n\nMeshing aborted.").
                arg((*itRegion)->Name()).arg((*itMeshRegion)->Name()).
                arg((*itRegion)->Name()).arg((*itMeshRegion)->Name()));
            }
            return false;
          }
        }

        if ((*itRegion)->GridSizeX() > (*itMeshRegion)->GridSizeX() || (*itRegion)->GridSizeY() > (*itMeshRegion)->GridSizeY())
        {
          if (bInside)
          {
            vcRegsInCurrentReg.push_back(*itRegion);
          }
        }
      }
    }
    // Create and retrieve all the points for the mesh region.
    pointszone = (*itMeshRegion)->MeshGrid();

    if (vcRegsInCurrentReg.size() == 0)
    {
      for (PtIt = pointszone.begin(); PtIt != pointszone.end(); PtIt++)
        point_map.insert(TPointMap2D::value_type(rs.ToLocal(*PtIt), *PtIt));
    }
    else
    {
      for (PtIt = pointszone.begin(); PtIt != pointszone.end(); PtIt++)
      {
        bool bInARegion = false;
        for (size_t i = 0; i < vcRegsInCurrentReg.size(); i++)
        {
          if (vcRegsInCurrentReg[i]->InsideRegion(*PtIt, false))
          {
            bInARegion = true;
            break;
          }
        }
        if (!bInARegion)
          point_map.insert(TPointMap2D::value_type(rs.ToLocal(*PtIt), *PtIt));
      }
    }

    m_vcRegions.push_back(*itMeshRegion);
    count++;
    progress.Step();
  }

  // Copy all the sorted points in the CArray.
  for (TPointMap2D::iterator copyPoint = point_map.begin(); copyPoint != point_map.end(); copyPoint++)
  {
    geo::CPoint point = copyPoint->second;
    m_aMeshGrid.PushBack(point);
  }

  return true;
}


bool CHexaMesher::CreateHorizonMeshGrid(CHexaFormation& formation, IProgressBase& progress, QString* pErrMsg)
{
  typedef std::map<geo::CPoint, geo::CPoint, CXYLess> TPointMap2D;
  //typedef std::set<geo::CPoint, CMarcoXYLess> TPointSet2D;
  geo::CArray<geo::CPoint> pointszone;

  m_aMeshGrid.Clear();
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  TPointMap2D point_map;
  long count = 0;
  progress.AddSteps(stNode.size());

  CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion;
  CHexaMeshRegionEntry::TNodeSet::iterator itRegion;
  std::vector<CHexaMeshRegionBase*> vcRegsInCurrentReg;
  geo::CArray<geo::CPoint>::iterator PtIt;
  m_vcRegions.clear();

  itMeshRegion = stNode.begin();

  for (itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {
    if ((*itMeshRegion)->Hull().NrOfPoints() < 4)
      delete *itMeshRegion;
  }

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  stNode = pEntry->EntryNodes();
  for (itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {
    CHexaSubMeshRegion *s = dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion);
    if (s && s->Formation(formation) == 0)
      continue;

    geo::CPoint ptMeshRegionMin = rs.LocalMin((*itMeshRegion)->Hull());
    geo::CPoint ptMeshRegionMax = rs.LocalMax((*itMeshRegion)->Hull());
    vcRegsInCurrentReg.clear();
    if (dynamic_cast<CHexaSubMeshRegion*>(*itMeshRegion))
    {
      // only when current meshregion is not the main mesh region.
      for (itRegion = stNode.begin(); itRegion != stNode.end(); itRegion++)
      {
        if (*itRegion == *itMeshRegion)
          continue;

        geo::CPoint ptRegionMin = rs.LocalMin((*itRegion)->Hull());
        geo::CPoint ptRegionMax = rs.LocalMax((*itRegion)->Hull());
        bool bInside = (ptRegionMin.X() > ptMeshRegionMin.X() && ptRegionMin.Y() > ptMeshRegionMin.Y() &&
          ptRegionMax.X() < ptMeshRegionMax.X() && ptRegionMax.Y() < ptMeshRegionMax.Y());

        if (bInside)
        {
          // don't allow smaller gridsizes
          if ((*itRegion)->GridSizeX() > (*itMeshRegion)->GridSizeX() || (*itRegion)->GridSizeY() > (*itMeshRegion)->GridSizeY())
          {
            if (pErrMsg)
            {
              pErrMsg->append(QString("The grid of region '%1' is courser than the grid of region '%2', "
                "but region '%3' is completely inside region '%4'.\nThis is not allowed.\n\nMeshing aborted.").
                arg((*itRegion)->Name()).arg((*itMeshRegion)->Name()).
                arg((*itRegion)->Name()).arg((*itMeshRegion)->Name()));
            }
            return false;
          }
        }

        if ((*itRegion)->GridSizeX() > (*itMeshRegion)->GridSizeX() || (*itRegion)->GridSizeY() > (*itMeshRegion)->GridSizeY())
        {
          if (bInside)
          {
            vcRegsInCurrentReg.push_back(*itRegion);
          }
        }
      }
    }
    // Create and retrieve all the points for the mesh region.
    pointszone = (*itMeshRegion)->MeshGrid();

    if (vcRegsInCurrentReg.size() == 0)
    {
      for (PtIt = pointszone.begin(); PtIt != pointszone.end(); PtIt++)
        point_map.insert(TPointMap2D::value_type(rs.ToLocal(*PtIt), *PtIt));
    }
    else
    {
      for (PtIt = pointszone.begin(); PtIt != pointszone.end(); PtIt++)
      {
        bool bInARegion = false;
        for (size_t i = 0; i < vcRegsInCurrentReg.size(); i++)
        {
          if (vcRegsInCurrentReg[i]->InsideRegion(*PtIt, false))
          {
            bInARegion = true;
            break;
          }
        }
        if (!bInARegion)
          point_map.insert(TPointMap2D::value_type(rs.ToLocal(*PtIt), *PtIt));
      }
    }

    m_vcRegions.push_back(*itMeshRegion);
    count++;
    progress.Step();
  }

  // Copy all the sorted points in the CArray.
  for (TPointMap2D::iterator copyPoint = point_map.begin(); copyPoint != point_map.end(); copyPoint++)
  {
    geo::CPoint point = copyPoint->second;
    m_aMeshGrid.PushBack(point);
  }

  return true;

}


bool CHexaMesher::CreateHorizonMeshGrid()
{
  IProgressBase progress;

  return CreateHorizonMeshGrid(progress);
}


// Create all the meshnodes for all the horizons. Use the meshgrid as a projectionlayer at the horizon.
void CHexaMesher::CreateMeshNodes(IProgressBase& progress)
{
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

  CHexaHorizon *pHorizon = 0;
  if (pFormation != 0)
    pHorizon = &pFormation->UpperHorizon();

  CHexaHorizon *pCountHorizon = pHorizon;
  CHexaFormation *pCountFormation = pFormation;
  long count = 0;
  while (pCountHorizon)
  {
    progress.AddSteps(m_aMeshGrid.Size());
    if (pCountFormation != 0)
    {
      pCountHorizon = &pCountFormation->LowerHorizon();
      pCountFormation = pCountHorizon->LowerFormation();
    }
    else
      pCountHorizon = 0;
  }

  while (pHorizon != 0)
  {
    for (geo::CArray<geo::CPoint>::iterator itPoint = m_aMeshGrid.begin(); itPoint != m_aMeshGrid.end(); itPoint++)
    {
      // A horizon with a constant depth. Just create points at this depth.
      geo::CValue dValue;
      if (pHorizon->ConstantDepth())
      {
        dValue = pHorizon->Depth().Value();
      }
      else
      {
        assert(pHorizon->SurfaceSize() > 0);
        if (pHorizon->SurfaceSize() > 1)
        {
          int i = 0;
          while ((i < pHorizon->SurfaceSize()) && !dValue.Valid())
          {
            dValue = pHorizon->Surface(i).InterpolateDepth(*itPoint, false);
            i++;
          }
          double dMinDistance = DBL_MAX;
          i = 0;
          while (i < pHorizon->SurfaceSize())
          {
            const geo::CPoint nearest_point = pHorizon->Surface(0).Surface().NearestXYPoint(*itPoint);
            geo::CPoint diff = nearest_point - *itPoint;
            double dDistance = diff.X() * diff.X() + diff.Y() * diff.Y();
            if (dDistance < dMinDistance)
            {
              dValue = geo::CValue(nearest_point.Z());
              dMinDistance = dDistance;
            }
            ++i;
          }
          assert(dValue.Valid());
        }
        else
          dValue = pHorizon->Surface(0).InterpolateDepth(*itPoint, true);
      }

      // We register a mesh node in the mesh
      int nIndex = Mesh().m_mesh.RegisterNode(geo::CPoint((*itPoint).X(), (*itPoint).Y(), dValue.Value()), false);
      m_pPoints.push_back(nIndex);
      count++;
      progress.Step();
    }

    if (pFormation != 0)
    {
      pHorizon = &pFormation->LowerHorizon();
      pFormation = pHorizon->LowerFormation();
    }
    else
      pHorizon = 0;
  }
}


// Check to see if a horizon has a sliplayer and if so, create a copy of
// those nodes which are at a sliplayer.
void CHexaMesher::InsertSlipLayers(IProgressBase& progress)
{
  int meshsize = m_aMeshGrid.Size();
  int iCounter = 1;

  long count, max;
  count = max = 0;
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());
  while (pFormation != 0)
  {
    CHexaHorizon *pHor = &pFormation->LowerHorizon();
    if (pHor->Slip())
    {
      max++;
    }

    // Next Formation.
    pFormation = pHor->LowerFormation();
  }
  progress.AddSteps(max*meshsize);

  pFormation = &(Mesh().GetTopFormation());
  while (pFormation != 0)
  {
    CHexaHorizon *pHor = &pFormation->LowerHorizon();
    if (pHor->Slip())
    {
      for (int i = 0; i < meshsize; i++)
      {
        // Creation of slip nodes ....
        int nIndex = Mesh().m_mesh.RegisterNode(Mesh().m_mesh.Point(m_pPoints[iCounter*meshsize + i]), false);
        m_pSlipPoints.push_back(nIndex);
        count++;
        progress.Step();
      }
    }
    iCounter++;

    // Next Formation.
    pFormation = pHor->LowerFormation();
  }
}
// Take a look at the minimum thickness of a formation. If this is smaller than the given value of
// dMinThichness for the formation, adjust the lower node.
void CHexaMesher::ModifyMeshNodes(IProgressBase& progress)
{
  int meshsize = m_aMeshGrid.Size();
  int iNumOfSets = m_pPoints.size() / meshsize;
  progress.AddSteps(iNumOfSets - 1);

  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

  for (int i = 0; i<iNumOfSets - 1; i++)
  {
    progress.Step();
    assert(pFormation);

    double dMinThickness = pFormation->MinThickness().Value();
    for (int j = 0; j<meshsize; j++)
    {
      if (Mesh().m_mesh.Point(m_pPoints[(i + 1)*meshsize + j]).Z() - Mesh().m_mesh.Point(m_pPoints[i*meshsize + j]).Z() < dMinThickness)
      {
        const geo::IPoint &p_old = Mesh().m_mesh.Point(m_pPoints[(i + 1)*meshsize + j]);
        Mesh().m_mesh.Node(m_pPoints[(i + 1)*meshsize + j], geo::CPoint(p_old.X(),
          p_old.Y(),
          Mesh().m_mesh.Point(m_pPoints[i*meshsize + j]).Z() + dMinThickness));
      }
    }
    // Next Formation.
    CHexaHorizon *pHor = &pFormation->LowerHorizon();
    pFormation = pHor->LowerFormation();
  }
}
// Create all the points which are in between the horizons. This depends on the 
// number of elements for a formation.
void CHexaMesher::InsertMeshNodes(IProgressBase& progress)
{
  int meshblock = 1;
  int meshsize = m_aMeshGrid.Size();

  // Initialization : Get First horizon ptr. == first formation pointer
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

  // Insert nodes for each formation.
  int iNumOfSets = m_pPoints.size() / meshsize;
  progress.AddSteps(iNumOfSets - 1);
  for (int mesh = 0; mesh < iNumOfSets - 1; mesh++)
  {
    progress.Step();
    assert(pFormation);

    // Create the number of elements per formation and per node.
    //		if(pFormation->Depleting() && pFormation->Elements() < 4)
    //			pFormation->Elements(4); // 4 elements for reservoirs
    /*		else*/
    //		if(pFormation->Elements() < 2)
    //			pFormation->Elements(2); // 2 for non-reservoirs

    meshblock = pFormation->Elements();
    for (int bl = 1; bl < meshblock; bl++)
    {
      for (int it = 0; it < meshsize; it++)
      {
        const geo::IPoint& point = Mesh().m_mesh.Point(m_pPoints[it + mesh*meshsize]);
        const geo::IPoint& point_ref = Mesh().m_mesh.Point(m_pPoints[(mesh + 1)*meshsize + it]);

        // Calculate the distance at the z-axis between the horizon of a formation and
        // acoording to the number of meshblocks.
        int nIndex = Mesh().m_mesh.RegisterNode(geo::CPoint(point.X(), point.Y(), point.Z() + bl*((point_ref.Z() - point.Z()) / meshblock)));
        m_pPointsInternal.push_back(nIndex);

      }
    }
    // Next Formation.
    CHexaHorizon *pHor = &pFormation->LowerHorizon();
    pFormation = pHor->LowerFormation();
  }
}


// Create hexahedron elements which consists out of 8 points. The pointarray is sorted so 
// this method don't need much intelligence.
void CHexaMesher::CreateHexahedronMesh(IProgressBase& progress)
{
  CHexaModel *pModel = dynamic_cast<CHexaModel*>(&Model());
  int iNrHexPerLayer;
  int meshblock = 1;
  int meshsize = m_aMeshGrid.Size();

  bool found;
  int iCount;
  int it, next, up, upnext, p, n, u, v;
  int iInternalCount = 0;
  int iSlipCounter = 0;

  pModel->ResultRegister().ClearAll();

  // Initialization : Get First horizon ptr. == first formation pointer
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());
  CHexaHorizon *pHorizon = &(pFormation->LowerHorizon());
  CHexaHorizon *pPrevHorizon = 0;
  int iNrOfHorizons = 0;

  // Build hexahedron elements for each formation.
  int iNumOfSets = m_pPoints.size() / meshsize;
  progress.AddSteps(m_pPoints.size());

  // use a rotated system to create the mesh in, so we can assume
  // that elements are aligned with the main axes
  CRotatedSystem rs(pModel->Boundary().BoxAzimuth(), pModel->Boundary().Box().MidPoint());

  geo::CPoint ptLocalMin = rs.LocalMin(pModel->Boundary().Box());
  geo::CPoint ptLocalMax = rs.LocalMax(pModel->Boundary().Box());

  double dYbegin = ptLocalMin.Y();

  geo::CLine CheckLine; // This line is the line from p to n in the hexahedron that was just created. If the p of the next
  // hexahedron is on this line, the loop stops with that point and tries with the next point.
  // This is done to prevent that elements ar constructed that are partly crossed by another element.

  for (int mesh = 0; mesh < iNumOfSets - 1; mesh++)
  {
    // The formation should not have a volume. We create a body group with an upper and lower side surface
    // on the mesh and formation volume.
    assert(pFormation && (pFormation->VolumeSize() == 1));
    geo::CBodyGroup *pGroup = new geo::CBodyGroup(Mesh().m_mesh);
    new geo::CBodyGroup::CSideSurface(*pGroup);	// SideSurface 0 (top)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 1 (bottom)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 2 (front)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 3 (back)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 4 (left)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 5 (right)
    VERIFY(pFormation->Volume(0).Volume(*pGroup));
    assert(pFormation->Volume(0).Volume().SideSurfaceSize() == 6);

    assert(pHorizon);

    // Reset the counter for counting the number of Hexahedron elements per layer per formation.
    iNrHexPerLayer = 0;

    iCount = mesh*meshsize;
    for (it = iCount; it < (mesh*meshsize) + meshsize; it++)
    {
      progress.Step();
      found = false;
      if (++iCount >(mesh*meshsize) + meshsize) break;
      p = m_pPoints[it];

      if (it != 0)
      { // Do not perform the check when this is the first point
        if (CheckLine.Contains(geo::CPoint(Mesh().m_mesh.Point(p).X(), Mesh().m_mesh.Point(p).Y(), 0), false))
          continue;
      }

      int iNextCount = iCount;
      // Search for the next point with at the same x-axis but which also make a square.
      for (next = it + 1; next < (mesh*meshsize) + meshsize; next++)
      {
        if (++iNextCount >(mesh*meshsize) + meshsize) break;

        n = m_pPoints[next];
        if (fabs(rs.ToLocal(Mesh().m_mesh.Point(n)).Y() - rs.ToLocal(Mesh().m_mesh.Point(p)).Y()) < EPS)
        {
          // Search for the next point with at the same y-axis but which also make a square.
          int iUpCount = iNextCount;
          for (up = next + 1; up < (mesh*meshsize) + meshsize; up++)
          {
            if (++iUpCount >(mesh*meshsize) + meshsize) break;

            u = m_pPoints[up];
            if (fabs(rs.ToLocal(Mesh().m_mesh.Point(u)).X() - rs.ToLocal(Mesh().m_mesh.Point(p)).X()) < EPS)
            {
              // Search for the last point of the square.
              int iUpNextCount = iUpCount;
              for (upnext = up + 1; upnext < (mesh*meshsize) + meshsize; upnext++)
              {
                if (++iUpNextCount >(mesh*meshsize) + meshsize) break;

                v = m_pPoints[upnext];
                // Check if it's a rectangle.
                if ((fabs(rs.ToLocal(Mesh().m_mesh.Point(v)).X() - rs.ToLocal(Mesh().m_mesh.Point(n)).X()) < EPS) &&
                  (fabs(rs.ToLocal(Mesh().m_mesh.Point(u)).Y() - rs.ToLocal(Mesh().m_mesh.Point(v)).Y()) < EPS))
                {
                  geo::CPoint local_p(rs.ToLocal(Mesh().m_mesh.Point(p)));
                  geo::CPoint local_v(rs.ToLocal(Mesh().m_mesh.Point(v)));

                  double dXOff = fabs(local_p.X() - local_v.X());
                  double dYOff = fabs(local_p.Y() - local_v.Y());
                  geo::CPoint ptCenter(local_p.X() + (dXOff / 2),
                    local_p.Y() + (dYOff / 2), 0);

                  bool bFalseRectangle = false;
                  for (size_t l = 0; l < m_vcRegions.size(); l++)
                  {
                    CHexaMeshRegionBase *pReg = m_vcRegions[l];
                    double dDistFromBound = fabs(local_p.Y() - dYbegin);
                    double dGridsizeX = pReg->GridSizeX();
                    double dGridsizeY = pReg->GridSizeY();
                    double dCheck = dDistFromBound + EPS / 2;
                    double dRest = fmod(dCheck, dGridsizeY);
                    if (pReg->InsideRegion(rs.ToGlobal(ptCenter), false) && fabs(dXOff - dGridsizeX) < EPS && fabs(dYOff - dGridsizeY) < EPS && dRest < EPS)
                      break;
                    if (l == m_vcRegions.size() - 1)
                      bFalseRectangle = true;
                  }

                  if (bFalseRectangle)
                    continue;

                  CheckLine.First(geo::CPoint(Mesh().m_mesh.Point(p).X(), Mesh().m_mesh.Point(p).Y(), 0));
                  CheckLine.Second(geo::CPoint(Mesh().m_mesh.Point(n).X(), Mesh().m_mesh.Point(n).Y(), 0));
                  // Create the number of elements per formation.
                  meshblock = pFormation->Elements();
                  for (int bl = 0; bl < meshblock; bl++)
                  {
                    std::vector<int> hexpoints;

                    // Calculate the distance at the z-axis between the horizon of a formation and
                    // acoording to the number of meshblocks.
                    if (bl == 0)
                    {
                      // Upper points of formation.
                      if ((pPrevHorizon != 0) && pPrevHorizon->Slip())
                      {
                        // Use the extra created slippoints.
                        hexpoints.push_back(m_pSlipPoints[(iCount - 1) - (mesh*meshsize) + (iSlipCounter - 1)*meshsize]);
                        hexpoints.push_back(m_pSlipPoints[(iNextCount - 1) - (mesh*meshsize) + (iSlipCounter - 1)*meshsize]);
                        hexpoints.push_back(m_pSlipPoints[(iUpNextCount - 1) - (mesh*meshsize) + (iSlipCounter - 1)*meshsize]);
                        hexpoints.push_back(m_pSlipPoints[(iUpCount - 1) - (mesh*meshsize) + (iSlipCounter - 1)*meshsize]);
                      }
                      else
                      {
                        // Use the regular points.
                        hexpoints.push_back(p);
                        hexpoints.push_back(n);
                        hexpoints.push_back(v);
                        hexpoints.push_back(u);
                      }
                      iNrHexPerLayer++;
                    }
                    else
                    {
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl - 1)*meshsize + (iCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl - 1)*meshsize + (iNextCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl - 1)*meshsize + (iUpNextCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl - 1)*meshsize + (iUpCount - mesh*meshsize) - 1]);
                    }

                    if (bl == meshblock - 1)
                    {
                      // Lower points of formation.
                      hexpoints.push_back(m_pPoints[meshsize + iCount - 1]);
                      hexpoints.push_back(m_pPoints[meshsize + iNextCount - 1]);
                      hexpoints.push_back(m_pPoints[meshsize + iUpNextCount - 1]);
                      hexpoints.push_back(m_pPoints[meshsize + iUpCount - 1]);
                    }
                    else
                    {
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl)*meshsize + (iCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl)*meshsize + (iNextCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl)*meshsize + (iUpNextCount - mesh*meshsize) - 1]);
                      hexpoints.push_back(m_pPointsInternal[(iInternalCount + bl)*meshsize + (iUpCount - mesh*meshsize) - 1]);
                    }

                    // Add hexahedron to the vectorlist.
                    geo::CHexahedron *hex = new geo::CHexahedron(Mesh().Mesh(), hexpoints);
                    assert(pFormation->VolumeSize() == 1);
                    assert(pFormation->Volume(0).Volume().SideSurfaceSize() == 6);
                    pFormation->Volume(0).Volume().AddBody(*hex);

                    // Add the face of the upper hexahedron of the formation to side surface nr. 0 (Upper surface)
                    if (bl == 0)
                    {
                      // Face to upper surface of formation ...
                      pFormation->Volume(0).Volume().SideSurface(TOP).AddFace(hex->Face(HEXA_UPPERFACE));
                    }

                    // Add the face of the lower hexahedron of the formation to side surface nr. 1 (Lower surface)
                    if (bl == meshblock - 1)
                    {
                      pFormation->Volume(0).Volume().SideSurface(BOTTOM).AddFace(hex->Face(HEXA_LOWERFACE));
                    }

                    geo::CPoint ptHexLocalMin(rs.LocalMin(*hex));
                    geo::CPoint ptHexLocalMax(rs.LocalMax(*hex));

                    if (fabs(ptHexLocalMin.X() - ptLocalMin.X()) < EPS)
                    {
                      pFormation->Volume(0).Volume().SideSurface(LEFT).AddFace(hex->Face(HEXA_LEFTFACE));
                    }

                    if (fabs(ptHexLocalMax.X() - ptLocalMax.X()) < EPS)
                    {
                      pFormation->Volume(0).Volume().SideSurface(RIGHT).AddFace(hex->Face(HEXA_RIGHTFACE));
                    }

                    if (fabs(ptHexLocalMin.Y() - ptLocalMin.Y()) < EPS)
                    {
                      pFormation->Volume(0).Volume().SideSurface(FRONT).AddFace(hex->Face(HEXA_FRONTFACE));
                    }

                    if (fabs(ptHexLocalMax.Y() - ptLocalMax.Y()) < EPS)
                    {
                      pFormation->Volume(0).Volume().SideSurface(BACK).AddFace(hex->Face(HEXA_BACKFACE));
                    }
                  }

                  found = true;
                  break;
                }
              }
              if (found) break;
            }
          }
          if (found) break;
        }
      }
    }

    // Used as a pointer in the m_pPointsInternal list.
    iInternalCount += (pFormation->Elements() - 1);

    // Set the number of hexahedron elements per layer per formation.
    //		pFormation->SetNrHexPerLayer(iNrHexPerLayer);

    pPrevHorizon = pHorizon;
    if ((pPrevHorizon != 0) && pPrevHorizon->Slip()) iSlipCounter++;

    // Next Formation & Horizon.
    pHorizon = &pFormation->LowerHorizon();
    pFormation = pHorizon->LowerFormation();

    // Get the next lower horizon.
    if (pFormation != 0) pHorizon = &pFormation->LowerHorizon();

    iNrOfHorizons++;
  } // Formation

}



void CHexaMesher::_Tyings_SetupPoints(_TyingsInfo& info)
{
  info.vcPoints.resize(m_pPoints.size() + m_pSlipPoints.size());

  // add the slip-points first, so the test below for the lowest points (which should not be tied in Z-direction) still works
  for (size_t i = 0; i < m_pSlipPoints.size(); ++i)
    info.vcPoints[i] = m_pSlipPoints[i];

  int nOffset = m_pSlipPoints.size();
  for (size_t i = 0; i < m_pPoints.size(); ++i)
    info.vcPoints[nOffset + i] = m_pPoints[i];

  // Create map for outer points 
  for (size_t i = 0; i < info.vcPoints.size(); i++)
  {
    TPointMap::value_type value_type(&Mesh().m_mesh.Point(info.vcPoints[i]), std::vector<int>());
    TPointMap::iterator it = info.mpPoint.insert(value_type).first;
    it->second.push_back(info.vcPoints[i]);
  }

  // Create a map for inner points
  for (size_t i = 0; i < m_pPointsInternal.size(); i++)
  {
    TPointMap::value_type value_type(&Mesh().m_mesh.Point(m_pPointsInternal[i]), std::vector<int>());
    TPointMap::iterator it = info.mpInternalPoint.insert(value_type).first;
    it->second.push_back(m_pPointsInternal[i]);
  }

}


void _local_Tyings_FindMasterRegion(const geo::IPoint& point, CHexaMeshRegionEntry::TNodeSet& stNode, CHexaMeshRegionEntry::TNodeSet::iterator& itMeshRegion, int& nDanglingType, CHexaMeshRegionBase **pMasterRegion)
{
  std::vector<CHexaMeshRegionBase*> vcRegions;
  vcRegions.reserve(20);

  for (CHexaMeshRegionEntry::TNodeSet::iterator itTempMeshReg = stNode.begin(); itTempMeshReg != stNode.end(); itTempMeshReg++)
  { // find the regions that contain the point (excluding the edge)
    if (*itTempMeshReg == *itMeshRegion) continue;

    if ((*itTempMeshReg)->InsideRegion(point, false))
    {
      vcRegions.push_back(*itTempMeshReg);
    }
  }

  double dMinGrid = 0;
  double dTmpMinGrid = 0;
  for (size_t k = 0; k < vcRegions.size(); k++)
  { // pMasterRegion should be the region with the smallest grid size (either in X or in Y direction)
    double dXGrid = vcRegions[k]->GridSizeX();
    double dYGrid = vcRegions[k]->GridSizeY();

    dTmpMinGrid = (dXGrid < dYGrid) ? dXGrid : dYGrid;
    if (dMinGrid == 0)
    {
      dMinGrid = dTmpMinGrid;
      *pMasterRegion = vcRegions[k];
      continue;
    }

    if (dTmpMinGrid < dMinGrid)
    {
      dMinGrid = dTmpMinGrid;
      *pMasterRegion = vcRegions[k];
    }
  }

  // Is the node dangling in the pMasterRegion?
  if (*pMasterRegion) nDanglingType = (*pMasterRegion)->IsDangling(*(*itMeshRegion), point, true);
}


void CHexaMesher::_Tyings_Create(_TyingsInfo& info, int index, int nMaster1, int nMaster2, double calcdist, bool do_z)
{

  //FILE *fp = fopen("E:\\Temp\\tyings.txt", "a");
  //fprintf(fp, "Tying %d: %d %d %d\n", index, nMaster1, nMaster2, do_z ? 1 : 0);
  //fclose(fp);

  Mesh().m_mesh.CreateTying(index, info.Xaxis, geo::CTying::TR,
    nMaster1, info.Xaxis, geo::CTying::TR, 1 - calcdist,
    nMaster2, info.Xaxis, geo::CTying::TR, calcdist);
  Mesh().m_mesh.CreateTying(index, info.Yaxis, geo::CTying::TR,
    nMaster1, info.Yaxis, geo::CTying::TR, 1 - calcdist,
    nMaster2, info.Yaxis, geo::CTying::TR, calcdist);
  if (do_z)
  {
    Mesh().m_mesh.CreateTying(index, geo::CVector::Zaxis, geo::CTying::TR,
      nMaster1, geo::CVector::Zaxis, geo::CTying::TR, 1 - calcdist,
      nMaster2, geo::CVector::Zaxis, geo::CTying::TR, calcdist);
  }

  info.mpCreatedTyings.insert(std::make_pair(index, std::make_pair(nMaster1, nMaster2)));
}


void CHexaMesher::_Tyings_CreateForPoints(_TyingsInfo& info, CRotatedSystem& rs, geo::CPoint& pp_local, bool force_z, std::vector<int>& points, TPointMap& mpPoint, int k0, int k1, double calcdist)
{
  int cnt = 0;
  // X and Y found, now lets go through all the points (Z differs).
  for (size_t i = 0; i < points.size(); i++)
  {
    geo::CPoint local = rs.ToLocal(Mesh().m_mesh.Point(points[i]));
    if ((fabs(local.X() - pp_local.X()) < EPS) && (fabs(local.Y() - pp_local.Y()) < EPS))
    {
      // Create the tying object.
      TTyingMap::iterator itTying = info.mpCreatedTyings.find(points[i]);
      if (itTying == info.mpCreatedTyings.end())
      {
        // not created yet
        int nMaster1 = mpPoint[&info.basicpoints->Object(k0)][cnt];
        int nMaster2 = mpPoint[&info.basicpoints->Object(k1)][cnt];
        _Tyings_Create(info, points[i], nMaster1, nMaster2, calcdist, force_z || (points.size() - i > m_aMeshGrid.Size()));
      }
      else
      {
        // make sure it's the same
        assert((mpPoint[&info.basicpoints->Object(k0)][cnt] == itTying->second.first && mpPoint[&info.basicpoints->Object(k1)][cnt] == itTying->second.second) ||
          (mpPoint[&info.basicpoints->Object(k1)][cnt] == itTying->second.first && mpPoint[&info.basicpoints->Object(k0)][cnt] == itTying->second.second));
      }
      cnt++;
    }
  }
}

void CHexaMesher::_Tyings_HandleHorizontal(_TyingsInfo& info, CRotatedSystem& rs, geo::CPoint& pp_local)
{
  // Search for masternodes at the x-axis.
  for (size_t k = 0; k < info.basicpoints->Size(); k++)
  {
    geo::CPoint local_0 = rs.ToLocal(info.basicpoints->Object(k));
    if (fabs(pp_local.Y() - local_0.Y()) < EPS)
    {
      geo::CPoint local_1 = rs.ToLocal(info.basicpoints->Object(k + 1));

      if ((pp_local.X() > local_0.X()) && (pp_local.X() < local_1.X()))
      {
        double dist = local_1.X() - local_0.X();
        double calcdist = (pp_local.X() - local_0.X()) / dist;

        _Tyings_CreateForPoints(info, rs, pp_local, false, info.vcPoints, info.mpPoint, k, k+1, calcdist);
        _Tyings_CreateForPoints(info, rs, pp_local, true, m_pPointsInternal, info.mpInternalPoint, k, k+1, calcdist);

        break;
      }
    }
    // We have past the search point so exit the loop.
    if (local_0.Y() > pp_local.Y() + EPS) break;
  }
}

void CHexaMesher::_Tyings_HandleVertical(_TyingsInfo& info, CRotatedSystem& rs, geo::CPoint& pp_local, CHexaMeshRegionBase* pMasterRegion)
{
  // Search for masternodes at the y-axis.
  for (size_t k = 0; k < info.basicpoints->Size(); k++)
  {
    geo::CPoint local_0 = rs.ToLocal(info.basicpoints->Object(k));
    if ((fabs(pp_local.X() - local_0.X()) < EPS) && ((pp_local.Y() - local_0.Y()) < pMasterRegion->GridSizeY()))
    {
      for (size_t l = k + 1; l < info.basicpoints->Size(); l++)
      {
        geo::CPoint local_1 = rs.ToLocal(info.basicpoints->Object(l));
        if ((fabs(pp_local.X() - local_1.X()) < EPS) && ((local_1.Y() - pp_local.Y()) < pMasterRegion->GridSizeY()))
        {
          double dist = local_1.Y() - local_0.Y();
          double calcdist = (pp_local.Y() - local_0.Y()) / dist;

          _Tyings_CreateForPoints(info, rs, pp_local, false, info.vcPoints, info.mpPoint, k, l, calcdist);
          _Tyings_CreateForPoints(info, rs, pp_local, true, m_pPointsInternal, info.mpInternalPoint, k, l, calcdist);
              
          break;
        }
      }
    }
    // We have past the search point so exit the loop.
    if (local_0.Y() > pp_local.Y() + EPS) break;
  }
}



// Create tying objects for all dangling nodes.
void CHexaMesher::CreateTyings(IProgressBase& progress)
{
  _TyingsInfo info;
  _Tyings_SetupPoints(info);

  // keep a map of all tyings that have already been created

  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  CHexaMainMeshRegion *pBasicRegion = &pEntry->Main();

  long count = 0;
  progress.AddSteps(stNode.size());


  // get a rotated X and Y axis
  CModelBase& model = static_cast<CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());
  info.Xaxis = rs.ToGlobal(geo::CVector::Xaxis);
  info.Yaxis = rs.ToGlobal(geo::CVector::Yaxis);

  // Loop over all the meshregions.
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
  {

    count++;
    progress.Step();

    // Check if it's not the main mesh region.
    if (*itMeshRegion != pBasicRegion)
    {
      const geo::CArray<geo::CPoint> &pointszone = (*itMeshRegion)->GetMeshRegion();
      for (size_t it = 0; it < pointszone.Size(); it++)
      {

        // Convert point from mesh region to real point.
        const geo::IPoint *pp = &Mesh().m_mesh.Point(info.mpPoint[&pointszone.Object(it)][0]);
        if (pp != 0)
        {
          int nDanglingType = MESH_NO_DANG;
          CHexaMeshRegionBase* pMasterRegion = 0; // region that will provide the master nodes for the tying

          _local_Tyings_FindMasterRegion(*pp, stNode, itMeshRegion, nDanglingType, &pMasterRegion);

          if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
          {
            geo::CPoint pp_local = rs.ToLocal(*pp);

            info.basicpoints = &pMasterRegion->GetMeshRegion();

            if (nDanglingType == MESH_HOR_DANG)
            {
              _Tyings_HandleHorizontal(info, rs, pp_local);
            }
            else // nDanglingType == MESH_VER_DANG
            {
              _Tyings_HandleVertical(info, rs, pp_local, pMasterRegion);
            }
          }
        }
      }
    }
  }
}

// Create interface elements and connectivity for slipping layers.
// The objects of the type CInterfaceElement, are added to CHorizon.
// Every InterfaceElement has a pointer to the body above and below.
void CHexaMesher::CreateInterfaceElements(IProgressBase& progress)
{
  CHexaModel *pModel = dynamic_cast<CHexaModel*>(&Model());
  int iNrHexPerLayer;
  int meshsize = m_aMeshGrid.Size();

  bool found;
  int iCount;
  const geo::IPoint *p, *n, *u, *v;
  int it, next, up, upnext;
  int iSlipCounter = 0;

  // Initialization : Get First horizon ptr. == first formation pointer
  long count = 0;
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

  CRotatedSystem rs(pModel->Boundary().BoxAzimuth(), pModel->Boundary().Box().MidPoint());

  // Build interface elements for each slipping horizon.
  int iNumOfSets = m_pPoints.size() / meshsize;
  progress.AddSteps(iNumOfSets - 1);
  for (int mesh = 0; mesh < iNumOfSets - 1; mesh++)
  {
    count++;
    progress.Step();
    assert(pFormation);
    CHexaHorizon *pHorizon = &pFormation->LowerHorizon();
    if (pHorizon->Slip())
    {
      // Now we're sure this is a slipping horizon.
      iNrHexPerLayer = 0;

      // Create a container for interface elements in the horizon
      geo::CElementGroup *pGroup = new geo::CElementGroup(Mesh().m_mesh);
      pHorizon->InterfaceElementGroup(*pGroup);

      iCount = mesh*meshsize;
      for (it = iCount; it < (mesh*meshsize) + meshsize; it++)
      {
        found = false;
        if (++iCount >(mesh*meshsize) + meshsize) break;
        p = &Mesh().m_mesh.Point(m_pPoints[it]);
        geo::CPoint local_p(rs.ToLocal(*p));

        int iNextCount = iCount;
        // Search for the next point with at the same x-axis but which also make a square.
        for (next = it + 1; next < (mesh*meshsize) + meshsize; next++)
        {
          if (++iNextCount >(mesh*meshsize) + meshsize) break;
          n = &Mesh().m_mesh.Point(m_pPoints[next]);
          geo::CPoint local_n(rs.ToLocal(*n));
          if (fabs(local_n.Y() - local_p.Y()) < EPS)
          {
            // Search for the next point with at the same y-axis but which also make a square.
            int iUpCount = iNextCount;
            for (up = next + 1; up < (mesh*meshsize) + meshsize; up++)
            {
              if (++iUpCount >(mesh*meshsize) + meshsize) break;
              u = &Mesh().m_mesh.Point(m_pPoints[up]);
              geo::CPoint local_u(rs.ToLocal(*u));
              if (fabs(local_u.X() - local_p.X()) < EPS)
              {
                // Search for the last point of the square.
                int iUpNextCount = iUpCount;
                for (upnext = up + 1; upnext < (mesh*meshsize) + meshsize; upnext++)
                {
                  if (++iUpNextCount >(mesh*meshsize) + meshsize) break;
                  v = &Mesh().m_mesh.Point(m_pPoints[upnext]);
                  geo::CPoint local_v(rs.ToLocal(*v));
                  // Check if it's square.
                  if ((fabs(local_v.X() - local_n.X()) < EPS) && (fabs(local_u.Y() - local_v.Y()) < EPS))
                  {
                    double dXOff = fabs(local_p.X() - local_v.X());
                    double dYOff = fabs(local_p.Y() - local_v.Y());
                    geo::CPoint ptCenter(local_p.X() + (dXOff / 2),
                      local_p.Y() + (dYOff / 2), 0);

                    bool bFalseRectangle = false;
                    assert(m_vcRegions.size() > 0);
                    for (size_t l = 0; l < m_vcRegions.size(); l++)
                    {
                      CHexaMeshRegionBase *pReg = m_vcRegions[l];
                      double dDistFromBound = fabs(local_p.Y() - rs.LocalMin(pModel->Boundary().Box()).Y());
                      double dGridsizeX = pReg->GridSizeX();
                      double dGridsizeY = pReg->GridSizeY();
                      double dCheck = dDistFromBound + EPS / 2;
                      double dRest = fmod(dCheck, dGridsizeY);
                      if (pReg->InsideRegion(rs.ToGlobal(ptCenter), false) && fabs(dXOff - dGridsizeX) < EPS && fabs(dYOff - dGridsizeY) < EPS && dRest < EPS)
                        break;
                      if (l == m_vcRegions.size() - 1)
                        bFalseRectangle = true;
                    }

                    if (bFalseRectangle)
                      continue;

                    iNrHexPerLayer++;

                    std::vector<int> hexpoints;

                    // The upper points are the same as the points from the upper element.
                    hexpoints.push_back(m_pPoints[meshsize + iCount - 1]);
                    hexpoints.push_back(m_pPoints[meshsize + iNextCount - 1]);
                    hexpoints.push_back(m_pPoints[meshsize + iUpNextCount - 1]);
                    hexpoints.push_back(m_pPoints[meshsize + iUpCount - 1]);
                    // The lower points are the smae as the points from the lower element.
                    hexpoints.push_back(m_pSlipPoints[(iCount - 1) - (mesh*meshsize) + iSlipCounter*meshsize]);
                    hexpoints.push_back(m_pSlipPoints[(iNextCount - 1) - (mesh*meshsize) + iSlipCounter*meshsize]);
                    hexpoints.push_back(m_pSlipPoints[(iUpNextCount - 1) - (mesh*meshsize) + iSlipCounter*meshsize]);
                    hexpoints.push_back(m_pSlipPoints[(iUpCount - 1) - (mesh*meshsize) + iSlipCounter*meshsize]);

                    // Search for body above and below.
                    assert(pFormation->VolumeSize() == 1);

                    int nrElements = pFormation->Elements();
                    const geo::IBody &upper = pFormation->Volume(0).Volume().Body((iNrHexPerLayer - 1)*nrElements + nrElements - 1);

                    CHexaHorizon *pHor = &pFormation->LowerHorizon();
                    nrElements = pHor->LowerFormation()->Elements();
                    const geo::IBody &lower = pHor->LowerFormation()->Volume(0).Volume().Body((iNrHexPerLayer - 1)*nrElements);

                    const geo::CBodyQuadrilateral& upperface = dynamic_cast<const geo::CBodyQuadrilateral&> (upper.Face(HEXA_LOWERFACE));
                    const geo::CBodyQuadrilateral& lowerface = dynamic_cast<const geo::CBodyQuadrilateral&> (lower.Face(HEXA_UPPERFACE));

                    // Add the interface element to the list.
                    geo::CInterfaceElement *ptr = new geo::CInterfaceElement(Mesh().m_mesh, &upperface, &lowerface, hexpoints);
                    pGroup->AddMeshElement(*ptr);

                    // Add the interface element to the map
                    VERIFY(Mesh().m_mpInterfaces.insert(std::make_pair(ptr, pHorizon)).second);

                    assert(pGroup == pHorizon->InterfaceElementGroup());
                    found = true;
                    break;
                  }
                }
                if (found) break;
              }
            }
            if (found) break;
          }
        }
      }
      iSlipCounter++;
    }

    // Next Formation.
    CHexaHorizon *pHor = &pFormation->LowerHorizon();
    pFormation = pHor->LowerFormation();
  } // Formation
}


bool CHexaMesher::OnGridLine(const geo::IPoint& pt1, const geo::IPoint& pt2) const
{
  if (fabs(pt1.X() - pt2.X()) < EPS || fabs(pt1.Y() - pt2.Y()) < EPS)
  { // the line is horizontal or vertical. could be exactly on a grid line

    CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(const_cast<CHexaMesher&>(*this).Model().GraphEntry(MD_HEXA_MESH_ZONE));
    CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

    CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion;
    for (itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); itMeshRegion++)
    {
      geo::CPoint Min = (*itMeshRegion)->Min();
      bool bOnHorGridLine1 = fabs(fmod(pt1.X() - Min.X() + EPS / 2, (*itMeshRegion)->GridSizeX())) < EPS;
      bool bOnVerGridLine1 = fabs(fmod(pt1.Y() - Min.Y() + EPS / 2, (*itMeshRegion)->GridSizeY())) < EPS;

      bool bOnHorGridLine2 = fabs(fmod(pt2.X() - Min.X() + EPS / 2, (*itMeshRegion)->GridSizeX())) < EPS;
      bool bOnVerGridLine2 = fabs(fmod(pt2.Y() - Min.Y() + EPS / 2, (*itMeshRegion)->GridSizeY())) < EPS;

      if ((bOnHorGridLine1 && bOnHorGridLine2 && bOnVerGridLine1 && bOnVerGridLine2))
      {

        return true;
      }
    }
  }
  return false;
}


void CHexaMesher::LoadPointIndices(std::vector<int>& vcIndex, CArchiveInterface& stream, IProgressBase& progress)
{
  assert(vcIndex.size() == 0);
  int nSize;
  stream >> nSize;
  vcIndex.resize(nSize);
  for (int i = 0; i < nSize; i++)
  {
    stream >> vcIndex[i];
    progress.Step();
  }
}


void CHexaMesher::SavePointIndices(const std::vector<int>& vcIndex, CArchiveInterface& stream, IProgressBase& progress)
{
  int nSize = vcIndex.size();
  stream << nSize;
  for (int i = 0; i < nSize; i++)
  {
    stream << vcIndex[i];
    progress.Step();
  }
}


void CHexaMesher::LoadStream(CArchiveInterface& stream, CStreamVersion& version, IProgressBase& progress)
{
  LoadPointIndices(m_pPoints, stream, progress);
  LoadPointIndices(m_pPointsInternal, stream, progress);
  LoadPointIndices(m_pSlipPoints, stream, progress);
}

void CHexaMesher::SaveStream(CArchiveInterface& stream, IProgressBase& progress)
{
  SavePointIndices(m_pPoints, stream, progress);
  SavePointIndices(m_pPointsInternal, stream, progress);
  SavePointIndices(m_pSlipPoints, stream, progress);
}

long CHexaMesher::SavedItems() const
{
  return m_pPoints.size() + m_pPointsInternal.size() + m_pSlipPoints.size();
}


