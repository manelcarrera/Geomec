 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _BODYLINE_H_
#define _BODYLINE_H_

#include "ILine.h"

#include "GeometryExports.h"

namespace geo {

class IBody;

class GEOMETRY_EXPORT  CBodyLine : public ILine
{
	IBody &m_parent;		// the parent body
	int    m_nIndex;		// the index in the parent body

public:
	CBodyLine(IBody& parent, int nIndex);
	virtual ~CBodyLine();

	const IBody& localParent() const;
	IBody& localParent();

	virtual size_t NrOfNodes() const;
	virtual const INode& Node(int nIndex) const;
	virtual void Node(int nIndex, const IPoint& point);
	virtual int PointIndex(int nIndex) const;

	virtual size_t Order() const;

	int BodyPoint(int nIndex) const;
	int BodyNode(int nIndex) const;
};

} // namespace geo

#endif //_BODYLINE_H_
