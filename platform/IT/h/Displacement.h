#ifndef _Displacement
#define _Displacement

#include "IT_Data.h"
#include "Data.h"

//----------------------------------------------------------- Displacement
/*
  Data object that encapsules some displacement properties:.

  Memebers:
  ---------

    Points:		coordinates: x, y, z

    wdat:		not implemented
          its value is hard-coded
  
    Subsidence:	calculated displacement.

    Header:		defines the content of 'subsidence' that can be whatever combination of 'dx', 'dy', and 'dx' ->'dx dy dz', 'dx dy', 'dx dz', 'dx', 'dy', dz
        
          'dx': subsidence in the 'x' axe
          'dy': subsidence in the 'y' axe
          'dz': subsidence in the 'z' axe
*/
class Displacement : public IT_Data
{
  enum Type
  {
    Elems,
    Z,
    Wdat,
    Subsidence,
    Num_Objects
  };

  static const int DEF_WDAT;

  //data
  TIntV m_elems;
  TCoordinateV m_z;
  double m_wdat;
  TValuesV m_dx, m_dy, m_dz;	// py:subs
  T3ValuesV m_subsidence;

  //data ptr
  TData z_d();
  TData elems_d();
  TData wdat_d();
  TData dx_d();
  TData dy_d();
  TData dz_d();
  TData subsidence_d();

public:
  Displacement();
  //set
  void elems( TIntV val ){			m_elems = val; };
  void wdat( double val ){			m_wdat = val; };
  void dx( TValuesV val ){			m_dx = val; };
  void dy( TValuesV val ){			m_dy = val; };
  void dz( TValuesV val ){			m_dz = val; };
  void z(TCoordinateV z){ m_z = z; }
  void subsidence( T3ValuesV val ){	m_subsidence = val; };
  //get
  //TPointsV& points(){			return m_points; };
  TIntV& elems(){				return m_elems; };
  double& wdat(){				return m_wdat; };
  TValuesV& dx(){				return m_dx; };
  TValuesV& dy(){				return m_dy; };
  TValuesV& dz(){				return m_dz; };
  T3ValuesV& subsidence(){	return m_subsidence; };
  TCoordinateV& z(){ return m_z; }

  virtual TData object( int _type );
  virtual int num_objects(){ return Type::Num_Objects; };
};


#endif