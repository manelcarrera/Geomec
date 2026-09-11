#ifndef _List
#define _List

#include "IT_Data.h"
#include "Data.h"

//----------------------------------------------------------- List
/*
  Data container that encapsules strings lists. 
*/
class CStringV : public IT_Data
{
public:

  enum Type
  {
    Elems,
    Num_Objects
  };

private:

  TStringV m_elemsV;
  std::string m_elems_serie;

  TData data();

  std::string serialize( TStringV& string_v, const std::string TOKEN_SEPARATOR = "*" );


public:
  CStringV();

  void elems( TStringV elems_v ){	m_elemsV = elems_v; };
  TStringV& elems(){ return m_elemsV; }; //FIXME

  virtual TData object( int _type = Elems );
  virtual int num_objects(){ return Type::Num_Objects; };
};

#endif
