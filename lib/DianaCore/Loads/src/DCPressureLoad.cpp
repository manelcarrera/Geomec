// CPressureLoad.cpp: implementation of the CPressureLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "DCPressureLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "DCLoadCase.h"

#include "IElement.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CPressureLoad::CPressureLoad(CLoadCase &loadcase, double size, const geo::IElement &element)
:	IStructuralLoad(loadcase, size), m_Element(element)
{
}

CPressureLoad::CPressureLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IElement &element)
:	IStructuralLoad(loadcase, vcValues), m_Element(element)
{
}

CPressureLoad::~CPressureLoad()
{
}

const geo::IElement &CPressureLoad::Element() const
{
	return m_Element;
}

bool CPressureLoad::WriteFilos() const
{
	ftn_int_t idx = Inquire("ELEMEN", "DIM");
	if(idx < 0) idx = 0;
	++idx;

	assert(!XistIndexed("ELEMEN/", &idx));

	PushDir();

	ChangeIndexedDir("ELEMEN/", &idx);

	ftn_int_t idxelm = (ftn_int_t) (Element().Index() + 1);
	PutItemLength("ELEMEN", &idxelm, 1);

	ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
	PutItem("CASE", &idxcase);

	PutItemLength("PRESSU", Values(), ValueSize());

	PopDir();

	return true;
}

} // namespace dia
