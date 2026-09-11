 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IValue.cpp: implementation of the IValue class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "Value.h"

// for EPS
#include "IObject.h"

#include <cmath>

namespace geo {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IValue::IValue()
{
}

IValue::~IValue()
{
}

const IValue &IValue::Min(const IValue& rhs) const
{
  if(Valid() && rhs.Valid())
  {
    if(Value() < rhs.Value())
      return *this;
    else
      return rhs;
  }

  if(Valid())
    return *this;

  return rhs;
}

const IValue &IValue::Max(const IValue& rhs) const
{
  if(Valid() && rhs.Valid())
  {
    if(Value() > rhs.Value())
      return *this;
    else
      return rhs;
  }

  if(Valid())
    return *this;

  return rhs;
}

bool IValue::operator==(const IValue& rhs) const
{
  if(Valid() != rhs.Valid())
    return false;

  if(!Valid()) return true; // both not valid is true (?)

  return (Value() == rhs.Value());
}

bool IValue::operator==(const double& rhs) const
{
  if(!Valid()) return false;

  return Value() == rhs;
}

CValue IValue::operator+(const IValue &rhs) const
{
  assert(Valid());
  assert(rhs.Valid());

  return CValue(Value() + rhs.Value());
}

CValue IValue::operator+(const double &rhs) const
{
  assert(Valid());

  return CValue(Value() + rhs);
}

IValue &IValue::operator+=(const IValue &rhs)
{
  assert(Valid());
  assert(rhs.Valid());

  Value(Value() + rhs.Value());

  return *this;
}

IValue &IValue::operator+=(const double &rhs)
{
  assert(Valid());

  Value(Value() + rhs);

  return *this;
}

CValue IValue::operator-(const IValue &rhs) const
{
  assert(Valid());
  assert(rhs.Valid());

  return CValue(Value() - rhs.Value());
}

CValue IValue::operator-(const double &rhs) const
{
  assert(Valid());

  return CValue(Value() - rhs);
}

IValue &IValue::operator-=(const IValue &rhs)
{
  assert(Valid());
  assert(rhs.Valid());

  Value(Value() - rhs.Value());

  return *this;
}

IValue &IValue::operator-=(const double &rhs)
{
  assert(Valid());

  Value(Value() - rhs);

  return *this;
}

CValue IValue::operator/(const IValue &rhs) const
{
  assert(Valid());
  assert(rhs.Valid());
  assert(fabs(rhs.Value()) > EPS);

  return CValue(Value() / rhs.Value());
}

CValue IValue::operator/(const double &rhs) const
{
  assert(Valid());
  assert(fabs(rhs) > EPS);

  return CValue(Value() / rhs);
}

IValue &IValue::operator/=(const IValue &rhs)
{
  assert(Valid());
  assert(rhs.Valid());
  assert(fabs(rhs.Value()) > EPS);

  Value(Value() / rhs.Value());

  return *this;
}

IValue &IValue::operator/=(const double &rhs)
{
  assert(Valid());
  assert(fabs(rhs) > EPS);

  Value(Value() / rhs);

  return *this;
}

CValue IValue::operator*(const IValue &rhs) const
{
  assert(Valid());
  assert(rhs.Valid());

  return CValue(Value() * rhs.Value());
}

CValue IValue::operator*(const double &rhs) const
{
  assert(Valid());

  return CValue(Value() * rhs);
}

IValue &IValue::operator*=(const IValue &rhs)
{
  assert(Valid());
  assert(rhs.Valid());

  Value(Value() * rhs.Value());

  return *this;
}

IValue &IValue::operator*=(const double &rhs)
{
  assert(Valid());

  Value(Value() * rhs);

  return *this;
}
bool IValue::operator<(const IValue& rhs) const
{
  if(Valid())
  {
    if(rhs.Valid())
      return Value() < rhs.Value();
    return false;
  }
  
  if(rhs.Valid())
    return true;

  return false;
}

}
