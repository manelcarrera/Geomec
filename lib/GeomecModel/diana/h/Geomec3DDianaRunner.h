// Geomec3DDianaRunner.h: interface for the CGeomec3DDianaRunner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMEC3DDIANARUNNER_H__DAD6BF32_8B63_42F5_872D_E94150E2E643__INCLUDED_)
#define AFX_GEOMEC3DDIANARUNNER_H__DAD6BF32_8B63_42F5_872D_E94150E2E643__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecCompactionDianaRunner.h"

class C3DModel;

namespace geo {
  class CInterfaceElement;
}

class CGeomec3DDianaRunner : public CGeomecCompactionDianaRunner
{
	
public:
	CGeomec3DDianaRunner(CMeshBase &meshbase,
						 C3DModel &model,
						 CDianaRunController& controller);
	virtual ~CGeomec3DDianaRunner();

	virtual const dia::IMaterial &Material(const geo::IElement &element) const;

protected:
	virtual int LoadsProgressSize() const;

	virtual bool CreateTopLoad(const CHorizonBase &top, dia::CLoadCase &lcase);
	virtual bool FetchInitialPressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialPressures);
	virtual bool CreateInitialLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
	virtual bool CreateStageLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
	virtual void CreateWeightLoad(dia::CLoadCase &lcase, const double &gravity);

	virtual const dia::IMaterial *InterfaceMaterial(const geo::IElement &element) const;

  virtual bool OnWriteMiscelaneous();

	virtual bool IsZoomInModel() const;

private:
	typedef CDisplacementSupportNode::CNodalSupportDef TNodalSupportDef;

	void WriteInterfaceInitialLoads(const CDepletionStage &stage, const geo::CInterfaceElement &iface, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures);
	void WriteInterfaceStageLoads(const CDepletionStage &stage, const geo::CInterfaceElement &iface, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures);
};

#endif // !defined(AFX_GEOMEC3DDIANARUNNER_H__DAD6BF32_8B63_42F5_872D_E94150E2E643__INCLUDED_)
