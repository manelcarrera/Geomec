#include "HexaMetaMesh.h"

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
#include "Triangle.h"

#include <algorithm>



namespace hmm
{


///////////////////////////////////////////////////////////////////////////////
// CXYLess (copy of IHexaMesh for now)
///////////////////////////////////////////////////////////////////////////////

class CXYLess
{
public:
  bool operator()(const geo::IPoint &P1, const geo::IPoint &P2) const
  {
    if (P1.Y() - P2.Y() < -EPS)
      return true;

    if (P1.Y() - P2.Y() > EPS)
      return false;

    if (P1.X() - P2.X() < -EPS)
      return true;

    return false;
  }
};



///////////////////////////////////////////////////////////////////////////////
// DefinitionLayer
///////////////////////////////////////////////////////////////////////////////

class DefinitionLayer
{
public:
  std::map<int, int>           regions;
  std::vector<geo::CPoint>     points;
  std::vector<std::set<int> >  mask;
  CHexaFormation              *formation;
};


///////////////////////////////////////////////////////////////////////////////
// Layer
///////////////////////////////////////////////////////////////////////////////

class Layer
{
public:
  int                 definition;
  std::set<int>       regions;
  std::vector<double> depths;
  std::vector<int>    points;
  std::vector<int>    marks;
  std::vector<int>    elts;

  Layer(int def);
  Layer(int def, std::vector<double>& depths);

  bool main_layer() const;
  bool main_node(const DefinitionLayer& def, int index) const;
  bool active(const DefinitionLayer& def, int index) const;
  bool active(const DefinitionLayer& def, int index, const std::set<int>& mask) const;
  bool active(const DefinitionLayer& def, int index, int mask) const;
  bool find(CRotatedSystem& rs, const DefinitionLayer& def, int& start, const geo::CPoint& local_point_0, geo::CPoint& point_1, int axis, int mask);
  bool findr(CRotatedSystem& rs, const DefinitionLayer& def, int& start, const geo::CPoint& local_point_0, geo::CPoint& point_1, int axis, int region, bool only_main = false);

  bool marked(int index) const;
  void mark(int index);
};

Layer::Layer(int def)
  : definition(def)
{
}

Layer::Layer(int def, std::vector<double>& depths)
  : definition(def)
  , depths(depths)
{
}


bool Layer::main_layer() const
{
  return regions.find(0) != regions.end();
}

bool Layer::main_node(const DefinitionLayer& def, int index) const
{
  return main_layer() && def.mask[index].find(0) != def.mask[index].end();
}

bool Layer::active(const DefinitionLayer& def, int index) const
{
  for (std::set<int>::iterator it = regions.begin(); it != regions.end(); ++it)
  {
    if (def.mask[index].find(*it) != def.mask[index].end())
      return true;
  }
  return false;
}

bool Layer::active(const DefinitionLayer& def, int index, const std::set<int>& mask) const
{
  if (active(def, index))
  {
    for (std::set<int>::iterator it = mask.begin(); it != mask.end(); ++it)
    {
      if (*it == -1)
        continue;
      if (def.mask[index].find(*it) != def.mask[index].end())
        return true;
    }
  }
  return false;
}

bool Layer::active(const DefinitionLayer& def, int index, int mask) const
{
  return active(def, index) && def.mask[index].find(mask) != def.mask[index].end();
}

bool Layer::find(CRotatedSystem& rs, const DefinitionLayer& def, int& start, const geo::CPoint& local_point_0, geo::CPoint& point_1, int axis, int mask)
{
  while (start < depths.size() && !active(def, start, def.mask[mask]))
    ++start;

  if (start == depths.size())
    return false;

  point_1.Set(def.points[start].X(), def.points[start].Y(), depths[start]);
  rs.ToLocalSet(point_1);

  switch (axis)
  {
  case 0:
    return fabs(local_point_0.X() - point_1.X()) < EPS;
  case 1:
    return fabs(local_point_0.Y() - point_1.Y()) < EPS;
  }

  return true;
}

bool Layer::findr(CRotatedSystem& rs, const DefinitionLayer& def, int& start, const geo::CPoint& local_point_0, geo::CPoint& point_1, int axis, int region, bool only_main)
{
  bool found = false;

  while (!found)
  {
    while (start < depths.size() && (only_main ? def.mask[start].find(0) == def.mask[start].end() : !active(def, start, region)))
      ++start;

    if (start == depths.size())
      break;

    point_1.Set(def.points[start].X(), def.points[start].Y(), depths[start]);
    rs.ToLocalSet(point_1);

    switch (axis)
    {
    case 0:
      found = fabs(local_point_0.X() - point_1.X()) < EPS;
      break;
    case 1:
      found = fabs(local_point_0.Y() - point_1.Y()) < EPS;
      break;
    }
    if (!found)
      ++start;
  }

  return found;
}

bool Layer::marked(int index) const
{
  return marks[index] > 0;
}

void Layer::mark(int index)
{
  ++marks[index];
}


///////////////////////////////////////////////////////////////////////////////
// Quadrilateral
///////////////////////////////////////////////////////////////////////////////

class Quadrilateral
{
public:
  int i[4];
  double iso[4];
  bool find_horizontal_rectangle(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, int index, bool only_main = false);
  bool find_vertical_square_x(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, geo::CPoint& pp_local, int index, int nMasterRegion);
  bool find_vertical_square_y(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, geo::CPoint& pp_local, int index, int nMasterRegion, double gridSizeY);

  void get_iso_horizontal2d(const geo::CPoint& pp_local);
  void get_iso_horizontal3d(const geo::CPoint& pp_local);
  void get_iso_vertical(const geo::CPoint& pp_local);
private:
  geo::CPoint p[4];
};


bool Quadrilateral::find_horizontal_rectangle(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, int index, bool only_main)
{
  double prev_area = DBL_MAX;

  if (layer.active(def, index) || only_main)
  {
    int size = layer.depths.size();

    int _i[4];
    geo::CPoint _p[4];

    _i[0] = index;
    _p[0].Set(def.points[index].X(), def.points[index].Y(), layer.depths[index]);
    rs.ToLocalSet(_p[0]);

    for (std::set<int>::const_iterator region = def.mask[index].begin(); region != def.mask[index].end(); ++region)
    {
      if (only_main && *region != 0)
        continue;

      _i[1] = _i[0] + 1;
      if (!layer.findr(rs, def, _i[1], _p[0], _p[1], 1, *region, only_main))
        continue;

      _i[2] = _i[1] + 1;
      if (!layer.findr(rs, def, _i[2], _p[0], _p[2], 0, *region, only_main))
        continue;

      _i[3] = _i[2] + 1;
      if (!layer.findr(rs, def, _i[3], _p[2], _p[3], 1, *region, only_main))
        continue;

      // test for rectangle
      double len_x_l = _p[1].X() - _p[0].X();
      double len_x_r = _p[3].X() - _p[2].X();

      if (fabs(len_x_l - len_x_r) > EPS)
        continue;

      double len_y_d = _p[2].Y() - _p[0].Y();
      double len_y_u = _p[3].Y() - _p[1].Y();

      if (fabs(len_y_d - len_y_u) > EPS)
        continue;

      bool marked = false;
      for (int k = 0; k < 4; ++k)
      {
        if (layer.marked(_i[k]))
        {
          marked = true;
          break;
        }
      }
      if (marked)
        continue;

      double area = len_x_l * len_y_d;

      // TODO: test for inside region

      if (area < prev_area)
      {
        for (int k = 0; k < 4; ++k)
        {
          i[k] = _i[k];
          p[k].Set(_p[k].X(), _p[k].Y(), _p[k].Z());
        }
        prev_area = area;
      }
    }
  }

  return prev_area < DBL_MAX;
}


bool Quadrilateral::find_vertical_square_x(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, geo::CPoint& pp_local, int index, int nMasterRegion)
{
  for (int k = index - 1; k >= 0; --k)
  {
    if (!top.active(def, k, nMasterRegion))
      continue;

    //if (!top.main_node(def, k))
    //  continue;

    if (top.marked(k))
      continue;

    if (bot.marked(k))
      continue;

    i[0] = k;

    p[0].Set(def.points[k].X(), def.points[k].Y(), top.depths[k]);
    rs.ToLocalSet(p[0]);

    if (fabs(pp_local.Y() - p[0].Y()) < EPS)
    {
      for (int l = index + 1; l < top.depths.size(); ++l)
      {
        if (!top.active(def, l, nMasterRegion))
          continue;

        //if (!top.main_node(def, l))
        //  continue;
        if (top.marked(l))
          continue;

        if (bot.marked(l))
          continue;

        i[1] = l;
        p[1].Set(def.points[l].X(), def.points[l].Y(), top.depths[l]);
        rs.ToLocalSet(p[1]);

        if ((pp_local.X() + EPS > p[0].X()) && (pp_local.X() < p[1].X() + EPS))
        {

          i[2] = k;
          p[2].Set(def.points[k].X(), def.points[k].Y(), bot.depths[k]);
          rs.ToLocalSet(p[2]);

          i[3] = l;
          p[3].Set(def.points[l].X(), def.points[l].Y(), bot.depths[l]);
          rs.ToLocalSet(p[3]);

          return true;
        }
      }
    }
    // We have past the search point so exit the loop.
    if (p[0].Y() > pp_local.Y() + EPS) break;
  }

  return false;
}

bool Quadrilateral::find_vertical_square_y(CRotatedSystem& rs, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, geo::CPoint& pp_local, int index, int nMasterRegion, double gridSizeY)
{
  for (int k = index - 1; k >= 0; --k)
  {
    if (!top.active(def, k, nMasterRegion))
      continue;

    //if (!top.main_node(def, k))
    //  continue;

    if (top.marked(k))
      continue;

    if (bot.marked(k))
      continue;

    i[0] = k;

    p[0].Set(def.points[k].X(), def.points[k].Y(), top.depths[k]);
    rs.ToLocalSet(p[0]);

    if (fabs(pp_local.X() - p[0].X()) < EPS) //&& ((pp_local.Y() - p[0].Y()) < gridSizeY + EPS))
    {
      for (int l = index + 1; l < top.depths.size(); ++l)
      {
        if (!top.active(def, l, nMasterRegion))
          continue;

        //if (!top.main_node(def, l))
        //  continue;

        if (top.marked(l))
          continue;

        if (bot.marked(l))
          continue;

        i[1] = l;
        p[1].Set(def.points[l].X(), def.points[l].Y(), top.depths[l]);
        rs.ToLocalSet(p[1]);

        if (fabs(pp_local.X() - p[1].X()) < EPS) //&& ((p[1].Y() - pp_local.Y()) < gridSizeY + EPS))
        {

          i[2] = k;
          p[2].Set(def.points[k].X(), def.points[k].Y(), bot.depths[k]);
          rs.ToLocalSet(p[2]);

          i[3] = l;
          p[3].Set(def.points[l].X(), def.points[l].Y(), bot.depths[l]);
          rs.ToLocalSet(p[3]);

          return true;
        }
      }
    }
    // We have past the search point so exit the loop.
    if (p[0].Y() > pp_local.Y() + EPS) break;
  }

  return false;
}


void Quadrilateral::get_iso_horizontal2d(const geo::CPoint& pp_local)
{
  geo::CVector dir_h = p[2] - p[0];
  geo::CVector dir_v = p[1] - p[0];

  geo::CVector v = pp_local - p[0];

  dir_h.Z(0);
  dir_v.Z(0);
  v.Z(0);

  double dot_h = v.DotProduct(dir_h) / dir_h.SquareLength();
  double dot_v = v.DotProduct(dir_v) / dir_v.SquareLength();

  iso[0] = (1 - dot_h) * (1 - dot_v);
  iso[1] = (1 - dot_h) * dot_v;
  iso[2] = dot_h * (1 - dot_v);
  iso[3] = dot_h * dot_v;
}

void Quadrilateral::get_iso_horizontal3d(const geo::CPoint& pp_local)
{
  geo::CVector dir_h = p[2] - p[0];
  geo::CVector dir_v = p[1] - p[0];

  geo::CVector v = pp_local - p[0];

  double dot_h = v.DotProduct(dir_h) / dir_h.SquareLength();
  double dot_v = v.DotProduct(dir_v) / dir_v.SquareLength();

  iso[0] = (1 - dot_h) * (1 - dot_v);
  iso[1] = (1 - dot_h) * dot_v;
  iso[2] = dot_h * (1 - dot_v);
  iso[3] = dot_h * dot_v;
}

void Quadrilateral::get_iso_vertical(const geo::CPoint& pp_local)
{
  geo::CVector dir_h = p[3] - p[2];
  geo::CVector dir_v0 = p[0] - p[2];
  geo::CVector dir_v1 = p[1] - p[3];


  geo::CVector v0 = pp_local - p[2];
  geo::CVector v1 = pp_local - p[3];

  double dot_h = v0.DotProduct(dir_h) / dir_h.SquareLength();
  double dot_v0 = v0.DotProduct(dir_v0) / dir_v0.SquareLength();
  double dot_v1 = v1.DotProduct(dir_v1) / dir_v1.SquareLength();

  double dot_v = dot_h * dot_v0 + (1 - dot_h) * dot_v1;

  iso[0] = (1 - dot_h) * dot_v;
  iso[1] = dot_h * dot_v;
  iso[2] = (1 - dot_h) * (1 - dot_v);
  iso[3] = dot_h * (1 - dot_v);
}




///////////////////////////////////////////////////////////////////////////////
// CHexaMetaMesh
///////////////////////////////////////////////////////////////////////////////

CHexaMetaMesh::CHexaMetaMesh(CHexaMesh& mesh)
  : m_mesh(mesh)
{
}

CHexaMetaMesh::~CHexaMetaMesh()
{
  clear();
}

CModelBase& CHexaMetaMesh::Model()
{
  return static_cast<CModelBase&>(m_mesh.Model());
}

CHexaMesh& CHexaMetaMesh::Mesh()
{
  return m_mesh;
}

void CHexaMetaMesh::clear()
{
  m_regions.clear();

  for (std::vector<DefinitionLayer *>::iterator it = defs.begin(); it != defs.end(); ++it)
    delete *it;
  defs.clear();
  for (std::vector<Layer *>::iterator it = layers.begin(); it != layers.end(); ++it)
    delete *it;
  layers.clear();
  mpCreatedTyings.clear();
}


void CHexaMetaMesh::CreateHexaMetaMesh(IProgressBase& progress)
{
  progress.NextJob("Process mesh regions");
  GetRegions(progress);\
  progress.NextJob("Process formations");
  LoopOverFormations(progress, &CreateLayerDefinition);
  progress.NextJob("Process main region depths");
  LoopOverFormations(progress, &CreateLayer);

  bool changeDepths = false;
  for (size_t i = 1; i < Regions().size(); ++i)
  {
    if (!static_cast<CHexaSubMeshRegion *>(Regions()[i])->FullModel())
    {
      changeDepths = true;
      break;
    }
  }

  if (changeDepths)
  {
    progress.NextJob("Process sub region depths");
    LoopOverLayers(progress, &FillSubRegionLayerDepths);
  }

  // disable for checkin; this whole BuildMetaMesh is only gathering data yet, so could be called without impact (other than using space-time)
  //_MetaMesh_LoopOverLayers(&CHexaMesher::_MetaMesh_PrintLayer);

  // TODO:
  // there are duplicate layers now, one for the formation up, one for the formation down
  // if it's a fault, we need both, otherwise we need one (STRAIGHT FORWARD, but we may want a flag)
  progress.NextJob("Register nodes");
  LoopOverLayers(progress, &RegisterNodes);

  // after that, the active nodes need to be registered in the mesh (STRAIGHT FORWARD)
  // and then we need to build hexas (STRAIGHT FORWARD -- use the old routine)

  progress.NextJob("Create hexahedron elements");
  LoopOverLayers(progress, &CreateHexahedrons);

  // at that point we can test how it looks
  progress.NextJob("Create interface elements");
  LoopOverLayers(progress, &CreateInterfaceElements);

  // mark nodes that will be slaves
  progress.NextJob("Analyze tyings");
  LoopOverLayers(progress, &MarkTyings);

  // if it looks okay, we need the tyings -- that should also be relatively straight forward
  progress.NextJob("Create tyings");
  LoopOverLayers(progress, &CreateTyings);

}


void CHexaMetaMesh::GetRegions(IProgressBase& progress)
{
  progress.AddSteps(3);

  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  // clean up: get rid of invalid regions
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); ++itMeshRegion)
  {
    if ((*itMeshRegion)->Hull().NrOfPoints() < 4)
      delete *itMeshRegion;
  }
  stNode = pEntry->EntryNodes();

  m_regions.reserve(stNode.size());

  progress.Step();

  // place main mesh region up front
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); ++itMeshRegion)
  {
    if (dynamic_cast<CHexaMainMeshRegion *>(*itMeshRegion))
    {
      m_regions.push_back(*itMeshRegion);
      break;
    }
  }
  progress.Step();
  // then the sub mesh regions
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); ++itMeshRegion)
  {
    if (dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion))
      m_regions.push_back(*itMeshRegion);
  }
  progress.Step();
}


void CHexaMetaMesh::CreateLayerDefinition(IProgressBase& progress, CHexaMetaMesh& metaMesh, int /*index*/, CHexaFormation& formation)
{
  typedef std::pair<geo::CPoint, std::set<int> > TPointMaskPair;
  typedef std::map<geo::CPoint, TPointMaskPair, CXYLess> TPointMap;

  TPointMap pointMap;

  std::vector<CHexaMeshRegionBase *> vcRegsInCurrentReg;

  geo::CArray<geo::CPoint> pointszone;

  const CModelBase& model = static_cast<const CModelBase&>(formation.Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  for (std::vector<CHexaMeshRegionBase *>::iterator itMeshRegion = metaMesh.m_regions.begin(); itMeshRegion != metaMesh.m_regions.end(); ++itMeshRegion)
  {
    int mask = std::distance(metaMesh.m_regions.begin(), itMeshRegion);

    CHexaSubMeshRegion *subRegion = dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion);

    if (subRegion && subRegion->Formation(formation) == 0)
    {
      mask = -1;
    }


    geo::CPoint ptMeshRegionMin = rs.LocalMin((*itMeshRegion)->Hull());
    geo::CPoint ptMeshRegionMax = rs.LocalMax((*itMeshRegion)->Hull());
    vcRegsInCurrentReg.clear();
    if (dynamic_cast<CHexaSubMeshRegion*>(*itMeshRegion))
    {
      // only when current meshregion is not the main mesh region.
      for (std::vector<CHexaMeshRegionBase *>::iterator itRegion = metaMesh.m_regions.begin(); itRegion != metaMesh.m_regions.end(); ++itRegion)
      {
        if (*itRegion == *itMeshRegion)
          continue;

        geo::CPoint ptRegionMin = rs.LocalMin((*itRegion)->Hull());
        geo::CPoint ptRegionMax = rs.LocalMax((*itRegion)->Hull());
        bool bInside = (ptRegionMin.X() > ptMeshRegionMin.X() && ptRegionMin.Y() > ptMeshRegionMin.Y() &&
          ptRegionMax.X() < ptMeshRegionMax.X() && ptRegionMax.Y() < ptMeshRegionMax.Y());

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
      for (geo::CArray<geo::CPoint>::iterator PtIt = pointszone.begin(); PtIt != pointszone.end(); ++PtIt)
      {
        TPointMap::iterator it = pointMap.insert(TPointMap::value_type(rs.ToLocal(*PtIt), std::make_pair(*PtIt, std::set<int>()))).first;
        it->second.second.insert(mask);
      }
    }
    else
    {
      for (geo::CArray<geo::CPoint>::iterator PtIt = pointszone.begin(); PtIt != pointszone.end(); ++PtIt)
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
        {
          TPointMap::iterator it = pointMap.insert(TPointMap::value_type(rs.ToLocal(*PtIt), std::make_pair(*PtIt, std::set<int>()))).first;
          it->second.second.insert(mask);
        }
      }
    }
  }

  // Copy all the sorted points in the CArray.

  metaMesh.defs.push_back(new DefinitionLayer);

  DefinitionLayer& def = *metaMesh.defs.back();
  def.formation = &formation;
  def.points.reserve(pointMap.size());
  def.mask.reserve(pointMap.size());

  int formation_elts = formation.Elements();

  for (TPointMap::iterator copyPoint = pointMap.begin(); copyPoint != pointMap.end(); ++copyPoint)
  {
    geo::CPoint& point = copyPoint->second.first;

    def.points.push_back(point);
    def.mask.push_back(copyPoint->second.second);

    for (std::set<int>::iterator it = copyPoint->second.second.begin(); it != copyPoint->second.second.end(); ++it)
    {
      if (*it > 0)
      {
        CHexaSubMeshRegion *region = static_cast<CHexaSubMeshRegion *>(metaMesh.m_regions[*it]);

        int region_elts = region->Formation(formation);
        int factor = 1;
        while ((formation_elts * factor) < region_elts)
          factor <<= 1;

        def.regions.insert(std::make_pair(*it, factor));
      }
    }
  }

}

void CHexaMetaMesh::CreateLayer(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index, CHexaFormation& formation)
{
  DefinitionLayer& def = *metaMesh.defs[index];

  // fill top and bottom depths
  std::vector<double> tops(def.points.size(), 0);
  std::vector<double> bots(def.points.size(), 0);

  if (index > 0)
    std::copy(metaMesh.layers.back()->depths.begin(), metaMesh.layers.back()->depths.end(), tops.begin());
  else
    metaMesh.FillMainRegionLayerDepths(progress, def, formation.UpperHorizon(), tops);

  metaMesh.FillMainRegionLayerDepths(progress, def, formation.LowerHorizon(), bots);

  // correct for minimum thickness
  double minThickness = formation.MinThickness().Value();

  for (size_t i = 0; i < tops.size(); ++i)
  {
    if (bots[i] - tops[i] < minThickness)
      bots[i] = tops[i] + minThickness;
  }


  // create layers
  int formation_elts = formation.Elements();
  int factor = 1;

  for (std::map<int, int>::iterator it = def.regions.begin(); it != def.regions.end(); ++it)
    if (it->second > factor)
      factor = it->second;

  int max_elts = formation_elts * factor;

  for (int i = 0; i <= max_elts; ++i)
  {
    metaMesh.layers.push_back(new Layer(index));

    Layer& layer = *metaMesh.layers.back();

    layer.depths.resize(tops.size(), 0);
    layer.marks.resize(tops.size(), 0);

    for (int j = 0; j < tops.size(); ++j)
    {
      layer.depths[j] = tops[j] + (bots[j] - tops[j]) * i / max_elts;
    }

    bool main_layer = (i % factor) == 0;
    if (main_layer)
      layer.regions.insert(0);
    for (std::map<int, int>::iterator it = def.regions.begin(); it != def.regions.end(); ++it)
      if (main_layer || (i % it->second) != 0)
        layer.regions.insert(it->first);
  }
}

void CHexaMetaMesh::FillMainRegionLayerDepths(IProgressBase& progress, DefinitionLayer& def, CHexaHorizon& horizon, std::vector<double>& depths)
{
  if (horizon.ConstantDepth())
    std::fill(depths.begin(), depths.end(), horizon.Depth().Value());
  else if (horizon.SurfaceSize() == 1)
  {
    for (size_t i = 0; i < def.points.size(); ++i)
      depths[i] = horizon.Surface(0).InterpolateDepth(def.points[i], true).Value();
  }
  else
  {
    geo::CValue dValue;
    for (size_t i = 0; i < def.points.size(); ++i)
    {
      int j = 0;
      while ((j < horizon.SurfaceSize()) && !dValue.Valid())
      {
        dValue = horizon.Surface(i).InterpolateDepth(def.points[i], false);
        ++j;
      }
      double dMinDistance = DBL_MAX;
      j = 0;
      while (j < horizon.SurfaceSize())
      {
        const geo::CPoint nearest_point = horizon.Surface(0).Surface().NearestXYPoint(def.points[i]);
        geo::CPoint diff = nearest_point - def.points[i];
        double dDistance = diff.X() * diff.X() + diff.Y() * diff.Y();
        if (dDistance < dMinDistance)
        {
          dValue = geo::CValue(nearest_point.Z());
          dMinDistance = dDistance;
        }
        ++j;
      }
      depths[i] = dValue.Value();
    }
  }
}

void CHexaMetaMesh::FillSubRegionLayerDepths(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{
  Layer& layer = *metaMesh.layers[index];
  DefinitionLayer& def = *metaMesh.defs[layer.definition];

  CModelBase& model = static_cast<CModelBase&>(metaMesh.Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());
  geo::CVector Xaxis = rs.ToGlobal(geo::CVector::Xaxis);
  geo::CVector Yaxis = rs.ToGlobal(geo::CVector::Yaxis);
  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  geo::CPoint point, pp_local, local_0;
  Quadrilateral square;

  int size = layer.depths.size();

  for (int i = 0; i < size; ++i)
  {
    if (def.mask[i].find(0) == def.mask[i].end())
    {
      point.Set(def.points[i].X(), def.points[i].Y(), layer.depths[i]);

      pp_local.Set(point.X(), point.Y(), point.Z());
      rs.ToLocalSet(pp_local);

      int k = 0;
      for (k = i - 1; k >= 0; --k)
      {
        if (def.mask[k].find(0) != def.mask[k].end())
        {
          local_0.Set(def.points[k].X(), def.points[k].Y(), layer.depths[k]);
          rs.ToLocalSet(local_0);

          if (local_0.X() < pp_local.X() + EPS && local_0.Y() < pp_local.Y() + EPS)
            break;
        }
      }
      if (k >= 0)
      {
        if (square.find_horizontal_rectangle(rs, def, layer, k, true))
        {
          square.get_iso_horizontal2d(pp_local);

          double interpolated_z = 0;

          for (int pointIndex = 0; pointIndex < 4; ++pointIndex)
            interpolated_z += layer.depths[square.i[pointIndex]] * square.iso[pointIndex];

          if (fabs(layer.depths[i] - interpolated_z) > EPS)
            layer.depths[i] = interpolated_z;
        }
      }
    }
  }
}

void CHexaMetaMesh::LoopOverFormations(IProgressBase& progress, void (*func)(IProgressBase& progress, CHexaMetaMesh&, int, CHexaFormation&))
{
  CHexaFormation *pFormation = &(Mesh().GetTopFormation());

  int i = 0;

  progress.AddSteps(Model().GraphEntry(MD_BASE_FORMATION)->GraphEntryNodes().size());

  while (pFormation)
  {
    func(progress, *this, i++, *pFormation);

    CHexaHorizon *pHorizon = &pFormation->LowerHorizon();
    pFormation = pHorizon->LowerFormation();

    progress.Step();
  }
}


void CHexaMetaMesh::PrintLayer(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{
  Layer& layer = *metaMesh.layers[index];
  DefinitionLayer& def = *metaMesh.defs[layer.definition];

  FILE *fp = fopen("E:\\Temp\\dump.txt", "a");
  fprintf(fp, "DEF %d\n", layer.definition);
  for (int i = 0; i < layer.depths.size(); ++i)
  {
    bool on = false;
    fprintf(fp, "%d\t%f\t%f\t%f\t(", i, def.points[i].X(), def.points[i].Y(), layer.depths[i]);
    for (std::set<int>::iterator it = layer.regions.begin(); it != layer.regions.end(); ++it)
    {
      if (it == layer.regions.begin())
        fprintf(fp, "%d", *it);
      else
        fprintf(fp, " %d", *it);
    }
    fprintf(fp, ")\t%s\t%s\n", layer.active(def, i) ? "ON" : "OFF", layer.main_node(def, i) ? "MAIN" : "TIED");
  }
  fprintf(fp, "\n");
  fclose(fp);
}

void CHexaMetaMesh::RegisterNodes(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{
  Layer& layer = *metaMesh.layers[index];
  DefinitionLayer& def = *metaMesh.defs[layer.definition];

  int size = layer.depths.size();
  layer.points.resize(size, -1);

  if (index > 0 && layer.definition != metaMesh.layers[index - 1]->definition && !def.formation->UpperHorizon().Slip())
    std::copy(metaMesh.layers[index - 1]->points.begin(), metaMesh.layers[index - 1]->points.end(), layer.points.begin());

  geo::CPoint point;
  for (int i = 0; i < size; ++i)
  {
    if (layer.points[i] == -1 && layer.active(def, i))
    {
      point.Set(def.points[i].X(), def.points[i].Y(), layer.depths[i]);
      layer.points[i] = metaMesh.Mesh().m_mesh.RegisterNode(point, false);
    }
  }

}





void CHexaMetaMesh::CreateHexahedrons(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{

  if (index == metaMesh.layers.size() - 1)
    return;

  Layer& top = *metaMesh.layers[index];
  Layer& bt = *metaMesh.layers[index + 1];

  if (top.definition != bt.definition)
    return;

  bool topLayer = index == 0 || metaMesh.layers[index - 1]->definition != top.definition;

  DefinitionLayer& def = *metaMesh.defs[top.definition];

  CHexaModel *pModel = dynamic_cast<CHexaModel*>(&metaMesh.Model());
  CHexaFormation *pFormation = def.formation;
  geo::CBodyGroup *pGroup = 0;

  bool topSlip = pFormation->UpperHorizon().Slip();
  bool botSlip = pFormation->LowerHorizon().Slip();

  if (pFormation->VolumeSize() == 1)
    pGroup = &pFormation->Volume(0).Volume();

  if (!pGroup)
  {
    pGroup = new geo::CBodyGroup(metaMesh.Mesh().m_mesh);
    new geo::CBodyGroup::CSideSurface(*pGroup);	// SideSurface 0 (top)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 1 (bottom)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 2 (front)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 3 (back)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 4 (left)
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 5 (right)
    pFormation->Volume(0).Volume(*pGroup);
  }

  CRotatedSystem rs(pModel->Boundary().BoxAzimuth(), pModel->Boundary().Box().MidPoint());

  geo::CPoint ptLocalMin = rs.LocalMin(pModel->Boundary().Box());
  geo::CPoint ptLocalMax = rs.LocalMax(pModel->Boundary().Box());

  //FILE *fp = fopen("E:\\Temp\\dump.txt", "a");

  Quadrilateral square;

  int i0, i1, i2, i3;

  int size = top.depths.size();

  std::vector<int> points(8, 0);

  for (i0 = 0; i0 < size; ++i0)
  {
    if (square.find_horizontal_rectangle(rs, def, top, i0))
    {
      //fprintf(fp, "HEXA\t%d\t%d\t%d\t%d\t%d\n", index, i0, i1, i2, i3);

      i1 = square.i[1];
      i2 = square.i[2];
      i3 = square.i[3];

      int n = 0;

      std::set<int> one, two, three;

      std::set_intersection(def.mask[i0].begin(), def.mask[i0].end(), def.mask[i1].begin(), def.mask[i1].end(), std::inserter(one, one.end()));
      std::set_intersection(def.mask[i2].begin(), def.mask[i2].end(), def.mask[i3].begin(), def.mask[i3].end(), std::inserter(two, two.end()));

      std::set_intersection(one.begin(), one.end(), two.begin(), two.end(), std::inserter(three, three.end()));

      double minGrid = metaMesh.m_regions[0]->GridSizeX();
      if (metaMesh.m_regions[0]->GridSizeY() < minGrid)
        minGrid = metaMesh.m_regions[0]->GridSizeY();

      for (std::set<int>::iterator it = three.begin(); it != three.end(); ++it)
      {
        if (*it < 0)
          continue;
        double tmpMin = metaMesh.m_regions[*it]->GridSizeX() < metaMesh.m_regions[*it]->GridSizeY() ? metaMesh.m_regions[*it]->GridSizeX() : metaMesh.m_regions[*it]->GridSizeY();
        if (tmpMin < minGrid)
        {
          minGrid = tmpMin;
          n = *it;
        }
      }

      Layer *bot = 0;
      int t = index + 1;
      while (!bot)
      {
        Layer *tt = metaMesh.layers[t++];
        if (tt->regions.find(n) != tt->regions.end())
          bot = tt;
      }

      bool botLayer = t == metaMesh.layers.size() || metaMesh.layers[t]->definition != top.definition;
      bool unique = !(topLayer || botLayer);

      points[0] = top.points[i0];
      points[1] = top.points[i1];
      points[2] = top.points[i3];
      points[3] = top.points[i2];
      points[4] = bot->points[i0];
      points[5] = bot->points[i1];
      points[6] = bot->points[i3];
      points[7] = bot->points[i2];

      geo::CHexahedron *hex = new geo::CHexahedron(metaMesh.Mesh().Mesh(), points);

      if (topLayer && topSlip)
      {
        if (top.elts.size() == 0)
          top.elts.resize(top.depths.size(), -1);
        top.elts[i0] = pFormation->Volume(0).Volume().BodySize();
      }

      if (botLayer && botSlip)
      {
        if (bot->elts.size() == 0)
          bot->elts.resize(bot->depths.size(), -1);
        bot->elts[i0] = pFormation->Volume(0).Volume().BodySize();
      }

      // debug: double v = hex->Size();
      pFormation->Volume(0).Volume().AddBody(*hex);

      // Add the face of the upper hexahedron of the formation to side surface nr. 0 (Upper surface)
      if (topLayer)
      {
        // Face to upper surface of formation ...
        pFormation->Volume(0).Volume().SideSurface(TOP).AddFace(hex->Face(HEXA_UPPERFACE));
      }

      // Add the face of the lower hexahedron of the formation to side surface nr. 1 (Lower surface)
      if (botLayer)
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
  }

  //fclose(fp);

}


void CHexaMetaMesh::CreateInterfaceElements(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{

  if (index == 0 || index == metaMesh.layers.size() - 1)
    return;

  Layer& top = *metaMesh.layers[index];
  Layer& bot = *metaMesh.layers[index + 1];

  if (top.definition == bot.definition)
    return;

  DefinitionLayer& topDef = *metaMesh.defs[top.definition];
  DefinitionLayer& botDef = *metaMesh.defs[bot.definition];

  CHexaHorizon& topHorizon = topDef.formation->LowerHorizon();

  if (!topHorizon.Slip())
    return;

  geo::CElementGroup *pGroup = new geo::CElementGroup(metaMesh.Mesh().m_mesh);
  topHorizon.InterfaceElementGroup(*pGroup);


  CRotatedSystem rs(metaMesh.Model().Boundary().BoxAzimuth(), metaMesh.Model().Boundary().Box().MidPoint());

  int i0, i1, i2, i3;

  int size = top.depths.size();

  std::vector<int> points(8, 0);

  Quadrilateral square;

  int elt = 0;

  for (i0 = 0; i0 < size; ++i0)
  {
    if (square.find_horizontal_rectangle(rs, topDef, top, i0))
    {
      i1 = square.i[1];
      i2 = square.i[2];
      i3 = square.i[3];

      points[0] = top.points[i0];
      points[1] = top.points[i1];
      points[2] = top.points[i3];
      points[3] = top.points[i2];
      points[4] = bot.points[i0];
      points[5] = bot.points[i1];
      points[6] = bot.points[i3];
      points[7] = bot.points[i2];

      const geo::IBody &topHex = topDef.formation->Volume(0).Volume().Body(top.elts[i0]);
      const geo::IBody &botHex = botDef.formation->Volume(0).Volume().Body(bot.elts[i0]);

      const geo::CBodyQuadrilateral& topFace = static_cast<const geo::CBodyQuadrilateral&>(topHex.Face(HEXA_LOWERFACE));
      const geo::CBodyQuadrilateral& botFace = static_cast<const geo::CBodyQuadrilateral&>(botHex.Face(HEXA_UPPERFACE));

      geo::CInterfaceElement *ifElt = new geo::CInterfaceElement(metaMesh.Mesh().m_mesh, &topFace, &botFace, points);
      pGroup->AddMeshElement(*ifElt);

      VERIFY(metaMesh.Mesh().m_mpInterfaces.insert(std::make_pair(ifElt, &topHorizon)).second);

      assert(pGroup == topHorizon.InterfaceElementGroup());
    }
  }
}



void CHexaMetaMesh::local_Tyings_FindMasterRegion(const geo::IPoint& point, std::set<int>& regions, int itMeshRegion, int& nDanglingType, int& itMasterRegion, bool check_region)
{
  std::vector<int> vcRegions;
  vcRegions.reserve(20);

  itMasterRegion = -1;

  for (std::set<int>::iterator itTempMeshReg = regions.begin(); itTempMeshReg != regions.end(); itTempMeshReg++)
  { // find the regions that contain the point (excluding the edge)
    if (*itTempMeshReg == itMeshRegion) continue;

    if (m_regions[*itTempMeshReg]->InsideRegion(point, false))
    {
      vcRegions.push_back(*itTempMeshReg);
    }
  }

  double dMinGrid = 0;
  double dTmpMinGrid = 0;
  for (size_t k = 0; k < vcRegions.size(); k++)
  { // pMasterRegion should be the region with the smallest grid size (either in X or in Y direction)
    double dXGrid = m_regions[k]->GridSizeX();
    double dYGrid = m_regions[k]->GridSizeY();

    dTmpMinGrid = (dXGrid < dYGrid) ? dXGrid : dYGrid;
    if (dMinGrid == 0)
    {
      dMinGrid = dTmpMinGrid;
      itMasterRegion = vcRegions[k];
      continue;
    }

    if (dTmpMinGrid < dMinGrid)
    {
      dMinGrid = dTmpMinGrid;
      itMasterRegion = vcRegions[k];
    }
  }

  // Is the node dangling in the pMasterRegion?
  if (itMasterRegion >= 0) nDanglingType = m_regions[check_region ? itMasterRegion : 0]->IsDangling(*m_regions[itMeshRegion], point, check_region);
}


void CHexaMetaMesh::Tyings_CreateTying(const geo::IVector& Xaxis, const geo::IVector& Yaxis, int index, int nMaster1, int nMaster2, bool do_z, double calcdist)
{
  if (mpCreatedTyings.find(index) == mpCreatedTyings.end())
  {
    std::vector<TTyingInfo> v(do_z ? 6 : 4);
    v[0].axis = 0;
    v[0].master = nMaster1;
    v[0].distance = 1 - calcdist;
    v[1].axis = 0;
    v[1].master = nMaster2;
    v[1].distance = calcdist;
    v[2].axis = 1;
    v[2].master = nMaster1;
    v[2].distance = 1 - calcdist;
    v[3].axis = 1;
    v[3].master = nMaster2;
    v[3].distance = calcdist;

    Mesh().m_mesh.CreateTying(index, Xaxis, geo::CTying::TR,
      nMaster1, Xaxis, geo::CTying::TR, 1 - calcdist,
      nMaster2, Xaxis, geo::CTying::TR, calcdist);
    Mesh().m_mesh.CreateTying(index, Yaxis, geo::CTying::TR,
      nMaster1, Yaxis, geo::CTying::TR, 1 - calcdist,
      nMaster2, Yaxis, geo::CTying::TR, calcdist);
    if (do_z)
    {
      v[4].axis = 2;
      v[4].master = nMaster1;
      v[4].distance = 1 - calcdist;
      v[5].axis = 2;
      v[5].master = nMaster2;
      v[5].distance = calcdist;

      Mesh().m_mesh.CreateTying(index, geo::CVector::Zaxis, geo::CTying::TR,
        nMaster1, geo::CVector::Zaxis, geo::CTying::TR, 1 - calcdist,
        nMaster2, geo::CVector::Zaxis, geo::CTying::TR, calcdist);
    }

    mpCreatedTyings.insert(std::make_pair(index, v));
  }
}


void CHexaMetaMesh::Tyings_HandleX(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nMasterRegion, bool do_z, bool only_main)
{
  // Search for masternodes at the x-axis.
  geo::CPoint local_0, local_1;

  for (int k = i_local - 1; k >= 0; --k)
  {
    if (!layer.active(def, k, nMasterRegion))
      continue;

    if (only_main && !layer.main_node(def, k))
      continue;

    if (layer.marked(k))
      continue;

    local_0.Set(def.points[k].X(), def.points[k].Y(), layer.depths[k]);
    rs.ToLocalSet(local_0);

    if (fabs(pp_local.Y() - local_0.Y()) < EPS)
    {
      for (int l = i_local + 1; l < layer.depths.size(); ++l)
      {
        if (!layer.active(def, l, nMasterRegion))
          continue;

        if (only_main && !layer.main_node(def, l))
          continue;

        if (layer.marked(l))
          continue;

        local_1.Set(def.points[l].X(), def.points[l].Y(), layer.depths[l]);
        rs.ToLocalSet(local_1);

        if ((pp_local.X() + EPS > local_0.X()) && (pp_local.X() < local_1.X() + EPS))
        {
          double dist = local_1.X() - local_0.X();
          double calcdist = (pp_local.X() - local_0.X()) / dist;

          // Tying i_local -> k l
          Tyings_CreateTying(Xaxis, Yaxis, layer.points[i_local], layer.points[k], layer.points[l], do_z, calcdist);

          //_Tyings_CreateForPoints(info, rs, pp_local, false, info.vcPoints, info.mpPoint, k, k + 1, calcdist);
          //_Tyings_CreateForPoints(info, rs, pp_local, true, m_pPointsInternal, info.mpInternalPoint, k, k + 1, calcdist);

          return;
        }
      }
    }
    // We have past the search point so exit the loop.
    if (local_0.Y() > pp_local.Y() + EPS) break;
  }
}

void CHexaMetaMesh::Tyings_HandleY(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nMasterRegion, bool do_z, bool only_main)
{
  geo::CPoint local_0, local_1;

  // Search for masternodes at the y-axis.
  for (int k = i_local - 1; k >= 0; --k)
  {
    if (!layer.active(def, k, nMasterRegion))
      continue;

    if (only_main && !layer.main_node(def, k))
      continue;

    if (layer.marked(k))
      continue;

    local_0.Set(def.points[k].X(), def.points[k].Y(), layer.depths[k]);
    rs.ToLocalSet(local_0);

    if ((fabs(pp_local.X() - local_0.X()) < EPS) && ((pp_local.Y() - local_0.Y()) < m_regions[nMasterRegion]->GridSizeY()))
    {
      for (int l = i_local + 1; l < layer.depths.size(); ++l)
      {
        if (!layer.active(def, l, nMasterRegion))
          continue;

        if (only_main && !layer.main_node(def, l))
          continue;

        if (layer.marked(l))
          continue;

        local_1.Set(def.points[l].X(), def.points[l].Y(), layer.depths[l]);
        rs.ToLocalSet(local_1);

        if ((fabs(pp_local.X() - local_1.X()) < EPS) && ((local_1.Y() - pp_local.Y()) < m_regions[nMasterRegion]->GridSizeY()))
        {
          double dist = local_1.Y() - local_0.Y();
          double calcdist = (pp_local.Y() - local_0.Y()) / dist;

          Tyings_CreateTying(Xaxis, Yaxis, layer.points[i_local], layer.points[k], layer.points[l], do_z, calcdist);
          // Tying i_local -> k l
          //_Tyings_CreateForPoints(info, rs, pp_local, false, info.vcPoints, info.mpPoint, k, l, calcdist);
          //_Tyings_CreateForPoints(info, rs, pp_local, true, m_pPointsInternal, info.mpInternalPoint, k, l, calcdist);

          return;
        }
      }
    }
    // We have past the search point so exit the loop.
    if (local_0.Y() > pp_local.Y() + EPS) break;
  }
}

void CHexaMetaMesh::Tyings_HandleZ(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nDanglingType, int nMasterRegion)
{
  if (mpCreatedTyings.find(layer.points[i_local]) != mpCreatedTyings.end())
    return;

  geo::CPoint local_0, local_1, local_2, local_3;

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  if (nDanglingType == MESH_NO_DANG)
  {
    local_0.Set(def.points[i_local].X(), def.points[i_local].Y(), top.depths[i_local]);
    rs.ToLocalSet(local_0);

    local_1.Set(def.points[i_local].X(), def.points[i_local].Y(), bot.depths[i_local]);
    rs.ToLocalSet(local_1);

    double dist = local_1.Z() - local_0.Z();
    double calcdist = (pp_local.Z() - local_0.Z()) / dist;

    Tyings_CreateTying(Xaxis, Yaxis, layer.points[i_local], top.points[i_local], bot.points[i_local], true, calcdist);
  }
  else if (nDanglingType == MESH_HOR_DANG)
  {
    Quadrilateral square;
    if (square.find_vertical_square_x(rs, def, layer, top, bot, pp_local, i_local, nMasterRegion))
    {
      int nodes[4];
      double isoPt[4];
      std::vector<geo::CTying::TMasterDef> vcMasters(4);

      nodes[0] = bot.points[square.i[2]];
      nodes[1] = bot.points[square.i[3]];
      nodes[2] = top.points[square.i[1]];
      nodes[3] = top.points[square.i[0]];

      square.get_iso_vertical(pp_local);

      isoPt[0] = square.iso[2];
      isoPt[1] = square.iso[3];
      isoPt[2] = square.iso[1];
      isoPt[3] = square.iso[0];

      std::vector<TTyingInfo> v(12);
      int v_i = 0;

      for (int axisIndex = 0; axisIndex < 3; ++axisIndex) // three axes
      {
        for (int pointIndex = 0; pointIndex < 4; ++pointIndex) // isoparametric factor for three points
        {
          v[v_i].axis = axisIndex;
          v[v_i].master = nodes[pointIndex];
          v[v_i].distance = isoPt[axisIndex];
          ++v_i;

          vcMasters[pointIndex].direction = &rsNormal[axisIndex];
          vcMasters[pointIndex].node = nodes[pointIndex];
          vcMasters[pointIndex].type = geo::CTying::TR;
          vcMasters[pointIndex].factor = isoPt[pointIndex]; //isoPt[axisIndex];
        }
        Mesh().m_mesh.CreateTying(layer.points[i_local], rsNormal[axisIndex], geo::CTying::TR, vcMasters);
      }

      mpCreatedTyings.insert(std::make_pair(layer.points[i_local], v));

    }
  }
  else if (nDanglingType == MESH_VERT_DANG)
  {
    Quadrilateral square;
    if (square.find_vertical_square_y(rs, def, layer, top, bot, pp_local, i_local, nMasterRegion, m_regions[nMasterRegion]->GridSizeY()))
    {
      int nodes[4];
      double isoPt[4];

      std::vector<geo::CTying::TMasterDef> vcMasters(4);

      nodes[0] = bot.points[square.i[2]];
      nodes[1] = bot.points[square.i[3]];
      nodes[2] = top.points[square.i[1]];
      nodes[3] = top.points[square.i[0]];

      square.get_iso_vertical(pp_local);

      isoPt[0] = square.iso[2];
      isoPt[1] = square.iso[3];
      isoPt[2] = square.iso[1];
      isoPt[3] = square.iso[0];

      std::vector<TTyingInfo> v(12);
      int v_i = 0;

      for (int axisIndex = 0; axisIndex < 3; ++axisIndex) // three axes
      {
        for (int pointIndex = 0; pointIndex < 4; ++pointIndex) // isoparametric factor for three points
        {
          v[v_i].axis = axisIndex;
          v[v_i].master = nodes[pointIndex];
          v[v_i].distance = isoPt[axisIndex];
          ++v_i;

          vcMasters[pointIndex].direction = &rsNormal[axisIndex];
          vcMasters[pointIndex].node = nodes[pointIndex];
          vcMasters[pointIndex].type = geo::CTying::TR;
          vcMasters[pointIndex].factor = isoPt[pointIndex]; //isoPt[axisIndex];
        }
        Mesh().m_mesh.CreateTying(layer.points[i_local], rsNormal[axisIndex], geo::CTying::TR, vcMasters);
      }
      mpCreatedTyings.insert(std::make_pair(layer.points[i_local], v));
    }
  }
}

void CHexaMetaMesh::Tyings_HandleXY(DefinitionLayer& def, Layer& layer, DefinitionLayer& topDef, Layer& topLayer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local)
{
  if (mpCreatedTyings.find(layer.points[i_local]) != mpCreatedTyings.end())
    return;

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  geo::CPoint local_0;
  int k = 0;
  for (k = i_local - 1; k >= 0; --k)
  {
    if (topLayer.active(topDef, k) && !layer.marked(k)/*&& topLayer.main_node(topDef, k)*/)
    {
      local_0.Set(def.points[k].X(), def.points[k].Y(), layer.depths[k]);
      rs.ToLocalSet(local_0);

      if (local_0.X() < pp_local.X() + EPS && local_0.Y() < pp_local.Y() + EPS)
        break;
    }
  }
  if (k >= 0)
  {
    Quadrilateral square;
    if (square.find_horizontal_rectangle(rs, topDef, topLayer, k, false))//true))
    {

      int nodes[4];
      std::vector<geo::CTying::TMasterDef> vcMasters(4);
      std::vector<double> isoPt(4, 0);

      nodes[0] = topLayer.points[square.i[0]];
      nodes[1] = topLayer.points[square.i[2]];
      nodes[2] = topLayer.points[square.i[3]];
      nodes[3] = topLayer.points[square.i[1]];

      square.get_iso_horizontal3d(pp_local);

      isoPt[0] = square.iso[0];
      isoPt[1] = square.iso[2];
      isoPt[2] = square.iso[3];
      isoPt[3] = square.iso[1];

      std::vector<TTyingInfo> v(12);
      int v_i = 0;

      for (int axisIndex = 0; axisIndex < 3; ++axisIndex) // three axes
      {
        for (int pointIndex = 0; pointIndex < 4; ++pointIndex) // isoparametric factor for three points
        {
          v[v_i].axis = axisIndex;
          v[v_i].master = nodes[pointIndex];
          v[v_i].distance = isoPt[axisIndex];
          ++v_i;

          vcMasters[pointIndex].direction = &rsNormal[axisIndex];
          vcMasters[pointIndex].node = nodes[pointIndex];
          vcMasters[pointIndex].type = geo::CTying::TR;
          vcMasters[pointIndex].factor = isoPt[pointIndex]; //isoPt[axisIndex];
        }
        Mesh().m_mesh.CreateTying(layer.points[i_local], rsNormal[axisIndex], geo::CTying::TR, vcMasters);
      }
      mpCreatedTyings.insert(std::make_pair(layer.points[i_local], v));

    }
  }
}


Layer *CHexaMetaMesh::find_top_layer(int layer, geo::CPoint& point, int i, std::set<int>& dregions, std::set<int>& lregions, int& nDanglingType, int& nMasterRegion)
{
  nMasterRegion = -1;
  for (int l = layer - 1; l >= 0; --l)
  {
    Layer& top = *layers[l];

    nDanglingType = MESH_NO_DANG;
    local_Tyings_FindMasterRegion(point, top.regions, i, nDanglingType, nMasterRegion, true);

    if (nMasterRegion != -1)
      //if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
      return &top;
  }

  return 0;
}

Layer *CHexaMetaMesh::find_bot_layer(int layer, int nMasterRegion)
{
  if (nMasterRegion >= 0)
  {
    for (int l = layer + 1; l < layers.size(); ++l)
    {
      Layer& bot = *layers[l];
      if (bot.regions.find(nMasterRegion) != bot.regions.end())
        return &bot;
    }
  }

  return 0;
}

bool same_masks(const std::set<int>& lhs, const std::set<int>& rhs)
{
  for (std::set<int>::const_iterator it = lhs.begin(); it != lhs.end(); ++it)
  {
    if (*it == -1)
      continue;
    if (rhs.find(*it) == rhs.end())
      return false;
  }
  for (std::set<int>::const_iterator it = rhs.begin(); it != rhs.end(); ++it)
  {
    if (*it == -1)
      continue;
    if (lhs.find(*it) == lhs.end())
      return false;
  }
  return true;
}

void CHexaMetaMesh::MarkTyings(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{
  Layer& layer = *metaMesh.layers[index];
  DefinitionLayer& def = *metaMesh.defs[layer.definition];

  bool not_last = index < metaMesh.layers.size() - 1;

  // get a rotated X and Y axis
  CModelBase& model = static_cast<CModelBase&>(metaMesh.Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());
  geo::CVector Xaxis = rs.ToGlobal(geo::CVector::Xaxis);
  geo::CVector Yaxis = rs.ToGlobal(geo::CVector::Yaxis);
  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same
  int psize = layer.points.size();

  geo::CPoint point;

  Layer *topLayer = 0, *botLayer = 0;
  DefinitionLayer *topDef = 0, *botDef = 0;

  bool isTopLayer = index > 0 && layer.definition != metaMesh.layers[index - 1]->definition;
  bool isBotLayer = index < metaMesh.layers.size() - 1 && layer.definition != metaMesh.layers[index + 1]->definition;

  if (isTopLayer)
  {
    topLayer = metaMesh.layers[index - 1];
    topDef = metaMesh.defs[topLayer->definition];
  }
  if (isBotLayer)
  {
    botLayer = metaMesh.layers[index + 1];
    botDef = metaMesh.defs[botLayer->definition];
  }
  // Loop over all the meshregions.
  for (int i = 0; i < psize; ++i)
  {
    if (layer.active(def, i) && !layer.main_node(def, i))
    {
      point = metaMesh.Mesh().m_mesh.Point(layer.points[i]);

      bool switch_region = false;
      if (isTopLayer)
      {
        bool t0 = !topLayer->active(*topDef, i);
        bool t1 = false; // !topLayer->main_node(*topDef, i);
        bool t2 = false; // !same_masks(topDef->mask[i], def.mask[i]); // topDef->mask[i] != def.mask[i];

        if (t0 || t1 || t2)
          switch_region = true;
      }
      if (isBotLayer)
      {
        bool t0 = !botLayer->active(*botDef, i);
        bool t1 = false; // !botLayer->main_node(*botDef, i);
        bool t2 = false; // !same_masks(botDef->mask[i], def.mask[i]); // botDef->mask[i] != def.mask[i];

        if (t0 || t1 || t2)
          switch_region = true;
      }

      if (!layer.main_layer())
      {
        int nDanglingType = MESH_NO_DANG, nMasterRegion;

        int region = -1;
        int count = 0;

        bool skip = false;

        double minRegion = DBL_MAX;

        for (std::set<int>::iterator reg = def.mask[i].begin(); reg != def.mask[i].end(); ++reg)
        {
          if (*reg < 0)
            continue;

          if (layer.regions.find(*reg) == layer.regions.end())
            continue;
#if 0
          if (!m_metaMesh.regions[*reg]->InsideRegion(point, false))
            region = *reg;
          if (layer.active(def, i, *reg))
            ++count;
#else
          if (layer.active(def, i, *reg) && metaMesh.m_regions[*reg]->InsideRegion(point, true))
          {
            if (!metaMesh.m_regions[*reg]->InsideRegion(point, false))
            {
              double min = metaMesh.m_regions[*reg]->GridSizeX() < metaMesh.m_regions[*reg]->GridSizeY() ? metaMesh.m_regions[*reg]->GridSizeX() : metaMesh.m_regions[*reg]->GridSizeY();

              if (min < minRegion)
              {
                region = *reg;
                minRegion = min;
              }
              ++count;
            }
            else
              skip = true;
          }
#endif

        }

        if (/*count == 1 && */!skip && region >= 0)
        {
          Layer *top = metaMesh.find_top_layer(index, point, region, def.mask[i], layer.regions, nDanglingType, nMasterRegion);
          Layer *bot = metaMesh.find_bot_layer(index, nMasterRegion);

          geo::CPoint pp_local = rs.ToLocal(point);

          if (layer.main_layer() && nMasterRegion > 0 && nMasterRegion != region && metaMesh.m_regions[nMasterRegion]->InsideRegion(point, false))
          {
            if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
            {
              layer.mark(i);
            }
          }
          else
          {
            layer.mark(i);
          }
        }
      }
      else
      {
        for (std::set<int>::iterator it = layer.regions.begin(); it != layer.regions.end(); ++it)
        {
          int nDanglingType = MESH_NO_DANG, nMasterRegion;
          metaMesh.local_Tyings_FindMasterRegion(point, layer.regions, *it, nDanglingType, nMasterRegion, !switch_region);


          if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
          {
            //geo::CPoint pp_local = rs.ToLocal(point);

            //info.basicpoints = &m_metaMesh.regions[nMasterRegion]->GetMeshRegion();
            //bool do_z = (topLayer && !topLayer->active(*topDef, i)) || (botLayer && !botLayer->active(*botDef, i));

            layer.mark(i);
          }
          else
          {
            if (topLayer) // && (!topLayer->active(*topDef, i) /* || !topLayer->main_node(*topDef, i) || topDef->mask[i] != def.mask[i] */))
            {
              bool t0 = !topLayer->active(*topDef, i);
              bool t1 = false; // !topLayer->main_node(*topDef, i);
              bool t2 = false; // !same_masks(topDef->mask[i], def.mask[i]); // topDef->mask[i] != def.mask[i];

              if (t0 || t1 || t2)
              {
                nDanglingType = MESH_NO_DANG;
                metaMesh.local_Tyings_FindMasterRegion(point, topLayer->regions, *it, nDanglingType, nMasterRegion, false);

                if (nDanglingType > MESH_NO_DANG && nMasterRegion >= 0)
                {
                  layer.mark(i);
                }
              }

            }
            if (botLayer) //&& (!botLayer->active(*botDef, i) /* || !botLayer->main_node(*botDef, i) || botDef->mask[i] != def.mask[i] */))
            {
              bool t0 = !botLayer->active(*botDef, i);
              bool t1 = false; // !botLayer->main_node(*botDef, i);
              bool t2 = false; // !same_masks(botDef->mask[i], def.mask[i]); // botDef->mask[i] != def.mask[i];

              if (t0 || t1 || t2)
              {
                nDanglingType = MESH_NO_DANG;
                metaMesh.local_Tyings_FindMasterRegion(point, botLayer->regions, *it, nDanglingType, nMasterRegion, false);

                if (nDanglingType > MESH_NO_DANG && nMasterRegion >= 0)
                {
                  layer.mark(i);
                }
              }

            }
          }
        }
      }
    }
  }
}


void CHexaMetaMesh::CreateTyings(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index)
{
  Layer& layer = *metaMesh.layers[index];
  DefinitionLayer& def = *metaMesh.defs[layer.definition];

  bool not_last = index < metaMesh.layers.size() - 1;

  // get a rotated X and Y axis
  CModelBase& model = static_cast<CModelBase&>(metaMesh.Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());
  geo::CVector Xaxis = rs.ToGlobal(geo::CVector::Xaxis);
  geo::CVector Yaxis = rs.ToGlobal(geo::CVector::Yaxis);
  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same
  int psize = layer.points.size();

  geo::CPoint point;

  Layer *topLayer = 0, *botLayer = 0;
  DefinitionLayer *topDef = 0, *botDef = 0;

  bool isTopLayer = index > 0 && layer.definition != metaMesh.layers[index - 1]->definition;
  bool isBotLayer = index < metaMesh.layers.size() - 1 && layer.definition != metaMesh.layers[index + 1]->definition;

  if (isTopLayer)
  {
    topLayer = metaMesh.layers[index - 1];
    topDef = metaMesh.defs[topLayer->definition];
  }
  if (isBotLayer)
  {
    botLayer = metaMesh.layers[index + 1];
    botDef = metaMesh.defs[botLayer->definition];
  }
  // Loop over all the meshregions.
  for (int i = 0; i < psize; ++i)
  {
    if (layer.active(def, i) && !layer.main_node(def, i))
    {
      point = metaMesh.Mesh().m_mesh.Point(layer.points[i]);

      bool switch_region = false;
      if (isTopLayer)
      {
        bool t0 = !topLayer->active(*topDef, i);
        bool t1 = false; // !topLayer->main_node(*topDef, i);
        bool t2 = false; // !same_masks(topDef->mask[i], def.mask[i]); // topDef->mask[i] != def.mask[i];

        if (t0 || t1 || t2)
          switch_region = true;
      }
      if (isBotLayer)
      {
        bool t0 = !botLayer->active(*botDef, i);
        bool t1 = false; // !botLayer->main_node(*botDef, i);
        bool t2 = false; // !same_masks(botDef->mask[i], def.mask[i]); // botDef->mask[i] != def.mask[i];

        if (t0 || t1 || t2)
          switch_region = true;
      }

      if (!layer.main_layer())
      {
        int nDanglingType = MESH_NO_DANG, nMasterRegion;

        int region = -1;
        int count = 0;

        bool skip = false;

        double minRegion = DBL_MAX;

        for (std::set<int>::iterator reg = def.mask[i].begin(); reg != def.mask[i].end(); ++reg)
        {
          if (*reg < 0)
            continue;

          if (layer.regions.find(*reg) == layer.regions.end())
            continue;
#if 0
          if (!m_metaMesh.regions[*reg]->InsideRegion(point, false))
            region = *reg;
          if (layer.active(def, i, *reg))
            ++count;
#else
          if (layer.active(def, i, *reg) && metaMesh.m_regions[*reg]->InsideRegion(point, true))
          {
            if (!metaMesh.m_regions[*reg]->InsideRegion(point, false))
            {
              double min = metaMesh.m_regions[*reg]->GridSizeX() < metaMesh.m_regions[*reg]->GridSizeY() ? metaMesh.m_regions[*reg]->GridSizeX() : metaMesh.m_regions[*reg]->GridSizeY();

              if (min < minRegion)
              {
                region = *reg;
                minRegion = min;
              }
              ++count;
            }
            else
              skip = true;
          }
#endif

        }

        if (/*count == 1 && */!skip && region >= 0)
        {
          Layer *top = metaMesh.find_top_layer(index, point, region, def.mask[i], layer.regions, nDanglingType, nMasterRegion);
          Layer *bot = metaMesh.find_bot_layer(index, nMasterRegion);

          geo::CPoint pp_local = rs.ToLocal(point);

          if (layer.main_layer() && nMasterRegion > 0 && nMasterRegion != region && metaMesh.m_regions[nMasterRegion]->InsideRegion(point, false))
          {
            if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
            {
              if (nDanglingType == MESH_HOR_DANG)
              {
                metaMesh.Tyings_HandleX(Xaxis, Yaxis, def, layer, rs, pp_local, i, nMasterRegion, false, false);
              }
              else // nDanglingType == MESH_VER_DANG
              {
                metaMesh.Tyings_HandleY(Xaxis, Yaxis, def, layer, rs, pp_local, i, nMasterRegion, false, false);
              }
            }
          }
          else
          {
            metaMesh.Tyings_HandleZ(Xaxis, Yaxis, def, layer, *top, *bot, rs, pp_local, i, nDanglingType, nMasterRegion);
          }
        }
      }
      else
      {
        for (std::set<int>::iterator it = layer.regions.begin(); it != layer.regions.end(); ++it)
        {
          int nDanglingType = MESH_NO_DANG, nMasterRegion;
          metaMesh.local_Tyings_FindMasterRegion(point, layer.regions, *it, nDanglingType, nMasterRegion, !switch_region);


          if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
          {
            geo::CPoint pp_local = rs.ToLocal(point);

            //info.basicpoints = &m_metaMesh.regions[nMasterRegion]->GetMeshRegion();
            //bool do_z = (topLayer && !topLayer->active(*topDef, i)) || (botLayer && !botLayer->active(*botDef, i));

            if (nDanglingType == MESH_HOR_DANG)
            {
              metaMesh.Tyings_HandleX(Xaxis, Yaxis, def, layer, rs, pp_local, i, nMasterRegion, switch_region || not_last, false);
            }
            else // nDanglingType == MESH_VER_DANG
            {
              metaMesh.Tyings_HandleY(Xaxis, Yaxis, def, layer, rs, pp_local, i, nMasterRegion, switch_region || not_last, false);
            }
          }
          else
          {
            if (topLayer) // && (!topLayer->active(*topDef, i) /* || !topLayer->main_node(*topDef, i) || topDef->mask[i] != def.mask[i] */))
            {
              bool t0 = !topLayer->active(*topDef, i);
              bool t1 = false; // !topLayer->main_node(*topDef, i);
              bool t2 = false; // !same_masks(topDef->mask[i], def.mask[i]); // topDef->mask[i] != def.mask[i];

              if (t0 || t1 || t2)
              {
                nDanglingType = MESH_NO_DANG;
                metaMesh.local_Tyings_FindMasterRegion(point, topLayer->regions, *it, nDanglingType, nMasterRegion, false);

                if (nDanglingType > MESH_NO_DANG && nMasterRegion >= 0)
                {
                  geo::CPoint pp_local = rs.ToLocal(point);
                  switch (nDanglingType)
                  {
                  case MESH_VERT_DANG:
                    metaMesh.Tyings_HandleY(Xaxis, Yaxis, def, layer, rs, pp_local, i, 0, true, false);
                    break;
                  case MESH_HOR_DANG:
                    metaMesh.Tyings_HandleX(Xaxis, Yaxis, def, layer, rs, pp_local, i, 0, true, false);
                    break;
                  case MESH_NOT_ON_GRID:
                    metaMesh.Tyings_HandleXY(def, layer, *topDef, *topLayer, rs, pp_local, i);
                    break;
                  }
                }
              }

            }
            if (botLayer) //&& (!botLayer->active(*botDef, i) /* || !botLayer->main_node(*botDef, i) || botDef->mask[i] != def.mask[i] */))
            {
              bool t0 = !botLayer->active(*botDef, i);
              bool t1 = false; // !botLayer->main_node(*botDef, i);
              bool t2 = false; // !same_masks(botDef->mask[i], def.mask[i]); // botDef->mask[i] != def.mask[i];

              if (t0 || t1 || t2)
              {
                nDanglingType = MESH_NO_DANG;
                metaMesh.local_Tyings_FindMasterRegion(point, botLayer->regions, *it, nDanglingType, nMasterRegion, false);

                if (nDanglingType > MESH_NO_DANG && nMasterRegion >= 0)
                {
                  geo::CPoint pp_local = rs.ToLocal(point);
                  switch (nDanglingType)
                  {
                  case MESH_VERT_DANG:
                    metaMesh.Tyings_HandleY(Xaxis, Yaxis, def, layer, rs, pp_local, i, 0, true, false);
                    break;
                  case MESH_HOR_DANG:
                    metaMesh.Tyings_HandleX(Xaxis, Yaxis, def, layer, rs, pp_local, i, 0, true, false);
                    break;
                  case MESH_NOT_ON_GRID:
                    metaMesh.Tyings_HandleXY(def, layer, *botDef, *botLayer, rs, pp_local, i);
                    break;
                  }
                }
              }

            }
            /*
            if (botLayer && !botLayer->active(*botDef, i))
            {
            _MetaMesh_local_Tyings_FindMasterRegion(point, botLayer->regions, *it, nDanglingType, nMasterRegion);

            if (nDanglingType != MESH_NO_DANG && nDanglingType != MESH_NOT_ON_GRID)
            {
            geo::CPoint pp_local = rs.ToLocal(point);

            if (nDanglingType == MESH_HOR_DANG)
            _MetaMesh_Tyings_HandleX(Xaxis, Yaxis, *botDef, *botLayer, rs, pp_local, i, nMasterRegion, true);
            else // nDanglingType == MESH_VER_DANG
            _MetaMesh_Tyings_HandleY(Xaxis, Yaxis, *botDef, *botLayer, rs, pp_local, i, nMasterRegion, true);
            }
            }
            */
          }
        }
      }
    }
  }
}

void CHexaMetaMesh::LoopOverLayers(IProgressBase& progress, void (*func)(IProgressBase& progress, CHexaMetaMesh&, int))
{
  progress.AddSteps(layers.size());
  for (int i = 0; i < layers.size(); ++i)
  {
    func(progress, *this, i);
    progress.Step();
  }
}




// META todo: we need to call the check that figures out whether the defined regions make sense
// or we will crash; I don't think we need to crash, but that's an extension of functionality

// META todo: revise the load/save/etc


}