// OpenGLSceneBase.h: interface for the COpenGLSceneBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVALUESCENE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
#define AFX_OPENGLVALUESCENE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>

class IColorScaleNode;
class IColorScaleEntry;
class CFemAppModel;
class CUnitNode;

#include "openglscenenode.h"
#include "ivaluecomponent.h"

class COpenGLValueScene : public COpenGLSceneNode  
{
public:
	typedef IValueDomainScalar::TMinMax TMinMax;
	typedef IValueDomainScalar::TValue TValue;
private:
	IValueComponentBase *m_pValue;
  IValueComposite* m_pComposite;
	IColorScaleNode *m_pColorScale;
	IColorScaleEntry* m_pColorScaleEntry;
	BOOL m_bShowColor;
	TMinMax m_MinMaxValue;
	CUnitNode* m_pUnit;
	BOOL RecalcMinMax(const COpenGLNode *newNode=0);
	// Private interface for min/max cache
	typedef std::map<const COpenGLNode*, TMinMax> TMinMaxMap;
	mutable TMinMaxMap m_mpMinMaxCache;
	void InvalidateMinMaxCache(const COpenGLNode *pNode);
	const TMinMax& MinMax(IProgressBase& progressBase, const COpenGLNode &node) const;
	int MinMaxSteps(const COpenGLNode &node) const;
	void init(CFemAppModel& model);
protected:
	virtual bool OnInsertNodeInLegend(const COpenGLNode& node) const;
public:
	// Construction / Destruction
	COpenGLValueScene(CFemAppModel &model);
	COpenGLValueScene(const QString &strName, CFemAppModel& model);
	COpenGLValueScene(const unsigned int uName, CFemAppModel& model);
	COpenGLValueScene(const COpenGLValueScene& rhs);
	virtual ~COpenGLValueScene();

	// Show color / show value
	BOOL ShowColor() const;
	void ShowColor(BOOL bShowColor);
	BOOL ShowValue() const;
	void ShowValue(BOOL bShowValue);

	// Access to the color scale
	const IColorScaleNode* ColorScale() const;
	IColorScaleNode* ColorScale();

	// Quick and dirty color scale settings
	void ColorScale(IColorScaleNode& color_scale);

	// Quantity of the scene
	const IValueComponentBase* ValueComponent() const;
	IValueComponentBase* ValueComponent();

	const TMinMax& MinMaxValue() const;
 
	//##ModelId=3BE7AEBE013E
	bool operator==(const COpenGLValueScene &rhs) const;
	//##ModelId=3BE7AEBE016C
	COpenGLValueScene& operator=(const COpenGLValueScene& rhs);

	// Access to Unit
	const CUnitNode& UnitNode() const;

	// GraphMessaging
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &node);

	// Add, modify and delete nodes
	virtual void OnOpenGLNodeDeleted(COpenGLNode& node);
	virtual void OnOpenGLNodeModified(COpenGLNode& node, enum ModifiedHint uHint);
	virtual void OnNewOpenGLNode(COpenGLNode& node);

	virtual std::vector<CDrawDef::TColor> OnColor(const COpenGLNode& node, const geo::IObject &object) const;
	virtual void OnSelect(CGraphNode& node);
	bool CanConnectItem(const CGraphNode &item) const;
	bool ConnectItem(const CGraphNode &node); 

	// Clear ....
	virtual void Clear();

  virtual bool BlockDrawing() const { return false; }
  virtual int OnCountDrawSteps() const;
  virtual void DrawScene();
};


#endif // !defined(AFX_OPENGLVALUESCENE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
