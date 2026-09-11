#include "StdAfx.h"
#include "DecimatingCellFilter.h"

#include <algorithm>

DecimatingCellFilterI::DecimatingCellFilterI()
  : m_fraction(1.0f)
  , m_maxVisibleIndex(0)
  , m_timestamp(0)
{
}

void DecimatingCellFilterI::init(size_t cellCount)
{
  // Only update when we need more elements
  if(cellCount > m_permutation.size())
  {
    std::vector<size_t> permutation;
    permutation.reserve(cellCount);

    for(size_t i=0; i < cellCount; ++i)
      permutation.push_back(i);

    std::random_shuffle(permutation.begin(), permutation.end());

    m_permutation.swap(permutation);

    m_maxVisibleIndex = (size_t)(m_fraction * m_permutation.size());
  }
}

void DecimatingCellFilterI::setVisibleFraction(float fraction)
{
  const float minFraction = 1e-3f;
  const float maxFraction = 1.0f;

  m_fraction = (fraction < minFraction) ? minFraction : ((fraction > maxFraction) ? maxFraction : fraction);
  m_maxVisibleIndex = (size_t)(fraction * m_permutation.size());

  m_timestamp++;
}

float DecimatingCellFilterI::visibleFraction() const
{
  return m_fraction;
}

bool DecimatingCellFilterI::acceptCell(size_t cellIndex) const
{
  size_t permutedIndex = m_permutation[cellIndex];
  return permutedIndex < m_maxVisibleIndex;
}

size_t DecimatingCellFilterI::getTimeStamp() const
{
  return m_timestamp;
}

