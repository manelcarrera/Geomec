#include "Reservoir.h"
#include "Data.h"

Reservoir::Reservoir(eElemType val) : IT_Data(), m_elem_type(val) {}

TData Reservoir::points_d() { return data<TPoint>(m_points, Cols_Point); }
TData Reservoir::tri_d() { return data<TTri>(m_tri, Cols_Elems_Tri, DataType::IntegerV); }
TData Reservoir::tetra_d() { return data<TTetra>(m_tetra, Cols_Elems_Tetra, DataType::IntegerV); }
TData Reservoir::params_d() { return data<T4Values>(m_params, Cols_Params); }

TData Reservoir::object(int _type) {
  switch (_type) {
  case Points:
    return points_d();
  case Elems:
    return m_elem_type == Tri ? tri_d() : tetra_d();
  case Params:
    return params_d();
  default:
    return TData_INVALID;
  }
}
