// WellSceneInterMed.cpp: implementation of the CWellSceneInterMed class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "WellPointDrawSpec.h"
#include "WellSectionDrawSpec.h"
#include "WellSceneInterMed.h"
#include "WaterLevelSymbol.h"
#include "GroundLevelSymbol.h"
#include "DrillingTowerSymbol.h"
#include "OpenGLScene.h"
#include <qmap.h> 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3F7AA97E0285
CWellSceneInterMed::CWellSceneInterMed(well::CWellPathBase& WellPath)
: m_pScene(0),
  m_pWellPath(&WellPath),
  m_bDrawWellInfo(true)
{
	m_WaterLevelSymbol=0;
	m_GroundLevelSymbol=0;
	m_DrillingTowerSymbol=0;
}

//##ModelId=3F7AA97E0288
CWellSceneInterMed::~CWellSceneInterMed()
{
	if(m_WaterLevelSymbol)
		delete m_WaterLevelSymbol;

	if(m_GroundLevelSymbol)
		delete m_GroundLevelSymbol;

	if(m_DrillingTowerSymbol)
		delete m_DrillingTowerSymbol;

	
	RemoveAllSections();
	RemoveAllPoints();

	DetachScene();
}

void CWellSceneInterMed::OnSceneDestroy(const COpenGLScene& /*scene*/)
{
	DetachScene();
}

void CWellSceneInterMed::AttachScene(COpenGLScene* scene)
{

	DetachScene();

	

	assert(!m_pScene);
	m_pScene=scene;
	connect(m_pScene,SIGNAL(OnDestroy(const COpenGLScene&)),this,SLOT(OnSceneDestroy(const COpenGLScene&)));

	//update point lists
	TPointDrawMap::iterator p_it = m_PointMap.begin();
	while(p_it != m_PointMap.end())
	{
		TWellPointDrawSpecList* pList = p_it.value();
		TWellPointDrawSpecList::iterator p_it2 = pList->begin();
		while(p_it2 != pList->end())
		{
			CWellPointDrawSpec* pDrawSpec = *p_it2;
			pDrawSpec->OnSceneAttached();
			p_it2++;
		}
		p_it++;
	}

	//update section lists
	TSectionDrawMap::iterator s_it = m_SectionMap.begin();
	while(s_it != m_SectionMap.end())
	{
		TWellSectionDrawSpecList* pList = s_it.value();
		TWellSectionDrawSpecList::iterator s_it2 = pList->begin();
		while(s_it2 != pList->end())
		{
			CWellSectionDrawSpec* pDrawSpec = *s_it2;
			pDrawSpec->OnSceneAttached();
			s_it2++;
		}
		s_it++;
	}

	if(m_WaterLevelSymbol)
		m_WaterLevelSymbol->OnSceneAttached();

	if(m_GroundLevelSymbol)
		m_GroundLevelSymbol->OnSceneAttached();

	if(m_DrillingTowerSymbol)
		m_DrillingTowerSymbol->OnSceneAttached();

	DrawWellInfo(m_bDrawWellInfo);
	
}


void CWellSceneInterMed::DetachScene()
{

	if(!m_pScene)
		return;

	disconnect();

	//update point lists
	TPointDrawMap::iterator p_it = m_PointMap.begin();
	while(p_it != m_PointMap.end())
	{
		TWellPointDrawSpecList* pList = p_it.value();
		TWellPointDrawSpecList::iterator p_it2 = pList->begin();
		while(p_it2 != pList->end())
		{
			CWellPointDrawSpec* pDrawSpec = *p_it2;
			pDrawSpec->OnSceneDetached();
			p_it2++;
		}
		p_it++;
	}
	


	//update section lists
	TSectionDrawMap::iterator s_it = m_SectionMap.begin();
	while(s_it != m_SectionMap.end())
	{
		TWellSectionDrawSpecList* pList = s_it.value();
		TWellSectionDrawSpecList::iterator s_it2 = pList->begin();
		while(s_it2 != pList->end())
		{
			CWellSectionDrawSpec* pDrawSpec = *s_it2;
			pDrawSpec->OnSceneDetached();
			s_it2++;
		}
		s_it++;
	}

	if(m_WaterLevelSymbol)
		m_WaterLevelSymbol->OnSceneDetached();

	if(m_GroundLevelSymbol)
		m_GroundLevelSymbol->OnSceneDetached();

	if(m_DrillingTowerSymbol)
		m_DrillingTowerSymbol->OnSceneDetached();

	
	m_pScene=0;
}

//##ModelId=3F7AA97E0293
CWellPointDrawSpec& CWellSceneInterMed::AddPoint(well::CWellPoint& WellPoint,CWellPointDrawSpec::TYPE type /*= CWellPointDrawSpec::LEVEL_SYMBOL*/)
{
	TWellPointDrawSpecList* pList;
	if(m_PointMap.find(&WellPoint) == m_PointMap.end())
	{
		pList = new TWellPointDrawSpecList;
//		pList->setAutoDelete(true);
    if(m_PointMap.find(&WellPoint) == m_PointMap.end())
		  m_PointMap.insert(&WellPoint,pList);
	}

	pList = m_PointMap[&WellPoint];
	CWellPointDrawSpec* p = new CWellPointDrawSpec(*this,WellPoint,type);
	pList->append(p);
	return *p;
}

//##ModelId=3F7AA97E0295
CWellPointDrawSpec& CWellSceneInterMed::GetPoint(well::CWellPoint& WellPoint,long index)
{
	TWellPointDrawSpecList* pList = m_PointMap[&WellPoint];
	return *pList->at(index);
}

//##ModelId=3F7AA97E0298
const CWellPointDrawSpec& CWellSceneInterMed::GetPoint(well::CWellPoint& WellPoint,long index) const
{
	TWellPointDrawSpecList* pList = m_PointMap[&WellPoint];
	return *pList->at(index);
}

//##ModelId=3F7AA97E029C
bool CWellSceneInterMed::ExistPoint(well::CWellPoint& WellPoint,long index) const
{
	if(m_PointMap.find(&WellPoint) == m_PointMap.end())
		return false;

	TWellPointDrawSpecList* pList = m_PointMap[&WellPoint];
  return (index < pList->size());
}


//##ModelId=3F7AA97E02A0
void CWellSceneInterMed::RemovePoint(well::CWellPoint& WellPoint,long index)
{
	TWellPointDrawSpecList* pList = m_PointMap[&WellPoint];
	pList->removeAt(index);
}

//##ModelId=3F7AA97E02A3
void CWellSceneInterMed::RemovePoints(well::CWellPoint& WellPoint)
{
	TWellPointDrawSpecList* pList = m_PointMap[&WellPoint];
	m_PointMap.remove(&WellPoint);
	delete pList;  //autoDelte !!!!
}

//##ModelId=3F7AA97E02A5
void CWellSceneInterMed::RemoveAllPoints()
{
	while(m_PointMap.count())
	{
		TPointDrawMap::iterator it = m_PointMap.begin();
		TWellPointDrawSpecList* pList = it.value();
		m_PointMap.erase(it);
    TWellPointDrawSpecList::iterator lit;
    for(lit = pList->begin(); lit != pList->end(); ++lit)
      delete *lit;
		delete pList;
		
	}
}






/////////////////////////////       SECTIONS   //////////////////////////////////////////////////////

//##ModelId=3F7AA97E02A6
CWellSectionDrawSpec& CWellSceneInterMed::AddSection(well::IWellSection& WellSection,CWellSectionDrawSpec::TYPE type)
{
	TWellSectionDrawSpecList* pList;
	if(m_SectionMap.find(&WellSection) == m_SectionMap.end())
	{
		pList = new TWellSectionDrawSpecList;
//		pList->setAutoDelete(true);
    if(m_SectionMap.find(&WellSection) == m_SectionMap.end())
		  m_SectionMap.insert(&WellSection,pList);
	}

	pList = m_SectionMap[&WellSection];
	CWellSectionDrawSpec* p = new CWellSectionDrawSpec(*this,WellSection,type);
	pList->append(p);
	return *p;
}


//##ModelId=3F7AA97E02A9
CWellSectionDrawSpec& CWellSceneInterMed::GetSection(well::IWellSection& WellSection,long index)
{
	TWellSectionDrawSpecList* pList = m_SectionMap[&WellSection];
	return *pList->at(index);
}


//##ModelId=3F7AA97E02AC
const CWellSectionDrawSpec& CWellSceneInterMed::GetSection(well::IWellSection& WellSection,long index) const
{
	TWellSectionDrawSpecList* pList = m_SectionMap[&WellSection];
	return *pList->at(index);
}


//##ModelId=3F7AA97E02B0
bool CWellSceneInterMed::ExistSection(well::IWellSection& WellSection,long index) const
{
	if(m_SectionMap.find(&WellSection) == m_SectionMap.end())
		return false;

	TWellSectionDrawSpecList* pList = m_SectionMap[&WellSection];
  return (index < pList->size());
}


//##ModelId=3F7AA97E02B4
void CWellSceneInterMed::RemoveSection(well::IWellSection& WellSection,long index)
{
	TWellSectionDrawSpecList* pList = m_SectionMap[&WellSection];
	pList->removeAt(index);
}

//##ModelId=3F7AA97E02B7
void CWellSceneInterMed::RemoveSections(well::IWellSection& WellSection)
{
	TWellSectionDrawSpecList* pList = m_SectionMap[&WellSection];
  if(pList)
  {
	  m_SectionMap.remove(&WellSection);
    TWellSectionDrawSpecList::iterator it;
    for(it = pList->begin(); it != pList->end(); ++it)
      delete *it;
	  delete pList;
  }
}

//##ModelId=3F7AA97E02B9
void CWellSceneInterMed::RemoveAllSections()
{
	while(m_SectionMap.count())
	{
		TSectionDrawMap::iterator it = m_SectionMap.begin();
    RemoveSections(*it.key());
//		TWellSectionDrawSpecList* pList = it.value();
//		m_SectionMap.erase(it);
//		delete pList;
	}
}




//##ModelId=3F7AA97E028A
well::CWellPathBase& CWellSceneInterMed::WellPath()
{
	return *m_pWellPath;
}

//##ModelId=3F7AA97E028B
COpenGLScene* CWellSceneInterMed::Scene()
{
	return m_pScene;
}


CWaterLevelSymbol& CWellSceneInterMed::WaterLevelSymbol()
{
	if(!m_WaterLevelSymbol)
		m_WaterLevelSymbol = new CWaterLevelSymbol(*this);

	m_WaterLevelSymbol->CreateScene();
	return *m_WaterLevelSymbol;
}

CGroundLevelSymbol& CWellSceneInterMed::GroundLevelSymbol()
{
	if(!m_GroundLevelSymbol)
		m_GroundLevelSymbol = new CGroundLevelSymbol(*this);

	m_GroundLevelSymbol->CreateScene();
	return *m_GroundLevelSymbol;
}

CDrillingTowerSymbol& CWellSceneInterMed::DrillingTowerSymbol()
{
	if(!m_DrillingTowerSymbol)
		m_DrillingTowerSymbol = new CDrillingTowerSymbol(*this);

	m_DrillingTowerSymbol->CreateScene();
	return *m_DrillingTowerSymbol;
}

//returns a new created wellpoint depending on selected objects in vec
//if no selection then return null
//caller is responsible for deleting return point
well::CWellPoint* CWellSceneInterMed::ProcessNewPointSelection(std::vector<const geo::IObject*>& vec,const geo::CLine& selection_line)
{
	//update section lists
	TSectionDrawMap::iterator s_it = m_SectionMap.begin();
	while(s_it != m_SectionMap.end())
	{
		TWellSectionDrawSpecList* pList = s_it.value();
		TWellSectionDrawSpecList::iterator s_it2 = pList->begin();
		while(s_it2 != pList->end())
		{
			CWellSectionDrawSpec* pDrawSpec = *s_it2;
			well::CWellPoint* p = pDrawSpec->ProcessNewPointSelection(vec,selection_line);
			if(p)
				return p;

			s_it2++;
		}
		s_it++;
	}
	return 0;
}

void CWellSceneInterMed::DrawWellInfo(bool bDraw)
{
	m_bDrawWellInfo = bDraw;

	// modify sections
	TSectionDrawMap::iterator sit;
	for(sit = m_SectionMap.begin(); sit != m_SectionMap.end(); ++sit)
	{
		TWellSectionDrawSpecList* pSectionList = sit.value();
		if(pSectionList)
		{
			for(TWellSectionDrawSpecList::iterator it = pSectionList->begin(); it != pSectionList->end(); ++it)
			{
				CWellSectionDrawSpec& ds = **it;
				if(ds.GetType() != CWellSectionDrawSpec::LINE && ds.GetType() != CWellSectionDrawSpec::GRADIENT)
					ds.SetVisible(bDraw);
			}
		}
	}
}

bool CWellSceneInterMed::DrawWellInfo() const
{
	return m_bDrawWellInfo;
}

