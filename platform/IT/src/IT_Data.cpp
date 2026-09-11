#include "IT_Data.h"

TDataV IT_Data::tuple()
{ 
  TDataV data_v;
  for( int i=0; i < num_objects(); i++ )
    data_v.push_back( object( i ) );
  return data_v;
};