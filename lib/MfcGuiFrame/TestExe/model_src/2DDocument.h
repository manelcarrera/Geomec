// 2DDocument.h: interface for the C2DDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DDOCUMENT_H__E2C68A2A_775B_40C0_BCCD_EB6471E1DADB__INCLUDED_)
#define AFX_2DDOCUMENT_H__E2C68A2A_775B_40C0_BCCD_EB6471E1DADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "2DVertex.h"
#include "2DPolyLine.h"
#include "2DBody.h"
#include "DocumentBase.h"

class C2DSegmentContainer;
class C2DDocument : public CDocumentBase
{
	C2DVertexContainer* m_pVertices;
	C2DSegmentContainer* m_pSegments;
	T2DPolyLineContainer* m_pPolyLines;
	T2DBodyContainer* m_pBodies;
protected:
	virtual void createContainers();
	virtual void createDefaults();
	void updateBodies();
	std::set<IModelObject*> Identifier(const std::vector<const C2DVertex*>& vcVertex) const;
public:
	C2DDocument();
	virtual ~C2DDocument();

	// Containers
	C2DVertexContainer& Vertices() { return *m_pVertices; }
	C2DSegmentContainer& Segments() { return *m_pSegments; }
	T2DPolyLineContainer& PolyLines() { return *m_pPolyLines; }
	T2DBodyContainer& Bodies() { return *m_pBodies; }


	virtual QString documentType() const;
	virtual CVersion documentVersion() const;
};

#endif // !defined(AFX_2DDOCUMENT_H__E2C68A2A_775B_40C0_BCCD_EB6471E1DADB__INCLUDED_)
