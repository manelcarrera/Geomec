// TetraMesh.h: interface for the CTetraMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRAMESH_H__3CED2035_A94C_46DF_B53D_8BD15BFADD29__INCLUDED_)
#define AFX_TETRAMESH_H__3CED2035_A94C_46DF_B53D_8BD15BFADD29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshBase.h"
#include "TetMeshBase.h"
#if !defined(_DEBUG) && defined(WIN32)
#define HAVE_CM2_MESHER
#include "TetMeshCM2.h"
#endif

//#define MESH_LOADED	350
class CTetraModel;
class CSurfaceBase;
class CShellGoCadFile;
class CTetraHorizonBase;
class CPressureSupportNode;
class CTetraSubHorizon;

namespace geo {
  class CInterfaceElement;
  class CSurfaceDesc;
  class CBodyTriangle;
}

namespace gm_dsf
{
class DSFIntermediateSurfaceGenerator;
}

namespace GeomecRGI
{
  class CLoadGridFromRGI;
}

#include "BodyGroup.h"
#include "GocadImport.h"
#include "GocadData.h"

class CTetraMesh : public CMeshBase
{
  friend class CGocadMeshImporter;
  friend class gm_dsf::DSFIntermediateSurfaceGenerator;
  friend class GeomecRGI::CLoadGridFromRGI;
  friend class CShellGoCadFile;

public:
  typedef CSingleCommandTemplate<CTetraMesh> TTetraMeshCommand;
  typedef enum eMesherType : int { FGV = 0, CM2 = 1 } eMesherType;
private:
  const CSurfaceBase* Surface(const geo::ISurface& surface);

  // The tetrahedron mesh object
  geo::CTetMeshBase *m_pMesh;
  // The mapping of surfaces sources 
  std::vector<COpenGLNode*> m_vcSurfaceSource;
  bool m_bCanInvalidateMesh;

  int AddSurface(const geo::ISurface& surface, const QString& sName, bool bSlip);
  CTetraHorizonBase* Horizon(const CSurfaceBase& base);
  void GenerateSupportsForSurfaceDesc(const geo::CSurfaceDesc &surfacedesc, const CPressureSupportNode &pnode, int iRefNode,
                                      const geo::IVector& vecNormal, const geo::IVector& vecTangent);

/*
  This map is udes for storing and loading the elements.
  expand this map when new element types are added to the mesh. Make sure the map is filled correctly
  in the constructor.
*/
  std::map<std::string, int> m_mpElementType;

  bool m_bBuildingFromGocad;
  double m_dMaxGocadSurfaceAngle; // in radians, when angles are larger surfaces will be split
  bool m_bImportedMesh; // which means it can not be regenerated after invalidation

#ifndef HAVE_CM2_MESHER
  eMesherType m_mesherType;
#endif

  bool SplitSurfaceNodes(geo::CSurfaceDesc& surfdesc);

  bool CombineSurfaceNodes(geo::CSurfaceDesc& surfdesc);

  bool GetFrontAndBack(const geo::CSurfaceDesc& surfdesc,
                       std::set<const geo::CBodyGroup*>& stFrontGroups,
                       std::set<const geo::CBodyGroup*>& stBackGroups,
                       std::set<const geo::CTetSurface*>& stFrontSurfaces,
                       std::set<const geo::CTetSurface*>& stBackSurfaces) const;

  void SeparateBodyGroups(const geo::CBodyGroup& currentgroup,
                          const std::set<const geo::CBodyGroup*>& stAdjacentBodyGroups,
                          std::set<const geo::CBodyGroup*>& stFrontGroups,
                          std::set<const geo::CBodyGroup*>& stBackGroups,
                          const std::set<const geo::CBodyGroup::CSideSurface*>& stFaultSurfaces) const;

public:
  // Construction of the mesher
  CTetraMesh(CTetraModel& model);
  virtual ~CTetraMesh();

  // Interface of element set
  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();
  virtual DIMENSION Dimension() const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;

#ifdef SKUA_NEW
  bool ImportMeshFromGoCadFile(const QString& string,
  bool(*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *>& solids,
      std::vector<int>& vcSolids));
  bool CreateMeshFromSolids(const QString& sPath,
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  const std::vector<int>& vcSolidIndices);
  bool CreateMeshFromSolids(const QString& sPath,
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  bool(*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *>& solids,
      std::vector<int>& vcSolids));
#else
  bool ImportMeshFromGoCadFile(	const QString& string,
                  bool (*selectGocadSolidDlg)(	const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
                                  std::vector<int>& vcSolids));
  bool CreateMeshFromSolids(const QString& sPath,
              std::vector <QSharedPointer <CGocadData::CTSolid> >& solids, 
              const std::vector<int>& vcSolidIndices);
  bool CreateMeshFromSolids(const QString& sPath,
              std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
              bool (*selectGocadSolidDlg)(	const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
                              std::vector<int>& vcSolids) );
#endif
  virtual const geo::IMesh& Mesh() const;
  virtual geo::IMesh& Mesh();

  // Save and load
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;
  
  // Fixed mesh ...
  virtual bool CanInvalidateMesh() const;

  // Meshing interface
  virtual bool IsMesh() const;
  virtual bool CanCreateMesh() const;
  virtual void CreateMesh();
  virtual void InvalidateMesh();

  eMesherType CurrentMesherType() const;
  std::string CurrentMesher() const;
  void SwitchMesher(std::string type);
  void SwitchMesher(eMesherType type);

  // Functions to retrieve the input surfaces. You can only access these functions when IsMesh()
  // is flagged. The input surface descriptor is returned with the graph where it is extracted from.
  // TetSurfaces (output) can accessed via the surface discriptor.
  virtual int InputSurfaceSize() const;
  typedef std::pair<const geo::CSurfaceDesc*, COpenGLNode*> TInputSurface;
  virtual TInputSurface InputSurface(int nIndex) const;

  // Retrieve adjacent graph nodes to a given volume
  TGraphNodeSet VolumeIdentity(const geo::CBodyGroup& volume) const;

  // Supports generation (currently for tetra models only)
  void GetModelOrientation(geo::IVector& vecNormal, geo::IVector& vecTangent);
  virtual void GenerateSupports();

  // get the 'fault' from a contact element
  virtual const CHorizonBase* SlipHorizon(const geo::IElement &element) const;

  void ExportInterfaces();

  const double& MaxGocadSurfaceAngle() const;
  void MaxGocadSurfaceAngle(const double& dValue);

  // see if the mesh was imported (can not be invalidated, can not be regenerated)
  virtual bool ImportedMesh() const;

  // returns false if the change is not allowed (e.g. isolated edges)
  bool OnHorizonSlipChanged(CTetraSubHorizon& hor);

  ACCEPT_GEOMECMODELVISITORS(VisitTetraMesh);

private:
  const geo::IFace *LoadBodyTriangle(TSTREAM& stream, CStreamVersion& version);
  void SaveBodyTriangle( const geo::IFace*, TSTREAM& stream );

  const geo::IFace *LoadTiedPointBodyTriangle(TSTREAM& stream, CStreamVersion& version);
  void SaveTiedBodyTriangle(const geo::IFace& face, TSTREAM& stream);

  void SaveInterfaceElement(TSTREAM &stream,TPROGRESS &progress,const geo::IElement &element);
  void LoadInterfaceElement(TSTREAM &stream,CStreamVersion& version,TPROGRESS &progress);
  void LoadContactElement(TSTREAM &stream,TPROGRESS &progress);

  void SaveTyings(TSTREAM& stream, TPROGRESS& progress);
  void LoadTyings(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
};

#endif // !defined(AFX_TETRAMESH_H__3CED2035_A94C_46DF_B53D_8BD15BFADD29__INCLUDED_)
