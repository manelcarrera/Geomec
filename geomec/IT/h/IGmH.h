#pragma once

//Interface
#include "IT_Data.h"
#include "IGmInc.h"

//Geomec
#include "IVectorResult.h"
#include "ValueTypes.h"
#include "BodyGroup.h"

//Gen
#include <string>
#include <list>

// BASE
class Depletion;
class CModelBase;
class CFormationBase;
class CGeomecDoc;
class StringV;
// SURFACE
class MaterialParameter; 
// IT
class Reservoir;
class Displacement;
struct T_Elem_And_Val_And_Z;
typedef std::vector< T_Elem_And_Val_And_Z > T_Elem_And_Val_And_Z_V;


namespace gm{

  enum eDepletionType{		Strain, Compressibility, Pore_Pressure };
  enum eMaterialParameter{	PoissonsRatio_, YoungsModulus_, NotDefined_ };
  enum eReservoirProperty{	Thickness,	PoissonsRatio, YoungsModulus, PorePressure };
  enum eComponent{			X, Y, Z };
  enum eType{					Surface_, Volume };
  enum eComposite{			C_Displacement, C_TotalStress, C_TotalStressChange, C_TotalStrain, C_PlasticStrain, C_EffectiveStress, C_EffectiveStressChange, C_PorePressure, 
                C_Temperature, C_Consolidation, C_Porosity, C_FaultEffectiveNormalStress, C_FaultShearStress, C_FaultDisplacement, C_FaultDilatation, C_FaultPlasticSlip, 
                C_Aperture, C_ShearCapacity, C_DerivedResults, C_NotDefined };
  enum eList{					L_Surface, L_Formation, L_Reservoir, L_Depletion, L_Displacement, L_BD_Composite };

  typedef std::vector<eComponent> TDisplacementComponentV;
  typedef std::vector<eReservoirProperty> TReservoirPropertyV;

  const int LAST_DEPLETION_STAGE = -1;
  const int ALL_DEPLETION_STAGES = -1;
  const std::string DEFAULT_POINT_SET_NAME;
  const std::string DEFAULT_DEPLETION_STAGE_NAME = "Depletion Stage %d";
  const std::string DEFAULT_PRESSURE_NAME;
  const std::string DEFAULT_STRAIN_NAME;
  const std::string DEFAULT_MATERIAL_NAME;

  const bool IN_HULL_FILTER = false;

  const int NOT_DEFINED = -1; //FIXME
  
  CGeomecDoc* doc();

  namespace model{ // [[[ MODEL ]]]
    CStringV& list( eList type_, int param = -1 );
    CModelBase* get();
    void run();
    namespace formation{
      CFormationBase* get( int idx, bool bReservoir = true );
      TReservoirL& list(  bool is_depleting = true );
      int num_volumes( int reservoir_id  );
      namespace surface{
        TSurfaceL& list( int reservoir_id );
        // [ SURFACE ]
        geo::CBodyGroup::CSideSurface* get(  int reservoir_id, int volume_idx, int surface_idx  );
        IT_Data::TPointsV points( int reservoir_id, int surface_id, int volume_idx );
        IT_Data::TPointsV points( int reservoir_id, int surface_id );
        IT_Data::TTriV elems( int reservoir_id, int surface_id );
        IT_Data::TTriV elems( int reservoir_id, int surface_id, int volume_idx );
        namespace properties{
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirProperty property_ );
          IT_Data::TValuesV get( int reservoir_id, int surface_id, int depletion_id, eReservoirProperty prop );
          double value( eReservoirProperty prop, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage );
          double value( eReservoirProperty prop, const geo::IPoint& point, CFormationBase* pFormation );
        }
      }
      // [ IT ]
      namespace IT{
        namespace reservoir{
          Reservoir& get( int reservoir_id, int surface_id, TReservoirPropertyV properties_v={ Thickness, PoissonsRatio, YoungsModulus, PorePressure}, int depletion_id = 0 );
          namespace properties{
            IT_Data::T4Values values( TReservoirPropertyV& properties_v, const geo::IPoint& point, CFormationBase* pFormation, CDepletionStage* pStage = nullptr );
          }
        } // reservoir
      } // IT

    }
    namespace depletion{
      void branch( int depletion_id = LAST_DEPLETION_STAGE, bool is_branch = true );
      int size();
      TDepletionL& list();
      CDepletionStage* get( int idx = LAST_DEPLETION_STAGE );
      void add( bool is_branch = true );
      void remove( int depletion_id = LAST_DEPLETION_STAGE );
      // [ IT ]
      namespace IT{
        namespace pressure{
          void add(	int reservoir_id, 
                const IT_Data::TPointsV& points_v, 
                const IT_Data::TValuesV& pressures_v,
                eDepletionType _type = Pore_Pressure,
                int depletion_id = LAST_DEPLETION_STAGE,
                bool is_branch = true );
          void add(	int reservoir_id,							// Formation to wich add the provided pressure
                Depletion& depletion,						// Pressure to add : points + pressure
                eDepletionType _type = Pore_Pressure,		// Format of the provided pressure : Strain, Compressibility or Pore_Pressure
                int depletion_id = LAST_DEPLETION_STAGE,	// Default params adds a new depletion stage
                bool is_branch = true );					// Branch | Results
        } // pressure
      } // IT
    }
  }
  namespace res{ // [[[ RES ]]]
    bool available();
    namespace composite{
      std::pair<TCompositeV,TPointSetV> list( eComposite composite_id );
      const IResult* result( eComposite composite_, CDepletionStage *pStage );
      int id( eComposite composite_ );
      CStringV& names_list( eComposite composite_id );
      namespace component{
        const IResultComponent* get( eComposite composant_, CDepletionStage *pStage, IVectorResult::VECTOR_COMPONENT _component );
        const IVectorResult::CVectorComponent* get_ONE(		eComposite composite_,
                                  CDepletionStage *pStage, 
                                  IVectorResult::VECTOR_COMPONENT component = IVectorResult::VECTOR_COMPONENT::VC_Z );
        const IVectorResult::CFullVectorComponent* get_FULL( eComposite composite_, CDepletionStage *pStage );
        IVectorResult::VECTOR_COMPONENT id( eComponent _type );
        // [ SURFACE ]
        namespace surface{
          IT_Data::T3Values value( const geo::IFace& face, TDisplacementComponentV& component_v, CDepletionStage* pStage, eComposite composite_ );
          double value( const geo::IFace& face, eComponent component_, CDepletionStage* pStage, eComposite composite_ );
          IT_Data::TValuesV values( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, eComposite composite_, eComponent component_ );
        } // surface
      // [ IT ]
      namespace IT{ 
        namespace displacement{ 
          Displacement& get( int reservoir_id, int surface_id, int depletion_id, int bd_composite_id, TDisplacementComponentV component_v );
        } // displacement
      } // IT

      } // component 
    } // composite
  } // res
  namespace bd{ // [[[ BD ]]]
    namespace composite{
      std::pair<IValueComposite*,CPointSet*> get( eComposite composite_id, int idx );
      IT_Data::TPointsV points( eComposite composite_, int composite_idx );
      IT_Data::T3ValuesV values( eComposite composite_, int composite_idx, TDisplacementComponentV component_v = {X,Y,Z} );
      IT_Data::T3Values value( const geo::IPoint& point, IValueComposite* composite, TDisplacementComponentV& component_v );
      double value( const geo::IPoint& point, IValueComposite* composite, eComponent component_ );
      IT_Data::TValuesV values( eComposite composite_, int composite_idx, eComponent component_ );
      // [ SURFACE ]
      namespace surface{
        IT_Data::TIntV elems( int reservoir_id, int volume_idx, int surface_id, eComposite composite_id, int composite_idx );
        IT_Data::TIntV elems( int reservoir_id, int surface_id, eComposite composite_, int composite_idx );
      // [ IT ]
      namespace IT{ 
        namespace displacement{
          T_Elem_And_Val_And_Z BD_elem_and_val_and_z( const geo::IPoint& point, geo::CBodyGroup::CSideSurface& surface, IValueComposite* composite, TDisplacementComponentV& component_v );
          T_Elem_And_Val_And_Z_V BD_elems_and_vals_and_z(  int reservoir_id, int composite_idx, int surface_id, TDisplacementComponentV component_v, int volume_idx, eComposite composite_id  );
          Displacement& get( int reservoir_id, int displacement_id, int surface_id, TDisplacementComponentV component_v );
        } // displacement
      }

    } // composite
    namespace pointset{
      int add(	const IT_Data::TPointsV& points_v, 
            eDepletionType _type, 
            int depletion_id );
      int add_values( int ps_id, const IT_Data::TValuesV& pressures_v );
      bool link( int ps_id, int reservoir_id, int depletion_id );
      bool filter( const geo::CPoint& point, CPointSet* ps );
      IT_Data::TPointsV points( CPointSet* ps );
      // [ SURFACE ]
      namespace surface{
        geo::CTriangle translate_face_to_point( const geo::CPoint& point, const geo::IFace& face  );
        IT_Data::TIntV elems( CPointSet* ps, geo::CBodyGroup::CSideSurface& surface );
      } // surface

    } // pointset
    namespace material{
      int id( eMaterialParameter material );
      CMaterialEntry* createMaterial(CModelBase& modelBase, const QString& materialName, int materialModel);
      void connectMaterials2Formations(CModelBase& modelBase, const std::vector <IMaterial*> materials );
      void connectMaterial2Formation(CModelBase& modelBase, IMaterial* material, CFormationBase* formationBase );
      bool LoadMaterial( CModelBase& model );
      eMaterialParameter convert( eReservoirProperty material );
      // [ SURFACE ]
      namespace surface{
        MaterialParameter& param( int reservoir_id, int depletion_id, eMaterialParameter _material_id, int surface_id );
        double param_value( CFormationBase* pFormation, CDepletionStage* pStage, int material_id_, const geo::IFace& body );
      } // surface
    } // material
  } // bd
} // gm

