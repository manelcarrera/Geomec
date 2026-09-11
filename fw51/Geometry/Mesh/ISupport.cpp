 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ISupport.cpp: implementation of the ISupport class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "IMesh.h"
#include "ISupport.h"
#include "INode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

// ISupport class
ISupport::ISupport(IMesh& mesh, int nLimitedNode, const IVector &direction)
:	m_mesh(mesh), m_nNode(nLimitedNode)
{
	m_Base.AddVector(direction);
}

ISupport::~ISupport()
{
}

const IMesh &ISupport::Mesh() const
{
	return m_mesh;
}

const INode &ISupport::Node() const
{
	return Mesh().Node(NodeIndex());
}

int ISupport::NodeIndex() const
{
	return m_nNode;
}

bool ISupport::AddDirection(const IVector &dir)
{
	return m_Base.AddVector(dir);
}

size_t ISupport::DirectionSize() const
{
	return m_Base.Size();
}

const IVector &ISupport::Direction(size_t nIndex) const
{
	return m_Base.Direction(nIndex);
}

CVector ISupport::Projection(const IVector &dir) const
{
	return m_Base.Projection(dir);
}

bool ISupport::IsSupportedDirection(const IVector &dir) const
{
	return m_Base.Dependent(dir);
}

void ISupport::AlignWith(const IVector &dir) const
{
	m_Base.AlignWith(dir);
}

// CTranslationSupport class
CTranslationSupport::CTranslationSupport(IMesh& mesh, int nLimitedNode, const IVector &direction)
:	ISupport(mesh, nLimitedNode, direction)
{
}

CTranslationSupport::~CTranslationSupport()
{
}

std::string CTranslationSupport::Type() const
{
	return std::string("TR");
}

// CRotationSupport class
CRotationSupport::CRotationSupport(IMesh& mesh, int nLimitedNode, const IVector &direction)
:	ISupport(mesh, nLimitedNode, direction)
{
}

CRotationSupport::~CRotationSupport()
{
}

std::string CRotationSupport::Type() const
{
	return std::string("RO");
}

} // namespace geo
