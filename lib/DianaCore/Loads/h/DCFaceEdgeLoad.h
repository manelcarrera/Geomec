// DCFaceEdgeLoad.h: interface for the CFaceEdgeLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCFACEEDGELOAD_H__6A27AE27_5AED_4A67_8BB8_61E4F811407C__INCLUDED_)
#define AFX_DCFACEEDGELOAD_H__6A27AE27_5AED_4A67_8BB8_61E4F811407C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"
#include "IDCDirectedLoad.h"

namespace dia {

class CLoadCase;

class CFaceEdgeLoad : public IDirectedLoad  
{
	const geo::IFace &m_Face;
	int m_iEdgeIndex;
	std::vector<geo::CVector> m_vcVectors;
	void WriteFilos(std::vector<double> vcValues, const geo::IVector &dir) const;
	char *EdgeName() const;

public:
	// the index of the edge is equal to the index of its first point

	// single value for entire edge
	CFaceEdgeLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face, int iEdge);
	// different values for each node
	CFaceEdgeLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IVector &vecDirection, const geo::IFace &face, int iEdge);
	// different vectors for each node
	CFaceEdgeLoad(CLoadCase &loadcase, const std::vector<geo::CVector> &vcVectors, const geo::IFace &face, int iEdge);
	virtual ~CFaceEdgeLoad();

	virtual bool WriteFilos() const;
	const geo::IFace &Face() const;
	int EdgeIndex() const;
};

}

#endif // !defined(AFX_DCFACEEDGELOAD_H__6A27AE27_5AED_4A67_8BB8_61E4F811407C__INCLUDED_)
