#include "stdafx.h"

#include "IT_Geomec_Interface_Surface.h"

#include "FormationBase.h"

#include "MeshResultTree.h"
#include "ResultTree.h"

#include "IValueComponent.h"
#include "IValueComposite.h"

#include "GeomecDoc.h" // model & PostMessage

#include "IT_Printer.h"
#include "NodalValueSet.h"
#include "PointSet.h"
#include "Pressure.h"
#include "TetraFormation.h"

#include "ISceneWrapper.h"

#include "ModelTreeView.h"
#include "ModelView.h"

#include "Material.h" //Poisson ratio

#include "IGeomec.h"

#include "Displacement.h"
#include "MaterialParameter.h"
#include "Reservoir.h"

#include "IT_Geomec_Interface_Dummy.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
typedef IT_Geomec_Interface_Dummy dummy;

#include "PyConfigFile.h"
typedef PyConfigFile config;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- GENERIC
//--------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

IT_Data::TPointsV IT_Geomec_Interface_Surface::G_points(int reservoir_id, int surface_id, int volume_idx) {
  IT_Data::TPointsV points_v;

  int num_vol = G_num_volumes(reservoir_id);
  for (int i = 0; i < num_vol; i++) {
    geo::CBodyGroup::CSideSurface *surface = G_surface(reservoir_id, volume_idx, surface_id);
    if (surface) {
      int nPoints = surface->PointSize();
      points_v.reserve(nPoints);

      // points
      for (int i = 0; i < nPoints; i++) {
        const geo::IPoint &point = surface->Point(i);
        points_v.push_back(IT_Data::TPoint{point.X(), point.Y(), point.Z()});
      }
    }
  }
  return points_v;
}

int IT_Geomec_Interface_Surface::G_num_volumes(int reservoir_id) {
  int num = 0;
  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME
  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);
    num = t.VolumeSize();
  }
  return num;
}

geo::CBodyGroup::CSideSurface *IT_Geomec_Interface_Surface::G_surface(int reservoir_id, int volume_idx,
                                                                      int surface_id) {
  geo::CBodyGroup::CSideSurface *surface = nullptr;
  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME
  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);
    surface = &t.Volume(volume_idx).Volume().SideSurface(surface_id);
  }
  return surface;
}

IT_Data::T3Values IT_Geomec_Interface_Surface::G_element_component_value(const geo::IFace &face,
                                                                         base::TDisplacementComponentV &component_v,
                                                                         CDepletionStage *pStage,
                                                                         eComposite composite_) {
  const geo::IPoint &point = face.MidPoint();

  int nComponents = component_v.size();
  IT_Data::T3Values values = {NaN, NaN, NaN};
  for (int j = 0; j < nComponents; j++) {
    base::eComponent _type = component_v[j];

    double &val = _type == base::X ? values.v1 : _type == base::Y ? values.v2 : values.v3;

    IVectorResult::VECTOR_COMPONENT type_ = base::geomec_component_type(_type);
    const IVectorResult::CVectorComponent *component =
        dynamic_cast<const IVectorResult::CVectorComponent *>(base::G_RES_component(composite_, pStage, type_));

    val = NaN;
    if (component) {
      // ValuePoint would map the body values to the point, but we need explicitly the face values mapped
      IValueDomainScalar::TValueVec vcValues = component->ValueElement(face);
      geo::IElement::TDoubleVec vcShape = face.IsoShapeFunction(point);

      assert(vcShape.size() == vcValues.size());

      geo::CValue c_val = vcValues[0] * vcShape[0];

      for (size_t i = 1; i < vcValues.size(); ++i)
        c_val += vcValues[i] * vcShape[i];

      val = c_val.Value() / vcValues.size();
    }
  }
  return values;
}

//------------------------------------------------- IT
//--------------------------------------------------------------------

T_Elem_And_Val_And_Z IT_Geomec_Interface_Surface::BD_elem_and_val_and_z(const geo::IPoint &point,
                                                                        geo::CBodyGroup::CSideSurface &surface,
                                                                        IValueComposite *composite,
                                                                        base::TDisplacementComponentV &component_v) {
  T_Elem_And_Val_And_Z res;

  std::set<int> faces_s = surface.Candidates(surface.NearestXYPoint(point));
  for (std::set<int>::iterator it = faces_s.begin(); it != faces_s.end(); it++) {
    const geo::IFace &face = surface.Face(*it);
    geo::CTriangle trans = translate_face_to_point(point, face);
    if (trans.Contains(point, true)) {
      res.elem_id = *it;
      IT_Data::T3Values values = {NaN, NaN, NaN};
      for (base::TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it) {
        base::eComponent _type = (*it);
        geo::CValue c_value =
            composite->Component(_type).ScalarData().ValuePoint(point); // values per each point in the element

        double &val = _type == base::X ? values.v1 : _type == base::Y ? values.v2 : values.v3;
        val = c_value.Value();
      }
      res.val = values;
      res.z = point.Z();
      break;
    }
  }
  return res;
}

T_Elem_And_Val_And_Z_V IT_Geomec_Interface_Surface::BD_elems_and_vals_and_z(int reservoir_id, int composite_idx,
                                                                            int surface_id,
                                                                            base::TDisplacementComponentV component_v,
                                                                            int volume_idx, eComposite composite_) {
  T_Elem_And_Val_And_Z_V res;

  CFormationBase *pFormation = base::formation(reservoir_id);
  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);
    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);

      std::pair<IValueComposite *, CPointSet *> composite_p = base::G_BD_composite(composite_, composite_idx);
      IValueComposite *composite = composite_p.first;
      CPointSet *ps = composite_p.second;

      for (int i = 0; i < ps->PointSize(); i++) {
        const geo::IPoint &point = ps->PointAt(i);
        if (base::G_Filter(point, ps))
          res.push_back(BD_elem_and_val_and_z(ps->PointAt(i), surface, composite, component_v));
      }
    }
  }
  return res;
}

//------------------------------------------------- ELEMS
//--------------------------------------------------------------------

geo::CTriangle IT_Geomec_Interface_Surface::translate_face_to_point(const geo::CPoint &point, const geo::IFace &face) {
  geo::CPoint tmp[3];

  for (int i = 0; i < 3; ++i)
    tmp[i].Set(face.Point(i).X(), face.Point(i).Y(), point.Z());

  return geo::CTriangle(tmp[0], tmp[1], tmp[2]);
}

int IT_Geomec_Interface_Surface::BD_elem(const geo::IPoint &point, geo::CBodyGroup::CSideSurface &surface) {
  int elem_id = -1;
  std::set<int> faces_s = surface.Candidates(surface.NearestXYPoint(point));
  for (std::set<int>::iterator it = faces_s.begin(); it != faces_s.end(); it++) {
    const geo::IFace &face = surface.Face(*it);
    geo::CTriangle trans = translate_face_to_point(point, face);
    if (trans.Contains(point, true)) {
      elem_id = *it;
      break;
    }
  }
  return elem_id;
}

IT_Data::TIntV IT_Geomec_Interface_Surface::BD_elems(CPointSet *ps, geo::CBodyGroup::CSideSurface &surface) {
  IT_Data::TIntV elems_v;
  for (int i = 0; i < ps->PointSize(); i++) {
    const geo::IPoint &point = ps->PointAt(i);
    if (base::G_Filter(point, ps))
      elems_v.push_back(BD_elem(point, surface));
  }
  return elems_v;
}

IT_Data::TIntV IT_Geomec_Interface_Surface::BD_elems(int reservoir_id, int surface_id, eComposite composite_,
                                                     int composite_idx) {
  IT_Data::TIntV elems_v;

  CFormationBase *pFormation = base::formation(reservoir_id);
  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);
    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      IT_Data::TIntV elems = BD_elems(reservoir_id, i, surface_id, composite_, composite_idx);
      elems_v.insert(elems_v.end(), elems.begin(), elems.end());
    }
  }
  return elems_v;
}

IT_Data::TIntV IT_Geomec_Interface_Surface::BD_elems(int reservoir_id, int volume_idx, int surface_id,
                                                     eComposite composite_, int composite_idx) {
  IT_Data::TIntV elems_v;

  geo::CBodyGroup::CSideSurface *surface = G_surface(reservoir_id, volume_idx, surface_id);
  if (surface) {
    std::pair<IValueComposite *, CPointSet *> composite_p = base::G_BD_composite(composite_, composite_idx);
    IValueComposite *composite = composite_p.first;
    CPointSet *ps = composite_p.second;

    IT_Data::TIntV elems = BD_elems(ps, *surface);
    elems_v.insert(elems_v.end(), elems.begin(), elems.end());
  }
  return elems_v;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Formations
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

double IT_Geomec_Interface_Surface::material_parameter_value(CFormationBase *pFormation, CDepletionStage *pStage,
                                                             int material_id_, const geo::IFace &body) {
  double val = NaN;

  if (pFormation && pStage) {
    const CFFMaterial &mat = pFormation->Material(*pStage).Material(body);

    if (mat.IsParameter(material_id_))
      val = mat.ParameterValue(material_id_);
  }
  return val;
}

MaterialParameter &IT_Geomec_Interface_Surface::material_parameter(int reservoir_id, int depletion_id,
                                                                   eMaterialParameter _material_id, int surface_id) {
  IT_Data::TValuesV values_v;

  int material_id_ = material_id(_material_id);

  MaterialParameter &param = *(new MaterialParameter());

  CFormationBase *pFormation = base::formation(reservoir_id);
  CDepletionStage *pStage = base::depletion_stage(depletion_id);

  if (pFormation && pStage) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);
      int nFaces = surface.FaceSize();
      for (int i = 0; i < nFaces; i++)
        values_v.push_back(material_parameter_value(pFormation, pStage, material_id_, surface.Face(i)));
    }
  }

  param.vals(values_v);

  return param;
}

IT_Data::T4Values IT_Geomec_Interface_Surface::formation_properties_values(TReservoirPropertyV &properties_v,
                                                                           const geo::IPoint &point,
                                                                           CFormationBase *pFormation,
                                                                           CDepletionStage *pStage) {
  int num_props = properties_v.size();

  IT_Data::T4Values values = {NaN, NaN, NaN, NaN};
  for (int j = 0; j < num_props; j++) {
    eReservoirProperty prop = properties_v[j];
    double &val = prop == Thickness       ? values.v1
                  : prop == PoissonsRatio ? values.v2
                  : prop == YoungsModulus ? values.v3
                                          : values.v4;
    val = formation_property_value(prop, point, pFormation, pStage);
  }
  return values;
}

double IT_Geomec_Interface_Surface::formation_property_value(eReservoirProperty prop, const geo::IPoint &point,
                                                             CFormationBase *pFormation, CDepletionStage *pStage) {
  double val = NaN;
  if (pFormation) {
    if (prop == Thickness) {
      geo::CPoint point_top;
      geo::CPoint point_bottom;

      val = pFormation->ThicknessAt(point, point_top, point_bottom, nullptr)
                .Value(); // FIXME: paralalize / it takes 5s. first time, then 0.05s.
    } else {
      if (pStage) {
        if (prop == PoissonsRatio || prop == YoungsModulus)
          val = pFormation->Material(*pStage).Material(point).ParameterValue(material_id(base::convert(prop)));
        else if (prop == PorePressure)
          val = pFormation->Pressure(*pStage).Component().ScalarData().ValuePoint(point).Value();
      }
    }
  }
  return val;
}

//--------------------------------------------------------------------------------------------------------------------------------------
Reservoir &IT_Geomec_Interface_Surface::reservoir(int reservoir_id, int surface_id, TReservoirPropertyV properties_v,
                                                  int depletion_id) {
  //------------------------------------- elems / points
  IT_Data::TTriV elems_v;
  IT_Data::TPointsV points_v;
  IT_Data::T4ValuesV params_v;
  //-------------------------------------

  Reservoir &reservoir = *(new Reservoir(Reservoir::eElemType::Tri));

  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME
  CDepletionStage *pStage = base::depletion_stage(depletion_id);

  if (pFormation && pStage) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare> point_reindex_map;

    const int num_points_per_elem = 3; // surface.Face( 0 ).NrOfPoints();

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);

      // (1) points and properties : thikness, PoissonsRatio, YoungsModulus, PorePressure
      int nPoints = surface.PointSize();

      for (int i = 0; i < nPoints; i++) {
        const geo::IPoint &point = surface.Point(i);
        IT_Data::TPoint data_point{point.X(), point.Y(), point.Z()};
        points_v.push_back(data_point);
        point_reindex_map[data_point] = i + 1;

        // Printer::_print_points_map( point_reindex_map );

        params_v.push_back(formation_properties_values(properties_v, point, pFormation, pStage));
      }

      // (2) elems: triangles: face is the abstract of a triangle
      int nFaces = surface.FaceSize();
      for (int i = 0; i < nFaces; i++) {
        const geo::IFace &face = surface.Face(i);

        IT_Data::TPoint point_d[num_points_per_elem] = {{face.Point(0).X(), face.Point(0).Y(), face.Point(0).Z()},
                                                        {face.Point(1).X(), face.Point(1).Y(), face.Point(1).Z()},
                                                        {face.Point(2).X(), face.Point(2).Y(), face.Point(2).Z()}};

        int reindexed[num_points_per_elem] = {-1, -1, -1};

        for (int j = 0; j < num_points_per_elem; ++j)
          reindexed[j] = point_reindex_map[point_d[j]];

        IT_Data::TTri elem_d = {reindexed[0], reindexed[1], reindexed[2]};
        elems_v.push_back(elem_d);
      }
    }
  }

  Printer::instance()->debug("get : res : surf : res_id:%d -> num: points:%d elems:%d params:%d header:%s",
                             reservoir_id, points_v.size(), elems_v.size(), params_v.size(),
                             base::reservoir_header(properties_v).c_str());

  reservoir.tri(elems_v);
  reservoir.points(points_v);
  reservoir.params(params_v);
  reservoir.header(base::reservoir_header(properties_v)); // FIXME: hard-coded

  return reservoir;
}

IT_Data::TTriV IT_Geomec_Interface_Surface::reservoir_E(int reservoir_id, int surface_id) {
  IT_Data::TTriV elems_v;

  CFormationBase *pFormation = base::formation(reservoir_id);

  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare> point_reindex_map;

    const int num_points_per_elem = 3; // surface.Face( 0 ).NrOfPoints();

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);

      int nPoints = surface.PointSize();
      for (int i = 0; i < nPoints; i++) {
        const geo::IPoint &point = surface.Point(i);
        IT_Data::TPoint data_point{point.X(), point.Y(), point.Z()};
        point_reindex_map[data_point] = i + 1;
      }

      int nFaces = surface.FaceSize();
      for (int i = 0; i < nFaces; i++) {
        const geo::IFace &face = surface.Face(i);

        IT_Data::TPoint point_d[num_points_per_elem] = {{face.Point(0).X(), face.Point(0).Y(), face.Point(0).Z()},
                                                        {face.Point(1).X(), face.Point(1).Y(), face.Point(1).Z()},
                                                        {face.Point(2).X(), face.Point(2).Y(), face.Point(2).Z()}};

        int reindexed[num_points_per_elem] = {-1, -1, -1};

        for (int j = 0; j < num_points_per_elem; ++j)
          reindexed[j] = point_reindex_map[point_d[j]];

        IT_Data::TTri elem_d = {reindexed[0], reindexed[1], reindexed[2]};
        elems_v.push_back(elem_d);
      }
    }
  }

  return elems_v;
}

IT_Data::TValuesV IT_Geomec_Interface_Surface::reservoir_property(int reservoir_id, int surface_id,
                                                                  eReservoirProperty property_, int depletion_id) {
  IT_Data::TValuesV values_v;

  CFormationBase *pFormation = base::formation(reservoir_id);
  CDepletionStage *pStage = base::depletion_stage(depletion_id);

  if (pFormation && pStage) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);

      int nPoints = surface.PointSize();
      for (int i = 0; i < nPoints; i++) {
        const geo::IPoint &point = surface.Point(i);
        values_v.push_back(formation_property_value(property_, point, pFormation, pStage));
      }
    }
  }
  return values_v;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Displacement
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

Displacement &IT_Geomec_Interface_Surface::BD_displacement(int reservoir_id, int displacement_id, int surface_id,
                                                           base::TDisplacementComponentV component_v) {
  IT_Data::TIntV elems_v;
  IT_Data::T3ValuesV subsidence_v;
  IT_Data::TCoordinateV depth_v;

  int nComponents = component_v.size();

  Displacement &displacement = *(new Displacement());

  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME

  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      T_Elem_And_Val_And_Z_V e_and_s_and_z_v =
          BD_elems_and_vals_and_z(reservoir_id, displacement_id, surface_id, component_v, i, C_Displacement);

      for (int i = 0; i < e_and_s_and_z_v.size(); i++) {
        elems_v.push_back(e_and_s_and_z_v[i].elem_id);
        subsidence_v.push_back(e_and_s_and_z_v[i].val);
        depth_v.push_back(e_and_s_and_z_v[i].z);
      }
    }
  }

  Printer::instance()->debug(
      "get : disp : surf : depth : res_id:%d -> num: elems:%d subs:%d depth:%d header:%s wdat:%.2f", reservoir_id,
      elems_v.size(), subsidence_v.size(), depth_v.size(), base::displacement_header(component_v).c_str(), dummy::WDAT);

  displacement.elems(elems_v);
  displacement.subsidence(subsidence_v);
  displacement.z(depth_v);
  displacement.header(base::displacement_header(component_v));
  displacement.wdat(dummy::WDAT); // FIXME: hard-coded

  return displacement;
}

Displacement &IT_Geomec_Interface_Surface::RES_displacement(int reservoir_id, int surface_id, int depletion_id,
                                                            int bd_composite_id,
                                                            base::TDisplacementComponentV component_v) {
  IT_Data::T3ValuesV subsidence_v;
  IT_Data::TIntV elems_v;

  Displacement &displacement = *(new Displacement());

  CDepletionStage *pStage = base::depletion_stage(depletion_id);
  CFormationBase *pFormation = base::formation(reservoir_id);

  if (pStage && pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    int num_volumes = t.VolumeSize();
    for (int i = 0; i < num_volumes; i++) {
      geo::CBodyGroup::CSideSurface &surface = t.Volume(i).Volume().SideSurface(surface_id);

      std::vector<int> elems_vol_v = BD_elems(reservoir_id, i, surface_id, C_Displacement,
                                              bd_composite_id); // gathered only points in the convex hull
      elems_v.insert(elems_v.end(), elems_vol_v.begin(), elems_vol_v.end());

      for (int j = 0; j < elems_vol_v.size(); j++)
        subsidence_v.push_back(
            G_element_component_value(surface.Face(elems_v[j]), component_v, pStage, C_Displacement));
    }
  }
  Printer::instance()->debug("get : disp : surf : res_id:%d -> num: points:%d subs:%d header:%s wdat:%.2f",
                             reservoir_id, elems_v.size(), subsidence_v.size(),
                             base::displacement_header(component_v).c_str(), dummy::WDAT);

  displacement.elems(elems_v);
  displacement.subsidence(subsidence_v);
  displacement.header(base::displacement_header(component_v));
  displacement.wdat(dummy::WDAT); // FIXME: hard-coded

  return displacement;
}

//--------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------- NOT USED
//------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
/*
IT_Data::T3ValuesV IT_Geomec_Interface_Surface::measured_subsidence(  int reservoir_id, int displacement_id,
base::TDisplacementComponentV component_v, int volume_idx  )
{
  IT_Data::T3ValuesV subsidence_v;

  int nComponents = component_v.size();

  geo::CBodyGroup::CSideSurface* surface = G_surface( reservoir_id, volume_idx, surface_idx()  );
  if( surface )
  {
    //subsidence
    IValueComposite* composite = base::G_BD_composite( IDT_VALUETYPE_DISPLACEMENT, displacement_id ).first;

    int num_points_per_elem = surface->Face( 0 ).NrOfPoints();

    int nFaces = surface->FaceSize();
    for( int i=0; i< nFaces; i++ )
    {
      const geo::IFace& face = surface->Face( i );

      IT_Data::T3Values* values_a = new IT_Data::T3Values[ num_points_per_elem ]; // volume -> 4 points
      for( int i=0; i < num_points_per_elem; i++ )
        values_a[ i ].v1 = values_a[ i ].v2 = values_a[ i ].v3 = NaN;

      for( base::TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it )
      {
        base::eComponent _type = (*it);

        std::vector< geo::CValue > elem_values_v = composite->Component( _type ).ScalarData().ValueElement( face );
//values per each point in the element

        int p=0;
        for( std::vector<geo::CValue>::iterator points_it = elem_values_v.begin(); points_it != elem_values_v.end();
++points_it )
        {
          double value = (*points_it).Value();

          if(			_type == base::X )	values_a[ p ].v1 = value;
          else if(	_type == base::Y )	values_a[ p ].v2 = value;
          else if(	_type == base::Z )	values_a[ p ].v3 = value;

          p++;
        }
      }
      for( int j=0; j< num_points_per_elem; j++ )
        subsidence_v.push_back( values_a[ j ] );

      delete[] values_a;
    }
  }
  return subsidence_v;
}


TPointsAndSubsidencesP IT_Geomec_Interface_Surface::measured_subsidence_and_points(  int reservoir_id, int
displacement_id,  base::TDisplacementComponentV component_v, int volume_idx  )
{
  IT_Data::TPointsV points_v;
  IT_Data::T3ValuesV subsidence_v;

  int nComponents = component_v.size();

  geo::CBodyGroup::CSideSurface* surface = G_surface( reservoir_id, volume_idx, surface_idx()  );
  if( surface )
  {
    //subsidence
    std::pair<IValueComposite*,CPointSet*> composite_p = base::G_BD_composite( IDT_VALUETYPE_DISPLACEMENT,
displacement_id ); IValueComposite* composite = composite_p.first; CPointSet* point_set = composite_p.second;

    int num_points_per_elem = surface->Face( 0 ).NrOfPoints();

    int nFaces = surface->FaceSize();

    //points_v.reserve( nPoints * nFaces );

    for( int i=0; i< nFaces; i++ )
    {
      const geo::IFace& face = surface->Face( i );

      //points
      int in_hull[ 3 ]={0,0,0}; //FIXME: size hard-coded
      int nPoints = face.NrOfPoints();
      for( int i=0; i< nPoints; i++ )
      {
        const geo::IPoint& point = face.Point( i );
        if( !IN_HULL_FILTER || point_set->PointInConvexHull( point ) )
        {
          points_v.push_back( IT_Data::TPoint{ point.X(), point.Y(), point.Z() } );
          in_hull[ i ]=1;
        }
      }

      IT_Data::T3Values* values_a = new IT_Data::T3Values[ num_points_per_elem ]; // volume -> 4 points
      for( int i=0; i < num_points_per_elem; i++ )
        values_a[ i ].v1 = values_a[ i ].v2 = values_a[ i ].v3 = NaN;

      for( base::TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it )
      {
        base::eComponent _type = (*it);

        std::vector< geo::CValue > elem_values_v = composite->Component( _type ).ScalarData().ValueElement( face );
//values per each point in the element

        int p=0;
        for( std::vector<geo::CValue>::iterator points_it = elem_values_v.begin(); points_it != elem_values_v.end();
++points_it )
        {
          if( !IN_HULL_FILTER || in_hull[ p ] )
          {
            double value = (*points_it).Value();
            double& val = _type == base::X ? values_a[ p ].v1 : _type == base::Y ? values_a[ p ].v2 : values_a[ p ].v3;
            val = value;
          }
          p++;
        }
      }
      for( int j=0; j< num_points_per_elem; j++ )
        if( !IN_HULL_FILTER || in_hull[ j ] )
          subsidence_v.push_back( values_a[ j ] );

      delete[] values_a;
    }
  }
  return std::make_pair( points_v, subsidence_v );
}

geo::CPoint IT_Geomec_Interface_Surface::translate_point_to_face( const geo::CPoint& point, const geo::IFace& face  )
{
  geo::CVector normal = face.Normal();
  double signedDist = face.SignedDistance(point, true);

  geo::CPoint trans(point.X() - signedDist * normal.X(), point.Y() - signedDist * normal.Y(), point.Z() - signedDist *
normal.Z());

  return trans;
}

geo::CTriangle IT_Geomec_Interface_Surface::face_triangle( const geo::CPoint& point, const geo::IFace& face  )
{
  geo::CPoint tmp_p[ 3 ];

  for( int k = 0; k < 3; ++k )
    tmp_p[k].Set( face.Point(k).X(), face.Point(k).Y(), point.Z());

  geo::CTriangle tri(tmp_p[0], tmp_p[1], tmp_p[2]);

  return tri;
}

//FIXME: Two passages so no optimal
Displacement& IT_Geomec_Interface_Surface::displacement_DEPRECATED( int reservoir_id, int depletion_id,
base::TDisplacementComponentV component_v )
{
  //Printer::instance()->debug("get_surface_displacement( %d )", reservoir_id);

  IT_Data::TPointsV points_v;
  IT_Data::T3ValuesV subsidence_v;

  int nComponents = component_v.size();

  Displacement& displacement = *( new Displacement() );

  CDepletionStage* pStage = base::depletion_stage( depletion_id );
  CFormationBase* pFormation = base::formation( reservoir_id ); //FIXME

  if( pStage && pFormation )
  {
    CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

    int num_volumes = t.VolumeSize();
    for( int i = 0; i < num_volumes; i++ )
    {
      geo::CBodyGroup::CSideSurface& surface = t.Volume( i ).Volume().SideSurface( surface_idx() );
      int nPoints = surface.PointSize();
      //points_v.reserve( nPoints );

      //points, subsidence
      for( int i=0; i< nPoints; i++ )
      {
        //points
        const geo::IPoint& point = surface.Point( i );
        points_v.push_back( IT_Data::TPoint{ point.X(), point.Y(), point.Z() } );

        //subsidence : //[dx dy dz]
        IT_Data::T3Values values={ NaN, NaN, NaN };
        for( int j = 0; j < nComponents; j++ )
        {
          base::eComponent _type = component_v[ j ];

          double& val = _type == base::X ? values.v1 : _type == base::Y ? values.v2 : values.v3;

          IVectorResult::VECTOR_COMPONENT type_ = base::geomec_component_type( _type );
          const IVectorResult::CVectorComponent* component = base::G_RES_component_ONE( C_Displacement, pStage, type_ );

          geo::CValue c_val = component ? component->ValuePoint( point ) : NaN;
          val = c_val.Value();
        }
        subsidence_v.push_back( values );
      }

    }
  }
  Printer::instance()->debug("get : disp : surf : res_id:%d -> num: points:%d subs:%d header:%s wdat:%.2f",
    reservoir_id, points_v.size(), subsidence_v.size(), base::displacement_header( component_v ).c_str(), dummy::WDAT );

  //displacement.points(		points_v );
  displacement.subsidence(	subsidence_v );
  displacement.header(		base::displacement_header( component_v ) );
  displacement.wdat(			dummy::WDAT );		//FIXME: hard-coded

  return displacement;
}


int IT_Geomec_Interface_Surface::top_surface_idx_from_file( int reservoir_id )
{
  config::instance().reload();
  if( config::instance().keyExists( "SURFACE_NAME" ) )
  {
    std::string target_name = config::instance().getValueOfKey<std::string>( "SURFACE_NAME" );

    CFormationBase* pFormation = base::formation( reservoir_id ); //FIXME
    if( pFormation )
    {
      CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

      int num_volumes = t.VolumeSize();
      for( int i = 0; i < num_volumes; i++ )
      {
        int num_surfaces = t.Volume( i ).Volume().SideSurfaceSize();
        for( int j=0; j < num_surfaces; j++ )
        {
          geo::CBodyGroup::CSideSurface& surface = t.Volume( i ).Volume().SideSurface( j );
          std::string surface_name = static_cast<geo::CTetSurface&>(surface).SurfaceDesc().Name();
          if( surface_name == target_name )
            return j;
        }
      }
    }
  }
  return DEF_TOP_SURFACE_IDX;
}
*/