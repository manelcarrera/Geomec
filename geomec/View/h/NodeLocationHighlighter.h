#ifndef _NODELOCATIONHIGHLIGHTER_H_
#define _NODELOCATIONHIGHLIGHTER_H_

#include <set>

class COpenGLSceneBase;

#include "openglnode.h"

class CNodeLocationHighlighter
{
public:
	CNodeLocationHighlighter(COpenGLSceneBase& scene);
	~CNodeLocationHighlighter();
	bool MouseMove(const IOpenGLFrame::TScreenPoint& point, bool bShowLocation);

private:
	typedef std::set<const geo::INode*> TNodeSet;
	void ProcessEntry(const geo::IObject& object, TNodeSet& stNodes);

public:
	class CPointHighlightNode : public COpenGLNode
	{
	public:
		CPointHighlightNode(const geo::INode& node, COpenGLSceneBase& scene);
		~CPointHighlightNode();

		const geo::INode& Node() const;
		void UpdateStatusBarText();

		virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;
		virtual int DisplayListSize() const;
		virtual const geo::IObject& DisplayList(int nIndex) const;
		virtual TColor Color() const;

		virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;
		virtual bool Empty() const;
		virtual long SavedItems() const;
		bool Value() const; //wjrx mantis 2568
    virtual bool BlocksDrawing() const { return false; }

	private:
		const geo::INode& m_node;
		COpenGLSceneBase& m_scene;
		CView* m_pView;

		class CPointDrawDef : public COpenGLNode::CDrawDef
		{
		public:
			CPointDrawDef(const COpenGLNode& node);
			virtual DrawDecisionFloat PointSize() const;
		};
	};

private:
	COpenGLSceneBase& m_scene;
	CPointHighlightNode* m_pPointHighlightNode;
	CToolTipCtrl m_tooltip;
};

#endif // _NODELOCATIONHIGHLIGHTER_H_
