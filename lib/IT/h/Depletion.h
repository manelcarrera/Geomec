#ifndef _Depletion
#define _Depletion

#include "IT_Data.h"


/*
	Data object that encapsules a strain, pore pressure or compressibility change.

	Used for both, points and triangles, depending on the case. 

	Memebers:
	---------

		Elemnents:
		Points:
		Points:
		Pressure:

		Values:		not used
*/
class Depletion : public IT_Data
{
	enum eElemType
	{
		Tri,
		Tetra
	};


public:

	enum eData
	{
		Points,
		Values
	};

private:

	eElemType m_elem_type;

	//data
	TIntV m_elems_id;		// py:x,y,z
	TPointsV m_points;		// py:x,y,z
	TValuesV m_pressure;	// py: ??

	int m_reservoir_idx;
	int m_stage_idx;
	std::string m_reservoir_name;

public:

	Depletion( eElemType _type =  Tetra ) : IT_Data(), m_elem_type( _type ){}
	//set
	void elems_id( TIntV val ){ m_elems_id = val; };
	void points( TPointsV _points ){ m_points = _points; };
	void pressure( TValuesV _pressure ){ m_pressure = _pressure; };
	//get
	TIntV& elems_id(){ return m_elems_id; };
	TPointsV& points(){ return m_points; };
	TValuesV& pressure(){ return m_pressure; };
};


#endif

