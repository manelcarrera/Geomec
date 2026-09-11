#include "stdafx.h"

#include "ColorManager.h"

const int ColorManager::s_availableHues[s_numUniqueColors] = {0,  225, 90,  315, 180, 45,  270, 135,
                                                              25, 335, 205, 70,  290, 160, 245, 115};

const int ColorManager::s_availableSaturations[s_numUniqueColors] = {255, 255, 255, 255, 255, 255, 255, 255,
                                                                     128, 128, 128, 128, 128, 128, 128, 128};

ColorManager::ColorManager() : m_lastColorIndex(-1) {}

QColor ColorManager::MakeCurveColor(int n) {
  int colorIndex = n % s_numUniqueColors;
  int hue = s_availableHues[colorIndex];
  int s = s_availableSaturations[colorIndex];
  return QColor::fromHsv(hue, s, 255);
}

QColor ColorManager::MakeMarkerColor(int n) {
  int colorIndex = n % s_numUniqueColors;
  int hue = s_availableHues[colorIndex];
  int s = s_availableSaturations[colorIndex];
  return QColor::fromHsv(hue, s, 128);
}

int ColorManager::allocateColorIndex() {
  for (int i = 1; i <= s_numUniqueColors; ++i) {
    int colorIndex = (m_lastColorIndex + i) % s_numUniqueColors;
    if (std::find(m_usedColorIndices.begin(), m_usedColorIndices.end(), colorIndex) == m_usedColorIndices.end()) {
      m_usedColorIndices.push_back(colorIndex);
      m_lastColorIndex = colorIndex;
      return colorIndex;
    }
  }
  return 0;
}

void ColorManager::releaseColorIndex(int index) {
  decltype(m_usedColorIndices.begin()) colorIndexIter;
  if ((colorIndexIter = std::find(m_usedColorIndices.begin(), m_usedColorIndices.end(), index)) !=
      m_usedColorIndices.end()) {
    m_usedColorIndices.erase(colorIndexIter);
  }
}
