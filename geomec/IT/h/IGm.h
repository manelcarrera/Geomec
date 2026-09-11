#pragma once

//Interface
#include "IT_Data.h"
#include "IT_Material.h"
#include "IGmInc.h"

//Geomec
#include "IVectorResult.h"
#include "ValueTypes.h"

//Gen
#include <string>
#include <list>

class Depletion;
class CModelBase;
class CFormationBase;
class CGeomecDoc;
class StringV;

namespace gm{

	enum eDepletionType{		D_Strain, D_Compressibility, D_Pore_Pressure, D_Temperature };
	
	enum eReservoirPropForm{	P1_Thickness };
	enum eReservoirPropMat{		P2_PoissonsRatio, P2_YoungsModulus, P2_Cm };
	enum eReservoirPropDep{		P3_PorePressure, P3_Strain };
	
	enum eComponent{			X, Y, Z };
	enum eType{					Surface_, Volume };
	enum eComposite{			C_Displacement, C_TotalStress, C_TotalStressChange, C_TotalStrain, C_PlasticStrain, C_EffectiveStress, C_EffectiveStressChange, C_PorePressure, 
								C_Temperature, C_Consolidation, C_Porosity, C_FaultEffectiveNormalStress, C_FaultShearStress, C_FaultDisplacement, C_FaultDilatation, C_FaultPlasticSlip, 
								C_Aperture, C_ShearCapacity, C_DerivedResults, C_NotDefined };
	enum eList{					L_Surface, L_Formation, L_Reservoir, L_Depletion, L_Displacement, L_BD_Composite };

	typedef std::vector<eComponent> TDisplacementComponentV;

	const int LAST_DEPLETION_STAGE = -1;
	const int ALL_DEPLETION_STAGES = -2;
	const int NEW_DEPLETION_STAGE =	 -3;
	const std::string DEFAULT_POINT_SET_NAME;
	const std::string DEFAULT_DEPLETION_STAGE_NAME = "Depletion stage IT %d";
	const std::string DEFAULT_PRESSURE_NAME;
	const std::string DEFAULT_STRAIN_NAME;
	const std::string DEFAULT_MATERIAL_NAME = "Material IT %d";

	const bool IN_HULL_FILTER = false;

	const int NOT_DEFINED = -1; //FIXME

	/**/
	CGeomecDoc* doc();
	/**/
	CStringV* list( eList type_, int param = -1 );
	/**/
	enum eDraw{ Enable, Disable };
	void draw( eDraw draw_ );
	/* format transformation */
	IT_Data::TPoint convert( const geo::IPoint& p );
	/* format transformation */
	IT_Data::TPointsV convert( std::vector< const geo::IPoint* > points_v_in );
	/* X, Y or Z list from a (X,Y,Z) list */
	IT_Data::TValuesV coordinate( IT_Data::TPointsV points_v, eComponent coordinate_ );
	/**/
	std::string format( IT_Data::TIntV elems_v );
	namespace model{
		/* The current model */
		CModelBase* get();
		/* Sends a post-message to Geomec to run a non-linear analysis */
		void run();
		/**/
		void run_step();
		/**/
		void wait_end_run();

		namespace formation{
			/**/
			CFormationBase* get( int idx, bool bReservoir = true );
			/**/
			TReservoirL* list(  bool is_depleting = true );
			/**/
			int num_volumes( int reservoir_id  );
			namespace surface{
				/**/
				TSurfaceL* list( int reservoir_id );
				/**/
				geo::CBodyGroup::CSideSurface* get( int surface_idx  ); //FIXME
			}
		}
		namespace depletion{
			/* Sets a given depletion stage output type to 'Branch' or 'Results' */
			void output( int depletion_id = LAST_DEPLETION_STAGE, bool is_branch = true );
			/* Gives the current model depletion stages number */
			int size();
			/* 
				All depletions names list. 
		
				Depletions names are provided in a single string delimited by tokens.
			*/
			TDepletionL* list();
			/* Depletion stage */
			CDepletionStage* get( int idx = LAST_DEPLETION_STAGE );
			/* 
			Adds a new depletion stage and sets its 'output type' to Branch or Results 

			Branch is the default 'output type'
			*/
			void add( bool is_branch = true );
	
			/* 
				Removes a given depletion stage.

				Params:
				-------
					None:					Then LAST_DEPLETION_STAGE
					Id:						Then the given depletion stage
					ALL_DEPLETION_STAGES:	Then ALL
			*/
			void remove( int depletion_id = LAST_DEPLETION_STAGE );
		}
	}
	namespace res{
		/* Has the model results ? */
		bool available();

		namespace composite{
			/**/
			std::pair<TCompositeV,TPointSetV> list( eComposite composite_id );
			/**/
			const IResult* result( eComposite composite_, CDepletionStage *pStage );
			/**/
			int id( eComposite composite_ );
			/**/
			CStringV* names_list( eComposite composite_id );

			namespace component{
				/**/
				IT_Data::TValuesV values( eComposite composite_id, eComponent component_id, int composite_idx, int depletion_id );
				/**/
				IT_Data::T3ValuesV values( eComposite composite_id, int composite_idx, int depletion_id, TDisplacementComponentV component_v );
				/**/
				IT_Data::T3Values value( const geo::CPoint& point, TDisplacementComponentV& component_v, CDepletionStage* pStage, eComposite composite_id );
				/**/
				double value( const geo::CPoint& point, eComponent component_id, CDepletionStage* pStage, eComposite composite_id );
				/**/
				const IResultComponent* get( eComposite composite_id, CDepletionStage *pStage, IVectorResult::VECTOR_COMPONENT _component );
				/* Returns just one component values */
				const IVectorResult::CVectorComponent* get_ONE(		eComposite composite_,
																	CDepletionStage *pStage, 
																	IVectorResult::VECTOR_COMPONENT component = IVectorResult::VECTOR_COMPONENT::VC_Z );
				/**/
				const IVectorResult::CFullVectorComponent* get_FULL( eComposite composite_, CDepletionStage *pStage );
				/* Conersion to Geomec IDs */
				IVectorResult::VECTOR_COMPONENT id( eComponent _type );
			} // component 
		} // composite
	} // res
	namespace bd{
		namespace composite{
			/**/
			std::pair<IValueComposite*,CPointSet*> get( eComposite composite_id, int idx );
			/**/
			IT_Data::TPointsV points( eComposite composite_, int composite_idx );
			namespace component{
				/**/
				IT_Data::T3ValuesV values( eComposite composite_, int composite_idx, TDisplacementComponentV component_v = {X,Y,Z} );
				/**/
				IT_Data::T3Values value( const geo::IPoint& point, IValueComposite* composite, TDisplacementComponentV& component_v );
				/**/
				double value( const geo::IPoint& point, IValueComposite* composite, eComponent component_ );
				/**/
				IT_Data::TValuesV values( eComposite composite_, int composite_idx, eComponent component_ );
			} // component
		} // composite
		namespace pointset{
			/**/
			bool create( int id, const IT_Data::TPointsV& points_v );
			/**/
			int add_values( int ps_id, eDepletionType _type, const IT_Data::TValuesV& pressures_v );
			/**/
			int replace_values( int ps_id, const IT_Data::TValuesV& pressures_v, int pos );
			/*just values length so far; later values themselfs*/
			IT_Data::TValuesV values( int ps_id, int values_id );
			/* just values count so far; later list of pairs (value_id, composite_id)*/
			IT_Data::TIntV values_list( int ps_id );
			/*remove pointset's values in a given idx*/
			bool remove_vals( int id, int values_idx );
			/**/
			bool remove( int id );
			/**/
			bool link( int ps_id, int idx, int reservoir_id, int depletion_id, bool link_ = true );
			/**/
			bool filter( const geo::CPoint& point, CPointSet* ps );
			/**/
			IT_Data::TPointsV points( CPointSet* ps );
		} // pointset
		namespace material{
			/* Conersion to Geomec IDs */
			int id( eReservoirPropMat material );
			/**/
			CStringV* list();
			/**/
			void create( int type_ );
			/**/
			bool remove( int idx );
			/**/
			IT_Data::TValuesV values( int idx );
			/**/
			void values( int idx, IT::Material* material );
			/**/
			bool link( int idx, int res_id );
			/**/
			bool unlink( int idx, int res_id );

			//eMaterialParameter convert( eReservoirProperty material );
		} // material
	} // bd
} // gm
