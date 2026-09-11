// WellPath.cpp: implementation of the CWellPath class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "WellPathFile.h"
#include "GeomecUtils.h"
#include "WellSceneInterMed.h"
#include "ResultTree.h"
#include "3dmodel.h"
#include "ImportFormat.h"
#include "PropertyFile.h"
#include "PropertyExcel.h"
#include "WellZoomInModel.h"
#include "LasPointSet.h"
#include "resourceIDS.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "WellPathIntermediate.h"
#include "WellSection.h"
#include "StreamVersion.h"
#include "WellLog.h"
#include "BodyGroup.h"
#include "FvGocadWellFile.h"
#include "ModifiedHint.h"
#include "ValidateModel.h"

#include "VersionNumbers.h"
#include "NewArchiveStdStringStream.h"
#include "NewWellPath.h"
#include "ModelLifetimeFacade.h"

//point list id
#define LST_PT_DSTABOR         1 //stabor point list
#define LST_PT_DCASING         2 //casing points list
#define LST_PT_GRAPH           3 //graph points
#define LST_PT_INTERSEC_MESH   4 //mesh intersection points
#define LST_PT_INTERSEC_FORM   5 //formation intersection points
#define LST_PT_FIST_EXP		   6 //points for FIST export

//#ifdef _DEBUG
#define ZOOMIN
//#endif

/////////////////////////////////////////////////////////////////////
// Implementation of wellpath
//////////////////////////////////////////////////////////////////////

void CWellPath::Init()
{
  m_pIntermediate = m_pCachingIntermediate = new CCachingIntermediate;

  m_bSectionsDirty = true;

	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());

	CDepletionStage &initial_stage = pModel->InitialDepletionStage();
	SetBirthDate(QDate(initial_stage.Time().GetYear(), initial_stage.Time().GetMonth(), 1));

	m_pDsbResults = new CDstaborResultMap(*this);

	InsertWellSectionList( new well::CWellSectionList(*this,true),LST_SC_FORM);
	GetWellSectionList(LST_SC_FORM).SetDescription("Formation");

	InsertWellSectionList( new well::CWellSectionList(*this,true),LST_SC_WELL);
	GetWellSectionList(LST_SC_WELL).SetDescription("Well section");


	InsertWellPointList(new well::CWellPointList(*this,true),LST_PT_DSTABOR);
	GetWellPointList(LST_PT_DSTABOR).SetDescription("Dstabor Points");

	InsertWellPointList(new well::CWellPointList(*this,true),LST_PT_DCASING);
	GetWellPointList(LST_PT_DCASING).SetDescription("DCasint Points");

	InsertWellPointList(new well::CWellPointList(*this,true),LST_PT_GRAPH);
  m_graphPointsCalculated = false;
	GetWellPointList(LST_PT_GRAPH).SetDescription("Graph points");

	InsertWellPointList(new well::CWellPointList(*this,true),LST_PT_INTERSEC_MESH);
	GetWellPointList(LST_PT_INTERSEC_MESH).SetDescription("Intersection points with mesh");

	InsertWellPointList(new well::CWellPointList(*this,true),LST_PT_INTERSEC_FORM);
	GetWellPointList(LST_PT_INTERSEC_FORM).SetDescription("Intersection points with formation");

	InsertWellPointList(new well::CWellPointList(*this, true), LST_PT_FIST_EXP);
	GetWellPointList(LST_PT_FIST_EXP).SetDescription("Points used for FIST export");

	AutoDeleteSectionLists(true);
	AutoDeletePointLists(true);

	m_DsbLiquidFluid=1;
	m_DsbFluidLossCoefficient=1;

	m_dMudGradRefDepth=0;
	m_bShowDsbPointsInScene= false; //wjrx mantis 2542

  m_pZoomInModel = 0;
}


CWellPath::CWellPath(CFemAppModel& model)
: CColorNode(model)
{
	IModelObject::create();
	Init();
}


CWellPath::CWellPath(const QString& sName, CFemAppModel& model)
: CColorNode(sName, model)
{
	IModelObject::create();
	Init();
	CGraphNode::create( Model().GraphEntry(MD_BASE_WELLPATH) );
	LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
	LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
}

CWellPath::CWellPath(const QString &sName, CFemAppModel &model, const geo::CPoint &BasePoint)
: CColorNode(sName, model),
  m_VerticalPointDef(BasePoint)
{
	IModelObject::create();
	Init();
	InitVerticalWell(true);
	CGraphNode::create(Model().GraphEntry(MD_BASE_WELLPATH));
	LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
	LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
}

CWellPath::CWellPath(const CWellPath &well_path, CFemAppModel &model)
: CColorNode(well_path.Name().toStdString().c_str(), model),
  CWellPathBase(well_path),
  m_VerticalPointDef(well_path.m_VerticalPointDef)
{
	Init();

	CGraphNode::create(Model().GraphEntry(MD_BASE_WELLPATH));
	LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
	LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

	// Copy color, derrickelevation, easting, northing, vertical and points
	Color(well_path.Color());
}

CWellPath::CWellPath(const CWellPath &WellPath)
: CColorNode(WellPath),
  CWellPathBase(WellPath),
  m_pIntermediate(WellPath.Intermediate())
{
	assert(false); // 13-02-04 There is reason to believe that this is not used anymore, so this assert is a test!!
	//copy constructor from atrributres dialoog
	AutoDeletePointLists(false);
	DefPointList() = WellPath.DefPointList();
	m_DsbLiquidFluid=WellPath.m_DsbLiquidFluid;
	m_DsbFluidLossCoefficient=WellPath.m_DsbFluidLossCoefficient;
	m_dMudGradRefDepth = WellPath.m_dMudGradRefDepth;
	m_bShowDsbPointsInScene=WellPath.m_bShowDsbPointsInScene;
  m_bSectionsDirty = WellPath.m_bSectionsDirty;
	
}

CWellPath::~CWellPath()
{
	m_display_list.ClearAndDelete();
	m_pIntermediate->Detach();
  delete m_pCachingIntermediate;
  m_pIntermediate = 0;

	RemoveWellPointList(LST_PT_DCASING);
	RemoveWellPointList(LST_PT_DSTABOR);
	RemoveWellPointList(LST_PT_GRAPH);
  m_graphPointsCalculated = false;
	RemoveWellPointList(LST_PT_INTERSEC_FORM);
	RemoveWellPointList(LST_PT_INTERSEC_MESH);
	RemoveWellSectionList(LST_SC_FORM);
	RemoveWellSectionList(LST_SC_WELL);

	

	delete m_pDsbResults;

	TSectDeplLogMap::iterator it;
	//TODO tnma crash..
//	for(it = m_mpSectDplLog.begin(); it != m_mpSectDplLog.end(); it++)
//	{
//		delete it->second.second;
//	}

  DestroyZoomInModel();

}

const well::CWellPointList& CWellPath::DStaborPoints() const
{
	return GetWellPointList(LST_PT_DSTABOR);
}

const well::CWellPointList& CWellPath::DCasintPoints() const
{
	const well::CWellPointList & List = GetWellPointList(LST_PT_DCASING);
	return List;
}

well::CWellPointList& CWellPath::DCasintPoints()
{
	well::CWellPointList& List = GetWellPointList(LST_PT_DCASING);
	return List;
}

well::CWellPointList& CWellPath::DStaborPoints(bool bCheck)
{
	if ( bCheck )
	{
		well::CWellPointList &dsp= GetWellPointList(LST_PT_DSTABOR);
		well::CWellPointList::Iterator it= dsp.begin();
		CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());
		CValidateModel validateModel(pModel);

		while (it != dsp.end())
		{
			CGeoWellPoint gwp( *pModel, **it);
			const CFormationBase *pFormation= gwp.Formation();
			if ( pFormation==0
			|| validateModel.checkMaterial(*pFormation)==false)
			{
				well::CWellPointList::Iterator it2= it;
				++it;
				dsp.RemovePoint(*it2);
			}
			else
			{
				++it;
			}
		}
	}
	return GetWellPointList(LST_PT_DSTABOR);
}

const well::CWellPointList& CWellPath::GraphPoints() const
{
	if(!m_graphPointsCalculated)
  {
		const_cast<CWellPath&>(*this).CalcGraphPoints();
  }

	return GetWellPointList(LST_PT_GRAPH);
}

well::CWellPointList& CWellPath::GraphPoints()
{
	if(!m_graphPointsCalculated)
		const_cast<CWellPath&>(*this).CalcGraphPoints();

	return GetWellPointList(LST_PT_GRAPH);
}

const well::CWellPointList& CWellPath::FormIntersecPoints() const
{
	if(GetWellPointList(LST_PT_INTERSEC_FORM).NrOfPoints() ==0)
		const_cast<CWellPath*>(this)->CalculateFormationIntersecPoints();

	return GetWellPointList(LST_PT_INTERSEC_FORM);
}

well::CWellPointList& CWellPath::FormIntersecPoints()
{
	if(GetWellPointList(LST_PT_INTERSEC_FORM).NrOfPoints() ==0)
		CalculateFormationIntersecPoints();

	return GetWellPointList(LST_PT_INTERSEC_FORM);
}

const well::CWellPointList& CWellPath::MeshIntersecPoints() const
{
	if(GetWellPointList(LST_PT_INTERSEC_MESH).NrOfPoints() ==0)
		const_cast<CWellPath*>(this)->CalculateMeshIntersecPoints();

	return GetWellPointList(LST_PT_INTERSEC_MESH);

}

well::CWellPointList& CWellPath::MeshIntersecPoints()
{
	if(GetWellPointList(LST_PT_INTERSEC_MESH).NrOfPoints() ==0)
		const_cast<CWellPath*>(this)->CalculateMeshIntersecPoints();

	return GetWellPointList(LST_PT_INTERSEC_MESH);

}

void CWellPath::InvalidatePointLists()
{
	GetWellPointList(LST_PT_DSTABOR).RemoveAllPoints();
	GetWellPointList(LST_PT_DCASING).RemoveAllPoints();
	GetWellPointList(LST_PT_GRAPH).RemoveAllPoints();
  m_graphPointsCalculated = false;
	GetWellPointList(LST_PT_INTERSEC_MESH).RemoveAllPoints();
	GetWellPointList(LST_PT_INTERSEC_FORM).RemoveAllPoints();
	GetWellPointList(LST_PT_FIST_EXP).RemoveAllPoints();
	GetWellSectionList(LST_SC_FORM).RemoveAllSections();
	GetWellSectionList(LST_SC_WELL).RemoveAllSections();
}

// This function calculates the intersection points with the mesh
// invalidates the dependent lists (formation intersec points and formation sections)
void CWellPath::CalculateMeshIntersecPoints()
{
	InvalidatePointLists();
  assert(GetWellPointList(LST_PT_INTERSEC_MESH).NrOfPoints() == 0);

	if(!dynamic_cast<CModelBase*>(&Model())->IsMesh())
		return;

	well::CWellPointList lst(*this);
	lst.AutoDelete(true);

	//loop over the volumes in a formation

	CalculateIntersections(dynamic_cast<CModelBase*>(&Model())->Mesh().Mesh(), lst);
	lst.RemoveUnDefinedPoints();

	//copy the points in the mesh intersec
	well::CWellPointList::Iterator it =lst.begin();
	while(it != lst.end())
	{
  	well::CWellPoint* p = *it;
    CGeoWellPoint* pWP = new CGeoWellPoint(*this,p->TMD().Value());
    pWP->setElements(p->getElements());
		GetWellPointList(LST_PT_INTERSEC_MESH).AddPoint(pWP);
		++it;
	}

	GetWellPointList(LST_PT_INTERSEC_MESH).RemoveUnDefinedPoints();
	//UpdateFormationSections();
}

// This function calculates the intersection points with the horizons/formations
// it uses the mesh intersection points
void CWellPath::CalculateFormationIntersecPoints()
{
	GetWellSectionList(LST_SC_FORM).RemoveAllSections();

	well::CWellPointList& lst = GetWellPointList(LST_PT_INTERSEC_FORM);
	lst.RemoveAllPoints();

	//well::CWellPointList& mesh_intersec = MeshInter
  const well::CWellPointList& wpl = MeshIntersecPoints();
	well::CWellPointList::Iterator it = wpl.begin();

	CGeoWellPoint* p1 = 0;
	CGeoWellPoint* p2 = 0;

  if(it != wpl.end())
    p1 = (CGeoWellPoint*)*it++;
  if(it != wpl.end())
    p2 = (CGeoWellPoint*)*it;

	const CFormationBase* pForm=0;

	while(p1 && p2)
	{
		//take a point in the middle
		double middle = (p1->TMD().Value() + p2->TMD().Value())/2.0;
		CGeoWellPoint mid1(*this,middle);
		const CFormationBase* form = mid1.Formation();

		// wjrx mantis 2540
		// If the midpoint is not in the formation look near p1 and p2
		//
		if ( ! form )
		{
			middle = p1->TMD().Value() + 0.01 * (p2->TMD().Value() - p1->TMD().Value());
			CGeoWellPoint mid2(*this,middle);
			form = mid2.Formation();
		}

		// wjrx mantis 2540
		if ( ! form )
		{
			middle = p1->TMD().Value() + 0.99 * (p2->TMD().Value() - p1->TMD().Value());
			CGeoWellPoint mid3(*this,middle);
			form= mid3.Formation();
		}

		if(!form)
		{
			lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			pForm = form;
		}
		else if(p1 == MeshIntersecPoints().First() /*wjrx mantis 2540 taken out: && IsVertical() */)
		{
			lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			pForm = form;
		}
		else
		{
			if(!pForm)
			{
				pForm = form;
			}
			else if(pForm != form)
			{
				pForm = form;
				lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			}
		}
		
		p1 = p2;
    ++it;
    if(it != wpl.end())
		  p2 = (CGeoWellPoint*)*it;
    else
      p2 = 0;

    // wedx 09052008: add the last point
    if(!p2)
    {
      if(DefPointList().Last()->TMD().Value() > p1->TMD().Value() + EPS)
      {
    		CGeoWellPoint* p = new CGeoWellPoint(*this,DefPointList().Last()->TMD().Value());
        if(p->Defined())
        {
          lst.AddPoint(p);
        }
        else
        {
          lst.AddPoint(new CGeoWellPoint(*this, p1->TMD().Value()));
          delete p;
        }
      }
      else
        lst.AddPoint(new CGeoWellPoint(*this, p1->TMD().Value()));
    }

	}

}

//This function creates the formation-sections,based on the formation intersection points
void CWellPath::UpdateFormationSections()
{
//  m_pCachingIntermediate->Clear();

	// The FormIntersecPoints() call below can lead to an
	// InvalidatePointLists() call if we don't Calculate... here.
	// This interferes with drawing the wellpath.
	// wjrx mantis 2555
	if(GetWellPointList(LST_PT_INTERSEC_FORM).NrOfPoints() ==0)
		CalculateFormationIntersecPoints();

	CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());

	GetWellSectionList(LST_SC_WELL).RemoveAllSections();

	if(DefPointList().First() && DefPointList().Last())
	{
		well::CWellSectionList& well_section = GetWellSectionList(LST_SC_WELL);
		
		well::CWellSection* pSection = new well::CWellSection(*this, DefPointList().First()->TMD().Value(), DefPointList().Last()->TMD().Value());
		well_section.AddSection(pSection);
    m_pIntermediate->AddSection(*pSection, IIntermediate::LINE, Color(), 3, true, QString());
	}

	well::CWellSectionList& sections = GetWellSectionList(LST_SC_FORM);
	sections.RemoveAllSections();//wjrx mantis 2554

	if(!pModel->IsMesh())
		return;

	well::CWellPointList& formIntersec = FormIntersecPoints();
	if(formIntersec.NrOfPoints() == 0)
		return;

	well::CWellPointList& lst = formIntersec;

	assert(lst.NrOfPoints() > 0);
	assert(DefPointList().NrOfPoints() > 0);

/* wedx mantis #2588: this seems to introduce very short formation sections at the beginning and/or the end
	if(!(*lst.First() == *DefPointList().First()))
	{
		CGeoWellPoint p(*this,DefPointList().First()->TMD().Value());
		if(p.Defined())
			lst.AddPoint( DefPointList().First());
	}

	if(!(*lst.Last() == *DefPointList().Last()))
	{
		CGeoWellPoint p(*this,DefPointList().Last()->TMD().Value());
		if(p.Defined())
			lst.AddPoint( DefPointList().Last());
	}
*/
	//need at least 2 intersection points to create a section
	if(lst.NrOfPoints() < 2)
		return;

	well::CWellPointList::Iterator it = lst.begin();

	CGeoWellPoint*  top = 0;
	CGeoWellPoint*  bottom = 0;

  if(it != lst.end())
    top = (CGeoWellPoint*)*it++;

  if(it != lst.end())
    bottom = (CGeoWellPoint*)*it;
	
	while(top && bottom)
	{
		CGeoWellPoint mid(*this,(top->TMD().Value() + bottom->TMD().Value())/2.0);

		assert(mid.well::CWellPoint::Defined());

		if(!mid.Formation())
		{ // this only happens when there is a void in the model caused by a fault made up from two surfaces
			top    = bottom;
      ++it;
      if(it != lst.end())
			  bottom = (CGeoWellPoint*)*it;
      else
        bottom = 0;
			continue;
		}
		// Link to formations to propagate Modified() signals. TODO: unlink formations again.
		if(!IsLinkedTo( *const_cast<CFormationBase*>(mid.Formation()) ))
			LinkTo( *const_cast<CFormationBase*>(mid.Formation()) );

		CFormationSection* pSection = new CFormationSection(*this, mid.Formation(), top, bottom);
		//pSection->SetDescription(QString(above.Formation()->Name()));
		sections.AddSection(pSection);	

//		FORMATION COLORS		
    m_pIntermediate->AddSection(*pSection, IIntermediate::LINE, Color(), 3, true, QString());

//		FORMATION ARROWS
    m_pIntermediate->AddSection(*pSection, IIntermediate::ARROW, qRgb(255, 255, 255), 1, true, mid.Formation()->Name());
/*
		CWellSectionDrawSpec& ds2 = m_pSceneInterMed->AddSection(*pSection,CWellSectionDrawSpec::ARROW);
		ds2.SetColor(qRgb(255,255,255));
		ds2.SetDescription(QString(mid.Formation()->Name()));
		ds2.SetLineWidth(1);
		ds2.SetVisible(true);
*/

		top    = bottom;

    ++it;
    if(it != lst.end())
		  bottom =(CGeoWellPoint*)*it;
    else
      bottom = 0;

	}

  m_bSectionsDirty = false;

#ifdef _DEBUG	
	//debug
/*	//if(true) wjrx mantis 2542 Code moved to ShowDsbAnalysisPointsInScene 
	{
		well::CWellPointList& points = MeshIntersecPoints();
		well::CWellPointList::Iterator itmp  = points.GetIterator();
		well::CWellPoint* p = itmp.current();
		while(p)
		{
			
			CWellPointDrawSpec& wpds = m_pSceneInterMed->AddPoint(*p , CWellPointDrawSpec::SINGLE_DOT);
			wpds.SetPointSize(8);
			wpds.SetColor(qRgb(255,0,0));
			wpds.SetVisible(true);
			p = ++itmp;
		}
	}
*/
	//debug
	if(true)
	{
		well::CWellPointList& points = DefPointList();
		well::CWellPointList::Iterator itmp  = points.begin();
		while(itmp != points.end())
		{
  		well::CWellPoint* p = *itmp;
      m_pIntermediate->AddPoint(*p, qRgb(0, 0, 255), 8, false);
/*
			CWellPointDrawSpec& wpds = m_pSceneInterMed->AddPoint(*p , CWellPointDrawSpec::SINGLE_DOT);
			wpds.SetPointSize(8);
			wpds.SetColor(qRgb(0,0,255));
*/
			//wpds.SetVisible(true);
			++itmp;
		}
	}
#endif

}

CWellPath::TColor CWellPath::Color() const
{
	return CColorNode::Color();
}

/*
void CWellPath::SetSectionColor(TColor nColor, long SectionListID)
{
	well::CWellSectionList& sections = GetWellSectionList(SectionListID);

	well::CWellSectionList::Iterator it =  sections.begin();

	while(it != sections.end())
	{
		long count=0;
		while(m_pSceneInterMed->ExistSection(**it,count))
		{
			CWellSectionDrawSpec* spec = &(m_pSceneInterMed->GetSection(**it,count));
			if(!(spec->GetType() == CWellSectionDrawSpec::ARROW))
			{
				spec->SetColor(nColor);
			}
			count++;
		}
		++it;
	}
}
*/
void CWellPath::Color(TColor nColor)
{
	CColorNode::Color(nColor);
  m_pIntermediate->Color(nColor);
/*
	SetSectionColor(nColor, LST_SC_FORM);
	SetSectionColor(nColor, LST_SC_WELL);
*/
}

const well::CWellSectionList& CWellPath::FormationSections() const
{
	if(m_bSectionsDirty || GetWellSectionList(LST_SC_FORM).NrOfSections() == 0)
		const_cast<CWellPath*>(this)->UpdateFormationSections();

	return GetWellSectionList(LST_SC_FORM);
}

well::CWellSectionList& CWellPath::FormationSections()
{
	if(m_bSectionsDirty || GetWellSectionList(LST_SC_FORM).NrOfSections() == 0)
		UpdateFormationSections();

	return GetWellSectionList(LST_SC_FORM); 
}

well::CWellSectionList& CWellPath::FormationSectionsForLoadingOldFiles()
{
  if(GetWellSectionList(LST_SC_FORM).NrOfSections() == 0)
    UpdateFormationSectionsForLoadingOldFiles();

  return GetWellSectionList(LST_SC_FORM);
}

// wedx 18012008:
// Retain the functionality for updating formation sections from before file version 3.0.93 to
// be able to load these older files, as the loading order depended on the way this function was
// implemented. After modification of the original function (UpdateFormationSections) these
// older files wouldn't load anymore since formation sections were suddenly found in the new
// implementation.
void CWellPath::UpdateFormationSectionsForLoadingOldFiles()
{
//  m_pCachingIntermediate->Clear();

	// The FormIntersecPoints() call below can lead to an
	// InvalidatePointLists() call if we don't Calculate... here.
	// This interferes with drawing the wellpath.
	// wjrx mantis 2555
	if(GetWellPointList(LST_PT_INTERSEC_FORM).NrOfPoints() ==0)
		CalculateFormationIntersecPointsForLoadingOldFiles();

	CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());

	GetWellSectionList(LST_SC_WELL).RemoveAllSections();

	if(DefPointList().First() && DefPointList().Last())
	{
		well::CWellSectionList& well_section = GetWellSectionList(LST_SC_WELL);
		
		well::CWellSection* pSection = new well::CWellSection(*this, DefPointList().First()->TMD().Value(), DefPointList().Last()->TMD().Value());
		well_section.AddSection(pSection);
    m_pIntermediate->AddSection(*pSection, IIntermediate::LINE, Color(), 3, true, QString());
/*
		CWellSectionDrawSpec* ds = &m_pSceneInterMed->AddSection(*pSection);
		ds->SetColor(Color());
		ds->SetLineWidth(3);
		ds->SetVisible(true);
*/
	}

	well::CWellSectionList& sections = GetWellSectionList(LST_SC_FORM);
  sections.RemoveAllSections();

	if(!pModel->IsMesh())
		return;

	well::CWellPointList& formIntersec = FormIntersecPointsForLoadingOldFiles();
	if(formIntersec.NrOfPoints() == 0)
		return;

	well::CWellPointList lst = formIntersec;
	lst.AutoDelete(false);

	assert(lst.NrOfPoints() > 0);
	assert(DefPointList().NrOfPoints() > 0);

	if(!(*lst.First() == *DefPointList().First()))
	{
		CGeoWellPoint p(*this,DefPointList().First()->TMD().Value());
		if(p.Defined())
			lst.AddPoint( DefPointList().First());
	}

	if(!(*lst.Last() == *DefPointList().Last()))
	{
		CGeoWellPoint p(*this,DefPointList().Last()->TMD().Value());
		if(p.Defined())
			lst.AddPoint( DefPointList().Last());
	}


	//need at least 2 intersection points to create a section
	if(lst.NrOfPoints() < 2)
		return;

	well::CWellPointList::Iterator it = lst.begin();

	CGeoWellPoint*  top = 0;
	CGeoWellPoint*  bottom = 0;

  if(it != lst.end())
    top = (CGeoWellPoint*)*it++;
  if(it != lst.end())
    bottom = (CGeoWellPoint*)*it;
	
	while(top && bottom)
	{
		CGeoWellPoint mid(*this,(top->TMD().Value() + bottom->TMD().Value())/2.0);

		assert(mid.well::CWellPoint::Defined());

		if(!mid.Formation())
		{ // this only happens when there is a void in the model caused by a fault made up from two surfaces
			top    = bottom;
      ++it;
      if(it != lst.end())
			  bottom = (CGeoWellPoint*)*it;
      else
        bottom = 0;
			continue;
		}
		// Link to formations to propagate Modified() signals. TODO: unlink formations again.
		if(!IsLinkedTo( *const_cast<CFormationBase*>(mid.Formation()) ))
			LinkTo( *const_cast<CFormationBase*>(mid.Formation()) );

		CFormationSection* pSection = new CFormationSection(*this, mid.Formation(), top, bottom);
		//pSection->SetDescription(QString(above.Formation()->Name()));
		sections.AddSection(pSection);	

//		FORMATION COLORS		
    m_pIntermediate->AddSection(*pSection, IIntermediate::LINE, Color(), 3, true, QString());
/*
		CWellSectionDrawSpec& ds = m_pSceneInterMed->AddSection(*pSection);
		//ds.SetColor(mid.Formation()->Color());
		ds.SetColor(Color());
		ds.SetLineWidth(3);
		ds.SetVisible(true);
*/

//		FORMATION ARROWS
    m_pIntermediate->AddSection(*pSection, IIntermediate::ARROW, qRgb(255, 255, 255), 1, true, mid.Formation()->Name());
/*
		CWellSectionDrawSpec& ds2 = m_pSceneInterMed->AddSection(*pSection,CWellSectionDrawSpec::ARROW);
		ds2.SetColor(qRgb(255,255,255));
		ds2.SetDescription(QString(mid.Formation()->Name()));
		ds2.SetLineWidth(1);
		ds2.SetVisible(true);
*/

		top    = bottom;
    ++it;
    if(it != lst.end())
		  bottom = (CGeoWellPoint*)*it;
    else
      bottom = 0;

	}

  m_bSectionsDirty = false;
}

well::CWellPointList& CWellPath::FormIntersecPointsForLoadingOldFiles()
{
	if(GetWellPointList(LST_PT_INTERSEC_FORM).NrOfPoints() ==0)
		CalculateFormationIntersecPointsForLoadingOldFiles();

	return GetWellPointList(LST_PT_INTERSEC_FORM);
}

// This function calculates the intersection points with the horizons/formations
// it uses the mesh intersection points
void CWellPath::CalculateFormationIntersecPointsForLoadingOldFiles()
{
	GetWellSectionList(LST_SC_FORM).RemoveAllSections();

	well::CWellPointList& lst = GetWellPointList(LST_PT_INTERSEC_FORM);
	lst.RemoveAllPoints();

	//well::CWellPointList& mesh_intersec = MeshInter
  well::CWellPointList& wpl = MeshIntersecPointsForLoadingOldFiles();
	well::CWellPointList::Iterator it = wpl.begin();
	CGeoWellPoint* p1 = 0;
	CGeoWellPoint* p2 = 0;

  if(it != wpl.end())
    p1 = (CGeoWellPoint*)*it++;
  if(it != wpl.end())
    p2 = (CGeoWellPoint*)*it;

  const CFormationBase* pForm=0;

	while(p1 && p2)
	{
		//take a point in the middle
		double middle = (p1->TMD().Value() + p2->TMD().Value())/2.0;
		CGeoWellPoint mid(*this,middle);

		const CFormationBase* form = mid.Formation();
		//assert(mid.Formation());
		if(!form)
		{
			lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			pForm = form;
		}
		else if(p1 == MeshIntersecPointsForLoadingOldFiles().First() && IsVertical())
		{
			lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			pForm = form;
		}
		else
		{
			if(!pForm)
			{
				pForm = form;
			}
			else if(pForm != form)
			{
				pForm = form;
				lst.AddPoint(new CGeoWellPoint(*this,p1->TMD().Value()));
			}
		}
		
		p1 = p2;
    ++it;
    if(it != wpl.end())
  		p2 = (CGeoWellPoint*)*it;
    else
      p2 = 0;

    // add the last point
    if(!p2)
    {
      if(DefPointList().Last()->TMD().Value() > p1->TMD().Value() + EPS)
      {
    		CGeoWellPoint* p = new CGeoWellPoint(*this,DefPointList().Last()->TMD().Value());
        if(p->Defined())
        {
          lst.AddPoint(p);
        }
        else
        {
          lst.AddPoint(new CGeoWellPoint(*this, p1->TMD().Value()));
          delete p;
        }
      }
      else
        lst.AddPoint(new CGeoWellPoint(*this, p1->TMD().Value()));
    }

	}

}

well::CWellPointList& CWellPath::MeshIntersecPointsForLoadingOldFiles()
{
	if(GetWellPointList(LST_PT_INTERSEC_MESH).NrOfPoints() ==0)
		const_cast<CWellPath*>(this)->CalculateMeshIntersecPointsForLoadingOldFiles();

	return GetWellPointList(LST_PT_INTERSEC_MESH);

}

void CWellPath::CalculateMeshIntersecPointsForLoadingOldFiles()
{
	InvalidatePointLists();

	if(!dynamic_cast<CModelBase*>(&Model())->IsMesh())
		return;

	well::CWellPointList lst(*this);
	lst.AutoDelete(true);

	//loop over the volumes in a formation

	CalculateIntersections(dynamic_cast<CModelBase*>(&Model())->Mesh().Mesh(), lst);
	lst.RemoveUnDefinedPoints();

	//copy the points in the mesh intersec
	well::CWellPointList::Iterator it =lst.begin();
	while(it != lst.end())
	{
  	well::CWellPoint* p = *it;
		GetWellPointList(LST_PT_INTERSEC_MESH).AddPoint(new CGeoWellPoint(*this,p->TMD().Value()));
		++it;
	}

	GetWellPointList(LST_PT_INTERSEC_MESH).RemoveUnDefinedPoints();
}


//well::IWellSection &CWellPath::Section(const CFormationBase *pFormation) const
//{
//	well::CWellSectionList list = FormationSections();
//	well::CWellSectionList::Iterator it = list.GetIterator();
//
//}


//double CResultRegister::MaterialParameterValue(int nElementIndex, unsigned int ValueTypeID) const
//{
//	const geo::IElement &element = Mesh().ElementSet().Element(nElementIndex);
//	const CFormationBase &form = Mesh().Formation(element);
//	return form.Material().Material(element).ParameterValue(ValueTypeID);
//}

void CWellPath::slotOnFinished()
{

}

well::CWellDefinitionPointList& CWellPath::DefPointList()
{

	if(CWellPathBase::DefPointList().NrOfPoints()==0 && !m_VerticalPointDef.Empty())
		InitVerticalWell(false);
	return CWellPathBase::DefPointList();
}

const well::CWellDefinitionPointList& CWellPath::DefPointList() const
{
	if(CWellPathBase::DefPointList().NrOfPoints()==0 && !m_VerticalPointDef.Empty())
		const_cast<CWellPath*>(this)->InitVerticalWell(false);
	
	

	return CWellPathBase::DefPointList();
}

void CWellPath::InitVerticalWell(bool bResetGlobalTVD)
{
	assert(!m_VerticalPointDef.Empty());

	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());

	if(!pModel->IsMesh())
		return;

	double Zmax = pModel->Mesh().Mesh().Max().Z();
	double Zmin = pModel->Mesh().Mesh().Min().Z();
	
	double maxZ = Zmax +(Zmax-Zmin)*0.5;
	double minZ = Zmin -(Zmax-Zmin)*0.5;

	geo::CPoint p1(m_VerticalPointDef.X(),m_VerticalPointDef.Y(),minZ); //
	geo::CPoint p2(m_VerticalPointDef.X(),m_VerticalPointDef.Y(),maxZ); //
	geo::CLine line(p1,p2);
	std::set<geo::CPoint> PointSet;
	GetInterSectionPointsWithFormationSkin(line,PointSet);		
	
	if(PointSet.size() == 0)
		return;

	const geo::CPoint* first = &*PointSet.begin();
	const geo::CPoint* last  = &*PointSet.rbegin();

	//initialization of a vertical wellpath (SI_UNIT)
//	InitVertical(	first->X(),
//					first->Y(),
//					first->Z(),
//					0,
//					last->Z());

	InitVertical(	first->X(),
					first->Y(),
					bResetGlobalTVD ? first->Z() : GlobalTVD().Value(),
					0,
					last->Z());

	UpdateFormationSections();
	m_display_list.ClearAndDelete();
}

void CWellPath::Recalculate()
{
	std::auto_ptr <IProgressBase> wait(_g->prog()->create(eProgress::Wait));

	ClearLogdataMaps();
	DsbResults().ClearAllResults();

	GetWellSectionList(LST_SC_FORM).RemoveAllSections();
	GetWellSectionList(LST_SC_WELL).RemoveAllSections();
	
	GetWellPointList(LST_PT_GRAPH).RemoveAllPoints();
  m_graphPointsCalculated = false;
	GetWellPointList(LST_PT_INTERSEC_FORM).RemoveAllPoints();
	GetWellPointList(LST_PT_INTERSEC_MESH).RemoveAllPoints();
	GetWellPointList(LST_PT_DSTABOR).RemoveAllPoints();
	GetWellPointList(LST_PT_DCASING).RemoveAllPoints();
	GetWellPointList(LST_PT_FIST_EXP).RemoveAllPoints();
	
	if(!m_VerticalPointDef.Empty())
		InitVerticalWell(false);

//TODO  std::set<COpenGLSceneBase*> stScenes = Links<COpenGLSceneBase>();
//TODO  if(!stScenes.empty())
    FormationSections(); // force update if necessary
}

//set a new x,y point for this vertical wellpath
void CWellPath::Recalculate(const geo::CPoint &NewPoint)
{
	CWellPathBase::DefPointList().RemoveAllPoints();
	m_VerticalPointDef=NewPoint;
	Recalculate();
}

//wjrx mantix 2542
void CWellPath::UpdateDsbAnalysisPointsInScene()
{
  m_pIntermediate->RemoveAllPoints();
//	m_pSceneInterMed->RemoveAllPoints();

	if(m_bShowDsbPointsInScene)
	{
		well::CWellPointList& points = DStaborPoints();
		well::CWellPointList::Iterator itmp  = points.begin();

		while(itmp != points.end())
		{
  		well::CWellPoint* p = *itmp;
      m_pIntermediate->AddPoint(*p, qRgb(255, 0, 0), 8, true);
/*
			CWellPointDrawSpec& wpds =
			  m_pSceneInterMed->AddPoint(*p,
					  CWellPointDrawSpec::SINGLE_DOT);
			wpds.SetPointSize(8);
			wpds.SetColor(qRgb(255,0,0));
			wpds.SetVisible(true);
*/
			++itmp;
		}
	}
}

void CWellPath::ShowDsbAnalysisPointsInScene(bool bShow)
{
	m_bShowDsbPointsInScene=bShow;
	UpdateDsbAnalysisPointsInScene();

	m_display_list.ClearAndDelete();
	Modified();
}
bool CWellPath::ShowDsbAnalysisPointsInScene()
{
	return m_bShowDsbPointsInScene;
}


geo::CPoint CWellPath::MaxPoint() const
{
	return DefPointList().MaxPoint();
}

geo::CPoint CWellPath::MinPoint() const
{
	return DefPointList().MinPoint();
}


CWellPath& CWellPath::operator =(const CWellPath &rhs)
{
	assert(false); // 13-02-04 There is reason to believe that this is not used anymore, so this assert is a test!!
	CColorNode::operator =(rhs);

	//conot convert from vertiacal to non vertical
	assert(IsVertical() == rhs.IsVertical());

	//see if the position is changed
	bool bPosChanged=false;

	m_DsbLiquidFluid=rhs.m_DsbLiquidFluid;
	m_DsbFluidLossCoefficient=rhs.m_DsbFluidLossCoefficient;


	if(fabs(GlobalNorthing().Value()-rhs.GlobalNorthing().Value()) > EPS)
		bPosChanged=true;

	if(fabs(GlobalEasting().Value()-rhs.GlobalEasting().Value()) > EPS)
		bPosChanged=true;


	m_bShowDsbPointsInScene=rhs.m_bShowDsbPointsInScene;

	//invalidate position specific data because position is changed
	if(bPosChanged)
	{
		assert(IsVertical());
		Recalculate(geo::CPoint(rhs.GlobalEasting().Value(),rhs.GlobalNorthing().Value()));
	}

	if(m_dMudGradRefDepth != rhs.m_dMudGradRefDepth)
	{
		m_dMudGradRefDepth = rhs.m_dMudGradRefDepth;

		if(m_dMudGradRefDepth > MinPoint().Z())
			m_dMudGradRefDepth=MinPoint().Z();

		DsbResults().ClearAllResults();
	}

	m_display_list.ClearAndDelete();
	Modified();
	return *this;

}

bool CWellPath::operator ==(const CWellPath &rhs) const
{
	if(!(CColorNode::operator ==(rhs)))
		return false;

	if(!(fabs(m_dMudGradRefDepth - rhs.m_dMudGradRefDepth) < EPS))
		return false;

	if(m_bShowDsbPointsInScene != rhs.m_bShowDsbPointsInScene)
		return false;

	if(!(fabs(m_DsbLiquidFluid - rhs.m_DsbLiquidFluid) < EPS))
		return false;

	if(!(fabs(m_DsbFluidLossCoefficient - rhs.m_DsbFluidLossCoefficient) < EPS))
		return false;


	return true;
}


bool CWellPath::CanExportToFist() const
{
	const CModelBase &model = (const CModelBase&)(Model());
	if(!model.IsResult())
		return false;
	return FistExportPoints().NrOfPoints() > 0;
}

int CWellPath::DisplayListSize() const
{
	return 1;
}

const geo::IObject& CWellPath::DisplayList(int /*nIndex*/) const 
{
	return m_display_list;
}

geo::CPtrArray <geo::IObject>& CWellPath::DisplayList()
{
	return m_display_list;
}

unsigned int CWellPath::IconId() const
{
	return IDI_WELLPATH;
}

unsigned int CWellPath::TypeId() const
{
	return IDT_TREE_WELL_PATHS;
}

QString CWellPath::TypeName() const
{
	return getStringTableEntry(IDS_TREE_WELL_PATHS);
}

void CWellPath::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	// Load the base first
	CColorNode::LoadStream(stream, version, progress);

	// Set progress indicator
	std::string pr("Loading WellPath ");
	pr += Name().toStdString();
	progress.StatusMessage(pr.c_str());

	//Load Birth Date 

	if(CStreamVersion(3, 0, 39) < version)
	{
		int year;
		int month;
		int day;
		stream >> year;
		stream >> month;
		stream >> day;
		SetBirthDate(QDate(year, month, day));
	}
	

	// Load position and elevation

	double global_northing,global_easting,global_depth;

	stream >> global_northing;
	stream >> global_easting;
	stream >> global_depth;

	stream >> m_dMudGradRefDepth;
		
	// Is the well vertical?
	int bVertical;
	stream >> bVertical;

	if ( bVertical ) m_GlobalTVD.SetValue(global_depth);// wjrx mantis 2555

	if(bVertical && CStreamVersion(3, 0, 45) < version)
	{
		double dX;
		double dY;
		stream >> dX;
		stream >> dY;
		m_VerticalPointDef = geo::CPoint(dX,dY);
	}


	QVector<double> tmd;
	QVector<double> azi;
	QVector<double> inc;

	// Load points
	int nSize;
	stream >> nSize;
	tmd.resize(nSize);
	azi.resize(nSize);
	inc.resize(nSize);

	for(int i = 0; i < nSize; i++)
	{
		double x, y, z, azimuth, inclination, alongholedepth;

		stream >> x;
		stream >> y;
		stream >> z;
		stream >> azimuth;
		stream >> inclination;
		stream >> alongholedepth;
		tmd[i]=alongholedepth;
		azi[i]=azimuth;
		inc[i]=inclination;
		progress.Step();
	}

	if(nSize > 0 && !bVertical)
	{
		assert(nSize > 1);
		InitFromArray_tmd_azi_inc( global_northing,
							       global_easting,
							       global_depth,
							       0,0,0,
									tmd,
									azi,
									inc);
	}

  // wedx 15102008 (mantis #2646):
  // Force update of formation sections here so loaded point lists are not invalidated when
  // formation sections are requested later on
  if(version >= CStreamVersion(3, 0, 93))
    FormationSections();
  else
    FormationSectionsForLoadingOldFiles();
	
	// load the d-stabor points
	int nDStaborSize;
	stream >> nDStaborSize;
	srand((unsigned)time( NULL ));

	for(int i = 0; i < nDStaborSize; i++)
	{
		// Load the point
		double x, y, z, azimuth, inclination, alongholedepth;

		stream >> x;
		stream >> y;
		stream >> z;
		stream >> azimuth;
		stream >> inclination;
		stream >> alongholedepth;

		CGeoWellPoint* pPoint = new CDstaborWellPoint(*this, alongholedepth);
		DStaborPoints().AddPoint(pPoint);

		// Get the results 
		int bLoadRes;
		stream >> bLoadRes;

		while(bLoadRes)
		{
			// Load depletion stage
			int nIndex;
			CAnalysisType antype;
			stream >> nIndex;

			if(nIndex < 0 )  //old databse
				nIndex=0;
			
			CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
			CDepletionStage *pDepl = dep_entry.FindIndex(nIndex);
      int bLinear;

			if(pDepl==0) //old database
			{
				//fake reading
				double dum;
				int idum;
				stream >> bLinear;
				stream >> idum;
				stream >> dum;
				stream >> bLoadRes;
				continue;
			}
			int status;
			double value;
			// Load linearity tag, status and value
			stream >> bLinear;
      CDsbStatusValue &rs = DsbResults().GetResult(*pPoint, *pDepl, (bLinear ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN));
			stream >> status;
			stream >> value ;
			rs.Value(value, (CDsbStatusValue::eDsbStatus)status);
			
			// Continue?
			stream >> bLoadRes;
		}

		progress.Step();
	}
		progress.Step();

	stream >> m_DsbLiquidFluid;
	stream >> m_DsbFluidLossCoefficient;

	if(CStreamVersion(3, 0, 36) < version)
	{
    if(version < CStreamVersion(3, 8, 2))
    {
      // DCasing material index
		  int nMaterial;
		  stream >> nMaterial;

      // Load DCasingGeneralParameters (13 CDoubleQuantity values)

      int iDefined;
      double dValue;
      for(int i = 0; i < 13; ++i)
      {
	      stream >> iDefined;
	      if(iDefined)
		      stream >> dValue;
      }
/*
		  CDCasingGeneralParameters &param = Casing().GetGeneralParameters();

		  // basic parameters
		  Load(param.OuterDiameter(), stream);
		  Load(param.WallThickness(), stream);
		  Load(param.DriftingDiameter(), stream);
		  Load(param.RequiredDrifting(), stream);
		  Load(param.CementYoungsModulus(), stream);
		  Load(param.CementPoissonRatio(), stream);
		  Load(param.ReferenceDepth(), stream);
		  Load(param.ReferencePressure(), stream);
		  Load(param.PressureGradient(), stream);

		  // advanced parameters 
		  Load(param.PipeLengthOD(), stream);
		  Load(param.JointSpacing(), stream);
		  Load(param.ModelOutDiamBoreDiamRatio(), stream);
		  Load(param.MaxAxialCompStrain(), stream);
*/
		  QString str;
		  stream >> str;

		  int iBool;
		  stream >> iBool;
    }

	}

	if(CStreamVersion(3, 0, 46) < version)
		LoadMeshIntersectionNew(stream, version, progress);

	if((CStreamVersion(3, 0, 37) < version) && (version < CStreamVersion(3, 0, 42)))
		LoadMeshIntersection(stream,version,progress);
	
	if(CStreamVersion(3, 0, 42) < version) 
		LoadWellLogData(stream,version,progress);

  if(version < CStreamVersion(3, 8, 2))
	  LoadDCasint(stream,version,progress);

	if( CStreamVersion(3, 0, 79) < version ) {
		long NrOfFistPoints;
		stream >> NrOfFistPoints;

		well::CWellPointList& fist_list = GetWellPointList(LST_PT_FIST_EXP);
		for(int i =0;i<NrOfFistPoints;i++)
		{
			double dTmd;
			stream >> dTmd;
			CGeoWellPoint* p=new CGeoWellPoint(*this, dTmd);
			fist_list.AddPoint(p);
		}
	}

	if( CStreamVersion(3, 0, 91) < version ) //wjrx mantis 2549
	{
    double dDerElev;
		stream >> dDerElev;
    m_GlobalTVD.SetValue(dDerElev);
	}

  if(CStreamVersion(3, 0, 108) < version)
  {
    // link LAS pointsets
    int sz;
    stream >> sz;
    TPointSetEntry* psentry = static_cast<TPointSetEntry*>(Model().GraphEntry(MD_BASE_POINTSET));

    for(int i = 0; i < sz; ++i)
    {
      int idx;
      stream >> idx;
      IPointSet* pPointSet = psentry->FindIndex(idx);
      assert(dynamic_cast<const CLasPointSet*>(pPointSet));
      LinkTo(*pPointSet);
    }
  }

	CGraphNode::create(Model().GraphEntry(MD_BASE_WELLPATH));
	LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
	LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  if(CStreamVersion(3, 0, 109) < version)
  {
    int iHasZoomIn;
    stream >> iHasZoomIn;

    if(iHasZoomIn)
    {
      DestroyZoomInModel();
      CreateZoomInModel();
      assert(m_pZoomInModel);
      CModelBase& model = static_cast<CModelBase&>(Model());
      model.LoadChildModel(stream, version, progress, *m_pZoomInModel);
    }
  }
}

void CWellPath::LoadMeshIntersectionNew(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& /*progress*/)
{
	int NrOfInterSecPoints;
	stream >> NrOfInterSecPoints;

	for(int i =0;i<NrOfInterSecPoints;i++)
	{
		double tmd;
		stream >> tmd;
//		CGeoWellPoint* p=new CGeoWellPoint(*this,tmd);
//		mesh_intersec_list.AddPoint(p);
	}
}

void CWellPath::SaveMeshIntersectionNew(TSTREAM& stream, TPROGRESS& /*progress*/)
{
	well::CWellPointList& mesh_intersec_list = GetWellPointList(LST_PT_INTERSEC_MESH);
	stream << mesh_intersec_list.NrOfPoints();
	
	for(int i =0;i<mesh_intersec_list.NrOfPoints();i++)
	{
		stream << mesh_intersec_list.At(i)->TMD().Value();
	}

}

void CWellPath::LoadMeshIntersection(TSTREAM& stream, CStreamVersion& version, TPROGRESS& /*progress*/)
{
	//load the mesh intersection points
	long NrOfInterSecPoints;
	stream >> NrOfInterSecPoints;

	well::CWellPointList& mesh_intersec_list = GetWellPointList(LST_PT_INTERSEC_MESH);
  int i;
	for(i =0;i<NrOfInterSecPoints;i++)
	{
		double tmd;
		stream >> tmd;
		CGeoWellPoint* p=new CGeoWellPoint(*this,tmd);
		mesh_intersec_list.AddPoint(p);
	}

	//load the formation intersection points
	stream >> NrOfInterSecPoints;
	well::CWellPointList& form_intersec_list = GetWellPointList(LST_PT_INTERSEC_FORM);

	for(i =0;i<NrOfInterSecPoints;i++)
	{
		double tmd;
		stream >> tmd;
		CGeoWellPoint* p = new CGeoWellPoint(*this,tmd);
		form_intersec_list.AddPoint(p);
	}

	well::CWellPointList lst = form_intersec_list;
	lst.AutoDelete(false);
	int use_defpoint;
	
	if(CStreamVersion(3, 0, 38) < version)
	{
		stream >> use_defpoint;
		if(use_defpoint)
			lst.AddPoint( DefPointList().First());

		stream >> use_defpoint;
		if(use_defpoint)
			lst.AddPoint( DefPointList().Last());

	}

	// construct the formationsections
	well::CWellSectionList& sections = FormationSections();
	sections.RemoveAllSections();

	well::CWellPointList::Iterator it = lst.begin();
	CGeoWellPoint*  top = 0;
	CGeoWellPoint*  bottom = 0;

  if(it != lst.end())
    top = (CGeoWellPoint*)*it++;
  if(it != lst.end())
    bottom = (CGeoWellPoint*)*it;
	
	while(top && bottom)
	{
		TFormationBaseEntry& formation_entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
		int index;
		stream >> index;
		CFormationBase* pForm = formation_entry.FindIndex(index);
		assert(pForm);
		
		CFormationSection* pSection = new CFormationSection(*this, pForm, top, bottom);
		sections.AddSection(pSection);	
		
    m_pIntermediate->AddSection(*pSection, IIntermediate::LINE, pForm->Color(), 3, true, QString());
/*
		CWellSectionDrawSpec& ds = m_pSceneInterMed->AddSection(*pSection);
		ds.SetColor(pForm->Color());
		ds.SetLineWidth(3);
		ds.SetVisible(true);
*/

    m_pIntermediate->AddSection(*pSection, IIntermediate::ARROW, qRgb(255, 255, 255), 1, true, pForm->Name());
/*
		CWellSectionDrawSpec& ds2 = m_pSceneInterMed->AddSection(*pSection,CWellSectionDrawSpec::ARROW);
		ds2.SetColor(qRgb(255,255,255));
		ds2.SetDescription(QString(pForm->Name()));
		ds2.SetLineWidth(1);
		ds2.SetVisible(true);
*/

		top    = bottom;
    ++it;
    if(it != lst.end())
		  bottom = (CGeoWellPoint*)*it;
    else
      bottom = 0;
	}

}


void CWellPath::SaveWellLogData(TSTREAM& stream, TPROGRESS& progress)
{
	well::CWellSectionList& formations = FormationSections();

  // wedx 18012008:
  // Save the status of the formation sections, so we can load it
  // instead of recalculating
  int iHasFormationSections = (formations.NrOfSections() > 0 ? 1 : 0);
  stream << iHasFormationSections;

	if(iHasFormationSections)
	{
		// Saving of the wellog data and the maps........
		CModelBase &model = (CModelBase&)(Model());
		TSectDeplLogMap::iterator it_1;
		int nStageIndex = 0;
		int nSectionIndex = 0;

		well::CWellSectionList::Iterator form_it = formations.begin();

		stream << int(m_mpSectDplLog.size());
		for(it_1 = m_mpSectDplLog.begin(); it_1 != m_mpSectDplLog.end(); it_1++)
		{
			
			CDepletionStage *pStage = &model.InitialDepletionStage();
			while(pStage)
			{
				if(pStage == it_1->second.first)
				{
					stream << nStageIndex;
					break;
				}
				nStageIndex++;
				if(pStage->Last())
					pStage = 0;
				else
					pStage = &pStage->Next();
			}
			
			form_it = formations.begin(); // reset the formation iterator...
			while(form_it != formations.end())
			{
				if(*form_it == it_1->first)
				{
					stream << nSectionIndex;
					break;
				}
				nSectionIndex++;
				++form_it;
			}
			
			SaveWellLog(it_1->second.second, stream, progress); // progress is increased in this function...
		}
		
/*
		nSectionIndex = 0;
		stream << int(m_mpSectEpsType.size());
		TSectEpsTypeMap::iterator it_2;
		for(it_2 = m_mpSectEpsType.begin(); it_2 != m_mpSectEpsType.end(); it_2++)
		{
			form_it = formations.begin(); // reset the formation iterator
			while(form_it != formations.end())
			{
				if(*form_it == it_2->first)
				{
					stream << nSectionIndex;
					break;
				}
				nSectionIndex++;
				++form_it;
			}

			// save the DCASINT_EPS_TYPE.....
			stream << (int)(it_2->second);
			progress.Step();
		}
*/
		nSectionIndex = 0;
		stream << int(m_mpCorrectAllStages.size());
		std::map<CFormationSection*, bool>::iterator it_3;
		for(it_3 = m_mpCorrectAllStages.begin(); it_3 != m_mpCorrectAllStages.end(); it_3++)
		{
			form_it = formations.begin(); // reset the formation iterator
			while(form_it != formations.end())
			{
				if(*form_it == it_3->first)
				{
					stream << nSectionIndex;
					break;
				}
				nSectionIndex++;
				++form_it;
			}

			if(it_3->second)
				stream << 0;
			else
				stream << 1;

			progress.Step();
		}
	}



}

void CWellPath::LoadWellLogData(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(version == CStreamVersion(3, 0, 38))
	{
		int nMapSize;
		stream >> nMapSize;
		stream >> nMapSize;
		stream >> nMapSize;
		return;
	}

	if(!(CStreamVersion(3, 0, 37) < version))
		return;


  if(version >= CStreamVersion(3, 0, 93))
  {
    // wedx 18012008:
    // Load formation sections status from file
    // Return if no sections are available
    int iHasFormationSections;
    stream >> iHasFormationSections;
    if(!iHasFormationSections)
      return;
  }
  else
  {
    // make sure no sections exist
    GetWellSectionList(LST_SC_FORM).RemoveAllSections();
    assert(GetWellSectionList(LST_SC_FORM).NrOfSections() == 0);
  	GetWellSectionList(LST_SC_WELL).RemoveAllSections();
    GetWellPointList(LST_PT_INTERSEC_FORM).RemoveAllPoints();
    GetWellPointList(LST_PT_INTERSEC_MESH).RemoveAllPoints();

	  bool bContinue = (FormationSectionsForLoadingOldFiles().NrOfSections() > 0);

    // remove all generated sections, so the new functionality is actually used
    // when it's needed
    GetWellSectionList(LST_SC_FORM).RemoveAllSections();
    assert(GetWellSectionList(LST_SC_FORM).NrOfSections() == 0);
  	GetWellSectionList(LST_SC_WELL).RemoveAllSections();
    GetWellPointList(LST_PT_INTERSEC_FORM).RemoveAllPoints();
    GetWellPointList(LST_PT_INTERSEC_MESH).RemoveAllPoints();

    if(!bContinue)
		  return;
  }

	// Loading of the log data and creating of the maps....
	CModelBase &model = (CModelBase&)(Model());
	well::CWellSectionList& formations = FormationSections();
	well::CWellSectionList::Iterator form_it = formations.begin();
	int nSectionIndex, nSectionNr, nStageIndex, nStageNr;
	int nMapSize;
	stream >> nMapSize;

	for(int d = 0; d < nMapSize; d++)
	{
		stream >> nStageNr;
		nStageIndex = 0;
		CDepletionStage *pStage = &model.InitialDepletionStage();
		while(pStage)
		{
			if(nStageIndex == nStageNr)
				break;
			
			nStageIndex++;
			if(pStage->Last())
				pStage = 0;
			else
				pStage = &pStage->Next();
		}
		
		stream >> nSectionNr;
		nSectionIndex = 0;
		form_it = formations.begin();
		while(form_it != formations.end())
		{
			if(nSectionIndex == nSectionNr)
				break;

			nSectionIndex++;
			++form_it;
		}

		well::CWellLog *pLog = new well::CWellLog(this,(CFormationSection*)(*form_it));
		LoadWellLog(pLog, stream, progress); // progress is increased in this function

		m_mpSectDplLog.insert(TSectDeplLogMap::value_type((CFormationSection*)(*form_it), TStageLogPair(pStage, pLog)));
	}

  if(version < CStreamVersion(3, 8, 2))
  {
	  stream >> nMapSize;
	  //DCASINT_EPS_TYPE 
	  int eps_type;
	  //int nEps;
	  for(int j = 0; j < nMapSize; j++)
	  {
		  stream >> nSectionNr;
		  stream >> eps_type;
		  progress.Step();
	  }
  }

	stream >> nMapSize;
	int correction;
	for(int k = 0; k < nMapSize; k++)
	{
		stream >> nSectionNr;
		nSectionIndex = 0;
		form_it = formations.begin();
		while(form_it != formations.end())
		{
			if(nSectionIndex == nSectionNr)
				break;

			nSectionIndex++;
			++form_it;
		}

		stream >> correction;
		m_mpCorrectAllStages.insert(std::map<CFormationSection*, bool>::value_type((CFormationSection*)(*form_it), correction != 0));
		progress.Step();
	}

}

void CWellPath::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	// Let save base first
	CColorNode::SaveStream(stream, progress);

	// Update progress indicator
	std::string pr("Saving WellPath ");
	pr += Name().toStdString();
	progress.StatusMessage(pr.c_str());
	
	// Save birth date
	stream << GetBirthDate().year(); 
	stream << GetBirthDate().month();
	stream << GetBirthDate().day();

	// Save position
	stream << GlobalNorthing().Value();
	stream << GlobalEasting().Value();

	// Elevation
	stream << GlobalTVD().Value();

	//ref depth
	stream << MudGradRefDepth();

	// Vertical
	if(IsVertical())
	{
		stream << (int) TRUE; // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
		stream << m_VerticalPointDef.X();
		stream << m_VerticalPointDef.Y();
	}
	else
	{
		stream << (int) FALSE;  // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.
	}

	stream << CWellPathBase::DefPointList().NrOfPoints(); //from base class, does not trigger intersection calculation for vertical wellpaths
	// Save all wellpoints
	well::CWellPointList::Iterator it  = CWellPathBase::DefPointList().begin();
  while(it != CWellPathBase::DefPointList().end())
	{
  	well::CWellPoint* point = *it;

		// Save position
		stream << point->X() << point->Y() << point->Z();

		// Save values
		stream << point->Azimuth().Value() << point->Inclination().Value() << point->TMD().Value();

		progress.Step();
		++it;
	}
	

	// Save le d-stabor analysis
	it = DStaborPoints().begin();
	int nSize = DStaborPoints().NrOfPoints();
	stream << nSize;
	while(it != DStaborPoints().end())
	{ 
    well::CWellPoint* point = *it;
		// Save le location
		stream << point->X() << point->Y() << point->Z();

		// Save azimuth, inclination and alonghole depth
		stream << point->Azimuth().Value() << point->Inclination().Value() << point->TMD().Value();

		// Save results
		CDepletionStage *pStage = &(((CModelBase*)&Model())->InitialDepletionStage());
		while(pStage)
		{
      if(DsbResults().ResultExist(*point,*pStage,CAnalysisType::AT_LINEAR))
			{
        CDsbStatusValue &linval = DsbResults().GetResult(*point,*pStage,CAnalysisType::AT_LINEAR);
				if (linval.IsCalculated())
				{
					// Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
					stream << (int) TRUE;		// Start reading tag
					stream << pStage->Index();
					// Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
					stream << (int) TRUE;		// Linear
					stream << linval.Status();
					stream << linval.Value();
				}
			}

      if(DsbResults().ResultExist(*point,*pStage,CAnalysisType::AT_NONLIN))
			{
        CDsbStatusValue &nonlinval = DsbResults().GetResult(*point,*pStage,CAnalysisType::AT_NONLIN);
				if (nonlinval.IsCalculated())
				{
					// Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
					stream << (int) TRUE;		// Start reading tag
					stream << pStage->Index();
					// Qt\4.6.1\src\corelib\global\qglobal.h, line 920.
					stream << (int) FALSE;	// Non linear
					stream << (int)nonlinval.Status();
					stream << nonlinval.Value();
				}
			}
					
			if(pStage->Last())
				pStage = 0;
			else
				pStage = &pStage->Next();
		}

		// Delimit results of point  with end tag
		stream << (int) FALSE;  // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.

		progress.Step();
		++it;
	}

	stream << m_DsbLiquidFluid;
	stream << m_DsbFluidLossCoefficient;

	SaveMeshIntersectionNew(stream, progress);

	SaveWellLogData(stream,progress);	
//	SaveDCasint(stream,progress);

	// Fist points...
	well::CWellPointList& fist_list = GetWellPointList(LST_PT_FIST_EXP);
	stream << (long)fist_list.NrOfPoints();
	for(int i =0;i<fist_list.NrOfPoints();i++) stream << fist_list.At(i)->TMD().Value();

	//Derrick elevation wjrx mantis 2549
	stream << DerElev();

  // collect and save the LAS pointsets linked to the wellpath
  std::vector<const CLasPointSet*> vcLasPointSets;
  CGraphEntry* psentry = Model().GraphEntry(MD_BASE_POINTSET);
  CGraphEntry::TEntryNodeSet stNodes = psentry->GraphEntryNodes();
  CGraphEntry::TEntryNodeSet::iterator itentry;
  for(itentry = stNodes.begin(); itentry != stNodes.end(); ++itentry)
  {
    const CLasPointSet* pLasPointSet = dynamic_cast<const CLasPointSet*>(*itentry);
    if(pLasPointSet)
    {
      if(IsLinkedTo(*pLasPointSet))
        vcLasPointSets.push_back(pLasPointSet);
    }
  }

  stream << int(vcLasPointSets.size());
  for(size_t i = 0; i < vcLasPointSets.size(); ++i)
    stream << vcLasPointSets[i]->Index();

  if(m_pZoomInModel)
  {
    stream << 1;
    CModelBase& model = static_cast<CModelBase&>(Model());
    model.SaveChildModel(stream, progress, *m_pZoomInModel);
  }
  else
  {
    stream << 0;
  }
}

void CWellPath::LoadDCasint(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(!(CStreamVersion(3, 0, 38) < version))
		return;

	//Load the casint points
//	well::CWellPointList::Iterator it  = DCasintPoints().begin();
//	well::CWellPoint* point = it.current();

	long nSize;
	stream >> nSize;

	for(int i=0; i< nSize ; i++)
	{ 
		double tmd;
		// store
		stream >> tmd;

    int iDefined;
    double dValue;
    for(int j = 0; j < 13; ++j)
    {
      stream >> iDefined;
      if(iDefined)
	      stream >> dValue;
    }
	}

	stream >> nSize;
	
	//loop over casint-result and store 
	for(int i=0; i< nSize ; i++)
	{
		int dep_index;
		int point_index;
    CAnalysisType antype;

    if(version < CStreamVersion(3, 0, 116))
    {
		  int linear;
		  stream >> linear;
    }
    else
    {
      antype.LoadStream(stream, version, progress);
    }
		stream >> dep_index;
		stream >> point_index;

    double ddum;
    int idum;
		stream >> ddum;
		stream >> ddum;
		stream >> ddum;
		stream >> idum;

		stream >> ddum;
		stream >> ddum;
		stream >> ddum;
		stream >> idum;

		stream >> ddum;
		stream >> ddum;
		stream >> idum;
	}
}

void CWellPath::SaveWellLog(well::CWellLog *log, TSTREAM& stream, TPROGRESS& progress)
{
	// First the log data //////////////////////////////////////////
	stream << log->LogPointSize();

	//first the tmd's...
	for(int i = 0; i < log->LogPointSize(); i++)
	{
		stream << log->Depth(well::CWellLog::ORIGINAL_TMD, i);
	}
	// now the tvd's...
	for(int j = 0; j < log->LogPointSize(); j++)
	{
		stream << log->Depth(well::CWellLog::ORIGINAL_TVD, j);
	}
	// the shalyness factors....
	for(int k = 0; k < log->LogPointSize(); k++)
	{
		stream << log->getShalyness(k);
	}
	// the compressibility....
	for(int l = 0; l < log->LogPointSize(); l++)
	{
		stream << log->getCompressibility(l);
	}
	// the sand cut-off value....
	if(log->LogPointSize() > 0)
		stream << log->SandCutOff();
	////////////////////////////////////////////////////////////////

	// Now the radioactive tag data ////////////////////////////////
	stream << log->RTagSize();

	// first the tmd's....
	for(int m = 0; m < log->RTagSize(); m++)
	{
		stream << log->RTTMD(m);
	}
	// now the tvd's...
	for(int n = 0; n < log->RTagSize(); n++)
	{
		stream << log->RTTVD(n);
	}
	// now the relative displacements.....
	for(int z = 0; z < log->RTagSize(); z++)
	{
		stream << log->RTRelDispl(z);
	}
	///////////////////////////////////////////////////////////////
	progress.Step();
}

void CWellPath::LoadWellLog(well::CWellLog *log,TSTREAM& stream,TPROGRESS& progress)
{
	int point_size, rat_size;
	double val;
	std::vector<double> vcTMD;
	std::vector<double> vcTVD;

	stream >> point_size;
	if(point_size > 0)
	{	
		std::vector<double> vcShaly;
		std::vector<double> vcComp;

		// load the tmd's
		for(int i = 0; i < point_size; i++)
		{
			stream >> val;
			vcTMD.push_back(val);
		}
		// now the tvd's
		for(int j = 0; j < point_size; j++)
		{
			stream >>val;
			vcTVD.push_back(val);
		}
		// next the shalyness...
		for(int k = 0; k < point_size; k++)
		{
			stream >> val;
			vcShaly.push_back(val);
		}
		// the compressibility.....
		for(int l = 0; l < point_size; l++)
		{
			stream >> val;
			vcComp.push_back(val);
		}
		double sand_cutoff;
		stream >> sand_cutoff;
		
		// intitiate the log...
		log->InitLog(vcTMD, vcTVD, vcShaly, vcComp, sand_cutoff);
	}
	stream >> rat_size;
	if(rat_size > 0)
	{
		vcTMD.clear();
		vcTVD.clear();
		std::vector<double> vcRelDispl;

		// load the tmd's...
		for(int m = 0; m < rat_size; m++)
		{
			stream >> val;
			vcTMD.push_back(val);
		}
		// now the tvd's...
		for(int n = 0; n < rat_size; n++)
		{
			stream >> val;
			vcTVD.push_back(val);
		}
		// now the relative displacements....
		for(int z = 0; z < rat_size; z++)
		{
			stream >> val;
			vcRelDispl.push_back(val);
		}

		// initiate the radioactive tags...
		log->InitRadioactiveTags(vcTMD, vcTVD, vcRelDispl);
	}

	progress.Step();
}



long CWellPath::SavedItems() const
{
	long nRet = CColorNode::SavedItems();

	if (IsVertical())
		nRet += DStaborPoints().NrOfPoints();
	else
		nRet += DefPointList().NrOfPoints()+ DStaborPoints().NrOfPoints();

	nRet += m_mpSectDplLog.size();
	nRet += m_mpCorrectAllStages.size();

  if(m_pZoomInModel)
  {
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    nRet += model.SavedItemsChildModel(*m_pZoomInModel);
  }

	return nRet;
}
	
bool CWellPath::Empty() const
{
	return DefPointList().NrOfPoints()==0;
}

/*virtual*/ bool CWellPath::IsVertical() const
{
	return !m_VerticalPointDef.Empty();
}


bool CWellPath::DsbCanCalculate(const CFormationBase& formation) const
{
	if(formation.FormationType() == CFormationBase::FT_SHALE        || 
	   formation.FormationType() == CFormationBase::FT_NOTSPECIFIED || //mantis 2541 wjrx
     formation.FormationType() == CFormationBase::FT_LIMESTONE)      //mantis 2618 wedx
		return true;

	return false;
}

bool CWellPath::DsbCanCalculateEFG(const CFormationBase& formation) const
{
	if(formation.FormationType()==CFormationBase::FT_SAND)
		return true;

	if(formation.FormationType()==CFormationBase::FT_SANDSTONE)
		return true;

	return false;
}


/* NEVER USED
void CWellPath::CalculateDsbResult(const CDepletionStage& stage, const CAnalysisType& antype) 
{
}
*/

/*void CWellPath::CalculateCasingPoints()
{
	well::CWellPointList& list = GetWellPointList(LST_PT_DCASING);

	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
	if(!pModel->IsMesh())
	{
		list.RemoveAllPoints();
		return;
	}

	if(list.NrOfPoints() > 0)
		return;

	CWaitCursor wait;

	well::CWellPointList& intersect = FormIntersecPoints();

	if(intersect.NrOfPoints() < 2)
		return;

	double delta = 0.1; //delta for above and below intersection

	const int NrOfPointsInFormation = 5; //number of points in formation

	assert(NrOfPointsInFormation > 2);

	well::CWellPointList::Iterator it  = intersect.GetIterator();

	CGeoWellPoint* p1 = (CGeoWellPoint*)it.current();
	CGeoWellPoint* p2 = (CGeoWellPoint*)++it;

	well::CWellPointList collect(*this); //collect created points that are not inserted.
	

	while(p1 && p2)
	{
		
		CGeoWellPoint* above = new CCasingWellPoint(*this,p1->TMD().Value()+delta);
		CGeoWellPoint* below = new CCasingWellPoint(*this,p2->TMD().Value()-delta);
		collect.AddPoint(above);
		collect.AddPoint(below);
		
		if(!list.EqualPointExist(*above) && above->Defined())
			list.AddPoint(above);
		
		if(!list.EqualPointExist(*below) && above->Defined())
			list.AddPoint(below);


		double delta_tmd = (below->TMD().Value() - above->TMD().Value())/(NrOfPointsInFormation-1);
		for(int i=0 ; i<NrOfPointsInFormation-2 ;i++)
		{
			double tmd = above->TMD().Value() + delta_tmd*(i+1);
			CGeoWellPoint* point = new CCasingWellPoint(*this,tmd);
			collect.AddPoint(point);

			if(!list.EqualPointExist(*point) && point->Defined())
				list.AddPoint(point);
			
		}
		p1 = p2;
		p2 = (CGeoWellPoint*)++it;
	}

	//removes and deletes non used points
	collect.AutoDelete(true);
		collect.RemoveNonDuplicatePoints(list);
	collect.AutoDelete(false);

	//removes and deletes non defined points
	list.RemoveUnDefinedPoints();

	well::CWellPointList::Iterator pt = list.GetIterator();
	Casing().RemoveAll();
	while(pt.current())
	{
		Casing().AddPoint(&((CCasingWellPoint*)pt.current())->CasingPoint());
		++pt;
	}
}
*/

void CWellPath::AddDefaultFistPoints(well::CWellPointList& list) const 
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	if(!pModel->IsMesh())
	{
		return;
	}

	std::auto_ptr <IProgressBase> wait(_g->prog()->create(eProgress::Wait));


	const well::CWellSectionList& sections = FormationSections();

	if(sections.NrOfSections() < 1)
		return;

	double delta = 0.1;
	const int NrOfPointsInFormation = 5; //number of points in formation


	well::CWellPointList& FistPoints =list;

	well::CWellPointList collect(*this); //collect created points that are not inserted.
	collect.AutoDelete(true);

	well::CWellSectionList::Iterator it  = sections.begin();

	while(it != sections.end())
	{
  	CFormationSection* section = (CFormationSection*)*it;

    CFormationBase::TFormationType formtype = section->Formation()->FormationType();
    if(section->Formation()->Depleting() &&
       (formtype == CFormationBase::FT_SAND || formtype == CFormationBase::FT_SANDSTONE || formtype == CFormationBase::FT_NOTSPECIFIED))
		{
		
			CGeoWellPoint* above = new CGeoWellPoint(*this,section->Top().TMD().Value()+delta);
			CGeoWellPoint* below = new CGeoWellPoint(*this,section->Bottom().TMD().Value()-delta);
			collect.AddPoint(above);
			collect.AddPoint(below);
			
			if(!FistPoints.EqualPointExist(*above) && above->Defined())
				FistPoints.AddPoint(above);
			
			if(!FistPoints.EqualPointExist(*below) && above->Defined())
				FistPoints.AddPoint(below);


			double delta_tmd = (below->TMD().Value() - above->TMD().Value())/(NrOfPointsInFormation-1);
			for(int i=0 ; i<NrOfPointsInFormation-2 ;i++)
			{
				double tmd = above->TMD().Value() + delta_tmd*(i+1);
				CGeoWellPoint* point = new CGeoWellPoint(*this,tmd);
				collect.AddPoint(point);

				if(!FistPoints.EqualPointExist(*point) && point->Defined())
					FistPoints.AddPoint(point);
				
			}
		}


		++it;

		
	}
	//removes and deletes non used points
	collect.RemoveNonDuplicatePoints(FistPoints);

	FistPoints.RemoveUnDefinedPoints();
	collect.AutoDelete(false);
}

void CWellPath::AddDefaultDstaborPoints(well::CWellPointList& list) const
{
	
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	if(!pModel->IsMesh())
	{
		return;
	}

	std::auto_ptr <IProgressBase> wait(_g->prog()->create(eProgress::Wait));


	const well::CWellSectionList& sections = FormationSections();

	if(sections.NrOfSections() < 1)
		return;

	double delta = 0.1;
	const int NrOfPointsInFormation = 5; //number of points in formation

	assert(NrOfPointsInFormation > 2);

	well::CWellPointList& DstaborPoints =list;

	well::CWellPointList collect(*this); //collect created points that are not inserted.
	collect.AutoDelete(true);

	well::CWellSectionList::Iterator it  = sections.begin();

	while(it != sections.end())
	{
  	well::IWellSection* section = *it;

		CGeoWellPoint* above = new CDstaborWellPoint(*this,section->Top().TMD().Value()+delta);
		CGeoWellPoint* below = new CDstaborWellPoint(*this,section->Bottom().TMD().Value()-delta);
		collect.AddPoint(above);
		collect.AddPoint(below);
		
		if(!DstaborPoints.EqualPointExist(*above) && above->Defined())
			DstaborPoints.AddPoint(above);
		
		if(!DstaborPoints.EqualPointExist(*below) && above->Defined())
			DstaborPoints.AddPoint(below);


		double delta_tmd = (below->TMD().Value() - above->TMD().Value())/(NrOfPointsInFormation-1);
		for(int i=0 ; i<NrOfPointsInFormation-2 ;i++)
		{
			double tmd = above->TMD().Value() + delta_tmd*(i+1);
			CGeoWellPoint* point = new CDstaborWellPoint(*this,tmd);
			collect.AddPoint(point);

			if(!DstaborPoints.EqualPointExist(*point) && point->Defined())
				DstaborPoints.AddPoint(point);
			
		}
		++it;
	}
	//removes and deletes non used points
	collect.RemoveNonDuplicatePoints(DstaborPoints);

	DstaborPoints.RemoveUnDefinedPoints();
	collect.AutoDelete(false);
}

void CWellPath::GetInterSectionPointsWithFormationSkin(const geo::ILine& line , std::set<geo::CPoint>& PointSet) const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());

	if(!pModel->IsMesh())
		return;

	//loop over the volumes in a formation
	CWellPath* pThis=const_cast<CWellPath*>(this);

	TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) pThis->Model().GraphEntry(MD_BASE_FORMATION))->EntryNodes();
	//loop over the formations
	for(TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); iter++)
	{
		//loop over the volumes in a formation
		for(int i = 0; i < (*iter)->ElementSetSize(); i++)
		{
			geo::CBodyGroup *pGeoVolume = dynamic_cast<geo::CBodyGroup*>(&(*iter)->ElementSet(i).ElementSet());
			assert(pGeoVolume);

			//get the intersection points with the volume and the vertical line
			pGeoVolume->IntersectionWithEdgeFaces(line,PointSet);
		}
	}
}

bool CWellPath::assignContainingElement(CGeoWellPoint& pt, const std::set<const geo::IElement*>& stElements) const
{
  for(std::set<const geo::IElement*>::const_iterator it = stElements.begin(); it != stElements.end(); ++it)
  {
    const geo::IElement& elm = **it;
    if(elm.Contains(pt, false))
    {
      pt.addElement(&elm);
      return true;
    }
  }

  return false;
}

void CWellPath::CalcGraphPoints()
{

	if(DefPointList().NrOfPoints()==0)
		Recalculate();

	std::auto_ptr <IProgressBase> wait(_g->prog()->create(eProgress::Wait));

	well::CWellPointList& graph_points=GetWellPointList(LST_PT_GRAPH);
	graph_points.RemoveAllPoints();
  

	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());

	if(!pModel->Mesh().IsMesh())
  {
      m_graphPointsCalculated = true;
		return;
  }

	const double dist = 0.01;

	well::CWellPointList& intersect = MeshIntersecPoints();

	if(intersect.NrOfPoints() < 2)
  {
      m_graphPointsCalculated = true;
      return;
  }
//	assert(intersect.NrOfPoints() >= 2); // no mesh?

	well::CWellPointList::Iterator it  = intersect.begin();

	while(it != intersect.end())
	{
  	well::CWellPoint* p = *it;
    const std::set<const geo::IElement*>& stElements = p->getElements();


		CGeoWellPoint* p1 = new CGeoWellPoint(*this,p->TMD().Value()-dist);
    assignContainingElement(*p1, stElements);
		graph_points.AddPoint(p1);

		CGeoWellPoint* p2 = new CGeoWellPoint(*this,p->TMD().Value()+dist);
    assignContainingElement(*p2, stElements);
		graph_points.AddPoint(p2);

		++it;
	}


	//add dstabor points to graph points
	
	it = DStaborPoints().begin();
	while(it != DStaborPoints().end())
 	{
  	CGeoWellPoint* dsb = (CGeoWellPoint*)*it;
		CGeoWellPoint* point = new CGeoWellPoint(*dsb);
		graph_points.AddPoint(point);
		++it;
	}

	graph_points.RemoveUnDefinedPoints();

	//set graph limits
	graph_points.RemoveUnDefinedPoints();
	m_display_list.ClearAndDelete();

  m_graphPointsCalculated = true;
//	Modified();
}

void CWellPath::GetIntersectionPointsWithPlane(geo::CPtrArray<CGeoWellPoint>& /*points*/,const geo::CPlane& /*plane*/) const
{
	//TODO htg// 
}

CDstaborResultMap& CWellPath::DsbResults()
{
	return *m_pDsbResults;
}

const CDstaborResultMap& CWellPath::DsbResults() const
{
	return *m_pDsbResults;
}

well::CWellDateList &CWellPath::FistDates()
{
	return m_FistDates;
}

const well::CWellDateList &CWellPath::FistDates() const
{
	 return m_FistDates;
}

const well::CWellPointList &CWellPath::FistExportPoints() const
{
	return GetWellPointList(LST_PT_FIST_EXP);
}

well::CWellPointList &CWellPath::FistExportPoints()
{
	return GetWellPointList(LST_PT_FIST_EXP);
}


void CWellPath::MudGradRefDepth(double newval)
{
	m_dMudGradRefDepth=newval;
}

double CWellPath::MudGradRefDepth() const
{
	return m_dMudGradRefDepth;
}

void CWellPath::DerElev(double newval) //wjrx mantis 2549
{
	m_GlobalTVD.SetValue(newval);
}

double CWellPath::DerElev() const //wjrx mantis 2549
{
	return m_GlobalTVD.Value();
}

double CWellPath::DsbLiquidFluid() const
{
	return m_DsbLiquidFluid;
}

void CWellPath::DsbLiquidFluid(double newval)
{
	m_DsbLiquidFluid=newval;
}

double CWellPath::DsbFluidLossCoefficient() const
{
	return m_DsbFluidLossCoefficient;
}

void CWellPath::DsbFluidLossCoefficient(double newval)
{
	m_DsbFluidLossCoefficient=newval;
}


void CWellPath::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
	const CMeshBase* pMesh=dynamic_cast<const CMeshBase*>(&node);
	if (pMesh)
	{
		if(!pMesh->IsMesh() || (uHint == MeshCreated))
		{
			ClearLogdataMaps();
			DsbResults().ClearAllResults();
			InvalidatePointLists();
			m_display_list.ClearAndDelete();
		//	FistDates().Clear();
			if(!m_VerticalPointDef.Empty())
			{

				CWellPathBase::DefPointList().RemoveAllPoints();
			}

			Modified();
		}

		return;
	}

	const CDepletionStage* pStage=dynamic_cast<const CDepletionStage*>(&node);
	if (pStage)
	{
		ClearLogdataMaps();
		DsbResults().ClearAllResults();
		Modified();
	}


	const CFormationBase* fb = dynamic_cast<const CFormationBase*>(&node);
	if ( fb )
	{
    // dirty sections
 //   SetSectionsDirty();
    well::CWellSectionList& form_sections = GetWellSectionList(LST_SC_FORM);
    well::CWellSectionList::Iterator it;
    for(it = form_sections.begin(); it != form_sections.end(); ++it)
    {
      well::IWellSection* pSection = *it;
      assert(dynamic_cast<CFormationSection*>(pSection));
      CFormationSection* pFormSection = static_cast<CFormationSection*>(pSection);
      if(pFormSection->Formation() == fb)
      {
        // update display list
        m_pIntermediate->SetSectionDescription(*pSection, fb->Name());
/*
        CWellSectionDrawSpec& ds = const_cast<CWellSectionDrawSpec&>(m_pSceneInterMed->GetSection(*pSection, 1));
        ds.SetDescription(QString(fb->Name())); // force rebuild
*/
      }
    }
    Modified();
	}
	
	CColorNode::OnNeighbourModified(node,uHint);
}

void CWellPath::SetSectionsDirty()
{
  m_bSectionsDirty = true;

  // update immediately when shown
//  if(m_pSceneInterMed->Scene())
  if(m_pIntermediate->IsVisible())
	  FormationSections();
	Modified();
}

bool CWellPath::hasZoomInModel() const
{
  return (m_pZoomInModel != 0);
}

void CWellPath::CreateZoomInModel()
{
#ifdef ZOOMIN
  if(!m_pZoomInModel)
  {
    constructZoomInModel();

    // inform model that there is a new child
    CModelBase& model = static_cast<CModelBase&>(Model());
    model.OnNewChildModel(*m_pZoomInModel, false);

    Modified();
  }
#endif
}

void CWellPath::constructZoomInModel()
{
  CModelBase& modelBase = dynamic_cast <CModelBase&> (Model());
  m_pZoomInModel = static_cast<CWellZoomInModel *>(IModelLifetimeFacade::NewChild(WELL_ZOOMIN_MODEL, modelBase, *this, Model().Logger(), modelBase.getVersionManager()));
}

void CWellPath::setNewModelFunction(bool (CNewModel::*newModel)(bool))
{
  m_newModelFunction = newModel;
}

CNewModel& CWellPath::getNewModel()
{
  return m_newModel;
}

void CWellPath::DestroyZoomInModel()
{
  if(m_pZoomInModel)
  {
    m_pZoomInModel->DestroyZoomInModel();
    if (m_pZoomInModel) // this one is 0, but I leave it in, so that we can look at this later. Ideally, we use the facade
      IModelLifetimeFacade::Delete(m_pZoomInModel);
    m_pZoomInModel = 0;
  }
}

void CWellPath::OnNeighbourDeleted(const CGraphNode &node)
{
  if(m_pIntermediate)
    m_pIntermediate->OnNeighbourDeleted(node);
	const CDepletionStage* pStage=dynamic_cast<const CDepletionStage*>(&node);
	if (pStage)
	{
		ClearLogdataMaps();
		DsbResults().ClearAllResults();
		Modified();
	}

  if(&node == m_pZoomInModel)
    m_pZoomInModel = 0;

	CColorNode::OnNeighbourDeleted(node);
}

void CWellPath::OnNewNeighbour(const CGraphNode& node)
{
  if(m_pIntermediate->OnNewNeighbour(node))
    FormationSections(); // trigger update
	const CDepletionStage* pStage=dynamic_cast<const CDepletionStage*>(&node);
	if (pStage)
	{
		ClearLogdataMaps();
		DsbResults().ClearAllResults();
		Modified();
	}

	CColorNode::OnNewNeighbour(node);
}

well::CWellLog *CWellPath::WellLog(CFormationSection *pSection) const
{
	CWellPath::TSectDeplLogMap::const_iterator it;
	it = m_mpSectDplLog.find(pSection);
	if(it == m_mpSectDplLog.end())
		return 0;

	return (*it).second.second;
}

void CWellPath::InsertLog(CFormationSection *pSection, CDepletionStage *pStage, well::CWellLog *pWellLog)
{
	CWellPath::TSectDeplLogMap::iterator it;
	it = m_mpSectDplLog.find(pSection);

	if(it != m_mpSectDplLog.end())
		m_mpSectDplLog.erase(it);

	m_mpSectDplLog.insert(CWellPath::TSectDeplLogMap::value_type(pSection, CWellPath::TStageLogPair(pStage, pWellLog)));
}

void CWellPath::InsertCorrection(CFormationSection *pSection, bool bCorrect)
{
	std::map<CFormationSection*, bool>::iterator it = m_mpCorrectAllStages.find(pSection);
	if(it == m_mpCorrectAllStages.end())
		m_mpCorrectAllStages.insert(std::map<CFormationSection*, bool>::value_type(pSection, bCorrect));
	else
		it->second = bCorrect;
}

bool CWellPath::UseCorrection(CFormationSection *pSection) const
{
	std::map<CFormationSection*, bool>::const_iterator it;
	it = m_mpCorrectAllStages.find(pSection);
	
	if(it == m_mpCorrectAllStages.end())
		return false; // something is wrong
	
	return it->second;
}

// This function is used whenever the 'regular' strains have to be displayes in a DCasint situation
// is called in DCasintView and in CDCasingLogRatDlg......
void CWellPath::GetGeomecStrainValues(well::CWellPointList &list, well::CWellPointValueMap &map, const CDepletionStage &stage, const CAnalysisType& antype) const
{
	map.ClearMap();
	const CModelBase &model = (const CModelBase&)(Model());
	const CResultTree &result_tree = model.ResultTree();
	int nMode = result_tree.TotalStrain().Components().Mode(antype, stage);
	assert(nMode >= 0);

	well::CWellPointList::Iterator it_list = list.begin();
	while(it_list != list.end())
	{
		CStrainTensor geom_eps(
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_XX, nMode).ScalarData().ValuePoint(**it_list).Value(),
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_YY, nMode).ScalarData().ValuePoint(**it_list).Value(),
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_ZZ, nMode).ScalarData().ValuePoint(**it_list).Value(),
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_XY, nMode).ScalarData().ValuePoint(**it_list).Value(),
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_YZ, nMode).ScalarData().ValuePoint(**it_list).Value(),
		result_tree.TotalStrain().Components().Component(ITensorGroup::CComponentComposite::TC_ZX, nMode).ScalarData().ValuePoint(**it_list).Value());

		geo::CVector direction = (*it_list)->GetDirection();
		direction = direction.UnitVector();
		geo::CVector perp1 = direction.GetNormal();
		perp1.UnitVector();
		geo::CVector perp2;
		perp2 = direction.CrossProduct(perp1);

		geo::CMatrix rot_mat(3, 3);
		rot_mat.Value(0, 0, perp1.X());	rot_mat.Value(0, 1, perp2.X()); rot_mat.Value(0, 2, direction.X());
		rot_mat.Value(1, 0, perp1.Y()); rot_mat.Value(1, 1, perp2.Y()); rot_mat.Value(1, 2, direction.Y());
		rot_mat.Value(2, 0, perp1.Z()); rot_mat.Value(2, 1, perp2.Z()); rot_mat.Value(2, 2, direction.Z());

		CStrainTensor tens_rot = geom_eps.RotateTensor(rot_mat);
			
		map.InsertItem(*it_list, tens_rot.ZZ());
		++it_list;
	} 
}

const CFormationSection *CWellPath::FormationSection(const CGeoWellPoint &pt) const
{
	const well::CWellSectionList &list = FormationSections();
	well::CWellSectionList formation_list = list.GetSections( pt );
	if( formation_list.NrOfSections() == 0 ) return 0;
	assert( formation_list.NrOfSections() == 1 );
	well::CWellSectionList::Iterator it = formation_list.begin();
	assert(it != formation_list.end());
	return dynamic_cast<const CFormationSection*>( *it );
}

void CWellPath::ClearLogdataMaps()
{
	m_mpCorrectAllStages.clear();

	TSectDeplLogMap::iterator it;
	for(it = m_mpSectDplLog.begin(); it != m_mpSectDplLog.end(); it++)
	{
		delete it->second.second;
	}


	m_mpSectDplLog.clear();
}

void CWellPath::ToggleDrawWellInfo()
{
	DrawWellInfo(!DrawWellInfo());
}

void CWellPath::DrawWellInfo(bool bDraw)
{
  if(bDraw != m_pIntermediate->DrawInfo())
  {
    m_pIntermediate->DrawInfo(bDraw);
    Modified();
  }
/*
	if(bDraw != DrawWellInfo())
	{
		m_pSceneInterMed->DrawWellInfo(bDraw);
		Modified();
	}
*/
}

bool CWellPath::DrawWellInfo() const
{
  return m_pIntermediate->DrawInfo();
//	return m_pSceneInterMed->DrawWellInfo();
}

bool CWellPath::CanZoomIn() const
{
  if(m_pZoomInModel != 0)
    return false;

  const well::CWellPointList& lstFormationPoints = FormIntersecPoints();
  return (lstFormationPoints.First() != 0 && lstFormationPoints.Last() != 0);
}

const CWellZoomInModel& CWellPath::ZoomInModel() const
{
  assert(m_pZoomInModel);
  return *m_pZoomInModel;
}

CWellZoomInModel& CWellPath::ZoomInModel()
{
  assert(m_pZoomInModel);
  return *m_pZoomInModel;
}

void CWellPath::SetIntermediate(IIntermediate* pIntermediate)
{
  m_pCachingIntermediate->SetIntermediate(pIntermediate);
}

CWellPath::IIntermediate* CWellPath::Intermediate() const
{
  return m_pIntermediate;
}


// CWellPathEntry implementation ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CWellPathEntry::CWellPathEntry(C3DModel &model) 
: BASE_CLASS(MD_BASE_WELLPATH, IDI_WELLPATH, IDS_TREE_WELLPATHS, model)
{
}

CWellPathEntry::~CWellPathEntry() 
{
	while (EntryNodes().begin() != EntryNodes().end())
	{
		(*EntryNodes().begin())->destroy(); // Normally things work with delete
	}
}

void CWellPathEntry::ShowAllWellPathInfo()
{
	AllWellPathInfo(true);
}

void CWellPathEntry::HideAllWellPathInfo()
{
	AllWellPathInfo(false);
}

void CWellPathEntry::AllWellPathInfo(bool bShow)
{
	CWellPathEntry::TNodeSet stWellPaths = EntryNodes();
	CWellPathEntry::TNodeSet::iterator it;
	for(it = stWellPaths.begin(); it != stWellPaths.end(); ++it)
		(*it)->DrawWellInfo(bShow);
}

bool CWellPathEntry::CanExport() const
{
  return !EntryNodes().empty();
}

bool CWellPathEntry::CanExportToFist() const
{
	CModelBase& model = (CModelBase&)(Model());
	if(!model.IsResult())
		return false;

	CWellPathEntry::TNodeSet well_paths = EntryNodes();
	if(well_paths.size() == 0)
		return false;
	for(CWellPathEntry::TNodeSet::iterator it = well_paths.begin(); it != well_paths.end(); it++)
	{
		if((*it)->FistExportPoints().NrOfPoints() > 0)
			return true;
	}

	return false;
}

bool CWellPathEntry::ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model, const CQuantity::UNIT& /*unit*/)
{
	QString sPath,sFile;
	SplitPathAndFileName(strFileName,sPath,sFile);
	std::auto_ptr <IProgressBase> prog(_g->prog()->create(eProgress::Geo, ""));
	if(sFile.right(4).compare(".MWR", Qt::CaseInsensitive) == 0) {
		assert(false);
	} else if((sFile.right(4).compare(".DAT", Qt::CaseInsensitive) == 0) ||
		      (sFile.right(4).compare(".TXT", Qt::CaseInsensitive) == 0)) {
    assert(false);
	} else if(sFile.right(4).compare(".XLS", Qt::CaseInsensitive) == 0 || sFile.right(5).left(4).compare(".XLS", Qt::CaseInsensitive) == 0) {
		CElementSetExcelFile file( (CModelBase&)model, false );
		CGraphNode::TNodeVec nodeVec;
		file.ReadSheets(strFileName.toStdString().c_str(), dlg, nodeVec);
	} else if(sFile.right(3).compare(".MX", Qt::CaseInsensitive) == 0) {

		// Load GoCad File
		CGoCadWellFile file;
		if(file.Open(strFileName, *prog)) {

			for(int k=0; k < file.wellSize();k++)
			{
				QString wellName = file.wellName(k);

				CWellPath* pWellPath = new CWellPath(wellName, model);
				int size = file.pointSize(k);

				QVector<geo::CPoint> vcPoint(size);

				for(int i=0 ; i< size;i++)
				{
					vcPoint[i]=file.pointAt(k,i);
				}
				pWellPath->InitFromPointArray(vcPoint);
			}
			
		} else {
			_m()->msg(file.ErrorMessage());
		}
	} else {

	}

	return true;
}

//*********************************************  CDstaborWellPoint ******************************************

CDstaborWellPoint::~CDstaborWellPoint()
{
	((CWellPath*)(&WellPath()))->DsbResults().RemoveResult(*this);
}

CDstaborWellPoint::CDstaborWellPoint(const CWellPath& wellpath , const double& tmd)
:CGeoWellPoint(wellpath,tmd)
{
}
//***********************************************************************************************************

CGeoWellPoint::CGeoWellPoint(const CWellPath& wellpath , const double& tmd)
:well::CWellPoint(wellpath,tmd),
  m_pFormation(0),
  m_bInit(false),
  m_model(dynamic_cast<const CModelBase*>(&wellpath.Model()))//(dynamic_cast<const CModelBase&>(wellpath.Model()))
{

}

CGeoWellPoint::CGeoWellPoint(const CGeoWellPoint& rhs)
:well::CWellPoint(rhs), m_pFormation(rhs.m_pFormation), m_bInit(rhs.m_bInit), m_model(rhs.m_model)
{

}

CGeoWellPoint& CGeoWellPoint::operator=(const CGeoWellPoint& rhs)
{

	CWellPoint::operator=(rhs);
	m_pFormation=rhs.m_pFormation;
	//const_cast<CModelBase&>(m_model) = rhs.m_model;
	m_model = rhs.m_model;
	m_bInit=rhs.m_bInit;
	return *this;
}

void CGeoWellPoint::InitPoint() const
{
	m_bInit = true;
	m_pFormation = 0;


	// Get the point from the mesh
	std::vector<int> vcElement = m_model->Mesh().Mesh().ElementsAt(*this);
	for(size_t i = 0; i < vcElement.size(); i++)
	{
		const geo::IElement& element = m_model->Mesh().Mesh().Element(vcElement[i]);
		if(dynamic_cast<const geo::IBody*>(&element))
		{
			if(element.Contains(*this,true))
			{
				m_pFormation = const_cast<CFormationBase*>(m_model->Mesh().Formation(element));
				return;
			}
		}
	}
}

CGeoWellPoint::CGeoWellPoint(CModelBase& model, const well::CWellPoint& wellpoint)
:well::CWellPoint(wellpoint),
  m_pFormation(0),
  m_bInit(false),
  m_model(&model)
{
}

/*virtual*/ bool CGeoWellPoint::Defined() const
{
	if(!CWellPoint::Defined())
		return false;

	if(!Formation())
		return false;

	return true;
}

/*virtual*/ void CGeoWellPoint::Invalidate()
{
	CWellPoint::Invalidate();
	m_bInit=false;
	m_pFormation=0;
}

const CFormationBase* CGeoWellPoint::Formation() const
{
	if(!m_bInit)
		InitPoint();


	return m_pFormation;
}


CGeoWellPoint::~CGeoWellPoint()
{

}

//*********************************************  CResultMap ******************************************
CWellPointResultKey::CWellPointResultKey(const well::CWellPoint& point ,const CDepletionStage& stage, const CAnalysisType& antype)
: m_point(point), m_stage(stage), m_antype(antype)
{
}

bool CWellPointResultKey::operator<( const CWellPointResultKey& rhs) const
{
	if(m_point > rhs.m_point)
		return false;

	if(m_point < rhs.m_point)
		return true;

	if(&m_stage > &rhs.m_stage)
		return false;

	if(&m_stage < &rhs.m_stage)
		return true;

  if(m_antype > rhs.m_antype)
    return false;

  if(m_antype < rhs.m_antype)
    return true;

	//equal point
	return false;
}
