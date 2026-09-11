// ZoomInCreator.h: interface for the CZoomInCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOOMINCREATOR_H__4FB90830_1ED5_4ffb_B2CC_851233CAC743__INCLUDED_)
#define AFX_ZOOMINCREATOR_H__4FB90830_1ED5_4ffb_B2CC_851233CAC743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IScalingBox.h"
#include "pointset.h"
#include "valuecompositeobserver.h"
#include "openglnodeobserver.h"
#include "doubleedit.h"

class CModelBase;
class CTetraModel;
class CHomogenizationRunner;
class CMaterialServer;
class IMaterialRock;
class IFormationElementSet;
class CElementSet;
class CElementValueSet;
class IResult;
class IResultComponent;
class CDepletionStage;

#include "StateBranch_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"

class CZoomInCreator
{
	CHomogenizationBox* m_box;
	void ExtractResults(CElementSet& element_set, const CAnalysisType& antype);
	void ExtractResults(CElementSet& element_set, const CDepletionStage& stage, const CAnalysisType& antype);
	typedef std::pair<const IResultComponent*, CElementValueSet*> TResultPair;
	typedef std::vector<TResultPair> TResultMap;
    void MapResults(const QString& sResultName,
					const IResult& source, 
					CValueType& target, 
					TResultMap &result_map,
					const CDepletionStage& stage, 
					const CAnalysisType& antype);
public:
	CZoomInCreator(CHomogenizationBox &box);
	void Create();
};

#endif // !defined(AFX_ZOOMINCREATOR_H__4FB90830_1ED5_4ffb_B2CC_851233CAC743__INCLUDED_)
