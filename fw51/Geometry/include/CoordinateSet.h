 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _COORDINATESET_H_
#define _COORDINATESET_H_

#include "Octree.h"
#include <functional>

#include "GeometryExports.h"

namespace geo {

/*!

The CCoordinateSet class template is a specialization of the COctree class template.
It mimics the interace of the std::set template to make it easy to replace any invalid
usage of the std::set with a non-strict weak ordering less operator (which is the case
when using tolerace in the sorting algorithm).

*/
template <class COORD, class EPSILON = CEpsilon>
class CCoordinateSet : public COctree<COORD, COORD, EPSILON>
{
public:
  typedef COORD value_type;
  typedef COORD key_type;
  typedef COctree<COORD, COORD, EPSILON> _Mybase;
  typedef typename _Mybase::iterator iterator;
  typedef typename _Mybase::const_iterator const_iterator;
  typedef typename _Mybase::size_type size_type;

public:
  CCoordinateSet()
  {
  }

  CCoordinateSet(const CCoordinateSet& rhs)
  : _Mybase(rhs)
  {
  }

  virtual const key_type& key(const value_type& val) const
  {
    return val;
  }
};

} // namespace geo

#endif // _COORDINATESET_H_
