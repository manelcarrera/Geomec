// ScenePtr.h: interface for the CScenePtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEPTR_H__33312A3B_D1EC_485B_B065_8DDDD5B7F792__INCLUDED_)
#define AFX_SCENEPTR_H__33312A3B_D1EC_485B_B065_8DDDD5B7F792__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFemAppDoc;
class COpenGLSceneBase;

typedef CGraphPtr<COpenGLSceneBase> TScenePtr;
class CScenePtr : public TScenePtr
{
	CFemAppDoc& m_doc;
public:
	CScenePtr(CFemAppDoc& doc);
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourDeleted(const CGraphNode& node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	CScenePtr &operator=(COpenGLSceneBase *pScene);
};

#endif // !defined(AFX_SCENEPTR_H__33312A3B_D1EC_485B_B065_8DDDD5B7F792__INCLUDED_)
