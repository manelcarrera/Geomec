 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ModelObjectScene.h: interface for the CModelObjectScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELOBJECTSCENE_H__191D8F32_BE7A_4AEF_95D0_F8B67920686F__INCLUDED_)
#define AFX_MODELOBJECTSCENE_H__191D8F32_BE7A_4AEF_95D0_F8B67920686F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLScene.h"

#include "GuiFrameExports.h"

class CLegendFrame;
class COpenGLObjectObserver;
class GUIFRAME_EXPORT CModelObjectScene : public COpenGLScene
{
	std::vector<COpenGLObjectObserver*> m_observers;
	// Prototype of function, which execute the handler of opengl node
	typedef std::vector<const geo::IObject*> TObjectVec;

  TObjectVec filterSelection(const COpenGLObjectObserver& observer, const TObjectVec& vcSelection) const;
	bool m_fill;
 	bool m_curvedDraw;
	friend class COpenGLObjectObserver;
	mutable CLegendFrame* m_legend;

public:
	// Construction
	CModelObjectScene(bool bDisplayAxis = true);
	virtual ~CModelObjectScene();

	// Handlers for the objects
	virtual bool MousePress(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseRelease(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseDblClk(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseMove(TKeyboardModifiers modifiers, TMouseButton button, const TScreenPoint& point);

	// Fill
	void setFill( bool fill );
	bool fill() const;

 //Curved draw
	void setCurvedDraw( bool curvedDraw);
	bool curvedDraw() const ;

	virtual std::vector<TColor> onColor(const COpenGLObjectObserver& node, const geo::IObject &object) const;

	CLegendFrame& legend();
	const CLegendFrame& legend() const;
};

#endif // !defined(AFX_MODELOBJECTSCENE_H__191D8F32_BE7A_4AEF_95D0_F8B67920686F__INCLUDED_)
