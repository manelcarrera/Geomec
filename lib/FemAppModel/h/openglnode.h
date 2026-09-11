// openglnode.h: interface for the COpenGLNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLNODE_H__FB2AAE42_BF02_4B63_B725_E610841923A3__INCLUDED_)
#define AFX_OPENGLNODE_H__FB2AAE42_BF02_4B63_B725_E610841923A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IOpenGLFrame.h"
#include "OpenGLNodeBase.h"
#include "ZeroDrawDef.h"
#include "MouseListener.h"

class COpenGLNode : public COpenGLNodeBase
{
public:
	virtual bool Value() const;//wjrx mantis 2568
	class CDrawDef
	{
		const COpenGLNode& m_node;
		const CZeroDrawDef m_zeroDrawDef;

	public:
		typedef QRgb TColor;

		typedef std::pair <bool, bool> DrawDecisionBool;
		typedef std::pair <bool, int> DrawDecisionInt;
		typedef std::pair <bool, long> DrawDecisionLong;
		typedef std::pair <bool, float> DrawDecisionFloat;
		typedef std::pair <bool, std::vector <TColor> > DrawDecisionVector;

		typedef std::pair <bool, GLint> DrawDecisionGLint;
		typedef std::pair <bool, GLuint> DrawDecisionGLuint;
		typedef std::pair <bool, GLenum> DrawDecisionGLenum;
		typedef std::pair <bool, TColor> DrawDecisionColor;
		typedef std::pair <bool, GLushort> DrawDecisionGLushort;

		CDrawDef(const COpenGLNode& node);
		virtual DrawDecisionVector Color(const geo::IObject &object) const;
		const COpenGLNode& OpenGLNode() const;
		virtual DrawDecisionBool Visible() const;
		virtual DrawDecisionBool PolyFillFront() const;
		virtual DrawDecisionBool PolyFillBack() const;
		virtual DrawDecisionBool LineStipple() const;
		virtual DrawDecisionFloat LineWidth() const;
		virtual DrawDecisionFloat PointSize() const;
		virtual DrawDecisionBool PolyDrawFront() const;
		virtual DrawDecisionBool PolyDrawBack() const;
		virtual DrawDecisionInt DepthFunc() const;
		//wjrx mantis 2568 virtual bool Value() const;
		virtual DrawDecisionBool CurvedDraw() const;
		virtual DrawDecisionBool IsSelectable(const geo::IObject &object) const;		

		virtual DrawDecisionColor TextColor() const;
		virtual DrawDecisionGLint LineStippleFactor() const;
		virtual DrawDecisionGLushort LineStipplePattern() const;

		virtual DrawDecisionBool RequireCompile() const;
		virtual DrawDecisionLong CircleNrOfSegments() const;

		virtual DrawDecisionBool LabelBorder() const;
		virtual DrawDecisionColor LabelBorderOutlineColor() const;
		virtual DrawDecisionColor LabelBorderInteriorColor() const;

		virtual DrawDecisionBool UseStencilBuffer() const;
		virtual DrawDecisionGLenum StencilFuncFunc() const;
		virtual DrawDecisionGLint StencilFuncRef() const;
		virtual DrawDecisionGLuint StencilFuncMask() const;
		virtual DrawDecisionGLenum StencilOpFail() const;
		virtual DrawDecisionGLenum StencilOpZFail() const;
		virtual DrawDecisionGLenum StencilOpZPass() const;
	};

	typedef std::vector<const geo::IObject*> TDisplayVec;

	// Construction ...
	COpenGLNode(CFemAppModel& model);
	COpenGLNode(const QString &strName, CFemAppModel& model);
	COpenGLNode(const unsigned int uName, CFemAppModel& model);
	COpenGLNode(const COpenGLNode &rhs);

	// Rendering and default color
	virtual int DisplayListSize() const = 0;
	virtual const geo::IObject& DisplayList(int nIndex) const = 0;		// Always ...
	virtual TColor Color() const = 0;

	virtual geo::CPoint Min() const;
	virtual geo::CPoint Max() const;

	virtual std::vector<CDrawDef::TColor> OnColor(const geo::IObject &object) const;
	virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;

	// Equal and assignment
	COpenGLNode& operator=(const COpenGLNode &rhs);
	bool operator==(const COpenGLNode &rhs) const;

	typedef CMouseListener::TFrame TFrame;
  typedef IOpenGLFrame::TKeyboardModifiers TKeyboardModifiers;
  typedef IOpenGLFrame::TMouseButton TMouseButton;
	typedef QPoint TScreenPoint;
	typedef IOpenGLFrame::TObjectVec TObjectVec;
	// selection priority
	virtual int SelectionPriority() const;
  virtual bool BlocksDrawing() const { return true; }

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitOpenGLNode(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitOpenGLNode(*this); }
};

typedef CSingleCommandTemplate<COpenGLNode> TOpenGLNodeCommand;
typedef CConstSingleCommandTemplate<COpenGLNode> TConstOpenGLNodeCommand;

#endif // !defined(AFX_OPENGLNODE_H__FB2AAE42_BF02_4B63_B725_E610841923A3__INCLUDED_)
