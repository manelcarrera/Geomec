/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _LOKI_H_
#define _LOKI_H_

#include "GeometryExports.h"

// template classes taken from the Loki library
// see 'Modern C++ design' by Andrei Alexandrescu
// with some modifications/additions
namespace Loki {

// NullType means 'nothing', invalid
class NullType {};

// template to figure out whether a type is a pointer or not
template <typename T> class TypeTraits {
private:
  template <class U> struct PointerTraits {
    enum { result = false };
    typedef NullType PointeeType;
    typedef U ReferencedType;
    static const ReferencedType &GetReference(const U &u) { return u; }
  };
  template <class U> struct PointerTraits<U *> {
    enum { result = true };
    typedef U PointeeType;
    typedef U ReferencedType;
    static const ReferencedType &GetReference(const U *u) { return *u; }
  };

public:
  typedef typename PointerTraits<T>::PointeeType PointeeType;
  typedef typename PointerTraits<T>::ReferencedType ReferencedType;
  static const ReferencedType &GetReference(const T &t) { return PointerTraits<T>::GetReference(t); }
};

} // namespace Loki

#endif // _LOKI_H_
