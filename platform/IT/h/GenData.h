#pragma once

#include "Data.h" // data<T>()
#include "IT_Data.h"

template <typename T, typename K, DataType type_, int size> class GenData : public IT_Data {
  T m_values;

public:
  GenData(T val) : IT_Data() { values(val); }
  GenData(T val, IT_Data::TIntV params) : IT_Data() {
    values(val);
    m_params = params;
  }
  void values(T val) { m_values = val; };
  T values() { return m_values; };
  TData values_d() { return data<K>(m_values, size, type_); };
  TData object(int i) { return values_d(); };
  virtual int num_objects() { return 1; };
};

// double
class GenData_3d : public GenData<IT_Data::TPointsV, IT_Data::TPoint, DataType::DoubleV, 3> {
public:
  GenData_3d(IT_Data::TPointsV points)
      : GenData<IT_Data::TPointsV, IT_Data::TPoint, DataType::DoubleV, 3>(points) {}; // GM -> IT
  GenData_3d(IT_Data::TPointsV points, IT_Data::TIntV params)
      : GenData<IT_Data::TPointsV, IT_Data::TPoint, DataType::DoubleV, 3>(points, params) {}; // GM <- IT
};

class GenData_1d : public GenData<IT_Data::TValuesV, double, DataType::DoubleV, 1> {
public:
  GenData_1d(IT_Data::TValuesV values)
      : GenData<IT_Data::TValuesV, double, DataType::DoubleV, 1>(values) {}; // GM -> IT
  GenData_1d(IT_Data::TValuesV values, IT_Data::TIntV params)
      : GenData<IT_Data::TValuesV, double, DataType::DoubleV, 1>(values, params) {}; // GM <- IT
};

// integer
class GenData_1i : public GenData<IT_Data::TIntV, int, DataType::IntegerV, 1> {
public:
  GenData_1i(IT_Data::TIntV values) : GenData<IT_Data::TIntV, int, DataType::IntegerV, 1>(values) {};
};

class GenData_3i : public GenData<IT_Data::TTriV, IT_Data::TTri, DataType::IntegerV, 3> {
public:
  GenData_3i(IT_Data::TTriV values) : GenData<IT_Data::TTriV, IT_Data::TTri, DataType::IntegerV, 3>(values) {};
};

// string
/*class GenData_1s : public GenData<IT_Data::TIntV, int, DataType::IntegerV, 1>
{
public:
  GenData_1s( IT_Data::TIntV values ) : GenData<IT_Data::TIntV, int, DataType::IntegerV, 1>( values ){};
};*/
