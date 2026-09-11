// OpenGLComposite.h: interface for the COpenGLComposite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLCOMPOSITE_H__1DD6BF9D_1F43_4B3D_8EC2_20C5137A637D__INCLUDED_)
#define AFX_OPENGLCOMPOSITE_H__1DD6BF9D_1F43_4B3D_8EC2_20C5137A637D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLNodeBase.h"

class COpenGLComposite : public COpenGLNodeBase {
public:
  typedef std::set<COpenGLNodeBase *> TOpenGLNodeSet;
  COpenGLComposite(CFemAppModel &model);
  COpenGLComposite(const QString &strName, CFemAppModel &model);
  COpenGLComposite(const unsigned int uName, CFemAppModel &model);
  COpenGLComposite(const COpenGLNodeBase &rhs);
  virtual TOpenGLNodeSet OpenGLNodes() const = 0;

  // Equal and assignment
  COpenGLComposite &operator=(const COpenGLComposite &rhs);
  bool operator==(const COpenGLComposite &rhs) const;

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitOpenGLComposite(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitOpenGLComposite(*this); }
};

#endif // !defined(AFX_OPENGLCOMPOSITE_H__1DD6BF9D_1F43_4B3D_8EC2_20C5137A637D__INCLUDED_)
