#include "stdafx.h"

#include "Algebra.h"
#include "Deformation.h"

/*
#define DEBUG
#ifdef DEBUG
#define IT_Printer::instance()->debug(m) IT_Printer::instance()->debug(m);
#else
#define IT_Printer::instance()->debug(m) do {} while(false);
#endif
*/

// FIXME: Deformation visualizing functionality

#include "WellCasingCementInterface.h"
#include "WellCasingNode.h"
#include "WellCasingSteel.h"

#include "IT_Printer.h"
// #include "IT_Geomec_Interface.h"
#include "GeomecDoc.h" //FIXME
#include "IGm.h"

#include "IProgressBase.h"
#include "OIDIGeometry.h"
#include "OIDIMesh.h"
#include "OIDISet.h"
#include "ValueMapper.h"

#include "Global.h"
#include "IProgressFactory.h"

// #include "ConsistencyGuard.h"

// FIXME
const OIDIGeometryI::TDeformation OIDIGeometryI::NO_DEFORMATION = OIDIGeometryI::TDeformation{
    false, std::vector<MbVec3d>(), std::make_pair(MbVec3d(0.0, 0.0, 0.0), MbVec3d(0.0, 0.0, 0.0))};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- DEFORMATION
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------

const Deformation::Data::Result::Info Deformation::INVALID_INFO =
    Data::Result::Info{std::vector<MbVec3d>(), // deformation
                       nullptr,                // geo
                       nullptr,                // mesh
                       ""};                    // name

const Deformation::Data::Result::Geo Deformation::INVALID_GEO = Data::Result::Geo{
    std::vector<MbVec3d>(),                                                                   // points
    Deformation::Data::Result::Geo::TMinMax{MbVec3d(0.0, 0.0, 0.0), MbVec3d(0.0, 0.0, 0.0)}}; // min_max

const Deformation::Data::Result Deformation::INVALID_RESULT = Data::Result{INVALID_INFO, INVALID_GEO};

const Deformation::Data::Input Deformation::INVALID_INPUT = Deformation::Data::Input{
    -1,                                         // stage
    Deformation::eFormationType::All,           // type
    Deformation::Data::Input::TScale{1.0, 1.0}, // scale: generic / deformation
    false                                       // active
};

const Deformation::Data Deformation::INVALID = Deformation::Data{INVALID_INPUT, std::vector<Data::Result>()};

Deformation::Deformation() : m_data(INVALID) {}

bool Deformation::is(OIDIMesh *mesh, eFormationType _type) {
  return Deformation::is((COpenGLNode *)mesh->getOpenGLNode(), _type);
}

bool Deformation::is(COpenGLNode *node, eFormationType _type) { return formation_type(node) == _type; }

Deformation::eFormationType Deformation::formation_type(OIDIMesh *mesh) {
  return formation_type((COpenGLNode *)mesh->getOpenGLNode());
}

Deformation::eFormationType Deformation::formation_type(COpenGLNode *node) {
  CWellCasingSteel *node_steel = dynamic_cast<CWellCasingSteel *>(node);

  if (node_steel)
    return eFormationType::Steel;

  CWellCasingCementInterface *node_cement_iface = dynamic_cast<CWellCasingCementInterface *>(node);
  if (node_cement_iface)
    return eFormationType::CementInterface;

  Casing3DFormation *node_cement = dynamic_cast<Casing3DFormation *>(node);
  if (node_cement)
    return eFormationType::Cement;

  return eFormationType::Default_; //'Burden', 'Reservoir'
}

const IVectorResult::CFullVectorComponent *Deformation::displacement_component(int stage_idx) {
  // CDepletionStage* pStage = IT_Geomec_Interface::depletion_stage( stage_idx );
  CDepletionStage *pStage = gm::model::depletion::get(stage_idx);
  if (pStage) {
    const IVectorResult::CFullVectorComponent *p_full_vector_component =
        gm::res::composite::component::get_FULL(gm::eComposite::C_Displacement, pStage);
    if (p_full_vector_component)
      return p_full_vector_component;
    else
      IT_Printer::instance()->debug("%s : component : nullptr", __FUNCTION__);
  } else
    IT_Printer::instance()->debug("%s : stage : nullptr", __FUNCTION__);

  return nullptr;
}

OIDIMesh *Deformation::mesh(eFormationType _type, OIDISet &meshes) {
  for (std::size_t i = 0; i < meshes.NrOfMesh(); ++i) {
    OIDIMesh *mesh = (OIDIMesh *)meshes.getMesh(i);
    if (mesh) {
      COpenGLNode *node_openGL = (COpenGLNode *)mesh->getOpenGLNode(); // FIXME: remove
      std::string name = mesh->getOpenGLNode()->Name().toStdString();  // FIXME: remove
      IT_Printer::instance()->debug("mesh : openGL_node: '%s'", mesh->getOpenGLNode()->Name().toStdString().c_str());

      if (Deformation::is(node_openGL, _type))
        return mesh;
    }
  }
  return nullptr;
}

std::vector<MbVec3d> Deformation::get(OIDIMesh *mesh, const IVectorResult::CFullVectorComponent *component) {
  CValueMapper *vm = CValueMapper::instance();
  std::unique_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Collecting values", !vm->UseLegacyCode()));

  if (component->CanMap(*(mesh->getOpenGLNode()))) {
    OIDIMesh::MeshType type_ = mesh->meshType();
    prog->AddSteps(mesh->meshType() == OIDIMesh::PointSetMesh ? mesh->getSize() : mesh->getNumCells());

    CDoubleQuantity::UNIT unit = GetGeomecDoc()->UnitNode().Unit();

    OIDIVectorSetI *deformation = mesh->collectVectorsByGeometry(component, unit, *prog);

    return Algebra::convert(deformation);
  } else {
    return std::vector<MbVec3d>{};
  }
}

void Deformation::add_result(OIDIMesh *mesh, const IVectorResult::CFullVectorComponent *component,
                             Data::Input::TScale scale) {
  Data::Result r = INVALID_RESULT;

  r.info.deformation = component && component->Defined()
                           ? get(mesh, component)
                           : std::vector<MbVec3d>(mesh->getSize(), MbVec3d(0.0, 0.0, 0.0));

  int diff = abs((int)mesh->getSize() - (int)r.info.deformation.size());
  if (diff) {
    std::vector<MbVec3d> append_v = std::vector<MbVec3d>(diff, MbVec3d(0.0, 0.0, 0.0));
    r.info.deformation.insert(r.info.deformation.end(), append_v.begin(), append_v.end());
  }

  r.info.geo = &(OIDIGeometryI &)mesh->getGeometry();
  r.info.name = mesh->getOpenGLNode()->Name().toStdString();

  r.geo.points = Algebra::points_mod((const OIDIGeometryI &)*r.info.geo, r.info.deformation, scale.gen, scale.def);

  std::pair<MbVec3d, MbVec3d> min_max = Algebra::min_max(r.geo.points);
  r.geo.min_max = Data::Result::Geo::TMinMax{min_max.first, min_max.second};

  m_data.results_v.push_back(r);

  IT_Printer::instance()->debug("deformation : add : openGL_node: '%s' : nP: %d", r.info.name.c_str(),
                                r.geo.points.size());
}

const Deformation::Data::Result &Deformation::result(int i) {
  return m_data.results_v.size() < i ? (const Data::Result &)m_data.results_v[i] : INVALID_RESULT;
}

bool Deformation::apply() {
  Deformation::Data &d = Deformation::current();
  if (d == Deformation::INVALID) {
    return false;
  } else {
    for (int i = 0; i < d.results_v.size(); i++) {
      Data::Result &r = d.results_v[i];
      if (d.input.active) {
        IT_Printer::instance()->debug("deformation : refresh : openGL_node: '%s' : nP: %d", r.info.name.c_str(),
                                      r.geo.points.size());
        r.info.geo->set_deformation(r.geo.points, std::make_pair(r.geo.min_max.min, r.geo.min_max.max));
      } else {
        IT_Printer::instance()->debug("deformation : remove geo : openGL_node: '%s'", r.info.name.c_str());
        r.info.geo->set_deformation(false);
      }
    }
    return true;
  }
}

void Deformation::undo() {
  Deformation::Data &d = Deformation::current();

  for (int i = 0; i < d.results_v.size(); i++) {
    Data::Result &r = d.results_v[i];
    IT_Printer::instance()->debug("deformation : remove geo : openGL_node: '%s'", r.info.name.c_str());
    r.info.geo->set_deformation(false);
  }
}

Deformation::eStatus Deformation::status() {
  CWellCasingModel *casing_model = dynamic_cast<CWellCasingModel *>(GetGeomecDoc()->Model());

  if (casing_model && !casing_model->IsMesh())
    invalidate_soft();

  return (!casing_model || m_data == INVALID) ? Invalid : Valid;

  /*bool is_model_in_scope = false;
  CModelBase* pModel = dynamic_cast<CModelBase*>( GetGeomecDoc()->Model() );
  if( pModel )
  {
    CConsistencyGuard* guard = pModel->GetConsistencyGuard();
    is_model_in_scope = guard && guard->is_model_in_scope( CConsistencyGuard::Deformation );
    if( is_model_in_scope && !pModel->IsMesh() )
      invalidate();
  }

  return ( !is_model_in_scope || m_data == INVALID ) ? Invalid : Valid; */
}
void Deformation::invalidate_soft() { m_data = INVALID; }

int Deformation::num_meshes() { return m_data.results_v.size(); }

Deformation::eStatus Deformation::refresh(OIDISet &meshes) {
  Deformation::Data &d = Deformation::current();
  return get(d.input, meshes, true);
}

Deformation::eStatus Deformation::get(Data::Input input, OIDISet &meshes, bool refresh) {
  bool bRefresh =
      refresh || (input != m_data.input && (m_data.input.active || input.active)); // != -> active, stage, scale

  if (bRefresh) {
    IT_Printer::instance()->debug("-> Refresh");

    invalidate_soft();

    const IVectorResult::CFullVectorComponent *component = Deformation::displacement_component(input.stage);
    for (std::size_t i = 0; i < meshes.NrOfMesh(); ++i) {
      OIDIMesh *mesh = (OIDIMesh *)meshes.getMesh(i);
      if (mesh) {
        add_result(mesh, component, input.scale);
        // Printer::instance()->debug("get : mesh : %d : %s : VU:%d", i, mesh->name().c_str(),
        // dynamic_cast<OIDIMeshVU*>( mesh ) );
      }

      // MCR 2019-05-28: why this? it looks wrong but never happens as type always 'All'
      if (input.type != All && is(mesh, input.type))
        break;
    }

    m_data.input = input;
  } else {
    IT_Printer::instance()->debug("-> NO Refresh (data available)");
  }

  return status();
}

// FIXME: wrong
std::vector<MbVec3d> Deformation::get_FAKE(OIDIMesh *mesh) {
  const OIDIGeometryI &g = mesh->getGeometry();

  MbVec3d translation = g.getTranslation();

  MbVec3d max = translation + g.getMax();
  MbVec3d min = translation + g.getMin();

  double ZONE_LOW = (max[2] + min[2]) / 3;
  double ZONE_UP = 2 * ZONE_LOW;

  double DEF_NOT = 0.000;
  double DEF_LOW = 0.001;
  double DEF_MED = 0.005;
  double DEF_HIG = 0.010;

  std::vector<MbVec3d> points_v;

  for (int i = 0; i < g.getSize(); i++) {
    const geo::IPoint *p = g.getPoint(i);
    if (Algebra::regular(p)) {
      bool is_inside = Algebra::inside(p);
      double def = p->Z() < ZONE_LOW ? DEF_LOW : p->Z() < ZONE_UP ? DEF_MED : DEF_HIG;
      MbVec3d def_p = MbVec3d(def, def, DEF_NOT);
      points_v.push_back(is_inside ? -def_p : def_p);
    } else
      points_v.push_back(MbVec3d(0.0, 0.0, 0.0));
  }
  return points_v;
}

void Deformation::Invalidate() {
  undo();
  invalidate_soft();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- DEFORMATION TEST
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------

void Deformation_TEST::log_displacement(std::vector<MbVec3d> &offset_v) {
  for (int i = 0; i < 5; i++) {
    MbVec3d &v = offset_v[i];
    IT_Printer::instance()->debug("%d : (%f,%f,%f)", i, v[0], v[1], v[2]);
  }
  for (int i = offset_v.size() - 6; i < offset_v.size() - 1; i++) {
    MbVec3d &v = offset_v[i];
    IT_Printer::instance()->debug("%d : (%f,%f,%f)", i, v[0], v[1], v[2]);
  }
}

std::vector<MbVec3d> Deformation_TEST::convert_data(OIDIVectorSetI *vectors, MbVec3d scale_def_3d) {
  static const double myNaN = -99999999.999;

  std::vector<MbVec3d> offset_v;

  double scale = 10.0;

  for (int i = 0; i < vectors->dataSize(); i++) {
    MbVec3d v = vectors->get(i);
    MbVec3d v_2 = MbVec3d(v[0] == myNaN ? 0.0 : scale * v[0], v[1] == myNaN ? 0.0 : scale * v[1],
                          v[2] == myNaN ? 0.0 : scale * v[2]);
    offset_v.push_back(scale_def_3d * v_2);
  }
  return offset_v;
}

std::vector<MbVec3d> Deformation_TEST::convert_data_FAKE(OIDIVectorSetI *vectors, MbVec3d scale_def_3d) {
  std::vector<MbVec3d> offset_v;
  double d0 = 0.001;
  double d1 = 0.01;
  double d2 = 0.1;
  double d = d0;
  int size = vectors->dataSize();

  for (int i = 0; i < size / 3; i++)
    offset_v.push_back(scale_def_3d * MbVec3d(d, d, 0));

  for (int i = size / 3; i < 2 * size / 3; i++)
    offset_v.push_back(scale_def_3d * MbVec3d(d1, d1, 0));

  for (int i = 2 * size / 3; i < size; i++)
    offset_v.push_back(scale_def_3d * MbVec3d(d2, d2, 0));

  return offset_v;
}

MbVec3d Deformation_TEST::_vector(const geo::IElement &e, int ini, int end) {
  TElemVectors v;

  const geo::IPoint &p_ini = e.Point(ini);
  const geo::IPoint &p_end = e.Point(end);

  return MbVec3d{p_end.X() - p_ini.X(), p_end.Y() - p_ini.Y(), p_end.Z() - p_ini.Z()};
}

void Deformation_TEST::explore_geometry_TEST(const OIDIGeometryI &g, double scale_gen) {
  const geo::IElementSet *p = g.Parent();
  int size = p->ElementSize();

  IT_Printer::instance()->debug("Elems:%d", size);

  for (int i = 0; i < size; i++) {
    const geo::IElement &e = p->Element(i);

    if (e.IsInterfaceElement())
      continue;

    // method : A
    /*MbVec3d i_ = Algebra::intersection( e, Algebra::Bottom );
    //IT_Printer::instance()->debug( "E:%d Intersection: %2f\t%2f\t%2f", i, i_[0], i_[1], i_[2] );
    Algebra::m_data._print();

    i_ = Algebra::intersection( e, Algebra::Top );
    Algebra::m_data._print();*/

    // method : B
    MbVec3d i2_ = Algebra::intersection(e, Algebra::Top);
    // IT_Printer::instance()->debug("E:%d I2: %f\t%f\t%f", i, i2_[0],i2_[1],i2_[2]);

    static const MbVec3d CENTER = MbVec3d(1013.0, 822.0, 8.0);
    if (!Algebra::equal(i2_, CENTER))
      IT_Printer::instance()->debug("ERROR : E:%d I2: %f\t%f\t%f", i, i2_[0], i2_[1], i2_[2]);

    std::vector<MbVec3d> v = Algebra::scaled_elem(e, scale_gen);
    Algebra::print_elem(e);
    Algebra::print_points(v);

    /*int nP = e.NrOfPoints();
    for( int j=0; j < nP; j++ )
    {
      const geo::IPoint& p = e.Point( j );
      //IT_Printer::instance()->debug( "E:%d P:%d : %2f\t%2f\t%2f", i, j, p.X(), p.Y(), p.Z() );
      IT_Printer::instance()->debug( "E:%d P:%d : %d\t%d\t%0.f", i, j, Algebra::fractional(p.X()),
    Algebra::fractional(p.Y()), p.Z() );
    }

    int a = 4;
    /if( i == 1 )

      break;*/

    break;
  }
}

Deformation_TEST::TElemVectorsV Deformation_TEST::_vectors(const OIDIGeometryI &g) {
  TElemVectorsV v;

  const geo::IElementSet *p = g.Parent();
  int size = p->ElementSize();

  for (int i = 0; i < size; i++) {
    const geo::IElement &e = p->Element(i);
    // int nP = e.NrOfPoints();
    v.push_back(TElemVectors{_vector(e, 4, 0), _vector(e, 2, 1), _vector(e, 7, 4), _vector(e, 6, 5)});

    if (i == 1)
      break;
  }
  return v;
}