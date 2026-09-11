#include "stdafx.h"

#include "IT_Geomec_Interface_Dummy.h"

#include "IGeomec.h"


#include "FormationBase.h"

#include "ResultTree.h"
#include "MeshResultTree.h"

#include "IValueComponent.h"
#include "IValueComposite.h"

#include "GeomecDoc.h" // model & PostMessage

#include "Printer.h"
#include "PointSet.h"
#include "NodalValueSet.h"
#include "Pressure.h"
#include "TetraFormation.h"

#include "ISceneWrapper.h"

#include "ModelTreeView.h"
#include "ModelView.h"

#include "Reservoir.h"
#include "Displacement.h"
#include "List.h"

//typedef IT_Geomec_Interface base;


//----------------------------------------------------------------------------------------------------------------------------------------

//#define CASE_1
#ifdef CASE_1 // case 1 : invented data
  static const IT_Data::TElemsV ELEMS_V		= IT_Data::TElemsV{	IT_Data::TElem{1,2,3}, 
                                IT_Data::TElem{3,4,5}};

  static const IT_Data::TPointsV POINTS_V	= IT_Data::TPointsV{	IT_Data::TPoint{100.00,101.01,102.02}, 
                                IT_Data::TPoint{200.00,201.01,202.02},
                                IT_Data::TPoint{300.00,301.01,302.02},
                                IT_Data::TPoint{400.00,401.01,402.02},				
                                IT_Data::TPoint{500.00,501.01,502.02} };

  static const IT_Data::TValuesV THICKNES_V	= IT_Data::TValuesV{	30.00, 
                                31.01,
                                32.02,
                                33.03,
                                34.04 };

  static const IT_Data::TValuesV DISPLACEMENT_V	= IT_Data::TValuesV{	-0.01, 
                                  -0.02,
                                  -0.03,
                                  -0.04,
                                  -0.05, };

  const IT_Data::TValuesV IT_Geomec_Interface_Dummy::PRESSURE_V	= IT_Data::TValuesV{	-0.0001, 
                                          -0.0002,
                                          -0.0003,
                                          -0.0004,
                                          -0.0005 };
#else

  const IT_Data::TTriV IT_Geomec_Interface_Dummy::TRIANGLES_V		= IT_Data::TTriV{	IT_Data::TTri{1,2,3} };
  const IT_Data::TPointsV IT_Geomec_Interface_Dummy::POINTS_V	= IT_Data::TPointsV{	IT_Data::TPoint{-1108.9432373046875,	-1135.0050048828125,	1000}, 
                                          IT_Data::TPoint{-1057.92626953125,		-1166.4217529296875,	1000},
                                          IT_Data::TPoint{-1111.427001953125,	-1185.33984375,			1000} };

  const IT_Data::TValuesV IT_Geomec_Interface_Dummy::THICKNESS_V	= IT_Data::TValuesV{	10.00, 
                                          10.00,
                                          10.00 };

  const IT_Data::TValuesV IT_Geomec_Interface_Dummy::DISPLACEMENT_V	= IT_Data::TValuesV{	-0.011148191, 
                                            -0.009287822,
                                            -0.015178746 };

  const IT_Data::TValuesV IT_Geomec_Interface_Dummy::PRESSURE_V	= IT_Data::TValuesV{	-1.50723E+07, 
                                          -4.21349E+06,
                                          -9.51209E+06 };
#endif

// common
const std::string IT_Geomec_Interface_Dummy::HEADER_DIS		= "x y z dz"; //FIXME: Space or tab?
const std::string IT_Geomec_Interface_Dummy::HEADER_RES		= "dz";
const double IT_Geomec_Interface_Dummy::WDAT				= 4000.0;

TReservoirL& IT_Geomec_Interface_Dummy::formations_list()
{
  TReservoirL& formations_v = *(new TReservoirL());
  formations_v.elems( IT_Data::TStringV{"Reservoir 1", "Reservoir 2" } );
  return formations_v;
}

TDepletionL& IT_Geomec_Interface_Dummy::depletions_list()
{
  TDepletionL& depletions_v = *(new TDepletionL());
  depletions_v.elems( IT_Data::TStringV{"Stage 1", "Stage2 2" } );
  return depletions_v;
}

Reservoir& IT_Geomec_Interface_Dummy::reservoir( int reservoir_id ) //FIXME: delete the Reservoir
{
  FUNC_ENTER
  Reservoir& reservoir = *(new Reservoir( Reservoir::eElemType::Tri ));
  //Reservoir& reservoir = *(new Reservoir());

  Printer::instance()->debug("get : res : surf : res_id:%d -> num: points:%d elems:%d params:%d header:%s", 
    reservoir_id, POINTS_V.size(), TRIANGLES_V.size(), THICKNESS_V.size(), HEADER_RES.c_str() );

  reservoir.tri(		TRIANGLES_V );
  reservoir.points(	POINTS_V );
  //reservoir.params(	THICKNESS_V ); //FIXME
  reservoir.header(	HEADER_RES );

  FUNC_EXIT
  return reservoir;
}

Displacement& IT_Geomec_Interface_Dummy::displacement_DEPRECATED( int reservoir_id ) // 2x points, 2x values
{
  FUNC_ENTER
  Displacement& displacement = *( new Displacement() );

  Printer::instance()->debug("get : disp : fake : res_id:%d -> num: points:%d subsidence:%d header:%s wdat:%.2f", 
    reservoir_id, POINTS_V.size(), DISPLACEMENT_V.size(), HEADER_DIS.c_str(), WDAT );

  //displacement.points(		POINTS_V );
  displacement.dx(			DISPLACEMENT_V );
  displacement.dy(			DISPLACEMENT_V );
  displacement.dz(			DISPLACEMENT_V );
  displacement.header(		HEADER_DIS );
  displacement.wdat(			WDAT );

  FUNC_EXIT
  return displacement;
}

void IT_Geomec_Interface_Dummy::add_pressure(	int reservoir_id, 
                        Depletion& depletion,
                        int depletion_id,
                        bool is_branch )
{
  FUNC_ENTER
  Printer::instance()->debug("add : depletion : fake : res_id:%d -> num: points:%d pressures:%d", 
    reservoir_id, POINTS_V.size(), PRESSURE_V.size() );

  //IT_Geomec_Interface::add_pressure( reservoir_id, POINTS_V, PRESSURE_V );
  FUNC_EXIT
}