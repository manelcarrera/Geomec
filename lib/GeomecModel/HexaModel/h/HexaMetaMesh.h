#pragma once

#include <vector>
#include <map>
#include <set>

class CHexaFormation;
class CHexaHorizon;
class CHexaMesherNG;
class CRotatedSystem;
class IProgressBase;
class CModelBase;
class CHexaMesh;
class CHexaMeshRegionBase;


#include "Point.h"

namespace geo
{
class IVector;
}



namespace hmm
{

typedef struct {
  int axis;
  int master;
  double distance;
} TTyingInfo;
typedef std::vector<TTyingInfo> TMasterVec;
typedef std::map<int, TMasterVec> TTyingMap;

class DefinitionLayer;
class Layer;

class CHexaMetaMesh
{

  std::vector<CHexaMeshRegionBase *> m_regions;
  std::vector<DefinitionLayer *>  defs;
  std::vector<Layer *>            layers;

  TTyingMap mpCreatedTyings;

  CHexaMesh& m_mesh;

public:
  CHexaMetaMesh(CHexaMesh& mesh);
  ~CHexaMetaMesh();

  void clear();

  std::vector<CHexaMeshRegionBase *>& Regions() { return m_regions; }
  TTyingMap& CreatedTyings() { return mpCreatedTyings; }

  void CreateHexaMetaMesh(IProgressBase& progress);

private:

  CModelBase& Model();
  CHexaMesh& Mesh();



  void GetRegions(IProgressBase& progress);

  static void CreateLayerDefinition(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index, CHexaFormation& formation);
  static void CreateLayer(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index, CHexaFormation& formation);
  
  void LoopOverFormations(IProgressBase& progress, void (*func)(IProgressBase& progress, CHexaMetaMesh&, int, CHexaFormation&));

  void FillMainRegionLayerDepths(IProgressBase& progress, DefinitionLayer& def, CHexaHorizon& horizon, std::vector<double>& depths);

  static void FillSubRegionLayerDepths(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void PrintLayer(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void RegisterNodes(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void CreateHexahedrons(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void CreateInterfaceElements(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void MarkTyings(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);
  static void CreateTyings(IProgressBase& progress, CHexaMetaMesh& metaMesh, int index);

  void LoopOverLayers(IProgressBase& progress, void (*func)(IProgressBase& progress, CHexaMetaMesh&, int));

  void local_Tyings_FindMasterRegion(const geo::IPoint& point, std::set<int>& regions, int itMeshRegion, int& nDanglingType, int& itMasterRegion, bool check_region);
  void Tyings_CreateTying(const geo::IVector& Xaxis, const geo::IVector& Yaxis, int index, int master1, int master2, bool force_z, double calcdist);
  void Tyings_HandleX(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nMasterRegion, bool do_z, bool only_main);
  void Tyings_HandleY(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nMasterRegion, bool do_z, bool only_main);
  void Tyings_HandleZ(const geo::IVector& Xaxis, const geo::IVector& Yaxis, DefinitionLayer& def, Layer& layer, Layer& top, Layer& bot, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local, int nDanglingType, int nMasterRegion);
  void Tyings_HandleXY(DefinitionLayer& def, Layer& layer, DefinitionLayer& topDef, Layer& topLayer, CRotatedSystem& rs, geo::CPoint& pp_local, int i_local);
  Layer* find_top_layer(int layer, geo::CPoint& point, int i, std::set<int>& dregions, std::set<int>& lregions, int& nDanglingType, int& nMasterRegion);
  Layer* find_bot_layer(int layer, int nMasterRegion);

};










}
