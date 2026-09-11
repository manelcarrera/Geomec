// GlobalInitialStressNode.h: interface for the CGlobalInitialStressNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALINITIALSTRESSNODE_H__62DF1E07_A01A_4C45_82D4_C8580D3E0EE1__INCLUDED_)
#define AFX_GLOBALINITIALSTRESSNODE_H__62DF1E07_A01A_4C45_82D4_C8580D3E0EE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stress.h"

namespace geo {
  class IElement;
}

#include "LengthQuantity.h"
#include "StorageNode.h"
#include "GeomecModelVisitor.h"

class CGlobalInitialStressNode : public CStorageNode
{
	CStressGradient m_water_column_stress_gradient;
	CLengthQuantity m_water_surf_depth;
	bool m_bMeshIsValid;

	void InvalidateResults() const;
public:
	CGlobalInitialStressNode(CFemAppModel& model,
							 const double& dWaterSurfaceDepth,
							 const double& dWaterColumnStressGradient);
	CGlobalInitialStressNode(const CGlobalInitialStressNode &rhs);
	CGlobalInitialStressNode(CFemAppModel&model);
	virtual ~CGlobalInitialStressNode();

	bool operator==(const CGlobalInitialStressNode &rhs) const;
	CGlobalInitialStressNode& operator=(const CGlobalInitialStressNode& rhs);

	double TotalVerticalStress(const double& z) const;

	std::vector<double> TotalVerticalStress(const geo::IElement &element) const;
	double TotalVerticalStressAtNode(const geo::IElement &element, int iNodeNr) const;

	// Notification
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourDeleted(const CGraphNode &node);

	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;

	virtual bool Empty() const;

	// Save and load
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

	// Stress Gradients and so	
	const CStressGradient& WaterColumnStressGradient() const;
	void WaterColumnStressGradient(const double& value, const CStressGradient::UNIT = CStressGradient::SI_UNIT) ;
	const CLengthQuantity& WaterSurfaceDepth() const;
	void WaterSurfaceDepth(const double& value, const CLengthQuantity::UNIT = CLengthQuantity::SI_UNIT) ;

  ACCEPT_GEOMECMODELVISITORS(VisitGlobalInitialStressNode);
};

#endif // !defined(AFX_GLOBALINITIALSTRESSNODE_H__62DF1E07_A01A_4C45_82D4_C8580D3E0EE1__INCLUDED_)
