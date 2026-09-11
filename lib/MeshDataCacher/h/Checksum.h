#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

#include "City.h"

namespace mdc {

struct CChecksum {
  unsigned int size; // size and type allow us to later change this, at which point Checksum must always have room for
                     // the largest type, and keep supporting older checksums
  unsigned int type;
  uint128 seed;
  uint128 hash;

  int eltSize;
  int pivot;
  int maxEltNodes;
  int maxIfNodes;

  CChecksum(int eltSize, int pivot, int maxEltNodes, int maxIfNodes);
  void reset();
};

} // namespace mdc

#endif