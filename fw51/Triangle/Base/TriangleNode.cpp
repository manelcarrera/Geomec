/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMeshNode.cpp: implementation of the CTriangleNode class.
//
//////////////////////////////////////////////////////////////////////
typedef double REAL;

#include "TriangleNode.h"
#include "TriangleSurface.h"
#include "tri_triangle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static double zero = 0;

CTriangleNode::CTriangleNode(CTriangleSurface &surface, int nIndex, double *coord)
    : m_nIndex(nIndex), m_surface(surface), m_pCoord(coord) {
  assert(m_pCoord);
}

CTriangleNode::~CTriangleNode() {}

const double &CTriangleNode::X() const { return m_pCoord[0]; }

const double &CTriangleNode::Y() const { return m_pCoord[1]; }

const double &CTriangleNode::Z() const { return zero; }

void CTriangleNode::X(const double & /*dX*/) { assert(false); }

void CTriangleNode::Y(const double & /*dY*/) { assert(false); }

void CTriangleNode::Z(const double & /*dZ*/) { assert(false); }

void CTriangleNode::AssertValid() const {}

bool CTriangleNode::Empty() const { return false; }
