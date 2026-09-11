// OpenGLNodeBase.cpp: implementation of the COpenGLNodeBaseBase class.
//
//////////////////////////////////////////////////////////////////////
#include "OpenGLNodeBase.h"
#include "FemAppEntryTypes.h"
#include "FemAppModel.h"
#include "StorageNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLNodeBase::COpenGLNodeBase(CFemAppModel &model) : CStorageNode(model) {}

COpenGLNodeBase::COpenGLNodeBase(const QString &strName, CFemAppModel &model) : CStorageNode(strName, model) {
  TOpenGLNodeBaseEntry *pEntry = (TOpenGLNodeBaseEntry *)Model().GraphEntry(MD_BASE_OPENGL_NODE);
  assert(pEntry);
  LinkTo(*pEntry);
}

COpenGLNodeBase::COpenGLNodeBase(const unsigned int uName, CFemAppModel &model) : CStorageNode(uName, model) {
  TOpenGLNodeBaseEntry *pEntry = (TOpenGLNodeBaseEntry *)Model().GraphEntry(MD_BASE_OPENGL_NODE);
  assert(pEntry);
  LinkTo(*pEntry);
}

COpenGLNodeBase::COpenGLNodeBase(const COpenGLNodeBase &rhs) : CStorageNode(rhs) {}

COpenGLNodeBase &COpenGLNodeBase::operator=(const COpenGLNodeBase &rhs) {
  CStorageNode::operator=(rhs);
  return *this;
}

bool COpenGLNodeBase::operator==(const COpenGLNodeBase &rhs) const { return CStorageNode::operator==(rhs); }

void COpenGLNodeBase::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CStorageNode::LoadStream(stream, version, progress);
  TOpenGLNodeBaseEntry *pEntry = (TOpenGLNodeBaseEntry *)Model().GraphEntry(MD_BASE_OPENGL_NODE);
  assert(pEntry);
  LinkTo(*pEntry);
}
