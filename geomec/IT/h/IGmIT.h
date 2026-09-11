#pragma once

#include "IGm.h"
#include "IGmSur.h"

class Reservoir;
class Displacement;

namespace gm{
	namespace model{
		namespace formation{
			namespace IT{
				/**/
				IT_Data::TTriV elems( int reservoir_id, int surface_id );
				namespace reservoir{
					enum eReservoirProperty{ Thickness,	PoissonsRatio, YoungsModulus, PorePressure, Strain, Cm };
					typedef std::vector<eReservoirProperty> TReservoirPropertyV;
					/*
					DOC UPDATE REQUIRED

					Returns points, trianlges, parameters and a header with info on the provided parameters of the topest surface of the requested reservoir.

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
					Reservoir* get( int reservoir_id, int surface_id, TReservoirPropertyV properties_v={ Thickness, PoissonsRatio, YoungsModulus, PorePressure}, int depletion_id = 0 );
					namespace properties{
						/**/
						IT_Data::T4ValuesV values( int reservoir_id, int surface_id, TReservoirPropertyV properties_v, int depletion_id );
						/* Point (some) properties values */
						IT_Data::T4Values values( TReservoirPropertyV& properties_v, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage = nullptr );
					}
				}
			} // IT
		} // formation 
		namespace depletion{
			namespace IT{
				namespace pressure{
					/**/
					void add(	int reservoir_id, 
								const IT_Data::TPointsV& points_v, 
								const IT_Data::TValuesV& pressures_v,
								eDepletionType _type = D_Pore_Pressure,
								int depletion_id = LAST_DEPLETION_STAGE,
								bool is_branch = true );
					/**/
					void add(	int reservoir_id,			// Formation to wich add the provided pressure
								eDepletionType _type,		// Format of the provided pressure : Strain, Compressibility or Pore_Pressure
								int depletion_id,			// Default params adds a new depletion stage
								bool is_branch, 			// Branch | Results
								Depletion* depletion);		// Pressure to add : points + pressure

				} // pressure
			} // IT
		} // depletion
	} // model
	namespace res{
		namespace composite{
			namespace component{
				namespace IT{
					/**/
					IT_Data::T3ValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, TDisplacementComponentV component_v );
					/**/
					IT_Data::TValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, eComponent component );
				} // IT
			} // component
			namespace IT{
				namespace displacement{
					/*
					Calculated displacement.

					Geomec runs the mdoel using the depletion provided by the Inversion Tool. 
					*/
					Displacement* get( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, TDisplacementComponentV component_v );
				} // displacement
			} // IT
		} // composite
	} // res
	namespace bd{
		namespace composite{
			namespace component{
				namespace IT{
					/* bd composite values twice filtered : HULK + point to surfce mapping */
					IT_Data::T3ValuesV values( eComposite composite_id, int composite_idx, TDisplacementComponentV component_v, std::vector< const geo::IPoint* > points );
				} // IT
			} // component
			namespace IT{
				/**/
				std::vector< const geo::IPoint* > points( int reservoir_id, int surface_id, eComposite composite_id, int composite_idx );
				/* the nearest (to a given point) elem (id) in a surface */
				int elem( const geo::IPoint& point, geo::CBodyGroup::CSideSurface* surface );
				/* elemnet id list; matches points in a bd compositepoint set with formation surface elements; what element? the closest to each point in the point set */
				IT_Data::TIntV elems( int reservoir_id, int surface_id, eComposite composite_id, int composite_idx );
				/**/
				IT_Data::TIntV elems( int reservoir_id, int surface_id, int volume_id, eComposite composite_id, int composite_idx );
				namespace displacement{
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

						wdat:		not used, hard-coded
									double value

					*/
					Displacement* get( int reservoir_id, int displacement_id, int surface_id, TDisplacementComponentV component_v );
				} // displacement
			} // IT
		} // 
		namespace pointset{
		}
		namespace material{
		}
	} // bd
} // gm