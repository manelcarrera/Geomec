#pragma once

#include "Printer.h"
#include "IT_Data.h"

#include <string>
#include <map>

class IT_Printer : public Printer
{

public:
	
	IT_Printer(){};

	static void _print_tri( IT_Data::TTriV tri_v, int num_items = 0 );
	static void _print_tri( IT_Data::TTriV elems_v, std::vector<int> id_v );

	static void _print_tetra( IT_Data::TTetraV tetra_v, int num_items = 0 );
	static void _print_points( IT_Data::TPointsV poiuts_v, int num_items = 0 );
	static void _print_points( IT_Data::TPointsV points_v, std::vector<int> id_v );

	static void _print_params( IT_Data::T4ValuesV params_v, int num_items = 0 );

	static void _print_points_map( std::map<IT_Data::TPoint, int, IT_Data::TPoint_Compare> points_m, int num_items = 0 );

	static void _print_vals( IT_Data::TValuesV vals_v, int num_items = 0 );

	static void numbers( std::vector< int > numbers_v, int num_items = 0 );
};