/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_ARRAY_TEMP_H__4B0061C1_A092_44EF_A669_116200BF6D1B__INCLUDED_)
#define AFX_ARRAY_TEMP_H__4B0061C1_A092_44EF_A669_116200BF6D1B__INCLUDED_

#include <cassert>
#include <vector>

#include "FieldValueFileExports.h"

template <class T> class array_temp {
  std::vector<std::vector<std::vector<T>>> m_data;

public:
  array_temp();
  array_temp(int x, int y, int z);
  array_temp(const array_temp<T> &rhs);
  void resize(int x, int y, int z);
  void set_at(int x, int y, int z, const T &type);
  T &get_at(int x, int y, int z);
  const T &get_at(int x, int y, int z) const;
  int size_x() const;
  int size_y() const;
  int size_z() const;
};

template <class T> array_temp<T>::array_temp() {}

template <class T> array_temp<T>::array_temp(int x, int y, int z) { resize(x, y, z); }

template <class T> array_temp<T>::array_temp(const array_temp<T> &rhs) : m_data(rhs.m_data) {}

template <class T> void array_temp<T>::resize(int x, int y, int z) {
  assert((x > 0) && (y > 0) && (z > 0));

  m_data.resize(x);
  for (size_t n_x = 0; n_x < m_data.size(); n_x++) {
    m_data[n_x].resize(y);
    for (size_t n_y = 0; n_y < m_data[n_x].size(); n_y++)
      m_data[n_x][n_y].resize(z);
  }
}

template <class T> T &array_temp<T>::get_at(int x, int y, int z) { return m_data.at(x).at(y).at(z); }

template <class T> const T &array_temp<T>::get_at(int x, int y, int z) const { return m_data[x][y][z]; }

template <class T> void array_temp<T>::set_at(const int x, const int y, const int z, const T &type) {
  m_data.at(x).at(y).at(z) = type;
}

template <class T> int array_temp<T>::size_x() const { return (int)m_data.size(); }

template <class T> int array_temp<T>::size_y() const { return (int)m_data[0].size(); }

template <class T> int array_temp<T>::size_z() const { return (int)m_data[0][0].size(); }

#endif // !defined(AFX_ARRAY_TEMP_H__4B0061C1_A092_44EF_A669_116200BF6D1B__INCLUDED_)
