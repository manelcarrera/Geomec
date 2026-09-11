#include "stdafx.h"

#include "IT_Geomec_Interface.h"
#include "IT_Geomec_Interface_Dummy.h" //FIXME
// #include "IT_Geomec_Interface_Fake.h" //FIXME

#include "FormationBase.h"

#include "MeshResultTree.h"
#include "ResultTree.h"

#include "IValueComponent.h"
#include "IValueComposite.h"

#include "GeomecDoc.h" // model & PostMessage

#include "NodalValueSet.h"
#include "PointSet.h"
#include "Pressure.h"
#include "Printer.h"
#include "TetraFormation.h"

#include "ISceneWrapper.h"
#include "treectrlbase.h"

#include "ModelTreeView.h"
#include "ModelView.h"

#include "Material.h" //Poisson ratio
#include "StrainLoad.h"

#include "IGeomec.h"

#include "Depletion.h"
#include "List.h"

#include "IT_Printer.h"

#include "SurfaceDesc.h"
#include "TetSurface.h"

#include "PointSetManager.h"

#include "MaterialHelperFactory.h"

// RESULTS
#include "ResultTree.h"

static const int NOT_DEFINED = -1; // FIXME

const std::string IT_Geomec_Interface::DEFAULT_DEPLETION_STAGE_NAME = "Depletion Stage %d";

const int IT_Geomec_Interface::LAST_DEPLETION_STAGE = -1;
const int IT_Geomec_Interface::ALL_DEPLETION_STAGES = -2;

static const bool IN_HULL_FILTER = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Base
//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

CModelBase *IT_Geomec_Interface::model() { return (CModelBase *)(GetGeomecDoc()->Model()); }
CGeomecDoc *IT_Geomec_Interface::doc() { return (CGeomecDoc *)(GetGeomecDoc()); }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Run model
//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void IT_Geomec_Interface::run_model() { AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_ON_RUN_MODEL, 0); }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------- MATERIAL
//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace {

bool copyMaterialLibraryValue(CLibraryMaterial *oldMatLib, CLibraryMaterial *newMatLib, int fromValueType,
                              int toValueType_1, int toValueType_2 = -1) {
  CLibraryMaterialParameter *oldParam, *newParam;

  if ((oldParam = oldMatLib->ParameterByValueTypeID(fromValueType))) {
    if (!newMatLib->ParameterByValueTypeID(fromValueType)) {
      double oldValue = oldParam->Value();

      if ((newParam = newMatLib->ParameterByValueTypeID(toValueType_1)))
        newParam->Value(oldValue);

      if (toValueType_2 > -1 && (newParam = newMatLib->ParameterByValueTypeID(IDT_VALUETYPE_YOUNGMODULUS_TRANS)))
        newParam->Value(oldValue);
    }
    return true;
  }
  return false;
}
} // namespace

bool IT_Geomec_Interface::LoadMaterial(CModelBase &model) {
  CMaterialEntry *materialEntry = createMaterial(model, "linear rgi", MM_LINEAR); // FIXME

  CGraphEntry::TEntryNodeSet entryNodeSet = materialEntry->GraphEntryNodes();

  IMaterial *material = dynamic_cast<IMaterial *>(*(entryNodeSet.begin()));

  TFormationBaseEntry *formationBaseEntry = dynamic_cast<TFormationBaseEntry *>(model.GraphEntry(MD_BASE_FORMATION));

  std::vector<IMaterial *> materials(formationBaseEntry->EntryNodes().size(), material);

  return true; // FIXME
}

CMaterialEntry *IT_Geomec_Interface::createMaterial(CModelBase &modelBase, const QString &materialName,
                                                    int materialModel) {
  CMaterialEntry *materialEntry = dynamic_cast<CMaterialEntry *>(modelBase.GraphEntry(MD_ROCK_MATERIAL));

  ml::CMaterialLibrary &materialLibrary = materialEntry->MaterialLibrary();

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  ml::TMaterialCreatorRefPtr materialCreator = f->getMatCreator(materialModel);

  QString name(!materialName.isEmpty() ? materialName : materialCreator->MaterialModelName() + " (inversion)");

  materialLibrary.AddMaterial(name, *materialCreator);

  return materialEntry;
}

void IT_Geomec_Interface::connectMaterials2Formations(CModelBase &modelBase, const std::vector<IMaterial *> materials) {
  TFormationBaseEntry *formationBaseEntry =
      dynamic_cast<TFormationBaseEntry *>(modelBase.GraphEntry(MD_BASE_FORMATION));

  size_t material = 0;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation = formationBaseEntry->EntryNodes().begin();
       formation != formationBaseEntry->EntryNodes().end(); ++formation, ++material) {
    connectMaterial2Formation(modelBase, materials[material], *formation);
  }
}

void IT_Geomec_Interface::connectMaterial2Formation(CModelBase &modelBase, IMaterial *material,
                                                    CFormationBase *formationBase) {
  if (formationBase) // formation may be 0 when we were handling interface elements
  {
    CDepletionStageEntry *depletionStageEntry =
        dynamic_cast<CDepletionStageEntry *>(modelBase.GraphEntry(MD_BASE_DEPLETION_STAGE));

    for (CDepletionStageEntry::iterator stage = depletionStageEntry->begin(); stage != depletionStageEntry->end();
         ++stage) {
      CMaterialServer *materialServer = formationBase->ConnectedMaterial(*stage);

      if (materialServer != 0) {
        if (materialServer->LibraryMaterial() != 0) {
          if (materialServer->MaterialModel() != material->MaterialModel()) {
            QString name = material->Name();

            material->CloneValues(*materialServer->LibraryMaterial());

            material->Name(name);

            CLibraryMaterial *oldMatLib = &materialServer->LibraryMaterial()->LibraryMaterial();
            CLibraryMaterial *newMatLib = &material->LibraryMaterial();

            copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNGS_MODULUS,
                                     IDT_VALUETYPE_YOUNGMODULUS_NORM, IDT_VALUETYPE_YOUNGMODULUS_TRANS);

            if (!copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNG_DECOMP,
                                          IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                                          IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP))
              copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNGS_MODULUS,
                                       IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);

            copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSONS_RATIO,
                                     IDT_VALUETYPE_POISSONRATIO_NORM, IDT_VALUETYPE_POISSONRATIO_TRANS);

            if (!copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSON_DECOMP,
                                          IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                                          IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP))
              copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSONS_RATIO,
                                       IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);

            copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_SHEARMODULUS,
                                     IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                                     IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);
          }

          formationBase->ConnectedMaterial(*stage)->UnLink(
              *(formationBase->ConnectedMaterial(*stage)->LibraryMaterial()));
        }

        formationBase->ConnectedMaterial(*stage)->LinkTo(*material);
        assert(formationBase->ConnectedMaterial(*stage)->IsLinkedTo(*material));
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Lists : reservoirs & depletions
//---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

TSurfaceL &
IT_Geomec_Interface::surfaces_list(int reservoir_id) // FIXME: need of 'TSurfaceL' and 'TStringL' -> verify this
{
  TSurfaceL &surfaces_v = *(new TSurfaceL());

  TStringL surfaces_list;

  CFormationBase *pFormation = formation(reservoir_id);
  CTetraFormation &t = static_cast<CTetraFormation &>(*pFormation);

  int num_volumes = t.VolumeSize();
  for (int i = 0; i < num_volumes; i++) {
    geo::CBodyGroup &bodys = t.Volume(i).Volume();

    int num_surfaces = bodys.SideSurfaceSize();
    for (int i = 0; i < num_surfaces; i++) {
      geo::CTetSurface &tet_surface = static_cast<geo::CTetSurface &>(bodys.SideSurface(i));
      surfaces_list.push_back(tet_surface.SurfaceDesc().Name()); // FIXME
    }
  }
  surfaces_v.elems({std::begin(surfaces_list), std::end(surfaces_list)});

  return surfaces_v;
}

TReservoirL &IT_Geomec_Interface::formations_list(bool is_depleting) {
  TReservoirL &formations_v = *(new TReservoirL());

  if (model()) {
    TStringL formations_list;

    TFormationBaseEntry::TNodeSet formations =
        static_cast<TFormationBaseEntry *>(model()->GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for (TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it) {
      CFormationBase &formation = dynamic_cast<CFormationBase &>(**it);
      if (is_depleting && formation.Depleting())
        formations_list.push_back(formation.Name().toStdString());
    }

    formations_v.elems({std::begin(formations_list), std::end(formations_list)});
  }

  return formations_v;
}

TDepletionL &IT_Geomec_Interface::depletions_list() {
  TDepletionL &depletions_v = *(new TDepletionL());

  if (model()) {
    TStringL depletions_list;

    CDepletionStage *stage = &model()->InitialDepletionStage();

    while (stage) {
      depletions_list.push_back(stage->Name().toStdString());
      stage = stage->Last() ? 0 : &stage->Next();
    }

    depletions_v.elems({std::begin(depletions_list), std::end(depletions_list)});
  }

  return depletions_v;
}

CStringV &IT_Geomec_Interface::list(eList type_, int param) {
  switch (type_) {
  case L_Surface:
    return surfaces_list(param);
  case L_Formation:
    return formations_list(false);
  case L_Reservoir:
    return formations_list(true);
  case L_Depletion:
    return depletions_list();
  case L_BD_Composite:
    return G_BD_composite_names_list((eComposite)param);
  default:
    return *(new CStringV());
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Depletion
//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int IT_Geomec_Interface::num_depletion_stages() {
  return model() ? model()->DepletionStageEntry().EntryNodes().size() : 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CDepletionStage *IT_Geomec_Interface::depletion_stage(int idx) {
  CDepletionStage *pStage = nullptr;
  if (model()) {

    if (idx == IT_Geomec_Interface::LAST_DEPLETION_STAGE)
      pStage = &model()->DepletionStageEntry().LastStage();
    else {
      pStage = &model()->InitialDepletionStage();

      int i = 0;
      while (i < idx) {
        if (!pStage->Last())
          pStage = &pStage->Next(); // perhaps give a warning, but it doesn't really matter
        i++;
      }
    }
  }
  assert(pStage);
  return pStage;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void IT_Geomec_Interface::branch(int depletion_id, bool is_branch) {
  CDepletionStage *pStage = IT_Geomec_Interface::depletion_stage(depletion_id);
  if (pStage)
    IT_Geomec_Interface::depletion_stage(depletion_id)
        ->setOutputType(is_branch ? CDepletionStage::BRANCH : CDepletionStage::RESULTS);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void IT_Geomec_Interface::add_depletion_stage(bool is_branch) {
  int id = num_depletion_stages();
  QString name = QString::asprintf(QString::fromStdString(DEFAULT_DEPLETION_STAGE_NAME).toLatin1().data(), id);
  CDepletionStage *pStage = depletion_stage(LAST_DEPLETION_STAGE);
  if (pStage) {
    new CDepletionStage(name, *pStage);
    branch(LAST_DEPLETION_STAGE, is_branch);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void IT_Geomec_Interface::remove_depletion_stage(int depletion_id) {
  if (depletion_id == ALL_DEPLETION_STAGES) {
    while (num_depletion_stages()) {
      CDepletionStage *pStage = depletion_stage(LAST_DEPLETION_STAGE);
      if (pStage->OutputType() == CDepletionStage::BRANCH)
        delete pStage;
      else
        break; // Supposition: (1)RESULTS (2)RESULTS (3)BRANCH BRANCH BRANCH BRANCH ... (n)BRANCH
    }
  } else {
    CDepletionStage *pStage = depletion_stage(depletion_id);
    delete pStage;
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------

int IT_Geomec_Interface::G_add_point_set(const IT_Data::TPointsV &points_v, eDepletionType _type, int depletion_id) {
  int ps_id = -1;
  if (points_v.size()) {
    PointSetManager *ps_mgr = PointSetManager::instance();
    int ps_id = ps_mgr->size() ? 0 : ps_mgr->create(_type, points_v, IPointSet::DIM_2D);
  }
  return ps_id;
}

int IT_Geomec_Interface::G_add_values_to_point_set(int ps_id, const IT_Data::TValuesV &pressures_v) {
  PointSetManager *ps_mgr = PointSetManager::instance();
  CPointSet *ps = ps_mgr->get(ps_id);

  if (pressures_v.size())
    ps_mgr->add_values(ps_id, pressures_v);

  return ps_mgr->points_size(ps_id);
}

bool IT_Geomec_Interface::G_link_point_set(int ps_id, int reservoir_id, int depletion_id) {
  bool res = false;

  CDepletionStage *pStage = depletion_stage(depletion_id); // FIXME
  CFormationBase *pFormation = formation(reservoir_id);
  if (pStage && pFormation) {
    CTreeCtrlBase::AllowPaint(false);

    if (doc()->CurrentScene())
      doc()->CurrentScene()->SetNonDrawing();

    PointSetManager *ps_mgr = PointSetManager::instance();
    res = ps_mgr->link(ps_id, pFormation, pStage);

    if (doc()->CurrentScene())
      doc()->CurrentScene()->SetDrawing();

    CTreeCtrlBase::AllowPaint(true);
  }
  return res;
}

void IT_Geomec_Interface::add_pressure(int reservoir_id, const IT_Data::TPointsV &points_v,
                                       const IT_Data::TValuesV &pressures_v, eDepletionType _type, int depletion_id,
                                       bool is_branch) {
  if (points_v.size() == 0 || (points_v.size() != pressures_v.size())) {
    if (points_v.size() == 0)
      globalMessage("Set property: received zero points; ignoring");
    else if (points_v.size() != pressures_v.size())
      globalMessage("Set property: different number of points and values; ignoring");
  } else {
    if (depletion_id == LAST_DEPLETION_STAGE)
      add_depletion_stage(is_branch);

    CDepletionStage *pStage = depletion_stage(depletion_id); // FIXME
    CFormationBase *pFormation = formation(reservoir_id);

    if (pStage && pFormation) {
      CTreeCtrlBase::AllowPaint(false);

      if (doc()->CurrentScene())
        doc()->CurrentScene()->SetNonDrawing();

      // TODO: verify this code refactoring
      PointSetManager *ps_mgr = PointSetManager::instance();
      int ps_id = ps_mgr->size() ? 0 : ps_mgr->create(_type, points_v, IPointSet::DIM_2D);
      bool res = ps_mgr->add_values(ps_id, pressures_v);
      bool res2 = ps_mgr->link(ps_id, pFormation, pStage);

      if (doc()->CurrentScene())
        doc()->CurrentScene()->SetDrawing();

      CTreeCtrlBase::AllowPaint(true);
    }
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void IT_Geomec_Interface::add_pressure(int reservoir_id, Depletion &depletion, eDepletionType _type, int depletion_id,
                                       bool is_branch) {
  add_pressure(reservoir_id, depletion.points(), depletion.pressure(), _type, depletion_id, is_branch);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Formations
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

CFormationBase *
IT_Geomec_Interface::formation(int idx,
                               bool bReservoir) // FIXME: 'formation' means 'reservoir' so do call it 'reservoir'
{
  if (model()) {
    int i = 0;
    TFormationBaseEntry::TNodeSet formations =
        static_cast<TFormationBaseEntry *>(model()->GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for (TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it) {
      CFormationBase *pFormation = dynamic_cast<CFormationBase *>(*it);

      if (!pFormation || (bReservoir && !pFormation->Depleting()))
        continue;

      if (i == idx)
        return pFormation;
      else
        i++;
    }
  }
  return nullptr;
}

int IT_Geomec_Interface::material_id(eMaterialParameter material) {
  switch (material) {
  case PoissonsRatio_:
    return IDT_VALUETYPE_POISSONS_RATIO;
  case YoungsModulus_:
    return IDT_VALUETYPE_YOUNGS_MODULUS;
  default:
    break;
  }
  return NOT_DEFINED;
}

IT_Geomec_Interface::eMaterialParameter IT_Geomec_Interface::convert(eReservoirProperty material) {
  switch (material) {
  case PoissonsRatio:
    return PoissonsRatio_;
  case YoungsModulus:
    return YoungsModulus_;
  default:
    break;
  }
  return NotDefined_;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- GENERIC
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool IT_Geomec_Interface::G_Filter(const geo::CPoint &point, CPointSet *ps) {
  return (!IN_HULL_FILTER || ps->PointInConvexHull(point));
}

IT_Data::TPointsV IT_Geomec_Interface::BD_points(CPointSet *ps) {
  IT_Data::TPointsV points;

  for (int i = 0; i < ps->PointSize(); i++) {
    const geo::IPoint &point = ps->PointAt(i);
    if (G_Filter(point, ps))
      points.push_back(IT_Data::TPoint{point.X(), point.Y(), point.Z()});
  }
  return points;
}

IT_Data::TPointsV IT_Geomec_Interface::BD_points(eComposite composite_, int composite_idx) {
  std::pair<IValueComposite *, CPointSet *> composite_p = G_BD_composite(composite_, composite_idx);
  IValueComposite *composite = composite_p.first;
  CPointSet *ps = composite_p.second;
  return BD_points(ps);
}

IT_Data::T3ValuesV IT_Geomec_Interface::BD_values(eComposite composite_, int composite_idx,
                                                  TDisplacementComponentV component_v) {
  IT_Data::T3ValuesV values_v;

  std::pair<IValueComposite *, CPointSet *> composite_p = G_BD_composite(composite_, composite_idx);
  IValueComposite *composite = composite_p.first;
  CPointSet *ps = composite_p.second;

  for (int i = 0; i < ps->PointSize(); i++) {
    const geo::IPoint &point = ps->PointAt(i);
    if (G_Filter(point, ps))
      values_v.push_back((BD_value(point, composite, component_v)));
  }
  return values_v;
}

IT_Data::T3Values IT_Geomec_Interface::BD_value(const geo::IPoint &point, IValueComposite *composite,
                                                TDisplacementComponentV &component_v) {
  IT_Data::T3Values values = {NaN, NaN, NaN};
  for (TDisplacementComponentV::iterator it = component_v.begin(); it != component_v.end(); ++it) {
    eComponent _type = (*it);
    geo::CValue c_value =
        composite->Component(_type).ScalarData().ValuePoint(point); // values per each point in the element

    double &val = _type == eComponent::X ? values.v1 : _type == eComponent::Y ? values.v2 : values.v3;
    val = c_value.Value();
  }
  return values;
}

CStringV &IT_Geomec_Interface::G_BD_composite_names_list(eComposite composite_) {
  CStringV &names_v = *(new CStringV());

  TStringL composites_list;

  TCompositeV composite_v = G_BD_composite_list(composite_).first;

  for (int i = 0; i < composite_v.size(); i++)
    composites_list.push_back(composite_v[i]->Name().toStdString());

  names_v.elems({std::begin(composites_list), std::end(composites_list)});
  return names_v;
}

std::pair<TCompositeV, TPointSetV> IT_Geomec_Interface::G_BD_composite_list(eComposite composite_) {
  TCompositeV composite_v;
  TPointSetV point_set_v;

  if (model()) {
    TPointSetEntry::TNodeSet point_set_set =
        static_cast<TPointSetEntry *>(model()->GraphEntry(MD_BASE_POINTSET))->EntryNodes();

    for (TPointSetEntry::TNodeSet::iterator it = point_set_set.begin(); it != point_set_set.end(); ++it) {
      CPointSet *point_set = dynamic_cast<CPointSet *>(*it);

      if (point_set) {
        // std::string point_set_name = point_set->Name().toStdString();
        int nComponents = point_set->NodalValueSetSize();
        CGraphNode *parent_already_added = nullptr;
        for (int i = 0; i < nComponents; i++) {
          CValueComponent *value_component = point_set->NodalValueSet(i).Component();
          if (value_component) {
            CGraphNode *parent = value_component->parent();
            if (value_component->parent()->TypeId() == G_composite_id(composite_)) {
              if (parent != parent_already_added) // if found for one component don't need to see the res, otherwise
                                                  // results are duplicated
              {
                composite_v.push_back(static_cast<IValueComposite *>(parent));
                point_set_v.push_back(point_set);
                parent_already_added = parent;
              }
            }
          }
        }
      }
    }
  }
  return std::make_pair(composite_v, point_set_v);
}

const IResultComponent *IT_Geomec_Interface::G_RES_component(eComposite composite_, CDepletionStage *pStage,
                                                             IVectorResult::VECTOR_COMPONENT _component) {
  const IResultComponent *pResultComponent = nullptr;
  const IResult *result = G_result(composite_, pStage);
  if (result) {
    CAnalysisType::TAnalysisType anType = model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN)
                                              ? CAnalysisType::AT_NONLIN
                                              : CAnalysisType::AT_LINEAR;
    pResultComponent = result->ResultComponent(*pStage, anType, 0, _component);
  }

  return pResultComponent;
}

const IResult *IT_Geomec_Interface::G_result(eComposite composite_, CDepletionStage *pStage) {
  const IResult *result = nullptr;
  if (model()) {
    CResultTree &tree = model()->ResultTree();
    switch (composite_) {
    case C_Displacement:
      result = &tree.Displacement();
      break;

      /*case C_TotalStress:				result = &tree.TotalStress();	break; //ResultTree.h
      case C_TotalStressChange:		result = &tree.TotalStressChange();		break; //ResultTree.h
      case C_TotalStrain:				result = &tree.TotalStrain();	break;
      case C_PlasticStrain:			result = &tree.PlasticStrain();	break;
      case C_EffectiveStress:			result = &tree.EffectiveStress();		break;
      case C_EffectiveStressChange:	result = &tree.EffectiveStressChange();	break; */

    case C_PorePressure:
      result = &tree.PorePressure();
      break; // 0: Absolute, 1: Change, 2: Gradient ????? -> giving the value '1' app crashes
    case C_Temperature:
      result = &tree.Temperature();
      break;
    case C_Consolidation:
      result = &tree.Consolidation();
      break;
    case C_Porosity:
      result = &tree.Porosity();
      break;

    // Faults
    case C_FaultEffectiveNormalStress:
      result = &tree.FaultEffectiveNormalStress();
      break;
    case C_FaultShearStress:
      result = &tree.FaultShearStress();
      break;
    case C_FaultDisplacement:
      result = &tree.FaultDisplacement();
      break;
    case C_FaultDilatation:
      result = &tree.FaultDilatation();
      break;
    case C_FaultPlasticSlip:
      result = &tree.FaultPlasticSlip();
      break;

    // case C_Aperture:				result = &tree.Aperture();			break;  // needs some params
    case C_ShearCapacity:
      result = &tree.ShearCapacity();
      break;
    // case C_DerivedResults:			result = &tree.DerivedResults();	break;
    case C_NotDefined:
      break;
    default:
      break;
    }
  }
  return result;
}

const IVectorResult::CVectorComponent *
IT_Geomec_Interface::G_RES_component_ONE(eComposite composite_, CDepletionStage *pStage,
                                         IVectorResult::VECTOR_COMPONENT _component) {
  const IVectorResult::CVectorComponent *component = nullptr;
  if (_component != IVectorResult::VECTOR_COMPONENT::VC_FULLVECTOR)
    component = dynamic_cast<const IVectorResult::CVectorComponent *>(G_RES_component(composite_, pStage, _component));
  return component;
}

const IVectorResult::CFullVectorComponent *IT_Geomec_Interface::G_RES_component_FULL(eComposite composite_,
                                                                                     CDepletionStage *pStage) {
  const IVectorResult::CFullVectorComponent *component = dynamic_cast<const IVectorResult::CFullVectorComponent *>(
      G_RES_component(composite_, pStage, IVectorResult::VECTOR_COMPONENT::VC_FULLVECTOR));
  return component;
}

int IT_Geomec_Interface::G_composite_id(eComposite composite_) {
  switch (composite_) {
  case C_Displacement:
    return IDT_VALUETYPE_DISPLACEMENT;
  // case C_PorePressure: break; // ????
  default:
    break;
  }
  return C_NotDefined;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Displacement
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

std::pair<IValueComposite *, CPointSet *> IT_Geomec_Interface::G_BD_composite(eComposite composite_id, int idx) {
  std::pair<TCompositeV, TPointSetV> list = G_BD_composite_list(composite_id);

  TCompositeV &composite_v = list.first;
  TPointSetV &point_set_v = list.second;

  return idx < composite_v.size() ? std::make_pair(composite_v[idx], point_set_v[idx])
                                  : std::make_pair(nullptr, nullptr);
}

bool IT_Geomec_Interface::results_available() { return model() ? model()->ResultRegister().ResultsAvailable() : false; }

IVectorResult::VECTOR_COMPONENT IT_Geomec_Interface::geomec_component_type(eComponent _type) {
  IVectorResult::VECTOR_COMPONENT type_ = _type == eComponent::X   ? IVectorResult::VECTOR_COMPONENT::VC_X
                                          : _type == eComponent::Y ? IVectorResult::VECTOR_COMPONENT::VC_Y
                                                                   : IVectorResult::VECTOR_COMPONENT::VC_Z;
  return type_;
}

std::string IT_Geomec_Interface::displacement_header(TDisplacementComponentV component_v) {
  int nComponents = component_v.size();
  std::string header = "x y z";
  for (int j = 0; j < nComponents; j++) {
    eComponent _type = component_v[j];
    if (_type == eComponent::X)
      header += " dx";
    else if (_type == eComponent::Y)
      header += " dy";
    else if (_type == eComponent::Z)
      header += " dz";
  }
  return header;
}

std::string IT_Geomec_Interface::reservoir_header(TReservoirPropertyV properties_v) {
  int nComponents = properties_v.size();
  std::string header = "";
  for (int j = 0; j < nComponents; j++) {
    eReservoirProperty _type = properties_v[j];
    if (_type == eReservoirProperty::Thickness)
      header += " dz";
    else if (_type == eReservoirProperty::PoissonsRatio)
      header += " pr";
    else if (_type == eReservoirProperty::YoungsModulus)
      header += " ym";
    else if (_type == eReservoirProperty::PorePressure)
      header += " pp";
  }
  return header;
}

//--------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------- NOT USED
//------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

/*CFormationBase* IT_Geomec_Interface::formation( const std::string formation_id )
{
  if( model() )
  {
    TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry* >( model()->GraphEntry(
MD_BASE_FORMATION ) )->EntryNodes(); for( TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it !=
formations.end(); ++it )
    {
      CFormationBase* _formation = dynamic_cast<CFormationBase*>(*it);
      if( _formation )
      {
        std::string name = _formation->Name().toStdString();

        if( name.compare( formation_id ) == 0 )
          return _formation;
      }
    }
  }
  return nullptr;
}

const geo::IElementSet& IT_Geomec_Interface::elements( const CFormationBase& formation ){	return
formation.ElementSet(0).ElementSet(); } const IPointSet& IT_Geomec_Interface::points( const CFormationBase& formation ){
return formation.ElementSet(0); }

TPointValueV IT_Geomec_Interface::pressure( int reservoir_id, int depletion_id )
{
  TPointValueV pressures_v;

  CFormationBase* pFormation = formation( reservoir_id );
  CDepletionStage* pStage = depletion_stage( depletion_id ); //FIXME
  const IResultComponent* component = depletion_component( pStage ); //FIXME

  if( pFormation && pStage && component )
  {
    geo::IElementSet& es = pFormation->ElementSet( 0 ).ElementSet();

    for(int e = 0; e < es.ElementSize(); e++)  //elems
    {
      const geo::IElement& element = es.Element( e );

      for( int p = 0; p < element.NrOfPoints(); p++ ) //point per elem
      {
        double value = component->ScalarData().ValuePoint( element.Point( p ) ).Value();
        int i = 0;

        geo::IPoint& point = (geo::IPoint&)element.Point( p );
        geo::CValue _value = component->ValuePoint( point );

        pressures_v.push_back( TPointValueP( point, _value ) );
      }
    }
  }
  return pressures_v;
}*/
