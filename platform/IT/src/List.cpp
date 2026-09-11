#include "List.h"
#include "Data.h"

CStringV::CStringV() : IT_Data(){}

TData CStringV::data()
{
  m_elems_serie = serialize( m_elemsV );
  return TData{ 1, IT_Data::Cols_Header, (TMemoryChunkPtr)m_elems_serie.data(), DataType::String }; 
}

TData CStringV::object( int _type ){ return data(); }

std::string CStringV::serialize( TStringV& string_v,  const std::string TOKEN_SEPARATOR ) //FIXME: now you know how to pass an string list si no needed 
{
  std::string string_s;

  for( int i=0; i < string_v.size(); i++ ) 
    string_s.append( string_v[ i ] + TOKEN_SEPARATOR );
  if( string_s.size() )
    string_s.pop_back();

  return string_s;
}
