#include "stdafx.h"

#include "IT_Geomec_Interface_Volume.h"

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
typedef IT_Geomec_Interface_Dummy dummy;

static const IT_Data::T3Values EMPTY_3_VALUES = {-9999, -9999, -9999};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Formations
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

MaterialParameter &IT_Geomec_Interface_Volume::material_parameter(int reservoir_id, int depletion_id,
                                                                  eMaterialParameter _material_id) {
  IT_Data::TValuesV values_v;

  int material_id_ = material_id(_material_id);

  MaterialParameter &param = *(new MaterialParameter());

  CFormationBase *pFormation = base::formation(reservoir_id);
  CDepletionStage *pStage = base::depletion_stage(depletion_id);

  if (pFormation && pStage) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    geo::IElementSet &elements = t.ElementSet(0).ElementSet();

    for (int i = 0; i < elements.ElementSize(); i++) // elems loop
    {
      const geo::IElement &e = elements.Element(i);

      const CFFMaterial &mat = pFormation->Material(*pStage).Material(e);

      if (mat.IsParameter(material_id_))
        values_v.push_back(mat.ParameterValue(material_id_));
    }
  }

  param.vals(values_v);

  return param;
}

//--------------------------------------------------------------------------------------------------------------------------------------
Reservoir &IT_Geomec_Interface_Volume::reservoir(int reservoir_id) {
  //------------------------------------- for the thickness
  geo::CPoint point_top;
  geo::CPoint point_bottom;
  //------------------------------------- elems / points
  IT_Data::TTetraV tetra_v;
  IT_Data::TPointsV points_v;
  IT_Data::T4ValuesV params_v;
  //-------------------------------------

  Reservoir &reservoir = *(new Reservoir(Reservoir::eElemType::Tetra));

  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME
  if (pFormation) {
    CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

    const int num_points_per_elem = 4; // FIXME

    geo::IElementSet &elements = t.ElementSet(0).ElementSet();
    // geo::IElementSet& elements = get_elements( formation );

    for (int i = 0; i < elements.ElementSize(); i++) // elems loop
    {
      const geo::IElement &e = elements.Element(i);

      IT_Data::TTetra elem_d = {e.PointIndex(0), e.PointIndex(1), e.PointIndex(2), e.PointIndex(3)};
      tetra_v.push_back(elem_d);

      IT_Data::T4Values *values_a = new IT_Data::T4Values[num_points_per_elem];

      for (int j = 0; j < e.NrOfPoints(); j++) // points per elem
      {
        const geo::IPoint &p = e.Point(j);

        points_v.push_back(IT_Data::TPoint{p.X(), p.Y(), p.Z()});

        double _value = pFormation->ThicknessAt(p, point_top, point_bottom, nullptr)
                            .Value(); // FIXME: paralalize / it takes 5s. first time, then 0.05s.
        // params_v.push_back( _value );
        values_a[i].v1 = _value;
      }
      for (int j = 0; j < num_points_per_elem; j++)
        params_v.push_back(values_a[j]);
      delete[] values_a;
    }
  }

  // remove_duplicates<IT_Data::TElem>( elemsV );
  // remove_duplicates<IT_Data::TPoint>( pointsV );

  reservoir.tetra(tetra_v);
  reservoir.points(points_v);
  reservoir.params(params_v);
  reservoir.header(dummy::HEADER_RES); // FIXME: hard-coded

  return reservoir;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Displacement
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

IT_Data::TPointsV IT_Geomec_Interface_Volume::points(int reservoir_id) {
  IT_Data::TPointsV points_v;

  CFormationBase *pFormation = base::formation(reservoir_id);
  if (pFormation) {
    geo::IElementSet &es = pFormation->ElementSet(0).ElementSet();
    int nPointsPerElem = es.Element(0).NrOfPoints();
    int size_v = es.ElementSize() * nPointsPerElem;
    points_v.reserve(size_v);

    for (int e = 0; e < es.ElementSize(); e++) // elems
    {
      const geo::IElement &element = es.Element(e);

      for (int p = 0; p < element.NrOfPoints(); p++) // point per elem
      {
        // point: x, y, z
        const geo::IPoint &point = element.Point(p);
        IT_Data::TPoint point_d = {point.X(), point.Y(), point.Z()};
        points_v.push_back(point_d);
      }
    }
  }
  return points_v;
}

Displacement &IT_Geomec_Interface_Volume::measured_displacement_DEPRECATED(int reservoir_id, int displacement_id,
                                                                           base::TDisplacementComponentV component_v) {
  IT_Data::TPointsV points_v;
  // int nComponents = component_v.size();

  Displacement &displacement = *(new Displacement());

  CFormationBase *pFormation = base::formation(reservoir_id);
  if (pFormation) {
    geo::IElementSet &es = pFormation->ElementSet(0).ElementSet();

    int nPointsPerElem = es.Element(0).NrOfPoints();
    int size_v = es.ElementSize() * nPointsPerElem;
    points_v.reserve(size_v);

    points_v = points(reservoir_id);
  }

  // subsidence
  IT_Data::T3ValuesV subsidence_v = measured_subsidence(reservoir_id, displacement_id, component_v);

  // displacement.points(		points_v );
  displacement.subsidence(subsidence_v);
  displacement.header(base::displacement_header(component_v)); // FIXME: hard-coded
  displacement.wdat(dummy::WDAT);                              // FIXME: hard-coded

  return displacement;
}

IT_Data::T3ValuesV IT_Geomec_Interface_Volume::measured_subsidence(int reservoir_id, int displacement_id,
                                                                   base::TDisplacementComponentV component_v) {
  IT_Data::T3ValuesV subsidence_v;

  CFormationBase *pFormation = base::formation(reservoir_id);
  if (pFormation) {
    geo::IElementSet &es = pFormation->ElementSet(0).ElementSet();

    int num_points_per_elem = es.Element(0).NrOfPoints();

    IValueComposite *composite = base::measured_displacement_composite(displacement_id).first;

    // int num_points_per_elem = 4;

    for (int e = 0; e < es.ElementSize(); e++) // elems
    {
      const geo::IElement &element = es.Element(e);

      // IT_Data::TValuesU u_values_a[ 4 ]; // volume -> 4 points

      IT_Data::T3Values *values_a = new IT_Data::T3Values[num_points_per_elem]; // volume -> 4 points

      for (base::TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it) {
        base::eComponent _type = (*it);

        std::vector<geo::CValue> elem_values_v =
            composite->Component(_type).ScalarData().ValueElement(element); // values per each point in the element

        int p = 0;
        for (std::vector<geo::CValue>::iterator points_it = elem_values_v.begin(); points_it != elem_values_v.end();
             ++points_it) {
          double value = (*points_it).Value();

          if (_type == base::X)
            values_a[p].v1 = value;
          else if (_type == base::Y)
            values_a[p].v2 = value;
          else if (_type == base::Z)
            values_a[p].v3 = value;

          p++;
        }
      }
      for (int j = 0; j < num_points_per_elem; j++)
        subsidence_v.push_back(values_a[j]);

      delete[] values_a;
    }
  }
  return subsidence_v;
}
IT_Data::T3ValuesV IT_Geomec_Interface_Volume::subsidence(int reservoir_id, int depletion_id,
                                                          base::TDisplacementComponentV component_v) {
  IT_Data::T3ValuesV subsidence_v;

  int nComponents = component_v.size();

  CDepletionStage *pStage = base::depletion_stage(depletion_id);
  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME

  if (pFormation && pStage) {
    geo::IElementSet &es = pFormation->ElementSet(0).ElementSet();

    for (int e = 0; e < es.ElementSize(); e++) // elems
    {
      const geo::IElement &element = es.Element(e);

      for (int p = 0; p < element.NrOfPoints(); p++) // point per elem
      {
        IT_Data::T3Values values = EMPTY_3_VALUES;
        for (int j = 0; j < nComponents; j++) // components
        {
          base::eComponent _type = component_v[j];
          IVectorResult::VECTOR_COMPONENT type_ = base::geomec_component_type(_type);
          const IVectorResult::CVectorComponent *component = base::displacement_component(pStage, type_);

          double &val = _type == base::X ? values.v1 : _type == base::Y ? values.v2 : values.v3;

          // value: subsidence
          geo::CValue c_val = component ? component->ValuePoint(element.Point(p)) : NaN;
          val = c_val.Value();
        }
        subsidence_v.push_back(values);
      }
    }
  }
  return subsidence_v;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// FIXME: Two passages so no optimal
Displacement &IT_Geomec_Interface_Volume::displacement_DEPRECATED(int reservoir_id, int depletion_id,
                                                                  base::TDisplacementComponentV component_v) {
  IT_Data::TPointsV points_v;
  // IT_Data::TValuesV params_v;
  IT_Data::TValuesV dx_v;
  IT_Data::TValuesV dy_v;
  IT_Data::TValuesV dz_v;

  int nComponents = component_v.size();

  Displacement &displacement = *(new Displacement());

  CDepletionStage *pStage = base::depletion_stage(depletion_id);
  CFormationBase *pFormation = base::formation(reservoir_id); // FIXME

  if (pStage && pFormation) {
    IFormationElementSet &es1 = pFormation->ElementSet(0);
    geo::IElementSet &es2 = es1.ElementSet();

    int nPointsPerElem = es2.Element(0).NrOfPoints();

    int size_v = es2.ElementSize() * nPointsPerElem;

    points_v.reserve(size_v);
    // params_v.reserve( size_v );

    // points, dx,dz, dz
    for (int j = 0; j < nComponents; j++) {
      base::eComponent _type = component_v[j];
      IVectorResult::VECTOR_COMPONENT type_ = base::geomec_component_type(_type);
      const IVectorResult::CVectorComponent *component = base::displacement_component(pStage, type_);

      if (component) {
        IT_Data::TValuesV &values_v = _type == base::X ? dx_v : _type == base::Y ? dy_v : dz_v;

        values_v.reserve(size_v);

        for (int e = 0; e < es2.ElementSize(); e++) // elems
        {
          const geo::IElement &element = es2.Element(e);

          std::vector<geo::CValue> _vector = component->ValueElement(
              element); // it's giving me 4 values... it's one for each point of the element ?? yes, in a tetra model
          for (std::vector<geo::CValue>::iterator it = _vector.begin(); it != _vector.end(); ++it) {
            // value: subsidence
            double value = (*it).Value();
            values_v.push_back(value);
          }
          for (int p = 0; p < element.NrOfPoints(); p++) // point per elem
          {
            // point: x, y, z
            const geo::IPoint &point = element.Point(p);
            IT_Data::TPoint point_d = {point.X(), point.Y(), point.Z()};
            points_v.push_back(point_d);
          }
        }
      }
    }
  }

  // subsidence
  IT_Data::T3ValuesV subsidence_v = IT_Geomec_Interface_Volume::subsidence(reservoir_id, depletion_id, component_v);

  // displacement.points(		points_v );
  displacement.subsidence(subsidence_v);
  displacement.dx(dx_v);
  displacement.dy(dy_v);
  displacement.dz(dz_v);
  displacement.header(base::displacement_header(component_v));
  displacement.wdat(dummy::WDAT); // FIXME: hard-coded

  return displacement;
}
