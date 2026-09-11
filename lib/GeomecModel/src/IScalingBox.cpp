// IScalingBox.cpp: implementation of the IScalingBox class.
//
//////////////////////////////////////////////////////////////////////
#include "IScalingBox.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IScalingBox::IScalingBox(CFemAppModel &model, const QString &strName)
    : CColorNode(strName, model), m_box(geo::CPoint(0, 0), geo::CPoint(100, 100)), m_dSideX(0), m_dSideY(0),
      m_dSideZ(0), m_dAzimuth(0), m_ptCenter(0.0, 0.0, 0.0) /*,
                                   m_Min(0.0, 0.0, 0.0),
                                   m_Max(0.0, 0.0, 0.0)*/
{}

IScalingBox::IScalingBox(CFemAppModel &model)
    : CColorNode(model), m_box(geo::CPoint(0, 0), geo::CPoint(100, 100)), m_dSideX(0), m_dSideY(0), m_dSideZ(0),
      m_dAzimuth(0), m_ptCenter(0.0, 0.0, 0.0) /*,
                      m_Min(0.0, 0.0, 0.0),
                      m_Max(0.0, 0.0, 0.0)*/
{}

IScalingBox::IScalingBox(CFemAppModel &model, const geo::IPoint &center, const double side)
    : CColorNode(model), m_box(geo::CPoint(0, 0), geo::CPoint(100, 100)), m_dSideX(side), m_dSideY(side),
      m_dSideZ(side), m_dAzimuth(0), m_ptCenter(center) /*,
                                      m_Min(0.0, 0.0, 0.0),
                                      m_Max(0.0, 0.0, 0.0)*/
{
  UpdateBox();
}

/*!
 * \brief Constructor
 * \param model Model to which the box belongs
 * \param center Center point of the box
 * \param sideX Length of the box along X axis
 * \param sideY Length of the box along Y axis
 * \param sideZ Length of the box along Z axis
 * \param azimuth Rotation around Z axis in degrees
 *
 * Creates a box with side lengths \c sideX, \c sideY and \c sideZ around point \c center.
 * The box is rotated around the Z axis (right hand rule) given the \c azimuth angle in degrees.
 */
IScalingBox::IScalingBox(CFemAppModel &model, const geo::IPoint &center, const double &sideX, const double &sideY,
                         const double &sideZ, const double &azimuth)
    : CColorNode(model), m_box(geo::CPoint(0, 0), geo::CPoint(100, 100)), m_dSideX(sideX), m_dSideY(sideY),
      m_dSideZ(sideZ), m_dAzimuth(azimuth), m_ptCenter(center) /*,
                                             m_Min(0.0, 0.0, 0.0),
                                             m_Max(0.0, 0.0, 0.0)*/
{
  UpdateBox();
}

/*!
 * \brief Copy constructor
 */
IScalingBox::IScalingBox(const IScalingBox &rhs)
    : CColorNode(rhs), m_box(rhs.m_box), m_dSideX(rhs.m_dSideX), m_dSideY(rhs.m_dSideY), m_dSideZ(rhs.m_dSideZ),
      m_dAzimuth(rhs.m_dAzimuth), m_ptCenter(rhs.m_ptCenter) /*,
                                   m_Min(rhs.m_Min),
                                   m_Max(rhs.m_Max)*/
{}

IScalingBox::~IScalingBox() {}

void IScalingBox::UpdateBox() {
  geo::CPoint ptMin;
  geo::CPoint ptMax;

  ptMin.Set(-m_dSideX / 2.0, -m_dSideY / 2.0, -m_dSideZ / 2.0);

  ptMax.Set(m_dSideX / 2.0, m_dSideY / 2.0, m_dSideZ / 2.0);

  geo::CVector directX(m_dSideX, 0.0, 0.0);
  geo::CVector directY(0.0, m_dSideY, 0.0);
  geo::CVector directZ(0.0, 0.0, m_dSideZ);

  geo::CPoint p1(ptMin + directX);
  geo::CPoint p2(ptMin + directX + directY);
  geo::CPoint p3(ptMin + directY);
  geo::CPoint p4(ptMin + directZ);
  geo::CPoint p5(ptMin + directZ + directX);
  geo::CPoint p6(ptMin + directZ + directX + directY);
  geo::CPoint p7(ptMin + directZ + directY);

  m_box.Point(0, ptMin);
  m_box.Point(1, p1);
  m_box.Point(2, p2);
  m_box.Point(3, p3);
  m_box.Point(4, p4);
  m_box.Point(5, p5);
  m_box.Point(6, p6);
  m_box.Point(7, p7);

  // Rotate method uses angle in degree, just like ourselves
  m_box.Rotate(geo::CVector::Zaxis, m_dAzimuth);

  m_box.Move(geo::CVector(m_ptCenter));

  // pick up min and max points from box
  //	m_Min = m_box.Min();
  //	m_Max = m_box.Max();
}

int IScalingBox::DisplayListSize() const { return 1; }

const geo::IObject &IScalingBox::DisplayList(int /*nIndex*/) const { return m_box; }

void IScalingBox::Side(const double &dSide, bool bUpdate) {
  if (fabs(m_dSideX - dSide) < EPS && fabs(m_dSideY - dSide) < EPS && fabs(m_dSideZ - dSide) < EPS)
    return;

  m_dSideX = m_dSideY = m_dSideZ = dSide;
  if (bUpdate)
    UpdateBox();
}

void IScalingBox::SideX(const double &dSize, bool bUpdate) {
  if (fabs(m_dSideX - dSize) < EPS)
    return;

  m_dSideX = dSize;
  if (bUpdate)
    UpdateBox();
}

void IScalingBox::SideY(const double &dSize, bool bUpdate) {
  if (fabs(m_dSideY - dSize) < EPS)
    return;

  m_dSideY = dSize;
  if (bUpdate)
    UpdateBox();
}

void IScalingBox::SideZ(const double &dSize, bool bUpdate) {
  if (fabs(m_dSideZ - dSize) < EPS)
    return;

  m_dSideZ = dSize;
  if (bUpdate)
    UpdateBox();
}

const double &IScalingBox::SideX() const { return m_dSideX; }

const double &IScalingBox::SideY() const { return m_dSideY; }

const double &IScalingBox::SideZ() const { return m_dSideZ; }

void IScalingBox::Azimuth(const double &azimuth, bool bUpdate) {
  if (fabs(m_dAzimuth - azimuth) < EPS)
    return;

  m_dAzimuth = azimuth;
  if (bUpdate)
    UpdateBox();
}

const double &IScalingBox::Azimuth() const { return m_dAzimuth; }

void IScalingBox::Center(const geo::IPoint &center, bool bUpdate) {
  if (m_ptCenter == center)
    return;

  m_ptCenter = center;
  if (bUpdate)
    UpdateBox();
}

const geo::IPoint &IScalingBox::Center() const { return m_ptCenter; }

IScalingBox &IScalingBox::operator=(const IScalingBox &rhs) {
  CColorNode::operator=(rhs);

  m_box = rhs.m_box;
  m_dSideX = rhs.m_dSideX;
  m_dSideY = rhs.m_dSideY;
  m_dSideZ = rhs.m_dSideZ;
  m_dAzimuth = rhs.m_dAzimuth;
  m_ptCenter = rhs.m_ptCenter;
  //	m_Min = rhs.m_Min;
  //	m_Max = rhs.m_Max;

  return *this;
}
