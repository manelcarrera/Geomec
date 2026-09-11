// EclipseFormation.h: interface for the CEclipseFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECLIPSEFORMATION_H__C87C300F_8695_4CB7_B804_1DA38F63584E__INCLUDED_)
#define AFX_ECLIPSEFORMATION_H__C87C300F_8695_4CB7_B804_1DA38F63584E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FormationBase.h"
#include "OpenGLNodeObserver.h"

class CEclipseModel;
class CEclipseFormation : public CFormationBase
{
	CEclipseReservoir m_reservoir;
	CEclipseFormation* m_pUpper;
	CEclipseFormation* m_pLower;
	geo::CArray<CEclipseCell> m_arView;
public:
	CEclipseFormation(CEclipseModel& model, const CEclipseReservoir& reservoir, CEclipseFormation* pUpper = 0);
	virtual ~CEclipseFormation();
	virtual UINT TypeNameId() const;
	virtual const geo::IArray& DisplayList() const;
	const CEclipseReservoir& Reservoir() const;
	virtual void Properties();
	virtual int ElementSize() const;
	virtual const geo::IBody &Element(int index) const;
};

typedef CGraphEntryTemp<CEclipseFormation> TEclipseFormationEntry;

typedef COpenGLNodeObserver<CEclipseFormation, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TEclipseFormationObserver;
class CEclipseFormationEntryObserver : public CStateBranch<TEclipseFormationEntry, CEclipseFormation, TEclipseFormationObserver, TRUE, FIXED_ITEM>
{
public:
	CEclipseFormationEntryObserver(TEclipseFormationEntry& entry, CGraphTreeView &view, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	virtual CTreeNode* InsertChild(CEclipseFormation& t);
};

#endif // !defined(AFX_ECLIPSEFORMATION_H__C87C300F_8695_4CB7_B804_1DA38F63584E__INCLUDED_)
