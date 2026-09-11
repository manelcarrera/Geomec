// CLineMoment.cpp: implementation of the CLineMoment class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCLineMoment.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "DCLoadCase.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CLineMoment::CLineMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ILine &line)
:	IDirectedLoad(loadcase, size, vecDirection), m_Line(line)
{
}

CLineMoment::~CLineMoment()
{
}

const geo::ILine &CLineMoment::Line() const
{
  return m_Line;
}

bool CLineMoment::WriteFilos() const
{
  ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(Direction());

  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if(idx < 0) idx = 0;
  idx++;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t) (Line().Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  PutItem("DIRECT", &idxdir);

  PutItemLength("MOMENT", Values(), ValueSize());

  SetActive("LINE", FTN_TRUE);

  PopDir();

  return true;
}

} // namespace dia
