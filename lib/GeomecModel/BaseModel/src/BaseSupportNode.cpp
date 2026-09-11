
#include <memory>

#include "BaseSupportNode.h"
#include "DepletionStage.h"
#include "BoundaryBase.h"
#include "ValueType.h"
#include "ElementSet.h"
#include "ModelBase.h"
#include "ValueComponent.h"
#include "ElementValueSet.h"
#include "ResultRegister.h"
#include "BaseEntryTypes.h"
#include "GlobalMessage.h"
#include "FemAppEntryTypes.h"
#include "resourceIDI.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "StreamVersion.h"

///// CSupportDepletion

CSupportDepletion::CSupportDepletion(const CDepletionStage& stage, CBaseSupportNode &snode)
: CStorageNode(stage.Name(), snode.Model()),
  m_pValueType(0),
  m_stage(stage)
{
	LinkTo(snode.Boundary());
	reParent(&snode);
}

bool CSupportDepletion::Less(const CGraphNode &node) const
{
	const CSupportDepletion *pSD = dynamic_cast<const CSupportDepletion*>(&node);
	
	if(pSD)
		return m_stage.Less(pSD->m_stage);

	return CStorageNode::Less(node);
}

void CSupportDepletion::OnNewNeighbour(const CGraphNode &node)
{
  const CValueType* valueType = dynamic_cast <const CValueType*> (&node);

  if (valueType != 0)
  {
    m_pValueType = valueType;
    Modified();
    InvalidateResults();
  }

  CStorageNode::OnNewNeighbour(node);
}

void CSupportDepletion::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint /*uHint*/)
{
	const CBoundaryBase* bound = dynamic_cast<const CBoundaryBase*>(&node);
	
	if(bound)
	{
		if(ValueType() && !IsValueTypeAllowed())
		{
		   UnLink(const_cast<CValueType&>(*m_pValueType));
       assert(!ValueType()); 
		}
	}
}

void CSupportDepletion::OnNeighbourDeleted(const CGraphNode &node)
{
	if( &node == m_pValueType )
  {
		m_pValueType = 0;
		Modified();
		InvalidateResults();
	}

	CStorageNode::OnNeighbourDeleted(node);
}

bool CSupportDepletion::IsValueTypeAllowed() const
{
  const CBaseSupportNode& supnode = static_cast<const CBaseSupportNode&>(*parent());
  return supnode.IsValueTypeAllowed(m_stage);
}

unsigned int CSupportDepletion::IconId() const
{
	return IDI_DEPLETION_STAGE;
}

unsigned int CSupportDepletion::TypeId() const
{
	return 0;
}
	
// Function to support drag and drop ...
bool CSupportDepletion::ConnectItem(const CGraphNode &item)
{
	std::auto_ptr <IProgressBase> wc(_g->prog()->create(eProgress::Wait));
	assert(CanConnectItem(item));
	const CValueType *pVT = dynamic_cast<const CValueType*>(&item);
	
	if(dynamic_cast<const IElementSet*>(&pVT->PointSet()))
	{
		/* Check to see if all the nodes of the boundary interface actually fall inside
		the elementset associated with this valuetype. */
		CModelBase *pModel = static_cast<CModelBase*>(&Model());
		assert(pModel);
    const CBoundaryBase& boundary = pModel->Boundary();
		assert(boundary.HasInterfaces() || m_stage.Initial());
		std::vector<int> indices;
		const CValueComponent &vc = dynamic_cast<const CValueComponent&>(pVT->Component(0));
		assert(&vc);
		const CElementValueSet &vs = dynamic_cast<const CElementValueSet&>(vc.ValueSet());
		assert(&vs);
		bool bOK = true;
		
		std::auto_ptr <IProgressBase> progress(_g->prog()->create( eProgress::Geo, "", false));
		progress->AddSteps(boundary.InterfaceNodeSize());
		progress->StatusMessage("Checking overlap between boundary and elementset.");
		for(int i = 0; i < boundary.InterfaceNodeSize(); i++)
		{
			indices.clear();
			indices = vs.ElementSet().ElementSet().ElementsAt(boundary.InterfaceNode(i));
			if(indices.size() == 0)
			{
				bOK = false;
				break;
			}
			progress->Step();
		}
	
		if(!bOK)
		{
			QString message;
			message = QString("The elementset associated with the valuetype does not enclose\nthe whole boundary. The valuetype will not be attached.");
			_m()->msg(message, MB_ICONINFORMATION|MB_OK);
			return true;
		}
	}

	if(m_pValueType)
		UnLink(*(const_cast<CValueType*>(m_pValueType)));

	LinkTo(*(const_cast<CValueType*>(pVT)));

	return true;
}

void CSupportDepletion::InvalidateResults() const
{
	CModelBase &model = (CModelBase&)(Model());
  if(!model.getDeleting())
  {
	  CResultRegister &rr = model.ResultRegister();
	  rr.ClearAll();
  }
}

bool CSupportDepletion::CanConnectItem(const CGraphNode &item) const
{
	const CValueType *pVT = dynamic_cast<const CValueType*>(&item);
	if(!pVT)
		return false;

	const CBaseSupportNode& supnode = static_cast<const CBaseSupportNode&>(*parent());

  return IsValueTypeAllowed() && supnode.IsValidValueTypeId(pVT->TypeId());
}

bool CSupportDepletion::CanDisconnectItem(const CGraphNode& /*item*/) const
{
	return true;
}

bool CSupportDepletion::Empty() const
{
	return false;
}

/*virtual*/ void CSupportDepletion::LoadStream(TSTREAM& stream, CStreamVersion &version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 68) < version)
	{
    CModelBase& model = static_cast<CModelBase&>(Model());
		TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*model.GraphEntry(MD_BASE_VALUE_COMPOSITE);

		int nIndex;
		stream >> nIndex;
		
		if(nIndex != -1)
		{
			composite_entry.LinkNodeToIndex(*this, nIndex);
		}
	}
	progress.Step();
}

/*virtual*/ void CSupportDepletion::SaveStream(TSTREAM& stream,TPROGRESS& progress)
{
	if(m_pValueType)
		stream << m_pValueType->Index();
	else
		stream << -1;
	progress.Step();
}

long CSupportDepletion::SavedItems() const
{
	return 1;
}


///// CBaseSupportNode

CBaseSupportNode::CBaseSupportNode(unsigned int uName, CBoundaryBase& boundary)
: COpenGLNode(uName, boundary.Model())
{
  reParent(&boundary);
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
}

CBaseSupportNode::CBaseSupportNode(const CBaseSupportNode& rhs)
: COpenGLNode(rhs)
{
}

CBaseSupportNode::~CBaseSupportNode()
{
}

const CSupportDepletion* CBaseSupportNode::SupportDepletion(const CDepletionStage& stage) const
{
  return (const_cast<CBaseSupportNode*>(this))->SupportDepletion(stage);
}

CSupportDepletion* CBaseSupportNode::SupportDepletion(const CDepletionStage& stage)
{
	std::vector<CSupportDepletion*>::size_type st;
	for(st = 0; st < m_vcDepStages.size(); st++)
	{
		if(&stage == &m_vcDepStages[st]->Stage())
			return m_vcDepStages[st];
	}

	return 0;
}

const CValueType* CBaseSupportNode::DistriValues(const CDepletionStage &stage) const
{
	if(SupportDepletion(stage))
	{
		return SupportDepletion(stage)->ValueType();
	}

	return 0;
}

bool CBaseSupportNode::Distributed(const CDepletionStage &stage) const
{
	return DistriValues(stage) != 0;
}

bool CBaseSupportNode::IsValueTypeAllowed(const CDepletionStage& /*stage*/) const
{
  return Boundary().HasInterfaces();
}

const CBoundaryBase& CBaseSupportNode::Boundary() const
{
  return static_cast<const CBoundaryBase&>(*parent());
}

CBoundaryBase& CBaseSupportNode::Boundary()
{
  return static_cast<CBoundaryBase&>(*parent());
}

void CBaseSupportNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
	const TDepletionStageEntry *pEntry = dynamic_cast<const TDepletionStageEntry *> (&node);
  if(pEntry && uHint != MarkedAsInitialChanged)
	{
		if(pEntry->EntryNodes().size() > 0)
		{
			CreateChildren();
		}
	}

	COpenGLNode::OnNeighbourModified(node, uHint);
}

void CBaseSupportNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 68) < version) // as from 3.0.69
	{
		int nDepStages;
		stream >> nDepStages;
		assert(m_vcDepStages.size() == nDepStages); // the children should have already been created
		for(int i = 0; i < nDepStages; i++)
		{
			m_vcDepStages[i]->LoadStream(stream, version, progress);
		}
	}
}

void CBaseSupportNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	stream << int(m_vcDepStages.size());
	std::vector<CSupportDepletion*>::iterator it_dep;
	for(it_dep = m_vcDepStages.begin(); it_dep != m_vcDepStages.end(); it_dep++)
	{
		(*it_dep)->SaveStream(stream, progress);
	}
}

long CBaseSupportNode::SavedItems() const
{
  long lRet = 0;
  for(size_t i = 0; i < m_vcDepStages.size(); ++i)
    lRet += m_vcDepStages[i]->SavedItems();

  return lRet;
}

void CBaseSupportNode::ClearChildren()
{
	std::vector<CSupportDepletion*>::size_type st;
	for(st = 0; st < m_vcDepStages.size(); st++)
	{
		UnLink(*m_vcDepStages[st]);
	}
	m_vcDepStages.clear();
}

void CBaseSupportNode::CreateChildren()
{
  // remember the children's attached valuetypes
  typedef std::map<const CDepletionStage*, const CValueType*> TValueTypeMap;
  TValueTypeMap mpValueType;
  for(size_t i = 0; i < m_vcDepStages.size(); ++i)
  {
    if(m_vcDepStages[i]->ValueType())
      mpValueType.insert(TValueTypeMap::value_type(&m_vcDepStages[i]->Stage(), m_vcDepStages[i]->ValueType()));
  }

	ClearChildren();
	CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
	assert(pModel);
	const CDepletionStage *pStage = &pModel->InitialDepletionStage();
	assert(pStage);

	while(pStage)
	{
    CSupportDepletion* pSD = new CSupportDepletion(*pStage, *this);
		assert(IsLinkedTo(*pSD));
		m_vcDepStages.push_back(pSD);

    // see if any children were attached and reattach them if so
    TValueTypeMap::const_iterator it = mpValueType.find(pStage);
    if(it != mpValueType.end())
    {
      if(pSD->CanConnectItem(*it->second))
        pSD->ConnectItem(*it->second);
    }

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}

	Modified();
}
