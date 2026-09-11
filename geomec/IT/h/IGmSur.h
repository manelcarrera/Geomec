#pragma once

#include "IGm.h"

class MaterialParameter;

typedef std::pair<IT_Data::TPointsV, IT_Data::T3ValuesV> TPointsAndSubsidencesP;

namespace gm{

  namespace model{
    namespace formation{
      namespace surface{
        /* function made for convinience */
        geo::CBodyGroup::CSideSurface* get(  int reservoir_id, int volume_idx, int surface_idx  );
        /* real get surface function */
        geo::CBodyGroup::CSideSurface* get( int reservoir_id, int surface_idx  );
        /**/
        IT_Data::TPointsV _points( int reservoir_id, int surface_id, int volume_idx );
        /**/
        IT_Data::TPointsV points( int reservoir_id, int surface_id );
        /* 
        mesh points indices, bot the order in wich points are retrieved when we ask for them to the formation 
        not used so far and maybe not usefull in any case
        */
        IT_Data::TTriV elems( int reservoir_id, int surface_id );
        /**/
        IT_Data::TTriV _elems( int reservoir_id, int surface_id, int volume_idx );
        namespace properties{
          /**/
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirPropMat property_ );
          /**/
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirPropDep property_ );
          /**/
          IT_Data::TValuesV get( int reservoir_id, int surface_id, eReservoirPropForm prop );
          /* Thickness, etc. */
          double value( eReservoirPropForm prop, const geo::IPoint& point, CFormationBase* pFormation );
          /* 
            Point property value 
            Used by 'reservoir()'
          */
          double value( eReservoirPropDep prop, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage );
          double value( eReservoirPropMat prop, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage );
        }
      } // surface
    } // formation
  } // model
  namespace res{
    namespace composite{
      namespace component{
        namespace surface{
          /**/
          IT_Data::T3Values value( const geo::IFace& face, TDisplacementComponentV& component_v, CDepletionStage* pStage, eComposite composite_ );
          /**/
          double value( const geo::IFace& face, eComponent component_, CDepletionStage* pStage, eComposite composite_ );
          /* It needs the bd composite point set (to get the elems), even if it returns values from the result */
          //IT_Data::TValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, eComponent component_ );
        } // surface
      } // surface
    } // surface
  } // res
  namespace bd{
    namespace composite{
      namespace surface{
      }
    }
    namespace pointset{
      namespace surface{
        /**/
        geo::CTriangle translate_face_to_point( const geo::CPoint& point, const geo::IFace& face  );
      } // surface
    } // pointset
    namespace material{
      namespace surface{
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
        MaterialParameter* param( int reservoir_id, int depletion_id, eReservoirPropMat _material_id, int surface_id );
        /**/
        double param_value( CFormationBase* pFormation, CDepletionStage* pStage, int material_id_, const geo::IFace& body );
      } // surface
    } // material
  } // dbd
}

