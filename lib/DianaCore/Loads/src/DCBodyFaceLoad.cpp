// DCBodyFaceLoad.cpp: implementation of the CBodyFaceLoad class.
//
//////////////////////////////////////////////////////////////////////
#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCBodyFaceLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "DCLoadCase.h"

#include "IBody.h"
#include "IFace.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

// same value for all nodes
CBodyFaceLoad::CBodyFaceLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IBody &body, int nFaceIndex)
:	IDirectedLoad(loadcase, size, vecDirection), m_Body(body), m_nFaceIndex(nFaceIndex)
{
  assert(vecDirection == vecDirection.UnitVector());
}

// different values for each node
CBodyFaceLoad::CBodyFaceLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IVector &vecDirection, const geo::IBody &body, int nFaceIndex)
:	IDirectedLoad(loadcase, vcValues, vecDirection), m_Body(body), m_nFaceIndex(nFaceIndex)
{
  assert(vcValues.size() == body.Face(nFaceIndex).NrOfPoints());
  assert(vecDirection == vecDirection.UnitVector());
}

CBodyFaceLoad::CBodyFaceLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IBody &body, int nFaceIndex)
: IDirectedLoad(loadcase, vcValues, geo::CVector()), m_Body(body), m_nFaceIndex(nFaceIndex)
{
}


CBodyFaceLoad::CBodyFaceLoad(CLoadCase &loadcase, double size, const geo::IBody &body, int nFaceIndex)
: IDirectedLoad(loadcase, size, geo::CVector()), m_Body(body), m_nFaceIndex(nFaceIndex)
{
}

CBodyFaceLoad::~CBodyFaceLoad()
{
}

const geo::IBody &CBodyFaceLoad::Body() const
{
  return m_Body;
}


bool CBodyFaceLoad::WriteFilos() const
{
  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if(idx < 0) idx = 0;
  idx++;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t) (m_Body.Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  char *name = DiStrsave(m_Body.FaceName(m_nFaceIndex));
  PutCharItem("FACE", name);
  DiFree(name, "CBodyFaceLoad::WriteFilos");

  assert(ValueSize() == 1 || ValueSize() == m_Body.Face(m_nFaceIndex).NrOfPoints());
  PutItemLength("FORCE", Values(), ValueSize());

  if( !Direction().Empty() )
  {
    ftn_int_t nDirection = (ftn_int_t) Manager().Runner().AddDirection( Direction() );
    PutItem("DIRECT", &nDirection);
  }
  else
  {
    const char *normal = "NORMAL";
    PutCharItem("DIRELM", normal);
  }

  PopDir();
  return true;
}

int CBodyFaceLoad::FaceIndex() const
{
  return m_nFaceIndex;
}

} // namespace dia
