#pragma once

#include "IT_Data.h"
#include "PointSet.h"

class CValueTypeFactory;
class CModelBase;
class CFormationBase;

class PointSetManager {
  struct PS_Composite {
    int id;
    std::string name;
  };

  struct PS_Values {
    int composite_id_usr;
    int composite_id;
    CValueType *composite;
  };

  PS_Values EMPTY_VALUES = {-1, -1, nullptr};

  struct PS_Info {
    CPointSet *ps;
    std::vector<PS_Values> values_v;

    PS_Info(CPointSet *ps_ = nullptr) { ps = ps_; }
  };

  std::map<int, PS_Info> m_ps_m;

  CModelBase *m_model;
  const CValueTypeFactory *m_factory;

public:
  static const std::string DEFAULT_POINT_SET_NAME;
  static const std::string DEFAULT_PRESSURE_NAME;
  static const std::string DEFAULT_STRAIN_NAME;
  static const std::string DEFAULT_MATERIAL_NAME;
  static const std::string DEFAULT_TEMPERATURE_NAME;

  static const int LAST_VALUES_ID;

private:
  PointSetManager();
  PointSetManager::PS_Composite PointSetManager::info(int _type); // IT_Geomec_Interface::eDepletionType
  PointSetManager::PS_Info *get_ext(int id);

public:
  static PointSetManager *instance();

  bool create(int id /*IT_Geomec_Interface::eDepletionType*/, const IT_Data::TPointsV &points_v,
              IPointSet::DIMENSION dim = IPointSet::DIM_2D, std::string name = std::string());
  CPointSet *get(int id);
  bool add_values(int id, int _type, const IT_Data::TValuesV &values_v);
  bool replace_values(int id, const IT_Data::TValuesV &values_v, int values_idx = -1);

  bool remove(int id);
  // values_id == -1 means last
  bool remove_vals(int ps_id, int values_id);

  bool link(int ps_id, int values_id, CFormationBase *pFormation, CDepletionStage *pStage); // add to the data tree
  bool unlink(int ps_id, int values_id, CFormationBase *pFormation,
              CDepletionStage *pStage); // remove from the data tree

  CModelBase *model() { return m_model; };
  void model(CModelBase *val) { m_model = val; };

  // number of pointsets
  int size() { return m_ps_m.size(); };

  // given a pointset its number of values (composites)
  int sets_size(int id);

  // number of points of a given pointset
  int points_size(int id);

  bool exist(int id);
  bool exist_val(int ps_id, int value_id);

  PS_Values values(int ps_id, int value_id);

  std::vector<int> composite_list(int ps_id);

  std::vector<double> composite_values(int ps_id, int composite_idx);

  void link_ps_to_model(int ps_id);
  bool link_dep_to_ps_composite(int ps_id, int values_id, CFormationBase *pFormation, CDepletionStage *pStage);

  bool unlink_dep_from_ps_composite(int ps_id, int values_id, CFormationBase *pFormation, CDepletionStage *pStage);
};
