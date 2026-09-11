/* Copyright (c) 2017 DIANA FEA BV                              Confidential */
#ifndef _TETMESH_CM2_H_
#define _TETMESH_CM2_H_

#if !defined(_DEBUG) && defined(WIN32)

#include "TetMeshBase.h"
#include <MesherInput.h>
#include <MesherOutput.h>

namespace geo {

#define DUMP_CM2_MATRICES 1

class CTetrahedron;

#include "RefinementBox.h"
typedef std::vector<  gocadmesh::RefinementBox > TBoxV_GoCad;


class TETMESH_EXPORT CTetMeshCM2 : public CTetMeshBase
{
public:
  CTetMeshCM2();
  virtual ~CTetMeshCM2();

  // generate the mesh, fails if no correct input was given
  virtual CTetMeshBase::TRet CreateMesh(const std::string &sDianaPath,
                                        const std::string &sDiaSharePath,
                                        const std::string &sDianaLibPath,
                                        const std::string &sDianaSLibPath,
                                        const std::string &sFgvExeName,
                                        CTetMeshProgress& progress /* = CTetMeshProgress()*/);

  typedef enum _Option {
    NO_STRICT_MODE = 0, // no strict allows repairs
    NO_CRASHING         // disable routines that crash in no strict
  } TOption;
  virtual void Option(int option, bool flag);
  virtual bool Option(int option) const;

public: // FIXME
  TBoxV_GoCad m_mesh_refinement_boxes_v;
public:
	void set_mesh_refinement_boxes( TBoxV_GoCad boxes_v ){ m_mesh_refinement_boxes_v = boxes_v; };


private:
    TRet _status;

    static std::vector<size_t> tetOrder;

    std::vector<CTetrahedron*> _tet_lookup;

    void CreateMesherInput(gocadmesh::MesherInput &input) const;
    void RegisterNodes(const gocadmesh::MesherOutput &output);
    void RegisterTetrahedrons(const gocadmesh::MesherOutput &output);
    void RegisterVolumes(const gocadmesh::MesherOutput &output);
    void RegisterOuterSurfaces(const gocadmesh::MesherOutput &output);
    void RegisterInterfaces(const gocadmesh::MesherOutput &output);

    void VerifyOutput(const gocadmesh::MesherOutput &output, const gocadmesh::MesherInput &input, bool verbose) const;
    bool TriangleListHasDuplicates(const vector< vector<size_t> > &triangle_list, bool) const;
    const vector<size_t> GetTetFace(const vector<size_t> &tet, const size_t face) const;

    void PrintTriangle(const vector<size_t> &triangle) const;

    static std::vector<size_t> TetOrder();

    bool m_bNoStrict;
    bool m_bNoCrash;
};

}

#endif

#endif