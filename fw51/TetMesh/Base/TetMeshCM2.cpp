/* Copyright (c) 2018 DIANA FEA BV                              Confidential */
#if !defined(_DEBUG) && defined(WIN32)
#include <IGocadMesh.h>
#include "TetMeshBase.h"
#include "TetMeshCM2.h"
#include "TetSurface.h"
#include "SurfaceDesc.h"
#include "Tetrahedron.h"
#include "IProgressBase.h"
#include "InterfaceElement.h"
#include "IFace.h"
#include "GlobalMessage.h"

#include "RefinementBox.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace gocadmesh;
using namespace mesher_info;

namespace {

// From http://www.computing-objects.com/biblio/mt_doc_tetra.pdf
enum error_type
{
  CM2_NO_ERROR, // 0
  CM2_LICENSE_ERROR, // -100
  CM2_MODE_ERROR, // -101
  CM2_DATA_ERROR, // -102
  CM2_NODES_LIMIT_ERROR, // -103
  CM2_NODE_ERROR, // -104
  CM2_EDGE_ERROR, // -105
  CM2_FACE_ERROR, // -106
  CM2_BOUNDARY_ERROR, // -107
  CM2_DEGENERATED_ELEMENT, // -108
  CM2_BACKGROUND_MESH_ERROR, // -109
  CM2_SYSTEM_MEMORY_ERROR, // -199
  CM2_INTERNAL_ERROR // -200 
};

QString parseError(QString error)
{
  QString output = error;

  QStringList l = error.split(' ');
  bool ok = false;

  int code = l.last().toInt(&ok);

  if (ok)
  {
    switch (code)
    {
    case CM2_NO_ERROR:
      output = "CM2: no error";
      break;
    case CM2_LICENSE_ERROR:
      output = "CM2: license error";
      break;
    case CM2_MODE_ERROR:
      output = "CM2: mode error";
      break;
    case CM2_DATA_ERROR:
      output = "CM2: the input data is not valid";
      break;
    case CM2_NODES_LIMIT_ERROR:
      output = "CM2: the limitation on nodes number is too low";
      break;
    case CM2_NODE_ERROR:
      output = "CM2: invalid input node(s). Strict mode only";
      break;
    case CM2_EDGE_ERROR:
      output = "CM2: invalid input edge(s). Strict mode only";
      break;
    case CM2_FACE_ERROR:
      output = "CM2: invalid input face(s). Strict mode only";
      break;
    case CM2_BOUNDARY_ERROR:
      output = "CM2: the external boundary is not closed";
      break;
    case CM2_DEGENERATED_ELEMENT:
      output = "CM2: one or more degenerated elements (volume <= 0)";
      break;
    case CM2_BACKGROUND_MESH_ERROR:
      output = "CM2: invalid background mesh";
      break;
    case CM2_INTERNAL_ERROR:
      output = "CM2: unknown cause of error. Contact support";
      break;
    }
  }

  return output;
}


}

namespace geo {

CTetMeshCM2::CTetMeshCM2()
  : CTetMeshBase()
  , m_bNoStrict(false)
  , m_bNoCrash(false)
{

}

void CTetMeshCM2::CreateMesherInput(MesherInput& input) const
{
    for(vector<CSurfaceDesc *>::const_iterator iter = m_vcSurfaceDesc.begin(); iter != m_vcSurfaceDesc.end(); ++iter)
    {
        const CSurfaceDesc* sd = *iter;
        const ISurface& surface = sd->Surface();
        vector< vector<double> > pos;
        vector< vector<size_t> > connect;
        map<int, size_t> node_lookup;
        for(int j = 0; j < surface.FaceSize(); ++j)
        {
            const IFace& face = surface.Face(j);
            assert(face.NrOfPoints() == 3);
            vector<size_t> t;
            for(int k = 0; k < face.NrOfPoints(); ++k)
            {
                const CPoint& n = face.Point(k);
                int n_i = face.PointIndex(k);
                if(node_lookup.find(n_i) == node_lookup.end())
                {
                    vector<double> p;
                    p.push_back(n.getX());
                    p.push_back(n.getY());
                    p.push_back(n.getZ());
                    node_lookup.insert(make_pair(n_i, pos.size()));
                    pos.push_back(p);
                }
                t.push_back(node_lookup[n_i]);
            }
            connect.push_back(t);
        }
        const size_t surface_id = distance(m_vcSurfaceDesc.begin(), iter);
        Surface s(surface_id, sd->Name(), pos, connect);
        if(sd->Slip())
        {
            input.AddFault(s);
        }
        else
        {
            input.AddBoundary(s);
        }
    }

	for( auto& box: m_mesh_refinement_boxes_v )
		input.AddRefinementBox( box );
}

void CTetMeshCM2::RegisterNodes(const MesherOutput &output)
{
    for(vector<vector<double> >::const_iterator iter = output.Nodes().begin(); iter != output.Nodes().end(); ++iter)
    {
        const vector<double> &p = *iter;
        int geo_node_id = RegisterNode(CPoint(p[0], p[1], p[2]), false);
    }
}

void CTetMeshCM2::RegisterTetrahedrons(const MesherOutput &output)
{
    _tet_lookup.clear();
    for(vector<vector<size_t> >::const_iterator iter = output.Tetrahedrons().begin(); iter != output.Tetrahedrons().end(); ++iter)
    {
        const vector<size_t> &t = *iter;
        CTetrahedron *ctet = new CTetrahedron(*this, t[tetOrder[0]], t[tetOrder[1]], t[tetOrder[2]], t[tetOrder[3]]);
        _tet_lookup.push_back(ctet);
    }
}

void CTetMeshCM2::RegisterVolumes(const MesherOutput &output)
{
    for(vector<vector<size_t> >::const_iterator iter = output.Volumes().begin(); iter != output.Volumes().end(); ++iter)
    {
        CBodyGroup* volume = new CBodyGroup(*this);
        m_vcVolume.push_back(volume);
        const vector<size_t> &v = *iter;
        for(vector<size_t>::const_iterator viter = v.begin(); viter != v.end(); ++viter)
        {
            const geo::IBody* body = dynamic_cast<const geo::IBody*>(&Element(*viter));
            assert(body);
            volume->AddBody(*body);
        }
    }
}

void CTetMeshCM2::RegisterOuterSurfaces(const MesherOutput &output)
{
    for(size_t i = 0; i < output.SideSurfaces().size(); ++i)
    {
        const MesherOutput::tet_surface_t &side_surface = output.SideSurfaces()[i];
        CBodyGroup* volume = m_vcVolume[side_surface.volume_id];
        CSurfaceDesc* surface_desc = m_vcSurfaceDesc[side_surface.surface_id];
        CTetSurface* tet_surface = new CTetSurface(*surface_desc, *volume);
        for(vector<MesherOutput::face_t>::const_iterator iter = side_surface.tet_surface.begin(); iter != side_surface.tet_surface.end(); ++iter)
        {
            MesherOutput::face_t face = *iter;
            const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&Element(face.tet_id));
            const IFace *f = &(pBody->Face(tetOrder[face.face_id]));
            tet_surface->AddFace(*f);
        }
    }
}

double ShortestEdge(const IFace *f)
{
    assert(f->NrOfNodes() == 3);
    double mindist = f->Node(0).Distance(f->Node(1));
    mindist = min(mindist, f->Node(1).Distance(f->Node(2)));
    mindist = min(mindist, f->Node(2).Distance(f->Node(0)));
    return mindist;
}

void CTetMeshCM2::RegisterInterfaces(const MesherOutput &output)
{
    for(size_t i = 0; i < output.Interfaces().size(); ++i)
    {
        const MesherOutput::interface_t &interface = output.Interfaces()[i];
        CSurfaceDesc *surf_desc = m_vcSurfaceDesc[interface.surface_id];
        for(size_t e = 0; e < interface.elements.size(); ++e)
        {
            const MesherOutput::interface_element_t &element = interface.elements[e];
            CTetrahedron *ctet_front = _tet_lookup[element.front.tet_id];
            CTetrahedron *ctet_rear = _tet_lookup[element.rear.tet_id];
            const IFace* face_front = &(ctet_front->Face(tetOrder[element.front.face_id]));
            const IFace* face_rear = &(ctet_rear->Face(tetOrder[element.rear.face_id]));
            vector<int> vcNode(6);
            double eps = 0.1 * ShortestEdge(face_front);
            for(size_t n = 0; n < 3; ++n)
            {
                const INode &front_node = face_front->Node(n);
                bool closeNodeFound = false;
                for(size_t m = 0; m < 3; ++m)
                {
                    const INode &rear_node = face_rear->Node(m);
                    if(front_node.CloseTo(rear_node, eps))
                    {
                        vcNode[n] = front_node.Index();
                        vcNode[n+3] = rear_node.Index();
                        closeNodeFound = true;
                        break;
                    }
                }
                assert(closeNodeFound);
            }
            CInterfaceElement* pElement = new CInterfaceElement(*this, face_front, face_rear, vcNode);
            surf_desc->AddInterfaceElement(*pElement);
        }
    }
}

void CTetMeshCM2::PrintTriangle(const vector<size_t> &triangle) const
{
    for(vector<size_t>::const_iterator iter = triangle.begin(); iter != triangle.end(); ++iter)
    {
        cout << *iter << " ";
    }
    cout << endl;
}

bool CTetMeshCM2::TriangleListHasDuplicates(const vector< vector<size_t> > &triangle_list, bool verbose = false) const
{
    if(triangle_list.size() <= 1) return false;

    vector< vector<size_t> > sorted_triangles;
    for(vector< vector<size_t> >::const_iterator tl_iter = triangle_list.begin(); tl_iter != triangle_list.end(); ++tl_iter)
    {
        vector<size_t> triangle = *tl_iter;
        sort(triangle.begin(), triangle.end());
        sorted_triangles.push_back(triangle);
    }
    sort(sorted_triangles.begin(), sorted_triangles.end());
    size_t duplicates = 0;
    for(size_t d = 1; d < sorted_triangles.size(); ++d)
    {
        if(sorted_triangles[d] == sorted_triangles[d-1])
        {
            ++duplicates;
        }
    }
    if(verbose && duplicates > 0)
    {
        cout << endl << "List of triangles that has at least one duplicate:" << endl;
        cout << "Number of triangles : " << triangle_list.size() << endl;
        cout << "Number of duplicates: " << duplicates << endl;
        for(vector< vector<size_t> >::const_iterator iter = sorted_triangles.begin(); iter != sorted_triangles.end(); ++iter)
        {
            const vector<size_t> &triangle = *iter;
            PrintTriangle(triangle);
        }
    }
    return duplicates > 0;
}

const vector<size_t> CTetMeshCM2::GetTetFace(const vector<size_t> &tet, const size_t face) const
{
    assert(tet.size() == 4);
    vector<size_t> triangle;
    switch(face)
    {
    case 0:
        triangle.push_back(tet[1]);
        triangle.push_back(tet[2]);
        triangle.push_back(tet[3]);
        break;
    case 1:
        triangle.push_back(tet[2]);
        triangle.push_back(tet[0]);
        triangle.push_back(tet[3]);
        break;
    case 2:
        triangle.push_back(tet[1]);
        triangle.push_back(tet[3]);
        triangle.push_back(tet[0]);
        break;
    case 3:
        triangle.push_back(tet[2]);
        triangle.push_back(tet[1]);
        triangle.push_back(tet[0]);
        break;
    default:
        throw range_error("Face can only be [0,3].");
    }
    return triangle;
}


void CTetMeshCM2::VerifyOutput(const MesherOutput &output, const MesherInput &input, bool verbose = false) const
{
    const vector< vector<size_t> > &connect = output.Tetrahedrons();
    const vector< vector<double> > &pos = output.Nodes();
    if(verbose) cout << "Verifying " << connect.size() << " terahedrons...";
    for(vector< vector<size_t> >::const_iterator tet_iter = connect.begin(); tet_iter != connect.end(); ++tet_iter)
    {
        vector<size_t> tet = *tet_iter;
        assert(tet.size() == 4 && "A tetrahedron must consist of exactly 4 nodes.");
        sort(tet.begin(), tet.end());
        for(int i = 1; i < 4; ++i)
        {
            assert(tet[i-1] != tet[i] && "A tetrahedron must constist of unique nodes.");
        }
        assert(*(max_element(tet.begin(), tet.end())) < pos.size() && "Each node in the tetrahedron must refer to a node in the pos matrix.");
        //todo: possibly add some verifcation that the coordinates of each tet node actually differ (note that the pos matrix may have double entries, due to the splitting of the fault nodes.
    }
    if(verbose) cout << "OK!" << endl;
    const vector<MesherOutput::interface_t> &interfaces = output.Interfaces();
    if(verbose) cout << "Verifying " << interfaces.size() << " interfaces." << endl;
    for(vector<MesherOutput::interface_t>::const_iterator int_iter = interfaces.begin(); int_iter != interfaces.end(); ++int_iter)
    {
        if(verbose) cout << "Interface " << distance(interfaces.begin(), int_iter);
        vector< vector<size_t> > triangles_front, triangles_rear;
        const MesherOutput::interface_t &intrfc = *int_iter;
        for(vector<Surface>::const_iterator fault_iter = input.Faults().begin(); fault_iter != input.Faults().end(); ++fault_iter)
        {
            const Surface &fault_input_surface = *fault_iter;
            if(fault_input_surface.Id() != intrfc.surface_id) continue;
            assert(intrfc.elements.size() >= fault_input_surface.Connect().size() && "Interface needs to have at least the number of elements as the original input surface for the fault.");
            break;
        }
        for(vector<MesherOutput::interface_element_t>::const_iterator elm_iter = intrfc.elements.begin(); elm_iter != intrfc.elements.end(); ++elm_iter)
        {
            const MesherOutput::interface_element_t &elm = *elm_iter;
            const MesherOutput::face_t &face_front = elm.front;
            const MesherOutput::face_t &face_rear = elm.rear;
            assert(face_front.tet_id < connect.size() && "Interface element front tet must refer to an actual tet in the model.");
            assert(face_rear.tet_id < connect.size() && "Interface element rear tet must refere to an actual tet in the model.");
            const vector<size_t> &front_connect = connect[face_front.tet_id];
            const vector<size_t> &rear_connect = connect[face_rear.tet_id];
            set<size_t> all_element_nodes;
            all_element_nodes.insert(front_connect.begin(), front_connect.end());
            all_element_nodes.insert(rear_connect.begin(), rear_connect.end());
            assert(all_element_nodes.size() >= 4 && "Interface elements need to consist of at least 4 unique nodes.");

            triangles_front.push_back(GetTetFace(connect[face_front.tet_id], face_front.face_id));
            triangles_rear.push_back(GetTetFace(connect[face_rear.tet_id], face_rear.face_id));
        }
        if(verbose) cout << " " << triangles_front.size() << " front triangles, " << triangles_rear.size() << " rear triangles...";
        assert(!TriangleListHasDuplicates(triangles_front) && "Front face of the fault needs to consist of unique triangles.");
        assert(!TriangleListHasDuplicates(triangles_rear) && "Rear face of the fault needs to consist of unique triangles.");
        if(verbose) cout << " OK!" << endl;
    }
    const vector<MesherOutput::tet_surface_t> &side_surfaces = output.SideSurfaces();
    if(verbose) cout << "Verifying " << side_surfaces.size() << " side surfaces." << endl;
    for(vector<MesherOutput::tet_surface_t>::const_iterator srf_iter = side_surfaces.begin(); srf_iter != side_surfaces.end(); ++srf_iter)
    {
        if(verbose) cout << "Side surface " << distance(side_surfaces.begin(), srf_iter);
        vector< vector<size_t> > triangles;
        const MesherOutput::tet_surface_t &tet_surf = *srf_iter;
        for(vector<MesherOutput::face_t>::const_iterator f_iter = tet_surf.tet_surface.begin(); f_iter != tet_surf.tet_surface.end(); ++f_iter)
        {
            const MesherOutput::face_t &face = *f_iter;
            triangles.push_back(GetTetFace(connect[face.tet_id], face.face_id));
        }
        if(verbose) cout << " " << triangles.size() << " triangles...";
        if(TriangleListHasDuplicates(triangles, verbose))
        {
            if(verbose) cout << "Duplicates found." << endl;
        }
        else
        {
            if(verbose) cout << " OK!" << endl;
        }
    }
    const vector< vector<size_t> > &volumes = output.Volumes();
    size_t volume_tets = 0;
    if(verbose) cout << "Verifying " << volumes.size() << " volumes." << endl;
    for(vector< vector<size_t> >::const_iterator vol_iter = volumes.begin(); vol_iter != volumes.end(); ++vol_iter)
    {
        vector<size_t> volume = *vol_iter;
        if(verbose) cout << "Volume " << distance(volumes.begin(), vol_iter) << " " << volume.size() << " tets";
        volume_tets += volume.size();
        if(volume.size() <= 1) continue;
        sort(volume.begin(), volume.end());
        for(int v = 1; v < volume.size(); ++v)
        {
            assert(volume[v] != volume[v-1] && "All tets in the volume must be unique.");
        }
        assert(*(max_element(volume.begin(), volume.end())) < connect.size() && "Each tet in the volume must refer to a tet that actually exists.");
        if(verbose) cout << " OK!" << endl;
    }
    assert(volume_tets == connect.size() && "Volumes must add up to total connect matrix.");
}


typedef struct
{
  vector<mesher_info::NodeCoordinate> nodes;
} TTriangleHack;


CTetMeshBase::TRet CTetMeshCM2::CreateMesh(const std::string &sDianaPath,
                const std::string &sDiaSharePath,
                const std::string &sDianaLibPath,
                const std::string &sDianaSLibPath,
                const std::string &sFgvExeName,
                CTetMeshProgress& progress /* = CTetMeshProgress()*/)
{
    _status.first.clear();
    progress.StartMesh(7);
    MesherInput input;
    progress.NewJob("Prepare mesher input data structures");
    CreateMesherInput(input);

    input.strict_mode = !m_bNoStrict;

    IGocadMesh *mesher = IGocadMesh::GetGocadMesher(input, Tolerance());
    progress.NewJob("Execute meshing");
    bool mesh_result = mesher->Mesh();
    if(mesh_result)
    {
        MesherOutput output = mesher->Results();
        VerifyOutput(output, input, false);
        progress.NewJob("Register result nodes");
        RegisterNodes(output);
        progress.NewJob("Register result tetrehedrons");
        RegisterTetrahedrons(output);
        progress.NewJob("Register result volumes");
        RegisterVolumes(output);
        progress.NewJob("Register result boundaries");
        RegisterOuterSurfaces(output);
        progress.NewJob("Register result interfaces");
        RegisterInterfaces(output);
    }


    for(vector<MesherInfo>::const_iterator iter = mesher->Info().begin(); iter != mesher->Info().end(); ++iter)
    {
        //if (iter->ErrorLevel() > 0)
        //    continue;

        stringstream info;
        info << *iter;
        _status.first.push_back(info.str());
    }

    delete mesher;

    _status.second = mesh_result;
    progress.StopMesh();
    return _status;
}

vector<size_t> CTetMeshCM2::TetOrder()
{
    vector<size_t> result;
    result.push_back(0);
    result.push_back(2);
    result.push_back(1);
    result.push_back(3);
    return result;
}

vector<size_t> CTetMeshCM2::tetOrder = CTetMeshCM2::TetOrder();


CTetMeshCM2::~CTetMeshCM2()
{
}


void CTetMeshCM2::Option(int option, bool flag)
{
  switch (option)
  {
  case NO_STRICT_MODE:
    m_bNoStrict = flag;
    break;
  case NO_CRASHING:
    m_bNoCrash = flag;
    break;
  default:
    assert(false);
  }
}

bool CTetMeshCM2::Option(int option) const
{
  switch (option)
  {
  case NO_STRICT_MODE:
    return m_bNoStrict;
  case NO_CRASHING:
    return m_bNoCrash;
  default:
    assert(false);
  }

  return false;
}


}

#endif