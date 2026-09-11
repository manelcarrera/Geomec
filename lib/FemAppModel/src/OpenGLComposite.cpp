// OpenGLComposite.cpp: implementation of the COpenGLComposite class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenGLComposite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLComposite::COpenGLComposite(CFemAppModel& model)
: COpenGLNodeBase(model)
{
}

COpenGLComposite::COpenGLComposite(const QString &strName, CFemAppModel& model)
: COpenGLNodeBase(strName, model)
{
}

COpenGLComposite::COpenGLComposite(const unsigned int uName, CFemAppModel& model)
: COpenGLNodeBase(uName, model)
{
}

COpenGLComposite::COpenGLComposite(const COpenGLNodeBase &rhs)
: COpenGLNodeBase(rhs)
{
}

COpenGLComposite& 	COpenGLComposite::operator=(const COpenGLComposite &rhs)
{
	COpenGLNodeBase::operator =(rhs);
	return *this;
}

bool COpenGLComposite::operator==(const COpenGLComposite &rhs) const
{
	return COpenGLNodeBase::operator ==(rhs);
}