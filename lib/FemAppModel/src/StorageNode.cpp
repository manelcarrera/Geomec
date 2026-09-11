// StorageNode.cpp: implementation of the CStorageNode class.
//
//////////////////////////////////////////////////////////////////////

#include "GraphNode.h"
#include "FemAppModel.h"
#include "StorageNode.h"
#include "SingleQuantity.h"
#include "StreamVersion.h"


#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStorageNode::CStorageNode(CFemAppModel& model)
: CGraphNode(""), m_nIndex(-1), m_model(model)
{
}

CStorageNode::CStorageNode(const QString &strName, CFemAppModel& model)
: CGraphNode(strName), m_nIndex(-1), m_model(model)
{
}

CStorageNode::CStorageNode(const unsigned int uName, CFemAppModel& model)
: CGraphNode(uName), m_nIndex(-1), m_model(model)
{
}

	
CStorageNode::CStorageNode(const CStorageNode &rhs)
: CGraphNode(rhs), m_nIndex(-1), m_model(rhs.m_model)
{
}

CFemAppModel& CStorageNode::Model()
{
	return m_model;
}

const CFemAppModel& CStorageNode::Model() const
{
	return m_model;
}


CStorageNode::~CStorageNode()
{
}

int CStorageNode::Index() const
{
	return m_nIndex;
}

void CStorageNode::Index(int nIndex) 
{
	m_nIndex = nIndex;

  CGraphEntry* pEntry = dynamic_cast<CGraphEntry*>(parent());
	if(pEntry) pEntry->OnIndexChanged(*this);
	for(size_t i = 0; i < referenceSize(); i++) {
		pEntry = dynamic_cast<CGraphEntry*>(&referenceAt(i));
		if(pEntry) pEntry->OnIndexChanged(*this);
	}
}

CStorageNode& CStorageNode::operator=(const CStorageNode &rhs)
{
	CGraphNode::operator=(rhs);
	return *this;
}

bool CStorageNode::operator==(const CStorageNode &rhs) const
{
	return CGraphNode::operator==(rhs);
}

void CStorageNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& /*progress*/)
{
	// Load the index
	QString sName;
	stream >> sName;
	Name(sName);
	int nIndex;
	stream >> nIndex;
	Index(nIndex);

  // although this data-member belongs to 'CGraphNode', it is loaded here

  if (CStreamVersion(4, 1, 35) < version)
	{
		int modifiedByAttributesDialog;

		stream >> modifiedByAttributesDialog;
		m_modifiedByAttributesDialog = (modifiedByAttributesDialog != 0);
	}
}

void CStorageNode::SaveStream(TSTREAM& stream, TPROGRESS& /*progress*/)
{
	// Save the index;
	stream << Name();
	stream << m_nIndex;

  // although this data-member belongs to 'CGraphNode', it is saved here

  int modifiedByAttributesDialog = m_modifiedByAttributesDialog ? 1 : 0;

  stream << modifiedByAttributesDialog;
}

long CStorageNode::SavedItems() const
{
	// Zero steps
	return 0;
}

void CStorageNode::Save(const CDoubleQuantity &quantity, TSTREAM &stream) const
{
	int iDefined = (!quantity.Undefined());
	stream << iDefined;
	if(iDefined) stream << quantity.Value();
}

void CStorageNode::Load(CDoubleQuantity &quantity, TSTREAM &stream) const
{
	int iDefined;
	stream >> iDefined;
	if(iDefined)
	{
		double dValue;
		stream >> dValue;
		quantity.SetValue(dValue);
	}
	else
	{
		quantity.Invalidate();
	}
}

void CStorageNode::InvalidateStorage()
{
	Model().OnInvalidateStorage();
}

void CStorageNode::InvalidateCalculation(int nType)
{
	Model().OnInvalidateCalculation(nType);
}

void CStorageNode::Modified(enum ModifiedHint uHint) 
{
	if(!IsCopy())
		InvalidateStorage();

	CGraphNode::Modified(uHint);
}

