#include "stdafx.h"

#include "IT_Printer.h"

void IT_Printer::_print_tri( IT_Data::TTriV elems_v, int num_items )
{
  for( int i = 0 ; i< elems_v.size(); i++ )
  {
    if( num_items && i == num_items ) break;

    IT_Data::TTri elem = elems_v[ i ];
    Printer::instance()->debug( "TRI [%d %d %d]", elem.p1, elem.p2, elem.p3 );
  }
}

void IT_Printer::_print_tri( IT_Data::TTriV elems_v, std::vector<int> id_v )
{
  for( int i = 0 ; i < id_v.size(); i++ )
  {
    int idx = id_v[ i ];
    if( idx < elems_v.size() )
    {
      IT_Data::TTri elem = elems_v[ idx ];
      Printer::instance()->debug( "TRI [%d %d %d]", elem.p1, elem.p2, elem.p3 );
    }
  }
}

void IT_Printer::_print_tetra( IT_Data::TTetraV elems_v, int num_items )
{
  for( int i = 0 ; i< elems_v.size(); i++ )
  {
    if( num_items && i == num_items ) break;

    IT_Data::TTetra elem = elems_v[ i ];
    Printer::instance()->debug( "TETRA [%d %d %d]", elem.p1, elem.p2, elem.p3 );
  }
}

void IT_Printer::_print_points( IT_Data::TPointsV points_v, int num_items )
{
  for( int i = 0 ; i< points_v.size(); i++ )
  {
    if( num_items && i == num_items ) break;

    IT_Data::TPoint point = points_v[ i ];
    Printer::instance()->debug( "POINT [%d] [%f %f %f]", i, point.x, point.y, point.z );
  }
}

void IT_Printer::_print_points( IT_Data::TPointsV points_v, std::vector<int> id_v )
{
  for( int i = 0 ; i < id_v.size(); i++ )
  {
    int idx = id_v[ i ];
    if( idx < points_v.size() )
    {
      IT_Data::TPoint point = points_v[ idx ];
      Printer::instance()->debug( "POINT [%d] [%f %f %f]", i, point.x, point.y, point.z );
    }
  }
}

void IT_Printer::_print_params( IT_Data::T4ValuesV params_v, int num_items )
{
  for( int i = 0 ; i< params_v.size(); i++ )
  {
    if( num_items && i == num_items ) break;

    IT_Data::T4Values params = params_v[ i ];
    Printer::instance()->debug( " PARAM [%f %f %f %f]", params.v1, params.v2, params.v3, params.v4 );
  }
}

void IT_Printer::_print_vals( IT_Data::TValuesV vals_v, int num_items )
{
  for( int i = 0 ; i< vals_v.size(); i++ )
  {
    if( num_items && i == num_items ) break;
    Printer::instance()->debug( " value: [%f]", vals_v[ i ] );
  }
}

void IT_Printer::numbers( std::vector< int > numbers_v, int num_items )
{
  std::string s = std::string();
  for( int i = 0 ; i< numbers_v.size(); i++ )
  {
    if( num_items && i == num_items ) 
      break;
    s +=  (std::to_string( numbers_v[ i ] ) + ", ");
  }
  Printer::instance()->debug( "numbers: [ %s ]", s.substr( 0, s.size()-2 ).c_str() );
}


void IT_Printer::_print_points_map( std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare> points_m, int num_items )
{
  int i=0;
  for( std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare>::const_iterator it = points_m.begin(); it != points_m.end(); ++it )
  {
    if( num_items && i == num_items ) break;

    IT_Data::TPoint p = (*it).first;
    int idx = (*it).second;
    Printer::instance()->debug("(%f,%f,%f):%d",p.x,p.y,p.z,idx);

    i++;
  }
}