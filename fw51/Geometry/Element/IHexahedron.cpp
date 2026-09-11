 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IHexahedron.h"
#include "BodyQuadrilateral.h"
#include "BodyLine.h"
#include <cmath>
#include "lbel.h"

#include <tbb/spin_rw_mutex.h>

namespace {

// See comment in IFace

tbb::spin_rw_mutex myGlobalIHexahedronMutex[16];

}


namespace geo {

static const int quad_point_indices[] =
{
	0,
	2,
	4,
	6,
	12,
	14,
	16,
	18
};

static const int cubic_point_indices[] =
{
	0,
	3,
	6,
	9,
	20,
	23,
	26,
	29
};

void IHexahedron::BuildIntegrationPoints( IElement::TIntPtVec& vec,
                                          int                  numint )
{
	int ixi, ieta, izeta;
	double xxi, xeta, xzeta;
	double wxi, weta, wzeta;

    for(ixi = 0; ixi < numint; ixi++) {
		GaussCoeff(ixi + 1, numint, &xxi, &wxi);
        for(ieta = 0; ieta < numint; ieta++) {
			GaussCoeff(ieta + 1, numint, &xeta, &weta);
            for(izeta = 0; izeta < numint; izeta++)	{
				GaussCoeff(izeta + 1, numint, &xzeta, &wzeta);
				std::pair<IElement::TDoubleVec, double> prGaussp;
				prGaussp.first.push_back(xxi);
				prGaussp.first.push_back(xeta);
				prGaussp.first.push_back(xzeta);
				prGaussp.second = wxi * weta * wzeta;
				vec.push_back(prGaussp);
			}
		}
	}
}

const IElement::TIntPtVec& IHexahedron::IntegrationPoints( int order )
{
    static TIntPtVec s_LinearIntegrationPoints;
    static TIntPtVec s_QuadIntegrationPoints;
    static TIntPtVec s_CubicIntegrationPoints;

    switch( order )	{
	case 1:
        if ( s_LinearIntegrationPoints.empty() )
            BuildIntegrationPoints( s_LinearIntegrationPoints, 1 );
		return s_LinearIntegrationPoints;
		break;
	case 2:
        if ( s_QuadIntegrationPoints.empty() )
            BuildIntegrationPoints( s_QuadIntegrationPoints, 2 );
		return s_QuadIntegrationPoints;
		break;
	case 3:
        if ( s_CubicIntegrationPoints.empty() )
            BuildIntegrationPoints( s_CubicIntegrationPoints, 3 );
		return s_CubicIntegrationPoints;
		break;
	}

	assert(false);
	TIntPtVec* pBogus = 0;
	return *pBogus;
}

const IElement::TIntPtVec& IHexahedron::IntegrationPoints() const
{
    return IntegrationPoints( Order() );
}

void IHexahedron::PrepareMapping()
{
  for (int i = 1; i < 4; ++i)
    IntegrationPoints(i);
}

IHexahedron::IHexahedron()
{
}

IHexahedron::~IHexahedron()
{
	int i;
	for(i = 0; i < m_vcFaces.size(); i++)
		if(m_vcFaces[i]) delete m_vcFaces[i];

	for(i = 0; i < m_vcLines.size(); i++)
		if(m_vcLines[i]) delete m_vcLines[i];
}

const IPoint &IHexahedron::Point(int nIndex) const
{
	return Node( nIndex );
#if 0
	assert(nIndex >= 0 && nIndex < NrOfPoints());
    switch( Order() ) {
    case 1:	return Node(nIndex);
    case 2:	return Node(quad_point_indices[nIndex]);
    case 3:	return Node(cubic_point_indices[nIndex]);
	}

	assert(false);
	const IPoint* pBogus = 0;
	return *pBogus;
#endif
}

void IHexahedron::Point(int nIndex, const IPoint &pt)
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

int IHexahedron::NrOfPoints() const
{
	return NrOfNodes();
//	return 8;
}

int IHexahedron::NrOfFaces() const
{
	return 6;
}

const IFace &IHexahedron::Face(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfFaces());

  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalIHexahedronMutex[index], false);

    if (m_vcFaces.empty())
    {
      lock.upgrade_to_writer();

      if (m_vcFaces.empty()) m_vcFaces.resize(6, 0);
      if (!m_vcFaces[nIndex]) m_vcFaces[nIndex] = new CBodyQuadrilateral(*const_cast<IHexahedron*>(this), nIndex);
    }
    else if (!m_vcFaces[nIndex])
    {
      lock.upgrade_to_writer();

      if (!m_vcFaces[nIndex]) m_vcFaces[nIndex] = new CBodyQuadrilateral(*const_cast<IHexahedron*>(this), nIndex);
    }
  }

	return *m_vcFaces[nIndex];
}

const IHexahedron::TIndexVec& IHexahedron::FacePointIndices(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfFaces());
    return FaceNodeIndices( 1, nIndex );
}

void IHexahedron::InitFaceNodeIndices( std::vector<TFaceNodeVec>& FaceNodeIdxs )
{
    assert( FaceNodeIdxs.size() == 0 );  // Init once ...

    FaceNodeIdxs.resize(3, TFaceNodeVec(6) );

	// Initialise for First Order elements
  // Face 0 - ZETA1
    FaceNodeIdxs[0][0].resize(4);
    FaceNodeIdxs[0][0][0] = 0;
    FaceNodeIdxs[0][0][1] = 3;
    FaceNodeIdxs[0][0][2] = 2;
    FaceNodeIdxs[0][0][3] = 1;
  // Face 1 - ETA1
    FaceNodeIdxs[0][1].resize(4);
    FaceNodeIdxs[0][1][0] = 0;
    FaceNodeIdxs[0][1][1] = 1;
    FaceNodeIdxs[0][1][2] = 5;
    FaceNodeIdxs[0][1][3] = 4;
  // Face 2 - KSI2
    FaceNodeIdxs[0][2].resize(4);
    FaceNodeIdxs[0][2][0] = 1;
    FaceNodeIdxs[0][2][1] = 2;
    FaceNodeIdxs[0][2][2] = 6;
    FaceNodeIdxs[0][2][3] = 5;
  // Face 3 - ETA2
    FaceNodeIdxs[0][3].resize(4);
    FaceNodeIdxs[0][3][0] = 2;
    FaceNodeIdxs[0][3][1] = 3;
    FaceNodeIdxs[0][3][2] = 7;
    FaceNodeIdxs[0][3][3] = 6;
  // Face 4 - KSI1
    FaceNodeIdxs[0][4].resize(4);
    FaceNodeIdxs[0][4][0] = 0;
    FaceNodeIdxs[0][4][1] = 4;
    FaceNodeIdxs[0][4][2] = 7;
    FaceNodeIdxs[0][4][3] = 3;
  // Face 5 - ZETA2
    FaceNodeIdxs[0][5].resize(4);
    FaceNodeIdxs[0][5][0] = 4;
    FaceNodeIdxs[0][5][1] = 5;
    FaceNodeIdxs[0][5][2] = 6;
    FaceNodeIdxs[0][5][3] = 7;

	// Initialise Second oder elements
  // Face 0 - ZETA1
    FaceNodeIdxs[1][0].resize(8);
    FaceNodeIdxs[1][0][0] = 0;
    FaceNodeIdxs[1][0][1] = 7;
    FaceNodeIdxs[1][0][2] = 6;
    FaceNodeIdxs[1][0][3] = 5;
    FaceNodeIdxs[1][0][4] = 4;
    FaceNodeIdxs[1][0][5] = 3;
    FaceNodeIdxs[1][0][6] = 2;
    FaceNodeIdxs[1][0][7] = 1;
  // Face 1 - ETA1
    FaceNodeIdxs[1][1].resize(8);
    FaceNodeIdxs[1][1][0] = 0;
    FaceNodeIdxs[1][1][1] = 1;
    FaceNodeIdxs[1][1][2] = 2;
    FaceNodeIdxs[1][1][3] = 9;
    FaceNodeIdxs[1][1][4] = 14;
    FaceNodeIdxs[1][1][5] = 13;
    FaceNodeIdxs[1][1][6] = 12;
    FaceNodeIdxs[1][1][7] = 8;
  // Face 2 - KSI2
    FaceNodeIdxs[1][2].resize(8);
    FaceNodeIdxs[1][2][0] = 2;
    FaceNodeIdxs[1][2][1] = 3;
    FaceNodeIdxs[1][2][2] = 4;
    FaceNodeIdxs[1][2][3] = 10;
    FaceNodeIdxs[1][2][4] = 16;
    FaceNodeIdxs[1][2][5] = 15;
    FaceNodeIdxs[1][2][6] = 14;
    FaceNodeIdxs[1][2][7] = 9;
  // Face 3 - ETA2
    FaceNodeIdxs[1][3].resize(8);
    FaceNodeIdxs[1][3][0] = 4;
    FaceNodeIdxs[1][3][1] = 5;
    FaceNodeIdxs[1][3][2] = 6;
    FaceNodeIdxs[1][3][3] = 11;
    FaceNodeIdxs[1][3][4] = 18;
    FaceNodeIdxs[1][3][5] = 17;
    FaceNodeIdxs[1][3][6] = 16;
    FaceNodeIdxs[1][3][7] = 10;
  // Face 4 - KSI1
    FaceNodeIdxs[1][4].resize(8);
    FaceNodeIdxs[1][4][0] = 0;
    FaceNodeIdxs[1][4][1] = 8;
    FaceNodeIdxs[1][4][2] = 12;
    FaceNodeIdxs[1][4][3] = 19;
    FaceNodeIdxs[1][4][4] = 18;
    FaceNodeIdxs[1][4][5] = 11;
    FaceNodeIdxs[1][4][6] = 6;
    FaceNodeIdxs[1][4][7] = 7;
  // Face 5 - ZETA2
    FaceNodeIdxs[1][5].resize(8);
    FaceNodeIdxs[1][5][0] = 12;
    FaceNodeIdxs[1][5][1] = 13;
    FaceNodeIdxs[1][5][2] = 14;
    FaceNodeIdxs[1][5][3] = 15;
    FaceNodeIdxs[1][5][4] = 16;
    FaceNodeIdxs[1][5][5] = 17;
    FaceNodeIdxs[1][5][6] = 18;
    FaceNodeIdxs[1][5][7] = 19;

	// Initialise for third order indices
	// Face 0
    FaceNodeIdxs[2][0].resize(12);
    FaceNodeIdxs[2][0][0] = 0;
    FaceNodeIdxs[2][0][1] = 11;
    FaceNodeIdxs[2][0][2] = 10;
    FaceNodeIdxs[2][0][3] = 9;
    FaceNodeIdxs[2][0][4] = 8;
    FaceNodeIdxs[2][0][5] = 7;
    FaceNodeIdxs[2][0][6] = 6;
    FaceNodeIdxs[2][0][7] = 5;
    FaceNodeIdxs[2][0][8] = 4;
    FaceNodeIdxs[2][0][9] = 3;
    FaceNodeIdxs[2][0][10] = 2;
    FaceNodeIdxs[2][0][11] = 1;
	// Face 1
    FaceNodeIdxs[2][1].resize(12);
    FaceNodeIdxs[2][1][0] = 0;
    FaceNodeIdxs[2][1][1] = 1;
    FaceNodeIdxs[2][1][2] = 2;
    FaceNodeIdxs[2][1][3] = 3;
    FaceNodeIdxs[2][1][4] = 13;
    FaceNodeIdxs[2][1][5] = 17;
    FaceNodeIdxs[2][1][6] = 23;
    FaceNodeIdxs[2][1][7] = 22;
    FaceNodeIdxs[2][1][8] = 21;
    FaceNodeIdxs[2][1][9] = 20;
    FaceNodeIdxs[2][1][10] = 16;
    FaceNodeIdxs[2][1][11] = 12;
	// Face 2
    FaceNodeIdxs[2][2].resize(12);
    FaceNodeIdxs[2][2][0] = 3;
    FaceNodeIdxs[2][2][1] = 4;
    FaceNodeIdxs[2][2][2] = 5;
    FaceNodeIdxs[2][2][3] = 6;
    FaceNodeIdxs[2][2][4] = 14;
    FaceNodeIdxs[2][2][5] = 18;
    FaceNodeIdxs[2][2][6] = 26;
    FaceNodeIdxs[2][2][7] = 25;
    FaceNodeIdxs[2][2][8] = 24;
    FaceNodeIdxs[2][2][9] = 23;
    FaceNodeIdxs[2][2][10] = 17;
    FaceNodeIdxs[2][2][11] = 13;
	// Face 3
    FaceNodeIdxs[2][3].resize(12);
    FaceNodeIdxs[2][3][0] = 6;
    FaceNodeIdxs[2][3][1] = 7;
    FaceNodeIdxs[2][3][2] = 8;
    FaceNodeIdxs[2][3][3] = 9;
    FaceNodeIdxs[2][3][4] = 15;
    FaceNodeIdxs[2][3][5] = 19;
    FaceNodeIdxs[2][3][6] = 29;
    FaceNodeIdxs[2][3][7] = 28;
    FaceNodeIdxs[2][3][8] = 27;
    FaceNodeIdxs[2][3][9] = 26;
    FaceNodeIdxs[2][3][10] = 18;
    FaceNodeIdxs[2][3][11] = 14;
	// Face 4
    FaceNodeIdxs[2][4].resize(12);
    FaceNodeIdxs[2][4][0] = 0;
    FaceNodeIdxs[2][4][1] = 12;
    FaceNodeIdxs[2][4][2] = 16;
    FaceNodeIdxs[2][4][3] = 20;
    FaceNodeIdxs[2][4][4] = 31;
    FaceNodeIdxs[2][4][5] = 30;
    FaceNodeIdxs[2][4][6] = 29;
    FaceNodeIdxs[2][4][7] = 19;
    FaceNodeIdxs[2][4][8] = 15;
    FaceNodeIdxs[2][4][9] = 9;
    FaceNodeIdxs[2][4][10] = 10;
    FaceNodeIdxs[2][4][11] = 11;
	// Face 5
    FaceNodeIdxs[2][5].resize(12);
    FaceNodeIdxs[2][5][0] = 20;
    FaceNodeIdxs[2][5][1] = 21;
    FaceNodeIdxs[2][5][2] = 22;
    FaceNodeIdxs[2][5][3] = 23;
    FaceNodeIdxs[2][5][4] = 24;
    FaceNodeIdxs[2][5][5] = 25;
    FaceNodeIdxs[2][5][6] = 26;
    FaceNodeIdxs[2][5][7] = 27;
    FaceNodeIdxs[2][5][8] = 28;
    FaceNodeIdxs[2][5][9] = 29;
    FaceNodeIdxs[2][5][10] = 30;
    FaceNodeIdxs[2][5][11] = 31;
}

const IHexahedron::TIndexVec& IHexahedron::FaceNodeIndices( int order,
                                                            int nIndex )
{
    static std::vector<TFaceNodeVec> s_FaceNodeIndices;

    //	assert(nIndex >= 0 && nIndex < NrOfFaces());
    if( s_FaceNodeIndices.empty() ) InitFaceNodeIndices( s_FaceNodeIndices );

    return s_FaceNodeIndices[order-1][nIndex];
}

const IHexahedron::TIndexVec& IHexahedron::FaceNodeIndices( int nIndex ) const
{
    return FaceNodeIndices( Order(), nIndex );
}

int IHexahedron::NrOfLines() const
{
	return 12;
}

const ILine &IHexahedron::Line(int nIndex) const
{
  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalIHexahedronMutex[index], false);

    if (m_vcLines.empty())
    {
      lock.upgrade_to_writer();

      if (m_vcLines.empty()) m_vcLines.resize(NrOfLines(), 0);

      if (!m_vcLines[nIndex])
        m_vcLines[nIndex] = new CBodyLine(*const_cast<IHexahedron*>(this), nIndex);
    }
    else if (!m_vcLines[nIndex])
    {
      lock.upgrade_to_writer();

      if (!m_vcLines[nIndex])
        m_vcLines[nIndex] = new CBodyLine(*const_cast<IHexahedron*>(this), nIndex);
    }
  }

	return *m_vcLines[nIndex];
}

const IHexahedron::TIndexVec& IHexahedron::LinePointIndices(int nIndex) const
{
  return LineNodeIndices( 1, nIndex );
}

void IHexahedron::InitLineNodeIndices( std::vector<TLineNodeVec>& LineNodeIdxs )
{
    assert( LineNodeIdxs.size() == 0 );  // Init once ...

    LineNodeIdxs.resize(3, TLineNodeVec(12) );

	// Initialise for First Order elements
	// Line 0
    LineNodeIdxs[0][0].resize(2);
    LineNodeIdxs[0][0][0] = 0;
    LineNodeIdxs[0][0][1] = 1;
	// Line 1
    LineNodeIdxs[0][1].resize(2);
    LineNodeIdxs[0][1][0] = 1;
    LineNodeIdxs[0][1][1] = 2;
	// Line 2
    LineNodeIdxs[0][2].resize(2);
    LineNodeIdxs[0][2][0] = 2;
    LineNodeIdxs[0][2][1] = 3;
	// Line 3
    LineNodeIdxs[0][3].resize(2);
    LineNodeIdxs[0][3][0] = 3;
    LineNodeIdxs[0][3][1] = 0;
	// Line 4
    LineNodeIdxs[0][4].resize(2);
    LineNodeIdxs[0][4][0] = 0;
    LineNodeIdxs[0][4][1] = 4;
	// Line 5
    LineNodeIdxs[0][5].resize(2);
    LineNodeIdxs[0][5][0] = 1;
    LineNodeIdxs[0][5][1] = 5;
	// Line 6
    LineNodeIdxs[0][6].resize(2);
    LineNodeIdxs[0][6][0] = 2;
    LineNodeIdxs[0][6][1] = 6;
	// Line 7
    LineNodeIdxs[0][7].resize(2);
    LineNodeIdxs[0][7][0] = 3;
    LineNodeIdxs[0][7][1] = 7;
	// Line 8
    LineNodeIdxs[0][8].resize(2);
    LineNodeIdxs[0][8][0] = 4;
    LineNodeIdxs[0][8][1] = 5;
	// Line 9
    LineNodeIdxs[0][9].resize(2);
    LineNodeIdxs[0][9][0] = 5;
    LineNodeIdxs[0][9][1] = 6;
	// Line 10
    LineNodeIdxs[0][10].resize(2);
    LineNodeIdxs[0][10][0] = 6;
    LineNodeIdxs[0][10][1] = 7;
	// Line 11
    LineNodeIdxs[0][11].resize(2);
    LineNodeIdxs[0][11][0] = 7;
    LineNodeIdxs[0][11][1] = 4;

	// Initialise for second order elements
	// Line 0
    LineNodeIdxs[1][0].resize(3);
    LineNodeIdxs[1][0][0] = 0;
    LineNodeIdxs[1][0][1] = 1;
    LineNodeIdxs[1][0][2] = 2;
	// Line 1
    LineNodeIdxs[1][1].resize(3);
    LineNodeIdxs[1][1][0] = 2;
    LineNodeIdxs[1][1][1] = 3;
    LineNodeIdxs[1][1][2] = 4;
	// Line 2
    LineNodeIdxs[1][2].resize(3);
    LineNodeIdxs[1][2][0] = 4;
    LineNodeIdxs[1][2][1] = 5;
    LineNodeIdxs[1][2][2] = 6;
	// Line 3
    LineNodeIdxs[1][3].resize(3);
    LineNodeIdxs[1][3][0] = 6;
    LineNodeIdxs[1][3][1] = 7;
    LineNodeIdxs[1][3][2] = 0;
	// Line 4
    LineNodeIdxs[1][4].resize(3);
    LineNodeIdxs[1][4][0] = 0;
    LineNodeIdxs[1][4][1] = 8;
    LineNodeIdxs[1][4][2] = 12;
	// Line 5
    LineNodeIdxs[1][5].resize(3);
    LineNodeIdxs[1][5][0] = 2;
    LineNodeIdxs[1][5][1] = 9;
    LineNodeIdxs[1][5][2] = 14;
	// Line 6
    LineNodeIdxs[1][6].resize(3);
    LineNodeIdxs[1][6][0] = 4;
    LineNodeIdxs[1][6][1] = 10;
    LineNodeIdxs[1][6][2] = 16;
	// Line 7
    LineNodeIdxs[1][7].resize(3);
    LineNodeIdxs[1][7][0] = 6;
    LineNodeIdxs[1][7][1] = 11;
    LineNodeIdxs[1][7][2] = 18;
	// Line 8
    LineNodeIdxs[1][8].resize(3);
    LineNodeIdxs[1][8][0] = 12;
    LineNodeIdxs[1][8][1] = 13;
    LineNodeIdxs[1][8][2] = 14;
	// Line 9
    LineNodeIdxs[1][9].resize(3);
    LineNodeIdxs[1][9][0] = 14;
    LineNodeIdxs[1][9][1] = 15;
    LineNodeIdxs[1][9][2] = 16;
	// Line 10
    LineNodeIdxs[1][10].resize(3);
    LineNodeIdxs[1][10][0] = 16;
    LineNodeIdxs[1][10][1] = 17;
    LineNodeIdxs[1][10][2] = 18;
	// Line 11
    LineNodeIdxs[1][11].resize(3);
    LineNodeIdxs[1][11][0] = 18;
    LineNodeIdxs[1][11][1] = 19;
    LineNodeIdxs[1][11][2] = 12;

	// Initialise for Third order elements
	// Line 0
    LineNodeIdxs[2][0].resize(4);
    LineNodeIdxs[2][0][0] = 0;
    LineNodeIdxs[2][0][1] = 1;
    LineNodeIdxs[2][0][2] = 2;
    LineNodeIdxs[2][0][3] = 3;
	// Line 1
    LineNodeIdxs[2][1].resize(4);
    LineNodeIdxs[2][1][0] = 3;
    LineNodeIdxs[2][1][1] = 4;
    LineNodeIdxs[2][1][2] = 5;
    LineNodeIdxs[2][1][3] = 6;
	// Line 2
    LineNodeIdxs[2][2].resize(4);
    LineNodeIdxs[2][2][0] = 6;
    LineNodeIdxs[2][2][1] = 7;
    LineNodeIdxs[2][2][2] = 8;
    LineNodeIdxs[2][2][3] = 9;
	// Line 3
    LineNodeIdxs[2][3].resize(4);
    LineNodeIdxs[2][3][0] = 9;
    LineNodeIdxs[2][3][1] = 10;
    LineNodeIdxs[2][3][2] = 11;
    LineNodeIdxs[2][3][3] = 0;
	// Line 4
    LineNodeIdxs[2][4].resize(4);
    LineNodeIdxs[2][4][0] = 0;
    LineNodeIdxs[2][4][1] = 12;
    LineNodeIdxs[2][4][2] = 16;
    LineNodeIdxs[2][4][3] = 20;
	// Line 5
    LineNodeIdxs[2][5].resize(4);
    LineNodeIdxs[2][5][0] = 3;
    LineNodeIdxs[2][5][1] = 13;
    LineNodeIdxs[2][5][2] = 17;
    LineNodeIdxs[2][5][3] = 23;
	// Line 6
    LineNodeIdxs[2][6].resize(4);
    LineNodeIdxs[2][6][0] = 6;
    LineNodeIdxs[2][6][1] = 14;
    LineNodeIdxs[2][6][2] = 18;
    LineNodeIdxs[2][6][3] = 26;
	// Line 7
    LineNodeIdxs[2][7].resize(4);
    LineNodeIdxs[2][7][0] = 9;
    LineNodeIdxs[2][7][1] = 15;
    LineNodeIdxs[2][7][2] = 19;
    LineNodeIdxs[2][7][3] = 29;
	// Line 8
    LineNodeIdxs[2][8].resize(4);
    LineNodeIdxs[2][8][0] = 20;
    LineNodeIdxs[2][8][1] = 21;
    LineNodeIdxs[2][8][2] = 22;
    LineNodeIdxs[2][8][3] = 23;
	// Line 9
    LineNodeIdxs[2][9].resize(4);
    LineNodeIdxs[2][9][0] = 23;
    LineNodeIdxs[2][9][1] = 24;
    LineNodeIdxs[2][9][2] = 25;
    LineNodeIdxs[2][9][3] = 26;
	// Line 10
    LineNodeIdxs[2][10].resize(4);
    LineNodeIdxs[2][10][0] = 26;
    LineNodeIdxs[2][10][1] = 27;
    LineNodeIdxs[2][10][2] = 28;
    LineNodeIdxs[2][10][3] = 29;
	// Line 11
    LineNodeIdxs[2][11].resize(4);
    LineNodeIdxs[2][11][0] = 29;
    LineNodeIdxs[2][11][1] = 30;
    LineNodeIdxs[2][11][2] = 31;
    LineNodeIdxs[2][11][3] = 20;

}

const IHexahedron::TIndexVec& IHexahedron::LineNodeIndices( int order,
                                                            int nIndex )
{
  static std::vector<TLineNodeVec> s_LineNodeIndices;
  assert(nIndex >= 0 && nIndex < 12 );
  if( s_LineNodeIndices.size() == 0 ) InitLineNodeIndices( s_LineNodeIndices );
  return s_LineNodeIndices[order-1][nIndex];
}

const IHexahedron::TIndexVec& IHexahedron::LineNodeIndices( int nIndex ) const
{
    return LineNodeIndices( Order(), nIndex );
}
IElement::TDoubleVec IHexahedron::ShapeFunction(const IElement::TDoubleVec& isocoords) const
{
	assert(isocoords.size() == 3); // xi, eta and zeta coordinate

	double *values = new double[NrOfNodes()];

	switch(Order())
	{
	case 1:
		HexaHedronShape(isocoords[0], isocoords[1], isocoords[2], values);
		break;
	case 2:
		HexaHedronShapeQuadIntPol(isocoords[0], isocoords[1], isocoords[2], values);
		break;
	case 3:
		HexaHedronShapeCubicIntPol(isocoords[0], isocoords[1], isocoords[2], values);
		break;
	default:
		assert(false);
	}

	IElement::TDoubleVec vcRet(NrOfNodes());
	int i;
	for(i = 0; i < NrOfNodes(); i++) vcRet[i] = values[i];

	delete[] values;

	return vcRet;
}

CMatrix IHexahedron::ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const
{
	assert(isocoords.size() == 3);

	int i;
//	for(i = 0; i < isocoords.size(); i++)
//		assert(isocoords[i] >= -1.0 && isocoords[i] <= 1.0);

	// 3 rows, NrOfNodes() columns
	CMatrix ret(3, NrOfNodes());

	double *p = new double[3 * NrOfNodes()];

	switch(Order())
	{
	case 1:
		DerivedHexaHedronShape(isocoords[0], isocoords[1], isocoords[2], 0, p);
		break;
	case 2:
		DerivedHexaHedronShapeQuadIntPol(isocoords[0], isocoords[1], isocoords[2], p);
		break;
	case 3:
		DerivedHexaHedronShapeCubicIntPol(isocoords[0], isocoords[1], isocoords[2], p);
		break;
	default:
		assert(false);
	}

	double *v = p;

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

std::vector<IElement::TDoubleVec> IHexahedron::IsoCoordinates() const
{
	std::vector<IElement::TDoubleVec> ret;

	switch(Order())
	{
	case 1:
		// node 1
		ret.push_back(MakeVec(-1, -1, -1));
		// node 2
		ret.push_back(MakeVec( 1, -1, -1));
		// node 3
		ret.push_back(MakeVec( 1,  1, -1));
		// node 4
		ret.push_back(MakeVec(-1,  1, -1));
		// node 5
		ret.push_back(MakeVec(-1, -1,  1));
		// node 6
		ret.push_back(MakeVec( 1, -1,  1));
		// node 7
		ret.push_back(MakeVec( 1,  1,  1));
		// node 8
		ret.push_back(MakeVec(-1,  1,  1));
		break;
	case 2:
		// node 1
		ret.push_back(MakeVec(-1, -1, -1));
		// node 2
		ret.push_back(MakeVec( 0, -1, -1));
		// node 3
		ret.push_back(MakeVec( 1, -1, -1));
		// node 4
		ret.push_back(MakeVec( 1,  0, -1));
		// node 5
		ret.push_back(MakeVec( 1,  1, -1));
		// node 6
		ret.push_back(MakeVec( 0,  1, -1));
		// node 7
		ret.push_back(MakeVec(-1,  1, -1));
		// node 8
		ret.push_back(MakeVec(-1,  0, -1));
		// node 9
		ret.push_back(MakeVec(-1, -1,  0));
		// node 10
		ret.push_back(MakeVec( 1, -1,  0));
		// node 11
		ret.push_back(MakeVec( 1,  1,  0));
		// node 12
		ret.push_back(MakeVec(-1,  1,  0));
		// node 13
		ret.push_back(MakeVec(-1, -1,  1));
		// node 14
		ret.push_back(MakeVec( 0, -1,  1));
		// node 15
		ret.push_back(MakeVec( 1, -1,  1));
		// node 16
		ret.push_back(MakeVec(-1,  0,  1));
		// node 17
		ret.push_back(MakeVec( 1,  1,  1));
		// node 18
		ret.push_back(MakeVec( 1,  0,  1));
		// node 19
		ret.push_back(MakeVec(-1,  1,  1));
		// node 20
		ret.push_back(MakeVec(-1,  1,  1));
		break;
	case 3:
		// node 1
		ret.push_back(MakeVec(-1,    -1,    -1   ));
		// node 2
		ret.push_back(MakeVec(-1./3, -1,    -1   ));
		// node 3
		ret.push_back(MakeVec( 1./3, -1,    -1   ));
		// node 4
		ret.push_back(MakeVec( 1,    -1,    -1   ));
		// node 5
		ret.push_back(MakeVec( 1,    -1./3, -1   ));
		// node 6
		ret.push_back(MakeVec( 1,     1./3, -1   ));
		// node 7
		ret.push_back(MakeVec( 1,     1,    -1   ));
		// node 8
		ret.push_back(MakeVec( 1./3,  1,    -1   ));
		// node 9
		ret.push_back(MakeVec(-1./3,  1,    -1   ));
		// node 10
		ret.push_back(MakeVec(-1,     1,    -1   ));
		// node 11
		ret.push_back(MakeVec(-1,     1./3, -1   ));
		// node 12
		ret.push_back(MakeVec(-1,    -1./3, -1   ));
		// node 13
		ret.push_back(MakeVec(-1,    -1,    -1./3));
		// node 14
		ret.push_back(MakeVec( 1,    -1,    -1./3));
		// node 15
		ret.push_back(MakeVec( 1,     1,    -1./3));
		// node 16
		ret.push_back(MakeVec(-1,     1,    -1./3));
		// node 17
		ret.push_back(MakeVec(-1,    -1,     1./3));
		// node 18
		ret.push_back(MakeVec( 1,    -1,     1./3));
		// node 19
		ret.push_back(MakeVec( 1,     1,     1./3));
		// node 20
		ret.push_back(MakeVec(-1,     1,     1./3));
		// node 21
		ret.push_back(MakeVec(-1,    -1,     1   ));
		// node 22
		ret.push_back(MakeVec(-1./3, -1,     1   ));
		// node 23
		ret.push_back(MakeVec( 1./3, -1,     1   ));
		// node 24
		ret.push_back(MakeVec( 1,    -1,     1   ));
		// node 25
		ret.push_back(MakeVec( 1,    -1./3,  1   ));
		// node 26
		ret.push_back(MakeVec( 1,     1./3,  1   ));
		// node 27
		ret.push_back(MakeVec( 1,     1,     1   ));
		// node 28
		ret.push_back(MakeVec( 1./3,  1,     1   ));
		// node 29
		ret.push_back(MakeVec(-1./3,  1,     1   ));
		// node 30
		ret.push_back(MakeVec(-1,     1,     1   ));
		// node 31
		ret.push_back(MakeVec(-1,     1./3,  1   ));
		// node 32
		ret.push_back(MakeVec(-1,    -1./3,  1   ));
		break;
	default:
		assert(false);
	}

	return ret;
}

int IHexahedron::IntegrationPointSize() const
{
	return (int)IntegrationPoints().size();
}

const IElement::TDoubleVec& IHexahedron::IntegrationPointCoords(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].first;
}

const double& IHexahedron::IntegrationPointWeight(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < IntegrationPointSize());
	return IntegrationPoints()[nIndex].second;
}

} // namespace geo
