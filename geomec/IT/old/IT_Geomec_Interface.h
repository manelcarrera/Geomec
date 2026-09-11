#ifndef _IT_Geomec_Interface
#define _IT_Geomec_Interface

#include <list>
#include <string>

#include "BodyGroup.h"
#include "IVectorResult.h"
#include "ValueTypes.h"

#include "IT_Data.h"

#include "IGmInc.h"

class Depletion;
class CStringV;
class CModelBase;
class CFormationBase;
class CGeomecDoc;

#define FUNC_ENTER Printer::instance()->debug("-> %s", __FUNCTION__);
#define FUNC_EXIT Printer::instance()->debug("<- %s", __FUNCTION__);

/*static const double NaN = -9999.999; //FIXME


//for displacements
typedef std::pair< geo::IPoint&, geo::CValue > TPointValueP;
typedef std::vector< TPointValueP > TPointValueV;

typedef std::list< std::string > TStringL;

//for pressures
typedef std::vector< geo::CValue > TValuesV;
typedef std::pair< geo::IElement&, TValuesV > TElemValuesP;
typedef std::vector< TElemValuesP > TElemValuesV;

// general
typedef std::vector< geo::IPoint& > TPointsV;
typedef std::vector< geo::IElement& > TElemsV;

typedef std::vector< geo::IPoint& > TPoint;

typedef std::vector<IVectorResult::VECTOR_COMPONENT> TGeomecDisplacementComponentV;

//class CStringV;
typedef CStringV TReservoirL;
typedef CStringV TDepletionL;
typedef CStringV TDisplacementL;
typedef CStringV TSurfaceL;
typedef std::vector<IValueComposite*> TCompositeV;
typedef std::vector<CPointSet*> TPointSetV;*/

class IT_Geomec_Interface {
public:
  enum eDepletionType { Strain, Compressibility, Pore_Pressure };
  enum eMaterialParameter { PoissonsRatio_, YoungsModulus_, NotDefined_ };
  enum eReservoirProperty { Thickness, PoissonsRatio, YoungsModulus, PorePressure };
  enum eComponent { X, Y, Z };
  enum eType { Surface_, Volume };
  enum eComposite {
    C_Displacement,
    C_TotalStress,
    C_TotalStressChange,
    C_TotalStrain,
    C_PlasticStrain,
    C_EffectiveStress,
    C_EffectiveStressChange,
    C_PorePressure,
    C_Temperature,
    C_Consolidation,
    C_Porosity,
    C_FaultEffectiveNormalStress,
    C_FaultShearStress,
    C_FaultDisplacement,
    C_FaultDilatation,
    C_FaultPlasticSlip,
    C_Aperture,
    C_ShearCapacity,
    C_DerivedResults,
    C_NotDefined
  };
  enum eList { L_Surface, L_Formation, L_Reservoir, L_Depletion, L_Displacement, L_BD_Composite };

  typedef std::vector<eComponent> TDisplacementComponentV;
  typedef std::vector<eReservoirProperty> TReservoirPropertyV;

  static const int LAST_DEPLETION_STAGE;
  static const int ALL_DEPLETION_STAGES;
  static const std::string DEFAULT_POINT_SET_NAME;
  static const std::string DEFAULT_DEPLETION_STAGE_NAME;
  static const std::string DEFAULT_PRESSURE_NAME;
  static const std::string DEFAULT_STRAIN_NAME;
  static const std::string DEFAULT_MATERIAL_NAME;

protected:
  //--------------------------------------------------- base
  //-----------------------------------------------------------------
  static CGeomecDoc *doc();

  //--------------------------------------------------- Formations
  //-----------------------------------------------------------------

  /**/
  static CFormationBase *formation(int idx, bool bReservoir = true);

  /* TO REMOVE IN PHASE II */
  static std::string reservoir_header(TReservoirPropertyV properties_v);

  /* Conersion to Geomec IDs */
  static int material_id(eMaterialParameter material);

  /**/
  static eMaterialParameter convert(eReservoirProperty material);

  //--------------------------------------------------- Displacement
  //-----------------------------------------------------------------
  /* Conersion to Geomec IDs */
  static IVectorResult::VECTOR_COMPONENT geomec_component_type(eComponent _type);

  /* Returns just one component values */
  static const IVectorResult::CVectorComponent *
  G_RES_component_ONE(eComposite composite_, CDepletionStage *pStage,
                      IVectorResult::VECTOR_COMPONENT component = IVectorResult::VECTOR_COMPONENT::VC_Z);
  /**/
  static std::pair<IValueComposite *, CPointSet *> G_BD_composite(eComposite composite_id, int idx);

  /* TO REMOVE IN PHASE II */
  static std::string displacement_header(TDisplacementComponentV component_v);

  //--------------------------------------------------- depletion
  //-----------------------------------------------------------------

  /* Sets a given depletion stage output type to 'Branch' or 'Results' */
  static void branch(int depletion_id = LAST_DEPLETION_STAGE, bool is_branch = true);

  //--------------------------------------------------- MATERIAL
  //-----------------------------------------------------------------

  /**/
  CMaterialEntry *createMaterial(CModelBase &modelBase, const QString &materialName, int materialModel);

  /**/
  void connectMaterials2Formations(CModelBase &modelBase, const std::vector<IMaterial *> materials);

  /**/
  void connectMaterial2Formation(CModelBase &modelBase, IMaterial *material, CFormationBase *formationBase);

  /**/
  bool LoadMaterial(CModelBase &model);

  //--------------------------------------------------- GENERIC
  //-----------------------------------------------------------------

  /* Generic function */
  static std::pair<TCompositeV, TPointSetV> G_BD_composite_list(eComposite composite_id);

  /* Generic function */
  static const IResult *G_result(eComposite composite_, CDepletionStage *pStage);

  /* Generic function */
  static const IResultComponent *G_RES_component(eComposite composant_, CDepletionStage *pStage,
                                                 IVectorResult::VECTOR_COMPONENT _component);

  /* Generic function */
  static int G_composite_id(eComposite composant_);

protected: /* NOT USED */
  /* NOT USED */
  // static CFormationBase* formation( const std::string formation_id );

  /* NOT USED */
  // static const IPointSet& points( const CFormationBase& formation );

  /* NOT USED */
  // static const geo::IElementSet& elements( const CFormationBase& formation );

private:
  //--------------------------------------------------- depletion
  //-----------------------------------------------------------------
  /*
    Adds a pressure changement to a given reservoir depletion stage.

    Pressure can be: strain, pore pressure or compressibility.

    By default a new depletion stage is added to the end and the pressure changement is added to it.

    By default created depletion stage is marked as 'branch' type; however 'results' type can be also choosen.

    First time a point set is created and pressure changement values are added to the point set.

    Next new point set is maped to the model.

    From second pressure chanegement onwards creted point set is re-used.

    Parameters:
    -----------

      reservoir_id:	reservoir index

      points_v:		points in the model to wich the pressure changement will be mapped

      pressures_v:	pressure changement values (per point)

      _type:			strain, pore pressure or compressibility

      depletion_id	depletion stage to witch add the pressure changemewnt
              if this paaremeter is not informed a new depletion stage is creted and pressure changement added to it

      is_branch		new created depletion stage type
              true means 'branch' type and false 'results'.

    Misc.:
    ------------
      IDT_VALUETYPE_ELASCOMPRES -> Material
      IDT_VALUETYPE_STRAIN -> TENSOR, 6x
      IDT_VALUETYPE_NRMSTRAIN -> Northern
      IDT_VALUETYPE_LATSTRAIN -> Lateral, 1x even if it's related to two dimensions

  */
  static void add_pressure(int reservoir_id, const IT_Data::TPointsV &points_v, const IT_Data::TValuesV &pressures_v,
                           eDepletionType _type = Pore_Pressure, int depletion_id = LAST_DEPLETION_STAGE,
                           bool is_branch = true);

  /* Gives the current model depletion stages number */
  static int num_depletion_stages();

  /**/
  static int G_add_point_set(const IT_Data::TPointsV &points_v, eDepletionType _type, int depletion_id);

  /**/
  int G_add_values_to_point_set(int ps_id, const IT_Data::TValuesV &pressures_v);

  /**/
  bool G_link_point_set(int ps_id, int reservoir_id, int depletion_id);

  //------------------------------------------------------- LISTs
  //--------------------------------------------------------------------------
  /**/
  static TReservoirL &formations_list(bool is_depleting = true);

  /**/
  static TSurfaceL &surfaces_list(int reservoir_id);

  /*
    All depletions names list.

    Depletions names are provided in a single string delimited by tokens.
  */
  static TDepletionL &depletions_list();

private: /* NOT USED */
  /* NOT USED */
  // TPointValueV pressure(		int reservoir_id,
  //							int depletion_id = 0 );

public:
  IT_Geomec_Interface() {};

  //------------------------------------------------------- base
  //--------------------------------------------------------------------------
  /* The current model */
  static CModelBase *model();

  /* Sends a post-message to Geomec to run a non-linear analysis */
  static void run_model();

  /* Has the model results ? */
  static bool results_available();

  //------------------------------------------------------- depletion
  //--------------------------------------------------------------------------
  /* Depletion stage */
  static CDepletionStage *depletion_stage(int idx = LAST_DEPLETION_STAGE);

  /*
    Just calls 'add_pressure()' private method.

    Look at it for further details.
  */
  static void
  add_pressure(int reservoir_id,     // Formation to wich add the provided pressure
               Depletion &depletion, // Pressure to add : points + pressure
               eDepletionType _type =
                   Pore_Pressure, // Format of the provided pressure : Strain, Compressibility or Pore_Pressure
               int depletion_id = LAST_DEPLETION_STAGE, // Default params adds a new depletion stage
               bool is_branch = true);                  // Branch | Results

  /*
    Adds a new depletion stage and sets its 'output type' to Branch or Results

    Branch is the default 'output type'
  */
  static void add_depletion_stage(bool is_branch = true);

  /*
    Removes a given depletion stage.

    Params:
    -------
      None:					Then LAST_DEPLETION_STAGE
      Id:						Then the given depletion stage
      ALL_DEPLETION_STAGES:	Then ALL
  */
  static void remove_depletion_stage(int depletion_id = LAST_DEPLETION_STAGE);

  static CStringV &list(eList type_, int param = -1);

  //------------------------------------------------------- displacement (LIST)
  //--------------------------------------------------------------------------
  /**/
  static const IVectorResult::CFullVectorComponent *G_RES_component_FULL(eComposite composite_,
                                                                         CDepletionStage *pStage);

  //------------------------------------------------------- GENERIC
  //--------------------------------------------------------------------------

  /* Generic function */
  static CStringV &G_BD_composite_names_list(eComposite composite_id);

  /**/
  static bool G_Filter(const geo::CPoint &point, CPointSet *ps);

  /**/
  static IT_Data::TPointsV BD_points(eComposite composite_, int composite_idx);

  /**/
  static IT_Data::TPointsV BD_points(CPointSet *ps);

  /**/
  static IT_Data::T3ValuesV BD_values(eComposite composite_, int composite_idx,
                                      TDisplacementComponentV component_v = {X, Y, Z});

  /**/
  static IT_Data::T3Values BD_value(const geo::IPoint &point, IValueComposite *composite,
                                    TDisplacementComponentV &component_v);
};

#endif