// GlobalInitialStessNode.cpp: implementation of the CGlobalInitialStessNode class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalInitialStressNode.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "resourceIDI.h"
#include "MeshBase.h"
#include "StreamVersion.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

static const QString cstrName = "Weight of seawater";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalInitialStressNode::CGlobalInitialStressNode(CFemAppModel& model,
												   const double& dWaterSurfaceDepth,
												   const double& dWaterColumnStressGradient)
: CStorageNode(cstrName, model),
  m_water_column_stress_gradient(dWaterColumnStressGradient, CQuantity::SI_UNIT),
  m_water_surf_depth(dWaterSurfaceDepth, CQuantity::SI_UNIT),
  m_bMeshIsValid(false)
{
}

CGlobalInitialStressNode::CGlobalInitialStressNode(CFemAppModel& model)
: CStorageNode(model),
  m_water_column_stress_gradient(0, CQuantity::SI_UNIT),
  m_water_surf_depth(0, CQuantity::SI_UNIT),
  m_bMeshIsValid(false)
{
}


CGlobalInitialStressNode::CGlobalInitialStressNode(const CGlobalInitialStressNode& rhs)
: CStorageNode(rhs),
  m_water_column_stress_gradient(rhs.m_water_column_stress_gradient),
  m_water_surf_depth(rhs.m_water_surf_depth),
  m_bMeshIsValid(rhs.m_bMeshIsValid)
{
}

CGlobalInitialStressNode::~CGlobalInitialStressNode()
{
}

bool CGlobalInitialStressNode::operator==(const CGlobalInitialStressNode &rhs) const
{
	if(!CStorageNode::operator ==(rhs))
		return false;

	return ((m_water_surf_depth == rhs.m_water_surf_depth) &&
			(m_water_column_stress_gradient == rhs.m_water_column_stress_gradient));
}

CGlobalInitialStressNode& CGlobalInitialStressNode::operator=(const CGlobalInitialStressNode& rhs)
{
	if(!((*this) == rhs))
	{
		CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());
		assert(pModel);
    pModel->ResultRegister().ClearLinear(false);
    pModel->ResultRegister().ClearNonLinear(false);
    pModel->ResultRegister().ClearMixture();
	}

	CStorageNode::operator =(rhs);

	m_water_surf_depth = rhs.m_water_surf_depth;
	m_water_column_stress_gradient = rhs.m_water_column_stress_gradient;

	m_bMeshIsValid = rhs.m_bMeshIsValid;

	return *this;
}

unsigned int CGlobalInitialStressNode::TypeId() const
{
	return 0;
}

unsigned int CGlobalInitialStressNode::IconId() const
{
	return IDI_GLOBAL_INITIAL_STRESS;
}

void CGlobalInitialStressNode::OnNewNeighbour(const CGraphNode &node)
{
	if(&node == &((CModelBase&)(Model())).Mesh())
	{
		m_bMeshIsValid = true;
	}

	CStorageNode::OnNewNeighbour(node);
}

void CGlobalInitialStressNode::InvalidateResults() const
{
	// wedx 03122006
	// Can only access the result register when there is a valid mesh.
	if(m_bMeshIsValid)
	{
		CModelBase &model = (CModelBase&)(Model());
		CResultRegister &rr = model.ResultRegister();
		rr.ClearAll();
	}
}

void CGlobalInitialStressNode::OnNeighbourDeleted(const CGraphNode &node)
{
	if(&node == &((CModelBase&)(Model())).Mesh())
	{
		// wedx 03122006
		// The global initial stress node can only access the result register when the mesh
		// is valid (the result register is actually a member of the mesh).
		// When the mesh is unlinked from the global initial stress node, we know we can not
		// access the result register anymore.
		m_bMeshIsValid = false;
	}

	CStorageNode::OnNeighbourDeleted(node);
}

bool CGlobalInitialStressNode::Empty() const
{
	return false;
}

const CStressGradient& CGlobalInitialStressNode::WaterColumnStressGradient() const
{
	return m_water_column_stress_gradient;
}

void CGlobalInitialStressNode::WaterColumnStressGradient(const double &value, const CStressGradient::UNIT unit)
{
	m_water_column_stress_gradient.Value(value, unit);
}

const CLengthQuantity& CGlobalInitialStressNode::WaterSurfaceDepth() const
{
	return m_water_surf_depth;
}

void CGlobalInitialStressNode::WaterSurfaceDepth(const double &value, const CLengthQuantity::UNIT unit)
{
	m_water_surf_depth.Value(value, unit);
}

std::vector<double> CGlobalInitialStressNode::TotalVerticalStress(const geo::IElement &element) const
{
	std::vector<double> vcRet(element.NrOfNodes());

	int i;
	for(i = 0; i < element.NrOfNodes(); ++i)
		vcRet[i] = TotalVerticalStressAtNode(element, i);

	return vcRet;
}

// will return the stress based on global settings or based on a distribution, set by the incoming boolean
// if no distribution is available, the global is returned anyhow.
double CGlobalInitialStressNode::TotalVerticalStressAtNode(const geo::IElement &element, int iNodeNr) const
{
	return TotalVerticalStress(element.Node(iNodeNr).Z());
}

double CGlobalInitialStressNode::TotalVerticalStress(const double& z) const
{
	double dWaterStressGrd = WaterColumnStressGradient().Value();
	double dZwater = m_water_surf_depth.Value();

	if(z < dZwater)
		return 0;

	return dWaterStressGrd * (z - dZwater);
}

void CGlobalInitialStressNode::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
{
	CStorageNode::LoadStream(stream,version,progress);

	if(version < CStreamVersion(3, 0, 82))
	{
		// old code
		int distriOnly;
		stream >> distriOnly;
//		DistributedOnly(distriOnly);
		double dTemp;
		stream >> dTemp;
//		VerticalStressGradientDry(dTemp);
		stream >> dTemp;
//		VerticalStressGradientWet(dTemp);
		stream >> dTemp;
		WaterColumnStressGradient(dTemp);
		stream >> dTemp;
		WaterSurfaceDepth(dTemp);
		stream >> dTemp;
//		VerticalStressExponent(dTemp);
		int distriSize;
		stream >> distriSize;
		int nIndex;
		for (int count = 0; count < distriSize; count++)
		{
//			TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);
			stream >> nIndex;
//			assert(composite_entry.FindIndex(nIndex));
//			TStressTensor *pStress = (TStressTensor*)composite_entry.FindIndex(nIndex);
//			assert(pStress);
//			LinkTo(*pStress);
		}
	}
	else
	{
		// new code (3.0.82 and higher)
		double dTemp;
		stream >> dTemp;
		WaterColumnStressGradient(dTemp);
		stream >> dTemp;
		WaterSurfaceDepth(dTemp);
	}

	// always use the new name
	Name(cstrName);
}

void CGlobalInitialStressNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	CStorageNode::SaveStream(stream,progress);

//	int distriOnly = DistributedOnly();
//	stream << distriOnly;
//	stream << VerticalStressGradientDry().Value();
//	stream << VerticalStressGradientWet().Value();
	stream << WaterColumnStressGradient().Value();
	stream << WaterSurfaceDepth().Value();
//	stream << VerticalStressExponent();
//	stream << DistributedSize();
//	for(int i = 0; i < DistributedSize(); i++)
//	{
//		stream << DistributedStress(i).Index();
//		progress.Step();
//	}
}

long CGlobalInitialStressNode::SavedItems() const
{
	return CStorageNode::SavedItems();// + DistributedSize();
}