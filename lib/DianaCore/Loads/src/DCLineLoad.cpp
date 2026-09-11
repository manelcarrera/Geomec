// CLineLoad.cpp: implementation of the CLineLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCLineLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "DCLoadCase.h"

#include "ElementGroup.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CLineLoad::CLineLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ILine &line)
:	IDirectedLoad(loadcase, size, vecDirection), m_Object(line), m_nSetIndex(-1)
{
}

CLineLoad::CLineLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IElementSet &group, int nSetIndex)
:	IDirectedLoad(loadcase, size, vecDirection), m_Object(group), m_nSetIndex(nSetIndex)
{
}

CLineLoad::~CLineLoad()
{
}

bool CLineLoad::WriteFilos() const
{
	ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(Direction());

	ftn_int_t idx = Inquire("ELEMEN", "DIM");
	if(idx < 0) idx = 0;
	idx++;

	assert(!XistIndexed("ELEMEN/", &idx));

	PushDir();

	ChangeIndexedDir("ELEMEN/", &idx);

	const geo::ILine *pLine = dynamic_cast<const geo::ILine *> (&m_Object);
	const geo::CElementGroup *pGroup = dynamic_cast<const geo::CElementGroup *> (&m_Object);
	assert(pLine || pGroup);

	if(pLine)
	{
		ftn_int_t idxelm = (ftn_int_t) (pLine->Index() + 1);
		PutItemLength("ELEMEN", &idxelm, 1);
	}
	else
	{
		assert(m_nSetIndex >= 0);
		WriteGroupName(*pGroup, m_nSetIndex);
	}

	ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
	PutItem("CASE", &idxcase);

	PutItem("DIRECT", &idxdir);

	PutItemLength("FORCE", Values(), ValueSize());

	SetActive("LINE", FTN_TRUE);

	PopDir();

	return true;
}

} // namespace dia
