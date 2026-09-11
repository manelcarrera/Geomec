#include "stdafx.h"

#include "IGeomec.h"
#include "Printer.h"

#include "FormationBase.h"

#include "ResultTree.h"
#include "MeshResultTree.h"

#include "IValueComponent.h"
#include "IValueComposite.h"

#include "GeomecDoc.h" // model & PostMessage

#include "Printer.h"
#include "PointSet.h"
#include "NodalValueSet.h"
#include "Pressure.h"
#include "TetraFormation.h"

#include "ISceneWrapper.h"

#include "ModelTreeView.h"
#include "ModelView.h"


namespace IGeomec
{

//--------------------------------------------------------------------------------------------------------------------------------- base
namespace base
{
	CModelBase& model(){													return (CModelBase&)*(GetGeomecDoc()->Model()); }
	CGeomecDoc& doc(){														return (CGeomecDoc&)*(GetGeomecDoc()); }
	const geo::IElementSet& elements( const CFormationBase& formation ){	return formation.ElementSet(0).ElementSet(); }
	const IPointSet& points( const CFormationBase& formation ){				return formation.ElementSet(0); }
	void branch( int depletion_id, bool is_branch ){}
}; //base

//--------------------------------------------------------------------------------------------------------------------------------- print
namespace print
{
void formation( const CFormationBase& formation )
{
	const geo::IElementSet& elems = base::elements( formation ); 
	print::points(		elems );
	print::elements(	elems );
}
void list( const TStringL list )
{
	for( TStringL::const_iterator it = list.begin(); it != list.end(); ++it )
	{
		const std::string _string = (*it);
		Printer::instance()->debug("%s", _string.c_str() );
	}
}

void elements( const geo::IElementSet& elements )
{
	for( int e = 0; e < elements.ElementSize(); e++ )  //elems
	{
		const geo::IElement& elem = elements.Element( e );
		print::element( elem );
	}
}

void element( const geo::IElement& element )
{
	Printer::instance()->debug( "%d %d %d %d", 
		element.PointIndex( 0 ), element.PointIndex( 1 ), element.PointIndex( 2 ), element.PointIndex( 3 ) );
}

void points( const geo::IElementSet& elements )
{
	for( int e = 0; e < elements.ElementSize(); e++ )  //elems
	{
		const geo::IElement& element = elements.Element( e );
		for(int p = 0; p < element.NrOfPoints(); p++) //point per elem
			print::point( element.PointIndex( p ), element.Point( p ) );
	}
}

void point( const int point_index, const geo::IPoint& point )
{
	Printer::instance()->debug( "%d\t%E\t%E\t%E", 
		point_index, 
		point.X(), point.Y(), point.Z() );
}

void point_value( const TPointValueV point_value_v )
{
	for( TPointValueV::const_iterator it = point_value_v.begin(); it != point_value_v.end(); ++it )
	{
		const TPointValueP& point_value = (*it);

		const geo::IPoint& point = point_value.first;
		const geo::CValue value = point_value.second;

		Printer::instance()->debug( "%E\t%E\t%E\t\t%E", 
			point.X(), point.Y(), point.Z(),
			value.Value() );
	}
}

void elem_values( const TElemValuesV elem_values_v )
{
	//TODO: to implement
	for( TElemValuesV::const_iterator it = elem_values_v.begin(); it != elem_values_v.end(); ++it )
	{
		const TElemValuesP& elem_values = (*it);

		const geo::IElement& elem = elem_values.first;
		const TValuesV values = elem_values.second;

		Printer::instance()->debug( "%d\t%E\t%E\t%E\t%E", 
			elem.Index(),
			values[ 0 ].Value(), values[ 1 ].Value(), values[ 2 ].Value(), values[ 3 ].Value() ); //FIXME: hard-coded for tetras
	}
}
}; //print

//--------------------------------------------------------------------------------------------------------------------------------- displacement
namespace displacement
{
void get_v1()
{
	//model().ResultTree().
	//TGraphNodeSet ns = model().MeshResultTree().GraphEntryNodes();
	//TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry* >( model().GraphEntry( MD_BASE_FORMATION ) )->EntryNodes();
	//CExportResultData exportResultData( model );
	//getExportResultData(&exportResultData, false);
	//CExportResults exportResults( exportResultData, model );
	//CResultGroup& r			= model().ResultTree();
	CResultTree& r			= base::model().ResultTree();

	// a)	
	//CResultGroup& output =  dynamic_cast< CResultGroup& >( r.Child( 1 ) ); //output
	//CResultTree::CDisplacementResult& displacement = dynamic_cast< CResultTree::CDisplacementResult& >( output.Child( 0 ) ); //displacements

	// b)
	CResultTree::CDisplacementResult& displacement = (CResultTree::CDisplacementResult&)r.Displacement(); // displacement it's a composite

	//IValueComponenBase& comp = displacement.Component();

	//enum VECTOR_COMPONENT { VC_X = 0, VC_Y, VC_Z, VC_LENGTH, VC_FULLVECTOR };
	IVectorResult::CVectorComponent& component_x = dynamic_cast< IVectorResult::CVectorComponent& >( displacement.childAt( 1 ) ); //FIXME: run through the array.

	//component_x.

	const IValueComponentBase& component = displacement.Component( IVectorResult::VECTOR_COMPONENT::VC_Z );

	
	TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry* >( base::model().GraphEntry( MD_BASE_FORMATION ) )->EntryNodes();
	for( TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it )
	{
		CFormationBase& formation = dynamic_cast<CFormationBase&>(**it);
		std::string name = formation.Name().toStdString();

		IFormationElementSet& es1 = formation.ElementSet( 0 );
		geo::IElementSet& es2 = es1.ElementSet(); 

		std::string _name = "Reservoir";//DEFAULT_RESERVOIR_NAME;

		if( name.compare( _name ) == 0 )
		{
			for(int e = 0; e < es2.ElementSize(); e++)  //elems
			{
				const geo::IElement& element = es2.Element( e );
				
				std::vector<geo::CValue> _vector = component_x.ValueElement( element ); // it's giving me 4 values... it's one for each point of the element ?? yes, in a tetra model

				for( std::vector<geo::CValue>::iterator it2 = _vector.begin(); it2 != _vector.end(); ++it2 )
				{
					double value = (*it2).Value();
					int a = 1;
				}
				
				for( int p = 0; p < element.NrOfPoints(); p++ ) //point per elem
				{
					const geo::IPoint& point = element.Point( p );
					geo::CValue _value = component_x.ValuePoint( point );
					double value = _value.Value();
				}
			}
		}
	}


	//const CResultTree::CDisplacementResult& v = r.Displacement();

	//v.m_cache

	//CMaterialResultTree& m	= model().MaterialResultTree();
	//CMeshResultTree& mesh_r = model().MeshResultTree();
	//CMeshBase& mesh			= model().Mesh();

	int i=1;


}

TPointValueV get( CFormationBase& formation, IVectorResult::CVectorComponent& component )
{
	TPointValueV displacements_v;

	//IVectorResult::CVectorComponent& component = get_displacement_component();
	//CFormationBase& formation = get_formation( formation_name );

	IFormationElementSet& es1 = formation.ElementSet( 0 );
	geo::IElementSet& es2 = es1.ElementSet(); 
	
	displacements_v.reserve( es2.ElementSize() );

	for(int e = 0; e < es2.ElementSize(); e++)  //elems
	{
		const geo::IElement& element = es2.Element( e );
		
		std::vector<geo::CValue> _vector = component.ValueElement( element ); // it's giving me 4 values... it's one for each point of the element ?? yes, in a tetra model
		int idx=0;
		for( std::vector<geo::CValue>::iterator it = _vector.begin(); it != _vector.end(); ++it )
		{
			//double value = (*it).Value();
			//int a = 1;

			geo::IPoint& point = (geo::IPoint&)element.Point( idx );
			geo::CValue _value = (*it);

			displacements_v.push_back( 
				TPointValueP( 
					point, _value ) );

			idx++;
		}
				
		/*for( int p = 0; p < element.NrOfPoints(); p++ ) //point per elem
		{
			//const geo::IPoint& point = element.Point( p );
			//geo::CValue _value = component.ValuePoint( point );
			//double value = _value.Value();

			double value = component.ValuePoint( element.Point( p ) ).Value();
			int i = 0;

			geo::IPoint& point = (geo::IPoint&)element.Point( p );
			geo::CValue _value = component.ValuePoint( point );

			displacements_v.push_back( 
				TPointValueP( 
					point, _value ) );
		}*/
	}
	return displacements_v;
}

};//displacement

namespace depletion
{
TPointValueV pressures( CFormationBase& formation, const IResultComponent& component )
{
	TPointValueV pressures_v;

	//IVectorResult::CVectorComponent& component = get_displacement_component();
	//CFormationBase& formation = get_formation( formation_name );

	IFormationElementSet& es1 = formation.ElementSet( 0 );
	geo::IElementSet& es2 = es1.ElementSet(); 

	for(int e = 0; e < es2.ElementSize(); e++)  //elems
	{
		const geo::IElement& element = es2.Element( e );
		
		for( int p = 0; p < element.NrOfPoints(); p++ ) //point per elem
		{
			double value = component.ScalarData().ValuePoint( element.Point( p ) ).Value();
			int i = 0;

			geo::IPoint& point = (geo::IPoint&)element.Point( p );
			geo::CValue _value = component.ValuePoint( point );

			pressures_v.push_back( 
				TPointValueP( 
					point, _value ) );
		}
	}
	return pressures_v;
}
};//depletion

//--------------------------------------------------------------------------------------------------------------------------------- draft
namespace draft
{
	void do_something()
{
		//CModelBase& model = (CModelBase&)*Model();
	//Foo foo( model );

	//-------------------------------------------------------------- Reservoir
	//const std::string formation_id =  "Reservoir";//DEFAULT_RESERVOIR_NAME;

	//const CFormationBase& _formation = formation( formation_id );
	//IGeomec::print::formation( _formation ); // Points & Elements
	//-------------------------------------------------------------- Formations / Depletions
	/*Printer::instance()->debug("FORMATIONS LIST:");
	foo.print_list( foo.get_formations_list() );

	Printer::instance()->debug("DEPLETIONS LIST:");
	foo.print_list( foo.get_depletions_list() );*/
	//return;
	/*//-------------------------------------------------------------- Formation: Points / Elements
	geo::IElementSet& elements = foo.get_elements( formation );
	
	Printer::instance()->debug("POINTS:");
	foo.print_points( elements );

	Printer::instance()->debug("ELEMENTS:");
	foo.print_elements( elements );
	//-------------------------------------------------------------- Displacement: depending on the (1)depletion and the selected (2)component
	CDepletionStage& pStage = model().InitialDepletionStage().Next();
	IVectorResult::CVectorComponent& component = foo.get_displacement_component( &pStage, IVectorResult::VECTOR_COMPONENT::VC_Z );

	TPointValueV displacement_v = foo.get_displacement( formation, component );

	Printer::instance()->debug("DISPLACEMENT:");
	foo.print_point_value( displacement_v );*/
	//-------------------------------------------------------------- Pressures: depending on the (1)depletion and the selected (2)component
	/*CDepletionStage& pStage = model().InitialDepletionStage().Next();
	const IResultComponent& component = foo.get_depletion_component( &pStage );

	//TPointValueV pressures_v = foo.get_pressures( formation, component );
	TElemValuesV pressures_v = foo.get_pressures( formation, component );

	Printer::instance()->debug("PRESSURES:");
	foo.print_elem_values( pressures_v );*/
	//--------------------------------------------------------------

}


}; //draft


}; //IGeomec

//--------------------------------------------------------------------------------------------------------------------------------- tofix


/*void IT_Geomec_Interface::print_formation_v1( CFormationBase& formation )
{
		const int POS_POINT_ID	= 0; //FIXME
		const int POS_X			= 1;
		const int POS_Y			= 2;
		const int POS_Z			= 3;

		IFormationElementSet& es1 = formation.ElementSet( 0 );
		geo::IElementSet& es2 = es1.ElementSet(); 

		const int POINTS_COLS = 4; //FXIME
		const int POINTS_PER_ELEM = 4; //FIXME

		int nElems = es2.ElementSize();
		int nPointsPerElem = es2.Element(0).NrOfPoints();

		const int& ref = nElems * nPointsPerElem;
		//const int POINTS_ROWS = const_cast< int& >( ref );
				
		const int _NUM_POINTS = 100*1000; //FIXME
		const int _NUM_ELEMS = 10*1000; //FIXME

		long double (*points)[	POINTS_COLS ]		= wrapper::_create_array< long double (*)[ POINTS_COLS ],		_NUM_POINTS,	POINTS_COLS >();
		long double (*elems)[	POINTS_PER_ELEM ]	= wrapper::_create_array< long double (*)[ POINTS_PER_ELEM ],	_NUM_ELEMS,		POINTS_PER_ELEM >();

		int idx = 0;
				
		for(int e = 0; e < es2.ElementSize(); e++)  //elems
		{
			const geo::IElement& element = es2.Element( e );

			for(int p = 0; p < element.NrOfPoints(); p++) //point per elem
			{
				points[ idx ][ POS_POINT_ID ]	= (double)element.PointIndex( p );
				points[ idx ][ POS_X ]			= element.Point( p ).X();
				points[ idx ][ POS_Y ]			= element.Point( p ).Y();
				points[ idx ][ POS_Z ]			= element.Point( p ).Z();

				elems[ e ][ p ]					= (double)element.PointIndex( p );

				idx++;
			}
		} 
		int a=1;

		for( int i = 0; i < idx; i++ )  // all the points
		{
			Printer::instance()->debug( "%.0f\t%E\t%E\t%E", 
				points[ i ][ POS_POINT_ID ],
				points[ i ][ POS_X ], 
				points[ i ][ POS_Y ], 
				points[ i ][ POS_Z ] );
		}
		a=2;
		for(int e = 0; e < es2.ElementSize(); e++)  //elems
		{
			Printer::instance()->debug( "%.0f %.0f %.0f %.0f", 
				elems[ e ][ 0 ],
				elems[ e ][ 1 ],
				elems[ e ][ 2 ],
				elems[ e ][ 3 ] );
		}
}*/



//----------------------------------------------------------------------------- Templates ------------------------------------------------------------------------------

/*template<typename T>
void remove_duplicates(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}*/
