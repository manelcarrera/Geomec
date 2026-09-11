/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IValue.h: interface for the IValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVALUE_H__45603E69_BDF5_4839_83C5_9E5579D19854__INCLUDED_)
#define AFX_IVALUE_H__45603E69_BDF5_4839_83C5_9E5579D19854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dimple.h"

#include <string>
#include <vector>

#include "GeometryExports.h"

namespace geo {

class CValue;

class GEOMETRY_EXPORT IValue {
public:
  IValue();
  virtual ~IValue();

  virtual bool Valid() const = 0;
  virtual void Invalidate() = 0;
  virtual const double &Value() const = 0;
  virtual void Value(const double &value) = 0;

  const IValue &Min(const IValue &rhs) const;
  const IValue &Max(const IValue &rhs) const;

  bool operator==(const IValue &rhs) const;
  bool operator==(const double &rhs) const;

  CValue operator+(const IValue &rhs) const;
  CValue operator+(const double &rhs) const;
  IValue &operator+=(const IValue &rhs);
  IValue &operator+=(const double &rhs);
  CValue operator-(const IValue &rhs) const;
  CValue operator-(const double &rhs) const;
  IValue &operator-=(const IValue &rhs);
  IValue &operator-=(const double &rhs);
  CValue operator/(const IValue &rhs) const;
  CValue operator/(const double &rhs) const;
  IValue &operator/=(const IValue &rhs);
  IValue &operator/=(const double &rhs);
  CValue operator*(const IValue &rhs) const;
  CValue operator*(const double &rhs) const;
  IValue &operator*=(const IValue &rhs);
  IValue &operator*=(const double &rhs);

  virtual std::string Representation() const = 0;

  bool operator<(const IValue &rhs) const;
};

} // namespace geo

#endif // !defined(AFX_IVALUE_H__45603E69_BDF5_4839_83C5_9E5579D19854__INCLUDED_)
