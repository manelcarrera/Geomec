#ifndef DECIMATINGCELLFILTER_H
#define DECIMATINGCELLFILTER_H

#include <MeshVizXLM/mesh/cell/MiCellFilterI.h>

#include <vector>

class DecimatingCellFilterI : public MiCellFilterI {
  std::vector<size_t> m_permutation;

  float m_fraction;
  size_t m_maxVisibleIndex;
  size_t m_timestamp;

public:
  DecimatingCellFilterI();

  void init(size_t cellCount);

  void setVisibleFraction(float frac);

  float visibleFraction() const;

  bool acceptCell(size_t cellIndex) const;

  size_t getTimeStamp() const;
};

#endif
