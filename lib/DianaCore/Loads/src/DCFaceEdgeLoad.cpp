// DCFaceEdgeLoad.cpp: implementation of the CFaceEdgeLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCFaceEdgeLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"
#include "DCLoadCase.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

// single value for entire edge
CFaceEdgeLoad::CFaceEdgeLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face, int iEdge)
: IDirectedLoad(loadcase, size, vecDirection), m_Face(face), m_iEdgeIndex(iEdge), m_vcVectors(1)
{
	// only works for triangles and quadrilaterals
	assert(face.NrOfPoints() == 3 || face.NrOfPoints() == 4);
	assert(vecDirection == vecDirection.UnitVector());
	assert(iEdge >= 0 && iEdge < face.NrOfLines());
	m_vcVectors[0] = geo::CVector(vecDirection * size);
}

// different values for each node
CFaceEdgeLoad::CFaceEdgeLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IVector &vecDirection, const geo::IFace &face, int iEdge)
: IDirectedLoad(loadcase, 0, vecDirection), m_Face(face), m_iEdgeIndex(iEdge), m_vcVectors(vcValues.size())
{
	// only works for triangles and quadrilaterals
	assert(face.NrOfPoints() == 3 || face.NrOfPoints() == 4);
	assert(vcValues.size() == 2);
	assert(vecDirection == vecDirection.UnitVector());
	assert(iEdge >= 0 && iEdge < face.NrOfLines());
	for(size_t i = 0; i < vcValues.size(); i++) m_vcVectors[i] = geo::CVector(vecDirection * vcValues[i]);
}

// different vectors for each node
CFaceEdgeLoad::CFaceEdgeLoad(CLoadCase &loadcase, const std::vector<geo::CVector> &vcVectors, const geo::IFace &face, int iEdge)
: IDirectedLoad(loadcase, 0, geo::CVector(0, 0, 0)), m_Face(face), m_iEdgeIndex(iEdge), m_vcVectors(vcVectors.size())
{
	// only works for triangles and quadrilaterals
	assert(face.NrOfPoints() == 3 || face.NrOfPoints() == 4);
	assert(vcVectors.size() == 2);
	assert(iEdge >= 0 && iEdge < face.NrOfLines());
	for(size_t i = 0; i < vcVectors.size(); i++) m_vcVectors[i] = vcVectors[i];
}

CFaceEdgeLoad::~CFaceEdgeLoad()
{
}

void CFaceEdgeLoad::WriteFilos(std::vector<double> vcValues, const geo::IVector &dir) const
{
	ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(dir);

	ftn_int_t idx = Inquire("ELEMEN", "DIM");
	if(idx < 0) idx = 0;
	idx++;

	assert(!XistIndexed("ELEMEN/", &idx));

	PushDir();

	ChangeIndexedDir("ELEMEN/", &idx);

	ftn_int_t idxelm = (ftn_int_t) (Face().Index() + 1);
	PutItemLength("ELEMEN", &idxelm, 1);

	ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
	PutItem("CASE", &idxcase);

	char *name = EdgeName();
	PutCharItem("EDGE", name);
	DiFree(name, "CFaceEdgeLoad::WriteFilos");

	assert(vcValues.size() == 2);
	ftn_double_t *values = (ftn_double_t *) DiMalloc(vcValues.size() * sizeof (ftn_double_t), "CFaceEdgeLoad::WriteFilos");
	for(size_t i = 0; i < vcValues.size(); i++) values[i] = vcValues[i];
	PutItemLength("FORCE", values, vcValues.size());
	DiFree(values, "CFaceEdgeLoad::WriteFilos");

	PutItem("DIRECT", &idxdir);

	PopDir();
}

bool CFaceEdgeLoad::WriteFilos() const
{
	std::vector<double> vcValues(m_vcVectors.size());
	bool nonzero;
	
	// write X direction
	nonzero = false;
	for(size_t i = 0; i < m_vcVectors.size(); i++)
	{
		vcValues[i] = m_vcVectors[i].X();
		if(fabs(vcValues[i]) > 0) nonzero = true;
	}
	if(nonzero) WriteFilos(vcValues, geo::CVector::Xaxis);

	// write Y direction
	nonzero = false;
	for(size_t i = 0; i < m_vcVectors.size(); i++)
	{
		vcValues[i] = m_vcVectors[i].Y();
		if(fabs(vcValues[i]) > 0) nonzero = true;
	}
	if(nonzero) WriteFilos(vcValues, geo::CVector::Yaxis);

	// write Z direction
	nonzero = false;
	for(size_t i = 0; i < m_vcVectors.size(); i++)
	{
		vcValues[i] = m_vcVectors[i].Z();
		if(fabs(vcValues[i]) > 0) nonzero = true;
	}
	if(nonzero) WriteFilos(vcValues, geo::CVector::Zaxis);

	return true;
}

const geo::IFace &CFaceEdgeLoad::Face() const
{
	return m_Face;
}

int CFaceEdgeLoad::EdgeIndex() const
{
	return m_iEdgeIndex;
}

char *CFaceEdgeLoad::EdgeName() const
{
	char *name = 0;

	switch(Face().NrOfLines())
	{
	case 3:
		{
			const char *facename[3] = { "L3", "L1", "L2" };
			name = DiStrsave(facename[EdgeIndex()]);
		}
		break;
	case 4:
		{
			const char *facename[4] = { "ETA1", "KSI2", "ETA2", "KSI1" };
			name = DiStrsave(facename[EdgeIndex()]);
		}
		break;
	default:
		assert(false);
	}

	return name;
}

} // namespace dia
