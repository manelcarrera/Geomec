#ifndef _IT_Geomec_Interface_Dummy
#define _IT_Geomec_Interface_Dummy

#include "IT_Data.h"
#include "IT_Geomec_Interface.h"

class Reservoir;
class Displacement;

class IT_Geomec_Interface_Dummy : public IT_Geomec_Interface
{

public: //FIXME
  //static const IT_Data::TElemsV ELEMS_V;
  static const IT_Data::TTriV TRIANGLES_V;
  static const IT_Data::TTetraV TETRAS_V;
  static const IT_Data::TPointsV POINTS_V;
  static const IT_Data::TValuesV THICKNESS_V;
  static const IT_Data::TValuesV DISPLACEMENT_V;
  static const IT_Data::TValuesV PRESSURE_V;
  static const std::string HEADER_DIS; //FIXME: Space or tab?
  static const std::string HEADER_RES;
  static const double WDAT;

public:

  IT_Geomec_Interface_Dummy(){};
  static Reservoir& reservoir( int reservoir_idx = 0 );
  static Displacement& displacement_DEPRECATED( int reservoir_idx = 0 );

  static TReservoirL& formations_list();
  static TDepletionL& depletions_list();

  static void add_pressure(	int reservoir_id, 
                Depletion& depletion,
                int depletion_id = IT_Geomec_Interface::LAST_DEPLETION_STAGE,
                bool is_branch = true );
};

#endif
