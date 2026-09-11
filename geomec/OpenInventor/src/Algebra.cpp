#include "stdafx.h"

#include "Algebra.h"
#include "IT_Printer.h"

#include "OIDIDataSetI.h"
#include "OIDIGeometry.h"


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------- ALGEBRA ----------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------- ALGEBRA::DATA -------------------------------------------------------------------------------------

void Algebra::Data::_print(){ IT_Printer::instance()->debug("a:%f b:%f d1:%f d:%f intersection: %f\t%f\t%f", alfa, beta, d1, distance, intersection[0],intersection[1],intersection[2]); }


//------------------------------------------------- ALGEBRA ----------------------------------------------------------------------------------------------

Algebra::Data Algebra::m_data = Algebra::Data();
const double Algebra::EPSILON = 0.0001;

MbVec3d Algebra::intersection_method_A( MbVec3d p0, MbVec3d p2, MbVec3d p3 )
{
	double alfa_ = alfa( vector(p0,p3), vector(p2,p3) );
	double beta_ = beta( alfa_ );
	double d1 = len( vector(p0,p3) );
	double distance = distance_to_intersection( d1, alfa_, beta_ );
	MbVec3d i_ = intersection_eq( p2, norm( vector(p3,p2) ), distance );

	m_data = Data(alfa_,beta_,d1,distance,i_);

	return i_;
}

MbVec3d Algebra::intersection_method_A( const geo::IElement& e, eSurface _type )
{
	return _type == Bottom ? 
		intersection_method_A( convert( e.Point(0) ), convert( e.Point(2) ), convert( e.Point(3) ) ) :
		intersection_method_A( convert( e.Point(4) ), convert( e.Point(6) ), convert( e.Point(7) ) );
}

MbVec3d Algebra::intersection( const geo::IElement& e, eSurface _type )
{ 
	return intersection_method_B( e, _type ); 
	//return intersection_method_A( e, _type );
}

Algebra::TAxisM Algebra::axis_map( const geo::IElementSet* elems )
{
	//log_elements_type( elems );
	//std::string s_keys;

	TAxisM axis_m;

	int nE = elems->ElementSize();

	//IT_Printer::instance()->debug("axis_map : elems : size : %d", nE);

	for( int i=0; i < nE; i++ )
	{
		const geo::IElement& e = elems->Element( i );

		// include also interfaces
		// if( e.IsInterfaceElement() )
		//	continue;

		MbVec3d i_b = intersection( e, Bottom );
		MbVec3d i_t = intersection( e, Top );

		int idx = e.Index();

		//s_keys += std::to_string( idx ) + " ";

		/*if( i < 10 )
			IT_Printer::instance()->debug("idx : %d", idx);*/

		axis_m[ idx ] = std::make_pair(i_b,i_t);
	}

	//IT_Printer::instance()->debug("Map elems:");
	//IT_Printer::instance()->debug("%s", s_keys.c_str());

	return axis_m;
}

void log_elements_type( const geo::IElementSet* elems )
{
	int nE = elems->ElementSize();

	int n_if = 0;
	int n_if_s = 0;
	int n_if_l = 0;

	for( int i=0; i < nE; i++ )
	{
		const geo::IElement& e = elems->Element( i );

		// MCR 2019-05-28
		if( e.IsInterfaceElement() )
		{
			n_if++;

			const geo::IPoint& p0 = e.Point( 0 );
			const geo::IPoint& p1 = e.Point( 1 );
			const geo::IPoint& p2 = e.Point( 2 );
			const geo::IPoint& p3 = e.Point( 3 );
			const geo::IPoint& p4 = e.Point( 4 );
			const geo::IPoint& p5 = e.Point( 5 );
			const geo::IPoint& p6 = e.Point( 6 );
			const geo::IPoint& p7 = e.Point( 7 );

			if( p0 == p1 && p2 == p3 ) //surface, not volume
			{
				n_if_s++;

				int b=0;
				if( p1 == p5 )  //line
					int c=0;
			}

			if( p1 == p5 )  //line
				int c=0;

			if( p1 == p5 && p0 == p4 && p2 == p6 && p3 == p7 )  //line
				n_if_l++;
		}
	}
	IT_Printer::instance()->debug("axis_map : elems:%d iface:%d iface_s:%d iface_l:%d", nE, n_if, n_if_s, n_if_l);
}


std::vector<MbVec3d> Algebra::points_mod_DEPRECATED( const OIDIGeometryI& g, OIDIVectorSetI* vectors, double scale_gen, double scale_def )
{
	std::vector<MbVec3d> p_def_v; //Test

	std::vector<MbVec3d> points_v;

	std::vector<MbVec3d> def = convert( vectors );

	int nD = def.size();

	IT_Printer::instance()->debug("Deformation: size : %d", nD);

	std::vector<MbVec3d> def_print; //FIXME: Just for test proposal
	for( int i = 0; i < 8 ; i++ )
		def_print.push_back( def[ i ] );

	const geo::IElementSet* p = g.Parent();
	int nE = p->ElementSize();

	IT_Printer::instance()->debug("Elems : size : %d", nE);

	for( int i=0; i < nE; i++ )
	{
		const geo::IElement& e = p->Element( i );

		if( (i+1)*8 > nD )
			break;

		if( e.IsInterfaceElement() )
		{
			/*for( int j = 0; j < e.NrOfPoints(); j++ )
				points_v.push_back( convert( e.Point( j ) ) );*/
		}
		else
		{
			MbVec3d i_b = intersection_method_B( e, Bottom );
			MbVec3d i_t = intersection_method_B( e, Top );

			int nP = e.NrOfPoints();
			for( int j = 0; j < nP; j++ )
			{
				int idx = i*nP + j;
				MbVec3d p_def = deformed_point( convert( e.Point( j ) ), def[ idx ], scale_def );
				MbVec3d p_def_esc = scaled_point( p_def, j < 4 ? i_b : i_t, scale_gen );

				if( i == 0 ) //FIXME: Test
					p_def_v.push_back( p_def );
				
				points_v.push_back( p_def_esc );
			}
			if( i == 0 )
			{
				IT_Printer::instance()->debug("\nElem:");
				IT_Printer::instance()->debug("\nElem:");
				print_elem( e );
				IT_Printer::instance()->debug("\nDeformation:");
				print_points( def_print );
				IT_Printer::instance()->debug("\nDeformed element:");
				print_points( p_def_v );
				IT_Printer::instance()->debug("\nScaled element:");
				print_points( points_v );
			}
		}
	}
	return points_v;
}


std::vector<MbVec3d> Algebra::points_mod( const OIDIGeometryI& g, std::vector<MbVec3d> deformation_v, double scale_gen, double scale_def )
{
  scale_def = scale_def < 0.001 ? 0.001 : scale_def;
	scale_gen = scale_gen < 1.0 ? 1.0 : scale_gen;

	std::string s_keys;

	int orphans = 0;
	int iface_c = 0;

	int nP = 0; //TEST

	std::vector<MbVec3d> p_esc_v, p_esc_def_v, intersections_v; //Test

	std::vector<MbVec3d> points_v;

	int nD = deformation_v.size();

	const geo::IElementSet* elems = g.Parent();
	TAxisM axis_m = axis_map( elems );

	MbVec3d IFACE(-1,-1,-1);
	MbVec3d NO_ELEMENT(-1,-1,-1);

	std::size_t g_size = g.getSize();
	for( std::size_t i = 0; i < g_size; ++i )
	{
		const geo::IPoint* p = g.getPoint(i);

		if( !p )
			continue;

		const geo::INode& node = dynamic_cast<const geo::INode&>( *p ); 

		if( orphan( p ) ) //FIXME:
		{
			orphans++;
			MbVec3d point = convert( *p );
			points_v.push_back( point );
			//Test
			p_esc_v.push_back( point );
			p_esc_def_v.push_back( point );
			intersections_v.push_back( NO_ELEMENT );
		}
		else
		{
			// MCR 2019-05-28 
			/*if( iface( p ) )
			{
				iface_c++;
				MbVec3d point = convert( *p );
				points_v.push_back( point ); //don't transform points belonging to the iface

				p_esc_v.push_back( point );
				p_esc_def_v.push_back( point );
				intersections_v.push_back( IFACE );
			}
			else
			{*/
				const geo::IElement& e = node.AttachedElement( 0 );

				MbVec3d intersection = index( p ) < 4 ? axis_m[ e.Index() ].first : axis_m[ e.Index() ].second;

				intersections_v.push_back( intersection );

				MbVec3d p_esc = scaled_point( convert( *p ), intersection, scale_gen );
				MbVec3d p_esc_def = deformed_point( p_esc, deformation_v[ i ], scale_def );
				points_v.push_back( p_esc_def );

				//Test
				p_esc_v.push_back( p_esc );
				p_esc_def_v.push_back( p_esc_def );
			//}
		}
	}
	//--------------------------------------- LOGS [Ini]
	
	IT_Printer::instance()->debug("SIZES : Def: %d - Geo: %d - Elems: %d - Orphans: %d - IFace: %d ", nD, g.getSize(), axis_m.size(), orphans, iface_c);

	for( std::size_t i = 0; i < nP; ++i )
	{
		if( i == 0 )
		{
			IT_Printer::instance()->debug("\n");
			IT_Printer::instance()->debug("point");
			IT_Printer::instance()->debug("intersection");
			IT_Printer::instance()->debug("scaled point");
			IT_Printer::instance()->debug("deformation");
			IT_Printer::instance()->debug("scaled deformation");
			IT_Printer::instance()->debug("scaled and deformed point");
			IT_Printer::instance()->debug("\n");
		}

		IT_Printer::instance()->debug("%d:", i);

		if( equal(intersections_v[ i ], IFACE ) || 
			equal(intersections_v[ i ], NO_ELEMENT ) )
		{
			IT_Printer::instance()->debug("INTERFACE | ORPHAN ");
		}
		else
		{
			print_point( *g.getPoint( i ) );							// point
			print_point( intersections_v[ i ] );						// intersection
			print_point( p_esc_v[ i ] );								// scaled point
			print_point( deformation_v[ i ] );							// deformation
			print_point( scale_gen * scale_def * deformation_v[ i ] );	// scaled deformation
			print_point( points_v[ i ] );								// scaled and deformed point
		}
		IT_Printer::instance()->debug("\n");
	}
	//--------------------------------------- LOGS [End]
	return points_v;
}

std::vector<MbVec3d> Algebra::points_mod_SAME( const OIDIGeometryI& g, std::vector<MbVec3d> deformation_v, double scale_gen, double scale_def )
{
	std::vector<MbVec3d> points_v;
	std::size_t g_size = g.getSize();
	for( std::size_t i = 0; i < g_size; ++i )
		points_v.push_back( convert( *g.getPoint(i) ) );
	return points_v;
}


std::vector<MbVec3d> Algebra::convert( OIDIVectorSetI* vectors )
{
	static const double NaN = -99999999.999;

	std::vector<MbVec3d> data_v;

	for( int i = 0; i < vectors->dataSize(); i++ )
	{
		MbVec3d v = vectors->get( i );
		MbVec3d v_2 = MbVec3d(	v[0] == NaN ? 0.0 : v[0], 
								v[1] == NaN ? 0.0 : v[1], 
								v[2] == NaN ? 0.0 : v[2] );
		data_v.push_back( v_2 );
	}
	return data_v;
}

bool Algebra::outside( const geo::IPoint* p ){ return !inside( p ); }

bool Algebra::inside( const geo::IPoint* p )
{
	int idx = Algebra::index( p );
	return ( idx == 0 || idx == 4 || idx == 3 || idx == 7 );
}
bool Algebra::regular( const geo::IPoint* p )
{
	return ( !orphan( p ) && !iface( p ) );
}

bool Algebra::orphan( const geo::IPoint* p )
{
	const geo::INode* node = dynamic_cast<const geo::INode*>( p ); 
	return !(node && node->AttachedElementSize());
}

bool Algebra::iface( const geo::IPoint* p )
{
	const geo::INode* node = dynamic_cast<const geo::INode*>( p ); 
	return ( node && node->AttachedElementSize() && node->AttachedElement( 0 ).IsInterfaceElement() );
	//int size = node->AttachedElementSize(); // 2, 4 
}

int Algebra::index( const geo::IPoint* p )
{
	const geo::INode& node = dynamic_cast<const geo::INode&>( *p ); 

	if( node.AttachedElementSize() )
	{
		const geo::IElement& e = node.AttachedElement( 0 );

		if( !e.IsInterfaceElement() ) //FIXME
		{
			for( int i=0; i< e.NrOfPoints() ; i++ )
			{
				const geo::IPoint& p_ = e.Point( i ); 
				if( &p_ == p )
					return i;
			}
		}
	}
	return -1;
}

void Algebra::geometry_points_TEST( const OIDIGeometryI& g )
{
	std::size_t g_size = g.getSize();

	for (std::size_t i = 0; i < g_size; ++i)
	{
		const geo::IPoint* p = g.getPoint(i);

		//geo::CMesh::CNode* node = dynamic_cast<geo::CMesh::CNode*>(p); 

		const geo::INode& node = dynamic_cast<const geo::INode&>( *p ); 

		if( node.AttachedElementSize() ) //FIXME:
		{
			const geo::IElement& e = node.AttachedElement( 0 );
			int idx = index( p );
			int a=0;
		}
		else
			IT_Printer::instance()->debug("Orphan point : idx : %d", node.Index() );
	}
}

	//---------------------- Aproach B: Triangle scaling

MbVec3d Algebra::intersection_method_B( const geo::IElement& e, eSurface _type )
{
	MbVec3d p_end = convert( e.Point( _type == Bottom ? 3 : 7 ) );
	MbVec3d p_ini = convert( e.Point( _type == Bottom ? 2 : 6 ) );

	MbVec3d v1 = vector(p_ini,p_end); //sign counts !!!! so order
	double h = len( v1 );
	//MbVec3d v1_norm = v1 / h;
	double scale_ = scale( e );

	//MbVec3d i_ = p3_ + scale_ * h * v1_norm;
	MbVec3d i2_ = p_end + scale( e ) * v1; // would it be the same ?

	return i2_;
}

void Algebra::print_points( std::vector<MbVec3d> v, int size_ )
{
	int size = size_ ? size_ : v.size();
	for( int i = 0; i < size; i++ )
		IT_Printer::instance()->debug("%d: %f\t%f\t%f",i, v[i][0],v[i][1],v[i][2]);
}

void Algebra::print_point( MbVec3d p ){ IT_Printer::instance()->debug("P: %f\t%f\t%f", p[0],p[1],p[2]); }

void Algebra::print_elem( const geo::IElement& e )
{
	for( int i = 0; i < e.NrOfPoints(); i++ )
		print_point( e.Point(i) );
}

void Algebra::print_point( const geo::IPoint& p ){ IT_Printer::instance()->debug("P : %f\t%f\t%f", p.X(), p.Y(), p.Z() ); }

std::vector<MbVec3d> Algebra::scaled_elem( const geo::IElement& e, double scale_ )
{
	std::vector<MbVec3d> v;

	MbVec3d i_ = intersection( e, Bottom );
	for( int i = 0; i < 4; i++ )
		v.push_back( scaled_point( convert( e.Point( i ) ), i_, scale_ ) );

	i_ = intersection( e, Top );
	for( int i = 4; i < 8; i++ )
		v.push_back( scaled_point( convert( e.Point( i ) ), i_, scale_ ) );

	return  v;
}

MbVec3d Algebra::scaled_point( MbVec3d p, MbVec3d intersection_, double scale )
{
	MbVec3d v = vector( intersection_, p );

	//double distance = len( v );
	MbVec3d p_new = intersection_ + v * scale;
	return p_new;
}

MbVec3d Algebra::deformed_point( MbVec3d p, MbVec3d deformation, double scale )
{
	MbVec3d p_new = p + deformation * scale;
	return p_new;
}

double Algebra::scale( const geo::IElement& e, eSurface _type )
{
	return _type == Bottom ? 
		scale( e.Point(0), e.Point(1), e.Point(2), e.Point(3) ) :
		scale( e.Point(4), e.Point(5), e.Point(6), e.Point(7) );
}

double Algebra::scale( const geo::IPoint& p0, const geo::IPoint& p1, const geo::IPoint& p2, const geo::IPoint& p3 )
{
	return scale( convert( p0 ), convert( p1 ), convert( p2 ), convert( p3 ) );
}

double Algebra::scale(MbVec3d p0, MbVec3d p1, MbVec3d p2, MbVec3d p3 )
{
	double a = len( vector(p0,p3) );
	double b = len( vector(p1,p2) );
	return ( a / (b-a) );
}

double Algebra::equal(MbVec3d p0, MbVec3d p1 )
{
	MbVec3d d = p0-p1; //diff
	if( abs( d[0] ) > EPSILON || abs( d[1] ) > EPSILON /*|| abs( d[2] ) > EPSILON*/ )
		return false;
	return true;
}

std::vector<bool> Algebra::iface_flags( const geo::IElementSet* elems )
{
	std::vector<bool> iface_flags_v;

	for( int i=0; i < elems->ElementSize(); i++ )
		iface_flags_v.push_back( elems->Element( i ).IsInterfaceElement() );

	return iface_flags_v;
}

MbVec3d Algebra::min( std::vector<MbVec3d> points_v )
{
	MbVec3d min = points_v[0];
	for( int i=0; i < points_v.size(); i++ )
	{
		MbVec3d p = points_v[ i ];
		for( int j=0; j < 3; j++ )
			min[j] = p[j] < min[j] ? p[j] : min[j];
	}
	return min;
}
MbVec3d Algebra::max( std::vector<MbVec3d> points_v )
{
	MbVec3d max = points_v[0];
	for( int i=0; i < points_v.size(); i++ )
	{
		MbVec3d p = points_v[ i ];
		for( int j=0; j < 3; j++ )
			max[j] = p[j] > max[j] ? p[j] : max[j];
	}
	return max;
}

std::pair<MbVec3d,MbVec3d> Algebra::min_max( std::vector<MbVec3d> points_v )
{
	MbVec3d min = points_v[0];
	MbVec3d max = points_v[0];
	for( int i=0; i < points_v.size(); i++ )
	{
		MbVec3d p = points_v[ i ];
		for( int j=0; j < 3; j++ )
		{
			min[j] = p[j] < min[j] ? p[j] : min[j];
			max[j] = p[j] > max[j] ? p[j] : max[j];
		}
	}
	return std::make_pair(min,max);
}
