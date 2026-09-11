#ifndef _IT_Geomec_Interface_Volume
#define _IT_Geomec_Interface_Volume

#include "IT_Geomec_Interface.h"

typedef IT_Geomec_Interface base;

class MaterialParameter;
class Reservoir;
class Displacement;

class IT_Geomec_Interface_Volume : public IT_Geomec_Interface {
public:
  // private:
  //--------------------------------------------------- Formations
  //-----------------------------------------------------------------
  static IT_Data::TPointsV points(int reservoir_id);
  static IT_Data::T3ValuesV subsidence(int reservoir_id, int depletion_id, base::TDisplacementComponentV component_v);
  static IT_Data::T3ValuesV measured_subsidence(int reservoir_id, int displacement_id,
                                                base::TDisplacementComponentV component_v);

  /*
    Returns points, tetrahedrons, parameters and a header with info on the provided parameters of the requested
    reservoir (volume).

    No re-index made.

    Paraemeters:
    ------------
      reservoir_id:	reservoir position in the model tree

    Result:
    -------

      Points:			points defined by its coordinates: x,y,z
              3-D doubles vector

      Tetrahedrons:	tethrahedrosn are made of 4 points ids (volume)
              4-D integers vector
              First point_id is '1' (instead of '0') to fulfill 'trimesh' python module needs

      Params:			only 'thickness' so far
              1-D doubles vector

      Header:			hard-coded to 'dz'
              tokens in a string with token delimiter '*'

  */
  static Reservoir &reservoir(int reservoir_id);
  /*
    Returns the requested volume property for a given reservoir's depletion stage.

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
      Vals:			parameter's values per each volume element
              1-D doubles vector
  */
  static MaterialParameter &material_parameter(int reservoir_id, int depletion_id,
                                               eMaterialParameter _material_id = PoissonsRatio_);

  //--------------------------------------------------- depletion
  //-----------------------------------------------------------------

  //--------------------------------------------------- Displacement
  //-----------------------------------------------------------------
  /*
    Volume displacement for a given reservoir and depletion stage.

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

      subsidence:	dimensions variable subsidence values per point
            doubles vector
            it can be 1-D, 2-D or 3-D depending on the components requested

      dx,dy,dz:	1-D (each) subsidence value per component and per point

      header:		informs on the components retrieved
            Examples: 'x y z dz', 'x y z dx dz' ('x y z in the begining is mandatory')

      wdat:		not used, hard-coded
            double value

  */
  static Displacement &displacement_DEPRECATED(int reservoir_id, int depletion_id,
                                               base::TDisplacementComponentV component_v = {base::eComponent::Z});

  /*
    Measured volume displacement retrieved from the data storage section after being imported from a file.

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
  static Displacement &measured_displacement_DEPRECATED(int reservoir_id, int displacement_id,
                                                        base::TDisplacementComponentV component_v);

public:
  IT_Geomec_Interface_Volume() {};
};

#endif