// OpenGLNodeBase.h: interface for the COpenGLNodeBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLNODEBASE_H__77A57B80_70F9_4F62_8804_0DFD189E320C__INCLUDED_)
#define AFX_OPENGLNODEBASE_H__77A57B80_70F9_4F62_8804_0DFD189E320C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QColor>

#include "StorageNode.h"

class COpenGLNodeBase : public CStorageNode   
{
public:
	COpenGLNodeBase(CFemAppModel& model);
	COpenGLNodeBase(const QString &strName, CFemAppModel& model);
	COpenGLNodeBase(const unsigned int uName, CFemAppModel& model);
	COpenGLNodeBase(const COpenGLNodeBase &rhs);

	typedef QRgb TColor;

	// Equal and assignment
	COpenGLNodeBase& operator=(const COpenGLNodeBase &rhs);
	bool operator==(const COpenGLNodeBase &rhs) const;

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitOpenGLNodeBase(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitOpenGLNodeBase(*this); }
};

typedef CGraphEntryTemp<COpenGLNodeBase> TOpenGLNodeBaseEntry;

#endif // !defined(AFX_OPENGLNODEBASE_H__77A57B80_70F9_4F62_8804_0DFD189E320C__INCLUDED_)
