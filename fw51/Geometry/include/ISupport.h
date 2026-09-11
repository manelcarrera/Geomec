 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ISupport.h: interface for the ISupport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISUPPORT_H__F9016FE9_AE84_49C3_8212_074C32FB3EE7__INCLUDED_)
#define AFX_ISUPPORT_H__F9016FE9_AE84_49C3_8212_074C32FB3EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OrthogonalBase.h"

#include "GeometryExports.h"

namespace geo {
class IMesh;
class CMesh;
class INode;
class IVector;
class GEOMETRY_EXPORT ISupport
{
	IMesh &m_mesh;
	int m_nNode;
	mutable COrthogonalBase m_Base;

protected:
	ISupport(IMesh& mesh, int nLimitedNode, const IVector &direction);

public:
	virtual ~ISupport();

	const IMesh &Mesh() const;

	const INode &Node() const;
	int NodeIndex() const;

	bool AddDirection(const IVector &dir);

  size_t DirectionSize() const;
  const geo::IVector &Direction(size_t nIndex) const;

	virtual std::string Type() const = 0;

	CVector Projection(const IVector &dir) const;
	bool IsSupportedDirection(const IVector &dir) const;
	void AlignWith(const IVector &dir) const;
};

class GEOMETRY_EXPORT  CTranslationSupport : public ISupport
{
	friend class IMesh;

private:
	// private constructor: derived class is not possible due to creation by IMesh
	CTranslationSupport(IMesh& mesh, int nLimitedNode, const IVector &direction);

public:
	virtual ~CTranslationSupport();

	virtual std::string Type() const;
};

class GEOMETRY_EXPORT  CRotationSupport : public ISupport
{
	friend class IMesh;

private:
	// private constructor: derived class is not possible due to creation by IMesh
	CRotationSupport(IMesh& mesh, int nLimitedNode, const IVector &direction);

public:
	virtual ~CRotationSupport();

	virtual std::string Type() const;
};

}

#endif // !defined(AFX_ISUPPORT_H__F9016FE9_AE84_49C3_8212_074C32FB3EE7__INCLUDED_)
