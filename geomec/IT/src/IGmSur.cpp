#include "stdafx.h"

#include "IGmSur.h"

#include "IGmIT.h" // -> bd::composite::IT::displacement::elems()

//SurfaceDesc().Name() ->
//#include "Printer.h"
//#include "TetSurface.h" 
//#include "SurfaceDesc.h"

#include "FormationBase.h"
#include "TetraFormation.h"
#include "ResultTree.h"
#include "Material.h" //Poisson ratio
#include "IT_MaterialParameter.h"
#include "Pressure.h"
#include "StrainLoad.h"

namespace gm{
  namespace model{
    namespace formation{
      namespace surface{
        geo::CBodyGroup::CSideSurface* get(  int reservoir_id, int volume_idx, int surface_idx  )
        {
          /*geo::CBodyGroup::CSideSurface* surface = nullptr;
          CFormationBase* pFormation = model::formation::get( reservoir_id ); //FIXME
          if( pFormation )
          {
            CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);
            surface = &t.Volume( volume_idx ).Volume().SideSurface( surface_idx );
          }
          return surface;*/
          return get( reservoir_id, surface_idx  );
        }
        geo::CBodyGroup::CSideSurface* get( int reservoir_id, int surface_idx  )
        {
          int count = 0;
          CFormationBase* pFormation = formation::get( reservoir_id );
          CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

          int num_volumes = model::formation::num_volumes( reservoir_id  );
          for( int i = 0; i < num_volumes; i++ )
          {
            geo::CBodyGroup& bodys = t.Volume( i ).Volume();

            int num_surfaces = bodys.SideSurfaceSize();
            for( int j=0; j < num_surfaces; j++ )
            {
              if( count == surface_idx )
              {
                geo::CBodyGroup::CSideSurface& surface = bodys.SideSurface( j );
                //geo::CTetSurface& tet_surface = static_cast< geo::CTetSurface& >( surface );

                //Printer::instance()->debug("gm::model::formation::surface::get( %d ) : %s", surface_idx, tet_surface.SurfaceDesc().Name().c_str() );
                return &surface;
              }
              else
                count++;
            }
          }
          return nullptr;
        }
        IT_Data::TPointsV _points( int reservoir_id, int surface_id, int volume_idx )
        {
          IT_Data::TPointsV points_v;
          geo::CBodyGroup::CSideSurface* surface = get( reservoir_id, volume_idx, surface_id  );
          if( surface )
          {
            int nPoints = surface->PointSize();
            points_v.reserve( nPoints );

            for( int i=0; i< nPoints; i++ )
            {
              const geo::IPoint& point = surface->Point( i );
              points_v.push_back( IT_Data::TPoint{ point.X(), point.Y(), point.Z() } );
            }
          }
          return points_v;
        }
        IT_Data::TPointsV points( int reservoir_id, int surface_id )
        {
          IT_Data::TPointsV points_v;
          int num_vol = model::formation::num_volumes( reservoir_id  );
          for( int i = 0; i < num_vol; i++ )
          {
            IT_Data::TPointsV points_ = _points( reservoir_id, surface_id, i );
            points_v.insert( points_v.end(), points_.begin(), points_.end() );
          }
          return points_v;
        }
        IT_Data::TTriV _elems( int reservoir_id, int surface_id, int volume_id )
        {
          IT_Data::TTriV elems_v;
          geo::CBodyGroup::CSideSurface* surface = model::formation::surface::get( reservoir_id, volume_id, surface_id  );
          if( surface )
          {
            int nFaces = surface->FaceSize();
            for( int i=0; i< nFaces; i++ )
            {
              const geo::IFace& face = surface->Face( i );
              elems_v.push_back( IT_Data::TTri{ face.PointIndex(0), face.PointIndex(1), face.PointIndex(2) } ); //FIXME: verify !!!!
            }
          }
          return elems_v;
        }
        IT_Data::TTriV elems( int reservoir_id, int surface_id )
        {
          IT_Data::TTriV elems_v;
          int num_vol = model::formation::num_volumes( reservoir_id  );
          for( int i = 0; i < num_vol; i++ )
          {
            IT_Data::TTriV elems_ = _elems( reservoir_id, surface_id, i );
            elems_v.insert( elems_v.end(), elems_.begin(), elems_.end() );
          }
          return elems_v;
        }
        namespace properties{
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirPropDep property_ )
          {
            IT_Data::TValuesV values_v;

            CFormationBase* pFormation = model::formation::get( reservoir_id );
            CDepletionStage* pStage = model::depletion::get( depletion_id );

            if( pFormation && pStage )
            {
              CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

              int num_vol = model::formation::num_volumes( reservoir_id  );
              for( int i = 0; i < num_vol; i++ )
              {
                geo::CBodyGroup::CSideSurface& surface = t.Volume( i ).Volume().SideSurface( surface_id );
  
                int nPoints = surface.PointSize();
                for( int j=0; j< nPoints; j++ )
                {
                  const geo::IPoint& point = surface.Point( j );
                  double val = value( property_, point, pFormation, pStage );
                  values_v.push_back( val );
                }		
              }
            }
            return values_v;
          }
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirPropMat property_ )
          {
            IT_Data::TValuesV values_v;

            CFormationBase* pFormation = model::formation::get( reservoir_id );
            CDepletionStage* pStage = model::depletion::get( depletion_id );

            if( pFormation && pStage )
            {
              CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

              int num_vol = model::formation::num_volumes( reservoir_id  );
              for( int i = 0; i < num_vol; i++ )
              {
                geo::CBodyGroup::CSideSurface* surface = surface::get( reservoir_id, i, surface_id  );
                if( surface )
                {
                  int nPoints = surface->PointSize();
                  for( int j=0; j< nPoints; j++ )
                  {
                    const geo::IPoint& point = surface->Point( j );
                    double val = value( property_, point, pFormation, pStage );
                    values_v.push_back( val );
                  }		
                }
              }
            }
            return values_v;
          }
          IT_Data::TValuesV get( int reservoir_id, int surface_id, eReservoirPropForm prop )
          {
            IT_Data::TValuesV values_v;

            CFormationBase* pFormation = model::formation::get( reservoir_id );

            int num_vol = model::formation::num_volumes( reservoir_id  );
            for( int i = 0; i < num_vol; i++ )
            {
              geo::CBodyGroup::CSideSurface* surface = surface::get( reservoir_id, i, surface_id  );
              if( surface )
              {
                int nPoints = surface->PointSize();
                for( int j=0; j< nPoints; j++ )
                {
                  const geo::IPoint& point = surface->Point( j );
                  values_v.push_back( value( prop, point, pFormation ) );
                }		
              }
            }
            return values_v;
          }
          double value( eReservoirPropForm prop, const geo::IPoint& point, CFormationBase* pFormation )
          {
            double val = NaN;
            if( pFormation )
            {
              if( prop == P1_Thickness )
              {
                geo::CPoint point_top;
                geo::CPoint point_bottom;

                val = pFormation->ThicknessAt( point, point_top, point_bottom, nullptr ).Value(); //FIXME: paralalize / it takes 5s. first time, then 0.05s.
              }
            }
            return val;
          }
          double value( eReservoirPropDep prop, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage )
          {
            double val = NaN;
            if( pFormation && pStage )
            {
              if( pStage )
              {
                if( prop == P3_PorePressure )
                  val = pFormation->Pressure( *pStage ).Component().ScalarData().ValuePoint(point).Value();
                else if( prop == P3_Strain )
                  val = pFormation->Strain( *pStage ).Component().ScalarData().ValuePoint(point).Value();
              }
            }
            return val;
          }
          double value( eReservoirPropMat prop, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage )
          {
            double val = NaN;
            if( pFormation && pStage )
            {
              if( pStage )
              {
                if( prop == P2_PoissonsRatio || prop == P2_YoungsModulus || prop == P2_Cm )
                  val	= pFormation->Material( *pStage ).Material( point ).ParameterValue( bd::material::id( prop ) ); //FIXME
              }
            }
            return val;
          }
        } // properties
      } // surface
    } // formation
  } // model
  namespace res{
    namespace composite{
      namespace component{
        namespace surface{
          IT_Data::T3Values value( const geo::IFace& face, TDisplacementComponentV& component_v, CDepletionStage* pStage, eComposite composite_ )
          {
            const geo::IPoint& point = face.MidPoint();

            int nComponents = component_v.size();
            IT_Data::T3Values values={ NaN, NaN, NaN };
            for( int j = 0; j < nComponents; j++ )
            {
              eComponent _type = component_v[ j ];
              double& val = _type == eComponent::X ? values.v1 : _type == eComponent::Y ? values.v2 : values.v3;
              val = value( face, _type, pStage, composite_ );
            }
            return values;
          }
          double value( const geo::IFace& face, eComponent component_, CDepletionStage* pStage, eComposite composite_ )
          {
            const geo::IPoint& point = face.MidPoint();

            IVectorResult::VECTOR_COMPONENT type_ = component::id( component_ );
            const IVectorResult::CVectorComponent* component = dynamic_cast< const IVectorResult::CVectorComponent* >( res::composite::component::get( composite_, pStage, type_ ) );

            double val = NaN;
            if (component)
            {
              // ValuePoint would map the body values to the point, but we need explicitly the face values mapped
              IValueDomainScalar::TValueVec vcValues = component->ValueElement(face);
              geo::IElement::TDoubleVec vcShape = face.IsoShapeFunction(point);

              assert(vcShape.size() == vcValues.size());

              geo::CValue c_val = vcValues[0] * vcShape[0];
      
              for (size_t i = 1; i < vcValues.size(); ++i) 
                c_val += vcValues[i] * vcShape[i];

              val = c_val.Value() / vcValues.size();
            }
            return val;
          }
        } // surface
      } // surface
    } // surface
  } // res
  namespace bd{
    namespace composite{
      namespace surface{
      } // surface
    } // composite
    namespace pointset{
      namespace surface{
        geo::CTriangle translate_face_to_point( const geo::CPoint& point, const geo::IFace& face  )
        {
          geo::CPoint tmp[3];

          for (int i = 0; i < 3; ++i)
          tmp[i].Set(face.Point(i).X(), face.Point(i).Y(), point.Z());

          return geo::CTriangle(tmp[0], tmp[1], tmp[2]);
        }
      } // surface
    } // pointset
    namespace material{
      namespace surface{
        MaterialParameter* param( int reservoir_id, int depletion_id, eReservoirPropMat _material_id, int surface_id )
        {
          IT_Data::TValuesV values_v;

          int material_id_ = id( _material_id );

          MaterialParameter* param = new MaterialParameter();

          CFormationBase* pFormation = model::formation::get( reservoir_id );
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
                int nFaces = surface->FaceSize();
                for( int j=0; j< nFaces; j++ )
                  values_v.push_back( param_value( pFormation, pStage, material_id_, surface->Face( j ) ) );
              }
            }
          }

          param->vals( values_v );
          return param;
        }
        double param_value( CFormationBase* pFormation, CDepletionStage* pStage, int material_id_, const geo::IFace& body )
        {
          double val = NaN;

          if( pFormation && pStage )
          {
            const CFFMaterial& mat = pFormation->Material( *pStage ).Material( body );

            if( mat.IsParameter( material_id_ ) )
              val = mat.ParameterValue( material_id_ );
          }
          return val;
        }
      } // surface
    } // material
  } // bd
} // gm