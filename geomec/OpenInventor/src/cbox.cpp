#include "stdafx.h"

#include "boxdlg.h"
#include "cbox.h"
#include "custombtn.h"

#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>

#include <Inventor/nodes/SoRotationXYZ.h>

namespace {}

float CBox::radians(float degrees) {
  float radians = ((M_PI / 2) / 90.0) * degrees;
  return radians;
}

int CBox::degrees(float radians) {
  float degrees = radians / ((M_PI / 2) / 90.0);
  return degrees;
}

CBox::CBox() {
  m_sw = new SoSwitch;
  m_b = new SoCube;

  m_t = new SoTranslation;

  m_rx = new SoRotationXYZ;
  m_rx->axis = SoRotationXYZ::X;

  m_ry = new SoRotationXYZ;
  m_ry->axis = SoRotationXYZ::Y;

  m_rz = new SoRotationXYZ;
  m_rz->axis = SoRotationXYZ::Z;

  m_s = new SoScale;

  const SbColor white(1.0f, 1.0f, 1.0f);
  SoMaterial *mat = new SoMaterial;
  mat->diffuseColor = white;
  mat->emissiveColor = white;

  SoDrawStyle *style = new SoDrawStyle;
  style->style.setValue(SoDrawStyle::LINES);
  style->lineWidth.setValue(2);
  style->linePattern.setValue(0xf0f0);

  addChild(m_sw);

  SoNode *END = (SoNode *)(-1);
  SoNode *nodes[] = {mat, style, m_rx, m_ry, m_rz, m_t, m_s, m_b, END};
  int i = 0;
  while (nodes[i] != END)
    m_sw->addChild(nodes[i++]);

  hide();
}

void CBox::show() { m_sw->whichChild = SO_SWITCH_ALL; }
bool CBox::is_show() { return m_sw->whichChild.getValue() == SO_SWITCH_ALL; }
void CBox::hide() { m_sw->whichChild = SO_SWITCH_NONE; }

// http://developer90.openinventor.com/content/55-transformations
void CBox::rotate(std::vector<float> vals) {
  m_rx->angle = radians(vals[0]);
  m_ry->angle = radians(vals[1]);
  m_rz->angle = radians(vals[2]);
}

void CBox::scale(std::vector<float> vals) { m_s->scaleFactor.setValue(vals[0], vals[1], vals[2]); }

// http://mevislabdownloads.mevis.de/docs/2.3/MeVisLab/Resources/Documentation/Publish/SDK/GettingStarted/ch07.html
void CBox::translate(std::vector<float> vals) {
  const SbVec3f t2 = m_t->translation.getValue();

  // float N = vals[ 1 ] == -1 ? t.getValue()[ 1 ] : vals[ 1 ];
  float Pe = vals[0] == -1 ? t2[0] : vals[0];
  float Pn = vals[1] == -1 ? t2[1] : vals[1];
  float Pd = vals[2] == -1 ? t2[2] : vals[2];

  SoSFVec3f &t = m_t->translation;
  t.setValue(Pe, Pn, Pd);

  // MCR: No one of this does anything
  // t.touch();
  // touch();
  // m_b->touch();
  // t.connectFrom( SoDB::getGlobalField("realTime") );
}

// GET

std::vector<float> CBox::translation() {
  const SbVec3f t = m_t->translation.getValue();
  return std::vector<float>{t[0], t[1], t[2]};
}

std::vector<int> CBox::rotation() {
  return std::vector<int>{degrees(m_rx->angle.getValue()), degrees(m_ry->angle.getValue()),
                          degrees(m_rz->angle.getValue())};
}

std::vector<float> CBox::scale() {
  const SbVec3f &s = m_s->scaleFactor.getValue();
  return std::vector<float>{s[0], s[1], s[2]};
}

// SET / STEPS

void CBox::translate(int coordinate, int step, int plus_or_minus) {
  const SbVec3f orig = m_t->translation.getValue();

  double inc = plus_or_minus * step;

  double Pe = orig[0];
  double Pn = orig[1];
  double Pd = orig[2];

  SoSFVec3f &t = m_t->translation;

  int STEPS = 1;
  for (int i = 0; i < STEPS; i++) // MCR: This doesn't render the intermedite steps
  {
    double inc2 = ((double)(i + 1) * inc / (double)STEPS);

    if (coordinate == CBoxDlg::Pe)
      Pe += inc2;
    else if (coordinate == CBoxDlg::Pn)
      Pn += inc2;
    else if (coordinate == CBoxDlg::Pd)
      Pd += inc2;

    t.setValue(Pe, Pn, Pd);
  }

  // t.touch(); // MCR: thos doesn't make the rendering faster
}

void CBox::rotate(int coordinate, int step, int plus_or_minus) {
  float inc = plus_or_minus * radians((float)step);

  if (coordinate == CBoxDlg::Rx)
    m_rx->angle = m_rx->angle.getValue() + inc;
  else if (coordinate == CBoxDlg::Ry)
    m_ry->angle = m_ry->angle.getValue() + inc;
  else if (coordinate == CBoxDlg::Rz)
    m_rz->angle = m_rz->angle.getValue() + inc;
}

void CBox::scale(int coordinate, int step, int plus_or_minus) {
  float factor = plus_or_minus == CBtn::Plus ? step : 1.0 / step;

  float fx = 1;
  float fy = 1;
  float fz = 1;

  if (coordinate == CBoxDlg::Sx)
    fx = factor;
  else if (coordinate == CBoxDlg::Sy)
    fy = factor;
  else if (coordinate == CBoxDlg::Sz)
    fz = factor;

  SoSFVec3f &sf = m_s->scaleFactor;
  const SbVec3f &val = sf.getValue();

  m_s->scaleFactor.setValue(fx * val[0], fy * val[1], fz * val[2]);
}