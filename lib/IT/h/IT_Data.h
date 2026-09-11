#ifndef _IT_Data
#define _IT_Data

#include <vector>
#include "Data.h"

//----------------------------------------------------------- Data
/*
	Base class of the data clases used to implement the data exchange in between 'Geomec' and the 'Inversion Tool'
*/
class IT_Data
{
public:

	enum ePyTuplePos{ Cmd_, Params, Data_ };

	const int Cols_Point		= 3; 
	const int Cols_Elems_Tri	= 3; 
	const int Cols_Elems_Tetra	= 4; 
	const int Cols_Params		= 4; 
	const int Cols_Subsidence	= 3; 
	const int Cols_Header		= 1; 
	const int Cols_Coordinate	= 1; 
	const int Cols_Values		= 1; 
	const int Cols_Elems		= 1; 

	struct TPointValue{ double x; double y; double z; double value; };
	struct TPoint{ double x; double y; double z; };

	struct T3Values{ double v1; double v2; double v3; };
	struct T4Values{ double v1; double v2; double v3; double v4; };

	struct TTri{ int p1; int p2; int p3; };
	struct TTetra{ int p1; int p2; int p3; int p4; };

	struct TPoint_Compare // 'std::abs' does itself 'std::fabs'
	{
		bool operator() (const TPoint& lhs, const TPoint& rhs) const
		{
			int factor_i = 10000;
			double factor_d = 10000.0;

			double x_l = round(lhs.x*factor_i)/factor_d;
			double x_r = round(rhs.x*factor_i)/factor_d;
			double y_l = round(lhs.y*factor_i)/factor_d;
			double y_r = round(rhs.y*factor_i)/factor_d;
			double z_l = round(lhs.z*factor_i)/factor_d;
			double z_r = round(rhs.z*factor_i)/factor_d;
			
			return (
				x_l < x_r ||
				x_l == x_r && (
					y_l < y_r ||
					y_l == y_r && (
						z_l < z_r
						)
					)
	         );
		}
	};

	typedef std::vector< TPointValue > TPointsValueV;
	typedef std::vector< TPoint > TPointsV;
	typedef std::vector< TTetra > TTetraV;
	typedef std::vector< TTri > TTriV;

	typedef std::vector< double > TValuesV;
	typedef std::vector< T4Values > T4ValuesV;
	typedef std::vector< T3Values > T3ValuesV;

	typedef std::vector< int > TIntV;
	typedef std::vector< std::string > TStringV;

	typedef double TCoordinate;
	typedef std::vector<TCoordinate> TCoordinateV;

	//params (IT Phase2)
	TIntV m_params;
	TIntV& params(){ return m_params; };
	void params( TIntV val ){ m_params = val; };


public:
	IT_Data(){};
	IT_Data( TIntV params_ ){ params( params_ ); };
	//IT_Data( TDataInput val ) : m_data_input( val ){}
	virtual TDataV tuple();
	virtual TData object( int _type = 0 ){ return {0,0,nullptr}; };

	//virtual int num_objects()=0;
	virtual int num_objects(){ return 0; };
};

#endif
