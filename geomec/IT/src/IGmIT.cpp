#include "stdafx.h"

#include "IGmIT.h"

#include "PointSetManager.h"
#include "IT_Printer.h"
#include "Reservoir.h"
#include "Displacement.h"
#include "Depletion.h"

#include "FormationBase.h"
#include "TetraFormation.h"
#include "Pressure.h"
#include "ISceneWrapper.h" // doc()->CurrentScene()
#include "ModelTreeView.h" // CTreeCtrlBase::AllowPaint( bool )
#include "GlobalMessage.h"

/*

  IT2:

  1. Measured dipslacment:

  points:	IT_Data::TPointsV bd::composite::points( eComposite composite_, int composite_idx );
  WDAT:	NOT IMPLEMENTED
  subs:	IT_Data::TValuesV bd::composite::values( eComposite composite_, int composite_idx, eComponent component_ );


  2. Full material models

  TODO

  3.1 
    PP,Cm,strain:	IT_Data::TValuesV formation::surface::properties::values( int reservoir_id, int surface_id, int depletion_id, eReservoirProperty prop )

    Left to do: Cm

  3.2
    Jeroen: If stage removed do we need to 'unlink' ps from depletion or is it automatically done ? -> NOT NEDDEDeven if orphan info (UNUSED but present)

    gm::model::depletion::
      void branch( int depletion_id = LAST_DEPLETION_STAGE, bool is_branch = true );
      int size();
      TDepletionL& list();
      CDepletionStage* get( int idx = LAST_DEPLETION_STAGE );
      void add( bool is_branch = true );
      void remove( int depletion_id = LAST_DEPLETION_STAGE );

    HANDLE 'NodalValueSet' from 'PointSetManager'

  3.3

    LEFT TO DO.

    JH: Look at 'geomec_shell' interface context

  4. Results: + -> 
      Modelled strain:	DONE
      Stress:				DONE
      Temperature:		DONE

      IT_Data::TValuesV gm::res::composite::component::surface::values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, eComponent component_ );

  5. Data storage (BD)

  5.1
    IT_Data::TValuesV bd::composite::values( eComposite composite_, int composite_idx, eComponent component_ );

  5.2 Temperature to stage (as for pressure)

    Next method (but the name) is generic enought to hadle pressures and temperatures

    depletion::IT::pressure::add(	int reservoir_id, 
                    const IT_Data::TPointsV& points_v, 
                    const IT_Data::TValuesV& pressures_v,
                    eDepletionType _type,
                    int depletion_id,
                    bool is_branch )
*/

namespace{
  const double WDAT = 4000.0;
}

namespace gm{
  namespace model{
    namespace formation{
      namespace IT{
        IT_Data::TTriV elems( int reservoir_id, int surface_id )
        {
          IT_Data::TTriV elems_v;

          CFormationBase* pFormation = model::formation::get( reservoir_id ); //FIXME

          if( pFormation )
          {
            CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

            std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare> point_reindex_map;

            const int num_points_per_elem = 3;//surface.Face( 0 ).NrOfPoints();

            int num_vol = model::formation::num_volumes( reservoir_id  );
            for( int i = 0; i < num_vol; i++ )
            {
              geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, i, surface_id  );
              if( surface )
              {
                // (1) points
                int nPoints = surface->PointSize();
    
                for( int j=0; j< nPoints; j++ )
                {
                  const geo::IPoint& point = surface->Point( j );
                  IT_Data::TPoint data_point{ point.X(), point.Y(), point.Z() };
                  point_reindex_map[ data_point ] = j + 1;
                }		

                // (2) elems: triangles: face is the abstract of a triangle
                int nFaces = surface->FaceSize();
                for( int j=0; j< nFaces; j++ )
                {
                  const geo::IFace& face = surface->Face( j );

                  IT_Data::TPoint point_d[ num_points_per_elem ] = 
                  { 
                    { face.Point(0).X(), face.Point(0).Y(), face.Point(0).Z() },
                    { face.Point(1).X(), face.Point(1).Y(), face.Point(1).Z() },
                    { face.Point(2).X(), face.Point(2).Y(), face.Point(2).Z() } 
                  };

                  int reindexed[ num_points_per_elem ] = { -1, -1, -1 };

                  for (int k = 0; k < num_points_per_elem; ++k)
                    reindexed[ k ] = point_reindex_map[ point_d[ k ] ]; 

                  IT_Data::TTri elem_d = { reindexed[ 0 ], reindexed[ 1 ], reindexed[ 2 ] };
                  elems_v.push_back( elem_d );
                }
              }
            }
          }
          return elems_v;
        }
        namespace reservoir{
          Reservoir* get( int reservoir_id, int surface_id, TReservoirPropertyV properties_v, int depletion_id )
          {
            Reservoir* reservoir = new Reservoir( Reservoir::eElemType::Tri );

            reservoir->tri(		IT::elems(			reservoir_id, surface_id ) );
            reservoir->points(	surface::points(	reservoir_id, surface_id ) );
            reservoir->params(	properties::values( reservoir_id, surface_id, properties_v, depletion_id ) );

            return reservoir;
          }
          namespace properties
          {
            IT_Data::T4ValuesV values( int reservoir_id, int surface_id, TReservoirPropertyV properties_v, int depletion_id )
            {
              IT_Data::T4ValuesV params_v;

              CFormationBase* pFormation = model::formation::get( reservoir_id ); //FIXME
              CDepletionStage* pStage = model::depletion::get( depletion_id );

              if( pFormation && pStage )
              {
                CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

                int num_vol = model::formation::num_volumes( reservoir_id  );
                for( int i = 0; i < num_vol; i++ )
                {
                  geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, i, surface_id  );
                  if( surface )
                  {
                    int nPoints = surface->PointSize();
                    for( int j=0; j< nPoints; j++ )
                    {
                      const geo::IPoint& point = surface->Point( j );
                      params_v.push_back( properties::values( properties_v, point, pFormation, pStage ) );
                    }		
                  }
                }
              }
              return params_v;
            }
            IT_Data::T4Values values( TReservoirPropertyV& properties_v, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage )
            {
              int num_props = properties_v.size();

              IT_Data::T4Values values={ NaN, NaN, NaN, NaN };
              for( int j = 0; j < num_props; j++ )
              {
                eReservoirProperty prop = properties_v[ j ]; 
                double& val = prop == Thickness ? values.v1 : prop == PoissonsRatio ? values.v2 : prop == YoungsModulus ? values.v3 : values.v4;
                val =	prop == Thickness		?	formation::surface::properties::value( P1_Thickness, point, pFormation ) : 
                    prop == PoissonsRatio	?	formation::surface::properties::value( P2_PoissonsRatio, point, pFormation, pStage ) :
                    prop == YoungsModulus	?	formation::surface::properties::value( P2_YoungsModulus, point, pFormation, pStage ) :
                                  formation::surface::properties::value( P3_PorePressure, point, pFormation, pStage );
              }
              return values;
            }
          }
        } // reservoir
      } // IT 
    } // formation
    namespace depletion{
      namespace IT{
        namespace pressure{
          void add(	int reservoir_id, 
                const IT_Data::TPointsV& points_v, 
                const IT_Data::TValuesV& pressures_v,
                eDepletionType _type,
                int depletion_id,
                bool is_branch )
          {
            if( points_v.size() == 0 || ( points_v.size() != pressures_v.size() ) )
            {
              if (points_v.size() == 0)
                _m()->msg("Set property: received zero points; ignoring");
              else
                if (points_v.size() != pressures_v.size())
                  _m()->msg("Set property: different number of points and values; ignoring");
            }
            else
            {
              if( depletion_id == NEW_DEPLETION_STAGE )
              {
                gm::model::depletion::add( is_branch );
                depletion_id = LAST_DEPLETION_STAGE;
              }

              CDepletionStage* pStage = gm::model::depletion::get( depletion_id ); //FIXME
              CFormationBase* pFormation = gm::model::formation::get( reservoir_id );

              if( pStage &&  pFormation )
              {
                int ps_id = 0;

                bool r1 = gm::bd::pointset::create(		ps_id, points_v ); // if ps_id already cretaed then create does nothing
                bool r2 = depletion_id == LAST_DEPLETION_STAGE ?	gm::bd::pointset::replace_values( ps_id, pressures_v, -1 ) : //FIXME: '-1' hard-coded
                                          gm::bd::pointset::add_values( ps_id, _type, pressures_v );

                bool r3 = gm::bd::pointset::link(		ps_id, reservoir_id, depletion_id, true ); // drow: Disable + Enable
              }
            }
          }
          void add(	int reservoir_id,
                eDepletionType _type,
                int depletion_id,
                bool is_branch,
                Depletion* depletion )
          {
            add( reservoir_id, depletion->points(), depletion->pressure(), _type, depletion_id, is_branch );
          }
        }
      } // IT
    } // depletion
  } // model
  namespace res{
    namespace composite{
      namespace component{
        namespace IT{
          IT_Data::T3ValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, TDisplacementComponentV component_v )
          {
            IT_Data::T3ValuesV values_v;

            CDepletionStage* pStage = model::depletion::get( depletion_id );
            CFormationBase* pFormation = model::formation::get( reservoir_id );

            if( pStage && pFormation )
            {
              CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

              int num_vol = model::formation::num_volumes( reservoir_id  );
              for( int i = 0; i < num_vol; i++ )
              {
                geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, i, surface_id  );
                if( surface )
                {
                  std::vector<int> elems = bd::composite::IT::elems( reservoir_id, i, surface_id, composite_, bd_composite_id ); // gathered only points in the convex hull 
                  for( int j = 0; j < elems.size(); j++ )
                    values_v.push_back( res::composite::component::surface::value( surface->Face( elems[ j ] ), component_v, pStage, composite_ ) );
                }
              }
            }
            return values_v;
          }
          IT_Data::TValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, eComponent component )
          {
            IT_Data::TValuesV values_v;

            CDepletionStage* pStage = model::depletion::get( depletion_id );
            CFormationBase* pFormation = model::formation::get( reservoir_id );

            if( pStage && pFormation )
            {
              CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

              int num_vol = model::formation::num_volumes( reservoir_id  );
              for( int i = 0; i < num_vol; i++ )
              {
                geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, i, surface_id  );
                if( surface )
                {
                  std::vector<int> elems = bd::composite::IT::elems( reservoir_id, i, surface_id, composite_, bd_composite_id ); // gathered only points in the convex hull 
                  for( int j = 0; j < elems.size(); j++ )
                    values_v.push_back( res::composite::component::surface::value( surface->Face( elems[ j ] ), component, pStage, composite_ ) );
                }
              }
            }
            return values_v;
          }
        } //IT
      } // component
      namespace IT{
        namespace displacement{
          Displacement* get( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, TDisplacementComponentV component_v )
          {
            Displacement* displacement = new Displacement();

            eComposite composite_id = eComposite::C_Displacement;
            int composite_idx		= bd_composite_id;

            //displacement->elems(		bd::composite::IT::elems( reservoir_id, surface_id, composite_id, composite_idx ) );
            // FIXME: bd_composite_id ?????? this was in IT1... what baout now?
            //displacement->subsidence(	component::IT::values( reservoir_id, surface_id, depletion_id, bd_composite_id, composite_id, component_v ) );
            displacement->subsidence(	component::values( composite_id, composite_idx, depletion_id, component_v ) );
            //displacement->wdat(			WDAT );		//FIXME: hard-coded

            //Printer::instance()->debug("elems: %d subs: %d", displacement->elems().size(), displacement->subsidence().size() );
            Printer::instance()->debug("subs: %d", displacement->subsidence().size() );

            return displacement;
          }
        } // displacement			
      } // IT
    } // composite
  } // res
  namespace bd{
    namespace composite{
      namespace component{
        namespace IT{
          IT_Data::T3ValuesV values( eComposite composite_id, int composite_idx, TDisplacementComponentV component_v, std::vector< const geo::IPoint* > points )
          {
            IT_Data::T3ValuesV vals;

            std::pair<IValueComposite*,CPointSet*> composite_p = bd::composite::get( composite_id, composite_idx );
            IValueComposite* composite = composite_p.first;
            CPointSet* ps = composite_p.second;

            for( int i=0; i<  points.size(); i++ )
              vals.push_back( composite::component::value( *points[ i ], composite, component_v ) );

            return vals;
          }
        } // IT
      } // component
      namespace IT{
        int elem( const geo::IPoint& point, geo::CBodyGroup::CSideSurface* surface )
        {
          int res=-1;
          std::set<int> faces_s = surface->Candidates( surface->NearestXYPoint(point) );
          for( std::set<int>::iterator it = faces_s.begin(); it != faces_s.end(); it++ )
          {
            const geo::IFace& face = surface->Face(*it);
            geo::CTriangle trans = bd::pointset::surface::translate_face_to_point( point, face );
            if (trans.Contains(point, true))
              return *it;
          }
          return res;
        }
        IT_Data::TIntV elems( int reservoir_id, int surface_id, eComposite composite_id, int composite_idx )
        {
          IT_Data::TIntV elems_;

          int num_vol = model::formation::num_volumes( reservoir_id  );
          for( int i = 0; i < num_vol; i++ )
          {
            IT_Data::TIntV elems_v = elems( reservoir_id, surface_id, i, composite_id, composite_idx );
            elems_.insert( elems_.end(), elems_v.begin(), elems_v.end() );
          }
          return elems_;
        }
        IT_Data::TIntV elems( int reservoir_id, int surface_id, int volume_id, eComposite composite_id, int composite_idx )
        {
          IT_Data::TIntV elems_;

          geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, volume_id, surface_id  );
          if( surface )
          {
            std::pair<IValueComposite*,CPointSet*> composite_p = bd::composite::get( composite_id, composite_idx );
            CPointSet* ps = composite_p.second;

            for( int i=0; i<  ps->PointSize(); i++ )
            {
              const geo::IPoint& point = ps->PointAt( i );
              if( pointset::filter( point, ps ) )
              {
                int elem_id = elem( point, surface );
                if( elem_id != -1 )
                  elems_.push_back( elem_id );
              }
            }
          }
          return elems_;
        }
        std::vector< const geo::IPoint* > points( int reservoir_id, int surface_id, eComposite composite_id, int composite_idx )
        {
          std::vector< const geo::IPoint* > points;

          int num_vol = model::formation::num_volumes( reservoir_id  );
          for( int i = 0; i < num_vol; i++ )
          {
            geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, i, surface_id  );
            if( surface )
            {
              std::pair<IValueComposite*,CPointSet*> composite_p = bd::composite::get( composite_id, composite_idx );
              IValueComposite* composite = composite_p.first;
              CPointSet* ps = composite_p.second;

              for( int j=0; j<  ps->PointSize(); j++ )
              {
                const geo::IPoint& p = ps->PointAt( j );
                if( pointset::filter( p, ps ) )
                {
                  int elem_id = elem( p, surface );
                  if( elem_id != -1 )
                    points.push_back( &p );
                }
              }
            }
          }
          return points;
        }
        namespace displacement{
          Displacement* get( int reservoir_id, int displacement_id, int surface_id, TDisplacementComponentV component_v )
          {
            Displacement* displacement = new Displacement();

            eComposite composite_id = eComposite::C_Displacement;
            int composite_idx		= displacement_id;

            displacement->elems( IT::elems(	reservoir_id, surface_id, composite_id, composite_idx ) );
            
            std::vector< const geo::IPoint* > points_v = IT::points( reservoir_id, surface_id, composite_id, composite_idx );

            displacement->subsidence(	component::IT::values( composite_id, composite_idx, component_v, points_v ) );
            displacement->z(			gm::coordinate( convert( points_v ), eComponent::Z ) );
            displacement->wdat(			WDAT );			//FIXME: hard-coded

            Printer::instance()->debug("elems: %d subs: %d z: %d", displacement->elems().size(), displacement->subsidence().size(), displacement->z().size());

            return displacement;
          }
        } // displacement
      } // IT
    } // composite
    namespace pointset{
    }
    namespace material{
    } // material
  } //bd
} //gm