#include "stdafx.h"

//Interface Geomec
#include "IGm.h"
#include "Printer.h"
#include "IT_Printer.h"
#include "List.h"
#include "Depletion.h"
#include "IT_Material.h"
#include "PointSetManager.h" 

//Geomec (itself)
#include "GeomecDoc.h" // model & PostMessage

#include "FormationBase.h"
#include "TetraFormation.h"
#include "TetSurface.h"
#include "SurfaceDesc.h"
#include "IValueComponent.h"
#include "IValueComposite.h"
#include "NodalValueSet.h"
#include "ISceneWrapper.h"
#include "treectrlbase.h"
#include "Material.h" //Poisson ratio
#include "StrainLoad.h"
#include "MaterialHelperFactory.h"
#include "ResultTree.h" // RESULTS

#include "Materialmanager.h"

namespace gm{
	CGeomecDoc* doc(){ return (CGeomecDoc*)(GetGeomecDoc()); }
	void draw( eDraw draw_ )
	{
		switch( draw_ )
		{
			case Enable:
				if( doc()->CurrentScene() )
					doc()->CurrentScene()->SetDrawing();
				CTreeCtrlBase::AllowPaint(true);
				break;
			case Disable:
				CTreeCtrlBase::AllowPaint(false);
				if (doc()->CurrentScene())
					doc()->CurrentScene()->SetNonDrawing();
				break;
		}
	}
	IT_Data::TPoint convert( const geo::IPoint& p ){ return IT_Data::TPoint{ p.X(),p.Y(),p.Z() }; }
	IT_Data::TPointsV convert( std::vector< const geo::IPoint* > points_v_in )
	{
		IT_Data::TPointsV points_v;
		for( int i=0; i< points_v_in.size(); i++ )
			points_v.push_back( convert( *points_v_in[ i ] ) );
		return points_v;
	}
	IT_Data::TValuesV coordinate( IT_Data::TPointsV points_v, eComponent coordinate_ )
	{
		IT_Data::TValuesV vals;
		for( int i=0; i< points_v.size(); i++ )
			vals.push_back( coordinate_ == eComponent::X ? points_v[i].x : eComponent::Y ? points_v[i].y : points_v[i].z );
		return vals;
	}
	std::string format( IT_Data::TIntV elems_v )
	{
		std::string s = "[ ";
		for( int i=0; i< elems_v.size(); i++ )
			s += ( std::to_string( elems_v[ i ] ) + " " );
		s += "]";
		return s;
	}
	CStringV* list( eList type_, int param )
	{
		switch( type_ )
		{
			case L_Surface:			return model::formation::surface::list( param );
			case L_Formation:		return model::formation::list( false );
			case L_Reservoir:		return model::formation::list( true );
			case L_Depletion:		return model::depletion::list();
			case L_BD_Composite:	return res::composite::names_list( (eComposite)param );
			default:				return new CStringV();
		}
	}
	namespace model{
		CModelBase* get(){ return (CModelBase*)(GetGeomecDoc()->Model()); }
		void run(){			AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_ON_RUN_MODEL, 0 ); }
		void run_step(){	AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_ON_RUN_STEP, 0 ); }
		void wait_end_run()
		{
			while( gm::doc()->m_bRunning )
				Sleep(1*1000);
		}
		namespace formation{
			int num_volumes( int reservoir_id  )
			{
				int num = 0;
				CFormationBase* pFormation = get( reservoir_id ); //FIXME
				if( pFormation )
				{
					CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);
					num = t.VolumeSize();
				}
				return num > 1 ? 1 : num; //FIXME: HC
			}
			CFormationBase* get( int idx, bool bReservoir )
			{
				if( model::get() )
				{
					int i=0;
					TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry* >( model::get()->GraphEntry( MD_BASE_FORMATION ) )->EntryNodes();
					for( TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it )
					{
						CFormationBase* pFormation = dynamic_cast<CFormationBase*>(*it);
		
						if( !pFormation || ( bReservoir && !pFormation->Depleting() ) )
							continue;

						if( i == idx )
							return pFormation;
						else
							i++;
					}
				}
				return nullptr;
			}
			TReservoirL* list(  bool is_depleting )
			{
				TReservoirL* formations_v = new TReservoirL();

				if( model::get() )
				{
					TStringL formations_list;

					TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry*>(model::get()->GraphEntry( MD_BASE_FORMATION ) )->EntryNodes();
					for( TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it )
					{
						CFormationBase& formation = dynamic_cast<CFormationBase&>(**it);
						if( is_depleting && formation.Depleting() )
							formations_list.push_back( formation.Name().toStdString() );
					}

					formations_v->elems( { std::begin( formations_list ), std::end( formations_list ) } );
				}

				return formations_v;
			}
			namespace surface{ // TODO: ????????
				TSurfaceL* list( int reservoir_id )
				{
					TSurfaceL* surfaces_v = new TSurfaceL();

					TStringL surfaces_list;

					CFormationBase* pFormation = formation::get( reservoir_id );
					CTetraFormation& t = static_cast<CTetraFormation&>(*pFormation);

					int num_vol = model::formation::num_volumes( reservoir_id  );
					for( int i = 0; i < num_vol; i++ )
					{
						geo::CBodyGroup& bodys = t.Volume( i ).Volume();

						int num_surfaces = bodys.SideSurfaceSize();
						for( int j=0; j < num_surfaces; j++ )
						{
							geo::CTetSurface& tet_surface = static_cast<geo::CTetSurface&>( bodys.SideSurface( j ) );
							surfaces_list.push_back( tet_surface.SurfaceDesc().Name() ); //FIXME
						}
					}
					surfaces_v->elems( { std::begin( surfaces_list ), std::end( surfaces_list ) } );

					return surfaces_v;
				}
			} // surface
		} // formation
		namespace depletion{
			void output( int depletion_id, bool is_branch )
			{
				CDepletionStage* pStage = get( depletion_id );
				if( pStage )
					get( depletion_id )->setOutputType( is_branch ? CDepletionStage::BRANCH : CDepletionStage::RESULTS );
			}
			int size(){ return model::get() ? model::get()->DepletionStageEntry().EntryNodes().size() : 0; }
			TDepletionL* list()
			{
				TDepletionL* depletions_v = new TDepletionL();

				if( model::get() )
				{
					TStringL depletions_list;

					CDepletionStage *stage = &model::get()->InitialDepletionStage();

					while (stage)
					{
						depletions_list.push_back(stage->Name().toStdString());
						stage = stage->Last() ? 0 : &stage->Next();
					}

					depletions_v->elems( { std::begin( depletions_list ), std::end( depletions_list ) } );
				}
				return depletions_v;
			}
			CDepletionStage* get( int idx )
			{
				CDepletionStage* pStage = nullptr;
				if( model::get() )
				{

					if( idx == LAST_DEPLETION_STAGE )
						pStage = &model::get()->DepletionStageEntry().LastStage();
					else
					{
						pStage = &model::get()->InitialDepletionStage();

						int i=0;
						while( i < idx )
						{
							if (!pStage->Last())
							pStage = &pStage->Next(); // perhaps give a warning, but it doesn't really matter
								i++;
						}
					}
				}
				assert( pStage );
				return pStage;
			}
			void add( bool is_branch )
			{
				int id = size();
				QString name = QString::asprintf( QString::fromStdString( DEFAULT_DEPLETION_STAGE_NAME ).toLatin1().data(), id );
				CDepletionStage* pStage = get( LAST_DEPLETION_STAGE );
				if( pStage )
				{
					gm::draw( Disable ); 
					new CDepletionStage( name, *pStage );
					output( LAST_DEPLETION_STAGE, is_branch );
					gm::draw( Enable ); 
				}
			}
			void remove( int depletion_id )
			{
				if( depletion_id == ALL_DEPLETION_STAGES )
				{
					while( size() )
					{
						CDepletionStage* pStage = get( LAST_DEPLETION_STAGE );
						if( pStage->OutputType() == CDepletionStage::BRANCH )
							delete pStage;
						else
							break; // Supposition: (1)RESULTS (2)RESULTS (3)BRANCH BRANCH BRANCH BRANCH ... (n)BRANCH
					}
				}
				else
				{
					CDepletionStage* pStage = get( depletion_id );
					delete pStage;
				}
			}
		} // depletion
	} // model
	namespace res{
		bool available(){ return model::get() ? model::get()->ResultRegister().ResultsAvailable() : false; }
		namespace composite{
			std::pair<IValueComposite*,CPointSet*> get( eComposite composite_, int idx )
			{
				std::pair<TCompositeV,TPointSetV> list_ = list( composite_ );

				TCompositeV& composite_v = list_.first;
				TPointSetV& point_set_v = list_.second;

				return idx < composite_v.size() ? std::make_pair(composite_v[ idx ], point_set_v[ idx ]) : std::make_pair(nullptr,nullptr);
			}
			std::pair<TCompositeV,TPointSetV> list( eComposite composite_ )
			{
				TCompositeV composite_v;
				TPointSetV point_set_v;

				if( model::get() )
				{
					TPointSetEntry::TNodeSet point_set_set = static_cast< TPointSetEntry* >( model::get()->GraphEntry( MD_BASE_POINTSET ) )->EntryNodes();

					for( TPointSetEntry::TNodeSet::iterator it = point_set_set.begin(); it != point_set_set.end(); ++it )
					{
						CPointSet* point_set = dynamic_cast<CPointSet*>(*it);

						if( point_set )
						{
							//std::string point_set_name = point_set->Name().toStdString();
							int nComponents = point_set->NodalValueSetSize();
							CGraphNode* parent_already_added = nullptr;
							for( int i = 0; i < nComponents ; i++ )
							{
								CValueComponent* value_component = point_set->NodalValueSet( i ).Component();
								if( value_component ) 
								{
									CGraphNode* parent = value_component->parent();
									if( (int)value_component->parent()->TypeId() == id( composite_ ) )
									{
										if( parent != parent_already_added ) //if found for one component don't need to see the res, otherwise results are duplicated
										{
											composite_v.push_back( static_cast<IValueComposite*>( parent ) );
											point_set_v.push_back( point_set );
											parent_already_added = parent;
										}
									}
								}
							}
						}
					}
				}
				return std::make_pair( composite_v, point_set_v );
			}
			const IResult* result( eComposite composite_, CDepletionStage *pStage )
			{
				const IResult* result = nullptr;
				if( model::get() )
				{
					CResultTree& tree =  model::get()->ResultTree();
					switch( composite_ )
					{
						case C_Displacement:			result = &tree.Displacement();	break;

						//CComponentComposite -> IResult
						case C_TotalStress:				result = &tree.TotalStress().Components();				break; 
						case C_TotalStressChange:		result = &tree.TotalStressChange().Components();		break; 
						case C_TotalStrain:				result = &tree.TotalStrain().Components();				break; 
						case C_PlasticStrain:			result = &tree.PlasticStrain().Components();			break; 
						case C_EffectiveStress:			result = &tree.EffectiveStress().Components();;			break;
						case C_EffectiveStressChange:	result = &tree.EffectiveStressChange().Components();;	break; 

						case C_PorePressure:			result = &tree.PorePressure();	break; // 0: Absolute, 1: Change, 2: Gradient ????? -> giving the value '1' app crashes
						case C_Temperature:				result = &tree.Temperature();	break; 
						case C_Consolidation:			result = &tree.Consolidation();	break; 
						case C_Porosity:				result = &tree.Porosity();		break; 

						// Faults
						case C_FaultEffectiveNormalStress:	result = &tree.FaultEffectiveNormalStress();	break; 
						case C_FaultShearStress:		result = &tree.FaultShearStress();	break; 
						case C_FaultDisplacement:		result = &tree.FaultDisplacement();	break; 
						case C_FaultDilatation:			result = &tree.FaultDilatation();	break; 
						case C_FaultPlasticSlip:		result = &tree.FaultPlasticSlip();	break; 

						//case C_Aperture:				result = &tree.Aperture();			break;  // needs some params 
						case C_ShearCapacity:			result = &tree.ShearCapacity();		break; 
						//case C_DerivedResults:			result = &tree.DerivedResults();	break; 
						case C_NotDefined:				break;
						default:						break;
					}
				}
				return result;
			}
			int id( eComposite composite_ )
			{
				switch( composite_ )
				{
					case C_Displacement: return IDT_VALUETYPE_DISPLACEMENT;
					//case C_PorePressure: break; // ????
					default: break;
				}
				return C_NotDefined;
			}
			CStringV* names_list( eComposite composite_ )
			{
				CStringV* names_v = new CStringV();

				TStringL composites_list;

				TCompositeV composite_v = list( composite_ ).first;
	
				for( int i=0; i < composite_v.size(); i++ )
					composites_list.push_back( composite_v[ i ]->Name().toStdString() );
	
				names_v->elems( { std::begin( composites_list ), std::end( composites_list ) } );
				return names_v;
			}
			namespace component{
				const IResultComponent* get( eComposite composite_id, CDepletionStage *pStage, IVectorResult::VECTOR_COMPONENT _component )
				{
					const IResultComponent* pResultComponent = nullptr;
					const IResult* result = composite::result( composite_id, pStage );
					if( result )
					{
						CAnalysisType::TAnalysisType anType = model::get()->ResultRegister().ResultsAvailable( CAnalysisType::AT_NONLIN ) ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR;
						pResultComponent = result->ResultComponent( *pStage, anType, 0, _component );
					}

					return pResultComponent;
				}
				const IVectorResult::CVectorComponent* get_ONE(	eComposite composite_, CDepletionStage *pStage, IVectorResult::VECTOR_COMPONENT _component )
				{
					const IVectorResult::CVectorComponent* component = nullptr;
					if( _component != IVectorResult::VECTOR_COMPONENT::VC_FULLVECTOR )
						component = dynamic_cast< const IVectorResult::CVectorComponent* >( get( composite_, pStage, _component) );
					return component;
				}
				const IVectorResult::CFullVectorComponent* get_FULL( eComposite composite_, CDepletionStage *pStage )
				{
					const IVectorResult::CFullVectorComponent* component = dynamic_cast< const IVectorResult::CFullVectorComponent* >(  get( composite_, pStage, IVectorResult::VECTOR_COMPONENT::VC_FULLVECTOR) );
					return component;
				}
				IVectorResult::VECTOR_COMPONENT id( eComponent _type )
				{
					IVectorResult::VECTOR_COMPONENT type_ = _type == eComponent::X ?	IVectorResult::VECTOR_COMPONENT::VC_X :
															_type == eComponent::Y ?	IVectorResult::VECTOR_COMPONENT::VC_Y :
																						IVectorResult::VECTOR_COMPONENT::VC_Z;
					return type_;
				}
				IT_Data::T3ValuesV values( eComposite composite_id, int composite_idx, int depletion_id, TDisplacementComponentV component_v )
				{
					IT_Data::T3ValuesV values_v;
					if( available() )
					{
						CDepletionStage* pStage = model::depletion::get( depletion_id );
						if( pStage )
						{
							std::pair<IValueComposite*,CPointSet*> composite_p = bd::composite::get( composite_id, composite_idx );
							IValueComposite* composite = composite_p.first;
							CPointSet* ps = composite_p.second;

							for( int i=0; i<  ps->PointSize(); i++ )
							{
								const geo::IPoint& point = ps->PointAt( i );
								if( bd::pointset::filter( point, ps ) )
									values_v.push_back( ( value( point, component_v, pStage, composite_id ) ) );
							}
						}
					}
					return values_v;
				}
				IT_Data::TValuesV values( eComposite composite_id, eComponent component_id, int composite_idx, int depletion_id )
				{
					IT_Data::TValuesV values_v;
					if( available() )
					{
						CDepletionStage* pStage = model::depletion::get( depletion_id );
						if( pStage )
						{
							std::pair<IValueComposite*,CPointSet*> composite_p = bd::composite::get( composite_id, composite_idx );
							IValueComposite* composite = composite_p.first;
							CPointSet* ps = composite_p.second;

							for( int i=0; i<  ps->PointSize(); i++ )
							{
								const geo::IPoint& point = ps->PointAt( i );
								if( bd::pointset::filter( point, ps ) )
									values_v.push_back( value( point, component_id, pStage, composite_id ) );
							}
						}
					}
					return values_v;
				}
				IT_Data::T3Values value( const geo::CPoint& point, TDisplacementComponentV& component_v, CDepletionStage* pStage, eComposite composite_id )
				{
					int nComponents = component_v.size();
					IT_Data::T3Values values={ NaN, NaN, NaN };
					for( int j = 0; j < nComponents; j++ )
					{
						eComponent _type = component_v[ j ];
						double& val = _type == eComponent::X ? values.v1 : _type == eComponent::Y ? values.v2 : values.v3;
						val = value( point, _type, pStage, composite_id );
					}
					return values;
				}
				double value( const geo::CPoint& point, eComponent component_id, CDepletionStage* pStage, eComposite composite_id )
				{
					IVectorResult::VECTOR_COMPONENT type_ = component::id( component_id );
					const IVectorResult::CVectorComponent* component = dynamic_cast< const IVectorResult::CVectorComponent* >( res::composite::component::get( composite_id, pStage, type_ ) );

					double val = NaN;
					if( component )
						val = component->ValuePoint( point ).Value();
					return val;
				}
			} // component
		} // composite
	} // res
	namespace bd{
		namespace composite{
			std::pair<TCompositeV,TPointSetV> list( eComposite composite_ )
			{
				TCompositeV composite_v;
				TPointSetV point_set_v;

				if( model::get() )
				{
					TPointSetEntry::TNodeSet point_set_set = static_cast< TPointSetEntry* >( model::get()->GraphEntry( MD_BASE_POINTSET ) )->EntryNodes();

					for( TPointSetEntry::TNodeSet::iterator it = point_set_set.begin(); it != point_set_set.end(); ++it )
					{
						CPointSet* point_set = dynamic_cast<CPointSet*>(*it);

						if( point_set )
						{
							//std::string point_set_name = point_set->Name().toStdString();
							int nComponents = point_set->NodalValueSetSize();
							CGraphNode* parent_already_added = nullptr;
							for( int i = 0; i < nComponents ; i++ )
							{
								CValueComponent* value_component = point_set->NodalValueSet( i ).Component();
								if( value_component ) 
								{
									CGraphNode* parent = value_component->parent();
									if( (int)value_component->parent()->TypeId() == res::composite::id( composite_ ) )
									{
										if( parent != parent_already_added ) //if found for one component don't need to see the res, otherwise results are duplicated
										{
											composite_v.push_back( static_cast<IValueComposite*>( parent ) );
											point_set_v.push_back( point_set );
											parent_already_added = parent;
										}
									}
								}
							}
						}
					}
				}
				return std::make_pair( composite_v, point_set_v );
			}
			std::pair<IValueComposite*,CPointSet*> get( eComposite composite_id, int idx )
			{
				std::pair<TCompositeV,TPointSetV> list_ = list( composite_id );

				TCompositeV& composite_v = list_.first;
				TPointSetV& point_set_v = list_.second;

				return idx < composite_v.size() ? std::make_pair(composite_v[ idx ], point_set_v[ idx ]) : std::make_pair(nullptr,nullptr);
			}
			IT_Data::TPointsV points( eComposite composite_, int composite_idx )
			{
				std::pair<IValueComposite*,CPointSet*> composite_p = get( composite_, composite_idx );
				IValueComposite* composite = composite_p.first;
				CPointSet* ps = composite_p.second;
				return bd::pointset::points( ps );
			}
			namespace component{
				IT_Data::T3ValuesV values( eComposite composite_, int composite_idx, TDisplacementComponentV component_v )
				{
					IT_Data::T3ValuesV values_v;

					std::pair<IValueComposite*,CPointSet*> composite_p = get( composite_, composite_idx );
					IValueComposite* composite = composite_p.first;
					CPointSet* ps = composite_p.second;

					for( int i=0; i<  ps->PointSize(); i++ )
					{
						const geo::IPoint& point = ps->PointAt( i );
						if( bd::pointset::filter( point, ps ) )
							values_v.push_back( ( value( point, composite, component_v ) ) );
					}
					return values_v;
				}
				IT_Data::T3Values value( const geo::IPoint& point, IValueComposite* composite, TDisplacementComponentV& component_v )
				{
					IT_Data::T3Values values = { NaN, NaN, NaN };
					for( TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it )
					{
						eComponent _type = (*it);
						geo::CValue c_value = composite->Component( _type ).ScalarData().ValuePoint( point ); //values per each point in the element

						double& val = _type == eComponent::X ? values.v1 : _type == eComponent::Y ? values.v2 : values.v3;
						val = c_value.Value();
					}
					return values;
				}
				double value( const geo::IPoint& point, IValueComposite* composite, eComponent component_ ){ return composite->Component( component_ ).ScalarData().ValuePoint( point ).Value(); }
				IT_Data::TValuesV values( eComposite composite_, int composite_idx, eComponent component_ )
				{
					IT_Data::TValuesV values_v;

					std::pair<IValueComposite*,CPointSet*> composite_p = get( composite_, composite_idx );
					IValueComposite* composite = composite_p.first;
					CPointSet* ps = composite_p.second;

					for( int i=0; i<  ps->PointSize(); i++ )
					{
						const geo::IPoint& point = ps->PointAt( i );
						if( bd::pointset::filter( point, ps ) )
							values_v.push_back( ( value( point, composite, component_ ) ) );
					}
					return values_v;
				}
			} // component
		} // composite
		namespace pointset{
			bool create( int id, const IT_Data::TPointsV& points_v )
			{
				return PointSetManager::instance()->create( id, points_v );
			}
			int add_values( int ps_id, eDepletionType _type, const IT_Data::TValuesV& pressures_v )
			{
				PointSetManager* ps_mgr = PointSetManager::instance();
				if( pressures_v.size() )
				{
          // Disabling draw needed for when pointset component visualized
					gm::draw( Disable ); 
					ps_mgr->add_values( ps_id, _type, pressures_v );
					gm::draw( Enable ); 
				}
				return ps_mgr->points_size( ps_id );
			}
			int replace_values( int ps_id, const IT_Data::TValuesV& pressures_v, int pos )
			{
				PointSetManager* ps_mgr = PointSetManager::instance();
				if( pressures_v.size() )
					ps_mgr->replace_values( ps_id, pressures_v, pos );
				return ps_mgr->points_size( ps_id );
			}
			bool remove( int id )
			{
				return PointSetManager::instance()->remove( id );
			}
			IT_Data::TValuesV values( int ps_id, int values_idx )
			{
				PointSetManager* ps_mgr = PointSetManager::instance();
				IT_Data::TValuesV res = ps_mgr->composite_values( ps_id, values_idx );
				return res;
			}
			IT_Data::TIntV values_list( int ps_id )
			{
				PointSetManager* ps_mgr = PointSetManager::instance();
				IT_Data::TIntV res = ps_mgr->composite_list( ps_id );
				return res;
			}
			bool remove_vals( int id, int values_idx )
			{
				PointSetManager* ps_mgr = PointSetManager::instance();
				return ps_mgr->remove_vals( id, values_idx );
			}
			bool link( int ps_id, int idx, int reservoir_id, int depletion_id, bool link_ )
			{
				bool res = false;

				CDepletionStage* pStage = model::depletion::get( depletion_id ); //FIXME
				CFormationBase* pFormation = model::formation::get( reservoir_id );
				if( pStage &&  pFormation )
				{
					gm::draw( Disable ); 
					PointSetManager* ps_mgr = PointSetManager::instance();
					res = link_ ?	ps_mgr->link(	ps_id, idx, pFormation, pStage ) : 
									ps_mgr->unlink( ps_id, idx, pFormation, pStage );
					gm::draw( Enable ); 
				}
				return res;
			}
			bool filter( const geo::CPoint& point, CPointSet* ps ){ return ( !IN_HULL_FILTER || ps->PointInConvexHull( point ) ); }
			IT_Data::TPointsV points( CPointSet* ps )
			{
				IT_Data::TPointsV points;

				for( int i=0; i<  ps->PointSize(); i++ )
				{
					const geo::IPoint& point = ps->PointAt( i );
					if( bd::pointset::filter( point, ps ) )
						points.push_back( IT_Data::TPoint{ point.X(), point.Y(), point.Z() } );
				}
				return points;
			}
		} // pointset
		namespace material{
			int id( eReservoirPropMat material )
			{
				switch( material )
				{
					case P2_PoissonsRatio:		return IDT_VALUETYPE_POISSONS_RATIO;
					case P2_YoungsModulus:		return IDT_VALUETYPE_YOUNGS_MODULUS;
					case P2_Cm:					return IDT_VALUETYPE_ELASCOMPRES;

					default:					break;
				}
				return NOT_DEFINED;
			}	
			CStringV* list()
			{
				CStringV* materials_v = new CStringV();
				MaterialManager mgr( *gm::model::get() );
				std::vector< std::string > list_ = mgr.list();
				materials_v->elems( { std::begin( list_ ), std::end( list_ ) } );
				return materials_v;
			}
			void create( int type_ )
			{
				MaterialManager mgr( *gm::model::get() );
				QString name = QString::asprintf( gm::DEFAULT_MATERIAL_NAME.c_str(), mgr.size() );
				mgr.create( name, type_ );
			}
			bool remove( int idx )
			{
				MaterialManager mgr( *gm::model::get() );
				return mgr.remove( idx );
			}
			IT_Data::TValuesV values( int idx )
			{
				MaterialManager mgr( *gm::model::get() );
				return mgr.values( idx );
			}
			void values( int idx, IT::Material* material )
			{
				MaterialManager mgr( *gm::model::get() );

				IT_Data::TIntV ids_v = material->ids();
				IT_Data::TValuesV vals_v = material->values();

				for( int i=0; i< ids_v.size(); i++ )
					mgr.value( idx, ids_v[ i ], vals_v[ i ] );
			}
			bool link( int idx, int res_id )
			{
				bool res = false;				
				MaterialManager mgr( *gm::model::get() );
				IMaterial* mat = mgr.material( idx );
				if( mat )
					res=mgr.connect_material_to_formation( mat, gm::model::formation::get( res_id ) );
				return res;
			}
			bool unlink( int idx, int res_id )
			{
				bool res = false;				
				MaterialManager mgr( *gm::model::get() );
				IMaterial* mat = mgr.material( idx );
				if( mat )
					res=mgr.unlink_mat_from_form_dep( mat, gm::model::formation::get( res_id ), *gm::model::depletion::get( 0 ) );
				return res;
			}
		} // material
	} // bd
} // gm
