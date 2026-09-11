// WellSceneInterMed.h: interface for the CWellSceneInterMed class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSCENEINTERMED_H__E234F49A_E7C8_45A7_83B2_6F09D58208F0__INCLUDED_)
#define AFX_WELLSCENEINTERMED_H__E234F49A_E7C8_45A7_83B2_6F09D58208F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellPointDrawSpec.h"
#include "WellSectionDrawSpec.h"
#include "WellSceneInterMed.h"
#include <qobject.h>

namespace well {
  class CWellPathBase;
}

class CWaterLevelSymbol;
class CGroundLevelSymbol;
class CDrillingTowerSymbol;
class COpenGLScene; 

class CWellSceneInterMed  : public QObject
{
	Q_OBJECT;

public:
	CWellSceneInterMed(well::CWellPathBase& WellPath);

	void AttachScene(COpenGLScene* scene);
	void DetachScene();

	virtual ~CWellSceneInterMed();

	well::CWellPathBase& WellPath();
	COpenGLScene* Scene();

	//Point operations
	CWellPointDrawSpec& AddPoint(well::CWellPoint& WellPoint,CWellPointDrawSpec::TYPE type = CWellPointDrawSpec::ARROW);
	CWellPointDrawSpec& GetPoint(well::CWellPoint& WellPoint,long index=0);
	const CWellPointDrawSpec& GetPoint(well::CWellPoint& WellPoint,long index=0) const;
	bool ExistPoint(well::CWellPoint& WellPoint,long index=0)const ;
	void RemovePoint(well::CWellPoint& WellPoint,long index=0);
	void RemovePoints(well::CWellPoint& WellPoint);
	void RemoveAllPoints();

	CWaterLevelSymbol&  WaterLevelSymbol();
	CGroundLevelSymbol& GroundLevelSymbol();
	CDrillingTowerSymbol& DrillingTowerSymbol();

	//Sections operations
	CWellSectionDrawSpec& AddSection(well::IWellSection& WellSection, CWellSectionDrawSpec::TYPE type = CWellSectionDrawSpec::LINE);
	CWellSectionDrawSpec& GetSection(well::IWellSection& WellSection,long index=0);
	const CWellSectionDrawSpec& GetSection(well::IWellSection& WellSection,long index=0) const;

	bool ExistSection(well::IWellSection& WellSection,long index=0)const ;
	void RemoveSection(well::IWellSection& WellSection,long index=0);
	void RemoveSections(well::IWellSection& WellSection);
	void RemoveAllSections();

	//returns a new created wellpoint depending on selected objects in vec
	//if no selection then return null
	//caller is responsible for deleting return point
	well::CWellPoint* ProcessNewPointSelection(std::vector<const geo::IObject*>& vec, const geo::CLine& selection_line );

	void DrawWellInfo(bool bDraw);
	bool DrawWellInfo() const;

private:
	typedef QList<CWellPointDrawSpec*>   TWellPointDrawSpecList;
	typedef QList<CWellSectionDrawSpec*> TWellSectionDrawSpecList;
	typedef QMap<well::CWellPoint*,TWellPointDrawSpecList*> TPointDrawMap;
	typedef QMap<well::IWellSection*,TWellSectionDrawSpecList*> TSectionDrawMap;

	TPointDrawMap m_PointMap;
	TSectionDrawMap m_SectionMap;

	COpenGLScene* m_pScene;
	well::CWellPathBase* m_pWellPath;

	//CWellDrawSpecBase* m_OverburdenSymbol;
	//CWellDrawSpecBase* m_WaterheightSymbol;
	CWaterLevelSymbol* m_WaterLevelSymbol;
	CGroundLevelSymbol* m_GroundLevelSymbol;
	CDrillingTowerSymbol* m_DrillingTowerSymbol;

	bool m_bDrawWellInfo;

private slots:
	void OnSceneDestroy(const COpenGLScene& scene);
};

#endif // !defined(AFX_WELLSCENEINTERMED_H__E234F49A_E7C8_45A7_83B2_6F09D58208F0__INCLUDED_)
