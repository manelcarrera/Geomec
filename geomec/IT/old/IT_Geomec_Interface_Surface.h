#ifndef _IT_Geomec_Interface_Surface
#define _IT_Geomec_Interface_Surface

#include "IT_Geomec_Interface.h"

typedef IT_Geomec_Interface base;

class Reservoir;
class Displacement;
class MaterialParameter;

typedef std::pair<IT_Data::TPointsV, IT_Data::T3ValuesV> TPointsAndSubsidencesP;

struct T_Elem_And_Val_And_Z {
  int elem_id;
  IT_Data::T3Values val;
  IT_Data::TCoordinate z;

  T_Elem_And_Val_And_Z(int &elem_id_, IT_Data::T3Values &val_, IT_Data::TCoordinate &z_)
      : elem_id(elem_id_), val(val_), z(z_) {}

  T_Elem_And_Val_And_Z() : elem_id(-1), val(IT_Data::T3Values{-1, -1, -1}), z(-1) {}
};

typedef std::vector<T_Elem_And_Val_And_Z> T_Elem_And_Val_And_Z_V;

class IT_Geomec_Interface_Surface : public IT_Geomec_Interface {

public:
  /*
    DOC UP-TO-DATE

    By now everything is related to surfaces and not to volumnes.

    This function returns the current surface from a reservoir to witch all the operations are referenced.

    User selects the the surface in the GUI.
  */
  // static void surface_idx( int val ){ m_surface_idx = val; };

  //--------------------------------------------------- Formations
  //-----------------------------------------------------------------

  /*
    DOC UPDATE REQUIRED

    Returns points, trianlges, parameters and a header with info on the provided parameters of the topest surface of the
    requested reservoir.

    Re-index neeeded to match points and triangles.

    Paraemeters:
    ------------
      reservoir_id:	reservoir position in the model tree

    Result:
    -------
      Points:			points defined by its coordinates: x,y,z
              3-D doubles vector

      Triangles:		triangles are made of 3 points ids (surface)
              3-D integers vector
              First point_id is '1' (instead of '0') to fulfill 'trimesh' python module needs

      Params:			only 'thickness' so far
              1-D doubles vector

      Header:			hard-coded to 'dz'
              tokens in a string with token delimiter '*'

  */
  static Reservoir &reservoir(int reservoir_id, int surface_id,
                              TReservoirPropertyV properties_v = {Thickness, PoissonsRatio, YoungsModulus,
                                                                  PorePressure},
                              int depletion_id = 0);

  /**/
  IT_Data::TValuesV reservoir_property(int reservoir_id, int surface_id, eReservoirProperty property, int depletion_id);

  /**/
  IT_Data::TTriV reservoir_E(int reservoir_id, int surface_id);

  /*
  NOT USED BUT REFERENCED

  Returns the requested surface property for a given reservoir's depletion stage.

  At this moment it's implemented to retrieve just the poisons ratio.

  Retrieved values are per element.

  Paraemeters:
  ------------
    reservoir_id:	reservoir position in the model tree

    depletion_id:	depletion stage

    parameter_id:	parameter to retrieve
            accepted values (so far):
              poisons ratio
              reduction factor

  Result:
  -------
    Vals:			parameter's values per each surface element
            1-D doubles vector
*/
  static MaterialParameter &material_parameter(int reservoir_id, int depletion_id, eMaterialParameter _material_id,
                                               int surface_id);

  /**/
  static double material_parameter_value(CFormationBase *pFormation, CDepletionStage *pStage, int material_id_,
                                         const geo::IFace &body);

  //--------------------------------------------------- Displacement
  //-----------------------------------------------------------------

  /*
    DOC UPDATE REQUIRED

    Measured surface displacement retrieved from the data storage section after being imported from a file.

    Displacement in any of three components (x,y,z) can be retrieved.

    Values are per point.

    Params:
    -------
      reservoir_id:		reservoir idx for the displacement to be mapped
      displacement_id:	displacement idx (order in the data storage section)
      component_v:		vector any ordered combination of components x, y and z:
                {'x'}, {'y'}, {'z'}, {'x','y'}, {'x','z'}, {'y','z'}, {'x','y','z'}

    Return:
    -------
      points:		reservoir points where the surface has been mapped
            x, y z coordinates
            3-D doubles vector

      subsidence:	dimensions variable subsidence values per point
            doubles vector
            it can be 1-D, 2-D or 3-D depending on the components requested

      header:		informs on the components retrieved
            Examples: 'x y z dz', 'x y z dx dz' ('x y z in the begining is mandatory')

      wdat:		not used, hard-coded
            double value

  */
  static Displacement &BD_displacement(int reservoir_id, int displacement_id, int surface_id,
                                       base::TDisplacementComponentV component_v);

  /*
    Calculated displacement.

    Geomec runs the mdoel using the depletion provided by the Inversion Tool.
  */
  static Displacement &RES_displacement(int reservoir_id, int surface_id, int depletion_id, int bd_composite_id,
                                        base::TDisplacementComponentV component_v);

  IT_Geomec_Interface_Surface() {};

private:
  /* Elem id, subsidence and depth for a point in a measured displacmenet point set for a point and a given surface */
  static T_Elem_And_Val_And_Z BD_elem_and_val_and_z(const geo::IPoint &point, geo::CBodyGroup::CSideSurface &surface,
                                                    IValueComposite *composite,
                                                    base::TDisplacementComponentV &component_v);

  /* Point (some) properties values */
  static IT_Data::T4Values formation_properties_values(TReservoirPropertyV &properties_v, const geo::IPoint &point,
                                                       CFormationBase *pFormation, CDepletionStage *pStage = nullptr);

  /*
    Point property value
    Used by 'reservoir()'
  */
  static double formation_property_value(eReservoirProperty prop, const geo::IPoint &point, CFormationBase *pFormation,
                                         CDepletionStage *pStage = nullptr);

  /* Used by 'measured_displacement()' */
  static T_Elem_And_Val_And_Z_V BD_elems_and_vals_and_z(int reservoir_id, int composite_idx, int surface_id,
                                                        base::TDisplacementComponentV component_v, int volume_idx,
                                                        eComposite composite_id);

  /* Used by 'displacement_from_point_set()' */
  static IT_Data::TIntV BD_elems(int reservoir_id, int volume_idx, int surface_id, eComposite composite_id,
                                 int composite_idx);

  /* Used by 'BD_elems()'*/
  static geo::CTriangle translate_face_to_point(const geo::CPoint &point, const geo::IFace &face);

  //--------------------------------- GENERIC -------------------------------------

  /**/
  static geo::CBodyGroup::CSideSurface *G_surface(int reservoir_id, int volume_idx, int surface_idx);

  /**/
  static IT_Data::T3Values G_element_component_value(const geo::IFace &face, base::TDisplacementComponentV &component_v,
                                                     CDepletionStage *pStage, eComposite composant_);

  /**/
  static IT_Data::TPointsV G_points(int reservoir_id, int surface_id, int volume_idx);

  /**/
  static int G_num_volumes(int reservoir_id);

  /**/
  static int BD_elem(const geo::IPoint &point, geo::CBodyGroup::CSideSurface &surface);

  /**/
  static IT_Data::TIntV BD_elems(CPointSet *ps, geo::CBodyGroup::CSideSurface &surface);

  /**/
  static IT_Data::TIntV BD_elems(int reservoir_id, int surface_id, eComposite composite_, int composite_idx);

private: /* NOT USED */
  /* NOT USED */
  // static IT_Data::T3ValuesV measured_subsidence(  int reservoir_id, int displacement_id,
  // base::TDisplacementComponentV component_v, int volume_idx );

  /* NOT USED */
  // static TPointsAndSubsidencesP measured_subsidence_and_points(  int reservoir_id, int displacement_id,
  // base::TDisplacementComponentV component_v, int volume_idx  );

  /* NOT USED */
  // static geo::CPoint translate_point_to_face( const geo::CPoint& point, const geo::IFace& face  );

  /* NOT USED */
  // static geo::CTriangle face_triangle( const geo::CPoint& point, const geo::IFace& face  );

  /*
    NOT USED

    Surface displacement for a given reservoir and depletion stage.

    Displacement in any of three components (x,y,z) can be retrieved.

    Values are per point.

    Params:
    -------
      reservoir_id:	idx
      depletion_id	idx
      component_v:	vector any ordered combination of components x, y and z:
              {'x'}, {'y'}, {'z'}, {'x','y'}, {'x','z'}, {'y','z'}, {'x','y','z'}

    Return:
    -------
      points:		x, y z coordinates
            3-D doubles vector

      subsidence:	dimensions variable subsidence values
            doubles vector
            it can be 1-D, 2-D or 3-D depending on the components requested

      dx,dy,dz:	1-D (each) subsidence values per component and per point

      header:		informs on the components retrieved
            Examples: 'x y z dz', 'x y z dx dz' ('x y z in the begining is mandatory')

      wdat:		not used, hard-coded
            double value

  */
  // static Displacement& displacement_DEPRECATED( int reservoir_id, int depletion_id, base::TDisplacementComponentV
  // component_v = { base::eComponent::Z } );

  // static int top_surface_idx_from_file( int reservoir_id );
};

#endif