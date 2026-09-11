#ifndef _ColorManager_h_
#define _ColorManager_h_

class ColorManager
{
  static const int s_numUniqueColors = 16;
  static const int s_availableHues[s_numUniqueColors];

  static const int s_availableSaturations[s_numUniqueColors];

private:
  std::vector<int> m_usedColorIndices;
  int m_lastColorIndex;

public:
  ColorManager();
  static QColor MakeCurveColor(int n);

  static QColor MakeMarkerColor(int n);

  int allocateColorIndex ();

  void releaseColorIndex(int index);
};

#endif  // _ColorManager_h_
