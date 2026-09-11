 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// GoCadVolume.cpp: implementation of the CGoCadVolume class.
//
//////////////////////////////////////////////////////////////////////
#include "IMesh.h"
#include "Tetrahedron.h"
#include "FvGocadHeader.h"
#include "FvGocadVolume.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGoCadVolume::CGoCadVolume(geo::IMesh& mesh)
: CBodyGroup(mesh), m_pHeader(0)
{
	
}

CGoCadVolume::~CGoCadVolume()
{
}

void CGoCadVolume::Header(const QString &sName)
{
	m_pHeader = new CGoCadHeader(sName);
}

bool CGoCadVolume::InsertPoint(const int nIndex, const double &dX, const double &dY, const double &dZ)
{
	int nRegInd = Mesh().RegisterNode(geo::CPoint(dX, dY , dZ), false);

	if(!m_mpPtInd.insert(TPtIndexMap::value_type(nIndex, nRegInd)).second)
		return false;

  m_nodes.push_back( nRegInd );
	return true;
}

bool CGoCadVolume::isPoint(int nIndex) const
{
  return m_mpPtInd.find(nIndex) != m_mpPtInd.end();
}

int CGoCadVolume::InsertTetrahedron(const int nIndex1, const int nIndex2, const int nIndex3, const int nIndex4)
{
	int p1 = (*m_mpPtInd.find(nIndex1)).second;
	int p2 = (*m_mpPtInd.find(nIndex2)).second;
	int p3 = (*m_mpPtInd.find(nIndex3)).second;
	int p4 = (*m_mpPtInd.find(nIndex4)).second;

	const geo::IPoint &pt1 = Mesh().Point(p1);
	const geo::IPoint &pt2 = Mesh().Point(p2);
	const geo::IPoint &pt3 = Mesh().Point(p3);
	const geo::IPoint &pt4 = Mesh().Point(p4);

	// we need to prevent negative volumes
	geo::CVector v1(pt2 - pt1);
	geo::CVector v2(pt3 - pt2);
	geo::CVector v3(pt4 - pt2);
	double dVol = (v1.DotProduct(v2.CrossProduct(v3))) / 6;

	geo::CTetrahedron *Tet;

	if(dVol > 0) Tet = new geo::CTetrahedron(Mesh(), p1, p2, p3, p4);
	else         Tet = new geo::CTetrahedron(Mesh(), p2, p1, p3, p4);

	AddBody(*Tet);																		   

	return Tet->Index();
}

int CGoCadVolume::VolumeIndex(const int nFileIndex) const
{
	TPtIndexMap::const_iterator it = m_mpPtInd.find(nFileIndex);
	assert(it != m_mpPtInd.end());

	return it->second;
}

int CGoCadVolume::PointSize() const
{
  return m_nodes.size();
}

const geo::IPoint &CGoCadVolume::Point(int nIndex) const
{
  assert( nIndex >= 0 && nIndex < PointSize() );
  return Mesh().Point( m_nodes[nIndex] );
}

// The nIndex is the index of the point as defined in the gocad-file
const geo::IPoint& CGoCadVolume::GetGoCadPoint( int nIndex ) const
{
  int meshIndex = VolumeIndex( nIndex );
  return Mesh().Point( meshIndex );
}
