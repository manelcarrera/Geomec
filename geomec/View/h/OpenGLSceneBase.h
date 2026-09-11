// OpenGLSceneBase.h: interface for the COpenGLSceneBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLSCENEBASE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
#define AFX_OPENGLSCENEBASE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COpenGLSceneBase_Delegate;

#include <set>
#include <map>

//##ModelId=3BC55D600257
class CCrossSection;
class CColorScaleEntry;
class IResultComponent;
class CNodeLocationHighlighter;

#include "GraphEntry_Delegate.h"
#include "NodeObserver_Delegate.h"
#include "openglvaluescene.h"
#include "resourceIDI.h"

class COpenGLSceneBase : public COpenGLValueScene
{
public:
	class CNodeLocationListener : public COpenGLValueScene::CDefaultListener
	{
	public:
		CNodeLocationListener(COpenGLSceneBase& scene);
		~CNodeLocationListener();
		virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);

	private:
		CNodeLocationHighlighter* m_pHighlighter;
	};

private:
	// Helper class to display boundingbox ...
	class CBoundaryObserver : public CGraphNode
	{
		COpenGLSceneBase& m_scene;
	public:
		CBoundaryObserver(COpenGLSceneBase &scene);
		~CBoundaryObserver();
		virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;
		virtual QString TypeName() const;
		virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	};
	friend class CBoundaryObserver;

	typedef std::vector<CCrossSection*> TXSectionVec;
	TXSectionVec m_vcXSection;
	void Initialize();
	void InitAxis();
	
	//##ModelId=3BE7AEBD0248
	CBoundaryObserver* m_pBoundaryObserver;
	CNodeLocationListener* m_pNodeLocationListener;

protected:
	void ClearExceptResult();
	void UpdateBoundary();
	void OnUpdateValueLegend();
	void AddResultPath(const IResultComponent& result_component);
	enum eShowMode { SHOW_ALL, SHOW_SELECTED, SHOW_UNSELECTED };
	int ShowEntry(unsigned int uEntry, enum eShowMode mode);
  virtual bool CanDisplayResultComponent(const IResultComponent& result_component) const;

public:
	// Construction / destruction
	COpenGLSceneBase(CFemAppModel &model);
	COpenGLSceneBase(const QString &strName, CFemAppModel& model);
	COpenGLSceneBase(const unsigned int uName, CFemAppModel& model);
	COpenGLSceneBase(const COpenGLSceneBase& rhs);
	virtual ~COpenGLSceneBase();

	// Connections
	virtual bool CanConnectItem(const CGraphNode& item) const;

	virtual void OnUpdateLegend();

	//##ModelId=3BC55D600296
	virtual unsigned int TypeId() const;
	//##ModelId=3BC55D600298
	virtual unsigned int IconId() const;

	//##ModelId=3BE7AEBE013E
	bool operator==(const COpenGLSceneBase &rhs) const;
	//##ModelId=3BE7AEBE016C
	virtual COpenGLSceneBase& operator=(const COpenGLSceneBase& rhs);

	// Max and min of the model ... 
	//##ModelId=3BE7AEBE019B
	void ShowBoundary(BOOL bShowBoundary);
	//##ModelId=3BE7AEBE01BA
	BOOL ShowBoundary() const;

	// Can connect ...
	//##ModelId=3BC55D6002D6
	virtual void OnNewNeighbour(const CGraphNode &node);
	//##ModelId=3BC55D6002D9
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	//##ModelId=3BC55D6002E6
	virtual void OnNeighbourDeleted(const CGraphNode &node);

	virtual void Clear();
	virtual void OnSelect(CGraphNode& node);
	virtual void BackgroundColor(TColor color);
	virtual TColor BackgroundColor() const { return COpenGLValueScene::BackgroundColor(); }
	QString GetResultPath(const IValueComponentBase& result_component) const;
	// Save and load
	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual QString GetTypeString() const {return "BASE";}

	virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point);

  virtual bool BlockDrawing() const;
	virtual TOpenGLNodeSet NodesInSceneForLegend() const;
};

template<class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CSceneObserver : public CNodeObserver_Delegate<COpenGLSceneBase, COpenGLSceneBase_Delegate, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> 
{
public:
	CSceneObserver(COpenGLSceneBase& node,
					CTreeNode &parent,
					const BOOL rename,
					const REMOVE_TYPE remove,
					HTREEITEM hInsertAfter = TVI_LAST)
	: CNodeObserver_Delegate<COpenGLSceneBase, COpenGLSceneBase_Delegate, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node, parent, rename, remove, hInsertAfter){}

	// Building a class for switching the icon ...
	virtual unsigned int Icon() const
	{
		// Get the document ...
		CGeomecDoc* pDoc = &((CGeomecApp*)AfxGetApp())->GetDoc();
		if(pDoc->CurrentScene() && (&ObservedItem() == pDoc->CurrentScene()->OpenGLScene() || &ObservedItem() == pDoc->CurrentScene()->OpenInventorScene()))
			return IDI_RADIO_CHECKED;
		return IDI_RADIO_UNCHECKED;
	}
};

#endif // !defined(AFX_OPENGLSCENEBASE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
