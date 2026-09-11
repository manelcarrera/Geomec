#include "IT_MaterialParameter.h"

MaterialParameter::MaterialParameter() : IT_Data(){};
TData MaterialParameter::vals_d(){ return data<double>(	m_vals,	IT_Data::Cols_Params ); }
void MaterialParameter::vals( TValuesV val ){ m_vals = val; };
TData MaterialParameter::object( int _type )
{ 
  switch( _type )
  {
    case Values:	return vals_d();
    default:		return TData_INVALID;
  }
}
