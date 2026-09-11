// ScenePtr.cpp: implementation of the CScenePtr class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "openglnode.h"
#include "OpenGLSceneNode.h"
#include "unitnode.h"
#include "FemAppDoc.h"
#include "ScenePtr.h"
#include "OpenGLSceneBase.h"
#include "ISceneWrapper.h"
#include "GeomecDoc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScenePtr::CScenePtr(CFemAppDoc& doc)
: m_doc(doc)
{
}

void CScenePtr::OnNewNeighbour(const CGraphNode &node)
{
  CGraphPtr<COpenGLSceneBase>::OnNewNeighbour(node);
  if (m_doc.CurrentScene())
  {
  if (m_doc.CurrentScene()->OpenInventorScene())
    	m_doc.UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject*)( m_doc.CurrentScene()->OpenInventorScene() ) );
  else
    	m_doc.UpdateAllViews(0, NEW_CURRENT_SCENE, (CObject*)( m_doc.CurrentScene()->OpenGLScene() ) );
  }
  else
  {
  	m_doc.UpdateAllViews(0, NEW_CURRENT_SCENE, (CObject*)( m_doc.CurrentScene() ) );
  }
}

void CScenePtr::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  CGraphPtr<COpenGLSceneBase>::OnNeighbourModified(node, uHint);
  if (m_doc.CurrentScene())
  {
  if (m_doc.CurrentScene()->OpenInventorScene())
      m_doc.UpdateAllViews(0, CURRENT_SCENE_MODIFIED, (CObject*)( m_doc.CurrentScene()->OpenInventorScene() ) );
  else
      m_doc.UpdateAllViews(0, CURRENT_SCENE_MODIFIED, (CObject*)( m_doc.CurrentScene()->OpenGLScene() ) );
  }
  else
  {
    m_doc.UpdateAllViews(0, CURRENT_SCENE_MODIFIED, (CObject*)( m_doc.CurrentScene() ) );
  }
}
  
void CScenePtr::OnNeighbourDeleted(const CGraphNode& node)
{
  CGraphPtr<COpenGLSceneBase>::OnNeighbourDeleted(node);
  m_doc.UpdateAllViews(0, NEW_CURRENT_SCENE, 0 );
}

CScenePtr &CScenePtr::operator=(COpenGLSceneBase *pScene)
{
  if (pScene)
  {
    if(pScene != Ptr())
    {
      assert(!IsLinkedTo(*pScene));
      LinkTo(*pScene);
    }
  }
  else if (Ptr())
    Clear();
  
  return *this;
}
