// Result.cpp: implementation of the CResult class.
//
//////////////////////////////////////////////////////////////////////

#include "ModelBase.h"
#include "ResultRegister.h"
#include "BoundaryBase.h"
#include "MeshBase.h"
#include "Result.h"
#include "resourceIDI.h"
#include "Box.h"
#include "IStrainTensorGroup.h"
#include "FemAppEntryTypes.h"
#include "ResultInfo.h"
#include "TextTagUtils.h"
#include "FieldFactor.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of CResultGroup
//////////////////////////////////////////////////////////////////////
CResultGroup::CResultGroup(const QString& sGroupName, CModelBase& model)
: CGraphNode(sGroupName), m_pParent(0), m_model(model)
{
	reParent(&model);
}

CResultGroup::CResultGroup(unsigned int uGroupName, CModelBase& model)
: CGraphNode(uGroupName), m_pParent(0), m_model(model)
{
	reParent(&model);
}

CResultGroup::CResultGroup(const QString& sGroupName, CResultGroup& parent)
: CGraphNode(sGroupName), m_pParent(&parent), m_model(parent.Model())
{
	// Link to parent and register by parent
	reParent(&parent);
	parent.m_vcChild.push_back(this);
}

CResultGroup::CResultGroup(unsigned int uGroupName, CResultGroup& parent)
: CGraphNode(uGroupName), m_pParent(&parent), m_model(parent.Model())
{
	// Link to parent and register by parent
	reParent(&parent);
	parent.m_vcChild.push_back(this);
}

CResultGroup::~CResultGroup()
{
	// Delete children
	std::vector<CGraphNode*> vcChild = m_vcChild;
	for(size_t i = 0; i < vcChild.size(); i++)
		delete vcChild[i];
}

// wjrx mantis 2985
bool CResultGroup::CanExportGocad() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	assert(pModel);
	return pModel->Mesh().IsMesh();
}

bool CResultGroup::CanImport() const
{
	//wjrx mantis 2976
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	assert(pModel);
	return pModel->CanImportResults();
}

bool CResultGroup::CanExport() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	assert(pModel);
  return pModel->Mesh().IsMesh();
}

const CResultGroup* CResultGroup::Parent() const
{
	return m_pParent;
}

CResultGroup* CResultGroup::Parent()
{
	return m_pParent;
}

int CResultGroup::ChildSize() const
{
	return m_vcChild.size();
}

const CGraphNode& CResultGroup::Child(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < ChildSize());

	return *m_vcChild[nIndex];
}

CGraphNode& CResultGroup::Child(int nIndex)
{
	assert(nIndex >= 0);
	assert(nIndex < ChildSize());

	return *m_vcChild[nIndex];
}

void CResultGroup::PushBack(CGraphNode& child)
{
  m_vcChild.push_back(&child);
}

void CResultGroup::OnNeighbourDeleted(const CGraphNode &node)
{
	for(int i = 0; i < ChildSize(); i++)
	{
		if(&Child(i) == &node)
		{
			m_vcChild.erase(m_vcChild.begin() + i);
			CGraphNode::OnNeighbourDeleted(node);
			return;
		}
	}

	CGraphNode::OnNeighbourDeleted(node);
}

unsigned int CResultGroup::IconId() const
{
	return IDI_RESULT_MAP;
}

unsigned int CResultGroup::TypeId() const
{
	return 0;
}

bool CResultGroup::Less(const CGraphNode &node) const
{
	// Result and results group have an order in the vcChild vector
	const CResultGroup *pGroup = dynamic_cast<const CResultGroup*>(&node);
	if(pGroup)
	{
		// Two result group with the same parent are compared with their indices
		if(pGroup->m_pParent == m_pParent)
		{
			
			int rhs_index = 0, this_index = 0;
			for(int i = 0; i < m_pParent->ChildSize(); i++)
			{
				const CGraphNode *pChild = &m_pParent->Child(i);
				if(pGroup == pChild)
					rhs_index = i;
				if(this == pChild)
					this_index = i;
			}

			return this_index < rhs_index;
		}

    if (!m_pParent)
      return true;
    if (!pGroup->m_pParent)
      return false;
    return m_pParent->Less(*pGroup->m_pParent);
	}

	const IResult *pResult = dynamic_cast<const IResult*>(&node);
	if(pResult)
	{
		// Two result group with the same parent are compared with their indices
		if(pResult->Parent() == m_pParent)
		{
			int rhs_index= 0, this_index = 0;
			for(int i = 0; i < m_pParent->ChildSize(); i++)
			{
				const CGraphNode *pChild = &m_pParent->Child(i);
				if(pResult == pChild)
					rhs_index = i;
				if(this == pChild)
					this_index = i;
			}

			return this_index < rhs_index;
		}

    if (!m_pParent)
      return true;
    if (!pResult->Parent())
      return false;
    return m_pParent->Less(*pResult->Parent());
  }

	return CGraphNode::Less(node); // assume this creates a strict weak ordering
}

bool CResultGroup::ValidName(const std::string &name, CResultInfo &resultInfo) const
{
  for ( int ii= 0; ii< ChildSize(); ++ii)
  {
    const CResultGroup *pResultGroup=
      dynamic_cast<const CResultGroup*>(&Child(ii));

    if ( pResultGroup)
    {
      if ( pResultGroup->ValidName(name, resultInfo) )
        return true;
    }
    else
    {
      const IResult *pIResult=
        dynamic_cast<const IResult *>(&Child(ii));
      if ( pIResult)
      {
        if ( pIResult->ValidName(name, resultInfo) )
          return true;
      }
      else
        assert(0); // Can only be CResultGroup or IResult
    }
  }

  resultInfo.Clear();
  return false;
}

//////////////////////////////////////////////////////////////////////
// Implementation of IResult
//////////////////////////////////////////////////////////////////////
IResult::CResultKey::CResultKey(int nStage, const CAnalysisType& antype, int nRegister)
: m_antype(antype),
  m_nStage(nStage),
  m_nRegister(nRegister)
{
}

bool IResult::CResultKey::operator<(const CResultKey& rhs) const
{
  if(m_antype < rhs.m_antype)
    return true;
  if(m_antype > rhs.m_antype)
    return false;
	if(m_nStage < rhs.m_nStage)
		return true;
	if(m_nStage > rhs.m_nStage)
		return false;
	return m_nRegister < rhs.m_nRegister;
}


IResult::IResult(unsigned int uResultName, CResultGroup& group)
: IValueComposite(uResultName, group.Model()), m_group(group)
{
	CModelBase& model = dynamic_cast<CModelBase&>(group.Model());

	// Link to the result register
	// We want to stay in touch with the depletionstage entry
	for(int i = 0; i < model.ResultRegisterSize(); i++)
	{
		LinkTo(model.ResultRegister(i));
		LinkTo(model.ResultRegister(i).DepletionStageEntry());
	}
	// Link to parent and register by parent
	reParent(&group);
	group.PushBack(*this);

  assert(Model().GraphEntry(MD_BASE_RESULT));
  LinkTo(*Model().GraphEntry(MD_BASE_RESULT));
}

IResult::IResult(const QString &sResultName, CResultGroup& group)
: IValueComposite(sResultName, group.Model()), m_group(group)
{
	CModelBase& model = dynamic_cast<CModelBase&>(group.Model());

	// Link to the result register
	// We want to stay in touch with the depletionstage entry
	for(int i = 0; i < model.ResultRegisterSize(); i++)
	{
		LinkTo(model.ResultRegister(i));
		LinkTo(model.ResultRegister(i).DepletionStageEntry());
	}

	// Link to parent and register by parent
	reParent(&group);
	group.PushBack(*this);

  assert(Model().GraphEntry(MD_BASE_RESULT));
  LinkTo(*Model().GraphEntry(MD_BASE_RESULT));
}

IResult::~IResult()
{
}

QString IResult::ExportLabel() const
{
  return ExportLabel( Component().ComponentIndex() );
}

const CResultGroup* IResult::Parent() const
{
	return &m_group;
}

CResultGroup* IResult::Parent()
{
	return &m_group;
}

void IResult::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
	for(int nRegister = 0; nRegister < ResultRegisterSize(); nRegister++)
	{
		if(&ResultRegister(nRegister).DepletionStageEntry() == &node || &ResultRegister(nRegister) == &node)
		{
			{
				COperation oper(Model());

				ClearComponents();
				BuildComponents();
			}

			Modified();
		}

		if(&ResultRegister(nRegister) == &node)
		{
			for(unsigned int nMode = 0; nMode < ModeSize(); nMode++)
			{
				for(unsigned int nComponent = 0; nComponent < ComponentSize(nMode); nComponent++)
				{
					IResultComponent* pComponent = dynamic_cast<IResultComponent*>(&Component(nComponent, nMode));
					if(pComponent->RegisterIndex() == nRegister)
						pComponent->Modified();
				}
			}
		}
	}
}

unsigned int IResult::IconId() const
{
	return IDI_RESULT;
}

unsigned int IResult::TypeId() const
{
	return 0;
}

bool IResult::Available(const CAnalysisType& antype, const CDepletionStage& stage, int nRegister) const
{
	if(Mode(antype, stage, nRegister) < 0)
		return false;

	if(Component(0, Mode(antype, stage, nRegister)).Defined())
		return true;

	return false;
}

std::vector<geo::CValue> IResult::ResultAtTime(const CAnalysisType& antype, const CGeomecTime &time, const geo::IElement &element, const IQuantityDouble::UNIT unit /*= IQuantityDouble::SI_UNIT*/, int nComponent /* = 0 */) const
{
	std::pair<const CDepletionStage*, const CDepletionStage*> pr = NeighboringStages(time);
	const CDepletionStage *pStage1 = pr.first;
	const CDepletionStage *pStage2 = pr.second;
	
	std::vector<geo::CValue> vcret;
	
	if(pStage1 == pStage2)
	{
		return Component(nComponent, Mode(antype, *pStage1)).ScalarData().ValueElement(element, unit);
	}
	else
	{
		std::vector<geo::CValue> vals1 = Component(nComponent, Mode(antype, *pStage1)).ScalarData().ValueElement(element, unit);
		std::vector<geo::CValue> vals2 = Component(nComponent, Mode(antype, *pStage2)).ScalarData().ValueElement(element, unit);

		double delta_total = pStage2->Time().SecondsTo(pStage1->Time());
		double delta1 = time.SecondsTo(pStage1->Time());
		double delta2 = pStage2->Time().SecondsTo(time);
		
		for(size_t i = 0; i < vals1.size(); i++)
		{
			vcret.push_back((vals1[i] * (delta2 / delta_total)) + (vals2[i] * (delta1 / delta_total)));
		}

		return vcret;
	}
}

int IResult::ResultRegisterSize() const
{
	const CModelBase* pModel = (const CModelBase*)(&Model());
	return pModel->ResultRegisterSize();
}

const CResultRegister& IResult::ResultRegister(int nIndex) const
{
	const CModelBase* pModel = (const CModelBase*)(&Model());
	return pModel->ResultRegister(nIndex);
}

CResultRegister& IResult::ResultRegister(int nIndex)
{
	CModelBase* pModel = (CModelBase*)(&Model());
	return pModel->ResultRegister(nIndex);
}



geo::CValue IResult::ResultAtTime(const CAnalysisType& antype, const CGeomecTime &time, const geo::IPoint &pt, const IQuantityDouble::UNIT unit /*= IQuantityDouble::SI_UNIT*/, int nComponent /* = 0 */) const
{
	std::pair<const CDepletionStage*, const CDepletionStage*> pr = NeighboringStages(time);
	const CDepletionStage *pStage1 = pr.first;
	const CDepletionStage *pStage2 = pr.second;
	
	if(pStage1 == pStage2)
	{
		return Component(nComponent, Mode(antype, *pStage1)).ScalarData().ValuePoint(pt, unit);
	}
	else
	{
		geo::CValue val1 = Component(nComponent, Mode(antype, *pStage1)).ScalarData().ValuePoint(pt, unit).Value();
		geo::CValue val2 = Component(nComponent, Mode(antype, *pStage2)).ScalarData().ValuePoint(pt, unit).Value();

		double delta_total = pStage2->Time().SecondsTo(pStage1->Time());
		double delta1 = time.SecondsTo(pStage1->Time());
		double delta2 = pStage2->Time().SecondsTo(time);

		double result = (val1.Value() * (delta2 / delta_total)) + (val2.Value() * (delta1 / delta_total));
		return  geo::CValue(result) ;
	}
}
	
std::pair<const CDepletionStage*, const CDepletionStage*> IResult::NeighboringStages(const CGeomecTime &time) const
{
	const CModelBase &model = (const CModelBase&)(Model());
	CDepletionStage *pStage1 = &const_cast<CDepletionStage&>(model.InitialDepletionStage());
	CDepletionStage *pStage2 = pStage1;
	CDepletionStage *pStage = pStage1;
	
	while(pStage)
	{
		if(pStage == &model.InitialDepletionStage())
		{
			if(pStage->Time() > time)
			{ // time is before initial depletion pStage
				pStage1 = pStage;
				pStage2 = pStage;
				break;
			}
		}

		if(pStage->Time() == time)
		{
			pStage1 = pStage;
			pStage2 = pStage;
			break;
		}

		if(pStage->Time() < time)
		{
			if(pStage->Last())
			{	// time is after last depletion pStage, take the value at the last pStage
				pStage1 = pStage;
				pStage2 = pStage;
				break;
			}

			if(pStage->Next().Time() > time)
			{
				pStage1 = pStage;
				pStage2 = &pStage->Next();
				break;
			}
		}
		pStage = &pStage->Next();
	}

	return std::pair<const CDepletionStage*, const CDepletionStage*>(pStage1, pStage2);
}


int IResult::RegisterMode(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
{
	// Insert the key
	CResultKey result_key(stage.Index(), antype, nRegister);

	TResultMap::iterator it = m_mpResult.insert(TResultMap::value_type(result_key, ModeSize())).first;
	assert(it != m_mpResult.end());
		return it->second;
}

int IResult::Mode(const CAnalysisType& antype, int nTimeStep, int nRegister) const
{
	TResultMap::const_iterator it = m_mpResult.find(CResultKey(nTimeStep, antype, nRegister));
	if(it != m_mpResult.end())
		return it->second;
	
	return -1;
}

const IResultComponent* IResult::ResultComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const
{
	return ResultComponent(stage.Index(), antype, nRegister, nComponent);
}

IResultComponent* IResult::ResultComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister, unsigned int nComponent)
{
	return ResultComponent(stage.Index(), antype, nRegister, nComponent);
}

const IResultComponent* IResult::ResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const
{
  return OnGetResultComponent(nTimeStep, antype, nRegister, nComponent);
/*
	const IResultComponent *pRes = 0;
	TResultMap::const_iterator it = m_mpResult.find(CResultKey(nTimeStep, antype, nRegister));
	if(it != m_mpResult.end())
		pRes = dynamic_cast<const IResultComponent*>(&Component(nComponent, it->second));
	return pRes;
*/
}

IResultComponent* IResult::ResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent)
{
  return OnGetResultComponent(nTimeStep, antype, nRegister, nComponent);
/*
	IResultComponent *pRes = 0;
	TResultMap::const_iterator it = m_mpResult.find(CResultKey(nTimeStep, antype, nRegister));
	if(it != m_mpResult.end())
		pRes = dynamic_cast<IResultComponent*>(&Component(nComponent, it->second));
	return pRes;
*/
}

IResultComponent* IResult::OnGetResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const
{
	const IResultComponent *pRes = 0;
	TResultMap::const_iterator it = m_mpResult.find(CResultKey(nTimeStep, antype, nRegister));
	if(it != m_mpResult.end())
		pRes = dynamic_cast<const IResultComponent*>(&Component(nComponent, it->second));
	return const_cast<IResultComponent*>(pRes);
}

int IResult::Mode(const CAnalysisType& antype, const CDepletionStage& stage, int nRegister) const
{
	return Mode(antype, stage.Index(), nRegister);
}

bool IResult::Empty() const
{
	return false;
}

long IResult::SavedItems() const
{
	return 0;
}

void IResult::ClearComponents()
{
	while(ModeSize() != 0)
	{
		// Create a vector of the components and clear them all
		std::vector<IValueComponentBase*> vcComponent;

		for(int i = 0; i < ComponentSize(0); i++)
			vcComponent.push_back(&Component(i, 0));

		for(size_t i = 0; i < vcComponent.size(); i++)
			delete vcComponent[i];
	}

	m_mpResult.clear();
}

void IResult::BuildComponents()
{
	// Get first depletion stage
	CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
	assert(pModel);

	// Walk over depletionstage and create components for every depletion stage and every analysis
	// type.
	for(int nRegister = 0; nRegister < pModel->ResultRegisterSize(); nRegister++)
	{
		CDepletionStage *pStage = &pModel->ResultRegister(nRegister).DepletionStageEntry().InitialStage();
		while(pStage)
		{
      BuildComponent(*pStage, CAnalysisType::AT_NONLIN,              nRegister);
      BuildComponent(*pStage, CAnalysisType::AT_LINEAR,              nRegister);
      BuildComponent(*pStage, CAnalysisType::AT_HEAT,                nRegister);
      BuildComponent(*pStage, CAnalysisType::AT_MIXTURE,             nRegister);
      BuildComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, nRegister);

			if(pStage->Last())
				pStage = 0;
			else
				pStage = &pStage->Next();
		}
	}
}

bool IResult::Less(const CGraphNode &node) const
{
	// Result and results group have an order in the vcChild vector
	const CResultGroup *pGroup = dynamic_cast<const CResultGroup*>(&node);
	if(pGroup)
	{
		// Two result group with the same parent are compared with their indices
		if(pGroup->Parent() == Parent())
		{
			int rhs_index = 0, this_index = 0;
			for(int i = 0; i < Parent()->ChildSize(); i++)
			{
				if(pGroup == &Parent()->Child(i))
					rhs_index = i;
				if(this == &Parent()->Child(i))
					this_index = i;
			}

			return this_index < rhs_index;
		}
  
    if (!Parent())
      return true;
    if (!pGroup->Parent())
      return false;

    return Parent()->Less(*pGroup->Parent());
  }

	const IResult *pResult = dynamic_cast<const IResult*>(&node);
	if(pResult)
	{
		// Two result group with the same parent are compared with their indices
		if(pResult->Parent() == Parent())
		{
			int rhs_index = 0, this_index = 0;
			for(int i = 0; i < Parent()->ChildSize(); i++)
			{
				if(pResult == &Parent()->Child(i))
					rhs_index = i;
				if(this == &Parent()->Child(i))
					this_index = i;
			}

			return this_index < rhs_index;
		}

    if (!Parent())
      return true;
    if (!pResult->Parent())
      return false;

    return Parent()->Less(*pResult->Parent());
	}	

	return IValueComposite::Less(node);
}

bool IResult::isResult() const
{
  return !isMaterialResult();
}

bool IResult::OnDefined(const IResultComponent& component) const
{
  return ResultRegister(component.RegisterIndex()).ResultsAvailable(component.AnalysisType());
}

bool IResult::CanExport() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	assert(pModel);
	return pModel->ResultRegister().ResultsAvailable();
}

// protected

bool IResult::OnBuildComponent(const CDepletionStage& /*stage*/,
  const CAnalysisType& /*antype*/, int /*nRegister*/) const
{
  return true;
}

bool IResult::validateName(CResultInfo& resultInfo, const std::string& name,
  const std::string& label, const std::string& componentList,
  const std::string& analysisTypeList) const
{
  bool valid= false;
  int timeStep= -1;

  if ( name.find( label ) != 0 )
    return false;

  std::string baseTag;
  std::string component;
  std::string analysisType;

  valid= SplitTextTag
    ( name
    , componentList
    , analysisTypeList
    , baseTag
    , component
    , timeStep
    , analysisType
    );

  if ( !valid ) 
    return false;

  if ( label != baseTag )
    return false;

  if ( timeStep < 0 )
  {
    if ( !analysisType.empty() )
      return false;
  }
  else
  {
    const CModelBase *model= dynamic_cast<const CModelBase *>(&Model());
    assert(model);

    if (! model) return false;

    const CDepletionStage *dp= model->FindDepletionStage(timeStep);

    if ( !dp)
      return false;

    const CAnalysisType anaType= AnalysisType(analysisType);

    if ( !anaType.Valid() )
      return false;

    if ( ! OnBuildComponent(*dp, anaType, 0))
      return false;
  }

  resultInfo.TimeStep(timeStep);
  resultInfo.Component(component);
  resultInfo.Result(this);
  resultInfo.Name(baseTag);
  return true;
}

bool IResult::validateName(CResultInfo& resultInfo, const std::string& name,
  const std::string& label, const std::string& componentList,
  const std::string& analysisTypeList, const ITensorGroup* parent) const
{
  bool valid= false;
  int timeStep= -1;

  if ( name.find( label ) != 0 )
    return false;

  std::string baseTag;
  std::string component;
  std::string analysisType;

  valid= SplitTextTag
    ( name
    , componentList
    , analysisTypeList
    , baseTag
    , component
    , timeStep
    , analysisType
    );

  if ( !valid ) 
    return false;

  if ( label != baseTag )
    return false;

  if ( timeStep < 0 )
  {
    if ( !analysisType.empty() )
      return false;
  }
  else
  {
    const CModelBase *model= dynamic_cast<const CModelBase *>(&Model());
    assert(model);

    if (! model) return false;

    const CDepletionStage *dp= model->FindDepletionStage(timeStep);

    if ( !dp)
      return false;

    const CAnalysisType anaType= AnalysisType(analysisType);

    if ( !anaType.Valid() )
      return false;

    if ( ! parent->OnBuildComponent(*dp, anaType, 0))
      return false;
  }

  resultInfo.TimeStep(timeStep);
  resultInfo.Component(component);
  resultInfo.Result(this);
  resultInfo.Name(baseTag);
  return true;
}

bool IResult::validateDerivedName(CResultInfo& resultInfo,
  const std::string& name, const std::string& label,
  const std::string& componentList, const std::string& analysisTypeList) const
{
  bool valid= false;
  bool validName= false;

  if ( name.find( label ) == 0 )
  {
    std::string baseTag;
    std::string component;
    std::string analysisType;
    int timeStep= -1;

    valid= SplitTextTag
      ( name
      , componentList
      , analysisTypeList
      , baseTag
      , component
      , timeStep
      , analysisType
      );

    validName= ( label == baseTag );
    if ( !validName)
      valid= false;

    if ( timeStep < 0 )
    {
      valid= valid && analysisType.empty();
    }
    else
    {
      valid= valid && !analysisType.empty();
    }

    if ( valid )
    {
      resultInfo.TimeStep(timeStep);
      resultInfo.Component(component);
      resultInfo.Result(this);
    }
  }

  return valid;
}

//////////////////////////////////////////////////////////////////////
// Implementation for CResultComponent
//////////////////////////////////////////////////////////////////////

IResultComponent::IResultComponent(unsigned int uName, IResult &result, 
								   const CDepletionStage& stage, 
								   const CAnalysisType& antype,
								   int nRegister,
								   unsigned int uComponent)
: IValueComponentBase(uName, result, uComponent, result.RegisterMode(stage, antype, nRegister)),
  m_stage(stage),
  m_antype(antype),
  m_nRegister(nRegister),
  m_Data(*this)
{
	assert(IsLinkedTo(result));		// Assert that we linked to our parent
	assert(result.Mode(antype, stage, nRegister) != -1);
	assert(&ResultRegister().DepletionStageEntry() == &stage.Entry());
	assert(stage.Index() >= 0);
	assert(stage.Index() <= ResultRegister().DepletionStageEntry().EntryNodes().size());
}

IResultComponent::IResultComponent(const QString& sName,
								   IResult& result,
								   const CDepletionStage& stage,
								   const CAnalysisType& antype,
								   int nRegister,
								   unsigned int uComponent)
: IValueComponentBase(sName, result, uComponent, result.RegisterMode(stage, antype, nRegister)),
  m_stage(stage),
  m_antype(antype),
  m_nRegister(nRegister),
  m_Data(*this)
{
	assert(IsLinkedTo(result));		// Assert that we linked to our parent
	assert(result.Mode(antype, stage, nRegister) != -1);
	assert(&ResultRegister().DepletionStageEntry() == &stage.Entry());
	assert(stage.Index() >= 0);
	assert(stage.Index() <= ResultRegister().DepletionStageEntry().EntryNodes().size());
}

IValueComponentBase::TValueComponentType IResultComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& IResultComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& IResultComponent::ScalarData()
{
  return m_Data;
}

bool IResultComponent::Defined() const
{
	const IResult* pResult = dynamic_cast<const IResult*>(&Parent());
	assert(pResult);

	return pResult->OnDefined(*this);
}

const CAnalysisType& IResultComponent::AnalysisType() const
{
	return m_antype;
}

const CDepletionStage& IResultComponent::Stage() const
{
	return m_stage;
}

int IResultComponent::RegisterIndex() const
{
	return m_nRegister;
}

IValueDomainScalar::TMinMax IResultComponent::MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
	const CModelBase *pBase = dynamic_cast<const CModelBase*> (&Model());
	assert(pBase);
	return ScalarData().MinMax(progressBase, geo::CBox(pBase->Boundary().Min(), pBase->Boundary().Max()), unit);
}

IValueDomainScalar::TValue IResultComponent::Average(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
	const CModelBase *pBase = dynamic_cast<const CModelBase*> (&Model());
	assert(pBase);
	return ScalarData().Average(progressBase, geo::CBox(pBase->Boundary().Min(), pBase->Boundary().Max()), unit);
}

const CResultRegister& IResultComponent::ResultRegister() const
{
	const CModelBase* pModel = (const CModelBase*)(&Model());
	return pModel->ResultRegister(m_nRegister);
}

CResultRegister& IResultComponent::ResultRegister()
{
	CModelBase* pModel = (CModelBase*)(&Model());
	return pModel->ResultRegister(m_nRegister);
}

const CMeshBase& IResultComponent::Mesh() const
{
	return ResultRegister().Mesh();
}

CMeshBase& IResultComponent::Mesh()
{
	return ResultRegister().Mesh();
}


bool IResultComponent::Empty() const
{
	return false;
}

long IResultComponent::SavedItems() const
{
	assert(false);
	return 0;
}

QString IResultComponent::ExportLabel() const
{
	// Get label from parent
	const IResult *pResult = dynamic_cast<const IResult*>(&Parent());
	assert(pResult);
	
	QString sRet;
	sRet = QString("%1_D%2_").arg(pResult->ExportLabel(ComponentIndex())).arg(
    Stage().Index());
  sRet += m_antype.ExportCharacter();

	return sRet;
} 

bool IResultComponent::CanMap(const COpenGLNode& node) const
{
	const IResult *pResult = dynamic_cast<const IResult*>(&Parent());
	assert(pResult);
	return pResult->CanMap(node, RegisterIndex());
}

IValueDomainScalar::TValue IResultComponent::ValuePoint(const geo::IPoint& /*pt*/, const CQuantity::UNIT /*unit*/, geo::IParallelInitializationCallback* /*cb*/) const
{
  return IValueDomainScalar::TValue();
}

IValueDomainScalar::TValue IResultComponent::GradientValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  assert(Gradient());

  std::set<const geo::IElement*> stElements;

  const geo::CElementPoint* pElementPoint = dynamic_cast<const geo::CElementPoint*>(&pt);
  if(pElementPoint)
  {
    stElements = pElementPoint->Elements();
  }
  else
  {
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    std::vector<int> vcElements = model.Mesh().Mesh().ElementsAt(pt);
    for(int i = 0; i < vcElements.size(); ++i)
      stElements.insert(&model.Mesh().Mesh().Element(vcElements[i]));
  }

  if(stElements.empty())
    return IValueDomainScalar::TValue();

  std::vector<double> vcElementValues;

  for(std::set<const geo::IElement*>::iterator it = stElements.begin(); it != stElements.end(); ++it)
  {
    const geo::IElement& elm = **it;
    IValueDomainScalar::TValueVec vcValues = ValueElement(elm, unit, cb);
    if(!vcValues.empty() && vcValues[0].Valid())
      vcElementValues.push_back(vcValues[0].Value());
  }

  if(vcElementValues.empty())
    return IValueDomainScalar::TValue();

  double dAverage = 0;
  for(int i = 0; i < vcElementValues.size(); ++i)
    dAverage += vcElementValues[i];

  dAverage /= vcElementValues.size();

  return geo::CValue(dAverage);
}

IValueDomainScalar::TValueVec IResultComponent::ValueElement(const geo::IElement& element, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	assert(Defined());

	IValueDomainScalar::TValueVec vcRet(element.NrOfNodes());
	if(vcRet.size() == 0)
		vcRet.resize( element.NrOfPoints() );

  // don't try to map values without a valid mesh
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(!model.Mesh().IsMesh())
    return vcRet;

  if(!Gradient())
  {
	  // Not a mesh element. Do element mapping
	  MapValueElement(element, vcRet, CValueType::MT_POINT, unit, cb);
  }
  else
  {
    // gradients
    const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&element);
    if(!pBody)
    {
      const geo::CBodyQuadrilateral* pBodyQuad1 = dynamic_cast<const geo::CBodyQuadrilateral*>(&element);
      if(pBodyQuad1)
        pBody = pBodyQuad1->Parent();
      else
      {
        const geo::CBodyTriangle* pBodyTri1 = dynamic_cast<const geo::CBodyTriangle*>(&element);
        if(pBodyTri1)
        {
          pBody = pBodyTri1->Parent();
        }
        else
        {
          const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&element);
          if(pInterface)
          {
            const geo::CBodyQuadrilateral* pBodyQuad2 = dynamic_cast<const geo::CBodyQuadrilateral*>(&pInterface->Front());
            if(pBodyQuad2)
              pBody = pBodyQuad2->Parent();
            else
            {
              const geo::CBodyTriangle* pBodyTri2 = dynamic_cast<const geo::CBodyTriangle*>(&pInterface->Front());
              if(pBodyTri2)
                pBody = pBodyTri2->Parent();
            }
          }
        }
      }
    }
    bool bSuccess = false;
    if(pBody && pBody->IndexingElementSet() == &model.Mesh().Mesh())
    {
      IValueDomainScalar::TValueVec vcBody(pBody->NrOfNodes());
      if(vcBody.size() == 0)
        vcBody.resize(pBody->NrOfPoints());

      MapValueElement(*pBody, vcBody, CValueType::MT_POINT, unit, cb);
      geo::CElementPoint ptMid = pBody->MidPoint();
      geo::CPoint ptMin = pBody->Min();
      geo::CPoint ptMax = pBody->Max();
      geo::CLine l(geo::CPoint(ptMid.X(), ptMid.Y(), ptMin.Z() - 1), geo::CPoint(ptMid.X(), ptMid.Y(), ptMax.Z() + 1));
      std::set<geo::CPoint> stIntersection;
      pBody->Intersection(l, stIntersection);
      if(stIntersection.size() == 2)
      {
        ptMin = *stIntersection.begin();
        ptMax = *stIntersection.rbegin();
        assert(ptMax.Z() >= ptMin.Z());
        geo::CValue valMin = pBody->InterpolateValue(ptMin, vcBody);
        geo::CValue valMax = pBody->InterpolateValue(ptMax, vcBody);
        if(valMin.Valid() && valMax.Valid())
        {
          double distance = ptMax.Z() - ptMin.Z();
          if (unit == CQuantity::FIELD_UNIT)
            distance *= FF_FACTOR_LENGTH;

          geo::CValue valGradient((valMax.Value() - valMin.Value()) / (distance));
          vcRet.assign(vcRet.size(), valGradient);
          bSuccess = true;
        }
      }
    }
    else
    {
      for(int i = 0; i < element.NrOfPoints(); ++i)
      {
        vcRet[i] = ValuePoint(element.Point(i), unit, cb);
      }
      bSuccess = true;
    }

    if(!bSuccess)
    {
      // gradient not possible, result invalid
      vcRet.assign(vcRet.size(), geo::CValue());
    }
  }

	return vcRet;
}

std::vector<CTensor> IResultComponent::TensorElement(const geo::IElement& element, const CQuantity::UNIT /*unit*/, geo::IParallelInitializationCallback *cb) const
{
	assert(Defined());

	std::vector<CTensor> vcRet(element.NrOfNodes());
	if (vcRet.size() == 0)
		vcRet.resize(element.NrOfPoints());

	// don't try to map values without a valid mesh
	const CModelBase& model = static_cast<const CModelBase&>(Model());
	if (!model.Mesh().IsMesh())
		return vcRet;

	// Not a mesh element. Do element mapping
	MapTensorElement(element, vcRet, cb);
	return vcRet;
}
bool IResultComponent::CanExport() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
	assert(pModel);
	return pModel->ResultRegister().ResultsAvailable();
}

bool IResultComponent::Gradient() const
{
  return false;
}

// Implementation of scalar valueset

IScalarResultComponent::CResultValueSet::CResultValueSet(const IScalarResultComponent& component, CQuantity::UNIT unit)
: m_component(component), m_unit(unit)
{
}

const geo::IElementSet &IScalarResultComponent::CResultValueSet::ElementSet() const
{
	return m_component.Mesh().Mesh();
}

CQuantity::UNIT IScalarResultComponent::CResultValueSet::Unit() const
{
	return m_unit;
}

void IScalarResultComponent::CResultValueSet::ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const
{
	assert(values.size() == ElementSet().Element(iElementIndex).NrOfPoints());
	m_component.ElementValues(values, ElementSet().Element(iElementIndex), Unit(), cb);
}

void IScalarResultComponent::CResultValueSet::Empty()
{
	assert(false);
}

bool IScalarResultComponent::CResultValueSet::IsEmpty() const
{
	return false;
}

int IScalarResultComponent::CResultValueSet::PushBack(const TValueVec &/*values*/)
{
	assert(false);
	return -1;
}

/// Implementation of IScalarResultValueSet
IScalarResultComponent::IScalarResultComponent(unsigned int uName, 
											   IResult &result, 
								               const CDepletionStage& stage, 
								               const CAnalysisType& antype,
								               int nRegister,
								               unsigned int uComponent)
: IResultComponent(uName, result, stage, antype, nRegister, uComponent),
  m_si(*this, CQuantity::SI_UNIT),
  m_field(*this, CQuantity::FIELD_UNIT)
{
}

IScalarResultComponent::IScalarResultComponent(const QString& sName, 
											   IResult &result, 
								               const CDepletionStage& stage, 
								               const CAnalysisType& antype,
								               int nRegister,
								               unsigned int uComponent)
: IResultComponent(sName, result, stage, antype, nRegister, uComponent),
  m_si(*this, CQuantity::SI_UNIT),
  m_field(*this, CQuantity::FIELD_UNIT)
{
}

const geo::IValueSet& IScalarResultComponent::ValueSet(const CQuantity::UNIT unit) const
{
	if(unit == CQuantity::SI_UNIT) return m_si;
	return m_field;
}

geo::CValue IScalarResultComponent::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	// Not a mesh element. Do point interpolation from library
  if(Gradient())
    return GradientValuePoint(pt, unit, cb);

	return ValueSet(unit).ValuePoint(pt, cb);//MapValues(element, vcRet, model.ResultTree().WeightingType());
}

void IScalarResultComponent::MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType /*map_type*/, UNIT unit, geo::IParallelInitializationCallback *cb) const
{
   ValueSet(unit).ElementValue(elm, values, cb);
}
