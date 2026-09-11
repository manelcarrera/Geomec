 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _COORDINATEMAP_H_
#define _COORDINATEMAP_H_

#include "Octree.h"
#include <utility>
#include <functional>

#include "GeometryExports.h"

namespace geo {

/*!

The CCoordinateMap class template is a specialization of the COctree class template.
It mimics the interace of the std::map template to make it easy to replace any invalid
usage of the std::map with a non-strict weak ordering less operator (which is the case
when using tolerace in the sorting algorithm).

*/
template <class COORD, class VALUE, class EPSILON = CEpsilon>
class CCoordinateMap : public COctree<COORD, std::pair<COORD, VALUE>, EPSILON>
{
public:
  typedef std::pair<COORD, VALUE> value_type;
  typedef COORD key_type;
  typedef VALUE mapped_type;
  typedef COctree<COORD, std::pair<COORD, VALUE>, EPSILON> _Mybase;
  typedef typename _Mybase::iterator iterator;
  typedef typename _Mybase::const_iterator const_iterator;
  typedef typename _Mybase::size_type size_type;

public:
  CCoordinateMap()
  {
  }

  CCoordinateMap(const CCoordinateMap& rhs)
  : _Mybase(rhs)
  {
  }

  virtual const key_type& key(const value_type& val) const
  {
  return val.first;
  }

  mapped_type& operator[](const key_type& key)
  {
  iterator it = this->find(key);
  if(it == COctree <COORD, std::pair <COORD, VALUE>, EPSILON> ::end())
      it = this->insert(value_type(key, mapped_type())).first;

  return it->second;
  }
};

} // namespace geo

#endif // _COORDINATEMAP_H_
