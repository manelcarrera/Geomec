 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IWedge.h"
#include "BodyTriangle.h"
#include "BodyQuadrilateral.h"
#include "BodyLine.h"

#include "lbel.h"

namespace geo {

static const int quad_point_indices[] =
{
	0,
	2,
	4,
	9,
	11,
	13
};

static const int cubic_point_indices[] =
{
	0,
	3,
	6,
	15,
	18,
	21
};


void IWedge::BuildIntegrationPoints( IElement::TIntPtVec& vec,
                                     int                  nintxy,
                                     int                  nintz )
{
	int ixy, iz;

	// the wedge has nintz layers of triangular schemes in the z direction
	for(iz = 0; iz < nintz; iz++)	{
		double coordz;
		double weightz;
		GaussCoeff(iz + 1, nintz, &coordz, &weightz);
    for(ixy = 0; ixy < nintxy; ixy++) {
			// the hammer coordinates returned include the third
			// (dependent) coordinate 1 - xi - eta
			double coordxy[3];
			double weightxy;
			HammerAreaIntegration(ixy + 1, nintxy, coordxy, &weightxy);
			std::pair<IElement::TDoubleVec, double> prIntpt;
			prIntpt.first.push_back(coordxy[0]);
			prIntpt.first.push_back(coordxy[1]);
			prIntpt.first.push_back(coordz);
			prIntpt.second = weightxy * weightz;
			vec.push_back(prIntpt);
		}
	}
}

const IElement::TIntPtVec& IWedge::IntegrationPoints( int order )
{
  // integration point location and weights
  static TIntPtVec s_LinearIntegrationPoints;
  static TIntPtVec s_QuadIntegrationPoints;
  static TIntPtVec s_CubicIntegrationPoints;

  switch( order ) {
	case 1:
		if(s_LinearIntegrationPoints.empty())
			BuildIntegrationPoints(s_LinearIntegrationPoints, 1, 1);
		return s_LinearIntegrationPoints;
		break;
	case 2:
		if(s_QuadIntegrationPoints.empty())
			BuildIntegrationPoints(s_QuadIntegrationPoints, 3, 2);
		return s_QuadIntegrationPoints;
		break;
	case 3:
		if(s_CubicIntegrationPoints.empty())
			BuildIntegrationPoints(s_CubicIntegrationPoints, 4, 3);
		return s_CubicIntegrationPoints;
		break;
	}

	assert(false);
	TIntPtVec* pBogus = 0;
	return *pBogus;
}

const IElement::TIntPtVec& IWedge::IntegrationPoints() const
{
  return IntegrationPoints( Order() );
}

void IWedge::PrepareMapping()
{
  for (int i = 1; i < 4; ++i)
    IntegrationPoints(i);
}

IWedge::IWedge()
{
}

IWedge::~IWedge()
{
}

int IWedge::NrOfPoints() const
{
	return NrOfNodes();
//	return 6;
}

const IPoint &IWedge::Point(int nIndex) const
{
	return Node( nIndex );
#if 0
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	switch(Order())
	{
	case 1:
		return Node(nIndex);
	case 2:
		return Node(quad_point_indices[nIndex]);
	case 3:
		return Node(cubic_point_indices[nIndex]);
	}

	assert(false);
	const IPoint* pBogus = 0;
	return *pBogus;
#endif
}

void IWedge::Point(int nIndex, const IPoint &pt)
{
	Node( nIndex, pt );
#if 0
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	switch(Order())
	{
	case 1:
		Node(nIndex, pt);
		break;
	case 2:
		Node(quad_point_indices[nIndex], pt);
		break;
	case 3:
		Node(cubic_point_indices[nIndex], pt);
		break;
	default:
		assert(false);
	}
#endif
}

int IWedge::NrOfFaces() const
{
	return 5;
}

const IFace &IWedge::Face(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfFaces());

	if(m_vcFaces.empty()) m_vcFaces.resize(NrOfFaces(), 0);

	if(!m_vcFaces[nIndex])
	{
		switch(nIndex)
		{
			// top and bottom are triangles
			case 0:
			case 4:
				m_vcFaces[nIndex] = new CBodyTriangle(*const_cast<IWedge*>(this), nIndex);
				break;
			// sides are quadrilaterals
			case 1:
			case 2:
			case 3:
				m_vcFaces[nIndex] = new CBodyQuadrilateral(*const_cast<IWedge*>(this), nIndex);
				break;
			default:
				assert(false);
		}
	}

	return *m_vcFaces[nIndex];
}

const IWedge::TIndexVec& IWedge::FacePointIndices(int nIndex) const
{
  return FaceNodeIndices( 1, nIndex );
}

void IWedge::InitFaceNodeIndices( std::vector<TFaceNodeVec>& FaceNodeIdxs )
{
  assert( FaceNodeIdxs.size() == 0 );  // Init once ...

  FaceNodeIdxs.resize(3, TFaceNodeVec(5) );

	// Initialise for First Order elements
	// Face 0
  FaceNodeIdxs[0][0].resize(3);
  FaceNodeIdxs[0][0][0] = 0;
  FaceNodeIdxs[0][0][1] = 2;
  FaceNodeIdxs[0][0][2] = 1;
	// Face 1
  FaceNodeIdxs[0][1].resize(4);
  FaceNodeIdxs[0][1][0] = 1;
  FaceNodeIdxs[0][1][1] = 2;
  FaceNodeIdxs[0][1][2] = 5;
  FaceNodeIdxs[0][1][3] = 4;
	// Face 2
  FaceNodeIdxs[0][2].resize(4);
  FaceNodeIdxs[0][2][0] = 0;
  FaceNodeIdxs[0][2][1] = 3;
  FaceNodeIdxs[0][2][2] = 5;
  FaceNodeIdxs[0][2][3] = 2;
	// Face 3
  FaceNodeIdxs[0][3].resize(4);
  FaceNodeIdxs[0][3][0] = 0;
  FaceNodeIdxs[0][3][1] = 1;
  FaceNodeIdxs[0][3][2] = 4;
  FaceNodeIdxs[0][3][3] = 3;
	// Face 4
  FaceNodeIdxs[0][4].resize(3);
  FaceNodeIdxs[0][4][0] = 3;
  FaceNodeIdxs[0][4][1] = 4;
  FaceNodeIdxs[0][4][2] = 5;
			
	// Initialise second order elements
	// Face 0
  FaceNodeIdxs[1][0].resize(6);
  FaceNodeIdxs[1][0][0] = 0;
  FaceNodeIdxs[1][0][1] = 5;
  FaceNodeIdxs[1][0][2] = 4;
  FaceNodeIdxs[1][0][3] = 3;
  FaceNodeIdxs[1][0][4] = 2;
  FaceNodeIdxs[1][0][5] = 1;
	// Face 1
  FaceNodeIdxs[1][1].resize(8);
  FaceNodeIdxs[1][1][0] = 2;
  FaceNodeIdxs[1][1][1] = 3;
  FaceNodeIdxs[1][1][2] = 4;
  FaceNodeIdxs[1][1][3] = 8;
  FaceNodeIdxs[1][1][4] = 13;
  FaceNodeIdxs[1][1][5] = 12;
  FaceNodeIdxs[1][1][6] = 11;
  FaceNodeIdxs[1][1][7] = 7;
	// Face 2
  FaceNodeIdxs[1][2].resize(8);
  FaceNodeIdxs[1][2][0] = 0;
  FaceNodeIdxs[1][2][1] = 6;
  FaceNodeIdxs[1][2][2] = 9;
  FaceNodeIdxs[1][2][3] = 14;
  FaceNodeIdxs[1][2][4] = 13;
  FaceNodeIdxs[1][2][5] = 8;
  FaceNodeIdxs[1][2][6] = 4;
  FaceNodeIdxs[1][2][7] = 5;
	// Face 3
  FaceNodeIdxs[1][3].resize(8);
  FaceNodeIdxs[1][3][0] = 0;
  FaceNodeIdxs[1][3][1] = 1;
  FaceNodeIdxs[1][3][2] = 2;
  FaceNodeIdxs[1][3][3] = 7;
  FaceNodeIdxs[1][3][4] = 11;
  FaceNodeIdxs[1][3][5] = 10;
  FaceNodeIdxs[1][3][6] = 9;
  FaceNodeIdxs[1][3][7] = 6;
	// Face 4
  FaceNodeIdxs[1][4].resize(6);
  FaceNodeIdxs[1][4][0] = 9;
  FaceNodeIdxs[1][4][1] = 10;
  FaceNodeIdxs[1][4][2] = 11;
  FaceNodeIdxs[1][4][3] = 12;
  FaceNodeIdxs[1][4][4] = 13;
  FaceNodeIdxs[1][4][5] = 14;

	// Initialise Third Order elements
	// Face 0
  FaceNodeIdxs[2][0].resize(9);
  FaceNodeIdxs[2][0][0] = 0;
  FaceNodeIdxs[2][0][1] = 8;
  FaceNodeIdxs[2][0][2] = 7;
  FaceNodeIdxs[2][0][3] = 6;
  FaceNodeIdxs[2][0][4] = 5;
  FaceNodeIdxs[2][0][5] = 4;
  FaceNodeIdxs[2][0][6] = 3;
  FaceNodeIdxs[2][0][7] = 2;
  FaceNodeIdxs[2][0][8] = 1;
	// Face 1
  FaceNodeIdxs[2][1].resize(12);
  FaceNodeIdxs[2][1][0] = 3;
  FaceNodeIdxs[2][1][1] = 4;
  FaceNodeIdxs[2][1][2] = 5;
  FaceNodeIdxs[2][1][3] = 6;
  FaceNodeIdxs[2][1][4] = 11;
  FaceNodeIdxs[2][1][5] = 14;
  FaceNodeIdxs[2][1][6] = 21;
  FaceNodeIdxs[2][1][7] = 20;
  FaceNodeIdxs[2][1][8] = 19;
  FaceNodeIdxs[2][1][9] = 18;
  FaceNodeIdxs[2][1][10] = 13;
  FaceNodeIdxs[2][1][11] = 10;
	// Face 2
  FaceNodeIdxs[2][2].resize(12);
  FaceNodeIdxs[2][2][0] = 0;
  FaceNodeIdxs[2][2][1] = 9;
  FaceNodeIdxs[2][2][2] = 12;
  FaceNodeIdxs[2][2][3] = 15;
  FaceNodeIdxs[2][2][4] = 23;
  FaceNodeIdxs[2][2][5] = 22;
  FaceNodeIdxs[2][2][6] = 21;
  FaceNodeIdxs[2][2][7] = 14;
  FaceNodeIdxs[2][2][8] = 11;
  FaceNodeIdxs[2][2][9] = 6;
  FaceNodeIdxs[2][2][10] = 7;
  FaceNodeIdxs[2][2][11] = 8;
	// Face 3
  FaceNodeIdxs[2][3].resize(12);
  FaceNodeIdxs[2][3][0] = 0;
  FaceNodeIdxs[2][3][1] = 1;
  FaceNodeIdxs[2][3][2] = 2;
  FaceNodeIdxs[2][3][3] = 3;
  FaceNodeIdxs[2][3][4] = 10;
  FaceNodeIdxs[2][3][5] = 13;
  FaceNodeIdxs[2][3][6] = 18;
  FaceNodeIdxs[2][3][7] = 17;
  FaceNodeIdxs[2][3][8] = 16;
  FaceNodeIdxs[2][3][9] = 15;
  FaceNodeIdxs[2][3][10] = 12;
  FaceNodeIdxs[2][3][11] = 9;
	// Face 4
  FaceNodeIdxs[2][4].resize(9);
  FaceNodeIdxs[2][4][0] = 15;
  FaceNodeIdxs[2][4][1] = 16;
  FaceNodeIdxs[2][4][2] = 17;
  FaceNodeIdxs[2][4][3] = 18;
  FaceNodeIdxs[2][4][4] = 19;
  FaceNodeIdxs[2][4][5] = 20;
  FaceNodeIdxs[2][4][6] = 21;
  FaceNodeIdxs[2][4][7] = 22;
  FaceNodeIdxs[2][4][8] = 23;
}

const IWedge::TIndexVec& IWedge::FaceNodeIndices( int order, int nIndex )
{
  static std::vector<TFaceNodeVec> s_FaceNodeIndices;

  assert(nIndex >= 0 && nIndex < 5 );
  if( s_FaceNodeIndices.size() == 0 ) InitFaceNodeIndices( s_FaceNodeIndices );

  return s_FaceNodeIndices[order-1][nIndex];
}

const IWedge::TIndexVec& IWedge::FaceNodeIndices( int nIndex ) const
{
  return FaceNodeIndices( Order(), nIndex );
}
int IWedge::NrOfLines() const
{
	return 9;
}

const ILine &IWedge::Line(int nIndex) const
{
	if(m_vcLines.empty()) m_vcLines.resize(NrOfLines(), 0);

	if(!m_vcLines[nIndex]) m_vcLines[nIndex] = new CBodyLine(*const_cast<IWedge*>(this), nIndex);

	return *m_vcLines[nIndex];
}

const IWedge::TIndexVec& IWedge::LinePointIndices(int nIndex) const
{
  return LineNodeIndices( 1, nIndex );
}

void IWedge::InitLineNodeIndices( std::vector<TLineNodeVec>& lineNodeIdxs )
{
  assert( lineNodeIdxs.size() == 0 );  // Init once ...

  lineNodeIdxs.resize(3, TLineNodeVec(9) );

	// Initialise for First Order elements
	// Line 0
  lineNodeIdxs[0][0].resize(2);
  lineNodeIdxs[0][0][0] = 0;
  lineNodeIdxs[0][0][1] = 1;
	// Line 1
  lineNodeIdxs[0][1].resize(2);
  lineNodeIdxs[0][1][0] = 1;
  lineNodeIdxs[0][1][1] = 2;
	// Line 2
  lineNodeIdxs[0][2].resize(2);
  lineNodeIdxs[0][2][0] = 2;
  lineNodeIdxs[0][2][1] = 0;
	// Line 3
  lineNodeIdxs[0][3].resize(2);
  lineNodeIdxs[0][3][0] = 0;
  lineNodeIdxs[0][3][1] = 3;
	// Line 4
  lineNodeIdxs[0][4].resize(2);
  lineNodeIdxs[0][4][0] = 1;
  lineNodeIdxs[0][4][1] = 4;
	// Line 5
  lineNodeIdxs[0][5].resize(2);
  lineNodeIdxs[0][5][0] = 2;
  lineNodeIdxs[0][5][1] = 5;
	// Line 6
  lineNodeIdxs[0][6].resize(2);
  lineNodeIdxs[0][6][0] = 3;
  lineNodeIdxs[0][6][1] = 4;
	// Line 7
  lineNodeIdxs[0][7].resize(2);
  lineNodeIdxs[0][7][0] = 4;
  lineNodeIdxs[0][7][1] = 5;
	// Line 8
  lineNodeIdxs[0][8].resize(2);
  lineNodeIdxs[0][8][0] = 5;
  lineNodeIdxs[0][8][1] = 0;

	// Initialise the second order elements
	// Line 0
  lineNodeIdxs[1][0].resize(3);
  lineNodeIdxs[1][0][0] = 0;
  lineNodeIdxs[1][0][1] = 1;
  lineNodeIdxs[1][0][2] = 2;
	// Line 1
  lineNodeIdxs[1][1].resize(3);
  lineNodeIdxs[1][1][0] = 2;
  lineNodeIdxs[1][1][1] = 3;
  lineNodeIdxs[1][1][2] = 4;
	// Line 2
  lineNodeIdxs[1][2].resize(3);
  lineNodeIdxs[1][2][0] = 4;
  lineNodeIdxs[1][2][1] = 5;
  lineNodeIdxs[1][2][2] = 6;
	// Line 3
  lineNodeIdxs[1][3].resize(3);
  lineNodeIdxs[1][3][0] = 0;
  lineNodeIdxs[1][3][1] = 6;
  lineNodeIdxs[1][3][2] = 9;
	// Line 4
  lineNodeIdxs[1][4].resize(3);
  lineNodeIdxs[1][4][0] = 2;
  lineNodeIdxs[1][4][1] = 7;
  lineNodeIdxs[1][4][2] = 11;
	// Line 5
  lineNodeIdxs[1][5].resize(3);
  lineNodeIdxs[1][5][0] = 4;
  lineNodeIdxs[1][5][1] = 8;
  lineNodeIdxs[1][5][2] = 13;
	// Line 6
  lineNodeIdxs[1][6].resize(3);
  lineNodeIdxs[1][6][0] = 9;
  lineNodeIdxs[1][6][1] = 10;
  lineNodeIdxs[1][6][2] = 11;
	// Line 7
  lineNodeIdxs[1][7].resize(3);
  lineNodeIdxs[1][7][0] = 11;
  lineNodeIdxs[1][7][1] = 12;
  lineNodeIdxs[1][7][2] = 13;
	// Line 8
  lineNodeIdxs[1][8].resize(3);
  lineNodeIdxs[1][8][0] = 13;
  lineNodeIdxs[1][8][1] = 14;
  lineNodeIdxs[1][8][2] = 9;

	// Initialise the third order elements
	// Line 0
  lineNodeIdxs[2][0].resize(4);
  lineNodeIdxs[2][0][0] = 0;
  lineNodeIdxs[2][0][1] = 1;
  lineNodeIdxs[2][0][2] = 2;
  lineNodeIdxs[2][0][3] = 3;
	// Line 1
  lineNodeIdxs[2][1].resize(4);
  lineNodeIdxs[2][1][0] = 3;
  lineNodeIdxs[2][1][1] = 4;
  lineNodeIdxs[2][1][2] = 5;
  lineNodeIdxs[2][1][3] = 6;
	// Line 2
  lineNodeIdxs[2][2].resize(4);
  lineNodeIdxs[2][2][0] = 6;
  lineNodeIdxs[2][2][1] = 7;
  lineNodeIdxs[2][2][2] = 8;
  lineNodeIdxs[2][2][3] = 0;
	// Line 3
  lineNodeIdxs[2][3].resize(4);
  lineNodeIdxs[2][3][0] = 0;
  lineNodeIdxs[2][3][1] = 9;
  lineNodeIdxs[2][3][2] = 12;
  lineNodeIdxs[2][3][3] = 15;
	// Line 4
  lineNodeIdxs[2][4].resize(4);
  lineNodeIdxs[2][4][0] = 3;
  lineNodeIdxs[2][4][1] = 10;
  lineNodeIdxs[2][4][2] = 13;
  lineNodeIdxs[2][4][3] = 18;
	// Line 5
  lineNodeIdxs[2][5].resize(4);
  lineNodeIdxs[2][5][0] = 6;
  lineNodeIdxs[2][5][1] = 11;
  lineNodeIdxs[2][5][2] = 14;
  lineNodeIdxs[2][5][3] = 21;
	// Line 6
  lineNodeIdxs[2][6].resize(4);
  lineNodeIdxs[2][6][0] = 15;
  lineNodeIdxs[2][6][1] = 16;
  lineNodeIdxs[2][6][2] = 17;
  lineNodeIdxs[2][6][3] = 18;
	// Line 7
  lineNodeIdxs[2][7].resize(4);
  lineNodeIdxs[2][7][0] = 18;
  lineNodeIdxs[2][7][1] = 19;
  lineNodeIdxs[2][7][2] = 20;
  lineNodeIdxs[2][7][3] = 21;
	// Line 8
  lineNodeIdxs[2][8].resize(4);
  lineNodeIdxs[2][8][0] = 21;
  lineNodeIdxs[2][8][1] = 22;
  lineNodeIdxs[2][8][2] = 23;
  lineNodeIdxs[2][8][3] = 15;
}

const IWedge::TIndexVec& IWedge::LineNodeIndices( int order, int nIndex )
{
  std::vector<TLineNodeVec> s_LineNodeIndices;

  assert(nIndex >= 0 && nIndex < 9 );
  if( s_LineNodeIndices.size() == 0 ) InitLineNodeIndices( s_LineNodeIndices );

  return s_LineNodeIndices[order-1][nIndex];
}

const IWedge::TIndexVec& IWedge::LineNodeIndices( int nIndex ) const
{
  return LineNodeIndices( Order(), nIndex );
}

IElement::TDoubleVec IWedge::ShapeFunction(const IElement::TDoubleVec& /*isocoords*/) const
{
	switch(Order())
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		assert(false);
	}

	IElement::TDoubleVec vcRet;
	return vcRet;
}

CMatrix IWedge::ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const
{
	assert(isocoords.size() == 3);

	assert(isocoords[0] >=  0.0 && isocoords[0] <= 1.0);
	assert(isocoords[1] >=  0.0 && isocoords[1] <= 1.0);
	assert(isocoords[2] >= -1.0 && isocoords[2] <= 1.0);

    // 3 rows, NrOfNodes() columns
    CMatrix ret(3, NrOfNodes());

    double *p = new double[3 * NrOfNodes()];

    switch(Order())
    {
    case 1: 
        assert(false); // not implemented yet
        break;
    case 2:
        assert(false); // not implemented yet
        break;
    case 3:
        assert(false); // not implemented yet
        break;
    default:
        assert(false);
    }

    double *v = p;

	int i;
	int j;
    for(j = 0; j < NrOfNodes(); j++)
    {
        for(i = 0; i < 3; i++)
        {
            ret.Value(i, j, *(v++));
        }
    }

    delete[] p;
    return ret;
}

std::vector<IElement::TDoubleVec> IWedge::IsoCoordinates() const
{
	std::vector<IElement::TDoubleVec> ret;

	switch(Order())
	{
	case 1:
		// node 1
		ret.push_back(MakeVec(1, 0, -1));
		// node 2
		ret.push_back(MakeVec(0, 1, -1));
		// node 3
		ret.push_back(MakeVec(0, 0, -1));
		// node 4
		ret.push_back(MakeVec(1, 0,  1));
		// node 5
		ret.push_back(MakeVec(0, 1,  1));
		// node 6
		ret.push_back(MakeVec(0, 0,  1));
		break;
	case 2:
		// node 1
		ret.push_back(MakeVec(1,   0,   -1));
		// node 2
		ret.push_back(MakeVec(0.5, 0.5, -1));
		// node 3
		ret.push_back(MakeVec(0,   1,   -1));
		// node 4
		ret.push_back(MakeVec(0,   0.5, -1));
		// node 5
		ret.push_back(MakeVec(0,   0,   -1));
		// node 6
		ret.push_back(MakeVec(0.5, 0,   -1));
		// node 7
		ret.push_back(MakeVec(1,   0,    0));
		// node 8
		ret.push_back(MakeVec(0,   1,    0));
		// node 9
		ret.push_back(MakeVec(0,   0,    0));
		// node 10
		ret.push_back(MakeVec(1,   0,    1));
		// node 11
		ret.push_back(MakeVec(0.5, 0.5,  1));
		// node 12
		ret.push_back(MakeVec(0,   1,    1));
		// node 13
		ret.push_back(MakeVec(0,   0.5,  1));
		// node 14
		ret.push_back(MakeVec(0,   0,    1));
		// node 15
		ret.push_back(MakeVec(0.5, 0,    1));
		break;
	case 3:
		// node 1
		ret.push_back(MakeVec(1,    0,    -1   ));
		// node 2
		ret.push_back(MakeVec(2./3, 1./3, -1   ));
		// node 3
		ret.push_back(MakeVec(1./3, 2./3, -1   ));
		// node 4
		ret.push_back(MakeVec(0,    1,    -1   ));
		// node 5
		ret.push_back(MakeVec(0,    2./3, -1   ));
		// node 6
		ret.push_back(MakeVec(0,    1./3, -1   ));
		// node 7
		ret.push_back(MakeVec(0,    0,    -1   ));
		// node 8
		ret.push_back(MakeVec(1./3, 0,    -1   ));
		// node 9
		ret.push_back(MakeVec(2./3, 0,    -1   ));
		// node 10
		ret.push_back(MakeVec(1,    0,    -1./3));
		// node 11
		ret.push_back(MakeVec(0,    1,    -1./3));
		// node 12
		ret.push_back(MakeVec(0,    0,    -1./3));
		// node 13
		ret.push_back(MakeVec(1,    0,     1./3));
		// node 14
		ret.push_back(MakeVec(0,    1,     1./3));
		// node 15
		ret.push_back(MakeVec(0,    0,     1./3));
		// node 16
		ret.push_back(MakeVec(1,    0,     1   ));
		// node 17
		ret.push_back(MakeVec(2./3, 1./3,  1   ));
		// node 18
		ret.push_back(MakeVec(1./3, 2./3,  1   ));
		// node 19
		ret.push_back(MakeVec(0,    1,     1   ));
		// node 20
		ret.push_back(MakeVec(0,    2./3,  1   ));
		// node 21
		ret.push_back(MakeVec(0,    1./3,  1   ));
		// node 22
		ret.push_back(MakeVec(0,    0,     1   ));
		// node 23
		ret.push_back(MakeVec(1./3, 0,     1   ));
		// node 24
		ret.push_back(MakeVec(2./3, 0,     1   ));
		break;
	default:
		assert(false);
	}

	return ret;
}

int IWedge::IntegrationPointSize() const
{
	return (int)IntegrationPoints().size();
}

const IElement::TDoubleVec& IWedge::IntegrationPointCoords(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].first;
}

const double& IWedge::IntegrationPointWeight(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].second;
}

} // namespace geo
