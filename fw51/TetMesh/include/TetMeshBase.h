/* Copyright (c) 2017 DIANA FEA BV                              Confidential */
#ifndef _TETMESH_BASE_H_
#define _TETMESH_BASE_H_

#include "IMesh3D.h"

#include "TetMeshExports.h"

namespace geo {

class CSurfaceDesc;
class CTetSurface;
class CContactElementSet;
class ISurface;
class IFace;

/*!
\class CTetMeshProgress
\brief The progress of the mesher indicated by this class.
The client can override functions to obtain the progress information.
*/
class TETMESH_EXPORT CTetMeshProgress
{
public:
  CTetMeshProgress();
  virtual void StartMesh(int nJobs);
  virtual void NewJob(std::string sJobName);
  virtual void StopMesh();
};

class TETMESH_EXPORT CTetMeshBase : public CMesh3D
{
public:
  typedef std::pair<std::vector<std::string>, bool> TRet;
protected:
  std::vector<CSurfaceDesc*> m_vcSurfaceDesc;
  std::vector<CBodyGroup*> m_vcVolume;
  CTetMeshBase();
  static const geo::INode& NearestNode(const IPoint& point,
  const IFace* face);
private:
  typedef std::vector<CTetSurface*> TTetSurfaceVec;
  typedef std::map<int, TTetSurfaceVec> TTetSurfaceMap;
  friend class CTetSurface;
  friend class CSurfaceDesc;
  // Input
  double m_dMeshTolerance;
  // Output
  int m_nBodySize;
  int m_nContactSize;
  int m_nInterfaceSize;

  void CopyPoint(const IPoint& point, double* pPoint) const;
  bool AddSurfaceToMesh(void* pMesh, const ISurface& surface, int nSurfaceId, const std::string& sName, bool bSlip);
  void AddSurfacePointsToMesh(void* pMesh, const ISurface& surface);
  bool RunFemGV(const std::string &sDianaPath, const std::string &sDiaSharePath,
  const std::string &sDianaLibPath, const std::string &sDianaSLibPath, const std::string &sFgvExeName);
  TRet OnMeshError(void* pMesh);

public:
  static CTetMeshBase *GetTetMesher(const std::string type = "");

  virtual ~CTetMeshBase();

  int AddSurface(const ISurface& surface, const std::string& sName, bool bSlip);

  virtual void Clear();

  // generate the mesh, fails if no correct input was given
  virtual TRet CreateMesh(const std::string &sDianaPath,
  const std::string &sDiaSharePath,
  const std::string &sDianaLibPath,
  const std::string &sDianaSLibPath,
  const std::string &sFgvExeName,
  CTetMeshProgress& progress /* = CTetMeshProgress()*/);

  //! Returns the tolerance of the mesher (EPS)
  ftn_double_t Tolerance() const { return m_dMeshTolerance; }
  //! Sets tolerance for the mesher
  void Tolerance(ftn_double_t dTolerance) { m_dMeshTolerance = dTolerance; }

  int NrOfSurfaces() const;
  const CSurfaceDesc& SurfaceDesc(int nIndex) const;
  CSurfaceDesc& SurfaceDesc(int nIndex);

  int NrOfVolumes() const;
  const CBodyGroup &Volume(int nIndex) const;
  CBodyGroup &Volume(int nIndex);


  /////////////////////////////////////////////////////////////////////////////////
  // the following functions are for loading purposes (Recover mesh from file)
  /////////////////////////////////////////////////////////////////////////////////
  virtual int RegisterElement(IElement& element);

  // adds a point to the main mesh and returns the index that can be used
  // for creating hexahedrons with the function 'RegisterTetrahedronInMainMesh'
  int RegisterNodeInMainMesh(IPoint& point, int nElements = 0);

  // adds a hexahedron element in the main mesh and returns the element index
  int RegisterTetrahedronInMainMesh(int nNode0, int nNode1, int nNode2, int nNode3);
  int RegisterTetrahedronInMainMesh(const std::vector<int>& vcNode);

  // adds a tet volume to the mesh and returns the volume index. the tet volume is
  // defined by the tetrahedron element indices.
  int RegisterTetVolume(const std::vector<int>& vcElement);

  // adds a tet surfaces to the main mesh and is defined by the tet volume, surface
  // descriptor and the node indexes of nodes in the main mesh
  bool RegisterTetSurface(CSurfaceDesc& desc, CBodyGroup& volume, const std::vector<int>& vcNode);

  int BodySize() const;
  int ContactElementSize() const;
  int InterfaceElementSize() const;

  virtual void Option(int option, bool flag);
  virtual bool Option(int) const;

  const IInterfaceElement *AddInterfaceElement(CSurfaceDesc& desc, const geo::IFace* front, const geo::IFace* back);
};

}

#endif
