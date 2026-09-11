#pragma once

#include <Inventor/nodes/SoSeparator.h>

class SoScale;
class SoTranslation;
class SoCube;
class SoRotationXYZ;
class SoSwitch;

class CBox : public SoSeparator {
  SoCube *m_b;
  SoSwitch *m_sw;
  SoTranslation *m_t;
  SoRotationXYZ *m_rx;
  SoRotationXYZ *m_ry;
  SoRotationXYZ *m_rz;
  SoScale *m_s;

public:
  CBox();

  void show();
  void hide();
  bool is_show();

  // set values
  void rotate(std::vector<float> vals);
  void scale(std::vector<float> vals);
  void translate(std::vector<float> vals);

  // get
  std::vector<float> translation();
  std::vector<int> rotation();
  std::vector<float> scale();

  // steps: '+' / '-'
  void translate(int coordinate, int step, int plus_or_minus);
  void rotate(int coordinate, int step, int plus_or_minus);
  void scale(int coordinate, int step, int plus_or_minus);

  // util
  static float radians(float degrees);
  static int degrees(float radians);
};