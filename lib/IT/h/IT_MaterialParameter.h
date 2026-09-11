#ifndef _MaterialParameter
#define _MaterialParameter

#include "IT_Data.h"

//----------------------------------------------------------- Value
/* 
	Data object that encapsules a vector of values (doubles) 
*/
class MaterialParameter : public IT_Data
{
	enum Type{ Values, Num_Objects };
	TValuesV m_vals;
	TData vals_d();

public:
	MaterialParameter();
	void vals( TValuesV val );//{	m_values = val; };

	virtual TData object( int _type = 0 );
	virtual int num_objects(){ return Type::Num_Objects; };
};

#endif