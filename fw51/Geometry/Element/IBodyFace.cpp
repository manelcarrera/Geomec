 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IBodyFace.cpp: implementation of the IBodyFace class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "IObject.h"
#include "IPoint.h"
#include "Point.h"
#include "VectorTempl.h"
#include "IElementSet.h"
#include "IValue.h"
#include "Value.h"
#include "IProgressBase.h"
#include "IValueSet.h"
#include "IElement.h"
#include "ILine.h"
#include "Line.h"
#include "IFace.h"
#include "IBodyFace.h"
#include "IBody.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IBodyFace::IBodyFace(const IBody& body, const std::vector<int>& vcPoint, const char *name)
: m_vcPoint(vcPoint), m_body(body), m_Name(name)
{
}

IBodyFace::IBodyFace(const IBody& body, int p0, int p1, int p2, int p3, const char *name)
: m_vcPoint(4), m_body(body), m_Name(name)
{
  m_vcPoint[0] = p0;
  m_vcPoint[1] = p1;
  m_vcPoint[2] = p2;
  m_vcPoint[3] = p3;
}

IBodyFace::IBodyFace(const IBody& body, int p0, int p1, int p2, const char *name)
: m_vcPoint(3), m_body(body), m_Name(name)
{
  m_vcPoint[0] = p0;
  m_vcPoint[1] = p1;
  m_vcPoint[2] = p2;
}

const IPoint &IBodyFace::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcPoint.size());
  return m_body.Point(m_vcPoint[nIndex]);
}

void IBodyFace::Point(int /*nIndex*/, const IPoint &/*pt*/)
{
  assert(false);
}

int IBodyFace::NrOfPoints() const
{
  return m_vcPoint.size();
}

const IBody &IBodyFace::Body() const
{
  return m_body;
}

int IBodyFace::BodyNode(int nIndex) const
{
  return m_vcPoint[nIndex];
}

const char *IBodyFace::Name() const
{
  return m_Name;
}

}
