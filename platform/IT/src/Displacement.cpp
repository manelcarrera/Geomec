#include "Displacement.h"

const int Displacement::DEF_WDAT = 4000;
Displacement::Displacement() : IT_Data(), m_wdat(DEF_WDAT) {}

TData Displacement::object(int _type) {
  switch (_type) {
  case Z:
    return z_d();
  case Elems:
    return elems_d();
  case Subsidence:
    return subsidence_d();
  case Wdat:
    return wdat_d();
  default:
    return TData_INVALID;
  }
}

TData Displacement::z_d() { return data<double>(m_z, IT_Data::Cols_Coordinate); }
TData Displacement::elems_d() { return data<int>(m_elems, IT_Data::Cols_Elems, DataType::IntegerV); }
TData Displacement::dx_d() { return data<double>(m_dx, IT_Data::Cols_Values); }
TData Displacement::dy_d() { return data<double>(m_dy, IT_Data::Cols_Values); }
TData Displacement::dz_d() { return data<double>(m_dz, IT_Data::Cols_Values); }
TData Displacement::subsidence_d() { return data<T3Values>(m_subsidence, IT_Data::Cols_Subsidence); }

TData Displacement::wdat_d() { return TData{1, IT_Data::Cols_Values, (TMemoryChunkPtr)&m_wdat, DataType::Double}; }
