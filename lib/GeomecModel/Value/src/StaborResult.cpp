// StaborResult.cpp: implementation of the CStaborResult class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseEntryTypes.h"
#include "StaborResult.h"
#include "NewWellPath.h"
#include "resourceIDS.h"
#include "FemAppModel.h"
#include "ResultTree.h"
#include "ResultInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaborResult::CStaborResult(CResultGroup& group)
: IResult(IDS_RG_DSTABOR, group), m_well_path_entry((CNewWellPathEntry&)*group.Model().GraphEntry(MD_NEW_WELLPATH))
{
	assert(group.Model().GraphEntry(MD_NEW_WELLPATH));
	LinkTo(*group.Model().GraphEntry(MD_NEW_WELLPATH));
	BuildComponents();
  Index(RC_DSTABOR);
}

CStaborResult::~CStaborResult()
{

}

void CStaborResult::OnNeighbourDeleted(const CGraphNode &node)
{
	if(&node == &m_well_path_entry)
	{
		delete this;
		return ;
	}

	IResult::OnNeighbourDeleted(node);
}

void CStaborResult::BuildComponent(const CDepletionStage &stage, const CAnalysisType& antype, int nRegister)
{
	assert(Mode(antype, stage) == -1);
	assert(nRegister == 0);

	new CStaborResultComponent(*this, stage, antype, nRegister);
}
	
bool CStaborResult::Empty() const
{
	return false;
}

long CStaborResult::SavedItems() const
{
	return 1;
}

bool CStaborResult::CanMap(const COpenGLNode& node, int nRegister) const
{
		return (dynamic_cast<const CNewWellPath*>(&node) != 0) && (nRegister == PRIMARY_RESULT);
}

QString CStaborResult::ExportLabel(int nComponent) const
{
	assert(nComponent == 0);

	return getStringTableEntry(IDS_ET_DSTABOR);
}

bool CStaborResult::ValidName
( const std::string &name
, CResultInfo &resultInfo
) const
{
  std::string label = getStringTableEntry(IDS_ET_DSTABOR);

  return validateName(resultInfo, name, label, "", "_L|_M|_C|_N|_H");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaborResult::CStaborResultComponent::CStaborResultComponent(CStaborResult& result, const CDepletionStage& stage, const CAnalysisType& antype, int nRegister)
: IScalarResultComponent("", result, stage, antype, nRegister, 0)
{

}

CStaborResult::CStaborResultComponent::~CStaborResultComponent()
{

}

void CStaborResult::CStaborResultComponent::Clear()
{
	m_mpValue.clear();
}

bool CStaborResult::CStaborResultComponent::InsertValue(const geo::IPoint &/*pt*/, const double &/*value*/)
{
	// Insert value and check if point exist in well path

	return true; 

}

bool CStaborResult::CStaborResultComponent::Empty() const
{
	return true;
}

long CStaborResult::CStaborResultComponent::SavedItems() const
{
	return 1;
}

unsigned int CStaborResult::CStaborResultComponent::IconId() const
{
	return 0;
}

unsigned int CStaborResult::CStaborResultComponent::TypeId() const
{
	return 0;
}

QString CStaborResult::CStaborResultComponent::UnitName(const CQuantity::UNIT unit) const
{
	if(unit == CQuantity::FIELD_UNIT)
		return QString("psi/ft");
		
	return QString("kPa/m");
}

void CStaborResult::CStaborResultComponent::ElementValues(IValueDomainScalar::TValueVec& values, const geo::IElement& /*mesh_element*/, UNIT /*unit*/, geo::IParallelInitializationCallback * /*cb*/) const
{
	for(size_t i = 0; i < values.size(); i++) values[i] = geo::CValue();
}

QString CStaborResult::CStaborResultComponent::ExportLabel() const
{
  return IScalarResultComponent::ExportLabel();
}
