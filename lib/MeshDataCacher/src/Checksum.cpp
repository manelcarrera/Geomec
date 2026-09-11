#include "Checksum.h"
#include <cassert>
#include <cstring>

namespace mdc {

CChecksum::CChecksum(int eltSize, int pivot, int maxEltNodes, int maxIfNodes)
    : size(sizeof(CChecksum)), type(2), seed(0, 0), hash(0, 0), eltSize(eltSize), pivot(pivot),
      maxEltNodes(maxEltNodes), maxIfNodes(maxIfNodes) {}

void CChecksum::reset() {
  assert(size == sizeof(CChecksum));
  assert(type == 2);
  memset(&seed, 0, sizeof(seed) + sizeof(hash));
}

} // namespace mdc