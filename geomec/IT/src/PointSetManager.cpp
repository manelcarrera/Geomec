#include "stdafx.h"

#include "PointSetManager.h"
// #include "IT_Geomec_Interface.h"
#include "DepletionStage.h"
#include "FormationBase.h"
#include "IGm.h"

#include "NodalValueSet.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "Temperature.h"
#include "ValueTypeFactory.h"

#include "Printer.h"

const std::string PointSetManager::DEFAULT_POINT_SET_NAME = "Point set IT";
// const std::string PointSetManager::DEFAULT_DEPLETION_STAGE_NAME	= "Depletion Stage %d";
const std::string PointSetManager::DEFAULT_PRESSURE_NAME = "Pressure IT";
const std::string PointSetManager::DEFAULT_STRAIN_NAME = "Strain IT";
const std::string PointSetManager::DEFAULT_MATERIAL_NAME = "Material IT";
const std::string PointSetManager::DEFAULT_TEMPERATURE_NAME = "Temperature IT";

const int PointSetManager::LAST_VALUES_ID = -1;

namespace {
static const int COORDINATES_LENTGH = 2; // FIXME: HC
}

PointSetManager::PointSetManager() {
  m_factory = CValueTypeFactory::instance();
  m_model = gm::model::get();
}

PointSetManager *PointSetManager::instance() {
  static PointSetManager _instance;
  return &_instance;
}

bool PointSetManager::exist(int id) { return (m_ps_m.find(id) != m_ps_m.end()); }
bool PointSetManager::exist_val(int ps_id, int value_id) {
  PS_Info *info = get_ext(ps_id);
  if (info && (value_id < info->values_v.size()))
    return true;
  return false;
}

PointSetManager::PS_Values PointSetManager::values(int ps_id, int value_id) {
  PS_Values vals = EMPTY_VALUES;
  if (exist_val(ps_id, value_id)) {
    PS_Info *info = get_ext(ps_id);
    vals = info->values_v[value_id];
  }
  return vals;
  // return  EMPTY_VALUES;
}

std::vector<double> PointSetManager::composite_values(int ps_id, int composite_idx) {
  std::vector<double> values = {95.95, 96.96}; // FIXME
  /*if( exist_val( ps_id, composite_idx ) )
  {
    CValueType* composite = get_ext( ps_id )->values_v[ composite_idx ].composite;
    //composite->
  }*/
  return values;
}

std::vector<int> PointSetManager::composite_list(int ps_id) {
  std::vector<int> composites;

  PS_Info *info = get_ext(ps_id);
  if (info) {
    int size = info->values_v.size();
    for (int i = 0; i < size; i++)
      composites.push_back(info->values_v[i].composite_id_usr);
  }
  return composites;
}

bool PointSetManager::create(int id, const IT_Data::TPointsV &points_v, IPointSet::DIMENSION dim,
                             std::string name) // int > IT_Geomec_Interface::eDepletionType
{
  Printer::instance()->debug("[PS] create : ps: %d", id);

  bool res = false;
  if (m_model) {
    if (!exist(id)) {
      if (points_v.size()) {
        CPointSet *ps = new CPointSet(QString::fromStdString(name.empty() ? DEFAULT_POINT_SET_NAME : name), *m_model,
                                      0 /*size*/, dim); // name = "Inversion Tool Pressure"
        m_ps_m[id] = PS_Info{ps};

        link_ps_to_model(id);

        for (int p = 0; p < points_v.size(); p++) // points / pressures loop
          ps->PushBack(std::vector<double>{points_v[p].x, points_v[p].y});

        res = true;
      }
    }
  }
  return res;
}

bool PointSetManager::remove(int id) // FIXME: Just started: many things need to by considered
{
  bool res = false;
  if (exist(id)) {
    PointSetManager::PS_Info *info = get_ext(id);
    if (info->ps)
      delete info->ps;

    for (int i = 0; i < info->values_v.size(); i++)
      if (info->values_v[i].composite)
        delete info->values_v[i].composite;

    info->values_v.clear();
    res = true;
  }
  return res;
}

bool PointSetManager::remove_vals(int ps_id, int values_id) // FIXME: Just started: many things need to by considered
{
  bool res = false;

  values_id = values_id == -1 ? sets_size(ps_id) - 1 - COORDINATES_LENTGH : values_id;

  int idx = values_id + COORDINATES_LENTGH;

  if (exist_val(ps_id, values_id)) {
    CPointSet *ps = get(ps_id);
    res = ps->NodalValueSet(idx).Destroy(); // TODO: JH ????

    PS_Info *info = get_ext(ps_id);

    // do this in the 'PS_Values' destructor?
    info->values_v[values_id].composite->Destroy();
    // CRASH:
    // delete info->values_v[ values_id ].composite;

    info->values_v.erase(info->values_v.begin() + values_id);
  }
  return res;
}

CPointSet *PointSetManager::get(int id) {
  CPointSet *ps = nullptr;
  if (exist(id))
    ps = m_ps_m[id].ps;
  return ps;
}

PointSetManager::PS_Info *PointSetManager::get_ext(int id) {
  PS_Info *ps_ext = nullptr;
  if (exist(id))
    ps_ext = &m_ps_m[id];
  return ps_ext;
}

int PointSetManager::sets_size(int id) {
  int size = -1;
  CPointSet *ps = get(id);
  if (ps)
    size = ps->NodalValueSetSize();
  return size;
}

int PointSetManager::points_size(int id) {
  int size = -1;
  CPointSet *ps = get(id);
  if (ps)
    size = ps->PointSize();
  return size;
}

bool PointSetManager::add_values(int id, int _type, const IT_Data::TValuesV &values_v) {
  bool res = false;
  CPointSet *ps = get(id);
  if (ps) {
    int idx = ps->AddNodalValueSet();

    Printer::instance()->debug("[PS] add : ps: %d col: %d", id, idx - COORDINATES_LENTGH);

    PS_Composite composite_info = info(_type);

    CValueType *composite =
        m_factory->BuildValueType(*ps, composite_info.id, QString::fromStdString(composite_info.name));

    PS_Info *info = get_ext(id);
    info->values_v.push_back(PS_Values{_type, composite_info.id, composite});

    // int size = ps->NodalValueSetSize();
    //  idx: x y z pressure1 pressure2 ... -> so '3' is the pressure
    //  a second pressure values will need an extra column
    ps->NodalValueSet(idx).LinkTo(composite->Component());

    // int size = ps->NodalValueSetSize();
    for (int p = 0; p < values_v.size(); p++) // points / pressures loop
      ps->NodalValueSet(idx).Value(p, values_v[p]);

    res = true; // TODO: verify length
  }
  return res;
}

bool PointSetManager::replace_values(int id, const IT_Data::TValuesV &values_v, int values_idx) {
  bool res = false;
  CPointSet *ps = get(id);
  if (ps) {
    int size = ps->NodalValueSetSize();
    int idx = values_idx == -1 ? size - 1 : values_idx + COORDINATES_LENTGH;

    Printer::instance()->debug("[PS] replace : ps: %d values_id: %d", id, values_idx);

    if (idx < size && idx >= COORDINATES_LENTGH /* cols: x,y,v1,v2...vn */) // FIXME: only valid for 'IPointSet::DIM_2D'
    {
      for (int p = 0; p < values_v.size(); p++) // points / pressures loop
        ps->NodalValueSet(idx).Value(p, values_v[p]);
      res = true;
    }
  }
  return res;
}

PointSetManager::PS_Composite PointSetManager::info(int _type) // IT_Geomec_Interface::eDepletionType
{
  PS_Composite comp;
  switch ((gm::eDepletionType)_type) {
  case gm::eDepletionType::D_Strain:
    comp = PS_Composite{IDT_VALUETYPE_VOLUMETRICSTRAIN, DEFAULT_STRAIN_NAME};
    break;
  case gm::eDepletionType::D_Compressibility:
    comp = PS_Composite{IDT_VALUETYPE_ELASCOMPRES, DEFAULT_MATERIAL_NAME};
    break;
  case gm::eDepletionType::D_Pore_Pressure:
    comp = PS_Composite{IDT_VALUETYPE_PRESSURE, DEFAULT_PRESSURE_NAME};
    break;
  case gm::eDepletionType::D_Temperature:
    comp = PS_Composite{IDT_VALUETYPE_TEMPERATURE, DEFAULT_TEMPERATURE_NAME};
    break;
  }
  return comp;
}

bool PointSetManager::link(int ps_id, int values_id, CFormationBase *pFormation, CDepletionStage *pStage) {
  // link_ps_to_model( ps_id );
  bool res = link_dep_to_ps_composite(ps_id, values_id, pFormation, pStage);
  return res;
}

bool PointSetManager::link_dep_to_ps_composite(int ps_id, int values_id, CFormationBase *pFormation,
                                               CDepletionStage *pStage) {
  bool res = false;

  if (pStage && pFormation) {
    PS_Info *info = get_ext(ps_id);
    if (info) {
      values_id = values_id == LAST_VALUES_ID ? (sets_size(ps_id) - 1 - COORDINATES_LENTGH) : values_id;

      if (exist_val(ps_id, values_id)) // Take care with this
      {
        PS_Values values = info->values_v[values_id];

        switch ((gm::eDepletionType)values.composite_id_usr) {
        case gm::eDepletionType::D_Strain:
          res = pFormation->Strain(*pStage).ConnectItem(*values.composite);
          break;
        case gm::eDepletionType::D_Compressibility:
          res = pFormation->Material(*pStage).ConnectItem(*values.composite);
          break;
        case gm::eDepletionType::D_Pore_Pressure:
          res = pFormation->Pressure(*pStage).ConnectItem(*values.composite);
          break;
        case gm::eDepletionType::D_Temperature:
          res = pFormation->UserTemperature(*pStage).ConnectItem(*values.composite);
          break;
        }
      }
    }
  }
  Printer::instance()->debug("[PS] link : ps: %d values_id: %d -> res: %d", ps_id, values_id, res);
  return res;
}

void PointSetManager::link_ps_to_model(int ps_id) {
  CGraphEntry &point_set_entry =
      *m_model->GraphEntry(MD_BASE_POINTSET); // this is also to retrieve point set, loop and compare with the name
  CPointSet *ps = get(ps_id);
  ps->LinkTo(point_set_entry);
  Printer::instance()->debug("[PS] link_ps_to_model : ps_id: %d ", ps_id);
}

bool PointSetManager::unlink(int ps_id, int values_id, CFormationBase *pFormation,
                             CDepletionStage *pStage) // Jeroen ? (1) unlink (2) wich order?
{
  bool res = unlink_dep_from_ps_composite(ps_id, values_id, pFormation, pStage);
  return res;
}

bool PointSetManager::unlink_dep_from_ps_composite(int ps_id, int values_id, CFormationBase *pFormation,
                                                   CDepletionStage *pStage) // Jeroen ? (1) unlink (2) wich order?
{
  bool res = false;

  if (pStage && pFormation) {
    PS_Info *info = get_ext(ps_id);
    if (info) {
      values_id = values_id == LAST_VALUES_ID ? (sets_size(ps_id) - 1 - COORDINATES_LENTGH) : values_id;

      Printer::instance()->debug("unlink_dep_from_ps_composite : ps: %d values_id: %d", ps_id, values_id);

      if (exist_val(ps_id, values_id)) // Take care with this
      {
        PS_Values values = info->values_v[values_id];

        switch ((gm::eDepletionType)values.composite_id_usr) {
        case gm::eDepletionType::D_Strain:
          pFormation->Strain(*pStage).UnLink(*values.composite);
          break;
        case gm::eDepletionType::D_Compressibility:
          pFormation->Material(*pStage).UnLink(*values.composite);
          break;
        case gm::eDepletionType::D_Pore_Pressure:
          pFormation->Pressure(*pStage).UnLink(*values.composite);
          break;
        case gm::eDepletionType::D_Temperature:
          pFormation->UserTemperature(*pStage).UnLink(*values.composite);
          break;
        }
        res = true;
      }
    }
  }
  return res;
}