 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
//#include "IDrawDef.h"
//#include "DrawDef.h"
#include "MouseListener.h"
#include "IOpenGLFrame.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////
// Implementation of contraints
///////////////////////////////////////////////////////////////////////
bool CInnerConstraint::ValidatePoint(const geo::IPoint &pt) const
{
	return !m_constraint.Contains(pt,true);
}

bool COuterConstraint::ValidatePoint(const geo::IPoint &pt) const
{
	return m_constraint.Contains(pt,true);
}

bool CCrossingConstraint::ValidateLine(const geo::ILine &newline) const
{
	bool ret=true;
	//the new line must also not crossing the inner poly (if concave)
	for(int k=0;k<m_constraint.NrOfLines();k++)
	{
		geo::CPoint pi =  newline.Intersection(m_constraint.Line(k));
		if(!pi.Empty())
			if(newline.Contains(pi) && m_constraint.Line(k).Contains(pi))
				ret = false;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseListener::CMouseListener(TFrame& frame)
:m_frame(frame), m_bDeleting(false)
{
	// Pop our selves on the view stack
	m_frame.m_skMouseListener.push(this);
}


CMouseListener::~CMouseListener()
{
	// Mouse listners can only be deleted by OK() or Cancel().
	// Don't call directly the destructor
	DIA_ASSERT(m_bDeleting);
	DIA_ASSERT(m_frame.m_skMouseListener.top() == this);
	m_frame.m_skMouseListener.pop();

	// Delete the constraints
	for(size_t i = 0; i < m_vcConstraints.size(); i++)
		delete m_vcConstraints[i];

	m_vcConstraints.clear();
}

bool CMouseListener::ValidatePoint(const geo::IPoint& point) const
{
	for(int i = 0; i < ConstraintSize(); i++)
	{
		if(!Constraint(i).ValidatePoint(point))
			return false;
	}

	return true;
}

bool CMouseListener::ValidateLine(const geo::ILine &line) const
{
	// Validate each point ...
	if(!ValidatePoint(line.First()))
		return false;
	if(!ValidatePoint(line.Second()))
		return false;

	// Validate each line
	for(int i = 0; i < ConstraintSize(); i++)
	{
		if(!Constraint(i).ValidateLine(line))
			return false;
	}

	return true;	
}

int CMouseListener::ConstraintSize() const
{
	return (int)m_vcConstraints.size();
}

const CConstraint& CMouseListener::Constraint(int nIndex) const
{
	DIA_ASSERT(nIndex >= 0);
	DIA_ASSERT(nIndex < ConstraintSize());
	return *m_vcConstraints[nIndex];
}

void CMouseListener::OnSetCursor(const geo::IPoint& /*point*/, bool bValidLocation)
{
	if(bValidLocation)
		Frame().SetCursor(Qt::CrossCursor);
	else
		Frame().SetCursor(Qt::ForbiddenCursor);
}

geo::CPoint CMouseListener::OnSetDragPoint(const geo::IPoint& ptDrag)
{
	return ptDrag;
}

void CMouseListener::PushBack(CConstraint *ptr) 
{
	m_vcConstraints.push_back(ptr);
}

void CMouseListener::Cancel()
{
	m_bDeleting = OnCancel();
	if(m_bDeleting)
		delete this;
}

void CMouseListener::OK()
{
	m_bDeleting = OnOK();
	if(m_bDeleting)
		delete this;
}

void CMouseListener::Delete()
{
	m_bDeleting = OnDelete();
	if(m_bDeleting)
		delete this;
}

// Pre	: Cancel() is called
// Post	: Return value
//			TRUE	Mouselistener is deleted
//			FALSE	Mouselistener is not deleted Cancel is not procedeed  
bool CMouseListener::OnCancel()
{
	return true;
}

// Pre	: OK() is called
// Post	: Return value
//			TRUE	Mouselistener is deleted
//			FALSE	Mouselistener is not deleted OK is not procedeed  
bool CMouseListener::OnOK()
{
	return true;
}

// Pre	: Delete() is called
// Post	: Return value
//			TRUE	Mouselistener is deleted
//			FALSE	Mouselistener is not deleted operation Delete is not procedeed  
bool CMouseListener::OnDelete()
{
	return true;
}

const CMouseListener::TFrame& CMouseListener::Frame() const
{
	return m_frame;
}

CMouseListener::TFrame& CMouseListener::Frame()
{
	return m_frame;
}

bool CMouseListener::MousePress(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return false;
}

bool CMouseListener::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return false;
}

bool CMouseListener::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return false;
}

bool CMouseListener::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint& /*point*/)
{
	return false;
}

bool CMouseListener::MouseMove(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return false;
}

bool CMouseListener::KeyPress(TKeyboardModifiers /*state*/, int /*nAscii*/, TKey /*key*/, int /*nRepCount*/)
{
	return false;
}

bool CMouseListener::KeyRelease(TKeyboardModifiers /*state*/, int /*nAscii*/, TKey /*key*/, int /*nRepCount*/)
{
	return false;
}


